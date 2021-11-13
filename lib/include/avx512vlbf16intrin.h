/*===--------- avx512vlbf16intrin.h - AVX512_BF16 intrinsics ---------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */
#ifndef __IMMINTRIN_H
#error "Never use <avx512vlbf16intrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef __AVX512VLBF16INTRIN_H
#define __AVX512VLBF16INTRIN_H

typedef short __m128bh __attribute__((__vector_size__(16), __aligned__(16)));

#define __DEFAULT_FN_ATTRS128 \
  __attribute__((__always_inline__, __nodebug__, \
                 __target__("avx512vl, avx512bf16"), __min_vector_width__(128)))
#define __DEFAULT_FN_ATTRS256 \
  __attribute__((__always_inline__, __nodebug__, \
                 __target__("avx512vl, avx512bf16"), __min_vector_width__(256)))

/// Convert Two Packed Single Data to One Packed BF16 Data.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VCVTNE2PS2BF16 </c> instructions.
///
/// \param __A
///    A 128-bit vector of [4 x float].
/// \param __B
///    A 128-bit vector of [4 x float].
/// \returns A 128-bit vector of [8 x bfloat] whose lower 64 bits come from
///    conversion of __B, and higher 64 bits come from conversion of __A.
static __inline__ __m128bh __DEFAULT_FN_ATTRS128
_mm_cvtne2ps_pbh(__m128 __A, __m128 __B) {
  return (__m128bh)__builtin_ia32_cvtne2ps2bf16_128((__v4sf) __A,
                                                    (__v4sf) __B);
}

/// Convert Two Packed Single Data to One Packed BF16 Data.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VCVTNE2PS2BF16 </c> instructions.
///
/// \param __A
///    A 128-bit vector of [4 x float].
/// \param __B
///    A 128-bit vector of [4 x float].
/// \param __W
///    A 128-bit vector of [8 x bfloat].
/// \param __U
///    A 8-bit mask value specifying what is chosen for each element.
///    A 1 means conversion of __A or __B. A 0 means element from __W.
/// \returns A 128-bit vector of [8 x bfloat] whose lower 64 bits come from
///    conversion of __B, and higher 64 bits come from conversion of __A.
static __inline__ __m128bh __DEFAULT_FN_ATTRS128
_mm_mask_cvtne2ps_pbh(__m128bh __W, __mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128bh)__builtin_ia32_selectw_128((__mmask8)__U,
                                             (__v8hi)_mm_cvtne2ps_pbh(__A, __B),
                                             (__v8hi)__W);
}

/// Convert Two Packed Single Data to One Packed BF16 Data.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VCVTNE2PS2BF16 </c> instructions.
///
/// \param __A
///    A 128-bit vector of [4 x float].
/// \param __B
///    A 128-bit vector of [4 x float].
/// \param __U
///    A 8-bit mask value specifying what is chosen for each element.
///    A 1 means conversion of __A or __B. A 0 means element is zero.
/// \returns A 128-bit vector of [8 x bfloat] whose lower 64 bits come from
///    conversion of __B, and higher 64 bits come from conversion of __A.
static __inline__ __m128bh __DEFAULT_FN_ATTRS128
_mm_maskz_cvtne2ps_pbh(__mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128bh)__builtin_ia32_selectw_128((__mmask8)__U,
                                             (__v8hi)_mm_cvtne2ps_pbh(__A, __B),
                                             (__v8hi)_mm_setzero_si128());
}

/// Convert Two Packed Single Data to One Packed BF16 Data.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VCVTNE2PS2BF16 </c> instructions.
///
/// \param __A
///    A 256-bit vector of [8 x float].
/// \param __B
///    A 256-bit vector of [8 x float].
/// \returns A 256-bit vector of [16 x bfloat] whose lower 128 bits come from
///    conversion of __B, and higher 128 bits come from conversion of __A.
static __inline__ __m256bh __DEFAULT_FN_ATTRS256
_mm256_cvtne2ps_pbh(__m256 __A, __m256 __B) {
  return (__m256bh)__builtin_ia32_cvtne2ps2bf16_256((__v8sf) __A,
                                                    (__v8sf) __B);
}

/// Convert Two Packed Single Data to One Packed BF16 Data.
///
/// \headerfile <x86intrin.h>
///
/// This intrinsic corresponds to the <c> VCVTNE2PS2BF16 </c> instructions.
///
/// \param __A
///    A 256-bit vector of [8 x float].
/// \param __B
///    A 256-bit vector of [8 x float].
/// \param __W
///    A 256-bit vector of [16 x bfloat].
/// \param __U
///    A 16-bit mask value specifying what is chosen for each element.
///    A 1 means conversion of __A or __B. A 0 means element from __W.
/// \returns A 256-bit vector of [16 x bfloat] whose lower 128 bits come from
///    conversion of __B, and higher 128 bits come from conversion of __A.
static __inline__ __m256bh __DEFAULT_FN_ATTRS256
_mm256_mask_cvtne2ps_pbh(__m256bh __W, __mmask16 __U, __m256 __A, __m256 __B) {
  return (__m256bh)__builtin_ia32_selectw_256((__mmask16)__U,
                                         (__v16hi)_mm256_cvtne2ps_pbh(__A, __B),
                                         (__v16hi)__W);
}

/// Convert Two Packed Single Data to One Packed BF16 Data.
///
/// \headerfile <x86intrin.h>
///
/// This i