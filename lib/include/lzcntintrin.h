/*===---- lzcntintrin.h - LZCNT intrinsics ---------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <lzcntintrin.h> directly; include <x86intrin.h> instead."
#endif

#ifndef __LZCNTINTRIN_H
#define __LZCNTINTRIN_H

/* Define the default attributes for the functions in this file. */
#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("lzcnt")))

#ifndef _MSC_VER
/// Counts the number of leadi