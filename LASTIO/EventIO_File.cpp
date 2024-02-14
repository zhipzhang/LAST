#include "EventIO_File.hh"
#include "Base_File.hh"
#include "XrdCl/XrdClFile.hh"
#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClXRootDResponses.hh"
#include "glog/logging.h"
#include "initial.h"
#include <byteswap.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <endian.h>

LAST_IO::EventIO_Object::EventIO_Object(string filename, unsigned long length) {
  LOG(INFO) << "Working at local file system , Local File Initialized";
  input_file = new PosixFile(filename);
  item_header = new HEADER();
  int len = 10000;
  sub_item_header = new HEADER();
  if ((buffer = (BYTE *)malloc(len + 8)) == (BYTE *)NULL) {
    LOG(ERROR) << "Insufficient memory for EventIO_Object";
    exit(EXIT_FAILURE);
  }
  is_allocated = 1;
  buflen = len;
  r_remaining = 0;
  item_level = 0;
  int nlevel = 10;
  item_length.resize(nlevel, 0);
  item_extension.resize(nlevel, 0);
  item_start_offset.resize(nlevel, 0);
  max_level = nlevel;
  max_length = length;
}

LAST_IO::EventIO_Object::EventIO_Object(std::string filename, unsigned long length, std::string remote_url)
{
  LOG(INFO) << "Working at eos file system , Xrootd File Initialized";
  filename += remote_url;
  input_file  = new XrdFile(filename);
  item_header = new HEADER();
  int len = 10000;
  sub_item_header = new HEADER();
  if ((buffer = (BYTE *)malloc(len + 8)) == (BYTE *)NULL) {
    LOG(ERROR) << "Insufficient memory for EventIO_Object";
    exit(EXIT_FAILURE);
  }
  is_allocated = 1;
  buflen = len;
  r_remaining = 0;
  item_level = 0;
  int nlevel = 10;
  item_length.resize(nlevel, 0);
  item_extension.resize(nlevel, 0);
  item_start_offset.resize(nlevel, 0);
  max_level = nlevel;
  max_length = length;
}
/**
 * @brief Find the beginning of the next I/O data block in the input file
 *        item_header and byte_order, use_extension will be filled in this
 *        function.
 *
 * @return int  0(O.K.) -1 (error)
 */
int LAST_IO::EventIO_Object::find_io_block() {
  long sync_count = 0;
  int block_found, byte_number, byte_order;
  int rc = 0;
  if (buffer == (BYTE *)NULL || buflen < 20) {
    // Wraing Message
  }
  data = buffer;
  if (input_file->IsOpen()) {
    for (sync_count = (-4L), byte_number = byte_order = block_found = 0;
         !block_found; sync_count++) {
      rc = read(1, byte_number);
      if (rc <= 0) {
        // Error
        return -1;
      }
      if (byte_number == 0) {
        if (buffer[0] == (BYTE)magic_number[0]) {
          byte_order = 1;
        } else if (buffer[0] == (BYTE)magic_number[3]) {
          byte_order = -1;
        }
        byte_number = 1;
      } else if (byte_order == 1) {
        if (buffer[byte_number] != magic_number[byte_number]) {
          byte_number = byte_order = 0;
          continue;
        }
        byte_number++;
      } else if (byte_order == -1) {
        if (buffer[byte_number] != magic_number[3 - byte_number]) {
          byte_number = byte_order = 0;
          continue;
        }
        byte_number++;
      }
      if (byte_number == 4) {
        block_found = 1;
      }
    }
    rc = read(12, 4); // Read from buffer + 4
    if (rc > 0 && rc != 12) {
      return -1;
    }
    if (rc > 0) {
      int32_t sync_tag;
      uint32_t len1 = 0, xbit = 0;
      data = buffer;
      r_remaining = buflen;
      order = 0;
      sync_tag = get_long();
#ifdef SIXTY_FOUR_BITS
      if (sync_tag == (int32_t)0xD41F8A37) {
        order = 0;
      } else if (sync_tag == (int32_t)0x378A1FD4) {
        order = 1;
      }
#else
      if (sync_tag == (int32_t)0xD41F8A37L)
        order = 0;
      else if (sync_tag == (int32_t)0x378A1FD4L)
        order = 1;
#endif
      else {
        // Warning Invalid byte ordering of input data
        return -1;
      }

      data = buffer + 12;
      len1 = get_uint32();
      xbit = len1 &
             (uint32_t)0x80000000UL; // Not really understand (bit 31 is not 0)
      if (xbit) {
        item_extension[0] = 1;
      }

      data = buffer;
      r_remaining = -1L;
    }
  }
  if (rc <= 0) {

    item_header->Bad_file();
    if (rc == 0) {
      // EOF of the file
      return -2;
    } else {
      return -1;
    }
  }

  item_header->type = 0;
  data_pending = 1;

  if (get_item_begin() != 0)
    return -1;
  item_level = 0;

  if (sync_count > 0) {
    LOG(WARNING) << "Synchronization Error skipeed" << sync_count
                 << " bytes of data";
  }
  return 0;
}

