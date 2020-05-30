/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _PDH_H_
#define _PDH_H_

#include <_mingw_unicode.h>
#include <windows.h>
#include <winperf.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef LONG PDH_STATUS;

#define PDH_FUNCTION PDH_STATUS WINAPI

#define PDH_CVERSION_WIN40 ((DWORD)(0x0400))
#define PDH_CVERSION_WIN50 ((DWORD)(0x0500))

#define PDH_VERSION ((DWORD)((PDH_CVERSION_WIN50) + 0x0003))

#define IsSuccessSeverity(ErrorCode) ((!((DWORD)(ErrorCode) & 0xC0000000)) ? TRUE : FALSE)
#define IsInformationalSeverity(ErrorCode) ((((DWORD)(ErrorCode) & 0xC0000000)==(DWORD)0x40000000) ? TRUE : FALSE)
#define IsWarningSeverity(ErrorCode) ((((DWORD)(ErrorCode) & 0xC0000000)==(DWORD)0x80000000) ? TRUE : FALSE)
#define IsErrorSeverity(ErrorCode) ((((DWORD)(ErrorCode) & 0xC0000000)==(DWORD)0xC0000000) ? TRUE : FALSE)

#define MAX_COUNTER_PATH 256

#define PDH_MAX_COUNTER_NAME 1024
#define PDH_MAX_INSTANCE_NAME 1024
#define PDH_MAX_COUNTER_PATH 2048
#define PDH_MAX_DATASOURCE_PATH 1024

  typedef HANDLE PDH_HCOUNTER;
  typedef HANDLE PDH_HQUERY;
  typedef HANDLE PDH_HLOG;

  typedef PDH_HCOUNTER HCOUNTER;
  typedef PDH_HQUERY HQUERY;
#ifndef _LMHLOGDEFINED_
  typedef PDH_HLOG HLOG;
#endif

#ifdef INVALID_HANDLE_VALUE
#undef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)((LONG_PTR)-1))
#endif

