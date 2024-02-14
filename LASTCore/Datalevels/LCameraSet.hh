
#ifndef _LCameraSet_HH_
#define _LCameraSet_HH_
#include "RtypesCore.h"
#include "TString.h"
#include "TObject.h"

class LCameraSet 
{
    public:
        TString name;
        int     tel_id;
        int     num_pixels;//number of pixels in this camera
        Double32_t *pix_x; //[num_pixels]  unit mm
        Double32_t *pix_y; //[num_pixels]  unit mm
        Double32_t  pixel_size;// unit mm
        int     pixel_shape;  // 0 for square, 1 for hexagon, 2 for circle

        LCameraSet();
        LCameraSet(std::string name, int tel_id, int num_pixels);
        ~LCameraSet();
        ClassDef(LCameraSet, 2)
};





#endif