/**
 * @file LDL2aArrayEvent.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _LRDL2aArrayEvent_HH
#define _LRDL2aArrayEvent_HH

#include "../LASTDL1/LDL1bArrayEvent.hh"
#include "../LShower.hh"
#include <math.h>
#include <root/RtypesCore.h>
#include <cmath>

class LRDL2aArrayEvent: public LRShower
{
    public:
        std::vector<int> rec_tels;
        Double32_t Estimate_Energy;
        Double32_t Estimate_Hadroness;
        Double32_t Hillas_rec_alt;
        Double32_t Hillas_rec_az;
        Double32_t Hillas_rec_core_x;
        Double32_t Hillas_rec_core_y;
        Double32_t Hillas_rec_hmax;
        Double32_t Hillas_direction_error;
        Double32_t weight;
        int  ntel;
        LRDL2aArrayEvent()
        {
            Estimate_Energy = -1;
        }
        virtual ~LRDL2aArrayEvent()
        {
            Clear();
        }
        void SetEstimateEnergy(double energy)
        {
            Estimate_Energy = energy;
        }
        void SetHadroness(double hadroness)
        {
            Estimate_Hadroness = hadroness;
        }
        void Clear()
        {
            rec_tels.clear();
            Estimate_Energy = -1;
            Hillas_rec_alt = 0;
            Hillas_rec_az = 0;
            Hillas_rec_core_x = 0;
            Hillas_rec_core_y = 0;
            Hillas_rec_hmax = 0;
            Hillas_direction_error = 0;
        }
        void Setweight(std::vector<double> weight)
        {
            if(weight.size() == 0)
            {
                return;
            }
            if(energy < 4)
            {
                this->weight = weight[0] * pow(energy, -0.57);
            }
            else if(energy < 40)
            {
                this->weight = weight[1] * pow(energy, -0.57);
            }
            else
            {
                this->weight = weight[2] * pow(energy, -0.57);
            
            }
        }
    LRDL2aArrayEvent& operator= (const LDL1bArrayEvent& dl1_array_event) {
        Hillas_rec_alt = dl1_array_event.hillas_alt;
        Hillas_rec_az  = dl1_array_event.hillas_az;
        Hillas_rec_core_x = dl1_array_event.hillas_core_x;
        Hillas_rec_core_y = dl1_array_event.hillas_core_y;
        Hillas_rec_hmax = dl1_array_event.hillas_hmax;
        Hillas_direction_error = dl1_array_event.hillas_direction_error;
        rec_tels = dl1_array_event.reconstruction_tels;
        ntel     = rec_tels.size();
        energy = dl1_array_event.energy;
        altitude = dl1_array_event.altitude;
        azimuth = dl1_array_event.azimuth;
        core_x = dl1_array_event.core_x;
        core_y = dl1_array_event.core_y;
        h_first_int = dl1_array_event.h_first_int;
        x_max  = dl1_array_event.x_max;
        shower_primary_id = dl1_array_event.shower_primary_id;
        obs_id = dl1_array_event.obs_id;
        array_point_alt = dl1_array_event.array_point_alt;
        array_point_az = dl1_array_event.array_point_az;

    return *this;
}
    ClassDef(LRDL2aArrayEvent, 4)

};
#endif