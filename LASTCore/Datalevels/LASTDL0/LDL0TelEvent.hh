/**
 * @file LDL0TelEvent.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-01-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */


 #ifndef _LDL0_TEL_EVENT_HH_
 #define _LDL0_TEL_EVENT_HH_
 #include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"
class LRDL0TelEvent :public TObject
 {
    public:
        LRDL0TelEvent();
        virtual ~LRDL0TelEvent();
        void SetWaveForm(bool flag) {have_waveform_pe = flag;};
        void Init(int tel_id, int event_id, Double32_t tel_alt, Double32_t tel_az, int num_pixels)
            {
                this->event_id = event_id;
                this->tel_alt = tel_alt;
                this->tel_az = tel_az;
                this->tel_id = tel_id;
                this->num_pixels = num_pixels;
                true_image_pe = new Double32_t[num_pixels];
                pix_pe = new Double32_t[num_pixels];
                std::fill(pix_pe, pix_pe + num_pixels, 0);
                pix_time = new Double32_t[num_pixels];
                std::fill(pix_time, pix_time + num_pixels, 0);
            };
        void SetTruePixPe(int ipix, Double32_t true_pe)
        {
            true_image_pe[ipix] = true_pe;
        }
        int GetNumPixels() const
        {
            return num_pixels;
        }
        Double32_t* GetTruePe() const
        {
            return true_image_pe;
        }
        int GetEventId() const
        {
            return event_id;
        }
        Double32_t GetTelAlt() const
        {
            return tel_alt;
        }
        Double32_t GetTelAz() const
        {
            return tel_az;
        }
        LRDL0TelEvent& operator=(const LRDL0TelEvent& rhs);
    private:
        int tel_id;
        int event_id;
        Double32_t tel_alt;
        Double32_t tel_az;       

        bool have_waveform_pe = false;
        int num_pixels;
        Double32_t* true_image_pe = nullptr; // [num_pixels] Adding the NSB here.
        Double32_t* pix_pe = nullptr;        // [num_pixels] Get from the waveform.
        Double32_t* pix_time = nullptr;    // [num_pixels] Get from the waveform.
    ClassDef(LRDL0TelEvent, 2)
 };

















 #endif