// Including  all simulation configuration :
// Shower run configuration  telescope configuration (layout camera and )

#ifndef _LSIMULATION_HH
#define _LSIMULATION_HH

#include "TObject.h"

class LRSimulationRunConfig : public TObject 
{
    public:
        int run_number;
        int corsika_version;
        int simtel_version;
        double energy_range[2];
        double prod_site_B[3];     // 0: Total, 1: declination, 2: inclination
        double prod_site_alt;
        double spectral_index;
        double shower_prog_start;  // Time of the shower simulation
        int    shower_prog_id;     // CORSIKA=1, ALTAI=2, KASCADE=3, MOCCA=4
        double detector_prog_start;
        int    nshowers;
        int    shower_reuse;
        double max_alt;
        double min_alt;
        double max_az;
        double min_az;
        bool   diffuse;
        double max_viewcone_radius;
        double min_viewcone_radius;
        int  atmposphere;
        int  corsika_iact_options;
        int  corsika_low_E_model;
        int  corsika_high_E_model;   
        int  corsika_bunchsize;
        int  corsika_wlen_min;
        int  corsika_wlen_max;

        LRSimulationRunConfig(){};
        ~LRSimulationRunConfig(){}

        ClassDef(LRSimulationRunConfig, 1)

};









#endif