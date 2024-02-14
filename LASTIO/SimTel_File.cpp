#include "SimTel_File.hh"
#include "EventIO_File.hh"
#include "glog/logging.h"
#include "io_hess.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cassert>

int LAST_IO::SimTelIO::read_hess_runheader() {
  RunHeader *rh = &hsdata->run_header;
  if (buffer == (BYTE *)NULL || (&hsdata->run_header) == NULL) {
    LOG(ERROR) << "buffer is not allocated or hsdata is not allocated";
    return -1;
  }
  item_header->type = IO_TYPE_HESS_RUNHEADER;
  int rc;

  if ((rc = get_item_begin()) < 0) {
    return rc;
  }
  if (item_header->version > 2) {
    /*
    Errir RToe
    */
    get_item_end();
    return -1;
  }
  rh->run = get_int32();
  rh->time = get_long();
  rh->run_type = get_int32();
  rh->tracking_mode = get_int32();
  if (item_header->version >= 2) {
    rh->reverse_flag = get_int32();
  } else {
    rh->reverse_flag = 0;
  }

  get_vector_of_real(rh->direction, 2);
  get_vector_of_real(rh->offset_fov, 2);
  rh->conv_depth = get_real();
  if (item_header->version >= 1) {
    get_vector_of_real(rh->conv_ref_pos, 2);
  } else {
    rh->conv_ref_pos[0] = rh->conv_ref_pos[1] = 0.0;
  }
  rh->ntel = get_int32();
  get_vector_of_int(rh->tel_id, rh->ntel);
  set_tel_idx(rh->ntel, rh->tel_id);

  get_vector_of_real(&rh->tel_pos[0][0], 3 * rh->ntel);
  rh->min_tel_trig = get_int32();
  rh->duration = get_int32();

  /*
      The Below codes are not understand now.
      The meaning of target  :
      The meaning of observer:
  */
  char line[1024];
  get_string(line, sizeof(line) - 1);
  if (rh->target != NULL && rh->max_len_target > 0)
    strncpy(rh->target, line, rh->max_len_target);
  else {
    int l = strlen(line);
    rh->max_len_target = 0;
    if (rh->target != NULL)
      free(rh->target);
    if ((rh->target = (char *)malloc(l + 1)) != NULL)
      strcpy(rh->target, line);
  }

  get_string(line, sizeof(line) - 1);
  if (rh->observer != NULL && rh->max_len_observer > 0)
    strncpy(rh->observer, line, rh->max_len_observer);
  else {
    int l = strlen(line);
    rh->max_len_observer = 0;
    if (rh->observer != NULL)
      free(rh->observer);
    if ((rh->observer = (char *)malloc(l + 1)) != NULL)
      strcpy(rh->observer, line);
  }

  return get_item_end();
};

