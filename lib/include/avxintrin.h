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
///    specified by the byte operand. The source values are rounded to integer
///    values and returned as floating-point values.
///
/// \headerfile <x86intrin.h>
///
/// \code
/// __m256 _mm256_round_ps(__m256 V, const int M);
/// \endcode
///
/// This intrinsic corresponds to the <c> VROUNDPS </c> instruction.
///
/// \param V
///    A 256-bit vector of [8 x float].
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
/// \returns A 256-bit vector of [8 x float] containing the rounded values.
#define _mm256_round_ps(V, M) \
  ((__m256)__builtin_ia32_roundps256((__v8sf)(__m256)(V), (M)))

/// Rounds up the values stored in a 256-bit vector of [4 x double]. The
///    source values are rounded up to integer values and returned as 64-bit
///    double-precision floating-point values.
///
/// \headerfile <x86intrin.h>
///
/// \code
/// __m256d _mm256_ceil_pd(__m256d V);
/// \endcode
///
/// This intrinsic corresponds to the <c> VROUNDPD </c> instruction.
///
/// \param V
///    A 256-bit vector of [4 x double].
/// \returns A 256-bit vector of [4 x double] containing the rounded up values.
#define _mm256_ceil_pd(V)  _mm256_round_pd((V), _MM_FROUND_CEIL)

/// Rounds down the values stored in a 256-bit vector of [4 x double].
///    The source values are rounded down to integer values and returned as
///    64-bit double-precision floating-point values.
///
/// \headerfile <x86intrin.h>
///
/// \code
/// __m256d _mm256_floor_pd(__m256d V);
/// \endcode
///
/// This intrinsic corresponds to the <c> VROUNDPD </c> instruction.
///
/// \param V
///    A 256-bit vector of [4 x double].
/// \returns A 256-bit vector of [4 x double] containing the rounded down
///    values.
#define _mm256_floor_pd(V) _mm256_round_pd((V), _MM_FROUND_FLOOR)

/// Rounds up the values stored in a 256-bit vector of [8 x float]. The
///    source values are rounded up to integer values and returned as
///    floating-point values.
///
/// \headerfile <x86intrin.h>
///
/// \code
/// __m256 _mm256_ceil_ps(__m256 V);
/// \endcode
///
/// This intrinsic corresponds to the <c> VROUNDPS </c> instruction.
///
/// \param V
///    A 256-bit vector of [8 x float].
/// \returns A 256-bit vector of [8 x float] containing the rounded up values.
#define _mm256_ceil_ps(V)  _mm256_round_ps((V), _MM_FROUND_CEIL)

/// Rounds down the values stored in a 256-bit vector of [8 x float]. The
///    source values are rounded down to integer values and returned as
///    floating-point values.
///
/// \headerfile <x86intrin.h>
///
/// \code
/// __m256 _mm256_floor_ps(__m256 V);
/// \endcode
///
/// This intrinsic corresponds to the <c> VROUNDPS </c> instruction.
///
/// \param V
///    A 256-bit vector of [8 x float].
/// \returns A 256-bit vector of [8 x float] containing the rounded down values.
#define _mm256_floor_ps(V) _mm256_round_ps((V), _MM_FROUND_FLOOR)

/* Logical */
/// Performs a bitwise AND of two 256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VANDPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing one of the source operands.
/// \param __b
///    A 256-bit vector of [4 x double] containing one of the source operands.
/// \returns A 256-bit vector of [4 x double] containing the bitwise AND of the
///    values between both operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_and_pd(__m256d __a, __m256d __b)
{
  return (__m256d)((__v4du)__a & (__v4du)__b);
}

/// Performs a bitwise AND of two 256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VANDPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing one of the source operands.
/// \param __b
///    A 256-bit vector of [8 x float] containing one of the source operands.
/// \returns A 256-bit vector of [8 x float] containing the bitwise AND of the
///    values between both operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_and_ps(__m256 __a, __m256 __b)
{
  return (__m256)((__v8su)__a & (__v8su)__b);
}

/// Performs a bitwise AND of two 256-bit vectors of [4 x double], using
///    the one's complement of the values contained in the first source operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VANDNPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing the left source operand. The
///    one's complement of this value is used in the bitwise AND.
/// \param __b
///    A 256-bit vector of [4 x double] containing the right source operand.
/// \returns A 256-bit vector of [4 x double] containing the bitwise AND of the
///    values of the second operand and the one's complement of the first
///    operand.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_andnot_pd(__m256d __a, __m256d __b)
{
  return (__m256d)(~(__v4du)__a & (__v4du)__b);
}

/// Performs a bitwise AND of two 256-bit vectors of [8 x float], using
///    the one's complement of the values contained in the first source operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VANDNPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing the left source operand. The
///    one's complement of this value is used in the bitwise AND.
/// \param __b
///    A 256-bit vector of [8 x float] containing the right source operand.
/// \returns A 256-bit vector of [8 x float] containing the bitwise AND of the
///    values of the second operand and the one's complement of the first
///    operand.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_andnot_ps(__m256 __a, __m256 __b)
{
  return (__m256)(~(__v8su)__a & (__v8su)__b);
}

