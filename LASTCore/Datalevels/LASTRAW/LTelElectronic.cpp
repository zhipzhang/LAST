#include "LTelElectronic.hh"
#include <algorithm>

LTelElectronic::LTelElectronic()
{
    adc_sample = nullptr;
    adc_sum = nullptr;
    adc_known = nullptr;
    tel_id = -1;
    num_gains = -1;
    num_pixels = -1;
    num_samples = -1;
    tel_alt = tel_az = -99;
}
LTelElectronic::~LTelElectronic()
{
    LOG(INFO) << "delete LTelElectronic";
    if( adc_sample)
    {
        delete[] adc_sample;
    }
    if( adc_sum)
    {
        delete[] adc_sum;
    }
    if( adc_known)
    {
        delete[] adc_known;
    }
}

LRTelElectronic& LRTelElectronic::operator=(const LRTelElectronic &other)
{
    // Check for self-assignment
    if (this == &other)
    {
        return *this;
    }

    if( adc_sample)
    {
        delete[] adc_sample;
    }
    if( adc_sum)
    {
        delete[] adc_sum;
    }
    if( adc_known)
    {
        delete[] adc_known;
    }


    // Perform the copy assignment
    // TODO: Add code to copy member variables from 'other' to 'this'
    num_pixels = other.num_pixels;
    event_id = other.event_id;
    tel_alt =   other.tel_alt;
    tel_az  =  other.tel_az;
    tel_id  = other.tel_id;
    num_gains = other.num_gains;
    num_samples = other.num_samples;
    Allocate();
    std::copy(other.adc_sample, other.adc_sample + num_gains * num_pixels * num_samples, adc_sample);
    std::copy(other.adc_sum, other.adc_sum + num_gains * num_pixels, adc_sum);
    std::copy(other.adc_known, other.adc_known + num_gains * num_pixels, adc_known);
    return *this;
}