#include "Datalevels/LASTDL0/LRDL0Event.hh"
#include "Datalevels/LASTDL1/LRDL1Event.hh"
#include "Datalevels/LASTConverter/LImageProcessor.hh"
#include "Datalevels/LJsonConfig.hh"
#include "glog/logging.h"



int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    google::SetStderrLogging(google::GLOG_INFO);
    LJsonConfig config(argc, argv);
    auto lastdl0 = new  LRDL0Event(config);
    auto lastdl1 = new LRDL1Event(config, 'w');
    auto last_image_processor = new LImageProcessor();
    lastdl0->ReadROOTFile(config.GetInputFileName());
    last_image_processor->InitGeometry(*(lastdl0->GetTelescopesConfig()));
    lastdl1->Read(lastdl0->GetRootFile());
    //lastdl1->CopyEvent();
    while(lastdl0->ReadEvent())
    {
        last_image_processor->ParameterizationImage(*lastdl0, *lastdl1);
        lastdl1->HandleEvent();
    }
    lastdl1->StoreTTree();
}