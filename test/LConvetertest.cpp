#include "LREventRaw.hh"
#include "glog/logging.h"
#include "LJsonConfig.hh"

int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    google::SetStderrLogging(google::GLOG_INFO);
    LJsonConfig config(argc, argv);
    LREventRaw* event_raw = new LREventRaw(config);
    while(event_raw->HandleEvent())
    {
    }

    event_raw->StoreTTree();

}

