/**
 * @file LRDL1bEvent.cpp
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-02-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "LRDL1bEvent.hh"
#include "LDL1bEvent.hh"
#include "TFile.h"
#include "spdlog/spdlog.h"


LRDL1bEvent::LRDL1bEvent(const LJsonConfig& config, const char mode): cmd_config(config), LDL1bEvent()
{
    if( mode == 'r')
    {
        ReadROOTFile(cmd_config.GetInputFileName());
    }
    if (mode == 'w')
    {
        InitROOTFile();
    }

}

void LRDL1bEvent::InitROOTFile()
{
    std::string fname  = cmd_config.GetOutputFileName();
    if(cmd_config.GetOutputFileName().compare(0, 4, "/eos") == 0)
    {
        fname = cmd_config.GetUrl() + cmd_config.GetOutputFileName();
    }
    root_file.reset(TFile::Open(fname.c_str(), "RECREATE"));
    dir = root_file->mkdir(dirname);
    dir->cd();
    dl1arraytree = new TTree("dl1arraytree", "dl1arraytree");
    dl1arraytree->Branch("dl1barray", &ldl1barrayevent);
    dl1teltree = new TTree("dl1teltree", "dl1teltree");
    dl1teltree->Branch("dl1btel", &dl1_televent);

}
void LRDL1bEvent::ReadROOTFile(std::string filename)
{
        if(filename.compare(0, 4, "/eos") == 0)
    {
        filename = cmd_config.GetUrl() + filename;
    }
    root_file.reset(TFile::Open(filename.c_str(), "READ"));
    dir = root_file->GetDirectory(dirname);
    if(dir)
    {
        dl1arraytree = dir->Get<TTree>("dl1arraytree");
        dl1teltree = dir->Get<TTree>("dl1teltree");
        if(dl1teltree->GetBranch("Estimate_Energy") != nullptr)
        {
            have_energy = true;
            dl1teltree->SetBranchAddress("Estimate_Energy", &Estimate_Energy);
        }
        if(dl1teltree->GetBranch("hadroness") != nullptr)
        {
            have_hadroness = true;
            dl1teltree->SetBranchAddress("hadroness", &Estimate_Hadroness);
        }
        dl1arraytree->SetBranchAddress("dl1barray", &ldl1barrayevent);
        dl1teltree->SetBranchAddress("dl1btel", &dl1_televent);
        nevents = dl1arraytree->GetEntries();

    }
}

bool LRDL1bEvent::ReadEvent()
{
    if( ievents >= nevents)
    {
        return false;
    }
    dl1arraytree->GetEntry(ievents++);
    ldl1bevent->Clear();
    for(auto itel: ldl1barrayevent->reconstruction_tels)
    {
        dl1teltree->GetEntry(telescope_flag++);
        if(dl1_televent->GetTelID() != itel || dl1_televent->GetEventID() != ldl1barrayevent->event_id)
        {
            spdlog::error("1Can't find the event {} in telescope {}", ldl1barrayevent->event_id, itel);
            spdlog::error("2Can't find the event {} in telescope {}", dl1_televent->GetEventID(), dl1_televent->GetTelID());
            spdlog::error("The telescope flag is {}", telescope_flag);
            spdlog::error("The ievent is {}", ievents);
            continue;
        }
        AddTelEvent(itel, *dl1_televent);
    }
    return true;
}
void LRDL1bEvent::HandleEvent()
{
    for(auto itel: ldl1barrayevent->reconstruction_tels)
    {
        dl1_televent = (*ldl1bevent)[itel].get();
        dl1teltree->Fill();
    }
    dl1arraytree->Fill();
}

void LRDL1bEvent::StoreTTree()
{
    dir->cd();
    //dl1teltree->BuildIndex("event_id", "tel_id");
    dl1arraytree->Write();
    dl1teltree->Write();
    root_file->Close();
}
