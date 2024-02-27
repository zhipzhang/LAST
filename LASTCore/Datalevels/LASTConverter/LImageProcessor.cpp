
#include "LImageProcessor.hh"
#include "Datalevels/LASTDL0/LDL0TelEvent.hh"
#include "Image/LCleaning.hh"
#include "Instrument/LCameraGeometry.hh"
#include "TMatrixDfwd.h"
#include "TPrincipal.h"
#include <algorithm>
#include <numeric>
#include "eigen3/Eigen/Dense"
#include <deque>

LHillasParameters LImageProcessor::Compute_Hillas(const LCameraGeometry& geom, const std::vector<double>& cleaned_image)
{
    double cog_x, cog_y, cog_r, cog_phi;
    double size, length, width, psi;
    double skewness, kurtosis;

    cog_x = TMath::Mean(geom.GetPixX().begin(), geom.GetPixX().end(), cleaned_image.begin());
    cog_y = TMath::Mean(geom.GetPixY().begin(), geom.GetPixY().end(), cleaned_image.begin());
    cog_r = TMath::Sqrt(cog_x * cog_x + cog_y * cog_y);
    cog_phi = TMath::ATan2(cog_y, cog_x);

    size = std::accumulate(cleaned_image.begin(), cleaned_image.end(), 0.0);
    Eigen::MatrixXd datamatrix1(2, cleaned_image.size());
    Eigen::MatrixXd datamatrix2( cleaned_image.size(), 2);

//    TPrincipal principal(2, "D");

    std::vector<double> delta_x(geom.GetPixX().size());
    std::vector<double> delta_y(geom.GetPixY().size());
    int num_cols = 0;
    for(auto i = 0; i < cleaned_image.size(); i++)
    {
        delta_x[i] = geom.GetXPix(i) - cog_x;
        delta_y[i] = geom.GetYpix(i) - cog_y;
        datamatrix1.col(i) = Eigen::Vector2d(delta_x[i] * cleaned_image[i]/size, delta_y[i] * cleaned_image[i]/size);          
        datamatrix2.row(i) = Eigen::Vector2d(delta_x[i] , delta_y[i]) ;
        if (cleaned_image[i] > 0)
        {
            num_cols++;
        }
    }

    Eigen::MatrixXd cov = datamatrix1 * datamatrix2;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(cov);
       if (eigensolver.info() != Eigen::Success) {
        LOG(WARNING) << "Eigenvalue decomposition failed!"; 
    }

    // Using TPrincipal is not working, so I use Eigen instead.
    //Perform the PCA
    //principal.MakePrincipals();
    // Get the eigenvalues of the covariance matrix, which represent the variances of the principal components.
    // The squaere root of the eigenvalues gives the standard deviation along the principal axes, which can be interpreted as the length and width of the ellipse.
    //double trace = 0;
    //const TMatrixD* covarianceMatrix = principal.GetCovarianceMatrix();
    /*
    covarianceMatrix->Print();
    trace = (*covarianceMatrix)[0][0] + (*covarianceMatrix)[1][1];
    const TVectorD* eigenvalues = principal.GetEigenValues();
    length = sqrt((*eigenvalues)[0]) * trace;
    width = sqrt((*eigenvalues)[1]) * trace;
    const TMatrixD* eigenvectors = principal.GetEigenVectors();
    double vx = (*eigenvectors)[0][0];
    double vy = (*eigenvectors)[1][0];
    */


    Eigen::VectorXd eigenvalues = eigensolver.eigenvalues();
    Eigen::MatrixXd eigenvectors = eigensolver.eigenvectors();
    double vx, vy;
    vx = eigenvectors(0, 0);
    vy = eigenvectors(1, 0);
    if(eigenvalues(0) > eigenvalues(1))
    {
        length = sqrt(eigenvalues(0));
        width = sqrt(eigenvalues(1));
    }
    else
    {
        length = sqrt(eigenvalues(1));
        width = sqrt(eigenvalues(0));
        vx = eigenvectors(0, 1);
        vy = eigenvectors(1, 1);
    }
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
    skewness = third_moment / pow(length, 3)/size;
    kurtosis = fourth_moment / pow(length, 4)/size;
    return LHillasParameters(length, width, size, psi, cog_x, cog_y, cog_r, cog_phi, skewness, kurtosis);
}