int LAST_IO::EventIO_Object::extend_io_buffer(long increment) {
  if (buffer == (BYTE *)NULL)
    return -1;
  if (!is_allocated) {
    return -1;
  }
  if (increment < 1048576 && buflen >= 8388608)
    increment = 1048576;
  else if (increment < 262144 && buflen >= 2097152)
    increment = 262144;
  else if (increment < 131072 && buflen >= 1048567)
    increment = 131072;
  else if (increment < 8192)
    increment = 8192;

  if (8 * increment < buflen && buflen > 65536) {
    long increment2 = ((buflen - 1) / 65536) * 8192;
    if (increment2 > increment)
      increment = increment2;
  }
  if ((buflen + increment) > max_length) {
    LOG(WARNING) << "Maximum length of I/O Buffer exceeded";
    return -1;
  }
  long iffset = data - buffer;
  BYTE *typr;
  if ((typr = (BYTE *)realloc(buffer, buflen + increment)) == (BYTE *)NULL) {
    LOG(WARNING) << "Insufficient memort for extending I/O block";
    return -1;
  } else {
    buffer = typr;
    data = buffer + iffset;
    buflen += increment;
  }
  r_remaining = buflen - (int)(data - buffer);
  return 0;
}
/**
 * @brief read io_block if we have data_pending = 1 , this function will
 *        read item_length bytes
 *
 * @return int 0(O.K.) -1(error) -3(block skipped extend_io_buffer field)
 */
int LAST_IO::EventIO_Object::read_io_block() {
  int rc = 0;
  unsigned long length;
  unsigned long rb = 0; // The number of bytes read
  if (data_pending <= 0) {
    // Cannot Read Now
    return -1;
  }
  if (item_level != 0 || item_length[0] < 0) {
    return -1;
  }
  length = (unsigned long)item_length[0];
  if (item_length[0] > 0) {
    int e4 = (item_extension[0] ? 4 : 0);
    if (buflen < (item_length[0] + 16 + e4)) {
      if (extend_io_buffer(item_length[0] + 16 + e4 - buflen) == -1) {
        LOG(WARNING) << "I/O buffer is too small, the block is skipped";
        /*
            Warning Too small io buffer I/O skipped
        */
        if ((rc = skip_io_block()) < 0) {
          return rc;
        } else {
          return -3;
        }
      }
    }

    rb = read(length, 16 + e4);
    if (rb > 0 && rb < length) {
      return -1;
    }
  } else if (item_length[0] < 0) {
    rc = -1;
  }
  data_pending = 0; // Rest the data_pending
  return 0;
}

int LAST_IO::EventIO_Object::skip_io_block() {
  if (data_pending <= 0) {
    /*
    Warning No I/O block
    */
    return -1;
  }
  if (item_level != 0 || item_length[0] < 0) {
    return -1;
  }
  long length = item_length[0];
  if (length == 0) {
    item_length[0] = -1;
    data_pending = 0;
    return 0;
  }
  seek_cur(length);
  return 0;
}

