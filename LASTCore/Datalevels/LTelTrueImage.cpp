#include "LTelTrueImage.hh"
#include "glog/logging.h"
LTelTrueImage::LTelTrueImage()
{
    true_pe = nullptr;
    event_id = -1;
    tel_id = -1;
    num_pixels  = -1;
    tel_alt = tel_az = -99;
}


LTelTrueImage::~LTelTrueImage()
{
    LOG(INFO) << "delete LTelTrueImage";
    if( true_pe != nullptr)
    {
        delete[] true_pe;
        true_pe = nullptr;
    }
}
LRTelTrueImage& LRTelTrueImage::operator=(const LRTelTrueImage& other)
{
        if(true_pe)
        {
            delete [] true_pe;
        }
        num_pixels = other.num_pixels;
        event_id = other.event_id;
        true_pe = new int[num_pixels];
        std::copy(other.true_pe, other.true_pe + num_pixels, true_pe);
        tel_alt = other.tel_alt;
        tel_az  = other.tel_az;
        tel_id = other.tel_id;
        return *this;
}