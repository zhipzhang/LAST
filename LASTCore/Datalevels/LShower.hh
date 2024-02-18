#ifndef _LSHOWER_HH
#define _LSHOWER_HH

#include "Rtypes.h"
#include "TObject.h"

/*
    Simulated shower parameters
*/
class LShower
{
    public:
    double energy;
    double altitude;
    double azimuth;
    double core_x;
    double core_y;
    double h_first_int;
    double x_max;
    int    shower_primary_id;
    int    obs_id;
    int    event_id;
    double array_point_az;
    double array_point_alt;
    LShower(){};
    virtual ~LShower(){};
    ClassDef(LShower, 2)
};

class LRShower:public LShower, public TObject
{  
    public:
        LRShower():LShower(){}
        virtual ~LRShower(){};
    ClassDef(LRShower, 0);
};

class LRArray: public LShower, public TObject
{
    public:
        LRArray():LShower(){}
        virtual ~LRArray(){Clear();};
        std::vector<int> trigger_tels;
        void Clear()
        {
            trigger_tels.clear();
        }
        void AddTel(int itel)
        {
            trigger_tels.push_back(itel);
        }
        int GetTrigNums() const {return trigger_tels.size();}
        std::vector<int> GetTelList() {return trigger_tels;}
        int GetEventID() {return event_id;}
    ClassDef(LRArray, 2);
};

#endif