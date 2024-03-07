#include "LREventRaw.hh"
#include "LJsonConfig.hh"
#include "RtypesCore.h"
#include "spdlog/spdlog.h"

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::info);
    gDebug = 5;
    LJsonConfig config(argc, argv);
    LREventRaw* event_raw = new LREventRaw(config);
    while(event_raw->HandleEvent())
    {
    }

    event_raw->StoreTTree();

}

