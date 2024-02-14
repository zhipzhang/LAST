

#ifndef _LDL1Event_HH
#define _LDL1Event_HH
#include "../LDataBase.hh"
#include <memory>
#include "../LTelescopesTemplate.hh"
#include "../LShower.hh"
#include "Image/LHillasParameters.hh"
#include "LDL1TelEvent.hh"

class LDL1Event : public LDataBase
{
    public:
        LDL1Event();
        ~LDL1Event();
        void SetTelHillas(int event_id, int tel_id, LHillasParameters hillas);
        void AddTelEvent(int tel_id, std::shared_ptr<LRDL1TelEvent> dl1televent) { ldl1array->AddTel(tel_id);ldl1event->AddTel(tel_id, dl1televent);};
        void AddTelEvent(int tel_id, LRDL1TelEvent*);
        void Clear(){ldl1event->Clear(); ldl1array->Clear();};
        LRDL1TelEvent& operator[](int tel_id) {return *((*ldl1event)[tel_id]);};
        const LRDL1TelEvent& operator[](int tel_id) const {return *((*ldl1event)[tel_id]);};
        std::vector<int> GetTelList() const {return ldl1array->GetTelList();};
        double GetPointingAz() const {return ldl1array->array_point_az;};
        double GetPointingAlt() const {return ldl1array->array_point_alt;};
    protected:
        std::shared_ptr<LTelescopes<std::shared_ptr<LRDL1TelEvent>>> ldl1event;
        LRDL1TelEvent* dl1_tel_event;
        
        LRArray* ldl1array;
        std::string output_fname;

    

};







#endif