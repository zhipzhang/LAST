#include "LEventElectronic.hh"
#include <memory>


void LEventElectronic::AddTelWaveform(int tel_id, LRTelElectronic* rtel_electronic)
{
    auto tel_electronic = std::make_shared<LRTelElectronic>();
    *tel_electronic = *rtel_electronic;
    telescopes_electronic->AddTel(tel_id, tel_electronic);
}