/* Copyright (C) 1995-2021 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

/*
 *      ISO C99 Standard: 7.24
 *	Extended multibyte and wide character utilities	<wchar.h>
 */

#ifndef _WCHAR_H
#define _WCHAR_H 1

#define __GLIBC_INTERNAL_STARTING_HEADER_IMPLEMENTATION
#include <bits/libc-header-start.h>

/* Gather machine dependent type support.  */
#include <bits/floatn.h>

#define __need_size_t
#define __need_wchar_t
#define __need_NULL
#include <stddef.h>

#define __need___va_list
#include <stdarg.h>

#include <bits/wchar.h>
#include <bits/types/wint_t.h>
#include <bits/types/mbstate_t.h>
#include <bits/types/__FILE.h>

#if defined __USE_UNIX98 || defined __USE_XOPEN2K
# include <bits/types/FILE.h>
#endif
#ifdef __USE_XOPEN2K8
# include <bits/types/locale_t.h>
#endif

/* Tell the caller that we provide correct C++ prototypes.  */
#if defined __cplusplus && __GNUC_PREREQ (4, 4)
# define __CORRECT_ISO_CPP_WCHAR_H_PROTO
#endif

#ifndef WCHAR_MIN
/* These constants might also be defined in <inttypes.h>.  */
# define WCHAR_MIN __WCHAR_MIN
# define WCHAR_MAX __WCHAR_MAX
#endif

#ifndef WEOF
# define WEOF (0xffffffffu)
#endif

/* All versions of XPG prior to the publication of ISO C99 required
   the bulk of <wctype.h>'s declarations to appear in this header
   (because <wctype.h> did not exist prior to C99).  In POSIX.1-2001
   those declarations were marked as XSI extensions; in -2008 they
   were additionally marked as obsolescent.  _GNU_SOURCE mode
   anticipates the removal of these declarations in the next revision
   of POSIX.  */
#if (defined __USE_XOPEN && !defined __USE_GNU \
     && !(defined __USE_XOPEN2K && !defined __USE_XOPEN2KXSI))
# include <bits/wctype-wchar.h>
#endif

__BEGIN_DECLS

/* This incomplete type is defined in <time.h> but needed here because
   of `wcsftime'.  */
struct tm;


/* Copy SRC to DEST.  */
extern wchar_t *wcscpy (wchar_t *__restrict __dest,
			const wchar_t *__restrict __src)
     __THROW __nonnull ((1, 2));

/* Copy no more than N wide-characters of SRC to DEST.  */
extern wchar_t *wcsncpy (wchar_t *__restrict __dest,
			 const wchar_t *__restrict __src, size_t __n)
     __THROW __nonnull ((1, 2));

/* Append SRC onto DEST.  */
extern wchar_t *wcscat (wchar_t *__restrict __dest,
			const wchar_t *__restrict __src)
     __THROW __nonnull ((1, 2));
/* Append no more than N wide-characters of SRC onto DEST.  */
extern wchar_t *wcsncat (wchar_t *__restrict __dest,
			 const wchar_t *__restrict __src, size_t __n)
     __THROW __nonnull ((1, 2));

/* Compare S1 and S2.  */
extern int wcscmp (const wchar_t *__s1, const wchar_t *__s2)
     __THROW __attribute_pure__ __nonnull ((1, 2));
/* Compare N wide-characters of S1 and S2.  */
extern int wcsncmp (const wchar_t *__s1, const wchar_t *__s2, size_t __n)
     __THROW __attribute_pure__ __nonnull ((1, 2));

#ifdef __USE_XOPEN2K8
/* Compare S1 and S2, ignoring case.  */
extern int wcscasecmp (const wchar_t *__s1, const wchar_t *__s2) __THROW;

/* Compare no more than N chars of S1 and S2, ignoring case.  */
extern int wcsncasecmp (const wchar_t *__s1, const wchar_t *__s2,
			size_t __n) __THROW;

/* Similar to the two functions above but take the information from
   the provided locale and not the global locale.  */
extern int wcscasecmp_l (const wchar_t *__s1, const wchar_t *__s2,
			 locale_t __loc) __THROW;

extern int wcsncasecmp_l (const wchar_t *__s1, const wchar_t *__s2,
			  size_t __n, locale_t __loc) __THROW;
#endif

/* Compare S1 and S2, both interpreted as appropriate to the
   LC_COLLATE category of the current locale.  */
extern int wcscoll (const wchar_t *__s1, const wchar_t *__s2) __THROW;
/* Transform S2 into array pointed to by S1 such that if wcscmp is
   applied to two transformed strings the result is the as applying
   `wcscoll' to the original strings.  */
extern size_t wcsxfrm (wchar_t *__restrict __s1,
		       const wchar_t *__restrict __s2, size_t __n) __THROW;

#ifdef __USE_XOPEN2K8
/* Similar to the two functions above but take the information from
   the provided locale and not the global locale.  */

/* Compare S1 and S2, both interpreted as appropriate to the
   LC_COLLATE category of the given locale.  */
extern int wcscoll_l (const wchar_t *__s1, const wchar_t *__s2,
		      locale_t __loc) __THROW;

/* Transform S2 into array pointed to by S1 such that if wcscmp is
   applied to two transformed strings the result is the as applying
   `wcscoll' to the original strings.  */
extern size_t wcsxfrm_l (wchar_t *__s1, const wchar_t *__s2,
			 size_t __n, locale_t __loc) __THROW;

/* Duplicate S, returning an identical malloc'd string.  */
extern wchar_t *wcsdup (const wchar_t *__s) __THROW
  __attribute_malloc__ __attr_dealloc_free;
#endif

/* Find the first occurrence of WC in WCS.  */
#ifdef __CORRECT_ISO_CPP_WCHAR_H_PROTO
extern "C++" wchar_t *wcschr (wchar_t *__wcs, wchar_t __wc)
     __THROW __asm ("wcschr") __attribute_pure__;
extern "C++" const wchar_t *wcschr (const wchar_t *__wcs, wchar_t __wc)
     __THROW __asm ("wcschr") __attribute_pure__;
#else
extern wchar_t *wcschr (const wchar_t *__wcs, wchar_t __wc)
     __THROW __attribute_pure__;
#endif
/* Find the last occurrence of WC in WCS.  */
#ifdef __CORRECT_ISO_CPP_WCHAR_H_PROTO
extern "C++" wchar_t *wcsrchr (wchar_t *__wcs, wchar_t __wc)
     __THROW __asm ("wcsrchr") __attribute_pure__;
extern "C++" const wchar_t *wcsrchr (const wchar_t *__wcs, wchar_t __wc)
     __THROW __asm ("wcsrchr") __attribute_pure__;
#else
extern wchar_t *wcsrchr (const wchar_t *__wcs, wchar_t __wc)
     __T