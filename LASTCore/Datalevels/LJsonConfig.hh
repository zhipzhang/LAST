

#ifndef _LJSONCONFIG_HH
#define _LJSONCONFIG_HH
#include <string>
#include "Image/LCleaning.hh"
#include "gflags/gflags.h"
#include "nlohmann/json.hpp"
#include "./LASTDL1/LDL1TelEvent.hh"

/**
 * @brief It not only handle the Json configuration but also the command line configuration.
 * 
 */

struct DataWriterinfo
{
    std::string owner_name = "Spam";
    std::string owner_email = "Spam.com";
    std::string owner_organization = "IHEP/USTC";
    bool write_config = true;
    bool write_showers =  false;
    bool remote_files = true;
    std::string remote_url = "root://eos01.ihep.ac.cn/";
};

struct LQualityCheck
{
    double min_size = -1;
    double min_width  = 0;
    int    min_n_pixels = -1;
    double max_leakage1 = -1;
    double max_leakage2 = -1;
    //bool   CheckImageprocess();
    int  CheckReconstructTel(const LRDL1TelEvent& dl1televent)
    {
        if(min_size > 0)
        {
            if(dl1televent.GetSize() < min_size)
            {
                return 0;
            }
        }
        if(dl1televent.GetWidth() < min_width)
        {
            return 0;
        }
        
        if(max_leakage1 > 0 && dl1televent.GetLeakage1() > max_leakage1)
        {
            return 0;
        }
        if(max_leakage2 > 0 && dl1televent.GetLeakage2() > max_leakage2)
        {
            return 0;
        }
        return 1;
    }
    LQualityCheck(){};
    ~LQualityCheck(){};


};
class LJsonConfig
{   
    unsigned long max_io_length = 10000000;
    std::string camera_name;
    std::string json_fname;
    std::string input_fname;
    std::string output_fname;
    DataWriterinfo* writer_info;
    LQualityCheck* quality_check;
    const std::vector<std::string> clean_methods{"TailCutsCleaner", "MARSCleaner"};
    bool StoreWaveform = false;
    public:
        LJsonConfig(int argc, char** argv)
        {
            ParseCommandLineFlags(argc, argv);
            ReadConfiguration();
            quality_check = new LQualityCheck();
        }
        ~LJsonConfig(){};
        void ParseCommandLineFlags(int argc, char** argv);
        void ReadConfiguration();
        int  StereoQuery(const LRDL1TelEvent& dl1televent) const
        {
            return quality_check->CheckReconstructTel(dl1televent);
        }
        std::string GetInputFileName() const
        {
            return input_fname;
        }
        std::string GetOutputFileName() const
        {
            return output_fname;
        }
        unsigned long GetMaxIOLength() const
        {
            return max_io_length;
        }
        std::string GetCameraName() const
        {
            return camera_name;
        }
        std::string GetUrl() const
        {
            return writer_info->remote_url;
        }
        bool WriteWaveform() const
        {
            return StoreWaveform;
        }

};










#endif