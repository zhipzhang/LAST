/**
 * @file LImageProcessor.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-01-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _LIMAGEPROCESSOR_HH_
#define _LIMAGEPROCESSOR_HH_

#include <memory>
#include <vector>
#include "LDL0Event.hh"
#include "LDL0TelEvent.hh"
#include "LDL1Event.hh"
#include "LDL1TelEvent.hh"
#include "Image/LCleaning.hh"
#include "LHillasParameters.hh"
#include "Datalevels/LTelescopesTemplate.hh"
#include "Instrument/LCameraGeometry.hh"
class LImageProcessor
{
    public:
        LImageProcessor() {cleaner = std::make_shared<LClean>();};
        virtual ~LImageProcessor() {};
        //virtual void Process() = 0;
        void ProcessClean(const LRDL0TelEvent&);
        void InitGeometry(const LTelescopes<std::shared_ptr<LRTelescopeConfig>>& tels_config);
        void ParameterizationImage(const LDL0Event& , LDL1Event&);
        void Compute_Leakage(LRDL1TelEvent& dl1televent);
        void Compute_Concentration(LRDL1TelEvent& dl1televent);
        void Compute_Intensity(LRDL1TelEvent& dl1televent);
        void Compute_Morphology(LRDL1TelEvent& dl1televent);
    private:
        std::vector<bool> clean_mask;
        std::vector<double> image;
        std::vector<double> cleaned_image;
        std::vector<double> island_size;    // Size for each island
        std::vector<std::vector<int>> island_pixels; // Pixels for each island
        int num_image_pixels;
        int min_image_pixels = 3;
        bool same_geometry = true;
        std::shared_ptr<LClean> cleaner;
        static LHillasParameters Compute_Hillas(const LCameraGeometry& geom, const std::vector<double>& cleaned_image);
        static void Compute_Morphology(const LCameraGeometry& geom, const std::vector<double>& cleaned_image, const std::vector<bool>& mask, std::vector<double>& island_size, std::vector<std::vector<int>>& island_labels);
        LCameraGeometry* tel_geometry;
        double focal_length;
        
};






#endif