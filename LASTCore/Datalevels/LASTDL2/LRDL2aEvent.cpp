#include "LRDL2aEvent.hh"
#include "../LASTDL2/LDL2aArrayEvent.hh"
#include "../LASTDL2/LDL2aTelEvent.hh"
#include "../LJsonConfig.hh"
#include <memory>

LRDL2aEvent::LRDL2aEvent(const LJsonConfig& cfg, const char mode): cmd_config(cfg)
{
    ldl2aarrayevent = new LRDL2aArrayEvent();
    ldl2aevent = std::make_shared<LTelescopes<std::shared_ptr<LRDL2aTelEvent>>>();
    if( mode == 'r')
    {
    }
    if (mode == 'w')
    {
        InitROOTFile();
    }

}

void LRDL2aEvent::InitROOTFile()
{
    std::string fname  = cmd_config.GetOutputFileName();
    if(cmd_config.GetOutputFileName().compare(0, 4, "/eos") == 0)
    {
        fname = cmd_config.GetUrl() + cmd_config.GetOutputFileName();
    }
    root_file.reset(TFile::Open(fname.c_str(), "RECREATE"));
    dir = root_file->mkdir(dirname);
    dir->cd();
    dl2a_arraytree = new TTree("dl2a_arraytree", "dl2a_arraytree");
    dl2a_arraytree->Branch("dl2aarrayevent", &ldl2aarrayevent);
    dl2a_teltree = new TTree("dl2a_teltree", "dl2a_teltree");
    dl2a_teltree->Branch("dl2atelevent", &dl2a_televent);
}
void LRDL2aEvent::GetEvent(const LRDL1bEvent & ldl1bevent)
{
    ldl2aevent->Clear();
    *ldl2aarrayevent = ldl1bevent.GetArrayEvent();
    ldl2aarrayevent->SetEstimateEnergy(ldl1bevent.GetRecEnergy());
    ldl2aarrayevent->SetHadroness(ldl1bevent.GetRecHadroness());
    for(int i = 0; i < ldl1bevent.GetArrayEvent().GetRecNums(); i++)
    {
        int tel_id = ldl1bevent.GetRecTelID(i);
        auto ldl2atelevent = std::make_shared<LRDL2aTelEvent>();
        *ldl2atelevent = ldl1bevent.GetTelEvent(i);
        ldl2atelevent->SetEstimateEnergy(ldl1bevent.GetTelEnergy(i));
        ldl2atelevent->SetEstimateHadroness(ldl1bevent.GetTelHadroness(i));
        ldl2aevent->AddTel(tel_id, ldl2atelevent);
        ldl2aarrayevent->Setweight(cmd_config.weight);
    }
}

void LRDL2aEvent::HandleEvent()
{
    for(const auto itel: ldl2aevent->GetKeys())
    {
        dl2a_televent = (*ldl2aevent)[itel].get();
        dl2a_teltree->Fill();
    }
    dl2a_arraytree->Fill();
}

void LRDL2aEvent::StoreTTree()
{
    dir->cd();
    dl2a_arraytree->Write();
    dl2a_teltree->Write();
    root_file->Close();
}