
#include "LEventRaw.hh"
#include <cstddef>
#include <memory>
#include "../LTelConfig.hh"
#include "../LDataBase.hh"
#include "../LEventElectronic.hh"
#include "io_hess.h"

LEventRaw::LEventRaw():LDataBase()
{
    InitObject();
}


void LEventRaw::InitObject()
{
    event = std::make_shared<LEvent>();
}
void LEventRaw::GetShower(AllHessData* hsdata)
{
    shower->energy = hsdata->mc_shower.energy;
    shower->altitude = hsdata->mc_shower.altitude;
    shower->azimuth  = hsdata->mc_shower.azimuth;
    shower->core_x   = hsdata->mc_event.xcore;
    shower->core_y   = hsdata->mc_event.ycore;
    shower->h_first_int = hsdata->mc_shower.h_first_int;
    shower->x_max    = hsdata->mc_shower.xmax;
    shower->shower_primary_id = hsdata->mc_shower.primary_id;
    shower->obs_id   = hsdata->run_header.run;
    shower->event_id = hsdata->mc_event.event;
};

void LEventRaw::GetEventShower(AllHessData* hsdata)
{
    event->event_shower->energy = hsdata->mc_shower.energy;
    event->event_shower->altitude = hsdata->mc_shower.altitude;
    event->event_shower->azimuth  = hsdata->mc_shower.azimuth;
    event->event_shower->core_x   = hsdata->mc_event.xcore;
    event->event_shower->core_y   = hsdata->mc_event.ycore;
    event->event_shower->h_first_int = hsdata->mc_shower.h_first_int;
    event->event_shower->x_max    = hsdata->mc_shower.xmax;
    event->event_shower->shower_primary_id = hsdata->mc_shower.primary_id;
    event->event_shower->obs_id   = hsdata->run_header.run;
    event->event_shower->event_id = hsdata->mc_event.event;
    event->event_shower->array_point_az = hsdata->run_header.direction[0];
    event->event_shower->array_point_alt = hsdata->run_header.direction[1];
}

