
#ifndef _LHILLAS_HH
#define _LHILLAS_HH

#include "Instrument/LCameraGeometry.hh"
class LHillasParameters 
{
    public:
        LHillasParameters();
        LHillasParameters(double length, double width, double size, double psi, double cog_x, double cog_y, double cog_r, double cog_phi, double skewness, double kurtosis): length(length), width(width), size(size), psi(psi), cog_x(cog_x), cog_y(cog_y), cog_r(cog_r), cog_phi(cog_phi), skewness(skewness), kurtosis(kurtosis) {};
        ~LHillasParameters(){};
        double GetSize()
        {
            return size;
        }
        double GetSize() const
        {
            return size;
        }
        double GetCogx() const
        {
            return cog_x;
        }
        double GetCogy() const
        {
            return cog_y;
        }
        double GetPsi() const
        {
            return psi;
        }
        double GetLength() const
        {
            return length;
        }
        double GetWidth() const
        {
            return width;
        }
        LHillasParameters& ConvertRad(double f)
        {
            length = length / f;
            width = width / f;
            cog_x = cog_x / f;
            cog_y = cog_y / f;
            cog_r = cog_r / f;
            return *this;
        }
        LHillasParameters transform_frame(std::pair<double, double> tel_pointing_direction, std::pair<double, double> array_pointing_direction) const;
        static void angles_to_offset(double obj_az, double obj_alt, double az, double alt, double focal_length, double& offset_x, double& offset_y);
        static void offset_to_angles(double offset_x, double offset_y,double az, double alt, double focal_length, double& obj_az, double& obj_alt);
    private:
        double length;          // length of the main axis  [rad]
        double width;           // length of the minor axis [rad]
        double size;            // total size of the image  [Pe.]
        double psi;             // angle between the main axis and the x-axis
        double cog_x;           // x coordinate of the center of gravity [rad]
        double cog_y;           // y coordinate of the center of gravity [rad])
        double cog_r;           // distance of the center of gravity from the center of the camera [m?]
        double cog_phi;             // atan (corey / corex)
        double skewness;
        double kurtosis;

        // Uncertainty of Hillas paramters like in ctapipe
        double length_uncertainty = -1; 
        double width_uncertainty  = -1; 
        double miss = -1;       // miss parameter
        ClassDef(LHillasParameters, 2)
};

#endif