int LAST_IO::EventIO_Object::next_subitem_type() {
  int this_type;
  REGISTER int ilevel;
  BYTE *previous_position;
  long previous_remaining;

  if (buffer == (BYTE *)NULL)
    return -1;

  if (r_remaining < 12) /* Not enough for any more headers */
    return -2;

  previous_position = data;
  previous_remaining = r_remaining;
  ilevel = item_level;

  if (ilevel >= max_level) {
    LOG(WARNING) << "Maximum level of sub-items in I/O Buffer exceeded";
    r_remaining = -1;
  }

  /* Are we beyond the last sub-item? */
  if (ilevel > 0) {
    /* First check if we are already beyond the top item and then if we */
    /* will be beyond the next smaller level (superiour) item after */
    /* reading this item's header. */
    if ((long)(data - buffer) >=
            item_length[0] + 16 + (item_extension[0] ? 4 : 0) ||
        (long)(data - buffer) + 12 >=
            item_start_offset[ilevel - 1] + item_length[ilevel - 1])
      return -2;
  }
  /* Not for top-level items */
  else if (ilevel == 0)
    return -1;

  /* Extract the actual type and version from the 'type/version' field. */
  this_type = (int)((unsigned long)get_long()) & 0x0000ffffL;
  data = previous_position;
  r_remaining = previous_remaining;
  return this_type;
}
int LAST_IO::EventIO_Object::next_subitem_ident() {
  int rc;
  long id;
  BYTE *previous_position;
  long previous_remaining;

  if (buffer == (BYTE *)NULL)
    return -1;

  if (r_remaining < 12) /* Not enough for any more headers */
    return -2;

  previous_position = data;
  previous_remaining = r_remaining;

  item_header->type = 0;
  if ((rc = get_item_begin()) < 0)
    id = rc;
  else {
    id = item_header->ident;
    if ((rc = unget_item()) < 0)
      id = rc;
  }

  data = previous_position;
  r_remaining = previous_remaining;

  return id;
}

int LAST_IO::EventIO_Object::skip_subitem() {
  int rc;
  item_header->type = 0;
  if ((rc = get_item_begin()) < 0)
    return rc;
  else
    return get_item_end();
}
/**
 * @brief Get 4-byte integral from data pointer
 *
 * @return long
 */
long LAST_IO::EventIO_Object::get_long() {
  int32_t num;

  union {
    int32_t lval;
    BYTE cval[4];
  } val[2];
  if ((r_remaining -= 4) < 0) {
    // Error
    return -1L;
  }
  if (order == 0)
    COPY_BYTES((void *)&num, (void *)data, 4);
  else {
    /*
        Here, don't consider the situation not have byteswap.h
    */
    COPY_BYTES((void *)&val[0].lval, (void *)data, 4);
    num = bswap_32(val[0].lval);
  }
  data += 4;

  return (long)num;
}

/**
 * @brief Get a vector of 4-byte integers as long int from I/O buffer
 *
 * @param vec  pointer returned
 * @param num  number of long to be read
 */
void LAST_IO::EventIO_Object::get_vector_of_long(long *vec, int num) {
  REGISTER int i;
  union {
    int32_t lval;
    BYTE cval[4];
  } val[2];

  if (num <= 0)
    return;
  if ((r_remaining -= 4 * num) < 0)
    return;
  if (vec == (long *)NULL) {
    data += 4 * num;
    return;
  }
  if (order == 0) {
    for (i = 0; i < num; i++) {
      COPY_BYTES((void *)&val[1].lval, (void *)data, (size_t)4);
      vec[i] = (long)val[1].lval;
      data += 4;
    }
  } else {
    for (i = 0; i < num; i++) {
      COPY_BYTES((void *)&val[0].lval, (void *)data, (size_t)4);
      vec[i] = (long)bswap_32(val[0].lval);
      data += 4;
    }
  }
}
/**
 * @brief Get 4-byte unsigned integer from data pointer
 *
 * @return uint32_t
 */
uint32_t LAST_IO::EventIO_Object::get_uint32() {
  int32_t num;

  union {
    int32_t lval;
    BYTE cval[4];
  } val[2];
  if ((r_remaining -= 4) < 0) {
    // Error
    return -1L;
  }
  if (order == 0)
    COPY_BYTES((void *)&num, (void *)data, (size_t)4);
  else {
    /*
        Here, don't consider the situation not have byteswap.h
    */
    COPY_BYTES((void *)&val[0].lval, (void *)data, (size_t)4);
    num = bswap_32(val[0].lval);
  }
  data += 4;

  return num;
}

