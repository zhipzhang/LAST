

#ifndef _LJSONCONFIG_HH
#define _LJSONCONFIG_HH
#include <string>
#include "gflags/gflags.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <sstream>

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
    int  CheckReconstructTel(double size, double width, double leakage1, double leakage2)
    {
        if(min_size > 0)
        {
            if(size < min_size)
            {
                return 0;
            }
        }
        if(width < min_width)
        {
            return 0;
        }
        
        if(max_leakage1 > 0 && leakage1 > max_leakage1)
        {
            return 0;
        }
        if(max_leakage2 > 0 && leakage2 > max_leakage2)
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
    static std::vector<int> splitStringToInt(const std::string& input, char delimiter) {
    std::vector<int> output;
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        try {
            output.push_back(std::stoi(token));
        } catch (const std::invalid_argument& e) {
            // 处理转换失败的情况
            std::cerr << "Invalid argument: " << token << std::endl;
        } catch (const std::out_of_range& e) {
            // 处理整数超出范围的情况
            std::cerr << "Out of range: " << token << std::endl;
        }
    }
        return output;
    }
    static std::vector<std::string> splitString(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    public:
        std::vector<std::string> input_fnames;
        std::vector<int> only_telescopes;
        LJsonConfig(int argc, char** argv)
        {
            ParseCommandLineFlags(argc, argv);
            ReadConfiguration();
            quality_check = new LQualityCheck();
            writer_info = new DataWriterinfo();
        }
        ~LJsonConfig(){ delete quality_check; delete writer_info;};
        void ParseCommandLineFlags(int argc, char** argv);
        void ReadConfiguration();
        int  StereoQuery(double size, double width, double leakage1, double leakage2) const
        {
            return quality_check->CheckReconstructTel(size, width, leakage1, leakage2);
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