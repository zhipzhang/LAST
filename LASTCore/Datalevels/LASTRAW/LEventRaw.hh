


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
#include "LEventElectronic.hh"
#include "../LJsonConfig.hh"
/**
 * @brief Class to Read Raw Data from the EVENTIO File
 * 
 */

class LEvent;
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
        void GetTrueImage( AllHessData* hsdata);
        void GetEventWaveform( AllHessData* hsdata);
        

};
class LEvent
{
    public:
    //friend class LREventRaw;
        void GetTrueImage( AllHessData* hsdata);
        void GetEventWaveform( AllHessData* hsdata);
        const std::vector<int> GetImageTelList() {return simulation_image->GetTelList();};
        const std::vector<int> GetWaveformTelList() {return event_electronic->GetTelList();};
        LRTelTrueImage* GetTelImage(int tel_id){return simulation_image->GetTelImage(tel_id);};
        LRTelElectronic* GetTelWaveform(int tel_id){return event_electronic->GetTelWaveform(tel_id);};
        void AddTelImage(int tel_id, LRTelTrueImage* rtel_image){simulation_image->AddTelImage(tel_id, rtel_image);};
        void AddTelImage(int tel_id, std::shared_ptr<LRTelTrueImage> t){simulation_image->AddTelImage(tel_id, t);};
        void AddTelWaveform(int tel_id, LRTelElectronic* rtel_electronic){event_electronic->AddTelWaveform(tel_id, rtel_electronic);};
        const std::vector<int> GetTelList() {return event_shower->GetTelList();};
        LShower* GetShowerAddress(){return event_shower;};
        LEvent(){
            simulation_image = std::make_shared<LSimulationImage>();
            event_electronic = std::make_shared<LEventElectronic>();
            event_shower = new LRArray();
            };
        void GetShower(AllHessData* hsdata);                // get the shower data from hsdta.
        ~LEvent(){ delete event_shower;}    
        LRArray* event_shower;
        void FilterTelescope(std::unordered_map<int, bool>);
        template<typename iTel>
        void FilterTel(std::shared_ptr<LTelescopes<iTel>>, std::unordered_map<int, bool>);
        void Clear();
    private:
        std::shared_ptr<LSimulationImage> simulation_image;       // Will store true Images/true pes.
        std::shared_ptr<LEventElectronic> event_electronic;           // Will store the waveform.


};








#endif