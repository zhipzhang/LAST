#include "LRDL0Event.hh"


LRDL0Event::LRDL0Event(const LJsonConfig& config, const char mode): cmd_config(config), LDL0Event()
{
    if(mode == 'r')
    {
        ReadROOTFile(config.GetInputFileName());   
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
    root_file.reset(TFile::Open(outname.c_str(), "RECREATE"));
    dl0_dir = root_file->mkdir(dl0_dirname);
    dl0_dir->cd();
    dl0event_tree = new TTree("dl0event", "dl0event");
    dl0event_tree->Branch("dl0_tel_event", &dl0_tel_event);
    arrayevent = new TTree("arrayevent", "arrayevent");
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
    dl0event_tree->BuildIndex("event_id", "tel_id");
    dl0event_tree->Write();
    arrayevent->Write();
    root_file->Write();
    root_file->Close();
}
void LRDL0Event::ReadROOTFile(std::string filename)
{
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
        LOG(ERROR) << "Can't find the directory " << dl0_dirname << " in file " << filename;
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
        auto flag = dl0event_tree->GetEntryWithIndex(event_id, itel);
        if( flag == -1)
        {
            LOG(ERROR) << "Can't find the event " << event_id << " in telescope " << itel;
            return false;
        }
        AddTelEvents(itel, dl0_tel_event);
    }
    return true;
}