void LEventRaw::GetEventWaveform(AllHessData* hsdata)
{
    event->ClearWaveform();
    for(auto i = 0; i < hsdata->event.central.num_teldata;i++)
    {
        int tel_id = hsdata->event.central.teldata_list[i];
        auto tel_electronic = std::make_shared<LRTelElectronic>();
        tel_electronic->tel_id = tel_id;
        tel_electronic->event_id = hsdata->mc_event.event;
        tel_electronic->num_gains = hsdata->event.teldata[tel_id - 1].raw->num_gains;
        tel_electronic->num_pixels = hsdata->event.teldata[tel_id - 1].raw->num_pixels;
        tel_electronic->num_samples = hsdata->event.teldata[tel_id - 1].raw->num_samples;
        tel_electronic->tel_alt = hsdata->event.trackdata[tel_id - 1].altitude_raw;
        tel_electronic->tel_az = hsdata->event.trackdata[tel_id - 1].azimuth_raw;
        tel_electronic->Allocate();
        for( auto igain = 0; igain < tel_electronic->num_gains; igain++)
        {
            for( auto ichannel = 0; ichannel < tel_electronic->num_pixels; ichannel++)
            {
                for( auto isample = 0; isample < tel_electronic->num_samples; isample++)
                {
                    auto index = igain * tel_electronic->num_pixels * tel_electronic->num_samples + ichannel * tel_electronic->num_samples + isample;
                    tel_electronic->adc_sample[index] = hsdata->event.teldata[tel_id - 1].raw->adc_sample[igain][ichannel][isample];
                }
                if(hsdata->event.teldata[tel_id - 1].raw->known)
                {
                    tel_electronic->adc_known[igain * tel_electronic->num_pixels + ichannel] = hsdata->event.teldata[tel_id - 1].raw->adc_known[igain][ichannel];
                    tel_electronic->adc_sum[igain * tel_electronic->num_pixels + ichannel] = hsdata->event.teldata[tel_id - 1].raw->adc_sum[igain][ichannel];
                }
            }
        }    
        event->AddTelWaveform(tel_id, *tel_electronic);
    }
}
void LEventRaw::GetTrueImage(AllHessData* hsdata)
{
    event->ClearImage();
    event->ClearShower();
    for(auto i = 0; i < hsdata->event.central.num_teltrg; i++)
    {
        int tel_id = hsdata->event.central.teltrg_list[i];
        auto tel_true_image = std::make_shared<LRTelTrueImage>();
        tel_true_image->event_id = hsdata->mc_event.event;
        tel_true_image->tel_id = tel_id;
        tel_true_image->num_pixels = hsdata->mc_event.mc_pe_list[tel_id - 1].pixels;
        tel_true_image->Allocate();
        if(hsdata->event.trackdata[tel_id - 1].raw_known )
        {
            tel_true_image->tel_alt = hsdata->event.trackdata[tel_id - 1].altitude_raw;
            tel_true_image->tel_az = hsdata->event.trackdata[tel_id - 1].azimuth_raw;
        }
        else 
        {
            tel_true_image->tel_alt = hsdata->run_header.direction[1];
            tel_true_image->tel_az = hsdata->run_header.direction[0];
        }
        for(auto ipix = 0; ipix < tel_true_image->num_pixels; ipix++)
        {
            tel_true_image->true_pe[ipix] = hsdata->mc_event.mc_pe_list[tel_id - 1].pe_count[ipix];
        }
        tel_true_image->Allocate_Pe(hsdata->mc_event.mc_pe_list[tel_id - 1].npe);
        if(hsdata->mc_event.mc_pe_list[tel_id -1].atimes)   
            for(int ipe = 0; ipe < hsdata->mc_event.mc_pe_list[tel_id - 1].npe; ipe++)
            {
                tel_true_image->pe_time[ipe] = hsdata->mc_event.mc_pe_list[tel_id - 1].atimes[ipe];
            }
        tel_true_image->Compute_Spread(); 
        event->AddTelImage(tel_id, *tel_true_image);
        event->event_shower->AddTel(tel_id);
    }
}
void LEventRaw::GetConfig(AllHessData* hsdata)
{
    // Fill all RunConfig parameters
    

    for( auto i = 0; i <hsdata->run_header.ntel; i++)
    {
        int tel_id = hsdata->run_header.tel_id[i];
        auto tel_config = std::make_shared<LRTelescopeConfig>();
        tel_config->SetTelPos(hsdata->run_header.tel_pos[i][0],hsdata->run_header.tel_pos[i][1],hsdata->run_header.tel_pos[i][2]);   
        int num_pixels = hsdata->camera_set[i].num_pixels;
        //tel_config->InitCameraSet(cmd_config.GetCameraName(), tel_id, num_pixels);
        tel_config->InitCameraSet("LAST", tel_id, num_pixels);
        for( auto  ipix= 0; ipix < num_pixels; ipix++)
        {
            tel_config->pix_x[ipix] = hsdata->camera_set[i].xpix[ipix];
            tel_config->pix_y[ipix] = hsdata->camera_set[i].ypix[ipix];
        }

        // Assuming all pixels have the same size and shape!!!
        int pixel_shape = hsdata->camera_set[i].pixel_shape[0];
        double pixel_size = hsdata->camera_set[i].size[0];
        tel_config->pix_shape = pixel_shape;
        tel_config->pix_size = pixel_size;
        tel_config->focal_length = hsdata->camera_set[i].flen;
        this->tel_config->AddTel(tel_id, tel_config);
    }

}

void LEventRaw::GetRunConfig(AllHessData* hsdata)
{   
    run_config->run_number = hsdata->run_header.run;
    run_config->corsika_version = hsdata->mc_run_header.shower_prog_vers;
    run_config->simtel_version = hsdata->mc_run_header.detector_prog_vers;
    run_config->energy_range[0] = hsdata->mc_run_header.E_range[0];
    run_config->energy_range[1] = hsdata->mc_run_header.E_range[1];
    run_config->prod_site_B[0] = hsdata->mc_run_header.B_total;
    run_config->prod_site_B[1] = hsdata->mc_run_header.B_declination;
    run_config->prod_site_B[2] = hsdata->mc_run_header.B_inclination;
    run_config->prod_site_alt  = hsdata->mc_run_header.obsheight;
    run_config->spectral_index = hsdata->mc_run_header.spectral_index;
    run_config->shower_prog_start = hsdata->mc_run_header.shower_prog_start;
    run_config->shower_prog_id = hsdata->mc_run_header.shower_prog_id;
    run_config->detector_prog_start = hsdata->mc_run_header.detector_prog_start;
    run_config->nshowers = hsdata->mc_run_header.num_showers;
    run_config->shower_reuse = hsdata->mc_run_header.num_use;
    run_config->max_alt = hsdata->mc_run_header.alt_range[1];
    run_config->min_alt = hsdata->mc_run_header.alt_range[0];
    run_config->max_az = hsdata->mc_run_header.az_range[1];
    run_config->min_az = hsdata->mc_run_header.az_range[0];
    run_config->diffuse = hsdata->mc_run_header.diffuse;
    run_config->max_viewcone_radius = hsdata->mc_run_header.viewcone[1];
    run_config->min_viewcone_radius = hsdata->mc_run_header.viewcone[0]; 
    run_config->atmposphere = hsdata->mc_run_header.atmosphere;
    run_config->corsika_iact_options = hsdata->mc_run_header.corsika_iact_options;
    run_config->corsika_bunchsize = (int)hsdata->mc_run_header.corsika_bunchsize;
    run_config->corsika_low_E_model = hsdata->mc_run_header.corsika_low_E_model;
    run_config->corsika_high_E_model = hsdata->mc_run_header.corsika_high_E_model;
    run_config->corsika_wlen_min = hsdata->mc_run_header.corsika_wlen_min;
    run_config->corsika_wlen_max = hsdata->mc_run_header.corsika_wlen_max;
}

