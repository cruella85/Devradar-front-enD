/*
 * Copyright (c) 1999-2006 Apple Inc.  All Rights Reserved.
 * 
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
// Copyright 1988-1996 NeXT Software, Inc.

#ifndef _OBJC_OBJC_API_H_
#define _OBJC_OBJC_API_H_

#include <Availability.h>
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>
#include <sys/types.h>

#ifndef __has_feature
#   define __has_feature(x) 0
#endif

#ifndef __has_extension
#   define __has_extension __has_feature
#endif

#ifndef __has_attribute
#   define __has_attribute(x) 0
#endif

#if !__has_feature(nullability)
#   ifndef _Nullable
#       define _Nullable
#   endif
#   ifndef _Nonnull
#       define _Nonnull
#   endif
#   ifndef _Null_unspecified
#       define _Null_unspecified
#   endif
#endif



/*
 * OBJC_API_VERSION 0 or undef: Tiger and earlier API only
 * OBJC_API_VERSION 2: Leopard and later API available
 */
#if !defined(OBJC_API_VERSION)
#   if defined(__MAC_OS_X_VERSION_MIN_REQUIRED)  &&  __MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_5
#       define OBJC_API_VERSION 0
#   else
#       define OBJC_API_VERSION 2
#   endif
#endif


/*
 * OBJC_NO_GC 1: GC is not supported
 * OBJC_NO_GC undef: GC is supported. This SDK no longer supports this mode.
 *
 * OBJC_NO_GC_API undef: Libraries must export any symbols that 
 *                       dual-mode code may links to.
 * OBJC_NO_GC_API 1: Libraries need not export GC-related symbols.
 */
#if defined(__OBJC_GC__)
#   error Objective-C garbage collection is not supported.
#elif TARGET_OS_OSX
    /* GC is unsupported. GC API symbols are exported. */
#   define OBJC_NO_GC 1
#   undef  OBJC_NO_GC_API
#else
    /* GC is unsupported. GC API symbols are not exported. */
#   define OBJC_NO_GC 1
#   define OBJC_NO_GC_API 1
#endif


/* NS_ENFORCE_NSOBJECT_DESIGNATED_INITIALIZER == 1 
 * 