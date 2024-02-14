/**
 * @file LDataBase.hh
 * @author zhipzhang (zhipzhang@mail.ustc.edu.cn)
 * @brief  This class is the base class for all data levels. It contains the shower data, the run configuration and the telescope configuration.
 * @version 0.1
 * @date 2023-12-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */




// Contain Configuration/Instrument and Shower Data
#ifndef _LDATA_BASE_HH_
#define _LDATA_BASE_HH_
#include "LShower.hh"
#include "LSimulationConfig.hh"
#include "LTelescopesTemplate.hh"
#include "LTelConfig.hh"
#include "TDirectory.h"
#include "TTree.h"
#include "TFile.h"
#include <algorithm>
#include <cstddef>
#include <memory>

class LDataBase
{
    public:
        LDataBase();
        virtual ~LDataBase();
        void Read(TFile* f);               // Read the data from ROOT file
        std::shared_ptr<LTelescopes<std::shared_ptr<LRTelescopeConfig>>> GetTelescopesConfig() const {return tel_config;};
    protected:
        std::unique_ptr<LRShower> shower;    // shower data
        LRSimulationRunConfig* run_config; // run configuration
        std::shared_ptr<LTelescopes<std::shared_ptr<LRTelescopeConfig> >> tel_config; // telescopes configuration

        void CopyShower(TFile* f);         //Copy the shower data from ROOT to ROOT
        void CopyRunConfig(TFile* f);      //Copy the run configuration from ROOT to ROOT
        void CopyTelConfig(TFile* f);      //Copy the telescope configuration from ROOT to ROOT
        virtual void ReadRunConfig(){};      // For Raw Data, it read from eventio file, else it read from ROOT file 
        virtual void ReadTelConfig(){};      // For Raw Data, it read from eventio file, else it read from ROOT file
        void WriteConfig(TFile* f);        // From EventIo to ROOT file
        void WriteShower(TFile* f);        // From EventIo to ROOT file
        TTree* shower_tree = nullptr;
        TTree* runconfig_tree = nullptr;
        TTree* telconfig_tree = nullptr;
        TDirectory* simulation_dir = nullptr;
        TDirectory* simulation_shower_dir = nullptr;     // When Write, it's directory what we write to; when Read, it's directory what we read from
        TDirectory* simulation_config_dir = nullptr;     // When Write, it's directory what we write to; when Read, it's directory what we read from
        TDirectory* instrument_dir = nullptr;            // When Write, it's directory what we write to; when Read, it's directory what we read from

    private:
        void CopyDirectory(TDirectory* source, TDirectory* dest);
        void FillTelConfig();              // It should be called after Read(TFile*)
        LRTelescopeConfig* itel_config ;
        LRShower* ishower;
        const char* simulation_config_dirname = "config";
        const char* simulation_shower_dirname = "shower";
        const char* instrument_dirname = "instrument";

};

 #endif