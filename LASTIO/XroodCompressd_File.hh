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
    input = {buffIn, bufferInSize, 0};
    output = {buffout, bufferOutSize, 0};
    ReadInputBuffer();
  }
  ~CXrdFile_Zst() {
    delete[] buffIn;
    delete[] buffout;
  }
  void ReadInputBuffer()
  {
    LASTByteNum rc;
    if((rc = XrdFile::read(bufferInSize, (BYTE*)buffIn)) != bufferInSize)
    {
      LOG(ERROR) << "Error in Reading, Maybe End of File";
    }
    input = {buffIn, rc, 0};
  }
  bool GetOutputBuff()
  {
    output = {buffout, bufferOutSize, 0};
    const size_t ret= ZSTD_decompressStream(ZSTD_DStream, &output, &input);
        if(ZSTD_isError(ret))
        {
          LOG(ERROR) << "Error in decompressing the zstd file";
          return false;
        }
    buf_remaining = output.pos;
    buf_offset = 0;
    return true;
  }
  bool  DeCompress()
  {
    bool flag = true;
    while(buf_remaining == 0 && flag)
    {
      if(input.pos == input.size)
      {
        ReadInputBuffer();
        flag = GetOutputBuff();
      }
      else 
      {
        flag = GetOutputBuff();
      }
    }
    return flag;
  }
  LASTByteNum read(LASTByteNum size, BYTE *buffer) override{
    bool flag = true;
    LASTByteNum have_read = 0;
    flag = DeCompress();
    if(buf_remaining >= size)
    {
      memcpy(buffer, buffout + buf_offset, size);
      buf_offset += size;
      buf_remaining -= size;
      have_read = size;
      return have_read;
    }
    else 
    {
      int offset = 0;
      memcpy(buffer, buffout + buf_offset, buf_remaining);
      buf_offset += buf_remaining;
      offset = buf_remaining;
      have_read += buf_remaining;
      size -= buf_remaining;
      buf_remaining = 0;
      if(flag)
        have_read += read(size, buffer + offset);
      else
        return 0;
    }
    return have_read;

  }
    void seek_cur(LASTFileOffset offset) override {
      DeCompress();
      if(buf_remaining >= offset)
      {
        buf_offset += offset;
        buf_remaining -= offset;
      }
      else 
      {
        offset -= buf_remaining;
        buf_remaining = 0;
        buf_offset = 0;
        seek_cur(offset);
      }
    }



 private:
    const size_t bufferInSize = ZSTD_DStreamInSize();
    const size_t bufferOutSize = ZSTD_DStreamOutSize();
    char* buffIn;
    char* buffout;
    LASTByteNum   buf_offset;
    LASTByteNum   buf_remaining;
    ZSTD_inBuffer input;
    ZSTD_outBuffer output;
    ZSTD_DCtx* ZSTD_DStream  = ZSTD_createDCtx();
};
#endif