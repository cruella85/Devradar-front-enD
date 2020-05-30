/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_CLFSW32
#define _INC_CLFSW32
#include <clfs.h>
#include <clfsmgmt.h>

#if (_WIN32_WINNT >= 0x0600)
#ifdef __cplusplus
extern "C" {
#endif

typedef PVOID (* CLFS_BLOCK_ALLOCATION) (ULONG cbBufferSize, PVOID pvUserContext);
typedef void  (* CLFS_BLOCK_DEALLOCATION) (PVOID pvBuffer, PVOID pvUserContext);
typedef FILE *PFILE;
typedef ULONG (__stdcall * CLFS_PRINT_RECORD_ROUTINE) (PFILE, CLFS_RECORD_TYPE, PVOID, ULONG);

WINBOOL WINAPI AdvanceLogBase(PVOID pvMarshal,PCLFS_LSN plsnBase,ULONG fFlags,LPOVERLAPPED pOverlapped);

WINBOOL WINAPI AlignReservedLog(PVOID pvMarshal,ULONG cReservedRecords,LONGLONG rgcbReservation,PLONGLONG pcbAlignReservation);
WINBOOL WINAPI AllocReservedLog(PVOID pvMarshal,ULONG cReservedRecords,PLONGLONG pcbAdjustment);

WINBOOL WINAPI AddLogContainer(HA