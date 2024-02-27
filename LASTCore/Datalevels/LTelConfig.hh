#ifndef _LTelConfig_HH_
#define _LTelConfig_HH_

#include <memory>
#include <vector>
#include "LCameraSet.hh"
#include "LTelescopesTemplate.hh"
#include <memory.h>
#include "RtypesCore.h"
#include "TObject.h"

class LCameraGeometry;
class LOptics;
struct Telescope_Pos
{
    double x;
    double y;
    double z;

    void SetTelPos(double xs, double ys, double zs)
    {
        x = xs;
        y = ys;
        z = zs;
    }
};

class LRTelescopeConfig : public TObject
{
    public:
    LRTelescopeConfig(){};
    ~LRTelescopeConfig();
    int tel_id;
    double pos[3];
    int num_pixels;
    double* pix_x = nullptr; //[num_pixels]
    double* pix_y = nullptr; //[num_pixels]
    std::string camera_name;
    Double32_t pix_size;
    int pix_shape;
    Double32_t focal_length;
    void SetTelPos(double x, double y, double z)
    {
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
    }
    void InitCameraSet(std::string, int , int);
    //void InitOptics(std::string, int);
    ClassDef(LRTelescopeConfig, 6)
};
#endif