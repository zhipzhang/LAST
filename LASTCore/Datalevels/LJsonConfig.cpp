#include "gflags/gflags.h"
#include "LJsonConfig.hh"
#include <fstream>
DEFINE_string(input_file, "", "Comma-separated list of input file PATHs");
DEFINE_string(output_file, "dst.root", "Output root file PATH");
DEFINE_string(json_path, "../config.json", "Json file path");
DEFINE_string(only_telescope,"","Comma-separated llist of telescope ids");


void LJsonConfig::ParseCommandLineFlags(int argc, char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    json_fname = FLAGS_json_path;
    output_fname = FLAGS_output_file;
    input_fname = FLAGS_input_file; // keep the one file interface
    input_fnames = splitString(FLAGS_input_file, ',');
    only_telescopes = splitStringToInt(FLAGS_only_telescope, ',');
}

void LJsonConfig::ReadConfiguration()
{
    std::ifstream f(json_fname);
    auto config = nlohmann::json::parse(f);
    max_io_length = config.at("MAX_IO_LENGTH");
    camera_name = config.at("Camera_Name");
}