void LImageProcessor::InitGeometry(const LTelescopes<std::shared_ptr<LRTelescopeConfig>>& tels_config)
{
    auto tels_id = tels_config.GetKeys();
    int itel = tels_id[0];
    tel_geometry = new LCameraGeometry((tels_config)[itel]->camera_name, (tels_config)[itel]->num_pixels, (tels_config)[itel]->pix_x, (tels_config)[itel]->pix_y, (tels_config)[itel]->pix_size, (tels_config)[itel]->pix_shape);
    focal_length = (tels_config)[itel]->focal_length;
    same_geometry = true;
}

void LImageProcessor::ProcessClean(const LRDL0TelEvent& dl0tel_event)
{
    num_image_pixels = 0;
    image.clear();
    clean_mask.clear();
    cleaned_image.clear();
    image.resize(dl0tel_event.GetNumPixels());
    clean_mask.resize(dl0tel_event.GetNumPixels());
    std::copy(dl0tel_event.GetTruePe(), dl0tel_event.GetTruePe() + dl0tel_event.GetNumPixels(), image.begin());
    cleaner->tailcuts_clean(*tel_geometry, image , clean_mask);
    for(int i = 0; i < dl0tel_event.GetNumPixels();i ++)
    {
        if(clean_mask[i])
        {
            cleaned_image.push_back(image[i]);
            num_image_pixels++;
        }
        else {
            cleaned_image.push_back(0);
        }
    }

}
void LImageProcessor::ParameterizationImage(const LDL0Event& dl0event, LDL1Event& dl1event)
{
    dl1event.Clear();
    for(auto itel :dl0event.GetTelList())
    {
        dl1event.GetEventArrayInfo() = dl0event.GetShowerInfo();
        int event_id = dl0event[itel].GetEventId();
        ProcessClean(dl0event[itel]);
        if(num_image_pixels < min_image_pixels)
        {
           continue; 
        }
        auto dl1televent = std::make_shared<LRDL1TelEvent>();
        dl1televent->SetTelHillas(event_id, itel, Compute_Hillas(*tel_geometry, cleaned_image).ConvertRad(focal_length));
        dl1televent->SetTelPointing(dl0event[itel].GetTelAlt(), dl0event[itel].GetTelAz());
        Compute_Leakage(*dl1televent);
        Compute_Concentration(*dl1televent);
        Compute_Intensity(*dl1televent);
        dl1event.AddTelEvent(itel, dl1televent);
    }
}

void LImageProcessor::Compute_Leakage(LRDL1TelEvent& dl1televent)
{
    double leakage1 = 0;
    double leakage2 = 0;
    double num_pix1 = 0;
    double num_pix2 = 0;
    for(int ipix = 0; ipix < cleaned_image.size(); ipix++)
    {
        if( cleaned_image[ipix] == 0)
        {
            continue;
        }
        if(tel_geometry->Isborder(ipix))
        {
            leakage1 += cleaned_image[ipix];
            num_pix1++;
        }
        if(tel_geometry->Isborder2(ipix))
        {
            leakage2 += cleaned_image[ipix];
            num_pix2++;
        }
    }
    dl1televent.SetLeakage(leakage1, num_pix1/num_image_pixels, leakage2, num_pix2/num_image_pixels);
}