/**
 * @brief Read configuration from the EVENTIO file
 * 
 */
bool LEventRaw::ProcessEvent()
{
    while( simtel_file->Have_Block())
    {
        switch (simtel_file->GetBlockType()) {
            case IO_TYPE_HESS_RUNHEADER:
                if(simtel_file->Read_Block())
                {
                    simtel_file->Read_Runheader();
                    //hsdata      = simtel_file->GetHsdata();
                }
                break;
            case IO_TYPE_HESS_MCRUNHEADER:
                if( simtel_file->Read_Block())
                {
                    simtel_file->read_hess_mcrunheader();
                }
                break;
            case IO_TYPE_HESS_CAMSETTINGS:
                if(simtel_file->Read_Block())
                {
                    simtel_file->Read_Camsettings();
                }
                break;
            case IO_TYPE_HESS_CAMORGAN:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_Camorgran();
                }
                break;
            case IO_TYPE_HESS_PIXELSET:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_Pixelset();
                }
                break;
            case IO_TYPE_HESS_CAMSOFTSET:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_Camsoftset();
                }
                break;
            case IO_TYPE_HESS_TRACKSET:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_Trackset();
                }
                break;
            case IO_TYPE_HESS_POINTINGCOR:
                if (simtel_file->Read_Block())
                {
                    simtel_file->Read_Pointingcor();
                }
                break;
            case IO_TYPE_HESS_MC_SHOWER:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_MC_Shower();
                    simtel_file->resetheader();
                }
                break;
            case IO_TYPE_MC_TELARRAY:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_MC_Phot();
                    simtel_file->resetheader();
                }
                break;
            case IO_TYPE_HESS_MC_EVENT:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_Mc_Event();
                    simtel_file->resetheader();
                    GetShower(simtel_file->GetHsdata());
                }
                break;
            case IO_TYPE_HESS_MC_PIXMON:
                simtel_file->skip_io_block();
                break;
                if( simtel_file->Read_Block())
                {
                    //simtel_file->Read_MC_Pixel_Moni();
                    simtel_file->resetheader();
                }
                break;
            case IO_TYPE_HESS_TEL_MONI:
                if(simtel_file->Read_Block())
                {
                    simtel_file->Read_Tel_Moni();
                    simtel_file->resetheader();
                }
                break;
            case IO_TYPE_HESS_LASCAL:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_Las_Cal();
                    simtel_file->resetheader();
                }
                break;
            case IO_TYPE_HESS_MC_PE_SUM:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_MC_Pe_Sum();
                    simtel_file->resetheader();
                }
                break;
                // Stop At Type 2020;
            case IO_TYPE_HESS_EVENT:
                if( simtel_file->Read_Block())
                {
                    simtel_file->Read_Event();
                    simtel_file->resetheader();
                }
                return true;
                break;
            default:
                simtel_file->skip_io_block();
                spdlog::info("Skip Block {}", simtel_file->GetBlockType());
                break;
        }
    }
    return false;
}
void LEventRaw::GetEvent()
{
    GetShower(simtel_file->GetHsdata());
    GetEventShower(simtel_file->GetHsdata());
    GetTrueImage(simtel_file->GetHsdata());
    GetEventWaveform(simtel_file->GetHsdata());
}
void LEventRaw::Close()
{
    event->Clear();
    LDataBase::Close();
}
bool LEventRaw::ReadEvent()
{
    bool flag = ProcessEvent();
    GetEvent();
    return flag;
}
