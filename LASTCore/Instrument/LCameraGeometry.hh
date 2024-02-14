


#ifndef _LCameraGeometry_HH
#define _LCameraGeometry_HH
#include <ostream>
#include <string>
#include <iostream>
#include <vector>
#include "TKDTree.h"
#include "TMatrixDSparse.h"
#include "TMatrixDSparsefwd.h"
#include "bitset"

enum PixelShape{
            Circle,
            Hexagon,
            Square,
};

class LCameraGeometry{
    public:
        LCameraGeometry();
        LCameraGeometry(std::string name, int num_pix, double* pix_x, double* pix_y, double pix_size, int pix_shape);
        ~LCameraGeometry();
        void calc_pixel_neighbors();        // Compute the neighbors of each pixel using kdtree
        void set_border_pixel(int depth = 2);
        friend std::ostream& operator<<(std::ostream& os, const LCameraGeometry& cam);
        bool Isborder(int pix_id){return mask1[pix_id];}
        bool Isborder2(int pix_id){return mask2[pix_id];}

        const TMatrixDSparse& GetNeighborMatrix()const {return *neighbors;}
        const TMatrixDSparse& GetNeighborMatrix(){return *neighbors;}
        const std::vector<double>& GetPixX()const {return pix_x;}
        const std::vector<double>& GetPixY()const {return pix_y;}
        const double GetXPix(int pix_id)const {return pix_x[pix_id];}
        const double GetYpix(int pix_id)const {return pix_y[pix_id];}
        std::vector<int> GetPixNeighbor(int pix_id) const;
        void FillCogPixels(double cog_x, double cog_y,  std::vector<int>& cog_pix);
        int  GetPixNum() const {return num_pix;};
    private:
        std::string name;
        int num_pix;
        std::vector<double> pix_x;
        std::vector<double> pix_y;
        double pix_size;                    // Pixel size in mm ( All pixels are same)
        PixelShape pix_shape;               
        int max_neighbors;
        double radius;
        int norm = 2;                       // L2 norm for hex and circle, L2 for square
        TKDTreeID* kdtree;                  // Store the kdtree for all pixels;
        TMatrixDSparse* neighbors;          // Store the neighbors of each pixel, the advantage of using TMatrixDSparse is the matrix multiplication can
                                            // be done easily(In image cleanning), and the matrix is sparse, so it is memory efficient.
        std::vector<bool> mask1;            // mask1 is used to mask the pixels that are too close to the border
        std::vector<bool> mask2;            // mask2 is used to mask the pixels that are second close to the border
};

#endif