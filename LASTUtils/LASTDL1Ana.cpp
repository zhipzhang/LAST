/**
 * @file LASTDL1Ana.cpp
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief Analysis the LAST DL1 Data
 * @version 0.1
 * @date 2024-03-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "LRDL1Event.hh"
#include "TH2.h"
#include "spdlog/spdlog.h"
#include "Datalevels/LJsonConfig.hh"
#include "TFile.h"
 #include "TProfile.h"
 #include "reconstruction/LHillasGeometryReconstructor.hh"
 #include "TMath.h"
 #include <vector>
 double  angle_between(double azimuth1, double altitude1, double azimuth2, double altitude2)
    {
        double ax1 = cos(azimuth1)*cos(altitude1);
        double ay1 = sin(-azimuth1)*cos(altitude1);
        double az1 = sin(altitude1);
        double ax2 = cos(azimuth2)*cos(altitude2);
        double ay2 = sin(-azimuth2)*cos(altitude2);
        double az2 = sin(altitude2);
        double cos_ang = ax1*ax2 + ay1*ay2 + az1*az2;
        /* Check for rounding errors pushing us outside the valid range. */
        if ( cos_ang <= -1. )
            return M_PI;
        else if ( cos_ang >= 1. )
            return 0.;
        else
            return acos(cos_ang);
    }
 int main(int argc, char** argv)
 {
    spdlog::set_level(spdlog::level::info);
    LJsonConfig config(argc, argv);
    std::string output_name = config.GetOutputFileName();
    if(config.GetOutputFileName().compare(0, 4, "/eos") == 0)
    {
      output_name = config.GetUrl() + output_name;
    }
    TFile* root_file = new TFile(output_name.c_str(), "RECREATE");
    // analyze the leakage data , leakage1 versus impact paramter in certen energy range
    TProfile* p1 = new TProfile("p1", "leakage1 versus impact parameter(1 - 2.5TeV)", 50, 0, 500);
    TProfile* p2 = new TProfile("p2", "leakage1 versus impact parameter(2.5 - 5TeV)", 50, 0, 500);
    TProfile* p3 = new TProfile("p3", "leakage1 versus impact parameter(5TeV - 10TeV)", 50, 0, 500);
    TProfile* p4 = new TProfile("p4", "leakage1 versus impact parameter(10 - 20TeV)", 50, 0, 500);
    TProfile* p5 = new TProfile("p5", "leakage1 versus impact parameter(20 - 30TeV)", 50, 0, 500);
    TProfile* p6 = new TProfile("p6", "leakage1 versus impact parameter(30TeV - 50TeV)", 50, 0, 500);
    TProfile* p7 = new TProfile("p7", "leakage2 versus impact parameter(1 - 2.5TeV)", 50, 0, 500);
    TProfile* p8 = new TProfile("p8", "leakage2 versus impact parameter(2.5 - 5TeV)", 50, 0, 500);
    TProfile* p9 = new TProfile("p9", "leakage2 versus impact parameter(5TeV - 10TeV)", 50, 0, 500);
    TProfile* p10 = new TProfile("p10", "leakage2 versus impact parameter(10 - 20TeV)", 50, 0, 500);
    TProfile* p11 = new TProfile("p11", "leakage2 versus impact parameter(20 - 30TeV)", 50, 0, 500);
    TProfile* p12 = new TProfile("p12", "leakage2 versus impact parameter(30TeV - 50TeV)", 50, 0, 500);
    TProfile* p13 = new TProfile("p13", "concentration_core versus impact parameter(30TeV - 50TeV)", 50, 0, 500);
    TProfile* p14 = new TProfile("p14", "concentration_core versus impact parameter(1 - 2.5TeV)", 50, 0, 500);
    TProfile* p15 = new TProfile("p15", "concentration_core versus impact parameter(2.5 - 5TeV)", 50, 0, 500);
    TProfile* p16 = new TProfile("p16", "concentration_core versus impact parameter(5TeV - 10TeV)", 50, 0, 500);
    TProfile* p17 = new TProfile("p17", "concentration_core versus impact parameter(10 - 20TeV)", 50, 0, 500);
    TProfile* p18 = new TProfile("p18", "concentration_core versus impact parameter(20 - 30TeV)", 50, 0, 500);
    TProfile* p19 = new TProfile("p19", "concentration_cog versus impact parameter(30TeV - 50TeV)", 50, 0, 500);
    TProfile* p20 = new TProfile("p20", "concentration_cog versus impact parameter(1 - 2.5TeV)", 50, 0, 500);
    TProfile* p21 = new TProfile("p21", "concentration_cog versus impact parameter(2.5 - 5TeV)", 50, 0, 500);
    TProfile* p22 = new TProfile("p22", "concentration_cog versus impact parameter(5TeV - 10TeV)", 50, 0, 500);
    TProfile* p23 = new TProfile("p23", "concentration_cog versus impact parameter(10 - 20TeV)", 50, 0, 500);
    TProfile* p24 = new TProfile("p24", "concentration_cog versus impact parameter(20 - 30TeV)", 50, 0, 500);
    LRDL1Event* dl1event = new LRDL1Event(config);
    for(auto input_fname: config.input_fnames)
    {
      dl1event->ReadROOTFile(input_fname);
      while(dl1event->ReadEvent())
      {
         double core_x = dl1event->GetMCxcore();
         double core_y = dl1event->GetMCycore();
         double mc_alt = dl1event->GetMCalt();
         double mc_az  = dl1event->GetMCaz();
         double mc_energy = dl1event->GetMCenergy();
         for(auto tel_id: dl1event->GetTelList())
         {
            double tel_position[2];
            auto tel_pos = dl1event->GetTelGroundPos(tel_id);
            tel_position[0] = tel_pos.first;
            tel_position[1] = tel_pos.second;
            double offset = angle_between(mc_az, mc_alt, 180 * TMath::DegToRad(), 20 * TMath::DegToRad());
            spdlog::info("offset: {}", offset);
            if(offset > 0.5)
            {
               break;
            }
            double rp = LHillasGeometryReconstructor::ComputeImpactdistance(tel_position, mc_alt, mc_az, core_x, core_y);
            if(mc_energy >= 1 && mc_energy < 2.5)
            {
               p1->Fill(rp, (*dl1event)[tel_id].GetLeakage1());
               p7->Fill(rp, (*dl1event)[tel_id].GetLeakage2());
               p13->Fill(rp, (*dl1event)[tel_id].GetConcentrationCore());
               p19->Fill(rp, (*dl1event)[tel_id].GetConcentrationCog());
            }
            else if(mc_energy > 2.5 && mc_energy < 5 )
            {
               p2->Fill(rp, (*dl1event)[tel_id].GetLeakage1());
               p8->Fill(rp, (*dl1event)[tel_id].GetLeakage2());
               p14->Fill(rp, (*dl1event)[tel_id].GetConcentrationCore());
               p20->Fill(rp, (*dl1event)[tel_id].GetConcentrationCog());
            }
            else if(mc_energy >=5 && mc_energy < 10)
            {
               p3->Fill(rp, (*dl1event)[tel_id].GetLeakage1());
               p9->Fill(rp, (*dl1event)[tel_id].GetLeakage2());
               p15->Fill(rp, (*dl1event)[tel_id].GetConcentrationCore());
               p21->Fill(rp, (*dl1event)[tel_id].GetConcentrationCog());
            }
            else if(mc_energy >= 10 && mc_energy < 20)
            {
               p4->Fill(rp, (*dl1event)[tel_id].GetLeakage1());
               p10->Fill(rp, (*dl1event)[tel_id].GetLeakage2());
               p16->Fill(rp, (*dl1event)[tel_id].GetConcentrationCore());
               p22->Fill(rp, (*dl1event)[tel_id].GetConcentrationCog());
            }
            else if(mc_energy >= 20 && mc_energy < 30)
            {
               p5->Fill(rp, (*dl1event)[tel_id].GetLeakage1());
               p11->Fill(rp, (*dl1event)[tel_id].GetLeakage2());
               p17->Fill(rp, (*dl1event)[tel_id].GetConcentrationCore());
               p23->Fill(rp, (*dl1event)[tel_id].GetConcentrationCog());
            }
            else if(mc_energy >= 30 && mc_energy < 50)
            {
               p6->Fill(rp, (*dl1event)[tel_id].GetLeakage1());
               p12->Fill(rp, (*dl1event)[tel_id].GetLeakage2());
               p18->Fill(rp, (*dl1event)[tel_id].GetConcentrationCore());
               p24->Fill(rp, (*dl1event)[tel_id].GetConcentrationCog());
            }
         }
      }
      dl1event->Close();
      spdlog::info("Finish reading file: {}", input_fname);
    }
    root_file->cd();
    p1->Write();
    p2->Write();
    p3->Write();
    p4->Write();
    p5->Write();
    p6->Write();
    p7->Write();
    p8->Write();
    p9->Write();
    p10->Write();
    p11->Write();
    p12->Write();
    p13->Write();
    p14->Write();
    p15->Write();
    p16->Write();
    p17->Write();
    p18->Write();
    p19->Write();
    p20->Write();
    p21->Write();
    p22->Write();
    p23->Write();
    p24->Write();
   root_file->Close();


    return 0;
 }