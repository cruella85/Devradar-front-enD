#ifdef TEST_FTRUNCATE64
#include <fcntl.h>
#include <sys/stat.h>
#endif /* TEST_FTRUNCATE64 */

#include <stdio.h>
#include <unistd.h>
#include <io.h>
#include <stdlib.h>
#include <errno.h>
#include <wchar.h>
#include <windows.h>
#include <psapi.h>

/* Mutually exclusive methods
  We check disk space as truncating more than the allowed space results
  in file getting mysteriously deleted
 */
#define _CHECK_SPACE_BY_VOLUME_METHOD_ 1 /* Needs to walk through all volumes */
#define _CHECK_SPACE_BY_PSAPI_METHOD_ 0 /* Requires psapi.dll */
#define _CHECK_SPACE_BY_VISTA_METHOD_ 0 /* Won't work on XP */

#if (_CHECK_SPACE_BY_PSAPI_METHOD_ == 1) /* Retrive actual volume path */
static LPWSTR getdirpath(const LPWSTR __str){
  int len, walk = 0;
  LPWSTR dirname;
  while (__str[walk] != L'\0'){
    walk++;
    if (__str[walk] == L'\\') len = walk + 1;
  }
  dirname = calloc(len + 1, sizeof(wchar_t));
  if (!dirname) return dirname; /* memory error */
  return wcsncpy(dirname,__str,len);
}

static LPWSTR xp_normalize_fn(const LPWSTR fn) {
  DWORD len, err, walker, isfound;
  LPWSTR drives = NULL;
  LPWSTR target = NULL;
  LPWSTR ret = NULL;
  wchar_t tmplt[3] = L" :"; /* Template */

  /*Get list of drive letters */
  len = GetLogicalDriveStringsW(0,NULL);
  drives = calloc(len,sizeof(wchar_t));
  if (!drives) return NULL;
  len = GetLogicalDriveStringsW(len,drives);

  /*Allocatate memory */
  target = calloc(MAX_PATH + 1,sizeof(wchar_t));
  if (!target) {
    free(drives);
    return NULL;
  }

  walker = 0;
  while ((walker < len) && !(drives[walker] == L'\0' && drives[walker + 1] == L'\0')){
    /* search through alphabets */
    if(iswalpha(drives[walker])) {
      *tmplt = drives[walker]; /* Put drive letter */
      err = QueryDosDeviceW(tmplt,target,MAX_PATH);
      if(!err) {
        free(drives);
        free(target);
        return NULL;
      }
      if( _wcsnicmp(target,fn,wcslen(target)) == 0) break;
      wmemset(target,L'\0',MAX_PATH);
      walker++;
    } else walker++;
  }

  if (!iswalpha(*tmplt)) {
    free(drives);
    free(target);
    return NULL; /* Finish walking without finding correct drive */
  }

  ret = calloc(MAX_PATH + 1,sizeof(wchar_t));
  if (!ret) {
    free(drives);
    free(target);
    return NULL;
  }
  _snwprintf(ret,MAX_PATH,L"%ws%ws",tmplt,fn+wcslen(target));

  return ret;
}

/* XP method of retrieving filename from handles, based on:
  http://msdn.microsoft.com/en-us/library/aa366789%28VS.85%29.aspx
 */
static LPWSTR xp_getfilepath(const HANDLE f, const LARGE_INTEGER fsize){
  HANDLE hFileMap = NULL;
  void* pMem = NULL;
  LPWSTR temp, ret;
  DWORD err;

  temp = calloc(MAX_PATH + 1, sizeof(wchar_t));
  if (!temp) goto errormap;

  /* CreateFileMappingW limitation: Cannot map 0 byte files, so extend it to 1 byte */
  if (!fsize.QuadPart) {
    SetFilePointer(f, 1, NULL, FILE_BEGIN);
    err = SetEndOfFile(f);
    if(!temp) goto errormap;
  }

  hFileMap = CreateFileMappingW(f,NULL,PAGE_READONLY,0,1,NULL);
  if(!hFileMap) goto errormap;
  pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);
  if(!pMem) goto errormap;
  err = GetMappedFileNameW(GetCurrentProcess(),pMem,temp,MAX_PATH);
  if(!err) goto errormap;

  if (pMem) UnmapViewOfFile(pMem);
  if (hFileMap) CloseHandle(hFileMap);
  ret = xp_normalize_fn(temp);
  free(temp);
  return ret;

  errormap:
  if (temp) free(temp);
  if (pMem) UnmapViewOfFile(pMem);
  if (hFileMap) CloseHandle(hFileMap);
  _set_errno(EBADF);
  return NULL;
}
#endif /* _CHECK_SPACE_BY_PSAPI_METHOD_ */

static int
checkfreespace (const HANDLE f, const ULONGLONG requiredspace)
{
  LPWSTR dirpath, volumeid, volumepath;
  ULARGE_INTEGER freespace;
  LARGE_INTEGER currentsize;
  DWORD check, volumeserial;
  BY_HANDLE_FILE_INFORMATION fileinfo;
  HANDLE vol;

  /* Get current size */
  check = GetFileSizeEx (f, &currentsize);
  if (!check)
  {
    _set_errno(EBADF);
    return -1; /* Error checking file size */
  }

  /* Short circuit disk space check if shrink operation */
  if ((ULONGLONG)currentsize.QuadPart >= requiredspace)
    return 0;

  /* We check available space to user before attempting to truncate */

#if (_CHECK_SPACE_BY_VISTA_METHOD_ == 1)
  /* Get path length */
  DWORD err;
  LPWSTR filepath = NULL;
  check = GetFinalPathNameByHandleW(f,filepath,0,FILE_NAME_NORMALIZED|VOLUME_NAME_GUID);
  err = GetLastError();
  if (err == ERROR_PATH_NOT_FOUND || err == ERROR_INVALID_PARAMETER) {
     _set_errno(EINVAL);
     return -1; /* IO error */
  }
  filepath = calloc(check + 1,sizeof(wchar_t));
  if (!filepath) {
    _set_errno(EBADF);
    return -1; /* Out of memory */
  }
  check = GetFinalPathNameByHandleW(f,filepath,check