/**
 * @file LDL1bArrayEvent.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-02-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

 #ifndef _LDL1bArrayEvent_HH
 #define _LDL1bArrayEvent_HH

 #include "../LShower.hh"
#include "RtypesCore.h"
class LDL1bArrayEvent: public LRArray
{
    public:

    bool       Isvalid = false;
    std::vector<int> reconstruction_tels;
    Double32_t hillas_alt;
    Double32_t hillas_az;
    Double32_t hillas_alt_uncertainty;
    Double32_t hillas_az_uncertainty;
    Double32_t hillas_core_x;
    Double32_t hillas_core_y;
    Double32_t hillas_tiled_x;
    Double32_t hillas_tiled_y;
    Double32_t hillas_tilted_uncertainty_x;
    Double32_t hillas_tilted_uncertainty_y;
    Double32_t avearge_intensity;
    Double32_t hillas_hmax;
    Double32_t hillas_hmax_uncertainty;
    Double32_t hillas_direction_error;
    int GetRecNums() const
    {
        return reconstruction_tels.size();
    }
    void Reset()
    {
        Isvalid = false;
        reconstruction_tels.clear();
        hillas_alt = hillas_az = hillas_alt_uncertainty = hillas_az_uncertainty = 0;
        hillas_tilted_uncertainty_x = hillas_tilted_uncertainty_y =  hillas_tiled_x = hillas_tiled_y =0;
        hillas_core_x = hillas_core_y = 0;
        avearge_intensity = hillas_hmax_uncertainty = hillas_hmax = hillas_direction_error = 0;
    }
    void SetRecTels(std::vector<int> rec_tels)
    {
        reconstruction_tels = rec_tels;
    }
    ClassDef(LDL1bArrayEvent, 2)
};




 #endif