#define H_REALTIME_DATASOURCE NULL
#define H_WBEM_DATASOURCE INVALID_HANDLE_VALUE

  typedef struct _PDH_RAW_COUNTER {
    DWORD CStatus;
    FILETIME TimeStamp;
    LONGLONG FirstValue;
    LONGLONG SecondValue;
    DWORD MultiCount;
  } PDH_RAW_COUNTER,*PPDH_RAW_COUNTER;

  typedef struct _PDH_RAW_COUNTER_ITEM_A {
    LPSTR szName;
    PDH_RAW_COUNTER RawValue;
  } PDH_RAW_COUNTER_ITEM_A,*PPDH_RAW_COUNTER_ITEM_A;

  typedef struct _PDH_RAW_COUNTER_ITEM_W {
    LPWSTR szName;
    PDH_RAW_COUNTER RawValue;
  } PDH_RAW_COUNTER_ITEM_W,*PPDH_RAW_COUNTER_ITEM_W;

  typedef struct _PDH_FMT_COUNTERVALUE {
    DWORD CStatus;
    __C89_NAMELESS union {
      LONG longValue;
      double doubleValue;
      LONGLONG largeValue;
      LPCSTR AnsiStringValue;
      LPCWSTR WideStringValue;
    };
  } PDH_FMT_COUNTERVALUE,*PPDH_FMT_COUNTERVALUE;

  typedef struct _PDH_FMT_COUNTERVALUE_ITEM_A {
    LPSTR szName;
    PDH_FMT_COUNTERVALUE FmtValue;
  } PDH_FMT_COUNTERVALUE_ITEM_A,*PPDH_FMT_COUNTERVALUE_ITEM_A;

  typedef struct _PDH_FMT_COUNTERVALUE_ITEM_W {
    LPWSTR szName;
    PDH_FMT_COUNTERVALUE FmtValue;
  } PDH_FMT_COUNTERVALUE_ITEM_W,*PPDH_FMT_COUNTERVALUE_ITEM_W;

  typedef struct _PDH_STATISTICS {
    DWORD dwFormat;
    DWORD count;
    PDH_FMT_COUNTERVALUE min;
    PDH_FMT_COUNTERVALUE max;
    PDH_FMT_COUNTERVALUE mean;
  } PDH_STATISTICS,*PPDH_STATISTICS;

  typedef struct _PDH_COUNTER_PATH_ELEMENTS_A {
    LPSTR szMachineName;
    LPSTR szObjectName;
    LPSTR szInstanceName;
    LPSTR szParentInstance;
    DWORD dwInstanceIndex;
    LPSTR szCounterName;
  } PDH_COUNTER_PATH_ELEMENTS_A,*PPDH_COUNTER_PATH_ELEMENTS_A;

  typedef struct _PDH_COUNTER_PATH_ELEMENTS_W {
    LPWSTR szMachineName;
    LPWSTR szObjectName;
    LPWSTR szInstanceName;
    LPWSTR szParentInstance;
    DWORD dwInstanceIndex;
    LPWSTR szCounterName;
  } PDH_COUNTER_PATH_ELEMENTS_W,*PPDH_COUNTER_PATH_ELEMENTS_W;

  typedef struct _PDH_DATA_ITEM_PATH_ELEMENTS_A {
    LPSTR szMachineName;
    GUID ObjectGUID;
    DWORD dwItemId;
    LPSTR szInstanceName;
  } PDH_DATA_ITEM_PATH_ELEMENTS_A,*PPDH_DATA_ITEM_PATH_ELEMENTS_A;

  typedef struct _PDH_DATA_ITEM_PATH_ELEMENTS_W {
    LPWSTR szMachineName;
    GUID ObjectGUID;
    DWORD dwItemId;
    LPWSTR szInstanceName;
  } PDH_DATA_ITEM_PATH_ELEMENTS_W,*PPDH_DATA_ITEM_PATH_ELEMENTS_W;

  typedef struct _PDH_COUNTER_INFO_A {
    DWORD dwLength;
    DWORD dwType;
    DWORD CVersion;
    DWORD CStatus;
    LONG lScale;
    LONG lDefaultScale;
    DWORD_PTR dwUserData;
    DWORD_PTR dwQueryUserData;
    LPSTR szFullPath;
    __C89_NAMELESS union {
      PDH_DATA_ITEM_PATH_ELEMENTS_A DataItemPath;
      PDH_COUNTER_PATH_ELEMENTS_A CounterPath;
      __C89_NAMELESS struct {
	LPSTR szMachineName;
	LPSTR szObjectName;
	LPSTR szInstanceName;
	LPSTR szParentInstance;
	DWORD dwInstanceIndex;
	LPSTR szCounterName;
      };
    };
    LPSTR szExplainText;
    DWORD DataBuffer[1];
  } PDH_COUNTER_INFO_A,*PPDH_COUNTER_INFO_A;

  typedef struct _PDH_COUNTER_INFO_W {
    DWORD dwLength;
    DWORD dwType;
    DWORD CVersion;
    DWORD CStatus;
    LONG lScale;
    LONG lDefaultScale;
    DWORD_PTR dwUserData;
    DWORD_PTR dwQueryUserData;
    LPWSTR szFullPath;
    __C89_NAMELESS union {
      PDH_DATA_ITEM_PATH_ELEMENTS_W DataItemPath;
      PDH_COUNTER_PATH_ELEMENTS_W CounterPath;
      __C89_NAMELESS struct {
	LPWSTR szMachineName;
	LPWSTR szObjectName;
	LPWSTR szInstanceName;
	LPWSTR szParentInstance;
	DWORD dwInstanceIndex;
	LPWSTR szCounterName;
      };
    };
    LPWSTR szExplainText;
    DWORD DataBuffer[1];
  } PDH_COUNTER_INFO_W,*PPDH_COUNTER_INFO_W;

  typedef struct _PDH_TIME_INFO {
    LONGLONG StartTime;
    LONGLONG EndTime;
    DWORD SampleCount;
  } PDH_TIME_INFO,*PPDH_TIME_INFO;

  typedef struct _PDH_RAW_LOG_RECORD {
    DWORD dwStructureSize;
    DWORD dwRecordType;
    DWORD dwItems;
    UCHAR RawBytes[1];
  } PDH_RAW_LOG_RECORD,*PPDH_RAW_LOG_RECORD;

  typedef struct _PDH_LOG_SERVICE_QUERY_INFO_A {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwLogQuota;
    LPSTR szLogFileCaption;
    LPSTR szDefaultDir;
    LPSTR szBaseFileName;
    DWORD dwFileType;
    DWORD dwReserved;
    __C89_NAMELESS union {
      __C89_NAMELESS struct {
	DWORD PdlAutoNameInterval;
	DWORD PdlAutoNameUnits;
	LPSTR PdlCommandFilename;
	LPSTR PdlCounterList;
	DWORD PdlAutoNameFormat;
	DWORD PdlSampleInterval;
	FILETIME PdlLogStartTime;
	FILETIME PdlLogEndTime;
      };
      __C89_NAMELESS struct {
	DWORD TlNumberOfBuffers;
	DWORD TlMinimumBuffers;
	DWORD TlMaximumBuffers;
	DWORD TlFreeBuffers;
	DWORD TlBufferSize;
	DWORD TlEventsLost;
	DWORD TlLoggerThreadId;
	DWORD TlBuffersWritten;
	DWORD TlLogHandle;
	LPSTR TlLogFileName;
      };
    };
  } PDH_LOG_SERVICE_QUERY_INFO_A,*PPDH_LOG_SERVICE_QUERY_INFO_A;

  typedef struct _PDH_LOG_SERVICE_QUERY_INFO_W {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwLogQuota;
    LPWSTR szLogFileCaption;
    LPWSTR szDefaultDir;
    LPWSTR szBaseFileName;
    DWORD dwFileType;
    DWORD dwReserved;
    __C89_NAMELESS union {
      __C89_NAMELESS struct {
	DWORD PdlAutoNameInterval;
	DWORD PdlAutoNameUnits;
	LPWSTR PdlCommandFilename;
	LPWSTR PdlCounterList;
	DWORD PdlAutoNameFormat;
	DWORD PdlSampleInterval;
	FILETIME PdlLogStartTime;
	FILETIME PdlLogEndTime;
      };
      __C89_NAMELESS struct {
	DWORD TlNumberOfBuffers;
	DWORD TlMinimumBuffers;
	DWORD TlMaximumBuffers;
	DWORD TlFreeBuffers;
	DWORD TlBufferSize;
	DWORD TlEventsLost;
	DWORD TlLoggerThreadId;
	DWORD TlBuffersWritten;
	DWORD TlLogHandle;
	LPWSTR TlLogFileName;
      };
    };
  } PDH_LOG_SERVICE_QUERY_INFO_W,*PPDH_LOG_SERVICE_QUERY_INFO_W;

