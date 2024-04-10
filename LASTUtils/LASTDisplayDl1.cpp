

 #include "TCanvas.h"
#include "TH2Poly.h"
#include "gflags/gflags.h"
 #include "Datalevels/LASTDL0/LRDL0Event.hh"
 #include "Datalevels/LJsonConfig.hh"
 #include "Datalevels/LASTDL1/LDL1Event.hh"
#include "spdlog/spdlog.h"
#include "Image/LImageProcessor.hh"
 
 DEFINE_int32(event_num, 1, "event_number to displat");
 int main(int argc, char** argv)
 {
    spdlog::set_level(spdlog::level::info);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    LJsonConfig config(argc, argv);
    LRDL0Event* event_dl0 = new LRDL0Event(config, 'r');
    auto event_dl1 = new LDL1Event();
    event_dl1->Read(event_dl0->GetRootFile());
    auto last_image_processor = new LImageProcessor();
    last_image_processor->InitGeometry(*(event_dl0->GetTelescopesConfig()));
    int ievent = 0;
    while(ievent < FLAGS_event_num)
    {
        event_dl0->ReadEvent();
        ievent++;
    }
    event_dl0->ReadEvent();
    last_image_processor->ParameterizationImage(*event_dl0, *event_dl1);
    std::vector<TCanvas*> canvases;
    event_dl1->Display(canvases);
    for(auto canvas: canvases)
    {
        canvas->Draw();
    }
    
 }