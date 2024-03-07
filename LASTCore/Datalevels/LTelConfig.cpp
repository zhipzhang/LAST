#include "LTelConfig.hh"
#include "LCameraSet.hh"
#include <algorithm>
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

LRTelescopeConfig& LRTelescopeConfig::operator=(const LRTelescopeConfig &other)
{
    std::copy(other.pos, other.pos + 3, this->pos);
    InitCameraSet(other.camera_name, other.tel_id, other.num_pixels);

    std::copy(other.pix_x, other.pix_x + other.num_pixels, this->pix_x);
    std::copy(other.pix_y, other.pix_y + other.num_pixels, this->pix_y);
    this->pix_shape = other.pix_shape;
    this->pix_size = other.pix_size;
    this->focal_length = other.focal_length;
    return *this;
}