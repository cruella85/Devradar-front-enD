/* Definition for struct stat.
   Copyright (C) 2020-2021 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.  */

#if !defined _SYS_STAT_H && !defined _FCNTL_H
# error "Never include <bits/struct_stat.h> directly; use <sys/stat.h> instead."
#endif

#ifndef _BITS_STRUCT_STAT_H
#define _BITS_STRUCT_STAT_H	1

#include <bits/endian.h>
#include <bits/wordsize.h>

struct stat
  {
    __dev_t st_dev;			/* Device.  */
#if __WORDSIZE == 64 || !defined __USE_FILE_OFFSET64
    unsigned short int __pad1;
    __ino_t st_ino;			/* File serial number.	*/
#else
    __ino64_t st_ino;			/* File serial number.	*/
#endif
    __mode_t st_mode;			/* File mode.  */
    __nlink_t st_nlink;			/* Link count.  */
    __uid_t st_uid;			/* User ID of the file's owner.	*/
    __gid_t st_gid;			/* Group ID of the file's group.*/
    __dev_t st_rdev;			/* Device number, if device.  */
    unsigned short int __pad2;
#ifndef __USE_FILE_OFFSET64
    __off_t st_size;			/* Size of file, in bytes.  */
#else
    __off64_t st_size;			/* Size of file, in bytes.  */
#endif
    __blksize_t st_blksize;		/* Optimal block size for I/O.  */

#ifndef __USE_FILE_OFFSET64
    __blkcnt_