#define MAX_TIME_VALUE ((LONGLONG) 0x7FFFFFFFFFFFFFFF)
#define MIN_TIME_VALUE ((LONGLONG) 0)

  PDH_FUNCTION PdhGetDllVersion(LPDWORD lpdwVersion);
  PDH_FUNCTION PdhOpenQueryW(LPCWSTR szDataSource,DWORD_PTR dwUserData,PDH_HQUERY *phQuery);
  PDH_FUNCTION PdhOpenQueryA(LPCSTR szDataSource,DWORD_PTR dwUserData,PDH_HQUERY *phQuery);
  PDH_FUNCTION PdhAddCounterW(PDH_HQUERY hQuery,LPCWSTR szFullCounterPath,DWORD_PTR dwUserData,PDH_HCOUNTER *phCounter);
  PDH_FUNCTION PdhAddCounterA(PDH_HQUERY hQuery,LPCSTR szFullCounterPath,DWORD_PTR dwUserData,PDH_HCOUNTER *phCounter);
  PDH_FUNCTION PdhRemoveCounter(PDH_HCOUNTER hCounter);
  PDH_FUNCTION PdhCollectQueryData(PDH_HQUERY hQuery);
  PDH_FUNCTION PdhCloseQuery(PDH_HQUERY hQuery);
  PDH_FUNCTION PdhGetFormattedCounterValue(PDH_HCOUNTER hCounter,DWORD dwFormat,LPDWORD lpdwType,PPDH_FMT_COUNTERVALUE pValue);
  PDH_FUNCTION PdhGetFormattedCounterArrayA(PDH_HCOUNTER hCounter,DWORD dwFormat,LPDWORD lpdwBufferSize,LPDWORD lpdwItemCount,PPDH_FMT_COUNTERVALUE_ITEM_A ItemBuffer);
  PDH_FUNCTION PdhGetFormattedCounterArrayW(PDH_HCOUNTER hCounter,DWORD dwFormat,LPDWORD lpdwBufferSize,LPDWORD lpdwItemCount,PPDH_FMT_COUNTERVALUE_ITEM_W ItemBuffer);

#define PDH_FMT_RAW ((DWORD) 0x00000010)
#define PDH_FMT_ANSI ((DWORD) 0x00000020)
#define PDH_FMT_UNICODE ((DWORD) 0x00000040)
#define PDH_FMT_LONG ((DWORD) 0x00000100)
#define PDH_FMT_DOUBLE ((DWORD) 0x00000200)
#define PDH_FMT_LARGE ((DWORD) 0x00000400)
#define PDH_FMT_NOSCALE ((DWORD) 0x00001000)
#define PDH_FMT_1000 ((DWORD) 0x00002000)
#define PDH_FMT_NODATA ((DWORD) 0x00004000)
#define PDH_FMT_NOCAP100 ((DWORD) 0x00008000)
#define PERF_DETAIL_COSTLY ((DWORD) 0x00010000)
#define PERF_DETAIL_STANDARD ((DWORD) 0x0000FFFF)

  PDH_FUNCTION PdhGetRawCounterValue(PDH_HCOUNTER hCounter,LPDWORD lpdwType,PPDH_RAW_COUNTER pValue);
  PDH_FUNCTION PdhGetRawCounterArrayA(PDH_HCOUNTER hCounter,LPDWORD lpdwBufferSize,LPDWORD lpdwItemCount,PPDH_RAW_COUNTER_ITEM_A ItemBuffer);
  PDH_FUNCTION PdhGetRawCou