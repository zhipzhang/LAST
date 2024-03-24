/**
 * @file LEvent.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief Event Class for Handling hsdata raw file
 * @version 0.1
 * @date 2024-03-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _LEVENT_HH
#define _LEVENT_HH
#include <vector>
#include <memory>
#include "LShower.hh"

#include "LSimulationImage.hh"
#include "LEventElectronic.hh"
class LEvent
{
    public:
    //friend class LREventRaw;
        const std::vector<int> GetImageTelList() {return simulation_image->GetTelList();};
        const std::vector<int> GetWaveformTelList() {return event_electronic->GetTelList();};
        LRTelTrueImage* GetTelImage(int tel_id){return simulation_image->GetTelImage(tel_id);};
        LRTelElectronic* GetTelWaveform(int tel_id){return event_electronic->GetTelWaveform(tel_id);};
        void AddTelImage(int tel_id, const LRTelTrueImage& rtel_image){simulation_image->AddTelImage(tel_id, rtel_image);};
        void AddTelWaveform(int tel_id, const LRTelElectronic& rtel_electronic){event_electronic->AddTelWaveform(tel_id, rtel_electronic);};
        const std::vector<int> GetTelList() {return event_shower->GetTelList();};
        LShower* GetShowerAddress(){return event_shower;};
        LEvent(){
            simulation_image = std::make_shared<LSimulationImage>();
            event_electronic = std::make_shared<LEventElectronic>();
            event_shower = new LRArray();
            };
        ~LEvent(){ delete event_shower;}    
        LRArray* event_shower;
        void FilterTelescope(std::unordered_map<int, bool>);
        template<typename iTel>
        void FilterTel(std::shared_ptr<LTelescopes<iTel>>, std::unordered_map<int, bool>);
        void Clear();
        void ClearWaveform(){event_electronic->Clear();};
        void ClearImage(){simulation_image->Clear();};
        void ClearShower(){event_shower->Clear();};
    private:
        std::shared_ptr<LSimulationImage> simulation_image;       // Will store true Images/true pes.
        std::shared_ptr<LEventElectronic> event_electronic;           // Will store the waveform.


};










#endif