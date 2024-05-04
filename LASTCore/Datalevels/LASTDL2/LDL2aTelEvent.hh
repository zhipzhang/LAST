/**
 * @file LDL2aTelEvent.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief This is used in pyLAST 
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "../LASTDL1/LDL1bTelEvent.hh"
#include <root/RtypesCore.h>


class LRDL2aTelEvent: public LRDL1bTelEvent
{
    public:
        Double32_t Estimate_Energy;
        Double32_t Estimate_Hadroness;

    LRDL2aTelEvent()
    {
        Estimate_Energy = -1;
    };
    virtual ~LRDL2aTelEvent(){};
        void SetEstimateEnergy(double energy)
        {
            Estimate_Energy = energy;
        }
        void SetEstimateHadroness(double hadroness)
        {
            Estimate_Hadroness = hadroness;
        }
    LRDL2aTelEvent& operator= (const LRDL1bTelEvent& dl1_tel_event) {
    LRDL1bTelEvent::operator=(dl1_tel_event);
    return *this;
    }
    LRDL2aTelEvent& operator= (const LRDL2aTelEvent& dl2_tel_event) {
        LRDL1bTelEvent::operator=(dl2_tel_event);
        Estimate_Energy = dl2_tel_event.Estimate_Energy;
        Estimate_Energy = dl2_tel_event.Estimate_Hadroness;
        return *this;
    }
    ClassDef(LRDL2aTelEvent, 1)
};