void LImageProcessor::Compute_Concentration(LRDL1TelEvent &dl1televent)
{
    double x = dl1televent.GetCogx();
    double y = dl1televent.GetCogy();
    double cog_size;
    double core_size;
    std::vector<int> cog_pixels;
    std::vector<int> core_pixels; // pixels in the ellipse
    tel_geometry->FillCogPixels(x, y, cog_pixels);

    for(auto ipix : cog_pixels)
    {
        cog_size += cleaned_image[ipix];
    }
    auto  max_size = std::max_element(cleaned_image.begin(), cleaned_image.end());
    dl1televent.SetConcentration(cog_size, core_size, *max_size);
}
void LImageProcessor::Compute_Intensity(LRDL1TelEvent &dl1televent)
{
    double mean = TMath::Mean(cleaned_image.begin(), cleaned_image.end(), clean_mask.begin());
    double std = TMath::RMS(cleaned_image.begin(), cleaned_image.end(), clean_mask.begin());
    std::vector<double> temp(cleaned_image.size());
    std::vector<double> temp2(cleaned_image.size());

    std::transform(cleaned_image.begin(), cleaned_image.end(), temp.begin(), [mean, std](double x){return pow((x - mean)/std, 3);});
    std::transform(cleaned_image.begin(), cleaned_image.end(), temp2.begin(), [mean, std](double x){return pow((x - mean)/std, 4);});
    double skewness = TMath::Mean(temp.begin(), temp.end(), clean_mask.begin());
    double kurtosis = TMath::Mean(temp2.begin(), temp2.end(), clean_mask.begin());

    dl1televent.SetIntensity(mean, std, skewness, kurtosis);

}

void LImageProcessor::Compute_Morphology(const LCameraGeometry& geom, const std::vector<double>& cleaned_image, const std::vector<bool>& mask, std::vector<double>& island_size, std::vector<std::vector<int>>& island_pixels)
{
    int num_pixels = cleaned_image.size();
    if(geom.GetPixX().size() != num_pixels)
    {
        LOG(ERROR) << "The number of pixels in the geometry is not equal to the number of pixels in the image!";
        return;
    }
    std::deque<int> pixel_queue;
    std::vector<bool> is_labeled(num_pixels, false);

    for( int ipix = 0 ; ipix < num_pixels; ++ipix)
    {
        std::vector<int> island;
        double islandsize = 0;
        if( !mask[ipix] || is_labeled[ipix])
        {
            continue;
        }
        is_labeled[ipix] = true;
        pixel_queue.push_back(ipix);
        while( !pixel_queue.empty())
        {
            int current_pix = pixel_queue.front();
            pixel_queue.pop_front();
            island.push_back(current_pix);
            islandsize += cleaned_image[current_pix];
            for( auto neighbor : geom.GetPixNeighbor(current_pix))
            {
                if( !mask[neighbor] || is_labeled[neighbor])
                {
                    continue;
                }
                is_labeled[neighbor] = true;
                pixel_queue.push_back(neighbor);
            }
        }
        island_size.push_back(islandsize);
        island_pixels.push_back(island);
    }
};

void LImageProcessor::Compute_Morphology(LRDL1TelEvent &dl1televent)
{
    island_pixels.clear();
    island_size.clear();
    LImageProcessor::Compute_Morphology(*tel_geometry, cleaned_image, clean_mask, island_size, island_pixels);
    int num_island = island_size.size();
    int num_large_island = 0;
    int num_medium_island = 0;
    int num_small_island = 0;
    int verified_pixels = 0;
    for( auto island : island_pixels)
    {
        if( island.size() < 3)
            num_small_island++;
        if( island.size() >= 3 && island.size() < 50)
            num_medium_island++;
        if( island.size() >= 50)
            num_large_island++;
        verified_pixels += island.size();
    }
    if( verified_pixels != num_image_pixels)
    {
        LOG(ERROR) << "The number of verified pixels is not equal to the number of image pixels!";
    }
    dl1televent.SetMorphology(num_island, num_image_pixels, num_large_island, num_medium_island, num_small_island);
}