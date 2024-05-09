

#ifndef _LDL1Event_HH
#define _LDL1Event_HH
#include "../LDataBase.hh"
#include <memory>
#include <unordered_map>
#include "../LTelescopesTemplate.hh"
#include "../LShower.hh"
#include "LHillasParameters.hh"
#include "LDL1TelEvent.hh"
#include "TCanvas.h"

class LDL1Event : public LDataBase
{
    public:
        LDL1Event();
        virtual ~LDL1Event();
        void SetTelHillas(int event_id, int tel_id, LHillasParameters hillas);
        void AddTelEvent(int tel_id, std::shared_ptr<LRDL1TelEvent> dl1televent) { ldl1array->AddTel(tel_id);ldl1event->AddTel(tel_id, dl1televent);};
        void AddTelEvent(int tel_id, LRDL1TelEvent*);
        void Clear(){ldl1event->Clear(); ldl1array->Clear(); tel_cleaned_pe.clear();};
        LRDL1TelEvent& operator[](int tel_id) {return *((*ldl1event)[tel_id]);};
        const LRDL1TelEvent& operator[](int tel_id) const {return *((*ldl1event)[tel_id]);};
        std::vector<int> GetTelList() const {return ldl1array->GetTelList();};
        double GetPointingAz() const {return ldl1array->array_point_az;};
        double GetPointingAlt() const {return ldl1array->array_point_alt;};
        const LRArray& GetEventArrayInfo() const {return *ldl1array;};
        LShower& GetEventArrayInfo() {return *ldl1array;};
        void Close();
        double GetMCxcore() const
        {
            return ldl1array->core_x;
        }
        double GetMCycore() const
        {
            return ldl1array->core_y;
        }
        double GetMCalt() const
        {
            return ldl1array->altitude;
        }
        double GetMCaz() const
        {
            return ldl1array->azimuth;
        }
        double GetMCenergy() const
        {
            return ldl1array->energy;
        }
        void FilterTelescope(const std::vector<int> tels) ;
        bool IsEmpty() const { if(ldl1event->GetTelNum() == 0) return true; return false;}
        void AddTelPe(int tel_id, std::vector<double> pe) {tel_cleaned_pe[tel_id] = pe;};
        void Display(std::vector<TCanvas*>& canvases);
    protected:
        std::shared_ptr<LTelescopes<std::shared_ptr<LRDL1TelEvent>>> ldl1event;
        LRDL1TelEvent* dl1_tel_event;
        
        LRArray* ldl1array;
        std::string output_fname;
        std::unordered_map<int, std::vector<double>> tel_cleaned_pe;
    private:
        static inline double ComputeImpactdistance(double tel_x,double tel_y, double altitude, double azimuth, double x, double y)
        {
            double cx = cos(altitude)*cos(azimuth);
            double cy = -cos(altitude)*sin(azimuth);
            double cz = sin(altitude);
            return line_point_distance(x, y, 0, cx, cy, cz, tel_x, tel_y, 0);
        }
                static inline double line_point_distance (double xp1, double yp1, double zp1, 
                    double cx, double cy, double cz,
                double x, double y, double z)
        {
            double a, a1, a2, a3, b;
    
            a1 = (y-yp1)*cz - (z-zp1)*cy;
            a2 = (z-zp1)*cx - (x-xp1)*cz;
            a3 = (x-xp1)*cy - (y-yp1)*cx;
            a  = a1*a1 + a2*a2 + a3*a3;
            b = cx*cx + cy*cy + cz*cz;
            if ( a<0. || b<= 0. )
                return -1;
            return sqrt(a/b);
        }


    

};







#endif