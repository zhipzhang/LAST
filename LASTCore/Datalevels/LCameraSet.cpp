#include "LCameraSet.hh"
LCameraSet::LCameraSet()
{
    name = "";
    tel_id = 0;
    num_pixels = 0;
    pix_x = nullptr;
    pix_y = nullptr;
    pixel_size = 0;
    pixel_shape = 0;
}

LCameraSet::LCameraSet(std::string name, int tel_id, int num_pixels)
{
    this->name = name;
    this->tel_id = tel_id;
    this->num_pixels = num_pixels;
    pix_x = new Double32_t[num_pixels];
    pix_y = new Double32_t[num_pixels];
    pixel_size = 0;
    pixel_shape = 0;
}
LCameraSet::~LCameraSet()
{
    if( num_pixels > 0)
    {
        delete[] pix_x;
        delete[] pix_y;
    }
}