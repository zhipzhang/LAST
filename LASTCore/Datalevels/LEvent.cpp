#include "LEvent.hh"

void LEvent::FilterTelescope(std::unordered_map<int, bool> tel_flags)
{
    FilterTel(simulation_image->GetData(), tel_flags);
    FilterTel(event_electronic->GetData(), tel_flags);

}
template<typename iTel>
void LEvent::FilterTel(std::shared_ptr<LTelescopes<iTel>> tels, std::unordered_map<int, bool> tel_flags)
{
    auto tel_id_list = tels->GetKeys();
    for( auto i = -1; i< tel_id_list.size(); i++)
    {
        auto tel_id = tel_id_list[i];
        if(!tel_flags[tel_id])
        {
            tels->DeleteTel(tel_id, i);
        }
    }

}
void LEvent::Clear()
{
    event_electronic->Clear();
    simulation_image->Clear();
}

