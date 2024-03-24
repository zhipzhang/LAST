#include "LDL1Event.hh"
#include "LDL1TelEvent.hh"
#include <memory>


LDL1Event::LDL1Event(): LDataBase()
{
    ldl1event = std::make_shared<LTelescopes<std::shared_ptr<LRDL1TelEvent>>>();
    ldl1array = new LRArray();
    dl1_tel_event = new LRDL1TelEvent();
};


LDL1Event::~LDL1Event()
{
    delete dl1_tel_event;
    delete ldl1array;
};
void LDL1Event::SetTelHillas(int event_id, int tel_id, LHillasParameters hillas)
{
    auto itel_dl1event = std::make_shared<LRDL1TelEvent>();
    itel_dl1event->SetTelHillas(event_id, tel_id, hillas);
}

void LDL1Event::AddTelEvent(int tel_id, LRDL1TelEvent* dl1televent)
{
    auto itel_event = std::make_shared<LRDL1TelEvent>();
    *itel_event = *dl1televent;
    ldl1event->AddTel(tel_id, itel_event);
}