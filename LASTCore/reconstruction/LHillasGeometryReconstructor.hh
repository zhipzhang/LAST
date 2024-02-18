


#ifndef _LHillasGeometryReconstructor_HH
#define _LHillasGeometryReconstructor_HH
#include "Datalevels/LDataBase.hh"
#include "Datalevels/LJsonConfig.hh"
#include "Datalevels/LTelConfig.hh"
#include  <map>
#include <memory>
#include <unordered_map>
#include "Image/LHillasParameters.hh"
#include "Datalevels/LTelescopesTemplate.hh"
class LHillasGeometryReconstructor 
{
    public:
    LHillasGeometryReconstructor(const LJsonConfig& config): cmd_config(config){};
    virtual ~LHillasGeometryReconstructor(){};
    const LJsonConfig& cmd_config;

    protected:
        void SetTelConfig(const LDataBase& data)
        {
            tel_config = data.GetTelescopesConfig();
        }
        double mc_alt;
        double mc_az;
        double MCxcore;
        double MCycore;
        std::shared_ptr<LTelescopes<std::shared_ptr<LRTelescopeConfig>>> tel_config;
        void SetMCAltAz(double alt, double az)
        {
            mc_alt = alt;
            mc_az = az;
        }
        void SetMCcore(double x, double y)
        {
            MCxcore = x;
            MCycore = y;
        }
        static inline double ComputeImpactdistance(double* tel_pos, double altitude, double azimuth, double x, double y)
        {
            double cx = cos(altitude)*cos(azimuth);
            double cy = -cos(altitude)*sin(azimuth);
            double cz = sin(altitude);
            return line_point_distance(x, y, 0, cx, cy, cz, tel_pos[0], tel_pos[1], tel_pos[2]);
        }
        static inline double line_point_distance (double xp1, double yp1, double zp1, 
                    double cx, double cy, double cz,
                double x, double y, double z)
        {
            double a, a1, a2, a3, b;
    
            a1 = (y-yp1)*cz - (z-zp1)*cy;
            a2 = (z-zp1)*cx - (x-xp1)*cz;
            a3 = (x-xp1)*cy - (y-yp1)*cx;
            a  = a1*a1 + a2*a2 + a3*a3;
            b = cx*cx + cy*cy + cz*cz;
            if ( a<0. || b<= 0. )
                return -1;
            return sqrt(a/b);
        }


    
};

#endif