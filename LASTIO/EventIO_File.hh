#ifndef _Event_IO_Header
#define _Event_IO_Header
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <string>
#include "initial.h"
#include <byteswap.h>
#include <vector>
#include "glog/logging.h"
#include "Base_File.hh"
typedef unsigned char BYTE;
// using json = nlohmann::json;
using string = std::string;

const BYTE magic_number[] = { 0xD4, 0x1F, 0x8A, 0x37 };
const BYTE little_magic_number[] = { 0x37, 0x8A, 0x1F, 0xD4 };

/* ------------------------ COPY_BYTES ------------------------- */

/* Copy byte without or with switching byte orders. */
/* Using the order of arguments as with memcpy: (to,from,nbytes) */
#define COPY_BYTES(_target, _source, _num) memcpy(_target, _source, _num)
#define COPY_BYTES_SWAB(_target, _source, _num) swab(_source, _target, _num)
namespace LAST_IO {
class HEADER {
 public:
  unsigned long type; /**< The type number telling the type of I/O block. */
  unsigned version;   /**< The version number used for the block. */
  int can_search;     /**< Set to 1 if I/O block consist of sub-blocks only. */
  int level;          /**< Tells how many levels deep we are nested now. */
  long ident;         /**< Identity number. */
  int user_flag; /**< One more bit in the header available for user data. */
  int
  use_extension; /**< Non-zero if the extension header field should be used. */
  size_t length; /**< Length of data field, for information only. */
  void Bad_file() {
    type = 0;
    can_search = 0;
  }
};

/**
 * @brief  Class EventIO_Object is similar to the IO_BUFFER Object
 *
 */
class EventIO_Object {
 protected:
  AbstractFile *input_file;
  unsigned char *buffer;
  int is_allocated; // flag for buffer to be allocated
  long buflen;      // Length of the buffer
  long r_remaining; // Remaining bytes for reading
  long max_length;
  BYTE *data; // Pointer to the place to read/write.
  HEADER *item_header;
  HEADER *sub_item_header;
  int order;        // bytes order in the system : 0 for little-endian; 1 for
                    // big-endian
  int data_pending; // flag for reading data
  int item_level;
  int max_level;
  std::vector<int> item_extension;
  std::vector<long> item_length;
  std::vector<long> item_start_offset; // Offset for the data field

 public:
  EventIO_Object(string filename, unsigned long max_length);
  EventIO_Object(string filename, unsigned long max_length,
                 std::string remote_url);
  int read(uint32_t size = 0, uint8_t buffer_off = 0) {
    uint32_t bytesread;
    bytesread = input_file->read(size, buffer + buffer_off);
    if (bytesread < size) {
      if (input_file->IsEnd()) {
        LOG(INFO) << "At End of the File";
        return 0;
      }
      LOG(INFO) << "the offset is " << input_file->GetOffset();
      LOG(ERROR) << "Error in read: Error Number " << bytesread;
    }
    return (int)bytesread;
  }
  void seek_cur(long off) {
    try {
      input_file->seek_cur(off);
    }
    catch (int err_num) {
      LOG(ERROR) << "Error in seek_cur: Error Number " << err_num;
    }
  }

  int extend_io_buffer(long increment);
  int find_io_block();
  int read_io_block();
  int skip_io_block();
  int next_subitem_type();
  int next_subitem_ident();
  int skip_subitem();

  long get_long(); // return a long integer
  void get_vector_of_long(long *vec, int num);

  uint32_t get_uint32();
  int32_t get_int32();
  void get_vector_of_int32(int32_t *vec, int num);

  int get_short();
  void get_vector_of_short(short *vec, int num);
  void get_vector_of_int(int *vec, int num);
  uint16_t get_uint16();
  void get_vector_of_uint16(uint16_t *vec, int num);

  double get_real();
  void get_vector_of_real(double *dvec, int num);
  void get_vector_of_float(float *fvec, int num);

  int get_string(char *str, int len);
  void get_vector_of_byte(BYTE *vec, int num);

  void get_vector_of_uint8(uint8_t *vec, int num);
  void get_vector_of_uint32_scount_differential(uint32_t *vec, int num);
  void get_vector_of_uint16_scount_differential(uint16_t *vec, int num);
  int unget_item();
  int get_item_begin();
  int get_item_end();

  inline char get_byte() {
    if (--r_remaining >= 0) {
      return *(data++);
    } else {
      exit(EXIT_FAILURE);
    }
  }

  /*
  Get count method is not understand well yet.
  */
  uintmax_t get_count();
  uint32_t get_count32();
  intmax_t get_scount();
  int32_t get_scount32();
  void get_vector_of_int_scount(int *vec, int num);
  double dbl_from_sfloat(const uint16_t *sum);
  float get_sfloat();
  void resetheader() {
    item_level = 0;
    item_extension.assign(item_extension.size(), 0);
    item_length.assign(item_extension.size(), 0);
    item_start_offset.assign(item_start_offset.size(), 0);
  }
};
};

#endif