/// Performs a bitwise OR of two 256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VORPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing one of the source operands.
/// \param __b
///    A 256-bit vector of [4 x double] containing one of the source operands.
/// \returns A 256-bit vector of [4 x double] containing the bitwise OR of the
///    values between both operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_or_pd(__m256d __a, __m256d __b)
{
  return (__m256d)((__v4du)__a | (__v4du)__b);
}

/// Performs a bitwise OR of two 256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VORPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing one of the source operands.
/// \param __b
///    A 256-bit vector of [8 x float] containing one of the source operands.
/// \returns A 256-bit vector of [8 x float] containing the bitwise OR of the
///    values between both operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_or_ps(__m256 __a, __m256 __b)
{
  return (__m256)((__v8su)__a | (__v8su)__b);
}

/// Performs a bitwise XOR of two 256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VXORPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing one of the source operands.
/// \param __b
///    A 256-bit vector of [4 x double] containing one of the source operands.
/// \returns A 256-bit vector of [4 x double] containing the bitwise XOR of the
///    values between both operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_xor_pd(__m256d __a, __m256d __b)
{
  return (__m256d)((__v4du)__a ^ (__v4du)__b);
}

/// Performs a bitwise XOR of two 256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VXORPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing one of the source operands.
/// \param __b
///    A 256-bit vector of [8 x float] containing one of the source operands.
/// \returns A 256-bit vector of [8 x float] containing the bitwise XOR of the
///    values between both operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_xor_ps(__m256 __a, __m256 __b)
{
  return (__m256)((__v8su)__a ^ (__v8su)__b);
}

/* Horizontal arithmetic */
/// Horizontally adds the adjacent pairs of values contained in two
///    256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VHADDPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing one of the source operands.
///    The horizontal sums of the values are returned in the even-indexed
///    elements of a vector of [4 x double].
/// \param __b
///    A 256-bit vector of [4 x double] containing one of the source operands.
///    The horizontal sums of the values are returned in the odd-indexed
///    elements of a vector of [4 x double].
/// \returns A 256-bit vector of [4 x double] containing the horizontal sums of
///    both operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_hadd_pd(__m256d __a, __m256d __b)
{
  return (__m256d)__builtin_ia32_haddpd256((__v4df)__a, (__v4df)__b);
}

/// Horizontally adds the adjacent pairs of values contained in two
///    256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VHADDPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing one of the source operands.
///    The horizontal sums of the values are returned in the elements with
///    index 0, 1, 4, 5 of a vector of [8 x float].
/// \param __b
///    A 256-bit vector of [8 x float] containing one of the source operands.
///    The horizontal sums of the values are returned in the elements with
///    index 2, 3, 6, 7 of a vector of [8 x float].
/// \returns A 256-bit vector of [8 x float] containing the horizontal sums of
///    both operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_hadd_ps(__m256 __a, __m256 __b)
{
  return (__m256)__builtin_ia32_haddps256((__v8sf)__a, (__v8sf)__b);
}

/// Horizontally subtracts the adjacent pairs of values contained in two
///    256-bit vectors of [4 x double].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VHSUBPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double] containing one of the source operands.
///    The horizontal differences between the values are returned in the
///    even-indexed elements of a vector of [4 x double].
/// \param __b
///    A 256-bit vector of [4 x double] containing one of the source operands.
///    The horizontal differences between the values are returned in the
///    odd-indexed elements of a vector of [4 x double].
/// \returns A 256-bit vector of [4 x double] containing the horizontal
///    differences of both operands.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_hsub_pd(__m256d __a, __m256d __b)
{
  return (__m256d)__builtin_ia32_hsubpd256((__v4df)__a, (__v4df)__b);
}

/// Horizontally subtracts the adjacent pairs of values contained in two
///    256-bit vectors of [8 x float].
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VHSUBPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float] containing one of the source operands.
///    The horizontal differences between the values are returned in the
///    elements with index 0, 1, 4, 5 of a vector of [8 x float].
/// \param __b
///    A 256-bit vector of [8 x float] containing one of the source operands.
///    The horizontal differences between the values are returned in the
///    elements with index 2, 3, 6, 7 of a vector of [8 x float].
/// \returns A 256-bit vector of [8 x float] containing the horizontal
///    differences of both operands.
static __inline __m256 __DEFAULT_FN_ATTRS
_mm256_hsub_ps(__m256 __a, __m256 __b)
{
  return (__m256)__builtin_ia32_hsubps256((__v8sf)__a, (__v8sf)__b);
}

