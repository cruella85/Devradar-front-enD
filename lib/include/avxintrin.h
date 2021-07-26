/*===---- avxintrin.h - AVX intrinsics -------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __IMMINTRIN_H
#error "Never use <avxintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef __AVXINTRIN_H
#define __AVXINTRIN_H

typedef double __v4df __attribute__ ((__vector_size__ (32)));
typedef float __v8sf __attribute__ ((__vector_size__ (32)));
typedef long long __v4di __attribute__ ((__vector_size__ (32)));
typedef int __v8si __attribute__ ((__vector_size__ (32)));
typedef short __v16hi __attribute__ ((__vector_size__ (32)));
typedef char __v32qi __attribute__ ((__vector_size__ (32)));

/* Unsigned types */
typedef unsigned long long __v4du __attribute__ ((__vector_size__ (32)));
typedef unsigned int __v8su __attribute__ ((__vector_size__ (32)));
typedef unsigned short __v16hu __attribute__ ((__vector_size__ (32)));
typedef unsigned char __v32qu __attribute__ ((__vector_size__ (32)));

/* We need an explicitly signed variant for char. Note that this shouldn't
 * appear in the interface though. */
typedef signed char __v32qs __attribute__((__vector_size__(32)));

typedef float __m256 __attribute__ ((__vector_size__ (32), __aligned__(32)));
typedef double __m256d __attribute__((__vector_size__(32), __aligned__(32)));
typedef long long __m256i __attribute__((__vector_size__(32), __aligned__(32)));

typedef float __m256_u __attribute__ ((__vector_size__ (32), __aligned__(1)));
typedef double __m256d_u __attribute__((__vector_size__(32), __aligned__(1)));
typedef long long __m256i_u __attribute__((__vector_size__(32), __aligned__(1)));

/* Define the default attributes for the functions in this file. */
#define __DEFAULT_FN_ATTRS __attribute__((__always_inline__, __nodebug__, __target__("avx"), __min_vector_width__(256)))
#define __DEFAULT_FN_ATTRS128 __attribute__((__always_inline__, __nodebug__, __target__("avx"), __min_vector_width__(128)))

/* Arithmetic */
/// Adds two 256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VADDPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing one of the source operands.
/// \param __b
///    A 256-bit vector of [4 x double] containing one of the source operands.
/// \returns A 256-bit vector of [4 x double] containing the sums of both
///    operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_add_pd(__m256d __a, __m256d __b)
{
  return (__m256d)((__v4df)__a+(__v4df)__b);
}

/// Adds two 256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VADDPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing one of the source operands.
/// \param __b
///    A 256-bit vector of [8 x float] containing one of the source operands.
/// \returns A 256-bit vector of [8 x float] containing the sums of both
///    operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_add_ps(__m256 __a, __m256 __b)
{
  return (__m256)((__v8sf)__a+(__v8sf)__b);
}

/// Subtracts two 256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VSUBPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing the minuend.
/// \param __b
///    A 256-bit vector of [4 x double] containing the subtrahend.
/// \returns A 256-bit vector of [4 x double] containing the differences between
///    both operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_sub_pd(__m256d __a, __m256d __b)
{
  return (__m256d)((__v4df)__a-(__v4df)__b);
}

/// Subtracts two 256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VSUBPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing the minuend.
/// \param __b
///    A 256-bit vector of [8 x float] containing the subtrahend.
/// \returns A 256-bit vector of [8 x float] containing the differences between
///    both operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_sub_ps(__m256 __a, __m256 __b)
{
  return (__m256)((__v8sf)__a-(__v8sf)__b);
}

/// Adds the even-indexed values and subtracts the odd-indexed values of
///    two 256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VADDSUBPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing the left source operand.
/// \param __b
///    A 256-bit vector of [4 x double] containing the right source operand.
/// \returns A 256-bit vector of [4 x double] containing the alternating sums
///    and differences between both operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_addsub_pd(__m256d __a, __m256d __b)
{
  return (__m256d)__builtin_ia32_addsubpd256