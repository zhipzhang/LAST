/**
 * @file LCleaning.hh
 * @author zhipzhang (zhipzhang@mail.ustc.edu.cn)
 * @brief All algorithms return a boolean mask that is True for pixels surviving
 *        the cleaning.
 * @version 0.1
 * @date 2023-12-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _LCLEANING_HH
#define _LCLEANING_HH

#include "Instrument/LCameraGeometry.hh"
#include <vector>
#include "TVectorD.h"
#include "TVectorDfwd.h"
class LClean {
    public:

    static void tailcuts_clean(const LCameraGeometry& geom, const std::vector<double>& image, std::vector<bool>& mask, const double picture_thresh = 10, const double boundary_thresh = 7, const bool keep_isolated_pixels = false, int min_number_picture_neighbors = 0)
    {
        // TODO
        TVectorD pixels_above_picture(image.size());
        TVectorD pixels_above_boundary(image.size());
        TVectorD pixels_in_picture(image.size());
        for( auto i = 0; i < image.size(); i++)
        {
            if ( image[i] > picture_thresh )
            {
                pixels_above_picture[i] = 1;
            }
            else
            {
                pixels_above_picture[i] = 0;
            }
            if ( image[i] > boundary_thresh )
            {
                pixels_above_boundary[i] = 1;
            }
            else
            {
                pixels_above_boundary[i] = 0;
            }
        }

        if( keep_isolated_pixels || min_number_picture_neighbors == 0)
        {
            pixels_in_picture = pixels_above_picture;
        }
        else 
        {
            TVectorD number_of_neighbors_above_picture = geom.GetNeighborMatrix() * pixels_above_picture;
            for( auto i = 0; i < image.size(); i++)
            {
                if( number_of_neighbors_above_picture[i] >= min_number_picture_neighbors)
                {
                    pixels_in_picture[i] = 1;
                }
                else
                {
                    pixels_in_picture[i] = 0;
                }
            }
        }
        TVectorD pixels_with_picture_neighbors = geom.GetNeighborMatrix() * pixels_in_picture;
        if( keep_isolated_pixels)
        {
            for( auto i = 0; i < image.size(); i++)
            {
                if( (pixels_with_picture_neighbors[i] > 0 && pixels_above_boundary[i] > 0) || pixels_in_picture[i] > 0)
                {
                    mask[i] = true;
                }
                else
                {
                    mask[i] = false;
                }
            }
        }
        else
        /*
            Don't keep isolated pixels, two-level cleaning
            1) above boundary and neighbor above picture
            2) above picture and neighbor above boundary
        */
        {
            auto pixels_with_boundary_neighbors = geom.GetNeighborMatrix() * pixels_above_boundary;
            for( auto i = 0; i < image.size(); i++)
            {
                if( (pixels_above_boundary[i] > 0 && pixels_with_picture_neighbors[i] > 0) || (pixels_in_picture[i] > 0 && pixels_with_boundary_neighbors[i] > 0))
                {
                    mask[i] = true;
                }
                else
                {
                    mask[i] = false;
                }
            }
        }

    }

    
    static void mars_cleaning( const LCameraGeometry& geom, const std::vector<double>& image, std::vector<bool>& mask, const double picture_thresh = 7, const double boundary_thresh = 5, const bool keep_isolated_pixels = false, int min_number_picture_neighbors = 0)
    {
        // This is more suitable for the LST camera (Low Energy Threshold IACT)
        // It may keep the neighbor of tail-cuts neighbors!
        TVectorD pixels_from_tailcuts_clean(image.size());
        TVectorD pixels_above_2nd_boundary(image.size());

        auto tmp_mask = mask;
        tailcuts_clean(geom, image, tmp_mask, picture_thresh, boundary_thresh, keep_isolated_pixels, min_number_picture_neighbors);
        for( auto i = 0; i < image.size(); i++)
        {
            if( tmp_mask[i])
            {
                pixels_from_tailcuts_clean[i] = 1;
            }
            if( image[i] > boundary_thresh)
            {
                pixels_above_2nd_boundary[i] = 1;
            }
        }
        // It contains the all neighbors of the tailcuts_clean pixels
        TVectorD pixels_with_previous_neighbors = geom.GetNeighborMatrix() * pixels_from_tailcuts_clean;
        if (keep_isolated_pixels)
        {
            for( int i = 0; i < image.size(); i++)
            {
                if( (pixels_above_2nd_boundary[i] > 0 && pixels_with_previous_neighbors[i] > 0 ) || (pixels_from_tailcuts_clean[i] > 0))
                {
                    mask[i] = true;
                }
                else
                {
                    mask[i] = false;
                }
            }
        }
        else 
        {
            TVectorD pixels_with_2ndboundary_neighbors = geom.GetNeighborMatrix() * pixels_above_2nd_boundary;    
            for( int i = 0; i < image.size(); i++)
            {
                if ((pixels_above_2nd_boundary[i] > 0 && pixels_with_previous_neighbors[i] > 0) || (pixels_from_tailcuts_clean[i] > 0 && pixels_with_2ndboundary_neighbors[i] > 0))
                {
                    mask[i] = true;
                }
                else
                {
                    mask[i] = false;
                }
            }
        }


    }
    static void dilate(const LCameraGeometry& geom, const std::vector<bool>& mask, std::vector<bool>& mask2);
    void SetTailcutsParameters(double picture_thresh, double boundary_thresh, int min_number_picture_neighbors);
    private:
        double picture_thresh = 5;
        double boundary_thresh = 10;
        int min_number_picture_neighbors = 4;
        
};














#endif