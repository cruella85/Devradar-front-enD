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
  return (__m256d)__builtin_ia32_addsubpd256((__v4df)__a, (__v4df)__b);
}

/// Adds the even-indexed values and subtracts the odd-indexed values of
///    two 256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VADDSUBPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing the left source operand.
/// \param __b
///    A 256-bit vector of [8 x float] containing the right source operand.
/// \returns A 256-bit vector of [8 x float] containing the alternating sums and
///    differences between both operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_addsub_ps(__m256 __a, __m256 __b)
{
  return (__m256)__builtin_ia32_addsubps256((__v8sf)__a, (__v8sf)__b);
}

/// Divides two 256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VDIVPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing the dividend.
/// \param __b
///    A 256-bit vector of [4 x double] containing the divisor.
/// \returns A 256-bit vector of [4 x double] containing the quotients of both
///    operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_div_pd(__m256d __a, __m256d __b)
{
  return (__m256d)((__v4df)__a/(__v4df)__b);
}

/// Divides two 256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VDIVPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing the dividend.
/// \param __b
///    A 256-bit vector of [8 x float] containing the divisor.
/// \returns A 256-bit vector of [8 x float] containing the quotients of both
///    operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_div_ps(__m256 __a, __m256 __b)
{
  return (__m256)((__v8sf)__a/(__v8sf)__b);
}

/// Compares two 256-bit vectors of [4 x double] and returns the greater
///    of each pair of values.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VMAXPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing one of the operands.
/// \param __b
///    A 256-bit vector of [4 x double] containing one of the operands.
/// \returns A 256-bit vector of [4 x double] containing the maximum values
///    between both operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_max_pd(__m256d __a, __m256d __b)
{
  return (__m256d)__builtin_ia32_maxpd256((__v4df)__a, (__v4df)__b);
}

/// Compares two 256-bit vectors of [8 x float] and returns the greater
///    of each pair of values.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VMAXPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing one of the operands.
/// \param __b
///    A 256-bit vector of [8 x float] containing one of the operands.
/// \returns A 256-bit vector of [8 x float] containing the maximum values
///    between both operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_max_ps(__m256 __a, __m256 __b)
{
  return (__m256)__builtin_ia32_maxps256((__v8sf)__a, (__v8sf)__b);
}

/// Compares two 256-bit vectors of [4 x double] and returns the lesser
///    of each pair of values.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VMINPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing one of the operands.
/// \param __b
///    A 256-bit vector of [4 x double] containing one of the operands.
/// \returns A 256-bit vector of [4 x double] containing the minimum values
///    between both operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_min_pd(__m256d __a, __m256d __b)
{
  return (__m256d)__builtin_ia32_minpd256((__v4df)__a, (__v4df)__b);
}

/// Compares two 256-bit vectors of [8 x float] and returns the lesser
///    of each pair of values.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VMINPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing one of the operands.
/// \param __b
///    A 256-bit vector of [8 x float] containing one of the operands.
/// \returns A 256-bit vector of [8 x float] containing the minimum values
///    between both operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_min_ps(__m256 __a, __m256 __b)
{
  return (__m256)__builtin_ia32_minps256((__v8sf)__a, (__v8sf)__b);
}

/// Multiplies two 256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VMULPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing one of the operands.
/// \param __b
///    A 256-bit vector of [4 x double] containing one of the operands.
/// \returns A 256-bit vector of [4 x double] containing the products of both
///    operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_mul_pd(__m256d __a, __m256d __b)
{
  return (__m256d)((__v4df)__a * (__v4df)__b);
}

/// Multiplies two 256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VMULPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing one of the operands.
/// \param __b
///    A 256-bit vector of [8 x float] containing one of the operands.
/// \returns A 256-bit vector of [8 x float] containing the products of both
///    operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_mul_ps(__m256 __a, __m256 __b)
{
  return (__m256)((__v8sf)__a * (__v8sf)__b);
}

/// Calculates the square roots of the values in a 256-bit vector of
///    [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VSQRTPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double].
/// \returns A 256-bit vector of [4 x double] containing the square roots of the
///    values in the operand.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_sqrt_pd(__m256d __a)
{
  return (__m256d)__builtin_ia32_sqrtpd256((__v4df)__a);
}

/// Calculates the square roots of the values in a 256-bit vector of
///    [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VSQRTPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float].
/// \returns A 256-bit vector of [8 x float] containing the square roots of the
///    values in the operand.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_sqrt_ps(__m256 __a)
{
  return (__m256)__builtin_ia32_sqrtps256((__v8sf)__a);
}

/// Calculates the reciprocal square roots of the values in a 256-bit
///    vector of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VRSQRTPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float].
/// \returns A 256-bit vector of [8 x float] containing the reciprocal square
///    roots of the values in the operand.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_rsqrt_ps(__m256 __a)
{
  return (__m256)__builtin_ia32_rsqrtps256((__v8sf)__a);
}

/// Calculates the reciprocals of the values in a 256-bit vector of
///    [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VRCPPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float].
/// \returns A 256-bit vector of [8 x float] containing the reciprocals of the
///    values in the operand.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_rcp_ps(__m256 __a)
{
  return (__m256)__builtin_ia32_rcpps256((__v8sf)__a);
}

/// Rounds the values in a 256-bit vector of [4 x double] as specified
///    by the byte operand. The source values are rounded to integer values and
///    returned as 64-bit double-precision floating-point values.
///
/// \headerfile <x86intrin.h>
///
/// \code
/// __m256d _mm256_round_pd(__m256d V, const int M);
/// \endcode
///
/// This intrinsic corresponds to the <c> VROUNDPD </c> instruction.
///
/// \param V
///    A 256-bit vector of [4 x double].
/// \param M
///    An integer value that specifies the rounding operation. \n
///    Bits [7:4] are reserved. \n
///    Bit [3] is a precision exception value: \n
///      0: A normal PE exception is used. \n
///      1: The PE field is not updated. \n
///    Bit [2] is the rounding control source: \n
///      0: Use bits [1:0] of \a M. \n
///      1: Use the current MXCSR setting. \n
///    Bits [1:0] contain the rounding control definition: \n
///      00: Nearest. \n
///      01: Downward (toward negative infinity). \n
///      10: Upward (toward positive infinity). \n
///      11: Truncated.
/// \returns A 256-bit vector of [4 x double] containing the rounded values.
#define _mm256_round_pd(V, M) \
  ((__m256d)__builtin_ia32_roundpd256((__v4df)(__m256d)(V), (M)))

/// Rounds the values stored in a 256-bit vector of [8 x float] as
///    specified by the byte operand. The source values are rounded