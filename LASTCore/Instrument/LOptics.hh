

#ifndef  _LOptics_HH_
#define _LOptics_HH_
#include <string>
#include "TString.h"
#include "TObject.h"

enum ReflectorShape{
    PARAVOLIC,
    DAVIES_COTTON,
    SPHERICAL
};
enum SizeType{
    LARGE,
    MEDIUM,
    SMALL
};


class LROpticsDescription :public TObject
{
    public:
    LROpticsDescription();
    LROpticsDescription(std::string m_name, int msize_type, int mreflector_type, double meffective_focal_length, double mequivalent_focal_length, double mmirror_area, int mn_mirror_tiles):name(m_name),size_type(msize_type),reflector_type(mreflector_type),effective_focal_length(meffective_focal_length),equivalent_focal_length(mequivalent_focal_length),mirror_area(mmirror_area),n_mirror_tiles(mn_mirror_tiles){};
    TString name;
    int size_type;                     // 0 for large, 1 for medium, 2 for small
    int reflector_type;               // 0 for parabolic, 1 for davies-cotton, 2 for spherical
    double effective_focal_length;
    double equivalent_focal_length;
    double mirror_area;
    int n_mirror_tiles;
    void PrintMirror();

    ClassDef(LROpticsDescription, 1)
};
#endif