


#ifndef _LDL1TelEvent_HH
#define _LDL1TelEvent_HH

#include "Image/LHillasParameters.hh"
#include "RtypesCore.h"
class LRDL1TelEvent : public TObject
{
    protected:
    LHillasParameters hillas;

    // Leakage Containers
    Double32_t leakage1;
    Double32_t leakage1_width;
    Double32_t leakage2;
    Double32_t leakage2_width;
    // Concentration Containers
    Double32_t concentration_cog;
    Double32_t concentration_core;
    Double32_t concentration_pixel;
    // Intensity Containers
    Double32_t intensity_mean;
    Double32_t intensity_std;
    Double32_t intensity_skewness;
    Double32_t intensity_kurtosis;
    // Morphology Containers
    Double32_t morphology_num_islands;
    Double32_t morphology_num_pixels;
    Double32_t morphology_num_large_islands;
    Double32_t morphology_num_medium_islands;
    Double32_t morphology_num_small_islands;

    int tel_id;             // telescope ID
    int event_id;

    public:
    double tel_alt;
    double tel_az;
        LRDL1TelEvent(){}
        LRDL1TelEvent(int tel_id, int event_id)
        {
            this->tel_id = tel_id;
            this->event_id = event_id;
        }
        virtual ~LRDL1TelEvent(){}
    void SetTelHillas(int event_id, int tel_id, LHillasParameters hillas)
    {
        this->event_id = event_id;
        this->tel_id = tel_id;
        this->hillas = hillas;
    }
    void SetLeakage(Double32_t leakage1, Double32_t leakage1_width, Double32_t leakage2, Double32_t leakage2_width)
    {
        this->leakage1 = leakage1/hillas.GetSize();
        this->leakage1_width = leakage1_width;
        this->leakage2 = leakage2/hillas.GetSize();
        this->leakage2_width = leakage2_width;
    }
    double GetCogx() const
    {
        return hillas.GetCogx();
    }
    double GetCogy() const
    {
        return hillas.GetCogy();
    }
    void SetConcentration(Double32_t cog_size, Double32_t core_size, Double32_t max_size)
    {
        this->concentration_cog = cog_size/hillas.GetSize();
        this->concentration_core = core_size/hillas.GetSize();
        this->concentration_pixel = max_size/hillas.GetSize();
    }
    void SetIntensity(Double32_t mean, Double32_t std, Double32_t skewness, Double32_t kurtosis)
    {
        this->intensity_mean = mean;
        this->intensity_std = std;
        this->intensity_skewness = skewness;
        this->intensity_kurtosis = kurtosis;
    }
    void SetMorphology(Double32_t num_islands, Double32_t num_pixels, Double32_t num_large_islands, Double32_t num_medium_islands, Double32_t num_small_islands)
    {
        this->morphology_num_islands = num_islands;
        this->morphology_num_pixels = num_pixels;
        this->morphology_num_large_islands = num_large_islands;
        this->morphology_num_medium_islands = num_medium_islands;
        this->morphology_num_small_islands = num_small_islands;
    }
    LHillasParameters transform_hillas(const std::pair<double,double>& tel_pointing_direction, const std::pair<double, double>& subarray_pointing_direction ) const
    {
        return hillas.transform_frame(tel_pointing_direction,  subarray_pointing_direction);
    }
    int GetEventID() const
    {
        return event_id;
    }
    int GetTelID() const
    {
        return tel_id;
    }
    void CopyMorphology(const LRDL1TelEvent& dl1_tel_event)
    {
        this->morphology_num_islands = dl1_tel_event.morphology_num_islands;
        this->morphology_num_pixels = dl1_tel_event.morphology_num_pixels;
        this->morphology_num_large_islands = dl1_tel_event.morphology_num_large_islands;
        this->morphology_num_medium_islands = dl1_tel_event.morphology_num_medium_islands;
        this->morphology_num_small_islands = dl1_tel_event.morphology_num_small_islands;
    }
    void CopyIntensity(const LRDL1TelEvent& dl1_tel_event)
    {
        this->intensity_mean = dl1_tel_event.intensity_mean;
        this->intensity_std = dl1_tel_event.intensity_std;
        this->intensity_skewness = dl1_tel_event.intensity_skewness;
        this->intensity_kurtosis = dl1_tel_event.intensity_kurtosis;
    }
    void CopyLeakage(const LRDL1TelEvent& dl1_tel_event)
    {
        this->leakage1 = dl1_tel_event.leakage1;
        this->leakage1_width = dl1_tel_event.leakage1_width;
        this->leakage2 = dl1_tel_event.leakage2;
        this->leakage2_width = dl1_tel_event.leakage2_width;
    }
    void CopyConcentration(const LRDL1TelEvent& dl1_tel_event)
    {
        this->concentration_cog = dl1_tel_event.concentration_cog;
        this->concentration_core = dl1_tel_event.concentration_core;
        this->concentration_pixel = dl1_tel_event.concentration_pixel;
    }
    ClassDef(LRDL1TelEvent, 1)

};










#endif