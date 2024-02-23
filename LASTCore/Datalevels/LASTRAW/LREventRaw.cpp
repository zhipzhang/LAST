#include "LEventRaw.hh"
#include "SimTel_File.hh"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"
#include "LREventRaw.hh"
#include <memory>

LREventRaw::LREventRaw(const LJsonConfig& cmd_config, const char mode): cmd_config(cmd_config),LEventRaw()
{
    input_fname = cmd_config.GetInputFileName();
    if( mode == 'w' || mode == 't')
    {
        if(input_fname.compare(0, 4, "/eos") == 0)
        {
            simtel_file = new LAST_IO::SimTelIO(input_fname, cmd_config.GetMaxIOLength(), cmd_config.GetUrl());
        }
        else 
        {
            simtel_file = new LAST_IO::SimTelIO(input_fname, cmd_config.GetMaxIOLength());
        }
        if( mode == 'w')
        {
            InitRootFile();
            if(!cmd_config.WriteWaveform())
            {
                waveform_tree->SetBranchStatus("*", 0);
            }
        }
    }
    if( mode == 'r')
    {
        ReadROOTFile(cmd_config.GetInputFileName());
    }
}
LREventRaw::LREventRaw(const LJsonConfig& cmd_config, std::string filename): cmd_config(cmd_config),LEventRaw()
{
    input_fname = filename;
    if(input_fname.compare(0, 4, "/eos") == 0)
    {
        input_fname = cmd_config.GetUrl() + input_fname;
    }
    ReadROOTFile(input_fname);
}

void LREventRaw::InitRootFile()
{
    if( output_fname.empty())
    {
        output_fname = cmd_config.GetOutputFileName();
        if(output_fname.compare(0, 4, "/eos") == 0)
        {
            output_fname = cmd_config.GetUrl() + output_fname;
        }
    }
    rootfile.reset(TFile::Open(output_fname.c_str(), "RECREATE"));
    simulation_dir = rootfile->mkdir("simulation");
    event_dir = simulation_dir->mkdir(dirname);
    true_image_tree = new TTree("true_image", "true image in the camera",99, event_dir);
    waveform_tree = new TTree("waveform", "waveform in the camera", 99, event_dir);
    arrayevent = new TTree("arrayevent", "array event information", 99, event_dir);

    rtel_electronic = new LRTelElectronic();
    rtel_true_image = new LRTelTrueImage();
    true_image_tree->Branch("TruePeContainer", &rtel_true_image);
    waveform_tree->Branch("WaveformContainer", &rtel_electronic);
    arrayevent->Branch("ArrayEvent", &event->event_shower);

}
bool LREventRaw::HandleEvent()
{
    bool flag = ProcessEvent();
    if( !flag)
    {
        return flag;
    }
    GetEvent();
    for( const auto itel: event->GetImageTelList())
    {
        rtel_true_image = event->GetTelImage(itel);
        true_image_tree->Fill();
    }
    for( const auto itel : event->GetWaveformTelList())
    {
        rtel_electronic = event->GetTelWaveform(itel);
        waveform_tree->Fill();
    }
    arrayevent->Fill();
    return flag;
}
void LREventRaw::GetShower(AllHessData *hsdata)
{
    LEventRaw::GetShower(hsdata);
    WriteShower(rootfile.get());
}
void LREventRaw::Close()
{
    rootfile->Close();
    LEventRaw::Close();
}
void LREventRaw::StoreTTree()
{
    ReadRunConfig();
    ReadTelConfig();
    true_image_tree->BuildIndex("event_id", "tel_id");
    if( cmd_config.WriteWaveform())
        waveform_tree->BuildIndex("event_id", "tel_id");
    WriteConfig(rootfile.get());
    event_dir->cd();
    true_image_tree->Write();
    waveform_tree->Write();
    arrayevent->Write();
    simulation_shower_dir->cd();
    shower_tree->Write();
    rootfile->Write();
    rootfile->Close();
}
void LREventRaw::ReadROOTFile(std::string filename)
{
    rootfile.reset(TFile::Open(filename.c_str(), "READ"));
    rootfile->cd("simulation");
    Read(rootfile.get());
    event_dir = gDirectory->GetDirectory(dirname);
    if( event_dir)
    {
        true_image_tree = (TTree*)event_dir->Get("true_image");
        waveform_tree = (TTree*)event_dir->Get("waveform");
        arrayevent = (TTree*)event_dir->Get("arrayevent");
        true_image_tree->SetBranchAddress("TruePeContainer", &rtel_true_image);
        waveform_tree->SetBranchAddress("WaveformContainer", &rtel_electronic);
        arrayevent->SetBranchAddress("ArrayEvent",& event->event_shower);
        nevents = arrayevent->GetEntries();
    }
    else
    {
        LOG(ERROR) << "Can't find the directory " << dirname << " in file " << filename;
    }
}
bool LREventRaw::ReadEvent()
{
    if(event_dir)
    {
        if( ievents >= nevents)
        {
            return false;
        }
        arrayevent->GetEntry(ievents);
        ievents++;
        int event_id = event->event_shower->GetEventID();
        event->Clear();
        for( const auto itel: event->event_shower->trigger_tels)
        {
            auto flag_true = true_image_tree->GetEntryWithIndex(event_id, itel);
            auto flag_wave = waveform_tree->GetEntryWithIndex(event_id, itel);
            if (flag_true == -1)
            {
                LOG(ERROR) << "Can't find the event " << event_id << " in telescope " << itel;
                return false;
            }

            event->AddTelImage(itel, rtel_true_image);
            if( flag_wave != -1)
            {    
                event->AddTelWaveform(itel, rtel_electronic);
                HaveWaveform = true;
            }

        }
        return true;

    }
    return false;
}
LREventRaw::~LREventRaw()
{
}