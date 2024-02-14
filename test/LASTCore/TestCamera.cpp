#include "gtest/gtest.h"
#include <sys/stat.h>
#include "Instrument/LCameraGeometry.hh"
#include <vector>


class LCameraGeometryTest: public ::testing::Test{
    protected:
        LCameraGeometryTest(){};
        ~LCameraGeometryTest() override{};
        static void SetUpTestSuite();
        static void TearDownTestSuite(){};
        void SetUp() override{};
        void TearDown() override{};
    public:
        static LCameraGeometry* cam;
};

void LCameraGeometryTest::SetUpTestSuite(){
    int n_pixels_grid = 5;
    double start = -5.0, end = 5.0;
    double step = (end - start) / (n_pixels_grid - 1);

    std::vector<double> x, y;

    for (int i = 0; i < n_pixels_grid; ++i) {
        for (int j = 0; j < n_pixels_grid; ++j) {
            x.push_back(start + i * step);
            y.push_back(start + j * step);
        }
    }

    int n_pixels = x.size();  // should be equal to n_pixels_grid * n_pixels_grid
    cam = new LCameraGeometry("test", n_pixels, x.data(), y.data(), 2, Square);
}

TEST_F(LCameraGeometryTest, FindNeighborPixels)
{
    auto neighbors = cam->GetNeighborMatrix();
    auto neighbor_pixels = cam->GetPixNeighbor(11);

    std::vector<int> expected_neighbor_pixels{16,6, 10, 12};
    EXPECT_EQ(neighbor_pixels, expected_neighbor_pixels);
}

