#ifndef _LREventRaw_HH
#define _LREventRaw_HH


#include "../LJsonConfig.hh"
#include "LEventRaw.hh"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TObject.h"
#include <memory>
#include <string>
#include "LEventElectronic.hh"
#include "../LSimulationImage.hh"
#include "TFile.h"
#include "TDirectory.h"
class LREventRaw : public LEventRaw
{
    public:
        LREventRaw(const LJsonConfig& cmd_config, const char mode = 'w');

        LREventRaw(const LJsonConfig& cmd_config, std::string filename); // Read File When Analysis Constructor.
        virtual ~LREventRaw();

        void InitRootFile();   // Open the root file and create the directory and tree.

        bool HandleEvent();    // Fill the shower/true_image/waveform to the tree.
        void StoreTTree();    // Write all tree.
        void GetShower(AllHessData* hsdata) override;                // get the shower data from hsdta. fill the shower
        void ReadROOTFile(std::string filename);                     // Read the root file and get the tree.
        bool ReadEvent() override;                                             // Read the event from the tree.
        TFile* GetRootFile(){return rootfile.get();};
    private:
       const  LJsonConfig& cmd_config;
       std::unique_ptr<TFile> rootfile;
       std::string  output_fname;
       LRTelTrueImage* rtel_true_image; 
       LRTelElectronic* rtel_electronic;
       const char *dirname = "event";
       TDirectory* event_dir;
       TTree* true_image_tree;
       TTree* waveform_tree;
       TTree* arrayevent;
       int nevents;
       int ievents = 0;




};
#endif