/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _WOWNT16_
#define _WOWNT16_

#ifdef __cplusplus
extern "C" {
#endif

  DWORD WINAPI GetVDMPointer32W(LPVOID vp,UINT fMode);
  DWORD WINAPI LoadLibraryEx32W(LPCSTR lpszLibFile,DWORD hFile,DWORD dwFlags);
  DWORD WINAPI GetProcAddress32W(DWORD hModule,LPCSTR lpszProc);
  DWORD WINAPI FreeLibrary32W(DWORD hLibMo