/**
 * @file XroodCompressd_File.hh
 * @author zhipzhang (zhipzhang@mail.ustc.edu.cn)
 * @brief  Inteface used to handle the compressed file use the XrootD protocol.
 * @version 0.1
 * @date 2023-10-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _XrootdCompressed_File__H
#define _XrootdCompressed_File__H
#include "Base_File.hh"
#include "EventIO_File.hh"
#include "LAST_types.hh"
#include <cstddef>
#include <cstring>
#include <zlib.h> // uncomress the gz file
#include <zstd.h>

class CXrdFile_Zst : public XrdFile {
 public:
  CXrdFile_Zst(std::string &filename) : XrdFile(filename) {
    buffIn = new char[bufferInSize];
    buffout = new char[bufferOutSize];
    buf_offset = 0;
    buf_remaining = 0;
    file_type = 3;
  }
  ~CXrdFile_Zst() {
    delete[] buffIn;
    delete[] buffout;
  }
  LASTByteNum read(LASTByteNum size, BYTE *buffer) override{
    have_read = 0;
    while( (size - have_read) >= buf_remaining)
    {
      LASTByteNum rc;
      if((rc = XrdFile::read(bufferInSize, (BYTE*)buffIn)) != bufferInSize)
      {
        return 0;
      }
      input = {buffIn, bufferInSize, 0};
      while(input.pos < input.size)
      {
        output = {buffout, bufferOutSize, 0};
        const size_t ret= ZSTD_decompressStream(ZSTD_DStream, &output, &input);
        if(ZSTD_isError(ret))
        {
          LOG(ERROR) << "Error in decompressing the zstd file";
          return 0;
        }
        memcpy(buffer + have_read, buffout, output.pos);
        have_read += output.pos;
      }
      buf_offset = 0;
    }
    memcpy(buffer + have_read, buffout + buf_offset, buf_remaining - (size - have_read));
    have_read += size - have_read;
    return have_read;
  }
    void seek_cur(LASTFileOffset offset) override {
      while(offset > buf_remaining)
      {
        offset -= buf_remaining;
        buf_remaining = 0;
        buf_offset = 0;
        XrdFile::read(bufferInSize, (BYTE*)buffIn);
        input = {buffIn, bufferInSize, 0};
        while(input.pos < input.size)
        {
          output = {buffout, bufferOutSize, 0};
          const size_t ret= ZSTD_decompressStream(ZSTD_DStream, &output, &input);
          if(ZSTD_isError(ret))
          {
            LOG(ERROR) << "Error in decompressing the zstd file";
            return;
          }
          buf_remaining = output.pos;
        }
        buf_offset = 0;
      }
      buf_remaining -= offset;
      buf_offset += offset;
    }



 private:
    const size_t bufferInSize = ZSTD_DStreamInSize();
    const size_t bufferOutSize = ZSTD_DStreamOutSize();
    char* buffIn;
    char* buffout;
    LASTByteNum   buf_offset;
    LASTByteNum   buf_remaining;
    LASTByteNum   have_read;
    ZSTD_inBuffer input;
    ZSTD_outBuffer output;
    ZSTD_DCtx* ZSTD_DStream  = ZSTD_createDCtx();
};
#endif