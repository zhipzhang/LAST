#include "LCameraGeometry.hh"
#include <cstdlib>
#include <ostream>
#include <string>
#include <vector>
#include "RtypesCore.h"
#include "TArray.h"
#include "TKDTree.h"
#include "TMatrixDSparsefwd.h"
#include "TArrayD.h"
#include "TArrayI.h"
#include "spdlog/spdlog.h"
using std::string;
LCameraGeometry::LCameraGeometry(string n, int num, double* x, double* y, double size, int shape)
{

    name = n;
    num_pix = num;
    pix_x = std::vector<double>(x, x+num);
    pix_y = std::vector<double>(y, y+num);
    pix_size = size;
    pix_shape = static_cast<PixelShape>(shape);
    if( pix_shape == PixelShape::Square )
    {
        norm = 2;
        max_neighbors = 8; // including the diagonal neighbors
        radius = 1.8;
    }
    else if ( pix_shape == PixelShape::Hexagon || pix_shape == PixelShape::Circle)
    {
        norm = 2;
        max_neighbors = 6;
        radius = 1.4;
    }
    else 
    {
        spdlog::error("Pixel shape not supported");
        exit(EXIT_FAILURE);
    }
    kdtree = new TKDTreeID(num_pix, 2, 1);
    kdtree->SetData(0, &pix_x[0]);
    kdtree->SetData(1, &pix_y[0]);
    kdtree->Build();

    /*
    Allocate the mask1 and mask2
    */
    mask1.resize(num_pix, 0);
    mask2.resize(num_pix, 0);
    calc_pixel_neighbors();

}

void LCameraGeometry::calc_pixel_neighbors()
{
    TArrayI row(max_neighbors * num_pix);
    TArrayI col(max_neighbors * num_pix);
    TArrayD data{max_neighbors * num_pix};
    data.Reset(1.);

    int num_neighbors = 0;                // all number of neighbors for all pixels  num_neighbors < max_neighbors * num_pix
    neighbors = new TMatrixDSparse(num_pix, num_pix);
    // Calculate the neighbors of each pixel using kdtree
    // The neighbors are stored in a 2D Matrix, each row is a pixel, each column is a neighbor
    for( int i = 0; i < num_pix; i++)
    {
        double point[2]{pix_x[i], pix_y[i]};
        std::vector<double> distance(max_neighbors + 1);
        std::vector<int> index(max_neighbors + 1);
        /*
        Try to Fill the sparse matrix
        */
        kdtree->FindNearestNeighbors(point, max_neighbors + 1, &index[0], &distance[0]);
        for(int j = 0; j < max_neighbors + 1; j++)
        {
            if(distance[j] > 0 && distance[j] < radius * pix_size)
            {
                row[num_neighbors] = i;
                col[num_neighbors] = index[j];
                num_neighbors++;
            }
        }
        neighbors->SetMatrixArray(num_neighbors, row.GetArray(), col.GetArray(), data.GetArray());
    }
    set_border_pixel(2);

}

void LCameraGeometry::set_border_pixel(int depth)
{
    if (depth <= 2)
    {
        const Int_t* rindex = neighbors->GetRowIndexArray();
        const Int_t* cindex = neighbors->GetColIndexArray();
        const double* data = neighbors->GetMatrixArray();
        for(Int_t irow = 0; irow < neighbors->GetNrows(); irow++)
        {
            const Int_t col_num = rindex[irow+1] - rindex[irow];
            if (col_num > 0 && col_num < max_neighbors)
            {
                mask1[irow] = true;
            }
        }
        mask2 = mask1;
        for( Int_t irow = 0; irow < neighbors->GetNrows(); irow++)
        {
            if(!mask1[irow])
            {
                continue;
            }
            /*
            Same as ROOT User Guide
            */
            const Int_t sIndex = rindex[irow];
            const Int_t eindex = rindex[irow + 1];
            for (Int_t index = sIndex; index < eindex; ++index)
            {
                const Int_t icol = cindex[index];
                mask2[icol] = true;
            }
        }
    }
    else 
    {
        spdlog::warn("Depth > 2 is not supported yet");
    }
        
}
std::vector<int> LCameraGeometry::GetPixNeighbor(int pix_id) const
{
    std::vector<int> neighbors;
    const Int_t* rindex = this->neighbors->GetRowIndexArray();
    const Int_t* cindex = this->neighbors->GetColIndexArray();
    const double* data = this->neighbors->GetMatrixArray();
    const Int_t sIndex = rindex[pix_id -1];
    const Int_t eindex = rindex[pix_id];
    for (Int_t index = sIndex; index < eindex; ++index)
    {
        const Int_t icol = cindex[index];
        neighbors.push_back(icol);
    }
    return neighbors;
}
std::ostream& operator<<(std::ostream& os, const LCameraGeometry& cam)
{
    os << "Camera Name: " << cam.name << std::endl;
    os << "Pixel Number: " << cam.num_pix << std::endl;
    os << "Pixel Size: " << cam.pix_size << std::endl;
    return os;
};
void LCameraGeometry::FillCogPixels(double cog_x, double cog_y, std::vector<int> &cog_pix)
{
    for(int ipix = 0; ipix < num_pix; ipix++)
    {
        if( (pix_x[ipix] - cog_x) * (pix_x[ipix] - cog_x) + (pix_y[ipix] - cog_y) * (pix_y[ipix] - cog_y) < pix_size * pix_size)
        {
            cog_pix.push_back(ipix);
        }
    }
}
LCameraGeometry::~LCameraGeometry()
{
    delete kdtree;
    delete neighbors;
    pix_x.clear();
    pix_y.clear();
    mask1.clear();
    mask2.clear();
}