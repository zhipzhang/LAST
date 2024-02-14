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

 #include "Datalevels/LShower.hh"
#include "RtypesCore.h"
class LDL1bArrayEvent: public LRArray
{
    public:

    bool       Isvalid = false;
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

    ClassDef(LDL1bArrayEvent, 1)
};




 #endif