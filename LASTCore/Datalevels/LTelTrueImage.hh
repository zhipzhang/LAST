#ifndef _LTelTrueImage_HH
#define _LTelTrueImage_HH
#include "Rtypes.h"
#include "TObject.h"
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
    LTelTrueImage();
    virtual ~LTelTrueImage();
    void Allocate()
    {
        if( num_pixels > 0)
        {
            true_pe = new int[num_pixels];
        }
    }
    ClassDef(LTelTrueImage, 1)
};

class LRTelTrueImage: public LTelTrueImage, public TObject
{
    public:
        LRTelTrueImage():LTelTrueImage(){}
        virtual ~LRTelTrueImage(){};
        LRTelTrueImage& operator=(const LRTelTrueImage& other);
        ClassDef(LRTelTrueImage, 0)
};

#endif