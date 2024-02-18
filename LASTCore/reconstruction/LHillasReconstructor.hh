#include "Datalevels/LASTDL1/LDL1TelEvent.hh"
#include "Datalevels/LASTDL1/LDL1bEvent.hh"
#include "Datalevels/LDataBase.hh"
#include "Datalevels/LShower.hh"
#include "Datalevels/LTelescopesTemplate.hh"
#include <unordered_map>
#include <utility>
#include <vector>
#include "Image/LHillasParameters.hh"
#include "reconstruction/LHillasGeometryReconstructor.hh"
#include <memory>
#include "Datalevels/LASTDL1/LDL1Event.hh"

class  LHillasReconstructor: public LHillasGeometryReconstructor
{
    public: 
        LHillasReconstructor();
        LHillasReconstructor(const LJsonConfig& config): LHillasGeometryReconstructor(config){};
        virtual ~LHillasReconstructor(){};
        bool ProcessEvent(const LDL1Event& dl1event, LDL1bEvent& dl1bevent);                            // process the event
        void Init(const LDataBase& dl1event)
        {
            SetTelConfig(dl1event);
        }
    private:
        void SetRecTels(const LDL1Event& dl1event);
        void SetPointing(const LDL1Event& dl1event)
        {
            subarray_pointing_direction = std::make_pair(dl1event.GetPointingAz(), dl1event.GetPointingAlt());
        }
        inline void SetTiltedPos();
        inline void AddTelPoint(int tel_id, double tel_alt, double tel_az);
        std::vector<int> reconstruct_tel;       // telescopes pass the quality check
        std::unordered_map<int, std::pair<double, double >> tel_pointing_direction;
        std::pair<double, double> subarray_pointing_direction;
        std::unordered_map<int, std::pair<double, double>> tiled_tel_pos;
        LTelescopes<LHillasParameters> hillas_dict;
        void Direction_Reconstruction(LDL1bEvent& ldl1bevent);
        void Core_Reconstruction(LDL1bEvent& ldl1bevent);
        void SetImpactParameters(LRDL1TelEvent& ldl1btelevent, const LShower& shower);
        double trans[3][3];
        static  inline int intersect_lines (double xp1, double yp1, double phi1,
        double xp2, double yp2, double phi2, double *xs, double *ys, double *sang)
        {
            double A1, B1, C1;
            double A2, B2, C2;
            double detAB, detBC, detCA;
            double s1, c1, s2, c2;
            
            /* Hesse normal form for line 1 */
            s1 = sin(phi1);
            c1 = cos(phi1);
            A1 = s1;
            B1 = -c1;
            C1 = yp1*c1 - xp1*s1;

            /* Hesse normal form for line 2 */
            s2 = sin(phi2);
            c2 = cos(phi2);
            A2 = s2;
            B2 = -c2;
            C2 = yp2*c2 - xp2*s2;

            detAB = (A1*B2-A2*B1);
            detBC = (B1*C2-B2*C1);
            detCA = (C1*A2-C2*A1);
            
            if ( fabs(detAB) < 1e-14 ) /* parallel */
            {
                if ( sang )
                    *sang = 0.;
                if ( fabs(detBC) < 1e-14 && fabs(detCA) < 1e-14 ) /* same lines */
                {
                    /* We could take any point on the line but use the middle here. */
                    *xs = 0.5*(xp1+xp2);
                    *ys = 0.5*(yp1+yp2);
                    return 2;
                }
                *xs = *ys = 0.;
                return 0;
            }
            
            *xs = detBC / detAB;
            *ys = detCA / detAB;
            
            if ( sang != NULL )
            {
                double dx1 = (*xs-xp1);
                double dx2 = (*xs-xp2);
                double dy1 = (*ys-yp1);
                double dy2 = (*ys-yp2);
                double dr1 = sqrt(dx1*dx1+dy1*dy1);
                double dr2 = sqrt(dx2*dx2+dy2*dy2);
                double cos_ang;
                if ( dr1*dr2 == 0. )
                    *sang = 0.;
                else
                {
                    cos_ang = (dx1*dx2+dy1*dy2) / (dr1*dr2);
                    if ( cos_ang >= 1. )
                        *sang = 0.;
                    else if ( cos_ang <= -1. )
                        *sang = M_PI;
                    else
                        *sang = acos(cos_ang);
                }
            }
            
            return 1;
        }
        static inline void get_shower_trans_matrix (double azimuth, double altitude, double trans[][3])
        {
            double cos_z = sin(altitude);
            double sin_z = cos(altitude);
            double cos_az = cos(azimuth);
            double sin_az = sin(azimuth);
            
            trans[0][0] = cos_z*cos_az;
            trans[1][0] = sin_az;
            trans[2][0] = sin_z*cos_az;
            
            trans[0][1] = -cos_z*sin_az;
            trans[1][1] = cos_az;
            trans[2][1] = -sin_z*sin_az;
            
            trans[0][2] = -sin_z;
            trans[1][2] = 0.;
            trans[2][2] = cos_z;
        }
        inline void project_to_ground (double x, double y, double z, double& x_projected, double& y_projected)
        {
            x_projected = x - trans[2][0] * z / trans[2][2];
            y_projected = y - trans[2][1] * z / trans[2][2];
        }
};