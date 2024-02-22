#include "Datalevels/LASTRAW/LEventRaw.hh"
#include "Datalevels/LASTRAW/LREventRaw.hh"
#include "TFile.h"
#include "TProfile.h"
#include "Datalevels/LJsonConfig.hh"
#include "reconstruction/LHillasGeometryReconstructor.hh"
#include "glog/logging.h"
#include <memory>

int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    google::SetStderrLogging(google::GLOG_INFO);
    LJsonConfig config(argc, argv);
    std::string output_fname = config.GetOutputFileName();
    if(config.GetOutputFileName().compare(0, 4, "/eos") == 0)
    {
        output_fname = config.GetUrl() + config.GetOutputFileName();

    }

    TFile* rootfile = new TFile(output_fname.c_str(), "RECREATE");
    TProfile *time_spread1 = new TProfile("time_spread1", "time_spread1", 100, 0, 1000, 0, 100);
    TProfile *time_spread2 = new TProfile("time_spread2", "time_spread2", 100, 0, 1000, 0, 100);
    for(auto input_fname: config.input_fnames)
    {
        LREventRaw* event_raw = new LREventRaw(config, input_fname);
        std::weak_ptr<LEvent> event;
        TFile* file = new TFile((config.GetUrl() + config.GetOutputFileName()).c_str(), "RECREATE");

        while(event_raw->ReadEvent())
        {
            event = event_raw->EventAddress();
            auto dl0event = event.lock();
            if(dl0event->event_shower->energy > 0.3 || dl0event->event_shower->energy < 0.2)
            {
                continue;
            }

            for(auto tel_id : dl0event->GetImageTelList())
            {
                double tel_postion[2];
                auto tel_image = dl0event->GetTelImage(tel_id);
                auto tel_pos = event_raw->GetTelGroundPos(tel_id);
                tel_postion[0] = tel_pos.first;
                tel_postion[1] = tel_pos.second;

                double x_core = dl0event->event_shower->core_x;
                double y_core = dl0event->event_shower->core_y;
                double tel_alt = dl0event->event_shower->altitude;
                double tel_az  = dl0event->event_shower->azimuth;
                double rp = LHillasGeometryReconstructor::ComputeImpactdistance(tel_postion, tel_alt, tel_az, x_core, y_core);
                time_spread1->Fill(rp, tel_image->time_spread1);
                time_spread2->Fill(rp, tel_image->time_spread2);

            }
            
        }
        delete event_raw;
    }
    time_spread1->Write();   
    time_spread2->Write();
    rootfile->Close();
}