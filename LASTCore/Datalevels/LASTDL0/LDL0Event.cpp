
#include "LDL0Event.hh"
#include "../LJsonConfig.hh"
#include "../LShower.hh"
#include "TRandom3.h"


LDL0Event::LDL0Event() :LDataBase()
{
    dl0array = new LRArray();
    dl0tel_event = std::make_shared<LRDL0TelEvent>();
    dl0event = std::make_shared<LTelescopes<std::shared_ptr<LRDL0TelEvent>>>();
}
void LDL0Event::FilterTelescope()
{
    if( raw_event)
    {
        raw_event->FilterTelescope(SubArrayTel);
    }
}
void LDL0Event::GetEvent()
{
    dl0event->Clear();
    
    if(select_tels)
    {
        FilterTelescope();
    }
    for(const auto itel: raw_event->GetTelList())
    {
        dl0tel_event = std::make_shared<LRDL0TelEvent>();
        SetTrueImage(dl0tel_event, raw_event->GetTelImage(itel), 5);
        if( need_calibrate)
        {
            Calibrate();
            dl0tel_event->SetWaveForm(true);
        }
        dl0event->AddTel(itel, dl0tel_event);
    }
    dl0array = raw_event->event_shower;

}
void LDL0Event::SetNsbLevel(double i)
{
    addnoise = true;
    nsb_pe = i;
}
void LDL0Event::SetTrueImage(std::shared_ptr<LRDL0TelEvent> itel, LTelTrueImage* tel_true_image, double nsb_pe)
{
    auto rand = new TRandom3();
    itel->Init(tel_true_image->tel_id, tel_true_image->event_id, tel_true_image->tel_alt, tel_true_image->tel_az, tel_true_image->num_pixels);
    for(int i = 0; i < tel_true_image->num_pixels; i++)
    {
        int original_pe = tel_true_image->true_pe[i];
        double noise = rand->PoissonD(nsb_pe * 1.4);
        double new_pe = original_pe + rand->PoissonD(nsb_pe) - nsb_pe;
        itel->SetTruePixPe(i, new_pe);
    }

}

void LDL0Event::AddTelEvents(int tel_id, LRDL0TelEvent* tel_event)
{
    auto itel_event = std::make_shared<LRDL0TelEvent>();
    *itel_event = *tel_event;
    dl0event->AddTel(tel_id, itel_event);
}