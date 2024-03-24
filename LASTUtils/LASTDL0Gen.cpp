#include "LRDL0Event.hh"
#include "LREventRaw.hh"
#include "../LJsonConfig.hh"
#include "spdlog/spdlog.h"


int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::info);
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


