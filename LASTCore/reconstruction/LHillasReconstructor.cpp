#include "LHillasReconstructor.hh"
#include "Datalevels/LASTDL1/LDL1bEvent.hh"
#include "Datalevels/LASTDL1/LDL1bTelEvent.hh"
#include "Image/LHillasParameters.hh"
#include "TMath.h"
#include "TMatrixTSparse.h"
#include "reconstruction/LHillasGeometryReconstructor.hh"
#include <algorithm>
#include <memory>
#include <utility>
#include "eigen3/Eigen/Dense"

void LHillasReconstructor::AddTelPoint(int tel_id, double tel_alt, double tel_az)
{
    tel_pointing_direction[tel_id] = std::make_pair(tel_az, tel_alt);
}
void LHillasReconstructor::SetRecTels(const LDL1Event& dl1event)
{
    reconstruct_tel.clear();
    tel_pointing_direction.clear();
    for(auto itel: dl1event.GetTelList())
    {
        if(cmd_config.StereoQuery(dl1event[itel]))
        {
            reconstruct_tel.push_back(itel);
            AddTelPoint(itel, dl1event[itel].tel_alt, dl1event[itel].tel_az);
        }
    }

}
void LHillasReconstructor::SetTiltedPos()
{
    tiled_tel_pos.clear();
    get_shower_trans_matrix(subarray_pointing_direction.first, subarray_pointing_direction.second, trans);
    for (auto itel: reconstruct_tel)
    {
        double xt, yt;
        xt = trans[0][0] * (*tel_config)[itel]->pos[0] +
             trans[0][1] * (*tel_config)[itel]->pos[1] +
             trans[0][2] * (*tel_config)[itel]->pos[2];
        yt = trans[1][0] * (*tel_config)[itel]->pos[0] +
             trans[1][1] * (*tel_config)[itel]->pos[1] + 
             trans[1][2] * (*tel_config)[itel]->pos[2];
        tiled_tel_pos[itel] = std::make_pair(xt, yt);
    }

}

bool LHillasReconstructor::ProcessEvent(const LDL1Event& dl1event, LDL1bEvent& dl1bevent)
{
    hillas_dict.Clear();
    SetRecTels(dl1event);
    SetPointing(dl1event);
    SetTiltedPos();
    if(reconstruct_tel.size() < 2)
    {
        return false;
    }
    for (auto itel: reconstruct_tel)
    {
        hillas_dict.AddTel(itel, dl1event[itel].transform_hillas(tel_pointing_direction[itel], subarray_pointing_direction));
    }
    Direction_Reconstruction(dl1bevent);
    Core_Reconstruction(dl1bevent);
    for(auto itel: reconstruct_tel)
    {
        auto dl1_tel_event = std::make_shared<LRDL1bTelEvent>();
        dl1_tel_event->SetTelHillas(dl1event[itel].GetEventID(), dl1event[itel].GetTelID(), hillas_dict[itel]);
        dl1_tel_event->CopyTelInfo(dl1event[itel]);
    }
    return true;
    
}
void LHillasReconstructor::Direction_Reconstruction(LDL1bEvent& ldl1bevent)
{
    std::vector<double> sx_weight;
    std::vector<double> sy_weight;
    std::vector<double> weight;
    double tmp_x, tmp_y, tmp_ang;
    for(auto itel: reconstruct_tel)
    {
        for(auto jtel: reconstruct_tel)
        {
            if(itel == jtel)
            {
                continue;
            }
            if(intersect_lines(hillas_dict[itel].GetCogx(), hillas_dict[itel].GetCogy(), hillas_dict[itel].GetPsi(), 
                hillas_dict[jtel].GetCogx(), hillas_dict[jtel].GetCogy(), hillas_dict[jtel].GetPsi(), &tmp_x, &tmp_y, &tmp_ang) == 1)
            {
                sx_weight.push_back(tmp_x);
                sy_weight.push_back(tmp_y);
                double scale_size = (hillas_dict[itel].GetSize() * hillas_dict[jtel].GetSize())/(hillas_dict[itel].GetSize() + hillas_dict[jtel].GetSize());
                double L_W_ratio  = (1 - hillas_dict[itel].GetWidth()/hillas_dict[itel].GetLength()) * (1 - hillas_dict[jtel].GetWidth()/hillas_dict[jtel].GetLength());
                weight.push_back(pow(scale_size * L_W_ratio * sin(tmp_ang), 2));
            }

        }
    }
    double rec_x, rec_y = 0;
    double rec_x_uncertainty, rec_y_uncertainty = 0;
    auto eigen_sx = Eigen::VectorXd::Map(sx_weight.data(), sx_weight.size());
    auto eigen_sy = Eigen::VectorXd::Map(sy_weight.data(), sy_weight.size());
    auto eigen_weight = Eigen::VectorXd::Map(weight.data(), weight.size());
    rec_x = eigen_weight.dot(eigen_sx)/eigen_weight.sum();
    rec_y = eigen_weight.dot(eigen_sy)/eigen_weight.sum();

    Eigen::VectorXd x_variance_weight = (eigen_sx.array() - rec_x).square();
    Eigen::VectorXd y_variance_weight = (eigen_sy.array() - rec_y).square();

    rec_x_uncertainty = (eigen_weight.dot(x_variance_weight)/eigen_weight.sum());
    rec_y_uncertainty = (eigen_weight.dot(y_variance_weight)/eigen_weight.sum());
    double rec_az, rec_alt = 0;
    LHillasParameters::offset_to_angles(rec_x, rec_y, subarray_pointing_direction.first, subarray_pointing_direction.second, 1, rec_az, rec_alt);
    ldl1bevent.SetRecDirection(rec_az, rec_alt, rec_x_uncertainty, rec_y_uncertainty);
}

