#include "LSimulationImage.hh"
#include <memory>

void LSimulationImage::AddTelImage(int tel_id, LRTelTrueImage *tel_image)
{
    auto tel_image_ptr = std::make_shared<LRTelTrueImage>();
    *tel_image_ptr = *tel_image;
    telescopes_true_image->AddTel(tel_id, tel_image_ptr);
}
