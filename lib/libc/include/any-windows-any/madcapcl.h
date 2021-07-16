/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _MADCAPCL_H_
#define _MADCAPCL_H_

#include <winternl.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#define MCAST_CLIENT_ID_LEN 17

  enum {
    MCAST_API_VERSION_0 = 0,MCAST_API_VERSION_1
  };

#define MCAST_API_CURRENT_VERSION MCAST_API_VERSION_1

  typedef unsigned short IP_ADDR_FAMILY;

  typedef union _IPNG_ADDRESS {
    DWORD IpAddrV4;
    BYTE IpAddrV6[16];
  } IPNG_ADDRESS,*PIPNG_ADDRESS;

  typedef struct