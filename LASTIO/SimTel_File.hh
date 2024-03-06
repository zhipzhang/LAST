#include "spdlog/spdlog.h"
#ifndef _SIMTEL_HEADER
#define _SIMTEL_HEADER 1
#include <cstdint>
#include <string>

#include "EventIO_File.hh"
#include "io_hess.h"
using string = std::string;
struct bunch {
  float photons; /**< Number of photons in bunch */
  float x, y;    /**< Arrival position relative to telescope (cm) */
  float cx, cy;  /**< Direction cosines of photon direction */
  float ctime;   /**< Arrival time (ns) */
  float zem;     /**< Height of emission point above sea level (cm) */
  float lambda;  /**< Wavelength in nanometers or 0 */
};
namespace LAST_IO {

class SimTelIO : public EventIO_Object {
 public:
  int rc = 0;
  int itel;
  int tel_id = 0;
  int max_pixels = H_MAX_PIX;
  int max_pixel_sector = 0; // maximum number of pixels in a sector
  int num_events = 0;
  int max_tel;
  AllHessData *hsdata = nullptr;
  int g_tel_idx[3][1 + H_MAX_TEL];
  int g_tel_idx_init[3];
  int g_tel_idx_ref = 0;

  void get_time_blob(HTime *t) {
    t->seconds = get_long();
    t->nanoseconds = get_long();
  }
  void reset_htime(HTime *t) { t->seconds = t->nanoseconds = 0; }
  void adc_reset(AdcData *raw) {
    int ipix, igain;
    //   int is;
    size_t nb;
    if (raw == NULL)
      return;
    raw->known = 0;
    raw->list_known = 0;
    raw->list_size = 0;
    nb = raw->num_samples * sizeof(raw->adc_sample[0][0][0]);
    for (igain = 0; igain < raw->num_gains; igain++) {
      for (ipix = 0; ipix < raw->num_pixels; ipix++) {
        raw->significant[ipix] = 0;
        raw->adc_known[igain][ipix] = 0;
        raw->adc_sum[igain][ipix] = 0;
        memset(&raw->adc_sample[igain][ipix][0], 0, nb);
      }
    }
  }
  int read_hess_runheader();
  int read_hess_camsettings(CameraSettings *cs);
  int read_hess_camorgran(CameraOrganisation *co);
  int read_hess_pixelset(PixelSetting *ps);
  int read_hess_pixeldis();
  int read_hess_camsoftset(CameraSoftSet *cs);
  int read_hess_trackset(TrackingSetup *ts);
  int read_hess_pointingcor(PointingCorrection *pc);
  int read_hess_centralevent(CentralEvent *ce);
  int read_hess_trackevent(TrackEvent *tke);
  int read_hess_televt_head(TelEvent *te);
  int read_hess_teladc_sums(AdcData *raw);
  void get_adcsum_as_uint16(uint32_t *adc_sum, int n);
  void get_adcsum_differential(uint32_t *adc_sum, int n);
  int read_hess_teladc_samples(AdcData *raw, int what);
  int read_hess_aux_trace_digital(AuxTraceD *auxd);
  int read_hess_aux_trace_analog(AuxTraceA *auxa);
  int read_hess_pixeltrg_time(PixelTrgTime *pt);
  int read_hess_pixtime(PixelTiming *pixtm);
  int read_hess_pixcalib(PixelCalibrated *pixcal);
  int read_hess_telimage(ImgData *img);
  int read_hess_televent(TelEvent *te, int what);
  int read_hess_shower(ShowerParameters *sp);
  int read_hess_event(FullEvent *ev, int what);
  int read_hess_calib_event(FullEvent *ev, int what, int *ptype);
  int read_hess_mc_shower(MCShower *mcs);
  int read_hess_mc_event(MCEvent *mce);
  int read_hess_mc_pe_sum(MCpeSum *mcpes);
  int read_hess_mc_pixel_moni(MCPixelMonitor *mcpixmon);
  int read_hess_tel_monitor(TelMoniData *mon);
  int read_hess_laser_calib(LasCalData *lcd);
  int read_hess_run_stat(RunStat *rs);
  int read_hess_mc_run_stat(MCRunStat *mcrs);
  int read_hess_mc_phot(MCEvent *mce);
  int read_photo_electrons(int max_pixel, int max_pe, int *array, int *tel,
                           int *npe, int *pixels, int *flags, int *pe_counts,
                           int *tstart, double *t, double *a,
                           int *photon_counts);
  int read_tel_photons(int max_bunches, int *array, int *tel, double *photons,
                       struct bunch *bunches, int *nbunches);
  int read_hess_pixel_list(PixelList *pl, int *telescope);

  int read_shower_extra_parameters(shower_extra_parameters *ep);
  int begin_read_tel_array(int *array);
  int end_read_tel_array();
  void set_tel_idx(int ntel, int *idx);
  int find_tel_idx(int tel_id);
  void Check();

 public:
  void Read_Runheader();
  void Read_Mc_Event();
  void Read_Calib_Event();
  void Read_Event();
  void Read_MC_Phot();
  void Read_Pointingcor();
  void Read_MC_Shower();
  void Read_Mc_Run_Stat();
  void Read_Run_Stat();
  void Read_Las_Cal();
  void Read_Tel_Moni();
  void Read_MC_Pe_Sum();
  void Read_Trackset();
  void Read_Camsoftset();
  void Read_Pixelset();
  void Read_Camorgran();
  void Read_Camsettings();
  void Read_MC_Pixel_Moni();
  int read_hess_mcrunheader();

  SimTelIO(string fname, unsigned long max_length, string remote_url)
      : EventIO_Object(fname, max_length, remote_url) {
    spdlog::info( "SimTelIO constructor: \n");
  }
  SimTelIO(string fname, unsigned long max_length)
      : EventIO_Object(fname, max_length) {
    spdlog::info("SimTelIO constructor: \n");
  }
  AllHessData *GetHsdata() { return hsdata; }
  ~SimTelIO() {
    if (hsdata != NULL) {
      free(hsdata);
    }
  }
  int GetBlockType() { return (int)item_header->type; }
  bool Have_Block() {
    int rc = find_io_block();
    if (rc == 0) {
      return true;
    } else {
      return false;
    }
  }
  bool Read_Block() {
    int rc = read_io_block();
    if (rc == 0) {
      return true;
    } else {
      spdlog::error("Error Reading block {}", (int)item_header->type);
      return false;
    }
  }
};
}
#endif