/**
 * @brief Get a four byte integer from an I/O buffer.
 *
 * @return int
 */
int LAST_IO::EventIO_Object::get_int32() {
  int32_t num;
  union {
    int32_t lval;
    BYTE cval[4];
  } val[2];
  if ((r_remaining -= 4) < 0)
    return -1L;
  if (order == 0) {
    COPY_BYTES((void *)&num, (void *)data, 4);
  } else {
    COPY_BYTES((void *)&val[0].lval, (void *)data, (size_t)4);
    num = bswap_32(val[0].lval);
  }
  data += 4;
  return num;
}

void LAST_IO::EventIO_Object::get_vector_of_int32(int32_t *vec, int num) {
  int i;
  if (vec == (int32_t *)NULL) {
    if (r_remaining -= (4 * num) >= 0)
      data += 4 * num;
    return;
  }
  for (i = 0; i < num; i++) {
    vec[i] = get_int32();
  }
}
/**
 * @brief Get a two-byte short integer from data
 *
 * @return int
 */
int LAST_IO::EventIO_Object::get_short() {
  int16_t num;
  if ((r_remaining -= 2) < 0) {
    return -1;
  }
  if (order == 0)
    COPY_BYTES((void *)&num, (void *)data, 2);
  else
    COPY_BYTES_SWAB((void *)&num, (void *)data, 2);

  data += 2;
  return ((int)num);
}
void LAST_IO::EventIO_Object::get_vector_of_short(short *vec, int num) {
  REGISTER int i;
  if (vec == (short *)NULL) {
    if ((r_remaining -= (2 * num)) >= 0)
      data += 2 * num;
    return;
  }
  for (i = 0; i < num; i++) {
    vec[i] = (short)get_short();
  }
}
/**
 * @brief Get vector of two-bytes integer
 *
 * @param vec
 * @param num
 */
void LAST_IO::EventIO_Object::get_vector_of_int(int *vec, int num) {
  REGISTER int i;
  if (vec == (int *)NULL) {
    if (r_remaining -= (2 * num) >= 0)
      data += 2 * num;
    return;
  }

  for (i = 0; i < num; i++) {
    vec[i] = get_short();
  }
}

/**
 * @brief get a floating point number
 *        We simply think the IEEE_FLOAT_FORMAT is used here
 *
 * @return double
 */
double LAST_IO::EventIO_Object::get_real() {

  union {
    float fnum;
    int32_t lnum;
  } val;
  val.lnum = get_int32();
  return val.fnum;
}

/**
 * @brief Get a vector of floating point numbers as 'doubles' from an I/O buffer
 *
 * @param dvec
 * @param num
 */
void LAST_IO::EventIO_Object::get_vector_of_real(double *dvec, int num) {
  int i;
  if (dvec == (double *)NULL) {
    if ((r_remaining -= (4 * num)) >= 0) {
      data += 4 * num;
      return;
    }
  }
  for (i = 0; i < num; i++)
    dvec[i] = get_real();
}
/**
 * @brief Get a vector of bytes from data
 *
 * @param vec
 * @param num
 */
void LAST_IO::EventIO_Object::get_vector_of_byte(BYTE *vec, int num) {
  if (num <= 0)
    return;
  if ((r_remaining -= num) < 0)
    return;

  if (vec != (BYTE *)NULL)
    COPY_BYTES((void *)vec, (void *)data, (size_t)num);
  data += num;
}

int LAST_IO::EventIO_Object::get_string(char *str, int len) {
  int nbytes, nread;

  nbytes = get_short();
  nread = (len - 1 < nbytes) ? len - 1 : nbytes;

  get_vector_of_byte((BYTE *)str, nread);
  if (nbytes > nread) {
    r_remaining -= (nbytes - nread);
    data += (nbytes - nread);
  }
  str[nread] = '\0';
  return (nbytes);
}

