#include "LDL1bEvent.hh"



LDL1bEvent::LDL1bEvent()
{
    ldl1bevent = std::make_shared<LTelescopes<std::shared_ptr<LRDL1bTelEvent>>>();
    ldl1barrayevent = new LDL1bArrayEvent();
}

LDL1bEvent::~LDL1bEvent()
{
    delete ldl1barrayevent;
}

void LDL1bEvent::SetRecCore(double rec_core_x, double rec_core_y, double rec_tiled_x, double rec_tiled_y, double rec_tilted_uncertainty_x, double rec_tilted_uncertainty_y)
{
    ldl1barrayevent->hillas_core_x = rec_core_x;
    ldl1barrayevent->hillas_core_y = rec_core_y;
    ldl1barrayevent->hillas_tiled_x = rec_tiled_x;
    ldl1barrayevent->hillas_tiled_y = rec_tiled_y;
    ldl1barrayevent->hillas_tilted_uncertainty_x = rec_tilted_uncertainty_x;
    ldl1barrayevent->hillas_tilted_uncertainty_y = rec_tilted_uncertainty_y;
}
void LDL1bEvent::SetRecDirection(double rec_az, double rec_alt, double rec_az_uncertainty, double rec_alt_uncertainty)
{
    ldl1barrayevent->Isvalid = true;
    ldl1barrayevent->hillas_alt = rec_alt;
    ldl1barrayevent->hillas_az = rec_az;
    ldl1barrayevent->hillas_alt_uncertainty = rec_alt_uncertainty;
    ldl1barrayevent->hillas_az_uncertainty = rec_az_uncertainty;
}
