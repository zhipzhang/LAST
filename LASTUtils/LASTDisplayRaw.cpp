/**
 * @file LASTDisplayRaw.cpp
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief  Display the raw image of the event
 * @version 0.1
 * @date 2024-04-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
 
 #include "TCanvas.h"
#include "TH2Poly.h"
#include "gflags/gflags.h"
 #include "Datalevels/LASTRAW/LREventRaw.hh"
 #include "Datalevels/LJsonConfig.hh"
 DEFINE_int32(event_num, 1, "event_number to displat");
 int main(int argc, char** argv)
 {
    spdlog::set_level(spdlog::level::info);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    LJsonConfig config(argc, argv);
    LREventRaw* event_raw = new LREventRaw(config, 'r');
    int ievent = 0;
    while(ievent < FLAGS_event_num)
    {
        event_raw->ReadEvent();
        ievent++;
    }
    event_raw->ReadEvent();
    std::vector<TCanvas*> canvases;
    event_raw->Display(canvases);
    for(auto canvas: canvases)
    {
        canvas->Draw();
    }
    
 }