

 #include "TCanvas.h"
#include "TH2Poly.h"
#include "gflags/gflags.h"
 #include "Datalevels/LASTDL0/LRDL0Event.hh"
 #include "Datalevels/LJsonConfig.hh"
#include "spdlog/spdlog.h"
 
 DEFINE_int32(event_num, 1, "event_number to displat");
 int main(int argc, char** argv)
 {
    spdlog::set_level(spdlog::level::info);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    LJsonConfig config(argc, argv);
    LRDL0Event* event_dl0 = new LRDL0Event(config, 'r');
    int ievent = 0;
    while(ievent < FLAGS_event_num)
    {
        event_dl0->ReadEvent();
        ievent++;
    }
    event_dl0->ReadEvent();
    std::vector<TCanvas*> canvases;
    event_dl0->Display(canvases);
    for(auto canvas: canvases)
    {
        canvas->Draw();
    }
    
 }