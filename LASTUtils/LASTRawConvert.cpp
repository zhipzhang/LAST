#include "LREventRaw.hh"
#include "../LJsonConfig.hh"
#include "spdlog/spdlog.h"

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::info);
    LJsonConfig config(argc, argv);
    LREventRaw* event_raw = new LREventRaw(config, 'w');
    while(event_raw->HandleEvent())
    {
    }

    event_raw->StoreTTree();

}

