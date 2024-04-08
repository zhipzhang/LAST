#include "LDL1Event.hh"
#include "LDL1TelEvent.hh"
#include <memory>
#include <unordered_map>


LDL1Event::LDL1Event(): LDataBase()
{
    ldl1event = std::make_shared<LTelescopes<std::shared_ptr<LRDL1TelEvent>>>();
    ldl1array = new LRArray();
    dl1_tel_event = new LRDL1TelEvent();
};


LDL1Event::~LDL1Event()
{
    /*
    delete dl1_tel_event;
    delete ldl1array;
    */
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
void LDL1Event::FilterTelescope(const std::vector<int> tels)
{
  std::unordered_map<int, bool> telescope_list;
  for(auto itel: tels)
  {
    telescope_list[itel] = true;
  }
  for(int i = ldl1event->GetTelNum() - 1; i >= 0 ; i--)
  {
    int tel_id = ldl1event->GetKeys()[i];
    if(!telescope_list[tel_id])
    {
        ldl1event->DeleteTel(tel_id, i);
        ldl1array->DeleteTel(i);
    }
  }
}
void LDL1Event::Close()
{
    Clear();
    LDataBase::Close();
}