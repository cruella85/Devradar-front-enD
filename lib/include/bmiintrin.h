/*===---- bmiintrin.h - BMI intrinsics -------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#if !defined __X86INTRIN_H && !defined __IMMINTRIN_H
#error "Never use <bmiintrin.h> directly; include <x86intrin.h> instead."
#endif

#ifndef __BMIINTRIN_H
#define __BMIINTRIN_H

/* Allow using the tzcnt intrinsics even for non-BMI targets. Since the TZCNT
   instruction behaves as BSF on non-BMI targets, there is code that expects
   to use it as a potentially faster version of BSF. */
#define __RELAXED_FN_ATTRS __attribute__((__always_inline__, __nodebug__))

#define _tzcnt_u16(a)     (__tzcnt_u16((a)))

/// Counts the number of trailing zero bits in the operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> TZCNT </c> instruction.
///
/// \param __X
///    An unsigned 16-bit integer whose trailing zeros are to be counted.
/// \returns An unsigned 16-bit integer containing the number of trailing zero
///    bits in the operand.
static __inline__ unsigned short __RELAXED_FN_ATTRS
__tzcnt_u16(unsigned short __X)
{
  return __builtin_ia32_tzcnt_u16(__X);
}

/// Counts the number of trailing zero bits in the operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> TZCNT </c> instruction.
///
/// \param __X
///    An unsigned 32-bit integer whose trailing zeros are to be counted.
/// \returns An unsigned 32-bit integer containing the number of trailing zero
///    bits in the operand.
/// \see _mm_tzcnt_32
static __inline__ unsigned int __RELAXED_FN_ATTRS
__tzcnt_u32(unsigned int __X)
{
  return __builtin_ia32_tzcnt_u32(__X);
}

/// Counts the number of trailing zero bits in the operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> TZCNT </c> instruction.
///
/// \param __X
///    An unsigned 32-bit integer whose trailing zeros are to be counted.
/// \returns An 32-bit integer containing the number of trailing zero bits in
///    the operand.
/// \see __tzcnt_u32
static __inline__ int __RELAXED_FN_ATTRS
_mm_tzcnt_32(unsigned int __X)
{
  return (int)__builtin_ia32_tzcnt_u32(__X);
}

#define _tzcnt_u32(a)     (__tzcnt_u32((a)))

#ifdef __x86_64__

/// Counts the number of trailing zero bits in the operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> TZCNT </c> instruction.
///
/// \param __X
///    An unsigned 64-bit integer whose trailing zeros are to be counted.
/// \returns An unsigned 64-bit integer containing the number of trailing zero
///    bits in the operand.
/// \see _mm_tzcnt_64
static __inline__ unsigned long long __RELAXED_FN_ATTRS
__tzcnt_u64(unsigned long long __X)
{
  return __builtin_ia32_tzcnt_u64(__X);
}

/// Counts the number of trailing zero bits in the operand.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> TZCNT </c> instruction.
///
/// \param __X
///    An unsigned 64-bit integer whose trailing zeros are to be counted.
/// \returns An 64-bit integer containing the number of trailing zero bits in
///    the operand.
/// \see __tzcnt_u64
static __inline__ long long __RELAXED_FN_ATTRS
_mm_tzcnt_64(unsigned long long __X)
{
  return (long long)__builtin_ia32_tzcnt_u64(__X);
}

#define _tzcnt_u64(a)     (__tzcnt_u64((a)))

#endif /* __x86_64__ */

#undef __RELAXED_FN_ATTRS

#if !(defined(_MSC_V