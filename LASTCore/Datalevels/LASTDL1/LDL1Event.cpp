#include "LDL1Event.hh"
#include "LDL1TelEvent.hh"
#include "TCanvas.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <root/TMath.h>
#include <unordered_map>
#include "TPaveText.h"
#include "TEllipse.h"


LDL1Event::LDL1Event(): LDataBase()
{
    ldl1event = std::make_shared<LTelescopes<std::shared_ptr<LRDL1TelEvent>>>();
    ldl1array = new LRArray();
    dl1_tel_event = new LRDL1TelEvent();
};


LDL1Event::~LDL1Event()
{
    /*
    delete dl1_tel_event;
    delete ldl1array;
    */
};
void LDL1Event::SetTelHillas(int event_id, int tel_id, LHillasParameters hillas)
{
    auto itel_dl1event = std::make_shared<LRDL1TelEvent>();
    itel_dl1event->SetTelHillas(event_id, tel_id, hillas);
}

void LDL1Event::AddTelEvent(int tel_id, LRDL1TelEvent* dl1televent)
{
    auto itel_event = std::make_shared<LRDL1TelEvent>();
    *itel_event = *dl1televent;
    ldl1event->AddTel(tel_id, itel_event);
}
void LDL1Event::FilterTelescope(const std::vector<int> tels)
{
  std::unordered_map<int, bool> telescope_list;
  for(auto itel: tels)
  {
    telescope_list[itel] = true;
  }
  for(int i = ldl1event->GetTelNum() - 1; i >= 0 ; i--)
  {
    int tel_id = ldl1event->GetKeys()[i];
    if(!telescope_list[tel_id])
    {
        ldl1event->DeleteTel(tel_id, i);
        ldl1array->DeleteTel(i);
    }
  }
}
void LDL1Event::Close()
{
    Clear();
    LDataBase::Close();
}

void LDL1Event::Display(std::vector<TCanvas *> &canvases)
{
  for(auto itel:ldl1event->GetKeys())
  {
        TCanvas* canvas = new TCanvas(Form("Camera_Image_%d", itel), "LACT RAW Image", 1800, 1800);
        TH2Poly* camera = new TH2Poly("camera_bin", "", -6, 6, -6, 6);
        camera->SetMinimum(0.1);
        camera->SetStats(0);
        auto tel_event = (*ldl1event)[itel];
        if(tel_cleaned_pe[itel].size() == 0)
        {
          spdlog::warn("No cleaned pe for telescope {}", itel);
          continue;
        }
        LDataBase::Fill2Poly<double>(camera, itel, &tel_cleaned_pe[itel][0]);
        camera->Draw("colz");
        TEllipse* ellipse = new TEllipse(tel_event->GetCogx()* TMath::RadToDeg(), tel_event->GetCogy() * TMath::RadToDeg(), 
                                        tel_event->GetLength() * TMath::RadToDeg(), tel_event->GetWidth() * TMath::RadToDeg(), 0, 360, tel_event->GetPsi() * TMath::RadToDeg());
        ellipse->SetLineWidth(3);
	      ellipse->SetLineColor(2);
	      ellipse->SetFillStyle(0);
	      ellipse->Draw("same");
        TPaveText *pavet = new TPaveText(-6, 6.3, 6, 7.6);

        pavet->SetFillStyle(0);
        pavet->AddText(Form("Event ID: %d, Telescope ID: %d Image Size: %.2lf", tel_event->GetEventID(), tel_event->GetTelID(), tel_event->GetSize()));
        pavet->Draw("same");
        
        canvas->SaveAs(Form("Event_%d_Tel_%d_dl1.png", tel_event->GetEventID(), tel_event->GetTelID()));

  }
}