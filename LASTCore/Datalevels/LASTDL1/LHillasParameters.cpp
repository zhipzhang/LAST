


#include "LHillasParameters.hh"
#include "TMath.h"
#include "TPrincipal.h"
#include <cmath>
#include <numeric>
#include <root/TMath.h>

LHillasParameters::LHillasParameters()
{
    length = -99;
    width = -99;
    size = -99;
    psi = -99;
    cog_x = -99;
    cog_y = -99;
    cog_r = -99;
    cog_phi = -99;
    skewness = -99;
    kurtosis = -99;
    length_uncertainty = -99;
    width_uncertainty = -99;
}

/**
 * @brief Tranform from Telescope frame to Array  Pointing frame
 * 
 * @param tel_pointing_direction : azimuth altitude
 * @param array_pointing_direction : azimuth altitude
 * @return LHillasParameters 
 */
LHillasParameters LHillasParameters::transform_frame(std::pair<double, double> tel_pointing_direction, std::pair<double, double> array_pointing_direction) const
{
    double tel_az = tel_pointing_direction.first;
    double tel_alt = tel_pointing_direction.second;
    double array_az = array_pointing_direction.first;
    double array_alt = array_pointing_direction.second;

    double new_x, new_y, new_psi, new_r, new_cogphi;
    double azm_img, alt_img;
    offset_to_angles(cog_x, cog_y, tel_az, tel_alt, 1, azm_img, alt_img);
    double dphi1 = -atan(tan(azm_img - tel_az) * sin(alt_img));
    angles_to_offset(azm_img, alt_img, array_az, array_alt, 1, new_x, new_y);
    double dphi2  = -atan(tan(azm_img - array_az) * sin(array_alt));
    new_psi = psi + (dphi2 - dphi1);
    new_r   = TMath::Sqrt(pow(new_x, 2) + pow(new_y, 2));
    new_cogphi = atan2(new_y, new_x);
    return LHillasParameters(length,width, size, new_psi, new_x, new_y, new_r, new_cogphi, skewness, kurtosis);
}

void LHillasParameters::angles_to_offset(double obj_az, double obj_alt, double az, double alt, double focal_length, double &offset_x, double &offset_y)
{
    double daz = obj_az - az;
    double coa = cos(obj_alt);

    double xp0 = -cos(daz) * coa;
    double yp0 = sin(daz) * coa;
    double zp0 = sin(obj_alt);

    double cx = sin(alt);
    double sx = cos(alt);
    double xp1 = cx * xp0 + sx * zp0;
    double yp1 = yp0;
    double zp1 = -sx * xp0 + cx * zp0;
    if( xp1 == 0 && yp1 == 0)
    {
        offset_x = offset_y = 0;
        return;
    }
    offset_x = focal_length * xp1 / zp1;
    offset_y = focal_length * yp1 / zp1;
}

void LHillasParameters::offset_to_angles(double offset_x, double offset_y, double az, double alt, double focal_length, double &obj_az, double &obj_alt)
{
    if(offset_x == 0 && offset_y == 0)
    {
        obj_az = az;
        obj_alt = alt;
        return;
    }
    else 
    {
        double d = sqrt(offset_x * offset_x + offset_y * offset_y);
        double q = atan(d/focal_length);

        double sq = sin(q);
        double xp1 = offset_x * sq / d;
        double yp1 = offset_y * sq / d;
        double zp1 = cos(q);

        double cx = sin(alt);
        double sx = cos(alt);

        double xp0 = cx * xp1 - sx * zp1;
        double yp0 = yp1;
        double zp0 = sx * xp1 + cx * zp1;

        obj_alt = asin(zp0);
        obj_az = az + atan2(yp0, -xp0);
        if(obj_az < 0)
        {
            obj_az += 2 * TMath::Pi();
        }
        else if(obj_az > 2 * TMath::Pi())
        {
            obj_az -= 2 * TMath::Pi();
        }
    }
}
/**
 * @brief Compute the hillas parameters from the cleaned image (Same as ctapipe/image/hillas.py)
 * 
 * @param geom 
 * @param cleaned_image : pe per pixel (after cleaning mask)
 */
/*
void LHillasParameters::Compute_Hillas(const LCameraGeometry &geom, const std::vector<double> &cleaned_image)
{

    cog_x = TMath::Mean(geom.GetPixX().begin(), geom.GetPixX().end(), cleaned_image.begin());
    cog_y = TMath::Mean(geom.GetPixY().begin(), geom.GetPixY().end(), cleaned_image.begin());
    cog_r = TMath::Sqrt(cog_x * cog_x + cog_y * cog_y);
    cog_phi = TMath::ATan2(cog_y, cog_x);

    size = std::accumulate(cleaned_image.begin(), cleaned_image.end(), 0.0);
    TPrincipal principal(2, "ND");
    std::vector<double> delta_x(geom.GetPixX().size());
    std::vector<double> delta_y(geom.GetPixY().size());
    for(auto i = 0; i < cleaned_image.size(); i++)
    {
        delta_x[i] = geom.GetXPix(i) - cog_x;
        delta_y[i] = geom.GetYpix(i) - cog_y;
        if (cleaned_image[i] > 0)
        {
            double data[2] = {geom.GetXPix(i) * cleaned_image[i]/size, geom.GetYpix(i) * cleaned_image[i]/size};
            principal.AddRow(data);
        }
    }


    //Perform the PCA
    principal.MakePrincipals();
    // Get the eigenvalues of the covariance matrix, which represent the variances of the principal components.
    // The square root of the eigenvalues gives the standard deviation along the principal axes, which can be interpreted as the length and width of the ellipse.
    const TVectorD* eigenvalues = principal.GetEigenValues();
    length = sqrt((*eigenvalues)[0]);
    width = sqrt((*eigenvalues)[1]);
    const TMatrixD* eigenvectors = principal.GetEigenVectors();
    double vx = (*eigenvectors)[0][0];
    double vy = (*eigenvectors)[1][0];
    if (length == 0)
    {
        psi = skewness =  kurtosis = TMath::QuietNaN();
    }
    else {
        if (vx != 0)
            psi = atan(vy / vx);
        else
            psi = TMath::Pi() / 2;
    }
    double third_moment = 0;
    double fourth_moment = 0;
    for( auto i = 0; i < cleaned_image.size(); i++)
    {
        double longitudal = delta_x[i] * cos(psi) + delta_y[i] * sin(psi);
        third_moment += cleaned_image[i] * pow(longitudal, 3);
        fourth_moment += cleaned_image[i] * pow(longitudal, 4);
    }
    skewness = third_moment / pow(length, 3);
    kurtosis = fourth_moment / pow(length, 4);

}

*/