

// It will store the true image of photonelectrons in the camera.
#ifndef _LSimulationImage_hh_
#define _LSimulationImage_hh_


#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"
#include "LTelescopesTemplate.hh"
#include <cstdint>
#include <memory>
#include "LTelTrueImage.hh"

class LSimulationImage
{
    public:
        LSimulationImage(){ telescopes_true_image = std::make_shared<LTelescopes<std::shared_ptr<LRTelTrueImage>>>();};
        ~LSimulationImage(){};
        void Clear(){telescopes_true_image->Clear();};
        std::shared_ptr<LTelescopes<std::shared_ptr<LRTelTrueImage>>> GetData() {return telescopes_true_image;}
        const std::vector<int> GetTelList(){return telescopes_true_image->GetKeys();}
        void AddTelImage(int tel_id, const LRTelTrueImage& tel_image);
        LRTelTrueImage* GetTelImage(int tel_id){return ((*telescopes_true_image)[tel_id]).get();}
        //std::shared_ptr<LTelescopes<std::shared_ptr<LRTelTrueImage>>> telescopes_true_image;
        std::shared_ptr<LTelescopes<std::shared_ptr<LRTelTrueImage>>> telescopes_true_image;

};

#endif