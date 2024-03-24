/**
 * @file LDL0Event.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief  LAST Data Level 0.
 * @version 0.1
 * @date 2024-01-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _LDL0_EVENT_HH_
#define _LDL0_EVENT_HH_

/*
2024.01.07 : Now, DL0 event should at least can implement the only_tel functions.
*/

#include "../LDataBase.hh"
#include <memory>
#include <unordered_map>
#include "../LTelescopesTemplate.hh"
#include "../LSimulationImage.hh"
#include "../LShower.hh"
#include "LDL0TelEvent.hh"
#include "../LEvent.hh"
class LDL0Event  : public LDataBase
{
    protected:
        LRArray* dl0array;
        std::shared_ptr<LTelescopes<std::shared_ptr<LRDL0TelEvent>>> dl0event;
        bool need_calibrate = false;
        bool select_tels = false;
        std::shared_ptr<LRDL0TelEvent> dl0tel_event;
        void Calibrate(){};
        bool addnoise = true;
        double nsb_pe = 0;
        void AddTelEvents(int tel_id, LRDL0TelEvent* tel_event);   // This should be wrapped in the dl0event 
        std::string outname;
    public:
        void SetEvent(std::shared_ptr<LEvent> event) {raw_event = event;};
        LDL0Event();
        virtual ~LDL0Event() {};
        std::unordered_map<int, bool>SubArrayTel;
        std::shared_ptr<LEvent> raw_event;
        void GetEvent();
        void FilterTelescope();
        void SetNsbLevel(double i);
        static void SetTrueImage(std::shared_ptr<LRDL0TelEvent>, LTelTrueImage*, double);
        std::vector<int> GetTelList() const {return dl0array->GetTelList();};
        const LRDL0TelEvent& GetTelEvent(int tel_id) const {return *((*dl0event)[tel_id]);};
        const LRDL0TelEvent& operator[]  (int tel_id) const {return *((*dl0event)[tel_id]);};
        LShower& GetShowerInfo() {return *dl0array;};
        const LShower& GetShowerInfo() const {return *dl0array;};
};

class Ldl0Event
{
    public:
        LRArray* event_shower;
        const std::vector<int> GetTelList() {return event->GetKeys();};
    private:
        std::shared_ptr<LTelescopes<std::shared_ptr<LRDL0TelEvent>>> event;
};






#endif