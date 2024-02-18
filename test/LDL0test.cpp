#include "LRDL0Event.hh"
#include "glog/logging.h"
#include "LREventRaw.hh"
#include "LJsonConfig.hh"


int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    google::SetStderrLogging(google::GLOG_INFO);
    LJsonConfig config(argc, argv);
    LREventRaw* event_raw = new LREventRaw(config, 'r');
    LRDL0Event* dl0event = new LRDL0Event(config, 'w');
    //dl0event->InitRootFile();
    dl0event->Read(event_raw->GetRootFile());
    dl0event->CopyEvent();
    while( event_raw->ReadEvent())
    {
        dl0event->SetEvent(event_raw->EventAddress());
        dl0event->HandleEvent();
    }
    dl0event->StoreTTree();
    
    return 0;
}


