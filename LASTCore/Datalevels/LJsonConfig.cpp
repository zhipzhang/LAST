#include "gflags/gflags.h"
#include "LJsonConfig.hh"
#include <fstream>
DEFINE_string(input_file, "", "Input_file path (eventio format for Raw , ROOT Format for others)");
DEFINE_string(output_file, "dst.root", "Output root file PATH");
DEFINE_string(json_path, "../config.json", "Json file path");

void LJsonConfig::ParseCommandLineFlags(int argc, char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    json_fname = FLAGS_json_path;
    output_fname = FLAGS_output_file;
    input_fname = FLAGS_input_file;

}

void LJsonConfig::ReadConfiguration()
{
    std::ifstream f(json_fname);
    auto config = nlohmann::json::parse(f);
    max_io_length = config.at("MAX_IO_LENGTH");
    camera_name = config.at("Camera_Name");
}


