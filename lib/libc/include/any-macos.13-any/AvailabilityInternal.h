
/*
 * Copyright (c) 2007-2016 by Apple Inc.. All rights reserved.
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

/*
    File:       AvailabilityInternal.h
 
    Contains:   implementation details of __OSX_AVAILABLE_* macros from <Availability.h>

*/
#ifndef __AVAILABILITY_INTERNAL__
#define __AVAILABILITY_INTERNAL__

#if __has_include(<AvailabilityInternalPrivate.h>)
  #include <AvailabilityInternalPrivate.h>
#endif

#ifndef __MAC_OS_X_VERSION_MIN_REQUIRED
    #ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
        /* compiler for Mac OS X sets __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ */
        #define __MAC_OS_X_VERSION_MIN_REQUIRED __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
    #endif
#endif /* __MAC_OS_X_VERSION_MIN_REQUIRED*/

#ifndef __IPHONE_OS_VERSION_MIN_REQUIRED
    #ifdef __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__
        /* compiler sets __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ when -miphoneos-version-min is used */
        #define __IPHONE_OS_VERSION_MIN_REQUIRED __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__
    /* set to 1 when RC_FALLBACK_PLATFORM=iphoneos */
    #elif 0
        #define __IPHONE_OS_VERSION_MIN_REQUIRED 0
    #endif
#endif /* __IPHONE_OS_VERSION_MIN_REQUIRED */

#ifndef __TV_OS_VERSION_MIN_REQUIRED
    #ifdef __ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__
        /* compiler sets __ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__ when -mtvos-version-min is used */
        #define __TV_OS_VERSION_MIN_REQUIRED __ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__
        #define __TV_OS_VERSION_MAX_ALLOWED __TVOS_16_1
        /* for compatibility with existing code.  New code should use platform specific checks */
        #define __IPHONE_OS_VERSION_MIN_REQUIRED 90000
    #endif
#endif

#ifndef __WATCH_OS_VERSION_MIN_REQUIRED
    #ifdef __ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__
        /* compiler sets __ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__ when -mwatchos-version-min is used */
        #define __WATCH_OS_VERSION_MIN_REQUIRED __ENVIRONMENT_WATCH_OS_VERSION_MIN_REQUIRED__
        #define __WATCH_OS_VERSION_MAX_ALLOWED __WATCHOS_9_1
        /* for compatibility with existing code.  New code should use platform specific checks */
        #define __IPHONE_OS_VERSION_MIN_REQUIRED 90000
    #endif
#endif

#ifndef __BRIDGE_OS_VERSION_MIN_REQUIRED
    #ifdef __ENVIRONMENT_BRIDGE_OS_VERSION_MIN_REQUIRED__
        
        #define __BRIDGE_OS_VERSION_MIN_REQUIRED __ENVIRONMENT_BRIDGE_OS_VERSION_MIN_REQUIRED__
        #define __BRIDGE_OS_VERSION_MAX_ALLOWED 70100
        /* for compatibility with existing code.  New code should use platform specific checks */
        #define __IPHONE_OS_VERSION_MIN_REQUIRED 110000
    #endif
#endif

#ifndef __DRIVERKIT_VERSION_MIN_REQUIRED
    #ifdef __ENVIRONMENT_DRIVERKIT_VERSION_MIN_REQUIRED__
        #define __DRIVERKIT_VERSION_MIN_REQUIRED __ENVIRONMENT_DRIVERKIT_VERSION_MIN_REQUIRED__
    #endif
#endif

#ifdef __MAC_OS_X_VERSION_MIN_REQUIRED
    /* make sure a default max version is set */
    #ifndef __MAC_OS_X_VERSION_MAX_ALLOWED
        #define __MAC_OS_X_VERSION_MAX_ALLOWED __MAC_13_0
    #endif
#endif /* __MAC_OS_X_VERSION_MIN_REQUIRED */

#ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
    /* make sure a default max version is set */
    #ifndef __IPHONE_OS_VERSION_MAX_ALLOWED
        #define __IPHONE_OS_VERSION_MAX_ALLOWED     __IPHONE_16_1
    #endif
    /* make sure a valid min is set */
    #if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_2_0
        #undef __IPHONE_OS_VERSION_MIN_REQUIRED
        #define __IPHONE_OS_VERSION_MIN_REQUIRED    __IPHONE_2_0
    #endif
#endif

#define __AVAILABILITY_INTERNAL_DEPRECATED            __attribute__((deprecated))
#ifdef __has_feature
    #if __has_feature(attribute_deprecated_with_message)
        #define __AVAILABILITY_INTERNAL_DEPRECATED_MSG(_msg)  __attribute__((deprecated(_msg)))
    #else
        #define __AVAILABILITY_INTERNAL_DEPRECATED_MSG(_msg)  __attribute__((deprecated))
    #endif
#elif defined(__GNUC__) && ((__GNUC__ >= 5) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)))
    #define __AVAILABILITY_INTERNAL_DEPRECATED_MSG(_msg)  __attribute__((deprecated(_msg)))
#else
    #define __AVAILABILITY_INTERNAL_DEPRECATED_MSG(_msg)  __attribute__((deprecated))
#endif
#define __AVAILABILITY_INTERNAL_UNAVAILABLE           __attribute__((unavailable))
#define __AVAILABILITY_INTERNAL_WEAK_IMPORT           __attribute__((weak_import))
#define __AVAILABILITY_INTERNAL_REGULAR            

#if defined(__has_builtin)
 #if __has_builtin(__is_target_arch)
  #if __has_builtin(__is_target_vendor)
   #if __has_builtin(__is_target_os)
    #if __has_builtin(__is_target_environment)
     #if __has_builtin(__is_target_variant_os)
      #if __has_builtin(__is_target_variant_environment)
       #if (__is_target_arch(x86_64) && __is_target_vendor(apple) && ((__is_target_os(ios) && __is_target_environment(macabi)) || (__is_target_variant_os(ios) && __is_target_variant_environment(macabi))))
         #define __ENABLE_LEGACY_IPHONE_AVAILABILITY 1
         #define __ENABLE_LEGACY_MAC_AVAILABILITY 1
       #endif /* # if __is_target_arch... */
      #endif /* #if __has_builtin(__is_target_variant_environment) */
     #endif /* #if __has_builtin(__is_target_variant_os) */
    #endif /* #if __has_builtin(__is_target_environment) */
   #endif /* #if __has_builtin(__is_target_os) */
  #endif /* #if __has_builtin(__is_target_vendor) */
 #endif /* #if __has_builtin(__is_target_arch) */
#endif /* #if defined(__has_builtin) */

#ifndef __ENABLE_LEGACY_IPHONE_AVAILABILITY
 #ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
  #define __ENABLE_LEGACY_IPHONE_AVAILABILITY 1
 #elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
  #define __ENABLE_LEGACY_MAC_AVAILABILITY 1
 #endif
#endif /* __ENABLE_LEGACY_IPHONE_AVAILABILITY */