void LHillasReconstructor::Core_Reconstruction(LDL1bEvent& ldl1bevent)
{
    std::vector<double> sx_weight;
    std::vector<double> sy_weight;
    std::vector<double> weight;
    double tmp_x, tmp_y, tmp_ang;
    for(auto itel: reconstruct_tel)
    {
        for(auto jtel: reconstruct_tel)
        {
            if(itel == jtel)
            {
                continue;
            }
            if(intersect_lines(tiled_tel_pos[itel].first, tiled_tel_pos[itel].second, hillas_dict[itel].GetPsi(), 
                tiled_tel_pos[jtel].first, tiled_tel_pos[jtel].second, hillas_dict[jtel].GetPsi(), &tmp_x, &tmp_y, &tmp_ang) == 1)
            {
                sx_weight.push_back(tmp_x);
                sy_weight.push_back(tmp_y);
                double scale_size = (hillas_dict[itel].GetSize() * hillas_dict[jtel].GetSize())/(hillas_dict[itel].GetSize() + hillas_dict[jtel].GetSize());
                double L_W_ratio  = (1 - hillas_dict[itel].GetWidth()/hillas_dict[itel].GetLength()) * (1 - hillas_dict[jtel].GetWidth()/hillas_dict[jtel].GetLength());
                weight.push_back(pow(scale_size * L_W_ratio * sin(tmp_ang), 2));
            }
        }
    }
    double x, y = 0;
    double x_uncertainty, y_uncertainty = 0;
    auto eigen_sx = Eigen::VectorXd::Map(sx_weight.data(), sx_weight.size());
    auto eigen_sy = Eigen::VectorXd::Map(sy_weight.data(), sy_weight.size());
    auto eigen_weight = Eigen::VectorXd::Map(weight.data(), weight.size());
    x = eigen_weight.dot(eigen_sx)/eigen_weight.sum();
    y = eigen_weight.dot(eigen_sy)/eigen_weight.sum();

    Eigen::VectorXd x_variance_weight = (eigen_sx.array() - x).square();
    Eigen::VectorXd y_variance_weight = (eigen_sy.array() - y).square();

    x_uncertainty = (eigen_weight.dot(x_variance_weight)/eigen_weight.sum());
    y_uncertainty = (eigen_weight.dot(y_variance_weight)/eigen_weight.sum());
    
    // transform back to the original coordinate system
    double xh = trans[0][0] * x + trans[1][0] * y;
    double yh = trans[0][1] * x + trans[1][1] * y;
    double zh = trans[0][2] * x + trans[1][2] * y;
    double core_x, core_y;
    project_to_ground(xh, yh, zh, core_x, core_y);
    ldl1bevent.SetRecCore(core_x, core_y, x, y, x_uncertainty, y_uncertainty);

}