uintmax_t LAST_IO::EventIO_Object::get_count() {
  uintmax_t v[9]; /* Scheme implemented for 32 and 64 bit systems. */

  v[0] = get_byte();

  if ((v[0] & 0x80) == 0)
    return v[0];
  v[1] = get_byte();
  if ((v[0] & 0xc0) == 0x80)
    return ((v[0] & 0x3f) << 8) | v[1];
  v[2] = get_byte();
  if ((v[0] & 0xe0) == 0xc0)
    return ((v[0] & 0x1f) << 16) | (v[1] << 8) | v[2];
  v[3] = get_byte();
  if ((v[0] & 0xf0) == 0xe0)
    return ((v[0] & 0x0f) << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
  v[4] = get_byte();
  if ((v[0] & 0xf8) == 0xf0)
    return ((v[0] & 0x07) << 32) | (v[1] << 24) | (v[2] << 16) | (v[3] << 8) |
           v[4];
/* With only 32-bit integers available, we may lose data from here on. */
#ifndef HAVE_64BIT_INT
  LOG(WARNING)<<"Data clipped to 32 bits in get_count function.");
#endif
  v[5] = get_byte();
  if ((v[0] & 0xfc) == 0xf8)
    return ((v[0] & 0x03) << 40) | (v[1] << 32) | (v[2] << 24) | (v[3] << 16) |
           (v[4] << 8) | v[5];
  v[6] = get_byte();
  if ((v[0] & 0xfe) == 0xfc)
    return ((v[0] & 0x01) << 48) | (v[1] << 40) | (v[2] << 32) | (v[3] << 24) |
           (v[4] << 16) | (v[5] << 8) | v[6];
  v[7] = get_byte();
  if ((v[0] & 0xff) == 0xfe)
    return (v[1] << 48) | (v[2] << 40) | (v[3] << 32) | (v[4] << 24) |
           (v[5] << 16) | (v[6] << 8) | v[7];
  v[8] = get_byte();
  return (v[1] << 56) | (v[2] << 48) | (v[3] << 40) | (v[4] << 32) |
         (v[5] << 24) | (v[6] << 16) | (v[7] << 8) | v[8];
}

intmax_t LAST_IO::EventIO_Object::get_scount() {
  uintmax_t u = get_count();
  /* u values of 0,1,2,3,4,... here correspond to signed values of */
  /*   0,-1,1,-2,2,... We have to test the least significant bit: */
  if ((u & 1) == 1) /* Negative number; */
    return -((intmax_t)(u >> 1)) - 1;
  else
    return (intmax_t)(u >> 1);
}

