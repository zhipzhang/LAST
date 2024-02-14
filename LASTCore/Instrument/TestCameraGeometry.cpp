#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include "LCameraGeometry.hh"
#include <vector>

class TestGeometry: public ::testing::Test
{
    protected:

    TestGeometry()
    {
        int n_pixels_grid = 20;
        double start = 0, end = 19;
        double step = (end  - start) /(n_pixels_grid - 1);
        std::vector<double> pix_x, pix_y;
        for (int i = 0; i < n_pixels_grid; ++i) {
            for (int j = 0; j < n_pixels_grid; ++j) {
            pix_x.push_back(start + i * step);
            pix_y.push_back(start + j * step);
            }
        }
        int n_pixels = pix_x.size();
        camera1 = new LCameraGeometry("test", n_pixels, pix_x.data(), pix_y.data(), step, 2);

        int npix2 = 5;
        std::vector<double> pix_x2, pix_y2;
        for( int i = 0; i < npix2; i++)
        {
            for( int j  = 0; j < npix2; j++)
            {
                pix_x2.push_back(i);
                pix_y2.push_back(j);
            }
        }
        camera2 = new LCameraGeometry("test", npix2*npix2, pix_x2.data(), pix_y2.data(), 1, 2);
    }
    void SetUp() override
    {

    }
    void TearDown() override
    {

    }

    ~TestGeometry() override;


    LCameraGeometry* camera1;
    LCameraGeometry* camera2;
};


// ...

TEST_F(TestGeometry, TestSquareNeighbors)
{
    auto neighbor = camera1->GetPixNeighbor(21);
    EXPECT_THAT(neighbor, ::testing::ElementsAre(0, 1, 2, 20, 22, 40, 41, 42));
}
TEST_F(TestGeometry, TestBorder)
{
    std::vector<int> edge;
    for(int ipix = 0; ipix < camera2->GetPixNum(); ipix++)
    {
        if(camera2->Isborder(ipix))
        {
            edge.push_back(ipix);
        }
    }
    EXPECT_THAT(edge, ::testing::ElementsAre(0, 1, 2, 3, 4, 5, 10, 15, 20, 21, 22, 23, 24, 19, 14, 9 ));
}
TEST_F(TestGeometry, TestBorder2)
{
    std::vector<int> edge;
    for(int ipix = 0; ipix < camera2->GetPixNum(); ipix++)
    {
        if(camera2->Isborder2(ipix))
        {
            edge.push_back(ipix);
        }
    }
    EXPECT_THAT(edge, ::testing::ElementsAre(0,1,2,3,4,5,6,7,8,9,10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24));
}

