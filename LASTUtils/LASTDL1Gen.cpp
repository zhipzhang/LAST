#include "LRDL0Event.hh"
#include "LRDL1Event.hh"
#include "Image/LImageProcessor.hh"
#include "../LJsonConfig.hh"
#include "spdlog/spdlog.h"



int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::info);
    LJsonConfig config(argc, argv);
    auto lastdl0 = new  LRDL0Event(config, 'r');
    auto lastdl1 = new LRDL1Event(config, 'w');
    auto last_image_processor = new LImageProcessor();
    //lastdl0->ReadROOTFile(config.GetInputFileName());
    last_image_processor->InitGeometry(*(lastdl0->GetTelescopesConfig()));
    lastdl1->Read(lastdl0->GetRootFile());
    lastdl1->CopyEvent();
    while(lastdl0->ReadEvent())
    {
        last_image_processor->ParameterizationImage(*lastdl0, *lastdl1);
        lastdl1->HandleEvent();
    }
    lastdl1->StoreTTree();
}