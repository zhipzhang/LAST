#ifndef  _LTELEVENTELECTRONIC_HH
#define  _LTELEVENTELECTRONIC_HH


#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"
#include "../LTelescopesTemplate.hh"
#include <cstdint>
#include <memory>
class LTelElectronic
{
    public:
        int tel_id;
        int event_id;
        int num_gains;
        int num_pixels;
        int num_samples;
        double tel_alt;
        double tel_az;
        uint16_t* adc_sample; //[num_gains][num_pixels][num_samples]
        uint32_t* adc_sum;    //[num_gains][num_pixels]
        uint8_t* adc_known;  //[num_gains][num_pixels]
        LTelElectronic();
        virtual ~LTelElectronic();
        void Allocate()
        {
            if( num_gains > 0 && num_pixels >0)
            {
                adc_sample = new uint16_t[num_gains * num_pixels * num_samples];
                adc_sum = new uint32_t[num_gains * num_pixels];
                adc_known = new uint8_t[num_gains * num_pixels];
            }
        }

        ClassDef(LTelElectronic, 3)
};

class LRTelElectronic: public LTelElectronic, public TObject
{
    public:
        LRTelElectronic():LTelElectronic(){}
        virtual ~LRTelElectronic(){};
        LRTelElectronic& operator=(const LRTelElectronic& other);
    ClassDef(LRTelElectronic, 0)
};

#endif