/* Vector permutations */
/// Copies the values in a 128-bit vector of [2 x double] as specified
///    by the 128-bit integer vector operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VPERMILPD </c> instruction.
///
/// \param __a
///    A 128-bit vector of [2 x double].
/// \param __c
///    A 128-bit integer vector operand specifying how the values are to be
///    copied. \n
///    Bit [1]: \n
///      0: Bits [63:0] of the source are copied to bits [63:0] of the returned
///         vector. \n
///      1: Bits [127:64] of the source are copied to bits [63:0] of the
///         returned vector. \n
///    Bit [65]: \n
///      0: Bits [63:0] of the source are copied to bits [127:64] of the
///         returned vector. \n
///      1: Bits [127:64] of the source are copied to bits [127:64] of the
///         returned vector.
/// \returns A 128-bit vector of [2 x double] containing the copied values.
static __inline __m128d __DEFAULT_FN_ATTRS128
_mm_permutevar_pd(__m128d __a, __m128i __c)
{
  return (__m128d)__builtin_ia32_vpermilvarpd((__v2df)__a, (__v2di)__c);
}

/// Copies the values in a 256-bit vector of [4 x double] as specified
///    by the 256-bit integer vector operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VPERMILPD </c> instruction.
///
/// \param __a
///    A 256-bit vector of [4 x double].
/// \param __c
///    A 256-bit integer vector operand specifying how the values are to be
///    copied. \n
///    Bit [1]: \n
///      0: Bits [63:0] of the source are copied to bits [63:0] of the returned
///         vector. \n
///      1: Bits [127:64] of the source are copied to bits [63:0] of the
///         returned vector. \n
///    Bit [65]: \n
///      0: Bits [63:0] of the source are copied to bits [127:64] of the
///         returned vector. \n
///      1: Bits [127:64] of the source are copied to bits [127:64] of the
///         returned vector. \n
///    Bit [129]: \n
///      0: Bits [191:128] of the source are copied to bits [191:128] of the
///         returned vector. \n
///      1: Bits [255:192] of the source are copied to bits [191:128] of the
///         returned vector. \n
///    Bit [193]: \n
///      0: Bits [191:128] of the source are copied to bits [255:192] of the
///         returned vector. \n
///      1: Bits [255:192] of the source are copied to bits [255:192] of the
///    returned vector.
/// \returns A 256-bit vector of [4 x double] containing the copied values.
static __inline __m256d __DEFAULT_FN_ATTRS
_mm256_permutevar_pd(__m256d __a, __m256i __c)
{
  return (__m256d)__builtin_ia32_vpermilvarpd256((__v4df)__a, (__v4di)__c);
}

/// Copies the values stored in a 128-bit vector of [4 x float] as
///    specified by the 128-bit integer vector operand.
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VPERMILPS </c> instruction.
///
/// \param __a
///    A 128-bit vector of [4 x float].
/// \param __c
///    A 128-bit integer vector operand specifying how the values are to be
///    copied. \n
///    Bits [1:0]: \n
///      00: Bits [31:0] of the source are copied to bits [31:0] of the
///          returned vector. \n
///      01: Bits [63:32] of the source are copied to bits [31:0] of the
///          returned vector. \n
///      10: Bits [95:64] of the source are copied to bits [31:0] of the
///          returned vector. \n
///      11: Bits [127:96] of the source are copied to bits [31:0] of the
///          returned vector. \n
///    Bits [33:32]: \n
///      00: Bits [31:0] of the source are copied to bits [63:32] of the
///          returned vector. \n
///      01: Bits [63:32] of the source are copied to bits [63:32] of the
///          returned vector. \n
///      10: Bits [95:64] of the source are copied to bits [63:32] of the
///          returned vector. \n
///      11: Bits [127:96] of the source are copied to bits [63:32] of the
///          returned vector. \n
///    Bits [65:64]: \n
///      00: Bits [31:0] of the source are copied to bits [95:64] of the
///          returned vector. \n
///      01: Bits [63:32] of the source are copied to bits [95:64] of the
///          returned vector. \n
///      10: Bits [95:64] of the source are copied to bits [95:64] of the
///          returned vector. \n
///      11: Bits [127:96] of the source are copied to bits [95:64] of the
///          returned vector. \n
///    Bits [97:96]: \n
///      00: Bits [31:0] of the source are copied to bits [127:96] of the
///          returned vector. \n
///      01: Bits [63:32] of the source are copied to bits [127:96] of the
///          returned vector. \n
///      10: Bits [95:64] of the source are copied to bits [127:96] of the
///          returned vector. \n
///      11: Bits [127:96] of the source are copied to bits [127:96] of the
///          returned vector.
/// \returns A 128-bit vector of [4 x float] containing the copied values.
static __inline __m128 __DEFAULT_FN_ATTRS128
_mm_permutevar_ps(__m128 __a, __m128i __c)
{
  return (__m128)__builtin_ia32_vpermilvarps((__v4sf)__a, (__v4si)__c);
}

/// Copies the values stored in a 256-bit vector of [8 x float] as
///    specified by the 256-bit integer vector operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VPERMILPS </c> instruction.
///
/// \param __a
///    A 256-bit vector of [8 x float].
/// \param __c
///    A 256-bit integer vector operand specifying how the valu