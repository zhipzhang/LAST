/**
 * @file LRDL1bTelEvent.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief Class for TMVA/pytorch model to reconstruct the energy and particle type of the event
 * @version 0.1
 * @date 2024-02-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _LRDL1bEvent_HH
#define _LRDL1bEvent_HH 


#include "LDL1TelEvent.hh"
#include "RtypesCore.h"

class LRDL1bTelEvent: public LRDL1TelEvent
{
    Double32_t True_Energy;
    Double32_t True_Alt;
    Double32_t True_Az;
    Double32_t True_TelImpactParameter;
    Double32_t Hillas_TelImpactParameter;
    Double32_t Hillas_AverageIntensity;
    Double32_t Hillas_h_max;
    int        n_triggered_tel;
    int        n_hiias_tel;
    public:
        LRDL1bTelEvent();
        void CopyTelInfo(const LRDL1TelEvent& dl1_tel_event)
        {
            this->tel_az  = dl1_tel_event.tel_az;
            this->tel_alt = dl1_tel_event.tel_alt;
            CopyLeakage(dl1_tel_event);
            CopyConcentration(dl1_tel_event);
            CopyIntensity(dl1_tel_event);
            CopyMorphology(dl1_tel_event);
        }
        void SetShowerInfo();
};





#endif