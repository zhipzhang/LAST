#include "LCleaning.hh"
#include "Instrument/LCameraGeometry.hh"
#include "TVectorDfwd.h"
#include "TVector.h"
#include "TVectorT.h"


void LClean::dilate(const LCameraGeometry& geom,  const std::vector<bool>& mask, std::vector<bool>& dilated_mask)
{   
    TVectorD mask_vector(mask.size());
    for( auto i = 0; i< mask.size(); i++)
    {
        if( mask[i])
        {
            mask_vector[i] = 1;
        }
        else
        {
            mask_vector[i] = 0;
        }
    }
    auto mask_neighbors = geom.GetNeighborMatrix() * mask_vector;
    for( auto i = 0; i < mask.size(); i++)
    {
        dilated_mask[i] = mask[i] || mask_neighbors[i];
    }
}