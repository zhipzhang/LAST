#include "Datalevels/LASTRAW/LEventRaw.hh"
#include "glog/logging.h"

int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    google::SetStderrLogging(google::GLOG_INFO);
    std::string input_fname = "/data/home/zhipz/MyCode/LAST/test/test.io";
    LEventRaw event_raw(input_fname);
    event_raw.ProcessEvent();
    event_raw.GetEvent();
    event_raw.ReadRunConfig();
    event_raw.ReadTelConfig();
    return 0;
}