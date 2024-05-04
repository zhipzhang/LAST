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

#ifndef _LRDL1bTelEvent_HH
#define _LRDL1bTelEvent_HH 

#include "../LShower.hh"
#include "LDL1TelEvent.hh"
#include "RtypesCore.h"
#include <root/RtypesCore.h>

class LRDL1bTelEvent: public LRDL1TelEvent
{
    public:
    Double32_t True_Energy;
    Double32_t True_Alt;
    Double32_t True_Az;
    Double32_t True_TelImpactParameter;
    Double32_t Hillas_TelImpactParameter;
    Double32_t Hillas_AverageIntensity;
    Double32_t Hillas_h_max;
    Double32_t Hillas_Camerax;
    Double32_t Hillas_Cameray;
    Double32_t Hillas_Camerar;
    int        n_triggered_tel;
    int        n_hiias_tel;
        LRDL1bTelEvent(): LRDL1TelEvent()
        {
            True_Energy = 0;
            True_Alt = 0;
            True_Az = 0;
            True_TelImpactParameter = 0;
            Hillas_TelImpactParameter = 0;
            Hillas_AverageIntensity = 0;
            Hillas_h_max = 0;
            n_triggered_tel = 0;
            n_hiias_tel = 0;
            Hillas_Camerax = 0;
            Hillas_Cameray = 0;
            Hillas_Camerar = 0;
        }
        virtual ~LRDL1bTelEvent(){};
        void CopyTelInfo(const LRDL1TelEvent& dl1_tel_event)
        {
            this->tel_az  = dl1_tel_event.tel_az;
            this->tel_alt = dl1_tel_event.tel_alt;
            CopyLeakage(dl1_tel_event);
            CopyConcentration(dl1_tel_event);
            CopyIntensity(dl1_tel_event);
            CopyMorphology(dl1_tel_event);
        }
        void SetShowerInfo(const LShower& shower)
        {
            True_Energy = shower.energy;
            True_Alt = shower.altitude;
            True_Az  = shower.azimuth;
        }
        void SetImpactParameters(double true_impact, double hillas_impact)
        {
            True_TelImpactParameter = true_impact;
            Hillas_TelImpactParameter = hillas_impact;
        }
        void SetShowerInfo(const LShower& shower, int n_triggered_tel, int n_hiias_tel)
        {
            True_Energy = shower.energy;
            True_Alt = shower.altitude;
            True_Az  = shower.azimuth;
            this->n_triggered_tel = n_triggered_tel;
            this->n_hiias_tel = n_hiias_tel;
        }
        void SetCameraPos(double rec_x, double rec_y)
        {
            Hillas_Camerax = rec_x;
            Hillas_Cameray = rec_y;
            Hillas_Camerar = atan(sqrt(rec_x*rec_x + rec_y*rec_y));
        }
        
        LRDL1bTelEvent& operator= (const LRDL1bTelEvent& dl1_tel_event)
        {
            this->tel_id = dl1_tel_event.tel_id;
            this->event_id = dl1_tel_event.event_id;
            this->tel_az  = dl1_tel_event.tel_az;
            this->tel_alt = dl1_tel_event.tel_alt;
            CopyLeakage(dl1_tel_event);
            CopyConcentration(dl1_tel_event);
            CopyIntensity(dl1_tel_event);
            CopyMorphology(dl1_tel_event);
            hillas = dl1_tel_event.hillas;
            True_Energy = dl1_tel_event.True_Energy;
            True_Alt = dl1_tel_event.True_Alt;
            True_Az = dl1_tel_event.True_Az;
            True_TelImpactParameter = dl1_tel_event.True_TelImpactParameter;
            Hillas_TelImpactParameter = dl1_tel_event.Hillas_TelImpactParameter;
            Hillas_AverageIntensity = dl1_tel_event.Hillas_AverageIntensity;
            Hillas_h_max = dl1_tel_event.Hillas_h_max;
            n_triggered_tel = dl1_tel_event.n_triggered_tel;
            n_hiias_tel = dl1_tel_event.n_hiias_tel;
            return *this;
        }
    ClassDef(LRDL1bTelEvent, 1)
};





#endif