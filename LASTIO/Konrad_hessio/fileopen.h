/* ============================================================================

   Copyright (C) 2000, 2009, 2010, 2014  Konrad Bernloehr

   This file is part of the eventio/hessio library.

   The eventio/hessio library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library. If not, see <http://www.gnu.org/licenses/>.

============================================================================ */

/** @file fileopen.h
 *  @short Function prototypes for fileopen.c.
 *
 *  @author  Konrad Bernloehr
 *  @date    @verbatim CVS $Date: 2019/02/04 13:29:01 $ @endverbatim
 *  @version @verbatim CVS $Revision: 1.11 $ @endverbatim
 */

#ifndef FILEOPEN_H__LOADED
#define FILEOPEN_H__LOADED 1

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

/* fileopen.c */
/** An element in a linked list of include paths. */
struct incpath {
  char *path;           /**< The path name */
  struct incpath *next; /**< The next element */
};

void initpath(const char *default_path);
void initexepath(const char *default_path);
void listpath(char *buffer, size_t bufsize);
void addpath(const char *name);
void addexepath(const char *name);
FILE *fileopen(const char *fname, const char *mode);
int fileclose(FILE *f);
struct incpath *get_include_path(void);

void set_permissive_pipes(int p);
void enable_permissive_pipes(void);
void disable_permissive_pipes(void);

#ifdef __cplusplus
}
#if defined(_BASIC_STRING_H) || defined(_LIBCPP_STRING)
/* Alternative only available if <string> was included earlier. */
inline FILE *fileopen(const std::string &fname, const char *mode) {
  return fileopen(fname.c_str(), mode);
}
#endif
#endif

#endif
