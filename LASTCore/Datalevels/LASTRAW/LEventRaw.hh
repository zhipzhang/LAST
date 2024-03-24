


#ifndef _LEVENTRAW_HH
#define _LEVENTRAW_HH

#include "../LDataBase.hh"
#include "../LSimulationConfig.hh"
#include "SimTel_File.hh"
#include <cstdlib>
#include <memory>
#include "io_hess.h"
#include "../LShower.hh"
#include <string>
#include <unordered_map>
#include "../LSimulationImage.hh"
#include "../LDataBase.hh"
#include "../LEventElectronic.hh"
#include "../LJsonConfig.hh"
#include "../LEvent.hh"
/**
 * @brief Class to Read Raw Data from the EVENTIO File
 * 
 */

class LCalibEvent;
class LEventRaw : public LDataBase{
    public:
        LEventRaw();
        virtual ~LEventRaw(){};
        void ReadRunConfig() override {GetRunConfig(simtel_file->GetHsdata());};    // Read Simulation Configuration From EventIO File
        void ReadTelConfig() override {GetConfig(simtel_file->GetHsdata());}; // Read Telescope Configuration From EventIO File
        bool ProcessEvent();              // simtel_file to read the eventio file.
        void GetEvent();                  // Get the event from the hsdata. (including the shower data/true_image/waveform)
        virtual bool ReadEvent();     // Read the event from the tree.
        std::shared_ptr<LEvent> EventAddress(){return event;};
        bool HaveWaveform = false;
        void Close();
    protected:
        LAST_IO::SimTelIO* simtel_file;  // Main Class to open the eventio File;

        std::shared_ptr<LEvent>  event;   // Main event class including the true image and the waveform.
        //std::shared_ptr<LCalibEvent> calib_event; // Not added now.

        virtual void InitObject();
        virtual void GetShower(AllHessData*);                // get the shower data from hsdta. fill the shower 
        void GetConfig(AllHessData*);                // get the telescope configuration from hsdata. fill the tel_config
        void GetRunConfig(AllHessData*);
        std::string input_fname;
        void GetEventWaveform(AllHessData* hsdata);
        void GetTrueImage(AllHessData* hsdata);
        void GetEventShower(AllHessData* hsdata);

};
#endif