#include "LRDL1Event.hh"
#include "Datalevels/LShower.hh"
#include "glog/logging.h"


LRDL1Event::LRDL1Event(const LJsonConfig& cfg, const char mode): cmd_config(cfg), LDL1Event()
{
    dl1_tel_event = new LRDL1TelEvent();
    ldl1array = new LRArray();
    if(mode == 'r')
    {
        
    }
    if( mode == 'w')
    {
        InitRootFile();
    }
}
void LRDL1Event::InitRootFile()
{
    if(outname.empty())
    {
        outname = cmd_config.GetOutputFileName();
    }
    root_file.reset(TFile::Open(outname.c_str(), "RECREATE"));
    dl1_dir = root_file->mkdir(dl1_dirname);
    dl1_dir->cd();
    dl1event_tree = new TTree("dl1event", "dl1event");
    dl1event_tree->Branch("dl1_tel_event", &dl1_tel_event);
    arrayevent = new TTree("arrayevent", "arrayevent");
    arrayevent->Branch("arrayevent", &ldl1array);
}

void LRDL1Event::ReadROOTFile(std::string filename)
{
    root_file.reset(TFile::Open(filename.c_str(), "READ"));
    Read(root_file.get());
    dl1_dir = root_file->GetDirectory(dl1_dirname);
    if( dl1_dir)
    {
        dl1event_tree = dl1_dir->Get<TTree>("dl1event");
        arrayevent    = dl1_dir->Get<TTree>("arrayevent");
        dl1event_tree->SetBranchAddress("dl1_tel_event", &dl1_tel_event);
        arrayevent->SetBranchAddress("arrayevent", &ldl1array);
        nevents = dl1event_tree->GetEntries();
    }
    else 
    {
        LOG(ERROR) << "Can't find the directory " << dl1_dirname << " in file " << filename;
    }

}

bool LRDL1Event::ReadEvent()
{
        if( ievents >= nevents)
        {
            return false;
        }
        dl1event_tree->GetEntry(ievents++);
        arrayevent->GetEntry(ievents);
        ldl1event->Clear();
        for( const auto itel: ldl1array->trigger_tels)
        {
            auto flag = dl1event_tree->GetEntryWithIndex(ldl1array->event_id, itel);
            if( flag == -1)
            {
                LOG(ERROR) << "Can't find the event " << ldl1array->event_id << " in telescope " << itel;
                return false;
            }
            AddTelEvent(itel, dl1_tel_event);
        }
        return true;
}
void LRDL1Event::HandleEvent()
{
    for(const auto itel: ldl1event->GetKeys())
    {
        dl1_tel_event = (*ldl1event)[itel].get();
        dl1event_tree->Fill();
    }
    arrayevent->Fill();
}

void LRDL1Event::StoreTTree()
{
    dl1_dir->cd();
    dl1event_tree->BuildIndex("event_id", "tel_id");
    dl1event_tree->Write();
    arrayevent->Write();
    root_file->Write();
    root_file->Close();
}

void LRDL1Event::CopyEvent()
{
    if(copyshower)
    {
        CopyShower(root_file.get());
    }
    if(copyinstrument)
    {
        CopyTelConfig(root_file.get());
    }
    if( copyrunconfig)
    {
        CopyRunConfig(root_file.get());
    }
}