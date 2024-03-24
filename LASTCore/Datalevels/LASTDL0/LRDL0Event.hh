/**
 * @file LRDL0Event.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-01-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

 #ifndef _LRDL0_EVENT_HH_
 #define _LRDL0_EVENT_HH_
 #include "LDL0Event.hh"
 #include <memory>
 #include <string>
 #include "TDirectory.h"
 #include "TFile.h"
 #include "../LJsonConfig.hh"
 #include "TCanvas.h"
 #include "TH2Poly.h"

 class LRDL0Event: public LDL0Event
 {
        public:
            void InitRootFile();
            LRDL0Event(const LJsonConfig& config, const char mode = 'r');
            void HandleEvent();
            void StoreTTree();
            void CopyEvent();
            void ReadROOTFile(std::string filename);
            bool ReadEvent();    // Read Events from TTree.
            TFile* GetRootFile() {return root_file.get();};
            virtual ~LRDL0Event(){};
            void DrawEvent(int ievent, int itel);
        private:
            int nevents;
            const LJsonConfig& cmd_config;
            int ievents = 0;
            LRDL0TelEvent* dl0_tel_event = nullptr;
            bool copyshower = false;
            bool copyrunconfig = true;
            bool copyinstrument = true;
            const char* dl0_dirname = "dl0";
            TDirectory* dl0_dir = nullptr;
            std::unique_ptr<TFile> root_file;
            TTree* arrayevent;
            TTree* dl0event_tree;
            
            
 };










 #endif