#include "Datalevels/LASTDL2/LRDL2aEvent.hh"

#include "Datalevels/LASTDL1/LRDL1bEvent.hh"
#include "Datalevels/LJsonConfig.hh"
#include "spdlog/spdlog.h"

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::info);
    LJsonConfig config(argc, argv);

    auto lastdl1b = new LRDL1bEvent(config, 'r');
    auto lastdl2a = new LRDL2aEvent(config, 'w');

    while(lastdl1b->ReadEvent())
    {
        lastdl2a->GetEvent(*lastdl1b);
        lastdl2a->HandleEvent();
    }
    lastdl2a->StoreTTree();

}