void LAST_IO::SimTelIO::Read_Runheader() {
  if (hsdata != NULL) {
    /*
    It's annoying to use c struct (no deconstructor)
    */
    for (itel = 0; itel < hsdata->run_header.ntel; itel++) {
      if (hsdata->event.teldata[itel].raw != NULL) {
        free(hsdata->event.teldata[itel].raw);
        hsdata->event.teldata[itel].raw = NULL;
      }
      if (hsdata->event.teldata[itel].pixtm != NULL) {
        free(hsdata->event.teldata[itel].pixtm);
        hsdata->event.teldata[itel].pixtm = NULL;
      }
      if (hsdata->event.teldata[itel].img != NULL) {
        free(hsdata->event.teldata[itel].img);
        hsdata->event.teldata[itel].img = NULL;
      }
    }
    /* Free main structure */
    free(hsdata);
    hsdata = NULL;
  }

  hsdata = (AllHessData *)calloc(1, sizeof(AllHessData));
  if ((rc = read_hess_runheader()) < 0) {
    /*
    Error
    */
  }
  for (itel = 0; itel < hsdata->run_header.ntel; itel++) {
    tel_id = hsdata->run_header.tel_id[itel];
    hsdata->camera_set[itel].tel_id = tel_id;
    std::cout << "\t initialize Telescope ID " << tel_id << " (is telescope # "
              << itel << ")" << std::endl;
    hsdata->camera_org[itel].tel_id = tel_id;
    hsdata->pixel_set[itel].tel_id = tel_id;
    hsdata->pixel_disabled[itel].tel_id = tel_id;
    hsdata->cam_soft_set[itel].tel_id = tel_id;
    hsdata->tracking_set[itel].tel_id = tel_id;
    hsdata->point_cor[itel].tel_id = tel_id;
    hsdata->event.num_tel = hsdata->run_header.ntel;
    hsdata->event.teldata[itel].tel_id = tel_id;
    hsdata->event.trackdata[itel].tel_id = tel_id;
    if ((hsdata->event.teldata[itel].raw =
             (AdcData *)calloc(1, sizeof(AdcData))) == NULL) {
      std::cout << "Not enough memory" << std::endl;
      exit(EXIT_FAILURE);
    }
    hsdata->event.teldata[itel].raw->tel_id = tel_id;
    if ((hsdata->event.teldata[itel].pixtm =
             (PixelTiming *)calloc(1, sizeof(PixelTiming))) == NULL) {
      std::cout << "Not enough memory" << std::endl;
      exit(EXIT_FAILURE);
    }
    hsdata->event.teldata[itel].pixtm->tel_id = tel_id;
    if ((hsdata->event.teldata[itel].img =
             (ImgData *)calloc(2, sizeof(ImgData))) == NULL) {
      std::cout << "Not enough memory" << std::endl;
      exit(EXIT_FAILURE);
    }
    hsdata->event.teldata[itel].max_image_sets = 2;
    hsdata->event.teldata[itel].img[0].tel_id = tel_id;
    hsdata->event.teldata[itel].img[1].tel_id = tel_id;
    hsdata->tel_moni[itel].tel_id = tel_id;
    hsdata->tel_lascal[itel].tel_id = tel_id;
  }
}
int LAST_IO::SimTelIO::read_hess_mcrunheader() {
  int rc;
  MCRunHeader *mcrh = &hsdata->mc_run_header;

  if (buffer == (BYTE *)NULL || mcrh == NULL) {
    LOG(ERROR) << "buffer is not allocated or hsdata is not allocated";
    return -1;
  }
  item_header->type = IO_TYPE_HESS_MCRUNHEADER; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 4) {
    fprintf(stderr, "Unsupported MC run header version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  mcrh->shower_prog_id = get_int32();
  mcrh->shower_prog_vers = get_int32();
  if (item_header->version >= 4)
    mcrh->shower_prog_start = get_int32();
  else
    mcrh->shower_prog_start = 0;
  mcrh->detector_prog_id = get_int32();
  mcrh->detector_prog_vers = get_int32();
  if (item_header->version >= 4)
    mcrh->detector_prog_start = get_int32();
  else
    mcrh->detector_prog_start = 0;
  mcrh->obsheight = get_real();
  mcrh->num_showers = get_int32();
  mcrh->num_use = get_int32();
  mcrh->core_pos_mode = get_int32();
  get_vector_of_real(mcrh->core_range, 2);
  get_vector_of_real(mcrh->alt_range, 2);
  get_vector_of_real(mcrh->az_range, 2);
  mcrh->diffuse = get_int32();
  get_vector_of_real(mcrh->viewcone, 2);
  get_vector_of_real(mcrh->E_range, 2);
  mcrh->spectral_index = get_real();
  mcrh->B_total = get_real();
  mcrh->B_inclination = get_real();
  mcrh->B_declination = get_real();
  mcrh->injection_height = get_real();
  mcrh->atmosphere = get_int32();

  if (item_header->version >= 2) {
    mcrh->corsika_iact_options = get_int32();
    mcrh->corsika_low_E_model = get_int32();
    mcrh->corsika_high_E_model = get_int32();
    mcrh->corsika_bunchsize = get_real();
    mcrh->corsika_wlen_min = get_real();
    mcrh->corsika_wlen_max = get_real();
  } else {
    mcrh->corsika_iact_options = mcrh->corsika_low_E_model =
        mcrh->corsika_high_E_model = 0;
    mcrh->corsika_bunchsize = mcrh->corsika_wlen_min = mcrh->corsika_wlen_max =
        0.;
  }

  if (item_header->version >= 3) {
    mcrh->corsika_low_E_detail = get_int32();
    mcrh->corsika_high_E_detail = get_int32();
  } else {
    mcrh->corsika_low_E_detail = 0;
    mcrh->corsika_high_E_detail = 0;
  }

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_Camsettings() {
  tel_id = item_header->ident;
  if ((itel = find_tel_idx(tel_id)) < 0) {
    LOG(ERROR) << "Tel ID " << tel_id << " not found";
  }

  rc = read_hess_camsettings(&hsdata->camera_set[itel]);
  if (rc != 0) {
    /*
     */
  }
}

int LAST_IO::SimTelIO::read_hess_camsettings(CameraSettings *cs) {
  int rc, i;
  if (buffer == (BYTE *)NULL || cs == NULL) {
    LOG(ERROR) << "buffer is not allocated or hsdata is not allocated";
    return -1;
  }
  item_header->type = IO_TYPE_HESS_CAMSETTINGS; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 6) {
    LOG(WARNING) << "Unsupported camera settings version: "
                 << item_header->version;
    /*
    Unsupported
    */
    get_item_end();
    return -1;
  }

  if (cs->tel_id >= 0 && item_header->ident != cs->tel_id) {
    /*
    Warning
    */
    get_item_end();
    return -1;
  }

  cs->num_pixels = get_int32();
  if (cs->num_pixels < 1 || cs->num_pixels > max_pixels) {
    /*
    Warning
    */
    cs->num_pixels = 0;
    get_item_end();
    return -1;
  }
  cs->flen = get_real();
  cs->eff_flen = 0.;
  if (item_header->version > 4)
    cs->eff_flen = cs->eff_flen_x = cs->eff_flen_y = get_real();
  if (item_header->version > 5) {
    cs->eff_flen_x = get_real();
    cs->eff_flen_y = get_real();
    cs->eff_flen_dx = get_real();
    cs->eff_flen_dy = get_real();
  }
  get_vector_of_real(cs->xpix, cs->num_pixels);
  get_vector_of_real(cs->ypix, cs->num_pixels);
  if (item_header->version >= 4) {
    cs->curved_surface = get_scount();
    cs->pixels_parallel = get_scount();
    if (cs->curved_surface) {
      get_vector_of_real(cs->zpix, cs->num_pixels);
    } else {
      for (i = 0; i < cs->num_pixels; i++) {
        cs->zpix[i] = 0.; /* Assume a flat camera */
      }
    }
    if (!cs->pixels_parallel) {
      get_vector_of_real(cs->nxpix, cs->num_pixels);
      get_vector_of_real(cs->nypix, cs->num_pixels);
    } else {
      for (i = 0; i < cs->num_pixels; i++) {
        cs->nxpix[i] = cs->nypix[i] =
            0.; /* Assume looking along optical axis */
      }
    }
    cs->common_pixel_shape = get_scount();
    if (!cs->common_pixel_shape) {
      /* Pixel geometric properties reported individually */
      get_vector_of_int_scount(cs->pixel_shape, cs->num_pixels);
      get_vector_of_real(cs->area, cs->num_pixels);
      get_vector_of_real(cs->size, cs->num_pixels);
    } else {
      /* All pixels of same shape and size */
      double area, size;
      int pixel_shape = get_scount();
      area = get_real();
      size = get_real();
      for (i = 0; i < cs->num_pixels; i++) {
        cs->pixel_shape[i] = pixel_shape;
        cs->area[i] = area;
        cs->size[i] = size;
      }
    }
  } else {
    cs->curved_surface = 0;
    cs->pixels_parallel = 1;
    cs->common_pixel_shape = 0;
    for (i = 0; i < cs->num_pixels; i++) {
      cs->zpix[i] = 0.;                 /* Assume a flat camera, */
      cs->nxpix[i] = cs->nypix[i] = 0.; /* assume looking along optical axis, */
      cs->pixel_shape[i] =
          -1; /* unknown shape (can be derived from pixel positions) */
    }
    get_vector_of_real(cs->area, cs->num_pixels);
    if (item_header->version >= 1)
      get_vector_of_real(cs->size, cs->num_pixels);
    else
      for (i = 0; i < cs->num_pixels; i++)
        cs->size[i] = 0.;
  }

  if (item_header->version >= 2) {
    cs->num_mirrors = get_int32();
    cs->mirror_area = get_real();
  } else {
    cs->num_mirrors = 0;
    cs->mirror_area = 0;
  }

  if (item_header->version >= 3) {
    cs->cam_rot = get_real();
  } else {
    cs->cam_rot = 0.;
  }

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_camorgran(CameraOrganisation *co) {
  int i, j, n;
  int rc;
  int w_psmx = 0, ix;
  if (buffer == (BYTE *)NULL || co == NULL) {
    LOG(ERROR) << "buffer is not allocated or hsdata is not allocated";
    return -1;
  }
  item_header->type = IO_TYPE_HESS_CAMORGAN; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 2) {
    /*

    */
    fflush(stdout);
    fprintf(stderr, "Unsupported camera organisation version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  if (co->tel_id >= 0 && item_header->ident != co->tel_id) {
    /*

    */
    fflush(stdout);
    fprintf(stderr,
            "Expected CameraOrganisation for telescope ID = %d, got %ld\n",
            co->tel_id, item_header->ident);
    // LOG(WARNING)<<"Refusing to copy CameraOrganisation for wrong telescope");
    get_item_end();
    return -1;
  }

  co->num_pixels = get_int32();
  co->num_drawers = get_int32();
  co->num_gains = get_int32();
  if (item_header->version >= 1)
    co->num_sectors = get_int32();
  if (co->num_pixels < 0 || co->num_pixels > H_MAX_PIX || co->num_gains < 0 ||
      co->num_gains > H_MAX_GAINS) {
    fflush(stdout);
    // LOG(WARNING)<<"Data size in CameraOrganisation is invalid.");
    fprintf(stderr, "  num_pixels = %d; allowed: %d\n", co->num_pixels,
            H_MAX_PIX);
    fprintf(stderr, "  num_gains  = %d; allowed: %d\n", co->num_gains,
            H_MAX_GAINS);
    co->num_pixels = co->num_drawers = co->num_gains = 0;
    get_item_end();
    return -1;
  }
  if (item_header->version <= 1) {
    get_vector_of_int(co->drawer, co->num_pixels);
    get_vector_of_int(&co->card[0][0], co->num_pixels * co->num_gains);
    get_vector_of_int(&co->chip[0][0], co->num_pixels * co->num_gains);
    get_vector_of_int(&co->channel[0][0], co->num_pixels * co->num_gains);
    for (i = 0; i < co->num_pixels; i++) {
      n = get_short();
      if (n > H_MAX_PIXSECTORS) {
        if (n > w_psmx) {
          fflush(stdout);
          fprintf(stderr,
                  "There are up to %d trigger groups ('sectors') associated to "
                  "pixels "
                  "but H_MAX_PIXSECTORS=%d. Extra sectors are ignored.\n",
                  n, H_MAX_PIXSECTORS);
          w_psmx = n;
        }
        get_vector_of_int(co->sectors[i], H_MAX_PIXSECTORS);
        for (ix = H_MAX_PIXSECTORS; ix < n; ix++)
          (void)get_short();
        n = H_MAX_PIXSECTORS;
      } else {
        get_vector_of_int(co->sectors[i], n);
      }
      /* Fix for old bug in write_hess_camorgan(): trailing '0' sectors. */
      /* Since the sector list is always ordered a sector number of 0 */
      /* after the first position indicates the end of the list. */
      for (j = 1; j < n; j++)
        if (co->sectors[i][j] == 0) {
          n = j;
          break;
        }
      for (j = n; j < H_MAX_PIXSECTORS; j++)
        co->sectors[i][j] = -1;
      co->nsect[i] = n;
    }
  } else {
    get_vector_of_int_scount(co->drawer, co->num_pixels);
    get_vector_of_int_scount(&co->card[0][0], co->num_pixels * co->num_gains);
    get_vector_of_int_scount(&co->chip[0][0], co->num_pixels * co->num_gains);
    get_vector_of_int_scount(&co->channel[0][0],
                             co->num_pixels * co->num_gains);
    for (i = 0; i < co->num_pixels; i++) {
      n = get_scount32();
      if (n > H_MAX_PIXSECTORS) {
        if (n > w_psmx) {
          fflush(stdout);
          fprintf(stderr,
                  "There are up to %d trigger groups ('sectors') associated to "
                  "pixels "
                  "but H_MAX_PIXSECTORS=%d. Extra sectors are ignored.\n",
                  n, H_MAX_PIXSECTORS);
          w_psmx = n;
        }
        get_vector_of_int_scount(co->sectors[i], H_MAX_PIXSECTORS);
        for (ix = H_MAX_PIXSECTORS; ix < n; ix++)
          (void)get_scount();
        n = H_MAX_PIXSECTORS;
      } else {
        get_vector_of_int_scount(co->sectors[i], n);
      }
      /* Fix for old bug in write_hess_camorgan(): trailing '0' sectors. */
      /* Since the sector list is always ordered a sector number of 0 */
      /* after the first position indicates the end of the list. */
      for (j = 1; j < n; j++)
        if (co->sectors[i][j] == 0) {
          n = j;
          break;
        }
      for (j = n; j < H_MAX_PIXSECTORS; j++)
        co->sectors[i][j] = -1;
      co->nsect[i] = n;
    }
  }

  if (item_header->version >= 1) {
    for (i = 0; i < co->num_sectors && i < H_MAX_SECTORS; i++) {
      co->sector_type[i] = get_byte();
      co->sector_threshold[i] = get_real();
      co->sector_pixthresh[i] = get_real();
    }
    if (co->num_sectors > H_MAX_SECTORS) {
      fflush(stdout);
      fprintf(stderr, "There are %d trigger groups ('sectors') in telescope ID "
                      "%d but only %d are supported.\n",
              co->num_sectors, co->tel_id, H_MAX_SECTORS);
      for (i = H_MAX_SECTORS; i < co->num_sectors; i++) {
        (void)get_byte();
        (void)get_real();
        (void)get_real();
      }
    }
  } else {
    for (i = 0; i < co->num_sectors; i++) {
      co->sector_type[i] = 0;
      co->sector_threshold[i] = 0.;
      co->sector_pixthresh[i] = 0.;
    }
  }

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_Camorgran() {
  tel_id = item_header->ident;
  if ((itel = find_tel_idx(tel_id)) < 0) {
    /*
     */
  }
  rc = read_hess_camorgran(&hsdata->camera_org[itel]);
  if (rc != 0) {
  }
  /*
   */
}

int LAST_IO::SimTelIO::read_hess_pixelset(PixelSetting *ps) {
  int rc;

  if (buffer == (BYTE *)NULL || ps == NULL) {
    LOG(ERROR) << "buffer is not allocated or ps is not allocated";
    return -1;
  }

  item_header->type = IO_TYPE_HESS_PIXELSET; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 3) {
    fprintf(stderr, "Unsupported pixel parameter version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  if (ps->tel_id >= 0 && item_header->ident != ps->tel_id) {
    // LOG(WARNING)<<"Refusing to copy PixelSetting for wrong telescope");
    LOG(WARNING) << "Refusing to copy PixelSetting for wrong telescope";
    get_item_end();
    return -1;
  }

  ps->setup_id = get_int32();
  ps->trigger_mode = get_int32();
  ps->min_pixel_mult = get_int32();

  /* One high voltage value for each pixel */
  ps->num_pixels = get_int32();
  if (ps->num_pixels < 0 || ps->num_pixels > H_MAX_PIX) {
    char message[1024];
    snprintf(message, sizeof(message),
             "Data size in PixelSetting is invalid: You attempt to"
             " read settings for %d pixels but the library was compiled"
             " for a maximum of %d.",
             ps->num_pixels, H_MAX_PIX);
    LOG(WARNING) << message;
    ps->num_pixels = 0;
    get_item_end();
    return -1;
  }
  get_vector_of_int32(ps->pixel_HV_DAC, ps->num_pixels);

  /* The same threshold for each pixel in a drawer */
  ps->num_drawers = get_int32();
  if (ps->num_drawers < 0 || ps->num_drawers > H_MAX_DRAWERS) {
    char message[1024];
    snprintf(message, sizeof(message),
             "Data size in PixelSetting is invalid: You attempt to"
             " read settings for %d drawers but the library was compiled"
             " for a maximum of %d.",
             ps->num_drawers, H_MAX_DRAWERS);
    LOG(WARNING) << message;
    ps->num_drawers = 0;
    get_item_end();
    return -1;
  }
  get_vector_of_int32(ps->threshold_DAC, ps->num_drawers);

  /* The same readout window for each pixel in a drawer */
  if ((rc = get_int32()) != ps->num_drawers) {
    /*
     */
    char message[1024];
    snprintf(message, sizeof(message),
             "Data size in PixelSetting is invalid: Expected data"
             " for %d drawers but got %d",
             ps->num_drawers, rc);
    // LOG(WARNING)<<message);
    ps->num_drawers = 0;
    get_item_end();
    return -1;
  }
  get_vector_of_int(ps->ADC_start, ps->num_drawers);
  get_vector_of_int(ps->ADC_count, ps->num_drawers);

  if (item_header->version >= 1) {
    ps->time_slice = get_real();
    ps->sum_bins = get_int32();
  } else {
    ps->time_slice = 0.;
    ps->sum_bins = 0;
  }

  /* We may also have reference pulse shapes available. */
  /* These could be of potential use for later pulse shape analysis. */
  if (item_header->version >= 2) {
    int i, n;
    ps->nrefshape = get_scount();
    ps->lrefshape = get_scount();
    if (ps->nrefshape > H_MAX_GAINS || ps->lrefshape > H_MAX_FSHAPE) {
      char message[1024];
      snprintf(message, sizeof(message),
               "Invalid reference pulse shape data in PixelSetting.\n");
      LOG(WARNING) << message;
      get_item_end();
      return -1;
    }
    ps->ref_step = get_real();
    for (n = 0; n < ps->nrefshape; n++) {
      for (i = 0; i < ps->lrefshape; i++)
        ps->refshape[n][i] = get_sfloat();
    }
  } else {
    ps->nrefshape = ps->lrefshape = 0;
    ps->ref_step = 0.;
  }

  if (item_header->version >= 3)
    ps->sum_offset = get_int32();
  else
    ps->sum_offset = 0; /* Actually not known */

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_Pixelset() {
  tel_id = item_header->ident;
  if ((itel = find_tel_idx(tel_id)) < 0) {
  }
  rc = read_hess_pixelset(&hsdata->pixel_set[itel]);
  if (rc != 0) {
    LOG(ERROR) << "read_hess_pixelset() error, the status is " << rc;
    exit(EXIT_FAILURE);
  }
}

int LAST_IO::SimTelIO::read_hess_camsoftset(CameraSoftSet *cs) {
  int rc;

  if (buffer == (BYTE *)NULL || cs == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_CAMSOFTSET; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version != 0) {
    fprintf(stderr, "Unsupported camera software settings version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  if (cs->tel_id >= 0 && item_header->ident != cs->tel_id) {
    LOG(WARNING) << ("Refusing to copy CameraSoftSet for wrong telescope");
    get_item_end();
    return -1;
  }

  cs->dyn_trig_mode = get_int32();
  cs->dyn_trig_threshold = get_int32();
  cs->dyn_HV_mode = get_int32();
  cs->dyn_HV_threshold = get_int32();
  cs->data_red_mode = get_int32();
  cs->zero_sup_mode = get_int32();
  cs->zero_sup_num_thr = get_int32();
  if (cs->zero_sup_num_thr < 0 ||
      (size_t)cs->zero_sup_num_thr > sizeof(cs->zero_sup_thresholds) /
                                         sizeof(cs->zero_sup_thresholds[0])) {
    LOG(WARNING) << "Data size invalid in CameraSoftSet";
    cs->zero_sup_num_thr = 0;
    get_item_end();
    return -1;
  }
  get_vector_of_int32(cs->zero_sup_thresholds, cs->zero_sup_num_thr);
  cs->unbiased_scale = get_int32();
  cs->dyn_ped_mode = get_int32();
  cs->dyn_ped_events = get_int32();
  cs->dyn_ped_period = get_int32();
  cs->monitor_cur_period = get_int32();
  cs->report_cur_period = get_int32();
  cs->monitor_HV_period = get_int32();
  cs->report_HV_period = get_int32();

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_Camsoftset() {
  tel_id = item_header->ident;
  if ((itel = find_tel_idx(tel_id)) < 0) {
    LOG(ERROR) << "Camera software settings for unkown telescope " << tel_id;
    exit(EXIT_FAILURE);
  }
  rc = read_hess_camsoftset(&hsdata->cam_soft_set[itel]);
  if (rc != 0) {
    LOG(WARNING) << "Camera software settings error for " << tel_id;
  }
}
int LAST_IO::SimTelIO::read_hess_trackset(TrackingSetup *ts) {
  int rc;

  if (buffer == (BYTE *)NULL || ts == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_TRACKSET; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version != 0) {
    fprintf(stderr, "Unsupported tracking settings version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  if (ts->tel_id >= 0 && item_header->ident != ts->tel_id) {
    // LOG(WARNING)<<"Refusing to copy TrackingSetup for wrong telescope");
    get_item_end();
    return -1;
  }

  ts->drive_type_az = get_short();
  ts->drive_type_alt = get_short();
  ts->zeropoint_az = get_real();
  ts->zeropoint_alt = get_real();
  ts->sign_az = get_real();
  ts->sign_alt = get_real();
  ts->resolution_az = get_real();
  ts->resolution_alt = get_real();
  ts->range_low_az = get_real();
  ts->range_low_alt = get_real();
  ts->range_high_az = get_real();
  ts->range_high_alt = get_real();
  ts->park_pos_az = get_real();
  ts->park_pos_alt = get_real();

  ts->known = 1;

  return get_item_end();
}
void LAST_IO::SimTelIO::Read_Trackset() {
  tel_id = item_header->ident;
  if ((itel = find_tel_idx(tel_id)) < 0) {
    LOG(ERROR) << "Tracking settings for unkown telescope " << tel_id;
    exit(EXIT_FAILURE);
  }
  rc = read_hess_trackset(&hsdata->tracking_set[itel]);
  if (rc != 0) {
    LOG(WARNING) << "Tracking settings error for " << tel_id;
  }
}
int LAST_IO::SimTelIO::read_hess_pointingcor(PointingCorrection *pc) {
  int rc;

  if (buffer == (BYTE *)NULL || pc == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_POINTINGCOR; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version != 0) {
    fprintf(stderr, "Unsupported pointing correction version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  if (pc->tel_id >= 0 && item_header->ident != pc->tel_id) {
    //("Refusing to copy PointingCorrection for wrong telescope");
    get_item_end();
    return -1;
  }

  pc->function_type = get_int32();
  pc->num_param = get_int32();
  if (pc->num_param < 0 ||
      (size_t)pc->num_param >
          sizeof(pc->pointing_param) / sizeof(pc->pointing_param[0])) {
    LOG(WARNING) << "Invalid data size for PointingCorrection";
    pc->num_param = 0;
    get_item_end();
    return -1;
  }
  get_vector_of_real(pc->pointing_param, pc->num_param);

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_Pointingcor() {
  tel_id = item_header->ident;
  if ((itel = find_tel_idx(tel_id)) < 0) {
    LOG(ERROR) << "Pointing correction for unkown telescope " << tel_id;
    exit(EXIT_FAILURE);
  }
  rc = read_hess_pointingcor(&hsdata->point_cor[itel]);
  if (rc != 0) {
    LOG(WARNING) << "Pointing correction error for " << tel_id;
  }
}
int LAST_IO::SimTelIO::read_hess_centralevent(CentralEvent *ce) {
  int rc, ntt, itel, itrg;

  if (buffer == (BYTE *)NULL || ce == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_CENTEVENT; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 3) {
    fprintf(stderr, "Unsupported central event version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  ce->glob_count = item_header->ident;

  get_time_blob(&ce->cpu_time);
  get_time_blob(&ce->gps_time);
  ce->teltrg_pattern = get_int32();
  ce->teldata_pattern = get_int32();

  if (item_header->version >= 1) {
    ce->num_teltrg = get_short();
    if (ce->num_teltrg > H_MAX_TEL) {
      fprintf(stderr,
              "Invalid number of triggered telescopes (%d) in central trigger "
              "block for event %d.\n",
              ce->num_teltrg, ce->glob_count);
      ce->num_teltrg = 0;
      get_item_end();
      return -1;
    }
    get_vector_of_int(ce->teltrg_list, ce->num_teltrg);
    get_vector_of_float(ce->teltrg_time, ce->num_teltrg);
    ce->num_teldata = get_short();
    if (ce->num_teldata > H_MAX_TEL) {
      fprintf(stderr,
              "Invalid number of telescopes with data (%d) in central trigger "
              "block for event %d.\n",
              ce->num_teldata, ce->glob_count);
      ce->num_teldata = 0;
      get_item_end();
      return -1;
    }
    get_vector_of_int(ce->teldata_list, ce->num_teldata);
  } else {
    ce->num_teltrg = 0;
    ce->num_teldata = 0;
  }

  if (item_header->version >= 2) {
    for (itel = 0; itel < ce->num_teltrg; itel++)
      ce->teltrg_type_mask[itel] = get_count32();
    for (itel = 0; itel < ce->num_teltrg; itel++) {
      ntt = 0;
      for (itrg = 0; itrg < H_MAX_TRG_TYPES; itrg++) {
        if ((ce->teltrg_type_mask[itel] & (1 << itrg))) {
          ntt++;
          ce->teltrg_time_by_type[itel][itrg] = ce->teltrg_time[itel];
        } else
          ce->teltrg_time_by_type[itel][itrg] = 9999.;
      }
      if (ntt >
          1) // Need type-specific trigger times only with more than one type
      {
        for (itrg = 0; itrg < H_MAX_TRG_TYPES; itrg++) {
          if ((ce->teltrg_type_mask[itel] & (1 << itrg)))
            ce->teltrg_time_by_type[itel][itrg] = get_real();
        }
      }
    }
  } else {
    for (itel = 0; itel < ce->num_teltrg; itel++) {
      ce->teltrg_type_mask[itel] = 1; // Older data was always majority trigger
      ce->teltrg_time_by_type[itel][0] = ce->teltrg_time[itel];
      for (itrg = 1; itrg < H_MAX_TRG_TYPES; itrg++)
        ce->teltrg_time_by_type[itel][itrg] = 9999.;
    }
  }

  if (item_header->version >= 3) {
    /* Data relevant for undoing the plane wavefront compensation */
    ce->az_comp = get_real();
    ce->alt_comp = get_real();
    ce->ls_comp = get_real();
  } else {
    ce->az_comp = ce->alt_comp = ce->ls_comp = 0.;
  }

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_trackevent(TrackEvent *tke) {
  int rc, tel_id;

  if (buffer == (BYTE *)NULL || tke == NULL)
    return -1;

  item_header->type = 0; /* No data type this time */
  if ((rc = get_item_begin()) < 0)
    return rc;
  // tel_id = (item_header->type - IO_TYPE_HESS_TRACKEVENT) % 100 +
  //         100 * ((item_header->type - IO_TYPE_HESS_TRACKEVENT) / 1000);
  tel_id =
      (item_header->ident & 0xff) | ((item_header->ident & 0x3f000000) >> 16);
  if (tel_id < 0 || tel_id != tke->tel_id) {
    LOG(WARNING) << "Not a tracking event block or one for the wrong telescope";
    get_item_end();
    return -1;
  }
  if (item_header->version != 0) {
    fprintf(stderr, "Unsupported tracking event version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  if (tke->tel_id != tel_id) {
    LOG(WARNING) << "Tracking data is for wrong telescope.";
    get_item_end();
    return -1;
  }

  tke->raw_known = (item_header->ident & 0x100) != 0;
  tke->cor_known = (item_header->ident & 0x200) != 0;

  if (tke->raw_known) {
    tke->azimuth_raw = get_real();
    tke->altitude_raw = get_real();
  }
  if (tke->cor_known) {
    tke->altitude_cor = get_real();
    tke->azimuth_cor = get_real();
  }

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_televt_head(TelEvent *te) {
  int rc = 0, i, t;

  if (buffer == (BYTE *)NULL || te == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_TELEVTHEAD; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 3) {
    fprintf(stderr, "Unsupported telescope event header version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  if (item_header->ident != te->tel_id) {
    LOG(WARNING) << "Event header is for wrong telescope";
    get_item_end();
    return -1;
  }

  te->loc_count = get_int32();
  te->glob_count = get_int32();
  get_time_blob(&te->cpu_time);
  get_time_blob(&te->gps_time);
  t = get_short();
  te->trg_source = t & 0xff;
  te->known_time_trgsect = 0;

  if ((t & 0x100)) {
    te->num_list_trgsect =
        (item_header->version <= 1 ? get_short() : get_scount32());
    for (i = 0; i < te->num_list_trgsect; i++)
      te->list_trgsect[i] =
          (item_header->version <= 1 ? get_short() : get_scount32());
    if (item_header->version >= 1 && (t & 0x400)) {
      for (i = 0; i < te->num_list_trgsect; i++)
        te->time_trgsect[i] = get_real();
      te->known_time_trgsect = 1;
    } else {
      for (i = 0; i < te->num_list_trgsect; i++)
        te->time_trgsect[i] = 0.;
    }
    //      if ( item_header->version >= ... )
    //      {
    //         for (i=0; i<te->num_list_trgsect; i++)
    //          te->type_trgsect[i] = get_byte();
    //      }
  }
  if ((t & 0x200)) {
    te->num_phys_addr =
        (item_header->version <= 1 ? get_short() : get_scount32());
    for (i = 0; i < te->num_phys_addr; i++)
      te->phys_addr[i] =
          (item_header->version <= 1 ? get_short() : get_scount32());
  }

  if (item_header->version >= 3) {
    te->time_readout = get_real();
    te->time_trg_rel = get_real();
  } else {
    te->time_readout = 0.;
    te->time_trg_rel = 0.;
  }

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_teladc_sums(AdcData *raw) {
  uint32_t flags;
  uint16_t offset_hg8 = 0, scale_hg8 = 1;
  uint16_t cflags, bflags, zbits;
  uint32_t lgval[16], hgval[16];
  uint8_t hgval8[16];
  int mlg, mhg16, mhg8, i, j, k, m, n;
  int rc;
#ifdef XXDEBUG
  int mlg_tot = 0, mhg16_tot = 0, mhg8_tot = 0, m_tot = 0;
#endif

  if (buffer == (BYTE *)NULL || raw == NULL)
    return -1;

  raw->known = 0;
  raw->num_pixels = 0;
  item_header->type = IO_TYPE_HESS_TELADCSUM; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 4) {
    fprintf(stderr, "Unsupported ADC sums version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  /* Lots of small data was packed into the ID */
  flags = (uint32_t)item_header->ident;
  raw->zero_sup_mode = flags & 0x1f;
  raw->data_red_mode = (flags >> 5) & 0x1f;

  if (item_header->version >= 4 && raw->data_red_mode == 2) {
    raw->threshold = get_scount32();
    raw->offset_hg8 = get_scount32();
    raw->scale_hg8 = get_scount32();
  }

  raw->list_known = (flags >> 10) & 0x01;
  if (item_header->version == 0) {
    raw->tel_id = (flags >> 25) & 0x1f; // High-order bits may be missing.
    raw->num_pixels = (flags >> 12) & 0x07ff;
    raw->num_gains = (flags >> 23) & 0x03;
  } else if (item_header->version == 1) {
    raw->tel_id = (flags >> 25) & 0x1f; // High-order bits may be missing.
    raw->num_pixels = (flags >> 12) & 0x0fff;
    raw->num_gains = (((flags >> 24) & 0x01) ? 2 : 1);
  } else {
    raw->tel_id = (flags >> 12) & 0xffff; // High-order bits may be missing.
    raw->num_pixels = get_long();
    raw->num_gains = get_short();
  }

  raw->num_samples = 0; // We have sums and not samples.

  if (raw->num_pixels > H_MAX_PIX || raw->num_gains > H_MAX_GAINS ||
      /* (is this fixed ????)
              (raw->num_gains != 2 &&
               (raw->zero_sup_mode != 0 || raw->data_red_mode != 0)) ||
      */
      (raw->num_pixels >= 32768 && raw->zero_sup_mode > 1) ||
      raw->zero_sup_mode > 2 || raw->data_red_mode > 2) {
    LOG(WARNING)
        << "Invalid raw data block is skipped (limits exceeded or bad mode).";
    fprintf(stderr, "Num_pixels=%d, num_gains=%d, zero_sup=%d, data_red=%d\n",
            raw->num_pixels, raw->num_gains, raw->zero_sup_mode,
            raw->data_red_mode);
    get_item_end();
    raw->num_pixels = 0;
    return -1;
  }

  if (raw->data_red_mode == 2) {
    raw->offset_hg8 = offset_hg8 = get_short();
    raw->scale_hg8 = scale_hg8 = get_short();
    if (scale_hg8 <= 0)
      scale_hg8 = 1;
  }

  /* Without zero-suppression and data-reduction, every channel is known */
  /* but if either is z.s. or d.r. is on, a channel is only known if */
  /* marked as such in the data. */
  if (raw->zero_sup_mode == 0 && raw->data_red_mode == 0)
    k = 1;
  else
    k = 0;

  if (k != 0) {
    /* Initialize values one by one */
    for (j = 0; j < raw->num_pixels; j++)
      raw->significant[j] = k;
    for (i = 0; i < raw->num_gains; i++) {
      for (j = 0; j < raw->num_pixels; j++) {
        raw->adc_known[i][j] = k;
        /* raw->adc_sum[i][j] = 0; now done with memset below */
      }
    }
  } else {
    /* Memset should be faster for setting all to zero */
    memset(raw->significant, 0,
           (size_t)raw->num_pixels * sizeof(raw->significant[0]));
    for (i = 0; i < raw->num_gains; i++)
      memset(raw->adc_known[i], 0,
             (size_t)raw->num_pixels * sizeof(raw->adc_known[0][0]));
  }
  for (i = 0; i < raw->num_gains; i++)
    memset(raw->adc_sum[i], 0,
           (size_t)raw->num_pixels * sizeof(raw->adc_sum[0][0]));

#ifdef XXDEBUG
  printf("### z = %d, d = %d\n", raw->zero_sup_mode, raw->data_red_mode);
#endif

  switch (raw->zero_sup_mode) {
  /* -------------- Zero suppression mode 0 --------------- */
  case 0: /* No zero suppression */
#ifdef XXDEBUG
    m_tot = raw->num_pixels;
#endif
    switch (raw->data_red_mode) {
    case 0: /* No data reduction */
      /* Note: in this mode ADC sums are stored in the internal order, */
      /* no matter how many different gains there are per PMT. */
      /* In all other modes, the order is fixed (lg/hg16/hg8) and limited */
      /* to two different gains per PMT. */
      for (i = 0; i < raw->num_gains; i++) {
        // get_vector_of_uint16(raw->adc_sum[i],raw->num_pixels,);
        if (item_header->version < 3)
          get_adcsum_as_uint16(raw->adc_sum[i], raw->num_pixels);
        else
#ifdef OLD_CODE
          get_adcsum_differential(raw->adc_sum[i], raw->num_pixels, );
#else
          get_vector_of_uint32_scount_differential(raw->adc_sum[i],
                                                   raw->num_pixels);
#endif
      }
      break;
#if (H_MAX_GAINS >= 2)
    case 1: /* Low low-gain channels were skipped (for two gains) */
      k = 0;
      while (k < raw->num_pixels) {
        get_vector_of_uint16(&cflags, 1);
        mlg = 0;
        if (k + 16 <= raw->num_pixels)
          n = 16;
        else
          n = raw->num_pixels - k;
        for (j = 0; j < n; j++)
          if ((cflags & (1 << j)))
            mlg++;
        // get_vector_of_uint16(lgval,mlg,);
        // get_vector_of_uint16(&raw->adc_sum[HI_GAIN][k],n,);
        if (item_header->version < 3) {
          if (raw->num_gains >= 2)
            get_adcsum_as_uint16(lgval, mlg);
          get_adcsum_as_uint16(&raw->adc_sum[HI_GAIN][k], n);
        } else {
#ifdef OLD_CODE
          if (raw->num_gains >= 2)
            get_adcsum_differential(lgval, mlg, );
          get_adcsum_differential(&raw->adc_sum[HI_GAIN][k], n, );
#else
          if (raw->num_gains >= 2)
            get_vector_of_uint32_scount_differential(lgval, mlg);
          get_vector_of_uint32_scount_differential(&raw->adc_sum[HI_GAIN][k],
                                                   n);
#endif
        }
#ifdef XXDEBUG
        mlg_tot += mlg;
        mhg16_tot += n;
#endif
        mlg = 0;
        for (j = 0; j < n; j++) {
          if ((cflags & (1 << j))) {
            raw->adc_sum[LO_GAIN][k + j] = lgval[mlg++];
            raw->adc_known[LO_GAIN][k + j] = 1;
          } else {
            raw->adc_sum[LO_GAIN][k + j] = 0;
            raw->adc_known[LO_GAIN][k + j] = 0;
          }
          raw->adc_known[HI_GAIN][k + j] = 1;
          raw->significant[k + j] = 1;
        }
        k += n;
      }
      break;
    case 2: /* Width of high-gain channel can be reduced */
      k = 0;
      while (k < raw->num_pixels) {
        get_vector_of_uint16(&cflags, 1);
        get_vector_of_uint16(&bflags, 1);
        mlg = mhg16 = mhg8 = 0;
        if (k + 16 <= raw->num_pixels)
          n = 16;
        else
          n = raw->num_pixels - k;
        for (j = 0; j < n; j++) {
          if ((cflags & (1 << j))) {
            mlg++;
            mhg16++;
          } else if ((bflags & (1 << j)))
            mhg8++;
          else
            mhg16++;
        }
        // get_vector_of_uint16(lgval,mlg,);
        // get_vector_of_uint16(hgval,mhg16,);
        if (item_header->version < 3) {
          if (raw->num_gains >= 2)
            get_adcsum_as_uint16(lgval, mlg);
          get_adcsum_as_uint16(hgval, mhg16);
        } else {
#ifdef OLD_CODE
          if (raw->num_gains >= 2)
            get_adcsum_differential(lgval, mlg, );
          get_adcsum_differential(hgval, mhg16, );
#else
          if (raw->num_gains >= 2)
            get_vector_of_uint32_scount_differential(lgval, mlg);
          get_vector_of_uint32_scount_differential(hgval, mhg16);
#endif
        }
        get_vector_of_uint8(hgval8, mhg8);
#ifdef XXDEBUG
        mlg_tot += mlg;
        mhg16_tot += mhg16;
        mhg8_tot += mhg8;
#endif
        mlg = mhg8 = mhg16 = 0;
        for (j = 0; j < n; j++) {
          if ((cflags & (1 << j))) {
            raw->adc_sum[LO_GAIN][k + j] = lgval[mlg++];
            raw->adc_known[LO_GAIN][k + j] = 1;
            raw->adc_sum[HI_GAIN][k + j] = hgval[mhg16++];
          } else {
            if ((bflags & (1 << j)))
              raw->adc_sum[HI_GAIN][k + j] =
                  hgval8[mhg8++] * scale_hg8 + offset_hg8;
            else
              raw->adc_sum[HI_GAIN][k + j] = hgval[mhg16++];
          }
          raw->adc_known[HI_GAIN][k + j] = 1;
          raw->significant[k + j] = 1;
        }
        k += n;
      }
      break;
#endif
    default:
      assert(0);
    }
    break;

  /* -------------- Zero suppression mode 1 --------------- */
  case 1: /* Bit pattern indicates zero suppression */
    switch (raw->data_red_mode) {
    case 0: /* No data reduction */
    case 1: /* Low low-gain channels were skipped (for two gains) */
    case 2: /* Width of high-gain channel can be reduced */
      k = 0;
      while (k < raw->num_pixels) {
        if (k + 16 <= raw->num_pixels)
          n = 16;
        else
          n = raw->num_pixels - k;
        get_vector_of_uint16(&zbits, 1);
#ifdef XXDEBUG
        printf("#+# z: %04x\n", zbits);
#endif
        m = mlg = mhg16 = mhg8 = 0;
        cflags = bflags = 0;
        if (zbits > 0) {
          for (j = 0; j < n; j++)
            if ((zbits & (1 << j)))
              m++;

          if (raw->data_red_mode >= 1) {
            get_vector_of_uint16(&cflags, 1);
#ifdef XXDEBUG
            printf("#+# c: %04x\n", cflags);
#endif
            if (raw->data_red_mode == 2) {
              get_vector_of_uint16(&bflags, 1);
#ifdef XXDEBUG
              printf("#+# b: %04x\n", bflags);
#endif
            }
            for (j = 0; j < n; j++) {
              if (!(zbits & (1 << j)))
                continue;
              if ((cflags & (1 << j))) {
                mlg++;
                mhg16++;
              } else {
                if (raw->data_red_mode == 2) {
                  if ((bflags & (1 << j)))
                    mhg8++;
                  else
                    mhg16++;
                } else
                  mhg16++;
              }
            }
          } else
            mlg = mhg16 = m;

          if (m > 0) {
            // get_vector_of_uint16(lgval,mlg,);
            // get_vector_of_uint16(hgval,mhg16,);
            if (item_header->version < 3) {
              if (raw->num_gains >= 2)
                get_adcsum_as_uint16(lgval, mlg);
              get_adcsum_as_uint16(hgval, mhg16);
            } else {
#ifdef OLD_CODE
              if (raw->num_gains >= 2)
                get_adcsum_differential(lgval, mlg, );
              get_adcsum_differential(hgval, mhg16, );
#else
              if (raw->num_gains >= 2)
                get_vector_of_uint32_scount_differential(lgval, mlg);
              get_vector_of_uint32_scount_differential(hgval, mhg16);
#endif
            }
            if (mhg8 > 0)
              get_vector_of_uint8(hgval8, mhg8);
#ifdef XXDEBUG
            printf("#++ %d %d %d (%d)\n", mlg, mhg16, mhg8, m);
            mlg_tot += mlg;
            mhg16_tot += mhg16;
            mhg8_tot += mhg8;
#endif
            mlg = mhg16 = mhg8 = 0;
            for (j = 0; j < n; j++) {
              if ((zbits & (1 << j))) {
#ifdef XXDEBUG
                m_tot++;
#endif
                raw->significant[k + j] = 1;
                if (raw->data_red_mode < 1 || (cflags & (1 << j))) {
#if (H_MAX_GAINS >= 2)
                  raw->adc_sum[LO_GAIN][k + j] = lgval[mlg++];
#endif
                  raw->adc_sum[HI_GAIN][k + j] = hgval[mhg16++];
#if (H_MAX_GAINS >= 2)
                  raw->adc_known[LO_GAIN][k + j] = 1;
#endif
                  raw->adc_known[HI_GAIN][k + j] = 1;
                } else {
#if (H_MAX_GAINS >= 2)
                  raw->adc_sum[LO_GAIN][k + j] = 0;
#endif
                  if (raw->data_red_mode == 2 && (bflags & (1 << j)))
                    raw->adc_sum[HI_GAIN][k + j] =
                        hgval8[mhg8++] * scale_hg8 + offset_hg8;
                  else
                    raw->adc_sum[HI_GAIN][k + j] = hgval[mhg16++];
                  raw->adc_known[HI_GAIN][k + j] = 1;
                }
              }
            }
          }
        }
        k += n;
      }
      break;

    default:
      assert(0);
    }
    break;

  /* -------------- Zero suppression mode 2 --------------- */
  case 2: /* List of not zero-suppressed pixels */
  {
    uint32_t adc_sum_l[H_MAX_GAINS][H_MAX_PIX];
    uint8_t adc_hg8[H_MAX_PIX];
    int adc_list_l[H_MAX_PIX];
#if (H_MAX_GAINS >= 2)
    int without_lg[H_MAX_PIX];
#endif
    int reduced_width[H_MAX_PIX];

    switch (raw->data_red_mode) {
    case 0: /* No data reduction */
    case 1: /* Low low-gain channels were skipped (for two gains) */
    case 2: /* Width of high-gain channel can be reduced */
      if (item_header->version >= 4) {
        raw->list_size = get_count();
        get_vector_of_int_scount(adc_list_l, raw->list_size);
      } else {
        raw->list_size = get_short();
        get_vector_of_int(adc_list_l, raw->list_size);
      }
      mlg = mhg16 = mhg8 = 0;
      for (j = 0; j < raw->list_size; j++) {
        if (item_header->version >= 4) {
          raw->adc_list[j] = k = adc_list_l[j] & 0x1fffff;
#if (H_MAX_GAINS >= 2)
          without_lg[j] = ((adc_list_l[j] & 0x200000) != 0);
#endif
          reduced_width[j] = ((adc_list_l[j] & 0x400000) != 0);
        } else {
          raw->adc_list[j] = k = adc_list_l[j] & 0x1fff;
#if (H_MAX_GAINS >= 2)
          without_lg[j] = ((adc_list_l[j] & 0x2000) != 0);
#endif
          reduced_width[j] = ((adc_list_l[j] & 0x4000) != 0);
        }
        if (reduced_width[j])
          mhg8++;
#if (H_MAX_GAINS >= 2)
        else if (raw->num_gains < 2 || without_lg[j])
          mhg16++;
        else {
          mlg++;
          mhg16++;
        }
#else
        else
          mhg16++;
#endif
      }

      if (item_header->version < 2) {
#if (H_MAX_GAINS >= 2)
        if (raw->num_gains >= 2)
          get_adcsum_as_uint16(adc_sum_l[LO_GAIN], mlg);
#endif
        get_adcsum_as_uint16(adc_sum_l[HI_GAIN], mhg16);
      } else {
#if (H_MAX_GAINS >= 2)
        if (raw->num_gains >= 2)
          get_adcsum_differential(adc_sum_l[LO_GAIN], mlg);
#endif
        get_adcsum_differential(adc_sum_l[HI_GAIN], mhg16);
      }
      if (mhg8 > 0)
        get_vector_of_uint8(adc_hg8, mhg8);
#ifdef XXDEBUG
      printf("#++ %d %d %d\n", mlg, mhg16, mhg8);
      m_tot += raw->list_size;
      mlg_tot += mlg;
      mhg16_tot += mhg16;
      mhg8_tot += mhg8;
#endif
      mlg = mhg16 = mhg8 = 0; /* Start from the beginning of each array */
      for (j = 0; j < raw->list_size; j++) {
        k = raw->adc_list[j];
        raw->significant[k] = 1;
        if (reduced_width[j])
          raw->adc_sum[HI_GAIN][k] = adc_hg8[mhg8++] * scale_hg8 + offset_hg8;
        else
          raw->adc_sum[HI_GAIN][k] = adc_sum_l[HI_GAIN][mhg16++];
        raw->adc_known[HI_GAIN][k] = 1;
#if (H_MAX_GAINS >= 2)
        if (raw->num_gains <= 1)
          raw->adc_known[LO_GAIN][k] = 0;
        else if (without_lg[j]) {
          raw->adc_sum[LO_GAIN][k] = 0;
          raw->adc_known[LO_GAIN][k] = 0;
        } else {
          raw->adc_sum[LO_GAIN][k] = adc_sum_l[LO_GAIN][mlg++];
          raw->adc_known[LO_GAIN][k] = 1;
        }
#endif
      }
      break;

    default:
      assert(0);
    }
  } break;

  default:
    assert(0);
  }

#ifdef XXDEBUG
  printf("#+# %d %d %d (%d)\n", mlg_tot, mhg16_tot, mhg8_tot, m_tot);
#endif

  raw->known = 1;

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_teladc_samples(AdcData *raw, int what) {
  uint32_t flags;
  int ipix, igain, isamp;
  int rc;
  uint32_t sum;
  int zero_sup_mode = 0, data_red_mode = 0, list_known = 0;

  if (buffer == (BYTE *)NULL || raw == NULL)
    return -1;

  // raw->known = 0; /* We may have read the ADC sums before */

  raw->num_pixels = 0;
  item_header->type = IO_TYPE_HESS_TELADCSAMP; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 3) {
    fprintf(stderr, "Unsupported ADC samples version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  /* Lots of small data was packed into the ID */
  flags = (uint32_t)item_header->ident;
  zero_sup_mode = flags & 0x1f;
  data_red_mode = (flags >> 5) & 0x1f;
  list_known = (flags >> 10) & 0x01; /* Bit 10 was never set, thus zero */
  if ((zero_sup_mode != 0 && item_header->version < 3) || data_red_mode != 0 ||
      list_known) {
    LOG(WARNING) << "Unsupported ADC sample format";
    get_item_end();
    return -1;
  }
  /* Sample-mode zero suppression and data reduction separated from sum data */
  raw->zero_sup_mode |= zero_sup_mode << 5;
  raw->data_red_mode |= data_red_mode << 5;
  /* If there was a list for sum data, it gets lost now - but do we want that?
   */
  /* The problem is there is only place for one list but zero suppression for
     sum data and samples may well be different. Without known list, the
     application processing the data should loop over all pixels and test for
     significant bits and adc_known bits. With known list it only needs to
     loop over pixels included in the list but still test significant and
     adc_known. */
  /* As long as pixels with samples are a subset of pixels with sums,
     we could preserve the sum data pixel list. */
  raw->list_known = 0; /* Since list_known here is always zero. */
  if (item_header->version == 0) {
    raw->tel_id = (flags >> 25) & 0x1f;
    raw->num_pixels = (flags >> 12) & 0x07ff;
    raw->num_gains = (flags >> 23) & 0x03;
  } else if (item_header->version == 1) {
    raw->tel_id = (flags >> 25) & 0x1f;
    raw->num_pixels = (flags >> 12) & 0x0fff;
    raw->num_gains = (((flags >> 24) & 0x01) ? 2 : 1);
  } else {
    raw->tel_id = (flags >> 12) & 0xffff;
    raw->num_pixels = get_long();
    raw->num_gains = get_short();
  }

  raw->num_samples = get_short();

  if (raw->num_pixels > H_MAX_PIX || raw->num_gains > H_MAX_GAINS ||
      raw->num_samples > H_MAX_SLICES) {
    LOG(WARNING) << "Invalid raw data block is skipped (limits exceeded).";
    fprintf(stderr, "Num_pixels=%d, num_gains=%d, num_samples=%d\n",
            raw->num_pixels, raw->num_gains, raw->num_samples);
    get_item_end();
    raw->num_pixels = 0;
    return -1;
  }

  if (zero_sup_mode) {
    int ilist, ipix1, ipix2;
    int pixel_list[H_MAX_PIX][2], list_size = 0;
#if (H_MAX_GAINS >= 2)
    int pixel_list_lg[H_MAX_PIX][2], list_size_lg = 0;
#endif
    for (ipix = 0; ipix < raw->num_pixels; ipix++) {
      raw->significant[ipix] &=
          ~0xe0; /* Clear sample mode significance bits. */
      raw->adc_known[0][ipix] &=
          0x01; /* Same for adc_known sample-mode bits; */
#if (H_MAX_GAINS >= 2)
      raw->adc_known[1][ipix] &=
          0x01; /* bit 0 is for sum, bit 1 for samples. */
#endif
    }

    /* Common or high-gain pixel list */
    list_size = get_scount32();
    if (list_size > H_MAX_PIX) {
      LOG(WARNING)
          << "Pixel list too large in zero-suppressed sample-mode data.";
      get_item_end();
      return -1;
    }

    for (ilist = 0; ilist < list_size; ilist++) {
      ipix1 = get_scount();
      if (ipix1 < 0) /* Single pixel */
      {
        ipix2 = -ipix1 - 1;
        ipix1 = ipix2;
      } else /* pixel range */
        ipix2 = get_scount();
      pixel_list[ilist][0] = ipix1;
      pixel_list[ilist][1] = ipix2;
    }

#if (H_MAX_GAINS >= 2)
    /* Read low-gain pixel list if needed. */
    if (data_red_mode && raw->num_gains > 1) {
      list_size_lg = get_scount32();
      if (list_size_lg > H_MAX_PIX) {
        LOG(WARNING) << "Pixel list too large in low-gain zero-suppressed "
                        "sample-mode data.";
        get_item_end();
        return -1;
      }
      for (ilist = 0; ilist < list_size_lg; ilist++) {
        ipix1 = get_scount();
        if (ipix1 < 0) /* Single pixel */
        {
          ipix2 = -ipix1 - 1;
          ipix1 = ipix2;
        } else /* pixel range */
          ipix2 = get_scount();
        pixel_list_lg[ilist][0] = ipix1;
        pixel_list_lg[ilist][1] = ipix2;
      }
      for (ilist = 0; ilist < list_size; ilist++) {
        for (ipix = pixel_list[ilist][0]; ipix <= pixel_list[ilist][1];
             ipix++) {
          get_vector_of_uint16_scount_differential(
              raw->adc_sample[HI_GAIN][ipix], raw->num_samples);
          raw->significant[ipix] |= 0x20;
          raw->adc_known[HI_GAIN][ipix] |= 2;
        }
      }
      for (ilist = 0; ilist < list_size_lg; ilist++)
        for (ipix = pixel_list_lg[ilist][0]; ipix <= pixel_list_lg[ilist][1];
             ipix++) {
          get_vector_of_uint16_scount_differential(
              raw->adc_sample[LO_GAIN][ipix], raw->num_samples);
          raw->adc_known[LO_GAIN][ipix] |= 2;
        }
      if ((what & RAWSUM_FLAG)) {
      }
    } else
#endif
    {
      for (igain = 0; igain < raw->num_gains; igain++) {
        for (ilist = 0; ilist < list_size; ilist++) {
          for (ipix = pixel_list[ilist][0]; ipix <= pixel_list[ilist][1];
               ipix++) {
#ifdef OLD_CODE
            get_adcsample_differential(raw->adc_sample[igain][ipix],
                                       raw->num_samples, );
#else
            get_vector_of_uint16_scount_differential(
                raw->adc_sample[igain][ipix], raw->num_samples);
#endif
            raw->significant[ipix] |= 0x20;

            /* Should the sampled data also be summed up here? There might be
             * sum data
             * preceding this sample mode data! */
            if (!raw->adc_known[igain][ipix]) {
              if ((what & RAWSUM_FLAG)) {
                /* Sum up all samples */
                sum = 0;
                for (isamp = 0; isamp < raw->num_samples; isamp++)
                  sum += raw->adc_sample[igain][ipix][isamp];
#if 1
                raw->adc_sum[igain][ipix] =
                    sum; /* No overflow of 32-bit unsigned assumed */
#else                    /* Back in the days when adc_sum was a 16-bit unsigned int */
                if (sum <= 65535)
                  raw->adc_sum[igain][ipix] = (uint16_t)sum;
                else
                  raw->adc_sum[igain][ipix] = (uint16_t)65535;
#endif
                raw->adc_known[igain][ipix] = 1;
              } else
                raw->adc_sum[igain][ipix] = 0;
            }
            raw->adc_known[igain][ipix] |= 2;
          }
        }
      }
    }
  } else /* No (sample data) zero suppression, no data reduction, no pixel
            lists. */
  {
    for (igain = 0; igain < raw->num_gains; igain++) {
      for (ipix = 0; ipix < raw->num_pixels; ipix++) {
        if (item_header->version < 3)
          get_vector_of_uint16(raw->adc_sample[igain][ipix], raw->num_samples);
        else
#ifdef OLD_CODE
          get_adcsample_differential(raw->adc_sample[igain][ipix],
                                     raw->num_samples, );
#else
          get_vector_of_uint16_scount_differential(raw->adc_sample[igain][ipix],
                                                   raw->num_samples);
#endif

        /* Should the sampled data be summed up here? If there is preceding sum
         * data, we
         * keep that. */
        /* Note that having non-zero-suppressed samples after sum data is
         * normally used.
         */
        /* In realistic data, there will be no sum known at this point. */
        if (!raw->adc_known[igain][ipix]) {
          if ((what & RAWSUM_FLAG)) {
            /* Sum up all samples */
            sum = 0;
            for (isamp = 0; isamp < raw->num_samples; isamp++)
              sum += raw->adc_sample[igain][ipix][isamp];
#if 1
            raw->adc_sum[igain][ipix] =
                sum; /* No overflow of 32-bit unsigned assumed */
#else                /* Back in the days when adc_sum was a 16-bit unsigned int */
            if (sum <= 65535)
              raw->adc_sum[igain][ipix] = (uint16_t)sum;
            else
              raw->adc_sum[igain][ipix] = (uint16_t)65535;
#endif
          } else
            raw->adc_sum[igain][ipix] = 0;
          raw->adc_known[igain][ipix] = 1;
        }
        raw->adc_known[igain][ipix] |= 2;
      }
    }
    for (ipix = 0; ipix < raw->num_pixels; ipix++)
      raw->significant[ipix] = 1;
  }

  raw->known |= 2;

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_aux_trace_digital(AuxTraceD *auxd) {
  size_t it, nt, lt;
  int rc;

  if (buffer == (BYTE *)NULL)
    return -1;

  if (auxd == NULL)
    return -1;
  auxd->known = 0;

  item_header->type = IO_TYPE_HESS_AUX_DIGITAL_TRACE; /* Data type */
  if ((rc = get_item_begin() < 0))
    return rc;
  if (item_header->version > 0) {
    fprintf(stderr, "Unsupported auxilliary digitzed traces version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  auxd->trace_type = item_header->ident;

  auxd->tel_id = get_long();
  auxd->time_scale = get_real();
  nt = get_count();
  lt = get_count();

  if (nt != auxd->num_traces || lt != auxd->len_traces ||
      auxd->trace_data == NULL) {
    if (auxd->trace_data != NULL)
      free(auxd->trace_data);
    auxd->trace_data =
        (uint16_t *)malloc(nt * lt * sizeof(auxd->trace_data[0]));
    if (auxd->trace_data == NULL) {
      fprintf(stderr, "Failed to allocate memory for %zu * %zu auxilliary "
                      "digitized trace values.\n",
              nt, lt);
      get_item_end();
      return -1;
    }
    auxd->num_traces = nt;
    auxd->len_traces = lt;
  }

  for (it = 0; it < auxd->num_traces; it++)
    get_vector_of_uint16_scount_differential(
        auxd->trace_data + it * auxd->len_traces, auxd->len_traces);

  auxd->known = 1;

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_aux_trace_analog(AuxTraceA *auxa) {
  size_t it, nt, lt;
  int rc;

  if (buffer == (BYTE *)NULL)
    return -1;

  if (auxa == NULL)
    return -1;
  auxa->known = 0;

  item_header->type = IO_TYPE_HESS_AUX_ANALOG_TRACE; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 0) {
    fprintf(stderr, "Unsupported auxilliary analog traces version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  auxa->trace_type = item_header->ident;

  auxa->tel_id = get_long();
  auxa->time_scale = get_real();
  nt = get_count();
  lt = get_count();

  if (nt != auxa->num_traces || lt != auxa->len_traces ||
      auxa->trace_data == NULL) {
    if (auxa->trace_data != NULL)
      free(auxa->trace_data);
    auxa->trace_data = (float *)malloc(nt * lt * sizeof(auxa->trace_data[0]));
    if (auxa->trace_data == NULL) {
      fprintf(stderr, "Failed to allocate memory for %zu * %zu auxilliary "
                      "analog trace values.\n",
              nt, lt);
      get_item_end();
      return -1;
    }
    auxa->num_traces = nt;
    auxa->len_traces = lt;
  }

  for (it = 0; it < auxa->num_traces; it++)
    get_vector_of_float(auxa->trace_data + it * auxa->len_traces,
                        auxa->len_traces);

  auxa->known = 1;

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_pixeltrg_time(PixelTrgTime *dt) {
  int rc;

  if (buffer == (BYTE *)NULL)
    return -1;

  if (dt == NULL)
    return -1;
  dt->known = 0;

  item_header->type = IO_TYPE_HESS_PIXELTRG_TM; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 0) {
    fprintf(stderr, "Unsupported pixel trigger time version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  if (item_header->ident != dt->tel_id) {
    fprintf(stderr, "Pixel trigger time data for wrong telescope.\n");
    get_item_end();
    return -1;
  }

  dt->time_step = get_real();
  dt->num_times = get_scount();
  if (dt->num_times < 0 || dt->num_times > H_MAX_PIX) {
    fprintf(stderr, "Number of pixel trigger times is out of range.\n");
    get_item_end();
    return -1;
  }
  get_vector_of_int_scount(dt->pixel_list, dt->num_times);
  get_vector_of_int_scount(dt->pixel_time, dt->num_times);

  dt->known = 1;

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_pixtime(PixelTiming *pixtm) {
  int i, j;
  double scale = 100.;
  int rc, with_sum = 0;
  int glob_only_selected = 0;
  int v0 = 0;

  if (buffer == (BYTE *)NULL || pixtm == NULL)
    return -1;

  pixtm->known = 0;

  pixtm->list_type = 1;
  pixtm->list_size = pixtm->num_types = 0;
  item_header->type = IO_TYPE_HESS_PIXELTIMING; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 2) {
    fprintf(stderr, "Unsupported pixel timing version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  if (item_header->version == 0)
    v0 = 1;
  if (item_header->version <= 1)
    pixtm->num_pixels = get_short();
  else
    pixtm->num_pixels = get_scount32();
  pixtm->num_gains = get_short();
  pixtm->before_peak = get_short();
  pixtm->after_peak = get_short();
  pixtm->list_type = get_short();
  if (pixtm->list_type != 1 && pixtm->list_type != 2) {
    fprintf(stderr, "Invalid type of pixel list in pixel timing data: %d.\n",
            pixtm->list_type);
    get_item_end();
    return -1;
  }
  if (item_header->version <= 1)
    pixtm->list_size = get_short();
  else
    pixtm->list_size = get_scount32();
  if (pixtm->list_size < 0 || pixtm->list_size > H_MAX_PIX) {
    fprintf(stderr, "Invalid size of pixel list in pixel timing data: %d.\n",
            pixtm->list_size);
    get_item_end();
    return -1;
  }
  if (item_header->version <= 1) {
    if (pixtm->list_type == 1)
      get_vector_of_int(pixtm->pixel_list, pixtm->list_size);
    else
      get_vector_of_int(pixtm->pixel_list, 2 * pixtm->list_size);
  } else {
    if (pixtm->list_type == 1)
      get_vector_of_int_scount(pixtm->pixel_list, pixtm->list_size);
    else
      get_vector_of_int_scount(pixtm->pixel_list, 2 * pixtm->list_size);
  }
  pixtm->threshold = get_short();
  if (pixtm->threshold < 0)
    glob_only_selected = 1;
  if (pixtm->before_peak >= 0 && pixtm->after_peak >= 0)
    with_sum = 1;
  pixtm->num_types = get_short();
  if (pixtm->num_types < 0 || pixtm->num_types > H_MAX_PIX_TIMES) {
    fprintf(stderr, "Invalid number of types in pixel timing data: %d.\n",
            pixtm->num_types);
    get_item_end();
    return -1;
  }
  get_vector_of_int(pixtm->time_type, pixtm->num_types);
  get_vector_of_float(pixtm->time_level, pixtm->num_types);
  pixtm->granularity = get_real();
  if (pixtm->granularity > 0.) {
    scale = pixtm->granularity;
  } else {
    scale = 0.01;
    pixtm->granularity = 0.01;
  }
  pixtm->peak_global = get_real();

  /* The first timing element is always initialised to indicate unknown. */
  for (i = 0; i < pixtm->num_pixels; i++)
    pixtm->timval[i][0] = -1.;
#if 0
   /* If users are sloppy we may have to initialise the global pulse sums as well. */
   if ( with_sum && glob_only_selected )
   {
      int igain, ipix;
      for ( igain=0; igain<pixtm->num_gains; igain++ )
         for ( ipix=0; ipix<pixtm->num_pixels; ipix++ )
            pixtm->pulse_sum_glob[igain][ipix] = 0.;
   }
#endif

  for (i = 0; i < pixtm->list_size; i++) {
    int ipix, k1, k2;
    if (pixtm->list_type == 1)
      k1 = k2 = pixtm->pixel_list[i];
    else {
      k1 = pixtm->pixel_list[2 * i];
      k2 = pixtm->pixel_list[2 * i + 1];
    }
    for (ipix = k1; ipix <= k2; ipix++) {
      for (j = 0; j < pixtm->num_types; j++) {
        pixtm->timval[ipix][j] = scale * get_short();
      }
      if (with_sum) {
        int igain;
        for (igain = 0; igain < pixtm->num_gains; igain++)
          pixtm->pulse_sum_loc[igain][ipix] =
              (v0 ? get_short() : get_scount32());
        if (glob_only_selected) {
          for (igain = 0; igain < pixtm->num_gains; igain++)
            pixtm->pulse_sum_glob[igain][ipix] =
                (v0 ? get_short() : get_scount32());
        }
      }
    }
  }

  if (with_sum && pixtm->list_size > 0 && !glob_only_selected) {
    int igain;
    for (igain = 0; igain < pixtm->num_gains; igain++) {
      for (j = 0; j < pixtm->num_pixels; j++)
        pixtm->pulse_sum_glob[igain][j] = (v0 ? get_short() : get_scount32());
    }
  }

  pixtm->known = 1;

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_pixcalib(PixelCalibrated *pixcal) {
  int rc, ipix, npix;

  if (buffer == (BYTE *)NULL || pixcal == NULL)
    return -1;

  pixcal->known = 0;

  item_header->type = IO_TYPE_HESS_PIXELCALIB; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 0) {
    fprintf(stderr, "Unsupported calibrated pixel intensities version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  pixcal->tel_id = item_header->ident;
  npix = (int)get_count();
  if (npix > H_MAX_PIX) {
    fprintf(stderr,
            "Invalid number of pixels in calibrated pixel intensities: %d.\n",
            npix);
    get_item_end();
    return -1;
  }
  pixcal->num_pixels = npix;
  pixcal->int_method = (int)get_scount32();
  pixcal->list_known = (int)get_scount32();
  pixcal->list_size = 0;
  if (pixcal->list_known == 2) /* all pixels to be marked as significant */
  {
    for (ipix = 0; ipix < npix; ipix++)
      pixcal->significant[ipix] = 1;
  } else {
    for (ipix = 0; ipix < npix; ipix++)
      pixcal->significant[ipix] = 0;
  }
  if (pixcal->list_known == 1) /* selected pixels by list of pixel IDs */
  {
    int i;
    pixcal->list_size = (int)get_count32();
    for (i = 0; i < pixcal->list_size; i++) {
      ipix = pixcal->pixel_list[i] = (int)get_count32();
      pixcal->significant[ipix] = 1;
    }
    for (i = 0; i < pixcal->list_size; i++) {
      ipix = pixcal->pixel_list[i];
      pixcal->pixel_pe[ipix] = get_sfloat();
    }
  } else if (pixcal->list_known == -1) /* selected pixels by bit(s) */
  {
    get_vector_of_byte(pixcal->significant, pixcal->num_pixels);
    for (ipix = 0; ipix < pixcal->num_pixels; ipix++) {
      if (pixcal->significant[ipix])
        pixcal->pixel_pe[ipix] = get_sfloat();
    }
  } else if (pixcal->list_known == 2) /* all pixels significant */
  {
    for (ipix = 0; ipix < pixcal->num_pixels; ipix++)
      pixcal->pixel_pe[ipix] = get_sfloat();
  }

  pixcal->known = 1;

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_telimage(ImgData *img) {
  uint32_t flags;
  int rc;

  if (buffer == (BYTE *)NULL || img == NULL)
    return -1;

  img->known = 0;

  item_header->type = IO_TYPE_HESS_TELIMAGE; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 6) {
    fprintf(stderr, "Unsupported telescope image version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  /* Lots of small data was packed into the ID */
  flags = (uint32_t)item_header->ident;

  if (((item_header->ident & 0xff) |
       ((item_header->ident & 0x3f000000) >> 16)) != img->tel_id) {
    LOG(WARNING) << "Image data is for wrong telescope";
    get_item_end();
    return -1;
  }

  img->cut_id = (item_header->ident & 0xff000) >> 12;
  img->pixels = 0; /* always reset it */
  img->num_sat = 0;
  img->clip_amp = 0.;
  if (item_header->version >= 6)
    img->pixels = get_scount32();
  else if (item_header->version >= 2)
    img->pixels = get_short();
  if (item_header->version >= 4) {
    if (item_header->version >= 6)
      img->num_sat = get_scount32();
    else
      img->num_sat = get_short();
    if (img->num_sat > 0 && item_header->version >= 5)
      img->clip_amp = get_real();
  }

  img->amplitude = get_real();
  img->x = get_real();
  img->y = get_real();
  img->phi = get_real();
  img->l = get_real();
  img->w = get_real();
  img->num_conc = get_short();
  img->concentration = get_real();

  if ((flags & 0x100)) /* Error estimates of 1st+2nd moments in data */
  {
    img->x_err = get_real();
    img->y_err = get_real();
    img->phi_err = get_real();
    img->l_err = get_real();
    img->w_err = get_real();
  } else {
    img->x_err = 0.;
    img->y_err = 0.;
    img->phi_err = 0.;
    img->l_err = 0.;
    img->w_err = 0.;
  }

  if ((flags & 0x200)) /* 3rd+4th moments plus errors in data */
  {
    img->skewness = get_real();
    img->skewness_err = get_real();
    img->kurtosis = get_real();
    img->kurtosis_err = get_real();
  } else {
    img->skewness = 0.;
    img->skewness_err = -1.;
    img->kurtosis = 0.;
    img->kurtosis_err = -1.;
  }

  if ((flags & 0x400)) /* ADC sum of high-intensity pixels in data */
  {
    if (item_header->version <= 5) {
      img->num_hot = get_short();
      get_vector_of_real(img->hot_amp, img->num_hot);
      if (item_header->version >= 1)
        get_vector_of_int(img->hot_pixel, img->num_hot);
    } else {
      img->num_hot = get_scount32();
      get_vector_of_real(img->hot_amp, img->num_hot);
      if (item_header->version >= 1)
        get_vector_of_int_scount(img->hot_pixel, img->num_hot);
    }
  } else
    img->num_hot = 0;

  if ((flags & 0x800) &&
      item_header->version >= 3) /* New in version 3: timing summary */
  {
    img->tm_slope = get_real();
    img->tm_residual = get_real();
    img->tm_width1 = get_real();
    img->tm_width2 = get_real();
    img->tm_rise = get_real();
  } else
    img->tm_slope = img->tm_residual = img->tm_width1 = img->tm_width2 =
        img->tm_rise = 0.;

  img->known = 1;

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_televent(TelEvent *te, int what) {
  int rc;
  AdcData *raw;
  ImgData *img;
  int tel_id;
  int tel_img = 0;
  int iaux;
  static int w_sum = 0, w_samp = 0, w_pixtm = 0, w_pixcal = 0;

  if (buffer == (BYTE *)NULL || te == NULL)
    return -1;

  te->known = 0;
  te->readout_mode = 0;

  item_header->type = 0; /* No data type this time */
  if ((rc = get_item_begin()) < 0)
    return rc;
  tel_id = (item_header->type - IO_TYPE_HESS_TELEVENT) % 100 +
           100 * ((item_header->type - IO_TYPE_HESS_TELEVENT) / 1000);
  if (tel_id < 0 || tel_id != te->tel_id) {
    LOG(WARNING)
        << "Not a telescope event block or one for the wrong telescope";
    get_item_end();
    return -1;
  }
  if (item_header->version > 1) {
    fprintf(stderr, "Unsupported telescope event version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  te->glob_count = item_header->ident;

  if ((raw = te->raw) != NULL)
    raw->known = 0;
  if (te->pixtm != NULL)
    te->pixtm->known = 0;
  if ((img = te->img) != NULL) {
    int j;
    for (j = 0; j < te->num_image_sets; j++)
      img[j].known = 0;
  }
  te->pixeltrg_time.known = 0;

  /* Telescope-specific event header is always used */
  if ((rc = read_hess_televt_head(te)) < 0) {
    get_item_end();
    return rc;
  }

  /* Pixel lists only available since version 1 */
  te->trigger_pixels.pixels = te->image_pixels.pixels = 0;

  for (;;) {
    int nt = next_subitem_type();
    rc = -9;

    switch (nt) {
    case IO_TYPE_HESS_TELADCSUM:
      if ((what & (RAWDATA_FLAG | RAWSUM_FLAG)) == 0 || raw == NULL) {
        if (w_sum++ < 1)
          LOG(WARNING) << "Telescope raw data ADC sums not selected to be read";
        rc = skip_subitem();
        continue;
      }
      rc = read_hess_teladc_sums(raw);
      te->readout_mode = 0; /* ADC sums */
      if (rc == 0)
        raw->known = 1;
      raw->tel_id = te->tel_id; /* For IDs beyond 31, bits may be missing */
      break;

    case IO_TYPE_HESS_TELADCSAMP:
      if ((what & RAWDATA_FLAG) == 0 || raw == NULL) {
        if (w_samp++ < 1)
          LOG(WARNING)
              << "Telescope raw data ADC samples not selected to be read";
        rc = skip_subitem();
        continue;
      }
      if (raw->known) /* Preceded by sum data? */
        te->readout_mode =
            2; /* sum + samples (perhaps different zero suppression) */
      else {
        adc_reset(raw);       /* Do we need that? */
        te->readout_mode = 1; /* ADC samples, sums usually rebuilt */
      }
      rc = read_hess_teladc_samples(raw, what);
      if (rc == 0) {
        raw->known |= 2;
      }
      raw->tel_id = te->tel_id; /* For IDs beyond 31, bits may be missing (?) */
      break;

    case IO_TYPE_HESS_PIXELTIMING:
      if (te->pixtm == NULL || (what & TIME_FLAG) == 0) {
        if (w_pixtm++ < 1)
          LOG(WARNING) << "Telescope pixel timing data not selected to be read";
        rc = skip_subitem();
        continue;
      }
      rc = read_hess_pixtime(te->pixtm);
      break;

    case IO_TYPE_HESS_PIXELCALIB:
      if (te->pixcal == NULL) {
        if (w_pixcal++ < 1)
          LOG(WARNING) << "Telescope calibrated pixel intensities found, "
                          "allocating structures.";
        if ((te->pixcal = (PixelCalibrated *)calloc(
                 1, sizeof(PixelCalibrated))) == NULL) {
          LOG(WARNING) << "Not enough memory for PixelCalibrated";
          break;
        }
        te->pixcal->tel_id = tel_id;
      }
      rc = read_hess_pixcalib(te->pixcal);
      break;

    case IO_TYPE_HESS_TELIMAGE:
      if (img == NULL || (what & IMAGE_FLAG) == 0)
        break;
      if (tel_img >= te->max_image_sets) {
        LOG(WARNING) << "Not enough space to read all image sets";
        break;
      }
      if ((rc = read_hess_telimage(&img[tel_img])) == 0) {
        img[tel_img].known = 1;
        tel_img++;
      }
      te->num_image_sets = tel_img;
      break;

    case IO_TYPE_HESS_PIXELLIST: {
      long id = sub_item_header->ident = next_subitem_ident();
      int code = id / 1000000;
      int tid = id % 1000000;
      if (code == 0 && tid == te->tel_id) {
        if ((rc = read_hess_pixel_list(&te->trigger_pixels, &tid)) < 0) {
          get_item_end();
          return rc;
        }
      } else if (code == 1 && tid == te->tel_id) {
        if ((rc = read_hess_pixel_list(&te->image_pixels, &tid)) < 0) {
          get_item_end();
          return rc;
        }
        /* Fix for missing number of pixels in image of older data format: */
        if (te->img != NULL && te->img[0].known && te->img[0].pixels == 0)
          te->img[0].pixels = te->image_pixels.pixels;
      }
#if defined(H_MAX_TRG) && (H_MAX_TRG > 1)
      else if (code >= 10 && code < 10 + H_MAX_TRG && tid == te->tel_id) {
        int ixtrg = code - 10;
        if ((rc = read_hess_pixel_list(, &te->xtrigger_pixels[ixtrg], &tid)) <
            0) {
          get_item_end(, );
          return rc;
        }
      }
#endif
      else {
        fprintf(stderr, "Skipping pixel list of type %d for telescope %d\n",
                code, tid);
        skip_subitem();
      }
    } break;

    case IO_TYPE_HESS_PIXELTRG_TM:
      te->pixeltrg_time.tel_id = te->tel_id;
      if ((rc = read_hess_pixeltrg_time(&te->pixeltrg_time)) < 0) {
        get_item_end();
        return rc;
      }
      break;

    case IO_TYPE_HESS_AUX_DIGITAL_TRACE:
      iaux = item_header->ident;
      if (iaux >= 0 && iaux < MAX_AUX_TRACE_D) {
        if ((rc = read_hess_aux_trace_digital(&te->aux_trace_d[iaux])) < 0) {
          get_item_end();
          return rc;
        }
      }
      break;

    case IO_TYPE_HESS_AUX_ANALOG_TRACE:
      iaux = item_header->ident;
      if (iaux >= 0 && iaux < MAX_AUX_TRACE_A) {
        if ((rc = read_hess_aux_trace_analog(&te->aux_trace_a[iaux])) < 0) {
          get_item_end();
          return rc;
        }
      }
      break;

    default:
      if (nt > 0) {
        fprintf(
            stderr,
            "Skipping telescope event sub-item of type %d for telescope %d\n",
            nt, te->tel_id);
        rc = skip_subitem();
      } else
        return get_item_end();
    }

    if (rc < 0) {
      get_item_end();
      return rc;
    }

    te->known = 1;
  }

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_shower(ShowerParameters *sp) {
  int rc;

  if (buffer == (BYTE *)NULL || sp == NULL)
    return -1;

  sp->known = 0;

  item_header->type = IO_TYPE_HESS_SHOWER; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 2) {
    fprintf(stderr, "Unsupported reconstructed shower version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  sp->result_bits = item_header->ident;

  sp->num_trg = get_short();
  sp->num_read = get_short();
  sp->num_img = get_short();
  if (item_header->version >= 1)
    sp->img_pattern = get_int32();
  else
    sp->img_pattern = 0;
  if (item_header->version >= 2)
    get_vector_of_int(sp->img_list, sp->num_img);
  if ((sp->result_bits & 0x01)) {
    sp->Az = get_real();
    sp->Alt = get_real();
  } else
    sp->Az = sp->Alt = 0.;
  if ((sp->result_bits & 0x02)) {
    sp->err_dir1 = get_real();
    sp->err_dir2 = get_real();
    sp->err_dir3 = get_real();
  } else
    sp->err_dir1 = sp->err_dir2 = sp->err_dir3 = 0.;
  if ((sp->result_bits & 0x04)) {
    sp->xc = get_real();
    sp->yc = get_real();
  } else
    sp->xc = sp->yc = 0.;
  if ((sp->result_bits & 0x08)) {
    sp->err_core1 = get_real();
    sp->err_core2 = get_real();
    sp->err_core3 = get_real();
  } else
    sp->err_core1 = sp->err_core2 = sp->err_core3 = 0.;
  if ((sp->result_bits & 0x10)) {
    sp->mscl = get_real();
    sp->mscw = get_real();
  } else
    sp->mscl = sp->mscw = -1;
  if ((sp->result_bits & 0x20)) {
    sp->err_mscl = get_real();
    sp->err_mscw = get_real();
  } else
    sp->err_mscl = sp->err_mscw = 0.;
  if ((sp->result_bits & 0x40))
    sp->energy = get_real();
  else
    sp->energy = -1.;
  if ((sp->result_bits & 0x80))
    sp->err_energy = get_real();
  else
    sp->err_energy = 0.;
  sp->xmax = 0.;
  if ((sp->result_bits & 0x0100))
    sp->xmax = get_real();
  sp->err_xmax = 0.;
  if ((sp->result_bits & 0x0200))
    sp->err_xmax = get_real();

  sp->known = 1;

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_event(FullEvent *ev, int what) {
  int type, tel_id, itel, id, rc, j;

  if (buffer == (BYTE *)NULL || ev == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_EVENT; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version != 0) {
    fprintf(stderr, "Unsupported event data version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  id = item_header->ident;

  assert(ev->num_tel <= H_MAX_TEL);

  ev->central.glob_count = ev->central.teltrg_pattern =
      ev->central.teldata_pattern = ev->central.num_teltrg =
          ev->central.num_teldata = 0;
  reset_htime(&ev->central.cpu_time);
  reset_htime(&ev->central.gps_time);
  ev->num_teldata = 0;
  for (j = 0; j < ev->num_tel; j++) {
    ev->teldata[j].known = 0;
    ev->trackdata[j].raw_known = ev->trackdata[j].cor_known = 0;
  }
  ev->shower.known = 0;

  while ((type = next_subitem_type()) > 0) {
    if (type == IO_TYPE_HESS_CENTEVENT) {
      if ((rc = read_hess_centralevent(&ev->central)) < 0) {
        get_item_end();
        return rc;
      }
    }
#if (H_MAX_TEL > 100)
    else if (type >= IO_TYPE_HESS_TRACKEVENT &&
             type % 1000 < (IO_TYPE_HESS_TRACKEVENT % 1000) + 100 &&
             (type - IO_TYPE_HESS_TRACKEVENT) % 100 +
                     100 * ((type - IO_TYPE_HESS_TRACKEVENT) / 1000) <=
                 H_MAX_TEL)
#else
    else if (type >= IO_TYPE_HESS_TRACKEVENT &&
             type <= IO_TYPE_HESS_TRACKEVENT + H_MAX_TEL)
#endif
    {
      tel_id = (type - IO_TYPE_HESS_TRACKEVENT) % 100 +
               100 * ((type - IO_TYPE_HESS_TRACKEVENT) / 1000);
      if ((itel = find_tel_idx(tel_id)) < 0) {
        LOG(WARNING) << "Telescope number out of range for tracking data";
        get_item_end();
        return -1;
      }
      if ((rc = read_hess_trackevent(&ev->trackdata[itel])) < 0) {
        get_item_end();
        return rc;
      }
    }
#if (H_MAX_TEL > 100)
    else if (type >= IO_TYPE_HESS_TELEVENT &&
             type % 1000 < (IO_TYPE_HESS_TELEVENT % 1000) + 100 &&
             (type - IO_TYPE_HESS_TELEVENT) % 100 +
                     100 * ((type - IO_TYPE_HESS_TELEVENT) / 1000) <=
                 H_MAX_TEL)
#else
    else if (type >= IO_TYPE_HESS_TELEVENT &&
             type <= IO_TYPE_HESS_TELEVENT + H_MAX_TEL)
#endif
    {
      tel_id = (type - IO_TYPE_HESS_TELEVENT) % 100 +
               100 * ((type - IO_TYPE_HESS_TELEVENT) / 1000);
      if ((itel = find_tel_idx(tel_id)) < 0) {
        LOG(WARNING)
            << "Telescope number out of range for telescope event data";
        get_item_end();
        return -1;
      }
      if ((rc = read_hess_televent(&ev->teldata[itel], what)) < 0) {
        get_item_end();
        return rc;
      }
      if (ev->num_teldata < H_MAX_TEL && ev->teldata[itel].known) {
        ev->teldata_list[ev->num_teldata++] = ev->teldata[itel].tel_id;
      }
    } else if (type == IO_TYPE_HESS_SHOWER) {
      if ((rc = read_hess_shower(&ev->shower)) < 0) {
        get_item_end();
        return rc;
      }
    } else {
      char msg[200];
      sprintf(msg, "Invalid item type %d in event %d.", type, id);
      LOG(WARNING) << msg;
      get_item_end();
      return -1;
    }
  }

  /* Fill in the list of telescopes not present in earlier versions */
  /* of the central trigger block. Assumes only triggered telescopes */
  /* are actually read out or that the array has no more than 16 telescopes. */
  if (ev->central.num_teltrg == 0 && ev->central.teltrg_pattern != 0) {
    int nt = 0, nd = 0;
    /* For small arrays we have all the information in the bitmasks. */
    if (ev->num_tel <= 16) {
      for (itel = 0; itel < ev->num_tel && itel < 16; itel++) {
        if ((ev->central.teltrg_pattern & (1 << itel)) != 0) {
          ev->central.teltrg_time[nt] = 0.; /* Not available, set to zero */
          ev->central.teltrg_list[nt] = ev->teldata[itel].tel_id;
          nt++;
        }
        if ((ev->central.teldata_pattern & (1 << itel)) != 0) {
          ev->central.teldata_list[nd] = ev->teldata[itel].tel_id;
          nd++;
        }
      }
    }
    /* For larger arrays we assume only triggered telescopes were read out. */
    else {
      for (j = 0; j < ev->num_teldata; j++) {
        tel_id = ev->teldata_list[j];
        if ((itel = find_tel_idx(tel_id)) < 0)
          continue;
        if (ev->teldata[itel].known) {
          ev->central.teltrg_time[nt] = 0.;
          ev->central.teltrg_list[nt++] = ev->teldata[itel].tel_id;
          ev->central.teldata_list[nd++] = ev->teldata[itel].tel_id;
        }
      }
    }
    ev->central.num_teltrg = nt;
    ev->central.num_teldata = nd;
  }

  /* Some programs may require basic central trigger data even for mono data
     where historically no such data was stored. Replicate from the list of
     telescopes
     with data. */
  if (ev->num_tel > 0 && ev->central.num_teltrg == 0 &&
      ev->central.num_teldata == 0) {
    int k = 0;
    /* Reconstruct basic data in central trigger block */
    for (j = 0; j < ev->num_tel; j++) {
      if (ev->teldata[j].known) {
        ev->central.teltrg_type_mask[k] = 0;
        ev->central.teltrg_time[k] = 0.;
        ev->central.teltrg_list[k] = ev->teldata[j].tel_id;
        ev->central.teldata_list[k] = ev->teldata[j].tel_id;
        k++;
      }
    }
    ev->central.num_teltrg = ev->central.num_teldata = k;
    /* Recovered central data is identified by zero time values. */
    reset_htime(&ev->central.cpu_time);
    reset_htime(&ev->central.gps_time);
  }

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_calib_event(FullEvent *ev, int what,
                                             int *ptype) {
  int type, rc;

  if (buffer == (BYTE *)NULL || ev == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_CALIBEVENT; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version != 0) {
    fprintf(stderr, "Unsupported calibevent data version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }
  type = item_header->ident;
  if (ptype != NULL)
    *ptype = type;

  read_hess_event(ev, what);

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_Calib_Event() {
  int type = -1;
  rc = read_hess_calib_event(&hsdata->event, -1, &type);
  if (rc != 0) {
    LOG(WARNING) << "Read Calib Events error, rc = " << rc;
  }
}
int LAST_IO::SimTelIO::read_hess_mc_phot(MCEvent *mce) {
  int iarray = 0, itel = 0, itel_pe = 0, tel_id = 0, jtel = 0, type,
      nbunches = 0, max_bunches = 0, flags = 0;
  int npe = 0, pixels = 0, max_npe = 0;
  int rc;
  HEADER *item_header;
  double photons = 0.;
  if ((rc = begin_read_tel_array(&iarray)) < 0)
    return rc;
  while ((type = next_subitem_type()) > 0) {
    switch (type) {
    case IO_TYPE_MC_PHOTONS:
      /* The purpose of this first call to read_tel_photons is only
         to retrieve the array and telescope numbers (the original offset
         number without ignored telescopes, basically telescope ID minus one),
         etc. */
      /* With a NULL pointer argument, we expect rc = -10 */
      rc = read_tel_photons(0, &iarray, &itel_pe, &photons, NULL, &nbunches);
      if (rc != -10) {
        get_item_end();
        return -1;
      }
      tel_id = itel_pe + 1;
      itel = find_tel_idx(tel_id);
      if (itel < 0 || itel >= H_MAX_TEL) {
        LOG(WARNING) << "Invalid telescope number in MC photons";
        get_item_end();
        return -1;
      }
      if (nbunches > mce->mc_photons[itel].max_bunches ||
          (nbunches < mce->mc_photons[itel].max_bunches / 4 &&
           mce->mc_photons[itel].max_bunches > 10000) ||
          mce->mc_photons[itel].bunches == NULL) {
        if (mce->mc_photons[itel].bunches != NULL)
          free(mce->mc_photons[itel].bunches);
        if ((mce->mc_photons[itel].bunches =
                 (bunch *)calloc(nbunches, sizeof(bunch))) == NULL) {
          mce->mc_photons[itel].max_bunches = 0;
          get_item_end();
          return -4;
        }
        mce->mc_photons[itel].max_bunches = max_bunches = nbunches;
      } else
        max_bunches = mce->mc_photons[itel].max_bunches;

      /* Now really get the photon bunches */
      rc = read_tel_photons(max_bunches, &iarray, &jtel, &photons,
                            mce->mc_photons[itel].bunches, &nbunches);

      if (rc < 0) {
        mce->mc_photons[itel].nbunches = 0;
        get_item_end();
        return rc;
      } else
        mce->mc_photons[itel].nbunches = nbunches;

      if (jtel != itel) {
        LOG(WARNING) << "Inconsistent telescope number for MC photons";
        get_item_end();
        return -5;
      }
      break;
    case IO_TYPE_MC_PE:
      /* The purpose of this first call to read_photo_electrons is only
         to retrieve the array and telescope offset numbers (the original offset
         number without ignored telescopes, basically telescope ID minus one),
         the number of p.e.s and pixels etc. */
      /* Here we expect as well rc = -10 */
      rc = read_photo_electrons(H_MAX_PIX, 0, &iarray, &itel_pe, &npe, &pixels,
                                &flags, NULL, NULL, NULL, NULL, NULL);
      if (rc != -10) {
        get_item_end();
        return -1;
      }
      /* The itel_pe value may differ from the itel index value that we
         are looking for if the telescope simulation had ignored telescopes.
         This can be fixed but still assumes that base_telescope_number = 1
         was used - as all known simulations do. */
      tel_id = itel_pe + 1; /* Also note: 1 <= tel_id <= 1000 */
      itel = find_tel_idx(tel_id);
      if (itel < 0 || itel >= H_MAX_TEL) {
        LOG(WARNING) << "Invalid telescope number in MC photons";
        get_item_end();
        return -1;
      }
      if (pixels > H_MAX_PIX) {
        LOG(WARNING) << "Invalid number of pixels in MC photons";
        get_item_end();
        return -1;
      }
      /* If the current p.e. list buffer is too small or
         non-existent or if it is unnecessarily large,
         we (re-) allocate a p.e. list buffer for p.e. times
         and, if requested, for amplitudes. */
      if (npe > mce->mc_pe_list[itel].max_npe ||
          (npe < mce->mc_pe_list[itel].max_npe / 4 &&
           mce->mc_pe_list[itel].max_npe > 20000) ||
          mce->mc_pe_list[itel].atimes == NULL ||
          (mce->mc_pe_list[itel].amplitudes == NULL && (flags & 1) != 0)) {
        if (mce->mc_pe_list[itel].atimes != NULL)
          free(mce->mc_pe_list[itel].atimes);
        if ((mce->mc_pe_list[itel].atimes =
                 (double *)calloc(npe > 0 ? npe : 1, sizeof(double))) == NULL) {
          mce->mc_pe_list[itel].max_npe = 0;
          get_item_end();
          return -4;
        }
        if (mce->mc_pe_list[itel].amplitudes != NULL)
          free(mce->mc_pe_list[itel].amplitudes);
        /* If the amplitude bit in flags is set, also check for that part */
        if ((flags & 1) != 0) {
          if ((mce->mc_pe_list[itel].amplitudes = (double *)calloc(
                   npe > 0 ? npe : 1, sizeof(double))) == NULL) {
            mce->mc_pe_list[itel].max_npe = 0;
            get_item_end();
            return -4;
          }
        }
        mce->mc_pe_list[itel].max_npe = max_npe = npe;
      } else
        max_npe = mce->mc_pe_list[itel].max_npe;

#ifdef STORE_PHOTO_ELECTRONS
      rc = read_photo_electrons(
          , H_MAX_PIX, max_npe, &iarray, &jtel, &npe, &pixels,
          &mce->mc_pe_list[itel].flags, mce->mc_pe_list[itel].pe_count,
          mce->mc_pe_list[itel].itstart, mce->mc_pe_list[itel].atimes,
          mce->mc_pe_list[itel].amplitudes, mce->mc_pe_list[itel].photon_count);
#else
      rc = read_photo_electrons(
          H_MAX_PIX, max_npe, &iarray, &jtel, &npe, &pixels,
          &mce->mc_pe_list[itel].flags, mce->mc_pe_list[itel].pe_count,
          mce->mc_pe_list[itel].itstart, mce->mc_pe_list[itel].atimes,
          mce->mc_pe_list[itel].amplitudes, NULL);
#endif

      mce->mc_pe_list[itel].pixels = pixels;

      if (rc < 0) {
        mce->mc_pe_list[itel].npe = 0;
        get_item_end();
        return rc;
      } else {
        mce->mc_pe_list[itel].npe = npe;
      }

      break;
    default:
      fprintf(stderr,
              "Fix me: unexpected item type %d in read_hess_mc_phot()\n", type);
      skip_subitem();
    }
  }

  return end_read_tel_array();
}
void LAST_IO::SimTelIO::Read_MC_Phot() {
  if (hsdata->run_header.ntel > 0) {
    rc = read_hess_mc_phot(&hsdata->mc_event);
  }
  if (rc != 0) {
    LOG(WARNING) << "Error reading MC photons rc = " << rc;
  }
}
void LAST_IO::SimTelIO::Read_Event() {
  rc = read_hess_event(&hsdata->event, -1);
  if (rc != 0) {
    LOG(WARNING) << "Error reading event rc = " << rc;
  }
  num_events++;
}

void LAST_IO::SimTelIO::Read_MC_Shower() {
  rc = read_hess_mc_shower(&hsdata->mc_shower);
}
int LAST_IO::SimTelIO::read_photo_electrons(int max_pixel, int max_pe,
                                            int *array, int *tel, int *npe,
                                            int *pixels, int *flags,
                                            int *pe_counts, int *tstart,
                                            double *t, double *a,
                                            int *photon_counts) {
  int i, it, ipix, rc, nonempty;

  if (buffer == (BYTE *)NULL)
    return -1;

  item_header->type = IO_TYPE_MC_PE; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version < 1 || item_header->version > 3) {
    fflush(stdout);
    fprintf(stderr, "Invalid version %d of photo-electrons block.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  *array = item_header->ident / 1000;
  *tel = item_header->ident % 1000;

  *npe = get_long();
  *pixels = get_long();
  if (item_header->version > 1)
    *flags = get_short();
  else
    *flags = 0;
  nonempty = get_long();

  /* We may have a first attempt for finding out how many there are. */
  if (pe_counts == NULL || tstart == NULL || t == NULL) {
    unget_item();
    return -10;
  }

  if ((*pixels) > max_pixels || (*npe) > max_pe || (*pixels) < 0 ||
      (*npe) < 0 || nonempty > (*pixels) || nonempty < 0) {
    if (*pixels > max_pixels || *pixels < 0) {
      fflush(stdout);
      fprintf(stderr,
              "Too many pixels specified in photo-electrons block: %d > %d\n",
              *pixels, max_pixels);
    }
    if (*npe > max_pe || *npe < 0) {
      fflush(stdout);
      fprintf(stderr, "Number of photo-electrons exceeds list size: %d > %d\n",
              *npe, max_pe);
    }
    if (nonempty > (*pixels) || nonempty < 0) {
      fflush(stdout);
      fprintf(stderr, "Number of non-empty pixels not consistent with total "
                      "number: %d / %d\n",
              nonempty, *pixels);
    }
    get_item_end();
    return -4;
  }
  for (ipix = 0; ipix < *pixels; ipix++)
    pe_counts[ipix] = tstart[ipix] = 0;

  if (((*flags) & 4) != 0 && photon_counts != NULL)
    for (ipix = 0; ipix < *pixels; ipix++)
      photon_counts[ipix] = 0;

  for (i = it = 0; i < nonempty; i++) {
    /* FIXME: limiting number of pixels to <= 32767 */
    if (item_header->version > 2)
      ipix = get_count();
    else
      ipix = get_short();
    if (ipix < 0 || ipix >= max_pixels) {
      LOG(WARNING) << "Invalid pixel number for photo-electron list";
      get_item_end();
      return -5;
    }
    pe_counts[ipix] = get_long();
    if (pe_counts[ipix] < 0 || pe_counts[ipix] > max_pe) {
      fflush(stdout);
      fprintf(stderr, "Invalid number of photo-electrons for pixel %d: %d\n",
              ipix, pe_counts[ipix]);
      get_item_end();
      return -5;
    }
    if (it + pe_counts[ipix] > max_pe) {
      pe_counts[ipix] = 0;
      fflush(stdout);
      fprintf(stderr, "Would read beyond end of photo-electron list\n");
      get_item_end();
      return -5;
    }
    tstart[ipix] = it;
    get_vector_of_real(t + it, pe_counts[ipix]);
    if (((*flags) & 1) != 0 && a != NULL)
      get_vector_of_real(a + it, pe_counts[ipix]);
    else if (((*flags) & 1) != 0 && a == NULL) {
      int j;
      for (j = 0; j < pe_counts[ipix]; j++)
        (void)get_real(); /* Amplitudes available but not used */
    }
    it += pe_counts[ipix];
  }

  if (((*flags) & 4) != 0 && photon_counts != NULL) {
    nonempty = get_long(); /* Non-empty with photons this time */
    for (i = it = 0; i < nonempty; i++) {
      ipix = get_short();
      if (ipix < 0 || ipix >= max_pixels) {
        LOG(WARNING) << "Invalid pixel number for photon count";
        get_item_end();
        return -5;
      }
      photon_counts[ipix] = get_long();
    }
  }

  return get_item_end();
}
int LAST_IO::SimTelIO::read_hess_pixel_list(PixelList *pl, int *telescope) {
  int rc;

  if (buffer == (BYTE *)NULL || pl == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_PIXELLIST; /* Data type */

  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 1) {
    fprintf(stderr, "Unsupported pixel list version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  pl->code = item_header->ident / 1000000;
  if (telescope != NULL)
    *telescope = item_header->ident % 1000000;

  pl->pixels = (item_header->version < 1 ? get_short() : get_scount32());
  if (pl->pixels > H_MAX_PIX) {
    fprintf(
        stderr,
        "Got a pixel list with %d pixels but can only handle lists up to %d.\n",
        pl->pixels, H_MAX_PIX);
    pl->pixels = 0;
    get_item_end();
    return -1;
  }

  if (item_header->version < 1)
    get_vector_of_int(pl->pixel_list, pl->pixels);
  else
    get_vector_of_int_scount(pl->pixel_list, pl->pixels);

  return get_item_end();
}
int LAST_IO::SimTelIO::read_hess_mc_shower(MCShower *mcs) {
  int rc, j;

  if (buffer == (BYTE *)NULL || mcs == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_MC_SHOWER; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 2) {
    fprintf(stderr, "Unsupported MC shower version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  mcs->shower_num = item_header->ident;

  mcs->primary_id = get_int32();
  mcs->energy = get_real();
  mcs->azimuth = get_real();
  mcs->altitude = get_real();
  if (item_header->version >= 1)
    mcs->depth_start = get_real();
  mcs->h_first_int = get_real();
  mcs->xmax = get_real();
  mcs->hmax = mcs->emax = mcs->cmax = 0.;
  if (item_header->version >= 1) {
    mcs->hmax = get_real();
    mcs->emax = get_real();
    mcs->cmax = get_real();
  }

  /* Get longitudinal/vertical profiles */
  mcs->num_profiles = get_short();
  for (j = 0; j < mcs->num_profiles && j < H_MAX_PROFILE; j++) {
    int skip = 0;
    mcs->profile[j].id = get_int32();
    mcs->profile[j].num_steps = get_int32();
    /* If there are more steps than we need, a dynamically */
    /* allocated content buffer may be reallocated. */
    /* Otherwise, the contents are skipped. */
    if (mcs->profile[j].num_steps > mcs->profile[j].max_steps) {
      if (mcs->profile[j].content != NULL) {
        if (mcs->profile[j].max_steps > 0) {
          free(mcs->profile[j].content);
          mcs->profile[j].content = NULL;
        } else
          skip = 1; // We have not enough space for results.
      }
    }
    mcs->profile[j].start = get_real();
    mcs->profile[j].end = get_real();
    if (mcs->profile[j].num_steps > 0)
      mcs->profile[j].binsize = (mcs->profile[j].end - mcs->profile[j].start) /
                                (double)mcs->profile[j].num_steps;
    if (mcs->profile[j].content == NULL) {
      mcs->profile[j].content =
          (double *)calloc(sizeof(double), mcs->profile[j].num_steps);
      if (mcs->profile[j].content == NULL) {
        fprintf(stderr, "Not enough memory.\n");
        get_item_end();
        return -5;
      }
      mcs->profile[j].max_steps = mcs->profile[j].num_steps;
    }
    if (skip) {
      int i;
      for (i = 0; i < mcs->profile[j].num_steps; i++)
        (void)get_real();
      mcs->profile[j].num_steps *= -1;
    } else
      get_vector_of_real(mcs->profile[j].content, mcs->profile[j].num_steps);
  }

  if (item_header->version >= 2)
    read_shower_extra_parameters(&mcs->extra_parameters);
  else
    read_shower_extra_parameters(&mcs->extra_parameters);

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_mc_event(MCEvent *mce) {
  int rc, itel;

  if (buffer == (BYTE *)NULL || mce == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_MC_EVENT; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 2) {
    fprintf(stderr, "Unsupported MC event version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  mce->event = item_header->ident;

  mce->shower_num = get_int32();
  mce->xcore = get_real();
  mce->ycore = get_real();
  if (item_header->version >= 2)
    mce->aweight = get_real();
  else
    mce->aweight = 0.;

  /* Reset substructure to avoid taking leftover data from */
  /* previous events unnoticed. */
  mce->mc_pesum.num_tel = 0;
  mce->mc_pesum.event = -1;
  for (itel = 0; itel < H_MAX_TEL; itel++) {
    mce->mc_pesum.num_pe[itel] = -1;
    mce->mc_photons[itel].nbunches = -1;
    mce->mc_pe_list[itel].npe = -1;
    mce->mc_pesum.photons[itel] = mce->mc_pesum.photons_atm[itel] =
        mce->mc_pesum.photons_atm_3_6[itel] =
            mce->mc_pesum.photons_atm_400[itel] =
                mce->mc_pesum.photons_atm_qe[itel] = 0.;
    mce->mc_phot_list[itel].nphot = 0;
  }

  /* Version 0 did have more data here which we just ignore now. */

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_Mc_Event() {
  rc = read_hess_mc_event(&hsdata->mc_event);
  if (rc != 0) {
    LOG(WARNING) << "Error reading MC event data";
  }
}
int LAST_IO::SimTelIO::read_hess_mc_pe_sum(MCpeSum *mcpes) {
  int rc, i;

  if (buffer == (BYTE *)NULL || mcpes == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_MC_PE_SUM; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 2) {
    fprintf(stderr, "Unsupported MC p.e. sums version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  mcpes->event = item_header->ident;

  mcpes->shower_num = get_int32();

  mcpes->num_tel = get_int32();
  if (mcpes->num_tel > H_MAX_TEL) {
    LOG(WARNING) << "Too many telescopes in p.e. sum";
    return -1;
  }

  get_vector_of_int32(mcpes->num_pe, mcpes->num_tel);
  get_vector_of_int32(mcpes->num_pixels, mcpes->num_tel);

  for (i = 0; i < mcpes->num_tel; i++) {
    int non_empty, j;
    int list[H_MAX_PIX];
    int pe[H_MAX_PIX];

    if (mcpes->num_pe[i] <= 0 || mcpes->num_pixels[i] <= 0)
      continue;

    if (mcpes->num_pixels[i] > H_MAX_PIX) {
      LOG(WARNING) << "Too many pixels in MC p.e. sum";
      get_item_end();
      return -1;
    }
    non_empty = get_short();
    get_vector_of_int(list, non_empty);
    get_vector_of_int32(pe, non_empty);
    for (j = 0; j < mcpes->num_pixels[i]; j++)
      mcpes->pix_pe[i][j] = 0.;
    for (j = 0; j < non_empty; j++) {
      if (list[j] < 0 || list[j] >= H_MAX_PIX)
        continue;
      mcpes->pix_pe[i][list[j]] = pe[j];
    }
  }

  if (item_header->version >= 1) {
    /* Since version 1: */
    get_vector_of_real(mcpes->photons, mcpes->num_tel);
    get_vector_of_real(mcpes->photons_atm, mcpes->num_tel);
    get_vector_of_real(mcpes->photons_atm_3_6, mcpes->num_tel);
    get_vector_of_real(mcpes->photons_atm_qe, mcpes->num_tel);
    if (item_header->version >= 2)
      get_vector_of_real(mcpes->photons_atm_400, mcpes->num_tel);
    else {
      int j;
      for (j = 0; j < mcpes->num_tel; j++)
        mcpes->photons_atm_400[j] = 0.;
    }
  } else {
    int j;
    for (j = 0; j < mcpes->num_tel; j++)
      mcpes->photons[j] = mcpes->photons_atm[j] = mcpes->photons_atm_3_6[j] =
          mcpes->photons_atm_qe[j] = mcpes->photons_atm_400[j] = 0.;
  }

  return get_item_end();
}
int LAST_IO::SimTelIO::read_hess_mc_pixel_moni(MCPixelMonitor *mcpixmon) {
  int flags, npix, ngain;
  int rc = 0;

  if (buffer == (BYTE *)NULL || mcpixmon == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_MC_PIXMON; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 0) {
    fprintf(stderr, "Unsupported MC pixel monitor version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  /* If there is already a telescope ID in the struct it must match the data */
  if (item_header->ident != mcpixmon->tel_id && mcpixmon->tel_id != 0) {
    LOG(WARNING) << "MC Pixel Monitor block is for wrong telescope";
    get_item_end();
    return -1;
  }

  mcpixmon->tel_id = item_header->ident;
  flags = get_int32();
  npix = get_int32();
  ngain = get_short();

  /* Check that the reported number of pixels is in the valid range and,
     if the struct is already in use, it does match the prior value. */
  if (npix <= 0 || npix > H_MAX_PIX ||
      (npix > 0 && mcpixmon->num_pixels > 0 && npix != mcpixmon->num_pixels)) {
    LOG(WARNING) << "MC Pixel Monitor block has invalid number of pixels";
    get_item_end();
    return -1;
  }

  /* Check that the reported number of gains is in the valid range and,
     if the struct is already in use, it does match the prior value. */
  if (ngain <= 0 || ngain > H_MAX_GAINS ||
      (ngain > 0 && mcpixmon->num_gains > 0 && ngain != mcpixmon->num_gains)) {
    LOG(WARNING) << "MC Pixel Monitor block has invalid number of gains";
    get_item_end();
    return -1;
  }

  mcpixmon->flags |= flags;
  mcpixmon->num_pixels = npix;
  mcpixmon->num_gains = ngain;

  if ((mcpixmon->flags & 0x01) != 0)
    get_vector_of_real(mcpixmon->nsb_pe_rate, mcpixmon->num_pixels);
  if ((mcpixmon->flags & 0x02) != 0)
    get_vector_of_real(mcpixmon->qe_rel, mcpixmon->num_pixels);
  if ((mcpixmon->flags & 0x04) != 0)
    get_vector_of_real(mcpixmon->gain_rel, mcpixmon->num_pixels);
  if ((mcpixmon->flags & 0x08) != 0)
    get_vector_of_real(mcpixmon->hv_rel, mcpixmon->num_pixels);
  if ((mcpixmon->flags & 0x10) != 0)
    get_vector_of_real(mcpixmon->current, mcpixmon->num_pixels);
  if ((mcpixmon->flags & 0x20) != 0)
    get_vector_of_real(mcpixmon->fadc_amp[0], mcpixmon->num_pixels);
  if ((mcpixmon->flags & 0x40) != 0 && ngain > 1)
    get_vector_of_real(mcpixmon->fadc_amp[1], mcpixmon->num_pixels);
  if ((mcpixmon->flags & 0x80) != 0)
    get_vector_of_byte(mcpixmon->disabled, mcpixmon->num_pixels);

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_MC_Pe_Sum() {
  rc = read_hess_mc_pe_sum(&hsdata->mc_event.mc_pesum);
}
int LAST_IO::SimTelIO::read_hess_tel_monitor(TelMoniData *mon) {
  int what, rc, ns, np, nd, ng, tel_id;

  if (buffer == (BYTE *)NULL || mon == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_TEL_MONI; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version != 0 && item_header->version != 1) {
    fprintf(stderr, "Unsupported telescope monitoring version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  tel_id =
      ((item_header->ident & 0xff) | ((item_header->ident & 0x3f000000) >> 16));
  if (tel_id != mon->tel_id) {
    LOG(WARNING) << "Monitor block is for wrong telescope";
    get_item_end();
    return -1;
  }

  what = ((item_header->ident & 0xffff00) >> 8) & 0xffff;

  mon->known |= get_short();
  mon->new_parts = get_short();

  mon->monitor_id = get_int32();
  get_time_blob(&mon->moni_time);

  /* Dimensions of various things */
  if (item_header->version == 0) {
    ns = get_short();
    np = get_short();
    nd = get_short();
    ng = get_short();
  } else {
    ns = get_scount32();
    np = get_scount32();
    nd = get_scount32();
    ng = get_scount32();
  }
  if ((mon->num_sectors != ns && mon->num_sectors != 0) ||
      (mon->num_pixels != np && mon->num_pixels != 0) ||
      (mon->num_drawers != nd && mon->num_drawers != 0) ||
      (mon->num_gains != ng && mon->num_gains != 0)) {
    LOG(WARNING) << "Monitor data is for a camera of different setup";
    if (ns > H_MAX_SECTORS || np > H_MAX_PIX || nd > H_MAX_DRAWERS ||
        ng > H_MAX_GAINS) {
      LOG(WARNING) << "Monitor data has invalid camera setup";
      get_item_end();
      mon->new_parts = 0;
      return -1;
    }
  }

  mon->num_sectors = ns;
  mon->num_pixels = np;
  mon->num_drawers = nd;
  mon->num_gains = ng;

  if ((what & 0x01)) /* Status only */
  {
    get_time_blob(&mon->status_time);
    mon->status_bits = get_int32();
  }
  if ((what & 0x02)) /* Counts + Rates */
  {
    get_time_blob(&mon->trig_time);
    mon->coinc_count = get_long();
    mon->event_count = get_long();
    mon->trigger_rate = get_real();
    get_vector_of_real(mon->sector_rate, mon->num_sectors);
    mon->event_rate = get_real();
    mon->data_rate = get_real();
    mon->mean_significant = get_real();
  }
  if ((what & 0x04)) /* Pedestals + noise */
  {
    int j, i;
    get_time_blob(&mon->ped_noise_time);
    mon->num_ped_slices = get_short();
    for (j = 0; j < mon->num_gains; j++) {
      get_vector_of_real(mon->pedestal[j], mon->num_pixels);
      if (mon->num_ped_slices > 0) {
        for (i = 0; i < mon->num_pixels; i++)
          mon->pedsamp[j][i] =
              mon->pedestal[j][i] / (double)mon->num_ped_slices;
      } else {
        for (i = 0; i < mon->num_pixels; i++)
          mon->pedsamp[j][i] = 0.;
      }
    }
    for (j = 0; j < mon->num_gains; j++)
      get_vector_of_real(mon->noise[j], mon->num_pixels);
  }
  if ((what & 0x08)) /* HV + temperatures (CNTRLMon) + others */
  {
    int j;
    get_time_blob(&mon->hv_temp_time);
    mon->num_drawer_temp = get_short();
    mon->num_camera_temp = get_short();

    /* From CNTRLMon messages: */
    get_vector_of_uint16(mon->hv_v_mon, mon->num_pixels);
    get_vector_of_uint16(mon->hv_i_mon, mon->num_pixels);
    get_vector_of_byte(mon->hv_stat, mon->num_pixels);
    for (j = 0; j < mon->num_drawers; j++)
      get_vector_of_short(mon->drawer_temp[j], mon->num_drawer_temp);

    /* From other sources: */
    get_vector_of_short(mon->camera_temp, mon->num_camera_temp);
    /* ... + various voltages and currents to be defined ... */
  }
  if ((what & 0x10)) /* Pixel scalers + DC currents (CNTRLCpt) */
  {
    get_time_blob(&mon->dc_rate_time);
    get_vector_of_uint16(mon->current, mon->num_pixels);
    get_vector_of_uint16(mon->scaler, mon->num_pixels);
  }
  if ((what & 0x20)) /* HV + thresholds settings (CNTRLSc+CNTRLHt) */
  {
    get_time_blob(&mon->set_hv_thr_time);
    get_vector_of_uint16(mon->hv_dac, mon->num_pixels);
    get_vector_of_uint16(mon->thresh_dac, mon->num_drawers);
    get_vector_of_byte(mon->hv_set, mon->num_pixels);
    get_vector_of_byte(mon->trig_set, mon->num_pixels);
  }
  if ((what & 0x40)) /* DAQ configuration (CNTRLDAQ) */
  {
    get_time_blob(&mon->set_daq_time);
    get_vector_of_uint16(&mon->daq_conf, 1);
    get_vector_of_uint16(&mon->daq_scaler_win, 1);
    get_vector_of_uint16(&mon->daq_nd, 1);
    get_vector_of_uint16(&mon->daq_acc, 1);
    get_vector_of_uint16(&mon->daq_nl, 1);
  }
  if ((what & 0x80)) /* Pedestal compensation */
  {
    int j;
    get_time_blob(&mon->set_pedcomp_time);
    for (j = 0; j < mon->num_gains; j++) {
      get_vector_of_int32(mon->ped_comp_rel[j], mon->num_pixels);
    }
  }

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_Tel_Moni() {
  tel_id =
      (item_header->ident & 0xff) | ((item_header->ident & 0x3f000000) >> 16);
  if ((itel = find_tel_idx(tel_id)) < 0) {
    LOG(ERROR) << " Telsceope monitor block for unkown telescope " << tel_id;
  }
  rc = read_hess_tel_monitor(&hsdata->tel_moni[itel]);
}
int LAST_IO::SimTelIO::read_hess_laser_calib(LasCalData *lcd) {
  int j, np, ng, rc;

  if (buffer == (BYTE *)NULL || lcd == NULL)
    return -1;
  lcd->known = 0;

  item_header->type = IO_TYPE_HESS_LASCAL; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version > 3) {
    fprintf(stderr, "Unsupported laser calibration version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  if (lcd->tel_id != item_header->ident) {
    LOG(WARNING) << "Laser calibration data is for wrong telescope";
    get_item_end();
    return -1;
  }

  np = get_short();
  ng = get_short();
  if ((np != lcd->num_pixels && lcd->num_pixels != 0) ||
      (ng != lcd->num_gains && lcd->num_gains != 0)) {
    LOG(WARNING) << "Laser calibration data is for different setup";
  }
  lcd->num_pixels = np;
  lcd->num_gains = ng;
  if (np > H_MAX_PIX || ng > H_MAX_GAINS) {
    LOG(WARNING) << "Laser calibration data is bad setup";
    get_item_end();
    return -1;
  }

  lcd->lascal_id = get_int32();

  for (j = 0; j < lcd->num_gains; j++) {
    int i;
    get_vector_of_real(lcd->calib[j], lcd->num_pixels);
    for (i = 0; i < np; i++) {
      if (lcd->calib[j][i] != 0.) {
        lcd->known = 1;
        break;
      }
    }
  }

  /* Set compatibility values for older data versions */
  for (j = 0; j < lcd->num_gains; j++) {
    int i;
    lcd->max_int_frac[j] = lcd->max_pixtm_frac[j] = 0.;
    for (i = 0; i < np; i++) {
      lcd->tm_calib[j][i] = 0.;
      lcd->ff_corr[j][i] = 1.0;
    }
  }

  /* Fractions of reference pulses in integration window */
  if (item_header->version >= 1) {
    for (j = 0; j < lcd->num_gains; j++) {
      lcd->max_int_frac[j] = get_real();
      lcd->max_pixtm_frac[j] = get_real();
    }
  }

  /* Time calibration */
  if (item_header->version >= 2) {
    for (j = 0; j < lcd->num_gains; j++)
      get_vector_of_real(lcd->tm_calib[j], lcd->num_pixels);
  }

  /* Flat-field corrections (which are included in calibration factors) */
  if (item_header->version >= 3) {
    for (j = 0; j < lcd->num_gains; j++)
      get_vector_of_real(lcd->ff_corr[j], lcd->num_pixels);
  }

  if (!lcd->known) {
    char message[1024];
    sprintf(message,
            "Laser calibration for telescope %d was not properly filled.",
            lcd->tel_id);
    LOG(WARNING) << message;
  }

  return get_item_end();
}

void LAST_IO::SimTelIO::Read_Las_Cal() {
  tel_id = item_header->ident;
  if ((itel = find_tel_idx(tel_id)) < 0) {
    LOG(ERROR) << " Laser calibration block for unkown telescope " << tel_id;
  }
  rc = read_hess_laser_calib(&hsdata->tel_lascal[itel]);
}

int LAST_IO::SimTelIO::read_hess_run_stat(RunStat *rs) {
  int rc;

  if (buffer == (BYTE *)NULL || rs == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_RUNSTAT; /* Data type */

  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version != 0) {
    fprintf(stderr, "Unsupported run statistics version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  rs->run_num = item_header->ident;

  rs->num_tel = get_int32();
  rs->num_central_trig = get_int32();
  get_vector_of_int32(rs->tel_ids, rs->num_tel);
  get_vector_of_int32(rs->num_local_trig, rs->num_tel);
  get_vector_of_int32(rs->num_local_sys_trig, rs->num_tel);
  get_vector_of_int32(rs->num_events, rs->num_tel);

  return get_item_end();
}

int LAST_IO::SimTelIO::read_hess_mc_run_stat(MCRunStat *mcrs) {
  int rc;

  if (buffer == (BYTE *)NULL || mcrs == NULL)
    return -1;

  item_header->type = IO_TYPE_HESS_MC_RUNSTAT; /* Data type */

  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version != 0) {
    fprintf(stderr, "Unsupported MC run statistics version: %d.\n",
            item_header->version);
    get_item_end();
    return -1;
  }

  mcrs->run_num = item_header->ident;

  mcrs->num_showers = get_int32();
  mcrs->num_events = get_int32();

  return get_item_end();
}
void LAST_IO::SimTelIO::Read_Mc_Run_Stat() {
  rc = read_hess_mc_run_stat(&hsdata->mc_run_stat);
}
void LAST_IO::SimTelIO::Read_Run_Stat() {
  rc = read_hess_run_stat(&hsdata->run_stat);
}
int LAST_IO::SimTelIO::begin_read_tel_array(int *array) {
  int rc;

  item_header->type = IO_TYPE_MC_TELARRAY; /* Data type */
  rc = get_item_begin();
  if (item_header->version != 0)
    return -1;

  *array = item_header->ident;
  return rc;
}
int LAST_IO::SimTelIO::end_read_tel_array() { return get_item_end(); }

void LAST_IO::SimTelIO::set_tel_idx(int ntel, int *idx) {
  int i;
  for (i = 0; (size_t)i < sizeof(g_tel_idx[g_tel_idx_ref]) /
                              sizeof(g_tel_idx[g_tel_idx_ref][0]);
       i++)
    g_tel_idx[g_tel_idx_ref][i] = -1;
  for (i = 0; i < ntel; i++) {
    if (idx[i] < 0 ||
        (size_t)idx[i] >= sizeof(g_tel_idx[g_tel_idx_ref]) /
                              sizeof(g_tel_idx[g_tel_idx_ref][0])) {
      fprintf(stderr, "Telescope ID %d is outside of valid range\n", idx[i]);
      exit(1);
    }
    if (g_tel_idx[g_tel_idx_ref][idx[i]] != -1) {
      fprintf(stderr, "Multiple telescope ID %d\n", idx[i]);
      fprintf(stderr, "Telescope ID %d is outside of valid range\n", idx[i]);
      exit(1);
    }
    g_tel_idx[g_tel_idx_ref][idx[i]] = i;
  }
  g_tel_idx_init[g_tel_idx_ref] = 1;
}

int LAST_IO::SimTelIO::find_tel_idx(int tel_id) {
  if (!g_tel_idx_init[g_tel_idx_ref])
    return -2;
  if (tel_id < 0 || (size_t)tel_id >= sizeof(g_tel_idx[g_tel_idx_ref]) /
                                          sizeof(g_tel_idx[g_tel_idx_ref][0]))
    return -1;
  return g_tel_idx[g_tel_idx_ref][tel_id];
}

void LAST_IO::SimTelIO::Check() {
  for (;;) {
    if (find_io_block() != 0) {
      break;
    }
    if (read_io_block() != 0) {
      break;
    }
    if (hsdata == NULL && item_header->type > IO_TYPE_HESS_RUNHEADER &&
        item_header->type < IO_TYPE_HESS_RUNHEADER + 200) {
      LOG(WARNING) << "Tring to read event data before run header block";
      continue;
    }
    switch ((int)item_header->type) {

    case IO_TYPE_HESS_RUNHEADER:
      Read_Runheader();
      break;
    default:
      std::cout << "Ignoreing data block type " << item_header->type
                << std::endl;
      break;
    }
  }
}

void LAST_IO::SimTelIO::get_adcsum_as_uint16(uint32_t *adc_sum, int n) {
  uint16_t short_adc_sum[H_MAX_PIX];
  int ipix;
  get_vector_of_uint16(short_adc_sum, n);
  for (ipix = 0; ipix < n; ipix++)
    adc_sum[ipix] = short_adc_sum[ipix];
}
void LAST_IO::SimTelIO::get_adcsum_differential(uint32_t *adc_sum, int n) {
  int ipix;
  int32_t prev_amp = 0, this_amp;
  for (ipix = 0; ipix < n; ipix++) {
    this_amp = get_scount32() + prev_amp;
    prev_amp = this_amp;
    adc_sum[ipix] = (uint32_t)this_amp;
  }
}
int LAST_IO::SimTelIO::read_tel_photons(int max_bunches, int *array, int *tel,
                                        double *photons, struct bunch *bunches,
                                        int *nbunches) {
  int i, rc;
  double check_photons = 0.;
  int is_particle_block = 0;

  if (buffer == (BYTE *)NULL)
    return -1;

  item_header->type = IO_TYPE_MC_PHOTONS; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version % 1000 != 0) {
    get_item_end();
    return -1;
  }

  *array = get_short();
  *tel = get_short();
  *photons = get_real();
  *nbunches = get_long();
  if (*array == 999 && *tel == 999)
    is_particle_block = 1;

  /* We may have a first attempt at finding out the numbers. */
  if (bunches == NULL) {
    unget_item();
    return -10;
  }

  if (*nbunches > max_bunches) {
    fflush(NULL);
    fprintf(stderr, "Too many photon bunches in item type %d.\n",
            IO_TYPE_MC_PHOTONS);
    fflush(NULL);
    get_item_end();
    return -1;
  }

  if (item_header->version / 1000 == 0) /* The long format */
  {
    for (i = 0; i < *nbunches; i++) {
      bunches[i].x = get_real();
      bunches[i].y = get_real();
      bunches[i].cx = get_real();
      bunches[i].cy = get_real();
      bunches[i].ctime = get_real();
      bunches[i].zem = get_real();
      bunches[i].photons = get_real();
      bunches[i].lambda = get_real();
      if (bunches[i].lambda < 9990. && !is_particle_block)
        check_photons +=
            fabs(bunches[i].photons); /* Negative value could be used for
                                         special purposes */
    }
  } else if (item_header->version / 1000 == 1) /* The compact format */
  {
    for (i = 0; i < *nbunches; i++) {
      bunches[i].x = 0.1 * get_short();
      bunches[i].y = 0.1 * get_short();
      bunches[i].cx = get_short() / 30000.;
      if (bunches[i].cx > 1.)
        bunches[i].cx = 1.;
      else if (bunches[i].cx < -1.)
        bunches[i].cx = -1.;
      bunches[i].cy = get_short() / 30000.;
      if (bunches[i].cy > 1.)
        bunches[i].cy = 1.;
      else if (bunches[i].cy < -1.)
        bunches[i].cy = -1.;
      bunches[i].ctime = 0.1 * get_short();
      bunches[i].zem = pow(10., 0.001 * get_short());
      bunches[i].photons = 0.01 * get_short();
      bunches[i].lambda = get_short();
      /* No particle blocks with compact format, thus no check needed for that
       */
      check_photons += fabs(
          bunches[i]
              .photons); /* Negative value could be used for special purposes */
    }
  } else {
    get_item_end();
    return -1;
  }

  if (!is_particle_block && *photons > 10.0) {
    if (fabs(check_photons - (*photons)) / (*photons) > 0.01) {
      fflush(stdout);
      fprintf(stderr,
              "Photon numbers do not match. Maybe problems with disk space?\n");
    }
  }

  return get_item_end();
}

int
LAST_IO::SimTelIO::read_shower_extra_parameters(shower_extra_parameters *ep) {

  size_t ni = 0, nf = 0, i;
  int rc;

  if (buffer == (BYTE *)NULL || ep == (ShowerExtraParam *)NULL)
    return -1;
  ep->is_set = 0;

  item_header->type = IO_TYPE_MC_EXTRA_PARAM; /* Data type */
  if ((rc = get_item_begin()) < 0)
    return rc;
  if (item_header->version != 1 || ep == NULL) {
    get_item_end();
    return -1;
  }
  ep->id = item_header->ident;

  ep->weight = get_real();
  ni = (size_t)get_count();
  nf = (size_t)get_count();

  if (ni > 0) {
    if (ni != ep->niparam) {
      if (ep->iparam != NULL)
        free(ep->iparam);
      if ((ep->iparam = (int *)calloc(ni, sizeof(int))) == NULL) {
        get_item_end();
        return -2;
      }
      for (i = 0; i < ni; i++)
        ep->iparam[i] = (int)get_int32();
    }
  }
  ep->niparam = ni;

  if (nf > 0) {
    if (nf != ep->nfparam) {
      if (ep->fparam != NULL)
        free(ep->fparam);
      if ((ep->fparam = (float *)calloc(nf, sizeof(float))) == NULL) {
        get_item_end();
        return -2;
      }
      for (i = 0; i < nf; i++)
        ep->fparam[i] = (float)get_real();
    }
  }
  ep->nfparam = nf;

  ep->is_set = 1;

  return get_item_end();
}