#include "LDL0TelEvent.hh"

LRDL0TelEvent::LRDL0TelEvent()
{
}

LRDL0TelEvent::~LRDL0TelEvent()
{
    if( num_pixels >0)
    {
    delete[] true_image_pe;
    delete[] pix_pe;
    delete[] pix_time;
    }
}
LRDL0TelEvent& LRDL0TelEvent::operator=(const LRDL0TelEvent& rhs)
{
    // Check for self-assignment
    if (this == &rhs)
    {
        return *this;
    }
    if( num_pixels >0)
    {
    delete[] true_image_pe;
    delete[] pix_pe;
    delete[] pix_time;
    }
    // Perform the copy assignment
    Init(rhs.tel_id, rhs.event_id, rhs.tel_alt, rhs.tel_az, rhs.num_pixels);
    std::copy(rhs.true_image_pe, rhs.true_image_pe + num_pixels, true_image_pe);
    std::copy(rhs.pix_pe, rhs.pix_pe + num_pixels, pix_pe);
    std::copy(rhs.pix_time, rhs.pix_time + num_pixels, pix_time);
    return *this;
}