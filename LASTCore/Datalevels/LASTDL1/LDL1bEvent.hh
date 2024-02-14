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
#include "Datalevels/LASTDL1/LDL1bTelEvent.hh"
#include "Datalevels/LTelescopesTemplate.hh"
#include "LDL1bArrayEvent.hh"
#include <memory>
class LDL1bEvent
{
    public:
        LDL1bEvent();
        ~LDL1bEvent();
        void SetRecDirection(double rec_az, double rec_alt, double rec_az_uncertainty, double rec_alt_uncertainty);
        void SetRecCore(double rec_core_x, double rec_core_y, double rec_tiled_x, double rec_tiled_y, double rec_tilted_uncertainty_x, double rec_tilted_uncertainty_y);  
    protected:
        std::shared_ptr<LTelescopes<std::shared_ptr<LRDL1bTelEvent>>> ldl1bevent;
        LRDL1bTelEvent* ldl1b_tel_event;
    
        std::shared_ptr<LDL1bArrayEvent> ldl1barrayevent;                  // Store Hillas Reconrtsuctor Results

};


#endif