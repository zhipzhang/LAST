/**
 * @file LDL1bEvent.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief LDL1bEvent: 1) 
 * @version 0.1
 * @date 2024-02-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _LDL1bEvent_HH
#define _LDL1bEvent_HH
#include "LDL1bTelEvent.hh"
#include "../LShower.hh"
#include "../LTelescopesTemplate.hh"
#include "LDL1bArrayEvent.hh"
#include <memory>
#include <root/TMath.h>
class LDL1bEvent
{
    public:
        LDL1bEvent();
        ~LDL1bEvent();
        void SetRecDirection(double rec_az, double rec_alt, double rec_az_uncertainty, double rec_alt_uncertainty);
        void SetRecCore(double rec_core_x, double rec_core_y, double rec_tiled_x, double rec_tiled_y, double rec_tilted_uncertainty_x, double rec_tilted_uncertainty_y);  
        double GetRecAlt()
        {
            return ldl1barrayevent->hillas_alt;
        }
        double GetRecAz()
        {
            return ldl1barrayevent->hillas_az;
        }
        double GetRecCoreX()
        {
            return ldl1barrayevent->hillas_core_x;
        }
        double GetRecCoreY()
        {
            return ldl1barrayevent->hillas_core_y;
        }
        void AddTelEvent(int tel_id, const LRDL1bTelEvent& dl1btelevent) 
        {
            auto televent = std::make_shared<LRDL1bTelEvent>();
            *televent = dl1btelevent;
            ldl1bevent->AddTel(tel_id, televent);
        }
        void Clear()
        {
            ldl1bevent->Clear();
            ldl1barrayevent->Reset();
        }
        void SetRecTels(std::vector<int> rec_tels)
        {
            ldl1barrayevent->SetRecTels(rec_tels);
        }
        void SetDirectionError()
        {
            double direction_error = angle_between(ldl1barrayevent->azimuth, ldl1barrayevent->altitude,ldl1barrayevent->hillas_az, ldl1barrayevent->hillas_alt);
            ldl1barrayevent->hillas_direction_error = direction_error * TMath::RadToDeg();
            for(auto itel: ldl1bevent->GetKeys())
            {
                (*ldl1bevent)[itel]->ComputeMiss(ldl1barrayevent->azimuth, ldl1barrayevent->altitude);
            }
        }
        LRArray& GetEventArrayInfo() {return *ldl1barrayevent;};
        const LRDL1bTelEvent& GetTelEvent(int i) const
        {
            int tel_id = ldl1barrayevent->reconstruction_tels[i];
            return *(*ldl1bevent)[tel_id];
        }
        int GetRecTelID(int i) const
        {
            return ldl1barrayevent->reconstruction_tels[i];
        }
        LRDL1bTelEvent& operator[](int tel_id)
        {
            return *(*ldl1bevent)[tel_id];
        }
        
    protected:
        std::shared_ptr<LTelescopes<std::shared_ptr<LRDL1bTelEvent>>> ldl1bevent;
        LDL1bArrayEvent* ldl1barrayevent;                  // Store Hillas Reconrtsuctor Results
        inline double  angle_between(double azimuth1, double altitude1, double azimuth2, double altitude2)
    {
        double ax1 = cos(azimuth1)*cos(altitude1);
        double ay1 = sin(-azimuth1)*cos(altitude1);
        double az1 = sin(altitude1);
        double ax2 = cos(azimuth2)*cos(altitude2);
        double ay2 = sin(-azimuth2)*cos(altitude2);
        double az2 = sin(altitude2);
        double cos_ang = ax1*ax2 + ay1*ay2 + az1*az2;
        /* Check for rounding errors pushing us outside the valid range. */
        if ( cos_ang <= -1. )
            return M_PI;
        else if ( cos_ang >= 1. )
            return 0.;
        else
            return acos(cos_ang);
    }

};


#endif