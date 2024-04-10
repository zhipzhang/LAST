#include "LRDL0Event.hh"
#include "TCanvas.h"
#include "spdlog/spdlog.h"
#include <root/RtypesCore.h>
#include "TPaveText.h"


LRDL0Event::LRDL0Event(const LJsonConfig& config, const char mode): cmd_config(config), LDL0Event()
{
    if(mode == 'r')
    {
        ReadROOTFile(cmd_config.GetInputFileName());   
    }
    if( mode == 'w')
    {
        InitRootFile();
    }
}
void LRDL0Event::InitRootFile()
{
    if(outname.empty())
    {
        outname = cmd_config.GetOutputFileName();
    }
    if(outname.compare(0, 4, "/eos") == 0)
    {
        outname = cmd_config.GetUrl() + outname;
    }
    root_file.reset(TFile::Open(outname.c_str(), "RECREATE"));
    dl0_dir = root_file->mkdir(dl0_dirname);
    dl0_dir->cd();
    dl0event_tree = new TTree("dl0event", "dl0event", 99, dl0_dir);
    dl0event_tree->Branch("dl0_tel_event", &dl0_tel_event);
    arrayevent = new TTree("arrayevent", "arrayevent", 99, dl0_dir);
    arrayevent->Branch("arrayevent", &dl0array);
}
void LRDL0Event::HandleEvent()
{
    LDL0Event::GetEvent();
    for(const auto itel: dl0event->GetKeys())
    {
        dl0_tel_event = (*dl0event)[itel].get();
        dl0event_tree->Fill();
    }
    dl0array = raw_event->event_shower;
    arrayevent->Fill();
}
void LRDL0Event::CopyEvent()
{
    if(copyshower)
    {
        CopyShower(root_file.get());
    }
    if(copyinstrument)
    {
        CopyTelConfig(root_file.get());
    }
    if(copyrunconfig)
    {
        CopyRunConfig(root_file.get());
    }
}
void LRDL0Event::StoreTTree()
{
    dl0_dir->cd();
    //dl0event_tree->BuildIndex("event_id", "tel_id");
    dl0event_tree->Write();
    arrayevent->Write();
    root_file->Write();
    root_file->Close();
}
void LRDL0Event::ReadROOTFile(std::string filename)
{
    if(filename.compare(0, 4, "/eos") == 0)
    {
        filename = cmd_config.GetUrl() + filename;
    }
    root_file.reset(TFile::Open(filename.c_str(), "READ"));
    Read(root_file.get());
    dl0_dir = root_file->GetDirectory(dl0_dirname);
    if( dl0_dir)
    {
        dl0event_tree = dl0_dir->Get<TTree>("dl0event");
        arrayevent = dl0_dir->Get<TTree>("arrayevent");
        dl0event_tree->SetBranchAddress("dl0_tel_event", &dl0_tel_event);
        arrayevent->SetBranchAddress("arrayevent", &dl0array);
        nevents = arrayevent->GetEntries();
    }
    else
    {
        spdlog::error("Can't find the directory {} in file {}", dl0_dirname, filename);
    }
}

bool LRDL0Event::ReadEvent()
{
    if(ievents >= nevents)
    {
        return false;
    }
    arrayevent->GetEntry(ievents++);
    int event_id = dl0array->event_id;
    dl0event->Clear();
    for( const auto itel : dl0array->trigger_tels)
    {
        dl0event_tree->GetEntry(telescope_flag++);
        if(dl0_tel_event->GetEventId() != event_id || dl0_tel_event->GetTelId() != itel)
        {
            spdlog::error("Can't find the event {} in telescope {}", event_id, itel);
            return false;
        }
        AddTelEvents(itel, dl0_tel_event);
    }
    return true;
}

void LRDL0Event::Display(std::vector<TCanvas *> &canvases)
{
    for(auto itel: dl0event->GetKeys())
    {
        TCanvas* canvas = new TCanvas(Form("Camera_Image_%d", itel), "LACT RAW Image", 1800, 1800);
        TH2Poly* camera = new TH2Poly("camera_bin", "", -6, 6, -6, 6);
        camera->SetMinimum(0.1);
        camera->SetStats(0);
        auto tel_event = (*dl0event)[itel];
        LDataBase::Fill2Poly<Double32_t>(camera, itel, tel_event->GetTruePe());
        camera->Draw("colz");
        TPaveText *pavet = new TPaveText(-6, 6.3, 6, 7.6);
        pavet->SetFillStyle(0);
        double image_sum = std::accumulate(tel_event->GetTruePe(), tel_event->GetTruePe() + tel_event->GetNumPixels(), 0);
        pavet->AddText(Form("Event ID: %d, Telescope ID: %d Image Size: %.2lf",tel_event->GetEventId() ,tel_event->GetTelId(), image_sum));
        pavet->Draw("same");
        canvas->SaveAs(Form("Event_%d_Tel_%d_dl0.png", tel_event->GetEventId(), tel_event->GetTelId()));
    }
}