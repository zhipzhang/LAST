/**
 * @file EventIO_Base.hh
 * @author zhipz (zhipz@mail.ustc.edu.cn)
 * @brief  In order to handle the complex situation: normal file system; eos
 *file system; compressed file[default]; uncompressed file;
 * @version 0.1
 * @date 2023-10-17
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _EVENT_IO_BASE_
#define _EVENT_IO_BASE_
#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClFile.hh"
#include <cstdint>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include "XrdCl/XrdClXRootDResponses.hh"
#include "initial.h"
#include <byteswap.h>
#include <vector>
#include "glog/logging.h"
typedef unsigned char BYTE;
using json = nlohmann::json;
using string = std::string;

const BYTE magic_number[] = { 0xD4, 0x1F, 0x8A, 0x37 };
const BYTE little_magic_number[] = { 0x37, 0x8A, 0x1F, 0xD4 };

/* ------------------------ COPY_BYTES ------------------------- */

/* Copy byte without or with switching byte orders. */
/* Using the order of arguments as with memcpy: (to,from,nbytes) */
#define COPY_BYTES(_target, _source, _num) memcpy(_target, _source, _num)
#define COPY_BYTES_SWAB(_target, _source, _num) swab(_source, _target, _num)
namespace LAST_IO {
class EventIO_File {
 public:
  string path;
  json config_json;
  string url;
  XrdCl::FileSystem *fs;
  XrdCl::File *file;

  EventIO_File(string default_config = "./config.json");
};
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
class EventIO_Base {
 protected:
};
}

#endif