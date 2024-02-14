#include "LTelConfig.hh"
#include "LCameraSet.hh"
#include <memory>


void LRTelescopeConfig::InitCameraSet(std::string name, int tel_id, int num_pixels)
{
    this->tel_id = tel_id;
    this->camera_name = name;
    this->num_pixels = num_pixels;
    this->pix_x = new double[num_pixels];
    this->pix_y = new double[num_pixels];

}

LRTelescopeConfig::~LRTelescopeConfig()
{
    if(pix_x)
    {
        delete [] pix_x;
    }
    if(pix_y)
    {
        delete [] pix_y;
    }
}