#ifndef _LTelTrueImage_HH
#define _LTelTrueImage_HH
#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"
#include "TH1D.h"
class LTelTrueImage  
{
    public:
    int event_id;
    int tel_id;
    int num_pixels = 0;
    Double32_t tel_alt;
    Double32_t tel_az;
    // Not know whether it needs to allocate memory for true_pe.
    int* true_pe = nullptr;        //[num_pixels] True Pe is represented as an integer.
    int  npe;
    Double32_t* pe_time = nullptr;    //![npe]
    Double32_t* pe_intensity = nullptr;//![npe]
    LTelTrueImage();
    virtual ~LTelTrueImage();
    void Allocate()
    {
        if( num_pixels > 0)
        {
            true_pe = new int[num_pixels];
        }
    }
    void Allocate_Pe(int npe)
    {
        this->npe = npe;
        if(npe > 0)
        {
            pe_time = new Double32_t[npe];
            pe_intensity = new Double32_t[npe];
        }
    }
    ClassDef(LTelTrueImage, 4)
};

class LRTelTrueImage: public LTelTrueImage, public TObject
{
    public:
        double time_spread1 = 0;  // Time spread for 10% - 90%
        double time_spread2 = 0;  // Time spread for 20% - 80%
        LRTelTrueImage():LTelTrueImage(){}
        virtual ~LRTelTrueImage(){};
        LRTelTrueImage& operator=(const LRTelTrueImage& other);
        void Compute_Spread();
        ClassDef(LRTelTrueImage, 1)
};

#endif