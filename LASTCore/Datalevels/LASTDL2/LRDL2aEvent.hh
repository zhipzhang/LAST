/**
 * @file LRDL2aEvent.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-04-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once


#include "../LASTDL1/LDL1bTelEvent.hh"
#include "../LASTDL1/LRDL1bEvent.hh"
#include "../LJsonConfig.hh"
#include "../LTelescopesTemplate.hh"
#include "LDL2aTelEvent.hh"
#include "LDL2aArrayEvent.hh"
#include "TDirectory.h"
#include <memory>
#include "TFile.h"
#include "TTree.h"


class LRDL2aEvent
{
    public:
        LRDL2aEvent(const LJsonConfig&, const char mode);
        ~LRDL2aEvent();
        void InitROOTFile();
        const LJsonConfig& cmd_config;
        void GetEvent(const LRDL1bEvent&);
        void HandleEvent();
        void StoreTTree();
       LRDL2aArrayEvent* ldl2aarrayevent;
       //Using LRDL1bTelEvent shared pointer to represent the LRDL2aTelEvent
       std::shared_ptr<LTelescopes<std::shared_ptr<LRDL2aTelEvent>>> ldl2aevent;
       std::string filename;
       const char* dirname = "dl2";
       TDirectory* dir     = nullptr;
       std::unique_ptr<TFile> root_file;
       TTree* dl2a_arraytree;
       TTree* dl2a_teltree;
       LRDL2aTelEvent* dl2a_televent = nullptr;


       
};