#ifdef __ENABLE_LEGACY_IPHONE_AVAILABILITY
    #if defined(__has_attribute) && defined(__has_feature)
        #if __has_attribute(availability)
            /* use better attributes if possible */
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0                    __attribute__((availability(ios,introduced=2.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=2.0,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=2.0,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=2.0,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=2.0,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_11_0   __attribute__((availability(ios,introduced=2.0,deprecated=11.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_2_0   __attribute__((availability(ios,introduced=2.0,deprecated=2.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_2_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=2.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_2_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=2.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_2_1   __attribute__((availability(ios,introduced=2.0,deprecated=2.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_2_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=2.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_2_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=2.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_2_2   __attribute__((availability(ios,introduced=2.0,deprecated=2.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_2_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=2.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_2_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=2.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_3_0   __attribute__((availability(ios,introduced=2.0,deprecated=3.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_3_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=3.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_3_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=3.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_3_1   __attribute__((availability(ios,introduced=2.0,deprecated=3.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=3.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=3.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_3_2   __attribute__((availability(ios,introduced=2.0,deprecated=3.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=3.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=3.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_0   __attribute__((availability(ios,introduced=2.0,deprecated=4.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=4.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=4.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_1   __attribute__((availability(ios,introduced=2.0,deprecated=4.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=4.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=4.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_2   __attribute__((availability(ios,introduced=2.0,deprecated=4.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=4.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=4.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=2.0,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=2.0,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=2.0,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=2.0,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=2.0,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=2.0,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=2.0,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=2.0,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=2.0,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=2.0,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=2.0,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=2.0,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=2.0,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=2.0,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=2.0,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=2.0,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.0,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=2.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_2_0_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=2.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1                    __attribute__((availability(ios,introduced=2.1)))
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=2.1,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=2.1,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=2.1,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=2.1,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_2_1   __attribute__((availability(ios,introduced=2.1,deprecated=2.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_2_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=2.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_2_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=2.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_2_2   __attribute__((availability(ios,introduced=2.1,deprecated=2.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_2_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=2.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_2_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=2.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_3_0   __attribute__((availability(ios,introduced=2.1,deprecated=3.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_3_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=3.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_3_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=3.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_3_1   __attribute__((availability(ios,introduced=2.1,deprecated=3.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=3.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=3.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_3_2   __attribute__((availability(ios,introduced=2.1,deprecated=3.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=3.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=3.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_0   __attribute__((availability(ios,introduced=2.1,deprecated=4.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=4.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=4.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_1   __attribute__((availability(ios,introduced=2.1,deprecated=4.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=4.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=4.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_2   __attribute__((availability(ios,introduced=2.1,deprecated=4.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=4.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=4.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=2.1,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=2.1,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=2.1,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=2.1,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=2.1,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=2.1,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=2.1,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=2.1,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=2.1,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=2.1,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=2.1,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=2.1,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=2.1,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=2.1,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=2.1,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=2.1,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.1,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=2.1)))
            #define __AVAILABILITY_INTERNAL__IPHONE_2_1_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=2.1)))
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2                    __attribute__((availability(ios,introduced=2.2)))
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=2.2,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=2.2,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=2.2,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=2.2,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_2_2   __attribute__((availability(ios,introduced=2.2,deprecated=2.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_2_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=2.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_2_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=2.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_3_0   __attribute__((availability(ios,introduced=2.2,deprecated=3.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_3_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=3.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_3_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=3.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_3_1   __attribute__((availability(ios,introduced=2.2,deprecated=3.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=3.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=3.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_3_2   __attribute__((availability(ios,introduced=2.2,deprecated=3.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=3.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=3.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_0   __attribute__((availability(ios,introduced=2.2,deprecated=4.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=4.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=4.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_1   __attribute__((availability(ios,introduced=2.2,deprecated=4.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=4.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=4.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_2   __attribute__((availability(ios,introduced=2.2,deprecated=4.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=4.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=4.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=2.2,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=2.2,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=2.2,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=2.2,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=2.2,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=2.2,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=2.2,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=2.2,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=2.2,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=2.2,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=2.2,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=2.2,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=2.2,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=2.2,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=2.2,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=2.2,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=2.2,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=2.2)))
            #define __AVAILABILITY_INTERNAL__IPHONE_2_2_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=2.2)))
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0                    __attribute__((availability(ios,introduced=3.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=3.0,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=3.0,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=3.0,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=3.0,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_0   __attribute__((availability(ios,introduced=3.0,deprecated=3.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=3.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=3.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_1   __attribute__((availability(ios,introduced=3.0,deprecated=3.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=3.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=3.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_2   __attribute__((availability(ios,introduced=3.0,deprecated=3.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=3.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=3.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_0   __attribute__((availability(ios,introduced=3.0,deprecated=4.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=4.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=4.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_1   __attribute__((availability(ios,introduced=3.0,deprecated=4.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=4.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=4.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_2   __attribute__((availability(ios,introduced=3.0,deprecated=4.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=4.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=4.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=3.0,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=3.0,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=3.0,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=3.0,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=3.0,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=3.0,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=3.0,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=3.0,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=3.0,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=3.0,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=3.0,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=3.0,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=3.0,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=3.0,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=3.0,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=3.0,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.0,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=3.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_3_0_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=3.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1                    __attribute__((availability(ios,introduced=3.1)))
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=3.1,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=3.1,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=3.1,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=3.1,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_3_1   __attribute__((availability(ios,introduced=3.1,deprecated=3.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=3.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_3_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=3.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_3_2   __attribute__((availability(ios,introduced=3.1,deprecated=3.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=3.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=3.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_0   __attribute__((availability(ios,introduced=3.1,deprecated=4.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=4.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=4.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_1   __attribute__((availability(ios,introduced=3.1,deprecated=4.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=4.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=4.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_2   __attribute__((availability(ios,introduced=3.1,deprecated=4.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=4.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=4.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=3.1,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=3.1,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=3.1,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=3.1,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=3.1,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=3.1,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=3.1,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=3.1,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=3.1,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=3.1,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=3.1,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=3.1,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=3.1,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=3.1,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=3.1,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=3.1,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.1,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=3.1)))
            #define __AVAILABILITY_INTERNAL__IPHONE_3_1_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=3.1)))
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2                    __attribute__((availability(ios,introduced=3.2)))
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=3.2,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=3.2,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=3.2,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=3.2,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_3_2   __attribute__((availability(ios,introduced=3.2,deprecated=3.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=3.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_3_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=3.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_0   __attribute__((availability(ios,introduced=3.2,deprecated=4.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=4.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=4.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_1   __attribute__((availability(ios,introduced=3.2,deprecated=4.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=4.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=4.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_2   __attribute__((availability(ios,introduced=3.2,deprecated=4.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=4.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=4.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=3.2,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=3.2,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=3.2,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=3.2,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=3.2,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=3.2,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=3.2,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=3.2,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=3.2,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=3.2,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=3.2,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=3.2,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=3.2,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=3.2,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=3.2,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=3.2,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=3.2,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=3.2)))
            #define __AVAILABILITY_INTERNAL__IPHONE_3_2_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=3.2)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0                    __attribute__((availability(ios,introduced=4.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=4.0,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=4.0,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=4.0,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=4.0,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=10.3)))
            #endif
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_12_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=12.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_12_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=12.0)))
            #endif            
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_0   __attribute__((availability(ios,introduced=4.0,deprecated=4.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=4.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=4.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_1   __attribute__((availability(ios,introduced=4.0,deprecated=4.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=4.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=4.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_2   __attribute__((availability(ios,introduced=4.0,deprecated=4.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=4.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=4.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=4.0,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=4.0,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=4.0,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=4.0,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=4.0,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=4.0,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=4.0,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=4.0,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=4.0,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=4.0,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=4.0,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=4.0,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=4.0,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=4.0,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=4.0,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=4.0,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.0,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=4.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_0_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=4.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1                    __attribute__((availability(ios,introduced=4.1)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=4.1,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=4.1,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=4.1,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=4.1,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_1   __attribute__((availability(ios,introduced=4.1,deprecated=4.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=4.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=4.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_2   __attribute__((availability(ios,introduced=4.1,deprecated=4.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=4.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=4.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=4.1,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=4.1,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=4.1,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=4.1,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=4.1,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=4.1,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=4.1,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=4.1,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=4.1,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=4.1,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=4.1,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=4.1,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=4.1,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=4.1,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=4.1,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=4.1,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.1,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=4.1)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_1_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=4.1)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2                    __attribute__((availability(ios,introduced=4.2)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=4.2,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=4.2,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=4.2,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=4.2,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_4_2   __attribute__((availability(ios,introduced=4.2,deprecated=4.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=4.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_4_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=4.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=4.2,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=4.2,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=4.2,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=4.2,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=4.2,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=4.2,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=4.2,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=4.2,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=4.2,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=4.2,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=4.2,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=4.2,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=4.2,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=4.2,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=4.2,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=4.2,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.2,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=4.2)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_2_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=4.2)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3                    __attribute__((availability(ios,introduced=4.3)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=4.3,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=4.3,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=4.3,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=4.3,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_4_3   __attribute__((availability(ios,introduced=4.3,deprecated=4.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=4.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_4_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=4.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=4.3,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=4.3,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=4.3,deprecated=6.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=6.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_6_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=6.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_6_1   __attribute__((availability(ios,introduced=4.3,deprecated=6.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=6.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_6_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=6.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_7_0   __attribute__((availability(ios,introduced=4.3,deprecated=7.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=7.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_7_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=7.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_7_1   __attribute__((availability(ios,introduced=4.3,deprecated=7.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=7.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_7_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=7.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_0   __attribute__((availability(ios,introduced=4.3,deprecated=8.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_1   __attribute__((availability(ios,introduced=4.3,deprecated=8.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_2   __attribute__((availability(ios,introduced=4.3,deprecated=8.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_3   __attribute__((availability(ios,introduced=4.3,deprecated=8.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_4   __attribute__((availability(ios,introduced=4.3,deprecated=8.4)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.4,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_8_4_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=8.4)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_0   __attribute__((availability(ios,introduced=4.3,deprecated=9.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=9.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_0_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=9.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_1   __attribute__((availability(ios,introduced=4.3,deprecated=9.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=9.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_1_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=9.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_2   __attribute__((availability(ios,introduced=4.3,deprecated=9.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=9.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_2_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=9.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_3   __attribute__((availability(ios,introduced=4.3,deprecated=9.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=9.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_9_3_MSG(_msg)   __attribute__((availability(ios,introduced=4.3,deprecated=9.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_NA   __attribute__((availability(ios,introduced=4.3)))
            #define __AVAILABILITY_INTERNAL__IPHONE_4_3_DEP__IPHONE_NA_MSG(_msg)   __attribute__((availability(ios,introduced=4.3)))
            #define __AVAILABILITY_INTERNAL__IPHONE_5_0                    __attribute__((availability(ios,introduced=5.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_0   __attribute__((availability(ios,introduced=5.0,deprecated=10.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=10.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_0_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=10.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_1   __attribute__((availability(ios,introduced=5.0,deprecated=10.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=10.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_1_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=10.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_2   __attribute__((availability(ios,introduced=5.0,deprecated=10.2)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=10.2,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_2_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=10.2)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_3   __attribute__((availability(ios,introduced=5.0,deprecated=10.3)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=10.3,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_10_3_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=10.3)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_11_0   __attribute__((availability(ios,introduced=5.0,deprecated=11.0)))
            #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_5_0   __attribute__((availability(ios,introduced=5.0,deprecated=5.0)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=5.0,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_5_0_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=5.0)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_5_1   __attribute__((availability(ios,introduced=5.0,deprecated=5.1)))
            #if __has_feature(attribute_availability_with_message)
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=5.1,message=_msg)))
            #else
                #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_5_1_MSG(_msg)   __attribute__((availability(ios,introduced=5.0,deprecated=5.1)))
            #endif
            #define __AVAILABILITY_INTERNAL__IPHONE_5_0_DEP__IPHONE_6_0   __attribute__((availability(ios,introduced=5.0,deprecated=6.0)))