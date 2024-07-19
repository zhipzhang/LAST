/**
 * @file LRDL1bEvent.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-02-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */


 #ifndef _LRDL1bEvent_HH
 #define _LRDL1bEvent_HH


#include "LDL1bTelEvent.hh"
#include "../LJsonConfig.hh"
#include "LDL1bEvent.hh"
#include "TDirectory.h"
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include <memory>

class LRDL1bEvent: public LDL1bEvent
{
    public:
        void InitROOTFile();

        LRDL1bEvent(const LJsonConfig&, const char mode = 'w');
        ~LRDL1bEvent();
        void StoreTTree();
        void ReadROOTFile(std::string filename);
        void HandleEvent();
        bool ReadEvent();
        const LDL1bArrayEvent& GetArrayEvent() const
        {
            return *ldl1barrayevent;
        }

    private:
        long nevents;
        long ievents = 0;
        int telescope_flag = 0;
        bool have_energy = false;
        bool have_hadroness = false;
        float Estimate_Energy;
        float Estimate_Hadroness;
        LRDL1bTelEvent* dl1_televent = nullptr;
        const LJsonConfig& cmd_config;
        const char* dirname = "dl1";
        TDirectory  *dir = nullptr;
        std::unique_ptr<TFile> root_file;
        TTree* dl1arraytree;
        TTree* dl1teltree;

};












 #endif
