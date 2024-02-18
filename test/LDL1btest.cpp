#include "Datalevels/LASTDL1/LRDL1bEvent.hh"
#include "reconstruction/LHillasReconstructor.hh"
#include "Datalevels/LJsonConfig.hh"
#include "glog/logging.h"
#include "Datalevels/LASTDL1/LRDL1Event.hh"


int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    google::SetStderrLogging(google::GLOG_INFO);
    LJsonConfig config(argc, argv);

    auto lasdtdl1 = new LRDL1Event(config, 'r');
    auto lastdl1b = new LRDL1bEvent(config, 'w');
    auto last_hillas_reconstructor = new LHillasReconstructor(config);
    last_hillas_reconstructor->Init(*lasdtdl1);
    while(lasdtdl1->ReadEvent())
    {
        last_hillas_reconstructor->ProcessEvent(*lasdtdl1, *lastdl1b);
        lastdl1b->HandleEvent();
    }
    lastdl1b->StoreTTree();

}