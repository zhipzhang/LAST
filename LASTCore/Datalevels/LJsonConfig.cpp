#include "gflags/gflags.h"
#include "LJsonConfig.hh"
#include <fstream>
DEFINE_string(input_file, "", "Comma-separated list of input file PATHs");
DEFINE_string(output_file, "dst.root", "Output root file PATH");
DEFINE_string(json_path, "../config.json", "Json file path");
DEFINE_string(only_telescope,"","Comma-separated llist of telescope ids");
DEFINE_int32(min_multiplicity, 0, "minimum multiplicity of the shower");
DEFINE_double(max_leakage2, 0, "max_leakage2 ");

void LJsonConfig::ParseCommandLineFlags(int argc, char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    json_fname = FLAGS_json_path;
    output_fname = FLAGS_output_file;
    input_fname = FLAGS_input_file; // keep the one file interface
    if(input_fname.find(',') != std::string::npos)
        input_fnames = splitString(FLAGS_input_file, ',');
    if(FLAGS_only_telescope != "")
    {
        filter_tel = true;
        only_telescopes = splitStringToInt(FLAGS_only_telescope, ',');
    }
}

void LJsonConfig::SetCommandLine()
{
    if(FLAGS_min_multiplicity > 0)
    {
        min_multiplicity = FLAGS_min_multiplicity;
    }
    if(FLAGS_max_leakage2 > 0)
    {
        quality_check->max_leakage2 = FLAGS_max_leakage2;
    }

}
void LJsonConfig::ReadConfiguration()
{
    std::ifstream f(json_fname);
    auto config = nlohmann::json::parse(f);
    max_io_length = config.at("MAX_IO_LENGTH");
    camera_name = config.at("Camera_Name");
    if(config.at("LImageQuality").at("min_size") != nullptr)
    {
        quality_check->min_size = config.at("LImageQuality").at("min_size");
    }
    if(config.at("LImageQuality").at("max_leakage2") != nullptr)
    {
        quality_check->max_leakage2 = config.at("LImageQuality").at("max_leakage2");
    }
    if(config.at("ShowerProcessor").at("min_multiplicity") != nullptr)
    {
        min_multiplicity = config.at("ShowerProcessor").at("min_multiplicity");
    }
}


