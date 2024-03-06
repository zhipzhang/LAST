#include "LTelTrueImage.hh"
#include "RtypesCore.h"
#include <algorithm>
#include <cstddef>
LTelTrueImage::LTelTrueImage()
{
    true_pe = nullptr;
    pe_time = nullptr;
    pe_intensity = nullptr;
    event_id = -1;
    tel_id = -1;
    num_pixels  = -1;
    tel_alt = tel_az = -99;
}


LTelTrueImage::~LTelTrueImage()
{
    if( true_pe != nullptr)
    {
        delete[] true_pe;
        true_pe = nullptr;
    }
    if(pe_time != nullptr)
    {
        delete[] pe_time;
        pe_time = nullptr;
    }
    if(pe_intensity != nullptr)
    {
        delete[] pe_intensity;
        pe_intensity = nullptr;
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
        npe      = other.npe;
        true_pe = new int[num_pixels];
        std::copy(other.true_pe, other.true_pe + num_pixels, true_pe);
        tel_alt = other.tel_alt;
        tel_az  = other.tel_az;
        tel_id = other.tel_id;
        return *this;
}

void LRTelTrueImage::Compute_Spread()
{
    if( npe > 0)
    {
        std::sort(pe_time, pe_time + npe);
    }
    size_t index10 = static_cast<size_t>(npe * 0.1);
    size_t index90 = static_cast<size_t>(npe * 0.9) - 1; // 减1以确保在数组范围内
    size_t index20 = static_cast<size_t>(npe * 0.2);
    size_t index80 = static_cast<size_t>(npe * 0.8) - 1; // 减1以确保在数组范围内

    // 确保索引在数组范围内
    index10 = std::max<size_t>(index10, 0);
    index90 = std::min<size_t>(index90, npe - 1);
    index20 = std::max<size_t>(index20, 0);
    index80 = std::min<size_t>(index80, npe - 1);

    time_spread1 = pe_time[index90] - pe_time[index10];
    time_spread2 = pe_time[index80] - pe_time[index20];
}