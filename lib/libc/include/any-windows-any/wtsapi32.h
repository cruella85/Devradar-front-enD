
/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_WTSAPI
#define _INC_WTSAPI

#include <_mingw_unicode.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WTS_CURRENT_SERVER ((HANDLE)NULL)
#define WTS_CURRENT_SERVER_HANDLE ((HANDLE)NULL)
#define WTS_CURRENT_SERVER_NAME (NULL)

#define WTS_CURRENT_SESSION ((DWORD)-1)

#ifndef IDTIMEOUT
#define IDTIMEOUT 32000
#endif
#ifndef IDASYNC
#define IDASYNC 32001
#endif

#define WTS_WSD_LOGOFF 0x1
#define WTS_WSD_SHUTDOWN 0x2
#define WTS_WSD_REBOOT 0x4
#define WTS_WSD_POWEROFF 0x8

#define WTS_WSD_FASTREBOOT 0x10

  typedef enum _WTS_CONNECTSTATE_CLASS {
    WTSActive,WTSConnected,WTSConnectQuery,WTSShadow,WTSDisconnected,WTSIdle,WTSListen,WTSReset,WTSDown,WTSInit
  } WTS_CONNECTSTATE_CLASS;