/**
 * @file FileBase.hh
 * @author zhipz (zhipz@mail.ustc.edu.cn)
 * @brief  Abstract file class and its derived class
 * @version 0.1
 * @date 2023-10-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __BASE_FILE__H
#define __BASE_FILE__H
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "XrdCl/XrdClFile.hh"
#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClXRootDResponses.hh"
#include "Konrad_hessio/fileopen.h"
#include <sys/stat.h>
#include "LAST_types.hh"
#include "spdlog/spdlog.h"
using string = std::string;
class AbstractFile {
 public:
  virtual ~AbstractFile() {}
  virtual bool open(string &filename) = 0;
  virtual bool close() = 0;
  virtual void SetFileSize() = 0;
  virtual void seek_cur(LASTFileOffset offset) = 0;
  virtual bool IsOpen() = 0;
  int GetFileType() const { return file_type; }
  /**
   * @brief Vitual function of reading ~
   *
   * @param size
   * @param pointer to buffer
   * @return LASTByteNum
   */
  virtual LASTByteNum read(LASTByteNum size, BYTE *) = 0;
  LASTFileSize getFileSize() const { return size; }
  bool IsEnd() const {
    if (offset == size)
      return true;
    else if (offset < size)
      return false;
    else {
      return true;
      spdlog::error("You are probably handling a compressed file (which are not handled well especially for xrootd file)");
    }
    // exit(EXIT_FAILURE);
  }
  const LASTFileOffset GetOffset() { return offset; }

 protected:
  void SetSize(LASTFileSize filesize) { size = filesize; }
  int file_type = 0; // file type 1: xrootd file ; 2: local file
  LASTFileOffset offset = 0;

 private:
  LASTFileSize size = 0;
};

class XrdFile : public AbstractFile {
 public:
  bool open(string &filename) override {
    status = input_file->Open(filename.c_str(), XrdCl::OpenFlags::Read);
    return (status.IsOK());
  }
  void SetFileSize() override {
    XrdCl::StatInfo *file_info;
    XrdCl::XRootDStatus status = input_file->Stat(true, file_info);
    SetSize((LASTFileSize)file_info->GetSize());
  }
  bool close() override {
    XrdCl::XRootDStatus status = input_file->Close();
    return (status.IsOK());
  }
  virtual LASTByteNum read(LASTByteNum size, BYTE *buffer) override {
    LASTByteNum bytes_read = 0;
    XrdCl::XRootDStatus status =
        input_file->Read(offset, size, buffer, bytes_read, 999);
    if (bytes_read < size) {
      spdlog::warn("Failed to read Xrootd file, offset: {0}, size: {1}, bytes_read: {2}", offset, size, bytes_read);
      return bytes_read;
    }
    offset += size;
    return bytes_read;
  }
  virtual void seek_cur(LASTFileOffset off) override {
    if ((offset + off) > (LASTFileOffset)getFileSize()) {
      spdlog::error("Seek cur Exceed Xrootd file size offset: {0}, file size: {1}", offset, getFileSize());
      throw - 71;
    }
    offset += off;
  }
  bool IsOpen() override { return input_file->IsOpen(); }
  XrdFile(string &filename) {
    input_file = new XrdCl::File();
    bool rc = open(filename);
    if (!rc) {
      throw "Open Xrootd file failed";
    }
    SetFileSize();
    file_type = 1;
  }
  ~XrdFile() {
    if (IsOpen()) {
      close();
    }
  }

 protected:
  XrdCl::File *input_file;
  XrdCl::XRootDStatus status;
};

class PosixFile : public AbstractFile {
 public:
  bool open(string &filename) override {
    input_file = fileopen(filename.c_str(), "rb");
    if (input_file == 0) {
      return false;
    } else {
      return true;
    }
    struct stat filestat;
    if (fstat(fileno(input_file), &filestat) < 0) {
      spdlog::error("Failed to get Posix file: {0} information", filename);
      exit(EXIT_FAILURE);
    }
    if (S_ISFIFO(filestat.st_mode)) {
      compressed = 1;
    }
  }
  void SetFileSize() override {
    long origin_pos = ftell(input_file);
    fseek(input_file, 0, SEEK_END);
    long end_pos = ftell(input_file);
    SetSize((LASTFileSize)(end_pos - origin_pos));
    fseek(input_file, 0, SEEK_SET);
  }
  bool close() override {
    int rc = fclose(input_file);
    if (rc == 0) {
      return true;
    } else {
      return false;
    }
  }
  LASTByteNum read(LASTByteNum size, BYTE *buffer) override {
    LASTByteNum bytes_read = fread(buffer, 1, size, input_file);
    if (bytes_read < size) {
      spdlog::warn("Failed to read Posix file  size: {0}, bytes_read: {1}", size, bytes_read);
    }
    offset = offset + bytes_read;
    return bytes_read;
  }
  void seek_cur(LASTFileOffset off) override {
    if (!compressed) {
      fseek(input_file, off, SEEK_CUR);
    } else {
      pipe_seek_cur(off);
    }
    offset += off;
  }
  bool IsOpen() override {
    if (input_file == 0) {
      return false;
    } else {
      return true;
    }
  }
  PosixFile(string &filename) {
    open(filename);
    if (!compressed) {
      SetFileSize();
    }
    file_type = 2;
  }
  ~PosixFile() {
    if (IsOpen()) {
      close();
    }
  }

 private:
  FILE *input_file;
  int compressed; // flag to show whether the file is open by compressed
  BYTE buffer[4096];
  void pipe_seek_cur(LASTFileOffset off) {
    int rb;
    int nbuf = off / 4096;
    int rbuf = off % 4096;
    for (int ibuf = 0; ibuf < nbuf; ibuf++) {
      rb = fread(buffer, 1, 4096, input_file);
    }
    if (rbuf > 0) {
      rb = fread(buffer, 1, rbuf, input_file);
      if (rb < rbuf) {
        spdlog::warn("End of file ?");
      }
    }
  }
};
#endif