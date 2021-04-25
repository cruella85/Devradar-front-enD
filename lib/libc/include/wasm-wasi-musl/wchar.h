#ifndef _WCHAR_H
#define _WCHAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#define __NEED_FILE
#define __NEED___isoc_va_list
#define __NEED_size_t
#define __NEED_wchar_t
#define __NEED_wint_t
#define __NEED_mbstate_t

#ifdef __wasilibc_unmodified_upstream /* WASI doesn't need to define FILE as a complete type */
#if __STDC_VERSION__ < 201112L
#define __NEED_struct__IO_FILE
#endif
#endif

#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) \
 || defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define __NEED_locale_t
#define __NEED_va_list
#endif

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define __NEED_wctype_t
#endif

#include <bits/alltypes.h>

#if L'\0'-1 > 0
#define WCHAR_MAX (0xffffffffu+L'\0')
#define WCHAR_MIN (0+L'\0')
#else
#define WCHAR_MAX (0x7fffffff+L'\0')
#define WCHAR_MIN (-1-0x7fffffff+L'\0')
#endif

#ifdef __wasilibc_unmodified_upstream /* Use the compiler's definition of NULL */
#if __cplusplus >= 201103L
#define NULL nullptr
#elif defined(__cplusplus)
#define NULL 0L
#else
#define NULL ((void*)0)
#endif
#else
#define __need_NULL
#include <stddef.h>
#endif

#undef WEOF
#define WEOF 0xffffffffU

wchar_t *wcscpy (wchar_t *__restrict, const wchar_t *__restrict);
wchar_t *wcsncpy (wchar_t *__restrict, const wchar_t *__restrict, size_t);

wchar_t *wcscat (wchar_t *__restrict, const wchar_t *__restrict);
wchar_t *wcsncat (wchar_t *__restrict, const wchar_t *__restrict, size_t);

int wcscmp (const wchar_t *, const wchar_t *);
int wcsncmp (const wchar_t *, const wchar_t *, size_t);

int wcscoll(const wchar_t *, const wchar_t *);
size_t wcsxfrm (wchar_t *__restrict, const wchar_t *__restrict, size_t);

wchar_t *wcschr (const wchar_t *, wchar_t);
wchar_t *wcsrchr (const wchar_t *, wchar_t);

size_t wcscspn (const wchar_t *, const wchar_t *);
size_t wcsspn (const wchar_t *, const wchar_t *);
wchar_t *wcspbrk (const wchar_t *, const wchar_t *);

wchar_t *wcstok (wchar_t *__restrict, const wchar_t *__restrict, wchar_t **__restrict);

size_t wcslen (const wchar_t *);

wchar_t *wcsstr (const wchar_t *__restrict, const wchar_t *__restrict);
wchar_t *wcswcs (const wchar_t *, const wchar_t *);

wchar_t *wmemchr (const wchar_t *, wchar_t, size_t);
int wmemcmp (const wchar_t *, const wchar_t *, size_t);
wchar_t *wmemcpy (wchar_t *__restrict, const wchar_t *__restrict, size_t);
wchar_t *wmemmove (wchar_t *, const wchar_t *, size_t);
wchar_t *wmemset (wchar_t *, wchar_t, size_t);

wint_t btowc (int);
int wctob (wint_t);

int mbsinit (const mbstate_t *);
size_t mbrtowc (wchar_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);
size_t wcrtomb (char *__restrict, wchar_t, mbstate_t *__restrict);

size_t mbrlen (const char *__restrict, size_t, mbstate_t *__restrict);

size_t mbsrtowcs (wchar_t *__restrict, const char **__restrict, size_t, mbstate_t *__restrict);
size_t wcsrtombs (char *__restrict, const wchar_t **__restrict, size_t, mbstate_t *__restrict);

float wcstof (const wchar_t *__restrict, wchar_t **__restrict);
double wcstod (const wchar_t *__restrict, wchar_t **__restrict);
long double wcstold (const wchar_t *__restrict, wchar_t **__restrict);

long wcstol (const wchar_t *__restrict, wchar_t **__restrict, int);
unsigned long wcstoul (const wchar_t *__restrict, wchar_t *