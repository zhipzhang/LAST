/**
 * @file LRDL1Event.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-01-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */


 #ifndef _LRDL1_EVENT_HH_
 #define _LRDL1_EVENT_HH_
#include "Datalevels/LJsonConfig.hh"
#include "LDL1Event.hh"
 class LRDL1Event: public LDL1Event 
 {
        public:
            void InitRootFile();
            LRDL1Event(const LJsonConfig&, const char mode = 'w'); // Constructor for produce dl1 file 
            LRDL1Event(const LJsonConfig&);                      // Constructor for read dl1 file
            
            void HandleEvent();
            void StoreTTree();
            void CopyEvent();
            void ReadROOTFile(std::string filename);
            bool ReadEvent();    // Read Events from TTree.

            virtual ~LRDL1Event(){};
            const LJsonConfig& cmd_config;

        private:
            int nevents;
            int ievents = 0;
            LRDL1TelEvent* dl1_tel_event;
            bool copyshower = false;
            bool copyrunconfig = true;
            bool copyinstrument = true;
            std::string outname;
            const char* dl1_dirname = "dl1";
            TDirectory* dl1_dir = nullptr;
            std::unique_ptr<TFile> root_file;
            TTree* arrayevent;
            TTree* dl1event_tree;
 };








 #endif