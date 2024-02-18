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
#include "Datalevels/LASTDL1/LDL1bEvent.hh"


LRDL1bEvent::LRDL1bEvent(const LJsonConfig& config, const char mode): cmd_config(config), LDL1bEvent()
{
    if( mode == 'r')
    {
        //ReadROOTFile(cmd_config.GetInputFileName());
    }
    if (mode == 'w')
    {
        InitROOTFile();
    }

}

void LRDL1bEvent::InitROOTFile()
{
    root_file = std::make_unique<TFile>(cmd_config.GetOutputFileName().c_str(), "RECREATE");
    dir = root_file->mkdir(dirname);
    dir->cd();
    dl1arraytree = new TTree("dl1arraytree", "dl1arraytree");
    dl1arraytree->Branch("dl1barray", &ldl1barrayevent);
    dl1teltree = new TTree("dl1teltree", "dl1teltree");
    dl1teltree->Branch("dl1btel", &dl1_televent);

}

void LRDL1bEvent::HandleEvent()
{
    for(auto itel: ldl1bevent->GetKeys())
    {
        dl1_televent = (*ldl1bevent)[itel].get();
        dl1teltree->Fill();
    }
    dl1arraytree->Fill();
}

void LRDL1bEvent::StoreTTree()
{
    dir->cd();
    dl1teltree->BuildIndex("event_id", "tel_id");
    dl1arraytree->Write();
    dl1teltree->Write();
    root_file->Close();
}