uint32_t LAST_IO::EventIO_Object::get_count32() {
  uint32_t v[9];

  v[0] = get_byte();

  if ((v[0] & 0x80) == 0)
    return v[0];
  v[1] = get_byte();
  if ((v[0] & 0xc0) == 0x80)
    return ((v[0] & 0x3f) << 8) | v[1];
  v[2] = get_byte();
  if ((v[0] & 0xe0) == 0xc0)
    return ((v[0] & 0x1f) << 16) | (v[1] << 8) | v[2];
  v[3] = get_byte();
  if ((v[0] & 0xf0) == 0xe0)
    return ((v[0] & 0x0f) << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
  v[4] = get_byte();
  if ((v[0] & 0xf8) == 0xf0) {
    if ((v[0] & 0x07) != 0x00)
      // LOG(WARNING)<<"Data too large in get_count32 function, clipped.");
      return (v[1] << 24) | (v[2] << 16) | (v[3] << 8) | v[4];
  }
  /* With only 32-bit integers available, we may lose data from here on. */
  // LOG(WARNING)<<"Data too large in get_count32 function.");
  v[5] = get_byte();
  if ((v[0] & 0xfc) == 0xf8)
    return 0;
  v[6] = get_byte();
  if ((v[0] & 0xfe) == 0xfc)
    return 0;
  v[7] = get_byte();
  if ((v[0] & 0xff) == 0xfe)
    return 0;
  v[8] = get_byte();
  return 0;
}

int32_t LAST_IO::EventIO_Object::get_scount32() {
  uint32_t u = get_count32();
  if ((u & 1) == 1) /* Negative number; */
    return -((int32_t)(u >> 1)) - (int32_t)1;
  else
    return (int32_t)(u >> 1);
}

void LAST_IO::EventIO_Object::get_vector_of_int_scount(int *vec, int num) {
  int i;
  for (i = 0; i < num; i++)
    vec[i] = get_scount32();
}

int LAST_IO::EventIO_Object::unget_item() {
  int old_level;

  old_level = item_header->level;
  if (old_level < 0 || old_level >= max_level || old_level >= item_level)
    return -1;
  if ((item_level = old_level) == 0)
    data = buffer;
  else
    data = buffer + item_start_offset[old_level] - 12 -
           (item_extension[old_level] ? 4 : 0);
  r_remaining =
      item_length[0] + 16 + (item_extension[0] ? 4 : 0) - (long)(data - buffer);

#ifdef BUG_CHECK
  bug_check();
#endif
  return 0;
}

uint16_t LAST_IO::EventIO_Object::get_uint16() {
  uint16_t uval;
  get_vector_of_uint16(&uval, 1);
  return uval;
}

double LAST_IO::EventIO_Object::dbl_from_sfloat(const uint16_t *snum) {
  uint16_t sign = ((*snum) & 0x8000U) >> 15;
  uint16_t exponent = ((*snum) & 0x7c00U) >> 10;
  uint16_t mantissa = ((*snum) & 0x03ffU);
  double s = (sign == 0) ? 1. : -1.;
  if (exponent == 0) /* De-normalized */
  {
    if (mantissa == 0)
      return s * 0.0;
    else
      return s * mantissa / (1024 * 16384);
  } else if (exponent < 31)
    return s * pow(2., exponent - 15.0) * (1. + mantissa / 1024.);
#ifdef INF
  else if (mantissa == 0)
    return s * INF;
#elif defined(INFINITY)
  else if (mantissa == 0)
    return s * INFINITY;
#endif
  else
#ifdef NAN
    return NAN;
#else
    return 0.;
#endif
}

float LAST_IO::EventIO_Object::get_sfloat() {
  uint16_t snum = get_uint16();
  return dbl_from_sfloat(&snum);
}
/**
 * @brief Begin reading an item. Fill the HEADER Class
 *
 *        Function to  find a specific item  is not
 *        added now
 * @return int  0 (O.K.), -1 (error), -2 (end-of-buffer)
 */
int LAST_IO::EventIO_Object::get_item_begin() {
  int32_t sync_tag;
  unsigned long this_type;
  unsigned long wanted_type;
  REGISTER int ilevel;
  BYTE *previous_position;
  long previous_remaining;
  int previous_level, previous_order;
  size_t length, extension = 0;

  previous_position = data;
  previous_remaining = r_remaining;
  previous_level = ilevel = item_level;
  previous_order = order;

  /*
      Consider whether we have beyond the top item and subitem
      This means the data pointer have beyond the item begin
  */
  if (ilevel > 0) {
    if ((long)(data - buffer) >=
            item_length[0] + 16 + (item_extension[0] ? 4 : 0) ||
        (long)(data - buffer) + 12 >=
            item_start_offset[ilevel - 1] + item_length[item_level - 1])
      return -2;
  }
  /*               Top item                   */
  else if (ilevel == 0) {
    if (data_pending < 0) {
      LOG(WARNING) << "You must get an I/O block before you read";
      /* no IO block for reading*/
      return -1;
    }
    if (buffer == (BYTE *)NULL) {
      LOG(ERROR) << "buffer is not allocated";
      return -1;
    }
    data = buffer;
    r_remaining = buflen;

    /*    Here we can check byte order again  */
    sync_tag = get_long();
  }
  wanted_type = item_header->type;
  this_type = (unsigned long)get_long();
  item_header->type = this_type & 0x0000ffffUL;
  item_header->version = (unsigned)(this_type >> 20) & 0xfff;

  item_header->use_extension = ((this_type & 0x00020000UL) != 0);

  item_header->ident = get_long();
  length = get_uint32();
  if ((length & 0x40000000UL) != 0)
    item_header->can_search = 1;
  else
    item_header->can_search = 0;

  if ((length & 0x80000000UL) != 0) {
    item_header->use_extension = 1;
    /* Check again that we are not beyond the superior item after reading the
     * extension */
    if (ilevel > 0 &&
        (long)(data - buffer) + 16 >=
            item_start_offset[ilevel - 1] + item_length[ilevel - 1])
      return -2;
    extension = get_uint32();
    /* Actual length consists of bits 0-29 of length field plus bits 0-11 of
     * extension field. */
    length = (length & 0x3FFFFFFFUL) | ((extension & 0x0FFFUL) << 30);
  } else
    length = (length & 0x3FFFFFFFUL);
  item_header->length = length;
  item_length[ilevel] = (long)length;

  item_start_offset[ilevel] = data - buffer;
  item_extension[ilevel] = item_header->use_extension;

  if (ilevel == 0) {
    r_remaining = item_length[0];
  }
  /*
      Compared the type with wanted type
  */
  if (wanted_type > 0 && wanted_type != item_header->type) {
    data = previous_position;
    r_remaining = previous_remaining;
    item_level = previous_level;
    order = previous_order;
    return -3;
  }

  item_header->level = item_level++;
  if (r_remaining < 0)
    return -1;

  return 0;
}
/**
 * @brief Generic version (Although slower)
 *
 * @param vec
 * @param num
 */
void
LAST_IO::EventIO_Object::get_vector_of_uint16_scount_differential(uint16_t *vec,
                                                                  int num) {
  int i;
  int32_t val = 0;
  for (i = 0; i < num; i++) {
    /* Largest possible changes are within +- 65535. */
    val += get_scount32();
    vec[i] = (uint16_t)val;
  }
}

/**
 * @brief Use the slower generic version of
 *get_vector_of_uint32_scount_differential
 *        detailed description can see the even
 *
 * @param vec
 * @param num
 */
void
LAST_IO::EventIO_Object::get_vector_of_uint32_scount_differential(uint32_t *vec,
                                                                  int num) {
  /* Generic version, via get_scount32, get_count32, get_byte: identical to
   * get_adcsample_differential */
  int i;
  int32_t val = 0;
  for (i = 0; i < num; i++) {
    /* Assume that largest possible changes are within -2^31 to 2^31-1. */
    val += get_scount32();
    vec[i] = (uint32_t)val;
  }
}

int LAST_IO::EventIO_Object::get_item_end() {
  long length;
  REGISTER int ilevel;

  if (item_header->level >= 0 && item_header->level <= max_level) {
    ilevel = item_level = item_header->level;
  } else {
    return -1;
  }
  if (item_length[ilevel] >= 0) {
    if (item_length[ilevel] !=
        (length = (long)(data - buffer) - item_start_offset[ilevel])) {
      if (length > item_length[ilevel]) {
        LOG(WARNING) << "Actual length of item type" << item_header->type
                     << "exceeds specified length";
      }
      data = buffer + item_start_offset[ilevel] + item_length[ilevel];
    }
  }
  if (item_level == 0)
    r_remaining = -1L;
  else
    r_remaining = item_length[0] + 16 + (item_extension[0] ? 4 : 0) -
                  (long)(data - buffer);
  return 0;
}

void LAST_IO::EventIO_Object::get_vector_of_uint16(uint16_t *vec, int num) {
  int i;
  if (r_remaining < 2 * num) {
    for (i = 0; i < num; i++) {
      vec[i] = 0;
    }
    data += r_remaining;
    r_remaining -= 2 * num;
    return;
  }
  if (order == 0)
    COPY_BYTES((void *)vec, (void *)data, (size_t)(2 * num));
  else
    COPY_BYTES_SWAB((void *)vec, (void *)data, (size_t)(2 * num));

  r_remaining -= (2 * num);
  data += 2 * num;
}

void LAST_IO::EventIO_Object::get_vector_of_float(float *fvec, int num) {
  int i;
  if (fvec == (float *)NULL) {
    if ((r_remaining -= (4 * num) >= 0)) {
      data += 4 * num;
    }
    return;
  }
  for (i = 0; i < num; i++) {
    fvec[i] = (float)get_real();
  }
}

void LAST_IO::EventIO_Object::get_vector_of_uint8(uint8_t *vec, int num) {
  get_vector_of_byte(vec, num);
}