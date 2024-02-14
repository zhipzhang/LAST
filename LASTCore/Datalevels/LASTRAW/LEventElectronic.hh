#ifndef  _LEVENTELECTRONIC_HH
#define  _LEVENTELECTRONIC_HH


#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"
#include "../LTelescopesTemplate.hh"
#include <cstdint>
#include <memory>
#include "glog/logging.h"
#include "LTelElectronic.hh"

class LEventElectronic
{
    public:
        LRTelElectronic* GetTelWaveform(int tel_id){return (*telescopes_electronic)[tel_id].get();};
        void AddTelWaveform(int tel_id, LRTelElectronic*);
        const std::vector<int> GetTelList() {return telescopes_electronic->GetKeys();};
        void Clear() {telescopes_electronic->Clear();}
        std::shared_ptr<LTelescopes<std::shared_ptr<LRTelElectronic>>>  GetData() {return telescopes_electronic;}
        LEventElectronic(){ telescopes_electronic = std::make_shared<LTelescopes<std::shared_ptr<LRTelElectronic>>>();};
        ~LEventElectronic(){LOG(INFO) << "delete LeventElectronic";}
        std::shared_ptr<LTelescopes<std::shared_ptr<LRTelElectronic>>> telescopes_electronic;
    
};

#endif