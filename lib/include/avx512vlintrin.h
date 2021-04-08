
/*===---- avx512vlintrin.h - AVX512VL intrinsics ---------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __IMMINTRIN_H
#error "Never use <avx512vlintrin.h> directly; include <immintrin.h> instead."
#endif

#ifndef __AVX512VLINTRIN_H
#define __AVX512VLINTRIN_H

#define __DEFAULT_FN_ATTRS128 __attribute__((__always_inline__, __nodebug__, __target__("avx512vl"), __min_vector_width__(128)))
#define __DEFAULT_FN_ATTRS256 __attribute__((__always_inline__, __nodebug__, __target__("avx512vl"), __min_vector_width__(256)))

typedef short __v2hi __attribute__((__vector_size__(4)));
typedef char __v4qi __attribute__((__vector_size__(4)));
typedef char __v2qi __attribute__((__vector_size__(2)));

/* Integer compare */

#define _mm_cmpeq_epi32_mask(A, B) \
    _mm_cmp_epi32_mask((A), (B), _MM_CMPINT_EQ)
#define _mm_mask_cmpeq_epi32_mask(k, A, B) \
    _mm_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_EQ)
#define _mm_cmpge_epi32_mask(A, B) \
    _mm_cmp_epi32_mask((A), (B), _MM_CMPINT_GE)
#define _mm_mask_cmpge_epi32_mask(k, A, B) \
    _mm_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_GE)
#define _mm_cmpgt_epi32_mask(A, B) \
    _mm_cmp_epi32_mask((A), (B), _MM_CMPINT_GT)
#define _mm_mask_cmpgt_epi32_mask(k, A, B) \
    _mm_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_GT)
#define _mm_cmple_epi32_mask(A, B) \
    _mm_cmp_epi32_mask((A), (B), _MM_CMPINT_LE)
#define _mm_mask_cmple_epi32_mask(k, A, B) \
    _mm_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_LE)
#define _mm_cmplt_epi32_mask(A, B) \
    _mm_cmp_epi32_mask((A), (B), _MM_CMPINT_LT)
#define _mm_mask_cmplt_epi32_mask(k, A, B) \
    _mm_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_LT)
#define _mm_cmpneq_epi32_mask(A, B) \
    _mm_cmp_epi32_mask((A), (B), _MM_CMPINT_NE)
#define _mm_mask_cmpneq_epi32_mask(k, A, B) \
    _mm_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_NE)

#define _mm256_cmpeq_epi32_mask(A, B) \
    _mm256_cmp_epi32_mask((A), (B), _MM_CMPINT_EQ)
#define _mm256_mask_cmpeq_epi32_mask(k, A, B) \
    _mm256_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_EQ)
#define _mm256_cmpge_epi32_mask(A, B) \
    _mm256_cmp_epi32_mask((A), (B), _MM_CMPINT_GE)
#define _mm256_mask_cmpge_epi32_mask(k, A, B) \
    _mm256_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_GE)
#define _mm256_cmpgt_epi32_mask(A, B) \
    _mm256_cmp_epi32_mask((A), (B), _MM_CMPINT_GT)
#define _mm256_mask_cmpgt_epi32_mask(k, A, B) \
    _mm256_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_GT)
#define _mm256_cmple_epi32_mask(A, B) \
    _mm256_cmp_epi32_mask((A), (B), _MM_CMPINT_LE)
#define _mm256_mask_cmple_epi32_mask(k, A, B) \
    _mm256_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_LE)
#define _mm256_cmplt_epi32_mask(A, B) \
    _mm256_cmp_epi32_mask((A), (B), _MM_CMPINT_LT)
#define _mm256_mask_cmplt_epi32_mask(k, A, B) \
    _mm256_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_LT)
#define _mm256_cmpneq_epi32_mask(A, B) \
    _mm256_cmp_epi32_mask((A), (B), _MM_CMPINT_NE)
#define _mm256_mask_cmpneq_epi32_mask(k, A, B) \
    _mm256_mask_cmp_epi32_mask((k), (A), (B), _MM_CMPINT_NE)

#define _mm_cmpeq_epu32_mask(A, B) \
    _mm_cmp_epu32_mask((A), (B), _MM_CMPINT_EQ)
#define _mm_mask_cmpeq_epu32_mask(k, A, B) \
    _mm_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_EQ)
#define _mm_cmpge_epu32_mask(A, B) \
    _mm_cmp_epu32_mask((A), (B), _MM_CMPINT_GE)
#define _mm_mask_cmpge_epu32_mask(k, A, B) \
    _mm_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_GE)
#define _mm_cmpgt_epu32_mask(A, B) \
    _mm_cmp_epu32_mask((A), (B), _MM_CMPINT_GT)
#define _mm_mask_cmpgt_epu32_mask(k, A, B) \
    _mm_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_GT)
#define _mm_cmple_epu32_mask(A, B) \
    _mm_cmp_epu32_mask((A), (B), _MM_CMPINT_LE)
#define _mm_mask_cmple_epu32_mask(k, A, B) \
    _mm_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_LE)
#define _mm_cmplt_epu32_mask(A, B) \
    _mm_cmp_epu32_mask((A), (B), _MM_CMPINT_LT)
#define _mm_mask_cmplt_epu32_mask(k, A, B) \
    _mm_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_LT)
#define _mm_cmpneq_epu32_mask(A, B) \
    _mm_cmp_epu32_mask((A), (B), _MM_CMPINT_NE)
#define _mm_mask_cmpneq_epu32_mask(k, A, B) \
    _mm_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_NE)

#define _mm256_cmpeq_epu32_mask(A, B) \
    _mm256_cmp_epu32_mask((A), (B), _MM_CMPINT_EQ)
#define _mm256_mask_cmpeq_epu32_mask(k, A, B) \
    _mm256_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_EQ)
#define _mm256_cmpge_epu32_mask(A, B) \
    _mm256_cmp_epu32_mask((A), (B), _MM_CMPINT_GE)
#define _mm256_mask_cmpge_epu32_mask(k, A, B) \
    _mm256_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_GE)
#define _mm256_cmpgt_epu32_mask(A, B) \
    _mm256_cmp_epu32_mask((A), (B), _MM_CMPINT_GT)
#define _mm256_mask_cmpgt_epu32_mask(k, A, B) \
    _mm256_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_GT)
#define _mm256_cmple_epu32_mask(A, B) \
    _mm256_cmp_epu32_mask((A), (B), _MM_CMPINT_LE)
#define _mm256_mask_cmple_epu32_mask(k, A, B) \
    _mm256_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_LE)
#define _mm256_cmplt_epu32_mask(A, B) \
    _mm256_cmp_epu32_mask((A), (B), _MM_CMPINT_LT)
#define _mm256_mask_cmplt_epu32_mask(k, A, B) \
    _mm256_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_LT)
#define _mm256_cmpneq_epu32_mask(A, B) \
    _mm256_cmp_epu32_mask((A), (B), _MM_CMPINT_NE)
#define _mm256_mask_cmpneq_epu32_mask(k, A, B) \
    _mm256_mask_cmp_epu32_mask((k), (A), (B), _MM_CMPINT_NE)

#define _mm_cmpeq_epi64_mask(A, B) \
    _mm_cmp_epi64_mask((A), (B), _MM_CMPINT_EQ)
#define _mm_mask_cmpeq_epi64_mask(k, A, B) \
    _mm_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_EQ)
#define _mm_cmpge_epi64_mask(A, B) \
    _mm_cmp_epi64_mask((A), (B), _MM_CMPINT_GE)
#define _mm_mask_cmpge_epi64_mask(k, A, B) \
    _mm_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_GE)
#define _mm_cmpgt_epi64_mask(A, B) \
    _mm_cmp_epi64_mask((A), (B), _MM_CMPINT_GT)
#define _mm_mask_cmpgt_epi64_mask(k, A, B) \
    _mm_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_GT)
#define _mm_cmple_epi64_mask(A, B) \
    _mm_cmp_epi64_mask((A), (B), _MM_CMPINT_LE)
#define _mm_mask_cmple_epi64_mask(k, A, B) \
    _mm_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_LE)
#define _mm_cmplt_epi64_mask(A, B) \
    _mm_cmp_epi64_mask((A), (B), _MM_CMPINT_LT)
#define _mm_mask_cmplt_epi64_mask(k, A, B) \
    _mm_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_LT)
#define _mm_cmpneq_epi64_mask(A, B) \
    _mm_cmp_epi64_mask((A), (B), _MM_CMPINT_NE)
#define _mm_mask_cmpneq_epi64_mask(k, A, B) \
    _mm_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_NE)

#define _mm256_cmpeq_epi64_mask(A, B) \
    _mm256_cmp_epi64_mask((A), (B), _MM_CMPINT_EQ)
#define _mm256_mask_cmpeq_epi64_mask(k, A, B) \
    _mm256_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_EQ)
#define _mm256_cmpge_epi64_mask(A, B) \
    _mm256_cmp_epi64_mask((A), (B), _MM_CMPINT_GE)
#define _mm256_mask_cmpge_epi64_mask(k, A, B) \
    _mm256_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_GE)
#define _mm256_cmpgt_epi64_mask(A, B) \
    _mm256_cmp_epi64_mask((A), (B), _MM_CMPINT_GT)
#define _mm256_mask_cmpgt_epi64_mask(k, A, B) \
    _mm256_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_GT)
#define _mm256_cmple_epi64_mask(A, B) \
    _mm256_cmp_epi64_mask((A), (B), _MM_CMPINT_LE)
#define _mm256_mask_cmple_epi64_mask(k, A, B) \
    _mm256_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_LE)
#define _mm256_cmplt_epi64_mask(A, B) \
    _mm256_cmp_epi64_mask((A), (B), _MM_CMPINT_LT)
#define _mm256_mask_cmplt_epi64_mask(k, A, B) \
    _mm256_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_LT)
#define _mm256_cmpneq_epi64_mask(A, B) \
    _mm256_cmp_epi64_mask((A), (B), _MM_CMPINT_NE)
#define _mm256_mask_cmpneq_epi64_mask(k, A, B) \
    _mm256_mask_cmp_epi64_mask((k), (A), (B), _MM_CMPINT_NE)

#define _mm_cmpeq_epu64_mask(A, B) \
    _mm_cmp_epu64_mask((A), (B), _MM_CMPINT_EQ)
#define _mm_mask_cmpeq_epu64_mask(k, A, B) \
    _mm_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_EQ)
#define _mm_cmpge_epu64_mask(A, B) \
    _mm_cmp_epu64_mask((A), (B), _MM_CMPINT_GE)
#define _mm_mask_cmpge_epu64_mask(k, A, B) \
    _mm_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_GE)
#define _mm_cmpgt_epu64_mask(A, B) \
    _mm_cmp_epu64_mask((A), (B), _MM_CMPINT_GT)
#define _mm_mask_cmpgt_epu64_mask(k, A, B) \
    _mm_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_GT)
#define _mm_cmple_epu64_mask(A, B) \
    _mm_cmp_epu64_mask((A), (B), _MM_CMPINT_LE)
#define _mm_mask_cmple_epu64_mask(k, A, B) \
    _mm_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_LE)
#define _mm_cmplt_epu64_mask(A, B) \
    _mm_cmp_epu64_mask((A), (B), _MM_CMPINT_LT)
#define _mm_mask_cmplt_epu64_mask(k, A, B) \
    _mm_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_LT)
#define _mm_cmpneq_epu64_mask(A, B) \
    _mm_cmp_epu64_mask((A), (B), _MM_CMPINT_NE)
#define _mm_mask_cmpneq_epu64_mask(k, A, B) \
    _mm_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_NE)

#define _mm256_cmpeq_epu64_mask(A, B) \
    _mm256_cmp_epu64_mask((A), (B), _MM_CMPINT_EQ)
#define _mm256_mask_cmpeq_epu64_mask(k, A, B) \
    _mm256_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_EQ)
#define _mm256_cmpge_epu64_mask(A, B) \
    _mm256_cmp_epu64_mask((A), (B), _MM_CMPINT_GE)
#define _mm256_mask_cmpge_epu64_mask(k, A, B) \
    _mm256_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_GE)
#define _mm256_cmpgt_epu64_mask(A, B) \
    _mm256_cmp_epu64_mask((A), (B), _MM_CMPINT_GT)
#define _mm256_mask_cmpgt_epu64_mask(k, A, B) \
    _mm256_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_GT)
#define _mm256_cmple_epu64_mask(A, B) \
    _mm256_cmp_epu64_mask((A), (B), _MM_CMPINT_LE)
#define _mm256_mask_cmple_epu64_mask(k, A, B) \
    _mm256_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_LE)
#define _mm256_cmplt_epu64_mask(A, B) \
    _mm256_cmp_epu64_mask((A), (B), _MM_CMPINT_LT)
#define _mm256_mask_cmplt_epu64_mask(k, A, B) \
    _mm256_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_LT)
#define _mm256_cmpneq_epu64_mask(A, B) \
    _mm256_cmp_epu64_mask((A), (B), _MM_CMPINT_NE)
#define _mm256_mask_cmpneq_epu64_mask(k, A, B) \
    _mm256_mask_cmp_epu64_mask((k), (A), (B), _MM_CMPINT_NE)

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_add_epi32(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_add_epi32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_add_epi32(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_add_epi32(__A, __B),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_add_epi64(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                             (__v4di)_mm256_add_epi64(__A, __B),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_add_epi64(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                             (__v4di)_mm256_add_epi64(__A, __B),
                                             (__v4di)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_sub_epi32(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_sub_epi32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_sub_epi32(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_sub_epi32(__A, __B),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_sub_epi64(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                             (__v4di)_mm256_sub_epi64(__A, __B),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_sub_epi64(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                             (__v4di)_mm256_sub_epi64(__A, __B),
                                             (__v4di)_mm256_setzero_si256());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_add_epi32(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_add_epi32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_add_epi32(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_add_epi32(__A, __B),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_add_epi64(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_add_epi64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_add_epi64(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_add_epi64(__A, __B),
                                             (__v2di)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_sub_epi32(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_sub_epi32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_sub_epi32(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_sub_epi32(__A, __B),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_sub_epi64(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_sub_epi64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_sub_epi64(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_sub_epi64(__A, __B),
                                             (__v2di)_mm_setzero_si128());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_mul_epi32(__m256i __W, __mmask8 __M, __m256i __X, __m256i __Y)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_mul_epi32(__X, __Y),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_mul_epi32(__mmask8 __M, __m256i __X, __m256i __Y)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_mul_epi32(__X, __Y),
                                             (__v4di)_mm256_setzero_si256());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_mul_epi32(__m128i __W, __mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_mul_epi32(__X, __Y),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_mul_epi32(__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_mul_epi32(__X, __Y),
                                             (__v2di)_mm_setzero_si128());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_mul_epu32(__m256i __W, __mmask8 __M, __m256i __X, __m256i __Y)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_mul_epu32(__X, __Y),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_mul_epu32(__mmask8 __M, __m256i __X, __m256i __Y)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_mul_epu32(__X, __Y),
                                             (__v4di)_mm256_setzero_si256());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_mul_epu32(__m128i __W, __mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_mul_epu32(__X, __Y),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_mul_epu32(__mmask8 __M, __m128i __X, __m128i __Y)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_mul_epu32(__X, __Y),
                                             (__v2di)_mm_setzero_si128());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_mullo_epi32(__mmask8 __M, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_mullo_epi32(__A, __B),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_mullo_epi32(__m256i __W, __mmask8 __M, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_mullo_epi32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_mullo_epi32(__mmask8 __M, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_mullo_epi32(__A, __B),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_mullo_epi32(__m128i __W, __mmask8 __M, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_mullo_epi32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_and_epi32(__m256i __a, __m256i __b)
{
  return (__m256i)((__v8su)__a & (__v8su)__b);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_and_epi32(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_and_epi32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_and_epi32(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)_mm256_mask_and_epi32(_mm256_setzero_si256(), __U, __A, __B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_and_epi32(__m128i __a, __m128i __b)
{
  return (__m128i)((__v4su)__a & (__v4su)__b);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_and_epi32(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_and_epi32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_and_epi32(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)_mm_mask_and_epi32(_mm_setzero_si128(), __U, __A, __B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_andnot_epi32(__m256i __A, __m256i __B)
{
  return (__m256i)(~(__v8su)__A & (__v8su)__B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_andnot_epi32(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                          (__v8si)_mm256_andnot_epi32(__A, __B),
                                          (__v8si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_andnot_epi32(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)_mm256_mask_andnot_epi32(_mm256_setzero_si256(),
                                           __U, __A, __B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_andnot_epi32(__m128i __A, __m128i __B)
{
  return (__m128i)(~(__v4su)__A & (__v4su)__B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_andnot_epi32(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_andnot_epi32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_andnot_epi32(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)_mm_mask_andnot_epi32(_mm_setzero_si128(), __U, __A, __B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_or_epi32(__m256i __a, __m256i __b)
{
  return (__m256i)((__v8su)__a | (__v8su)__b);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_or_epi32 (__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_or_epi32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_or_epi32(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)_mm256_mask_or_epi32(_mm256_setzero_si256(), __U, __A, __B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_or_epi32(__m128i __a, __m128i __b)
{
  return (__m128i)((__v4su)__a | (__v4su)__b);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_or_epi32(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_or_epi32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_or_epi32(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)_mm_mask_or_epi32(_mm_setzero_si128(), __U, __A, __B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_xor_epi32(__m256i __a, __m256i __b)
{
  return (__m256i)((__v8su)__a ^ (__v8su)__b);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_xor_epi32(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_xor_epi32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_xor_epi32(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)_mm256_mask_xor_epi32(_mm256_setzero_si256(), __U, __A, __B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_xor_epi32(__m128i __a, __m128i __b)
{
  return (__m128i)((__v4su)__a ^ (__v4su)__b);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_xor_epi32(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_xor_epi32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_xor_epi32(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)_mm_mask_xor_epi32(_mm_setzero_si128(), __U, __A, __B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_and_epi64(__m256i __a, __m256i __b)
{
  return (__m256i)((__v4du)__a & (__v4du)__b);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_and_epi64(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                             (__v4di)_mm256_and_epi64(__A, __B),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_and_epi64(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)_mm256_mask_and_epi64(_mm256_setzero_si256(), __U, __A, __B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_and_epi64(__m128i __a, __m128i __b)
{
  return (__m128i)((__v2du)__a & (__v2du)__b);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_and_epi64(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_and_epi64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_and_epi64(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)_mm_mask_and_epi64(_mm_setzero_si128(), __U, __A, __B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_andnot_epi64(__m256i __A, __m256i __B)
{
  return (__m256i)(~(__v4du)__A & (__v4du)__B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_andnot_epi64(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                          (__v4di)_mm256_andnot_epi64(__A, __B),
                                          (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_andnot_epi64(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)_mm256_mask_andnot_epi64(_mm256_setzero_si256(),
                                           __U, __A, __B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_andnot_epi64(__m128i __A, __m128i __B)
{
  return (__m128i)(~(__v2du)__A & (__v2du)__B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_andnot_epi64(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_andnot_epi64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_andnot_epi64(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)_mm_mask_andnot_epi64(_mm_setzero_si128(), __U, __A, __B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_or_epi64(__m256i __a, __m256i __b)
{
  return (__m256i)((__v4du)__a | (__v4du)__b);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_or_epi64(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                             (__v4di)_mm256_or_epi64(__A, __B),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_or_epi64(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)_mm256_mask_or_epi64(_mm256_setzero_si256(), __U, __A, __B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_or_epi64(__m128i __a, __m128i __b)
{
  return (__m128i)((__v2du)__a | (__v2du)__b);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_or_epi64(__m128i __W, __mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_or_epi64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_or_epi64(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)_mm_mask_or_epi64(_mm_setzero_si128(), __U, __A, __B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_xor_epi64(__m256i __a, __m256i __b)
{
  return (__m256i)((__v4du)__a ^ (__v4du)__b);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_xor_epi64(__m256i __W, __mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                             (__v4di)_mm256_xor_epi64(__A, __B),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_xor_epi64(__mmask8 __U, __m256i __A, __m256i __B)
{
  return (__m256i)_mm256_mask_xor_epi64(_mm256_setzero_si256(), __U, __A, __B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_xor_epi64(__m128i __a, __m128i __b)
{
  return (__m128i)((__v2du)__a ^ (__v2du)__b);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_xor_epi64(__m128i __W, __mmask8 __U, __m128i __A,
        __m128i __B)
{
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_xor_epi64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_xor_epi64(__mmask8 __U, __m128i __A, __m128i __B)
{
  return (__m128i)_mm_mask_xor_epi64(_mm_setzero_si128(), __U, __A, __B);
}

#define _mm_cmp_epi32_mask(a, b, p) \
  ((__mmask8)__builtin_ia32_cmpd128_mask((__v4si)(__m128i)(a), \
                                         (__v4si)(__m128i)(b), (int)(p), \
                                         (__mmask8)-1))

#define _mm_mask_cmp_epi32_mask(m, a, b, p) \
  ((__mmask8)__builtin_ia32_cmpd128_mask((__v4si)(__m128i)(a), \
                                         (__v4si)(__m128i)(b), (int)(p), \
                                         (__mmask8)(m)))

#define _mm_cmp_epu32_mask(a, b, p) \
  ((__mmask8)__builtin_ia32_ucmpd128_mask((__v4si)(__m128i)(a), \
                                          (__v4si)(__m128i)(b), (int)(p), \
                                          (__mmask8)-1))

#define _mm_mask_cmp_epu32_mask(m, a, b, p) \
  ((__mmask8)__builtin_ia32_ucmpd128_mask((__v4si)(__m128i)(a), \
                                          (__v4si)(__m128i)(b), (int)(p), \
                                          (__mmask8)(m)))

#define _mm256_cmp_epi32_mask(a, b, p) \
  ((__mmask8)__builtin_ia32_cmpd256_mask((__v8si)(__m256i)(a), \
                                         (__v8si)(__m256i)(b), (int)(p), \
                                         (__mmask8)-1))

#define _mm256_mask_cmp_epi32_mask(m, a, b, p) \
  ((__mmask8)__builtin_ia32_cmpd256_mask((__v8si)(__m256i)(a), \
                                         (__v8si)(__m256i)(b), (int)(p), \
                                         (__mmask8)(m)))

#define _mm256_cmp_epu32_mask(a, b, p) \
  ((__mmask8)__builtin_ia32_ucmpd256_mask((__v8si)(__m256i)(a), \
                                          (__v8si)(__m256i)(b), (int)(p), \
                                          (__mmask8)-1))

#define _mm256_mask_cmp_epu32_mask(m, a, b, p) \
  ((__mmask8)__builtin_ia32_ucmpd256_mask((__v8si)(__m256i)(a), \
                                          (__v8si)(__m256i)(b), (int)(p), \
                                          (__mmask8)(m)))

#define _mm_cmp_epi64_mask(a, b, p) \
  ((__mmask8)__builtin_ia32_cmpq128_mask((__v2di)(__m128i)(a), \
                                         (__v2di)(__m128i)(b), (int)(p), \
                                         (__mmask8)-1))

#define _mm_mask_cmp_epi64_mask(m, a, b, p) \
  ((__mmask8)__builtin_ia32_cmpq128_mask((__v2di)(__m128i)(a), \
                                         (__v2di)(__m128i)(b), (int)(p), \
                                         (__mmask8)(m)))

#define _mm_cmp_epu64_mask(a, b, p) \
  ((__mmask8)__builtin_ia32_ucmpq128_mask((__v2di)(__m128i)(a), \
                                          (__v2di)(__m128i)(b), (int)(p), \
                                          (__mmask8)-1))

#define _mm_mask_cmp_epu64_mask(m, a, b, p) \
  ((__mmask8)__builtin_ia32_ucmpq128_mask((__v2di)(__m128i)(a), \
                                          (__v2di)(__m128i)(b), (int)(p), \
                                          (__mmask8)(m)))

#define _mm256_cmp_epi64_mask(a, b, p) \
  ((__mmask8)__builtin_ia32_cmpq256_mask((__v4di)(__m256i)(a), \
                                         (__v4di)(__m256i)(b), (int)(p), \
                                         (__mmask8)-1))

#define _mm256_mask_cmp_epi64_mask(m, a, b, p) \
  ((__mmask8)__builtin_ia32_cmpq256_mask((__v4di)(__m256i)(a), \
                                         (__v4di)(__m256i)(b), (int)(p), \
                                         (__mmask8)(m)))

#define _mm256_cmp_epu64_mask(a, b, p) \
  ((__mmask8)__builtin_ia32_ucmpq256_mask((__v4di)(__m256i)(a), \
                                          (__v4di)(__m256i)(b), (int)(p), \
                                          (__mmask8)-1))

#define _mm256_mask_cmp_epu64_mask(m, a, b, p) \
  ((__mmask8)__builtin_ia32_ucmpq256_mask((__v4di)(__m256i)(a), \
                                          (__v4di)(__m256i)(b), (int)(p), \
                                          (__mmask8)(m)))

#define _mm256_cmp_ps_mask(a, b, p)  \
  ((__mmask8)__builtin_ia32_cmpps256_mask((__v8sf)(__m256)(a), \
                                          (__v8sf)(__m256)(b), (int)(p), \
                                          (__mmask8)-1))

#define _mm256_mask_cmp_ps_mask(m, a, b, p)  \
  ((__mmask8)__builtin_ia32_cmpps256_mask((__v8sf)(__m256)(a), \
                                          (__v8sf)(__m256)(b), (int)(p), \
                                          (__mmask8)(m)))

#define _mm256_cmp_pd_mask(a, b, p)  \
  ((__mmask8)__builtin_ia32_cmppd256_mask((__v4df)(__m256d)(a), \
                                          (__v4df)(__m256d)(b), (int)(p), \
                                          (__mmask8)-1))

#define _mm256_mask_cmp_pd_mask(m, a, b, p)  \
  ((__mmask8)__builtin_ia32_cmppd256_mask((__v4df)(__m256d)(a), \
                                          (__v4df)(__m256d)(b), (int)(p), \
                                          (__mmask8)(m)))

#define _mm_cmp_ps_mask(a, b, p)  \
  ((__mmask8)__builtin_ia32_cmpps128_mask((__v4sf)(__m128)(a), \
                                          (__v4sf)(__m128)(b), (int)(p), \
                                          (__mmask8)-1))

#define _mm_mask_cmp_ps_mask(m, a, b, p)  \
  ((__mmask8)__builtin_ia32_cmpps128_mask((__v4sf)(__m128)(a), \
                                          (__v4sf)(__m128)(b), (int)(p), \
                                          (__mmask8)(m)))

#define _mm_cmp_pd_mask(a, b, p)  \
  ((__mmask8)__builtin_ia32_cmppd128_mask((__v2df)(__m128d)(a), \
                                          (__v2df)(__m128d)(b), (int)(p), \
                                          (__mmask8)-1))

#define _mm_mask_cmp_pd_mask(m, a, b, p)  \
  ((__mmask8)__builtin_ia32_cmppd128_mask((__v2df)(__m128d)(a), \
                                          (__v2df)(__m128d)(b), (int)(p), \
                                          (__mmask8)(m)))

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_fmadd_pd(__m128d __A, __mmask8 __U, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd ((__v2df) __A,
                                             (__v2df) __B,
                                             (__v2df) __C),
                    (__v2df) __A);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask3_fmadd_pd(__m128d __A, __m128d __B, __m128d __C, __mmask8 __U)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd ((__v2df) __A,
                                             (__v2df) __B,
                                             (__v2df) __C),
                    (__v2df) __C);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_fmadd_pd(__mmask8 __U, __m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd ((__v2df) __A,
                                             (__v2df) __B,
                                             (__v2df) __C),
                    (__v2df)_mm_setzero_pd());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_fmsub_pd(__m128d __A, __mmask8 __U, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd ((__v2df) __A,
                                             (__v2df) __B,
                                             -(__v2df) __C),
                    (__v2df) __A);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_fmsub_pd(__mmask8 __U, __m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd ((__v2df) __A,
                                             (__v2df) __B,
                                             -(__v2df) __C),
                    (__v2df)_mm_setzero_pd());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask3_fnmadd_pd(__m128d __A, __m128d __B, __m128d __C, __mmask8 __U)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd (-(__v2df) __A,
                                             (__v2df) __B,
                                             (__v2df) __C),
                    (__v2df) __C);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_fnmadd_pd(__mmask8 __U, __m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd (-(__v2df) __A,
                                             (__v2df) __B,
                                             (__v2df) __C),
                    (__v2df)_mm_setzero_pd());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_fnmsub_pd(__mmask8 __U, __m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd (-(__v2df) __A,
                                             (__v2df) __B,
                                             -(__v2df) __C),
                    (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_fmadd_pd(__m256d __A, __mmask8 __U, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 ((__v4df) __A,
                                                (__v4df) __B,
                                                (__v4df) __C),
                    (__v4df) __A);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask3_fmadd_pd(__m256d __A, __m256d __B, __m256d __C, __mmask8 __U)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 ((__v4df) __A,
                                                (__v4df) __B,
                                                (__v4df) __C),
                    (__v4df) __C);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_fmadd_pd(__mmask8 __U, __m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 ((__v4df) __A,
                                                (__v4df) __B,
                                                (__v4df) __C),
                    (__v4df)_mm256_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_fmsub_pd(__m256d __A, __mmask8 __U, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 ((__v4df) __A,
                                                (__v4df) __B,
                                                -(__v4df) __C),
                    (__v4df) __A);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_fmsub_pd(__mmask8 __U, __m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 ((__v4df) __A,
                                                (__v4df) __B,
                                                -(__v4df) __C),
                    (__v4df)_mm256_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask3_fnmadd_pd(__m256d __A, __m256d __B, __m256d __C, __mmask8 __U)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 (-(__v4df) __A,
                                                (__v4df) __B,
                                                (__v4df) __C),
                    (__v4df) __C);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_fnmadd_pd(__mmask8 __U, __m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 (-(__v4df) __A,
                                                (__v4df) __B,
                                                (__v4df) __C),
                    (__v4df)_mm256_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_fnmsub_pd(__mmask8 __U, __m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 (-(__v4df) __A,
                                                (__v4df) __B,
                                                -(__v4df) __C),
                    (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_fmadd_ps(__m128 __A, __mmask8 __U, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps ((__v4sf) __A,
                                             (__v4sf) __B,
                                             (__v4sf) __C),
                    (__v4sf) __A);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask3_fmadd_ps(__m128 __A, __m128 __B, __m128 __C, __mmask8 __U)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps ((__v4sf) __A,
                                             (__v4sf) __B,
                                             (__v4sf) __C),
                    (__v4sf) __C);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_fmadd_ps(__mmask8 __U, __m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps ((__v4sf) __A,
                                             (__v4sf) __B,
                                             (__v4sf) __C),
                    (__v4sf)_mm_setzero_ps());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_fmsub_ps(__m128 __A, __mmask8 __U, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps ((__v4sf) __A,
                                             (__v4sf) __B,
                                             -(__v4sf) __C),
                    (__v4sf) __A);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_fmsub_ps(__mmask8 __U, __m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps ((__v4sf) __A,
                                             (__v4sf) __B,
                                             -(__v4sf) __C),
                    (__v4sf)_mm_setzero_ps());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask3_fnmadd_ps(__m128 __A, __m128 __B, __m128 __C, __mmask8 __U)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps (-(__v4sf) __A,
                                             (__v4sf) __B,
                                             (__v4sf) __C),
                    (__v4sf) __C);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_fnmadd_ps(__mmask8 __U, __m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps (-(__v4sf) __A,
                                             (__v4sf) __B,
                                             (__v4sf) __C),
                    (__v4sf)_mm_setzero_ps());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_fnmsub_ps(__mmask8 __U, __m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps (-(__v4sf) __A,
                                             (__v4sf) __B,
                                             -(__v4sf) __C),
                    (__v4sf)_mm_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_fmadd_ps(__m256 __A, __mmask8 __U, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 ((__v8sf) __A,
                                                (__v8sf) __B,
                                                (__v8sf) __C),
                    (__v8sf) __A);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask3_fmadd_ps(__m256 __A, __m256 __B, __m256 __C, __mmask8 __U)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 ((__v8sf) __A,
                                                (__v8sf) __B,
                                                (__v8sf) __C),
                    (__v8sf) __C);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_fmadd_ps(__mmask8 __U, __m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 ((__v8sf) __A,
                                                (__v8sf) __B,
                                                (__v8sf) __C),
                    (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_fmsub_ps(__m256 __A, __mmask8 __U, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 ((__v8sf) __A,
                                                (__v8sf) __B,
                                                -(__v8sf) __C),
                    (__v8sf) __A);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_fmsub_ps(__mmask8 __U, __m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 ((__v8sf) __A,
                                                (__v8sf) __B,
                                                -(__v8sf) __C),
                    (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask3_fnmadd_ps(__m256 __A, __m256 __B, __m256 __C, __mmask8 __U)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 (-(__v8sf) __A,
                                                (__v8sf) __B,
                                                (__v8sf) __C),
                    (__v8sf) __C);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_fnmadd_ps(__mmask8 __U, __m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 (-(__v8sf) __A,
                                                (__v8sf) __B,
                                                (__v8sf) __C),
                    (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_fnmsub_ps(__mmask8 __U, __m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 (-(__v8sf) __A,
                                                (__v8sf) __B,
                                                -(__v8sf) __C),
                    (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_fmaddsub_pd(__m128d __A, __mmask8 __U, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd ((__v2df) __A,
                                                (__v2df) __B,
                                                (__v2df) __C),
                    (__v2df) __A);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask3_fmaddsub_pd(__m128d __A, __m128d __B, __m128d __C, __mmask8 __U)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd ((__v2df) __A,
                                                (__v2df) __B,
                                                (__v2df) __C),
                    (__v2df) __C);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_fmaddsub_pd(__mmask8 __U, __m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd ((__v2df) __A,
                                                (__v2df) __B,
                                                (__v2df) __C),
                    (__v2df)_mm_setzero_pd());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_fmsubadd_pd(__m128d __A, __mmask8 __U, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd ((__v2df) __A,
                                                (__v2df) __B,
                                                -(__v2df) __C),
                    (__v2df) __A);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_fmsubadd_pd(__mmask8 __U, __m128d __A, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd ((__v2df) __A,
                                                (__v2df) __B,
                                                -(__v2df) __C),
                    (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_fmaddsub_pd(__m256d __A, __mmask8 __U, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd256 ((__v4df) __A,
                                                   (__v4df) __B,
                                                   (__v4df) __C),
                    (__v4df) __A);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask3_fmaddsub_pd(__m256d __A, __m256d __B, __m256d __C, __mmask8 __U)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd256 ((__v4df) __A,
                                                   (__v4df) __B,
                                                   (__v4df) __C),
                    (__v4df) __C);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_fmaddsub_pd(__mmask8 __U, __m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd256 ((__v4df) __A,
                                                   (__v4df) __B,
                                                   (__v4df) __C),
                    (__v4df)_mm256_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_fmsubadd_pd(__m256d __A, __mmask8 __U, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd256 ((__v4df) __A,
                                                   (__v4df) __B,
                                                   -(__v4df) __C),
                    (__v4df) __A);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_fmsubadd_pd(__mmask8 __U, __m256d __A, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd256 ((__v4df) __A,
                                                   (__v4df) __B,
                                                   -(__v4df) __C),
                    (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_fmaddsub_ps(__m128 __A, __mmask8 __U, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps ((__v4sf) __A,
                                                (__v4sf) __B,
                                                (__v4sf) __C),
                    (__v4sf) __A);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask3_fmaddsub_ps(__m128 __A, __m128 __B, __m128 __C, __mmask8 __U)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps ((__v4sf) __A,
                                                (__v4sf) __B,
                                                (__v4sf) __C),
                    (__v4sf) __C);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_fmaddsub_ps(__mmask8 __U, __m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps ((__v4sf) __A,
                                                (__v4sf) __B,
                                                (__v4sf) __C),
                    (__v4sf)_mm_setzero_ps());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_fmsubadd_ps(__m128 __A, __mmask8 __U, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps ((__v4sf) __A,
                                                (__v4sf) __B,
                                                -(__v4sf) __C),
                    (__v4sf) __A);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_fmsubadd_ps(__mmask8 __U, __m128 __A, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps ((__v4sf) __A,
                                                (__v4sf) __B,
                                                -(__v4sf) __C),
                    (__v4sf)_mm_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_fmaddsub_ps(__m256 __A, __mmask8 __U, __m256 __B,
                         __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps256 ((__v8sf) __A,
                                                   (__v8sf) __B,
                                                   (__v8sf) __C),
                    (__v8sf) __A);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask3_fmaddsub_ps(__m256 __A, __m256 __B, __m256 __C, __mmask8 __U)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps256 ((__v8sf) __A,
                                                   (__v8sf) __B,
                                                   (__v8sf) __C),
                    (__v8sf) __C);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_fmaddsub_ps(__mmask8 __U, __m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps256 ((__v8sf) __A,
                                                   (__v8sf) __B,
                                                   (__v8sf) __C),
                    (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_fmsubadd_ps(__m256 __A, __mmask8 __U, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps256 ((__v8sf) __A,
                                                   (__v8sf) __B,
                                                   -(__v8sf) __C),
                    (__v8sf) __A);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_fmsubadd_ps(__mmask8 __U, __m256 __A, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps256 ((__v8sf) __A,
                                                   (__v8sf) __B,
                                                   -(__v8sf) __C),
                    (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask3_fmsub_pd(__m128d __A, __m128d __B, __m128d __C, __mmask8 __U)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd ((__v2df) __A,
                                             (__v2df) __B,
                                             -(__v2df) __C),
                    (__v2df) __C);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask3_fmsub_pd(__m256d __A, __m256d __B, __m256d __C, __mmask8 __U)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 ((__v4df) __A,
                                                (__v4df) __B,
                                                -(__v4df) __C),
                    (__v4df) __C);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask3_fmsub_ps(__m128 __A, __m128 __B, __m128 __C, __mmask8 __U)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps ((__v4sf) __A,
                                             (__v4sf) __B,
                                             -(__v4sf) __C),
                    (__v4sf) __C);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask3_fmsub_ps(__m256 __A, __m256 __B, __m256 __C, __mmask8 __U)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 ((__v8sf) __A,
                                                (__v8sf) __B,
                                                -(__v8sf) __C),
                    (__v8sf) __C);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask3_fmsubadd_pd(__m128d __A, __m128d __B, __m128d __C, __mmask8 __U)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd ((__v2df) __A,
                                                (__v2df) __B,
                                                -(__v2df) __C),
                    (__v2df) __C);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask3_fmsubadd_pd(__m256d __A, __m256d __B, __m256d __C, __mmask8 __U)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubpd256 ((__v4df) __A,
                                                   (__v4df) __B,
                                                   -(__v4df) __C),
                    (__v4df) __C);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask3_fmsubadd_ps(__m128 __A, __m128 __B, __m128 __C, __mmask8 __U)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps ((__v4sf) __A,
                                                (__v4sf) __B,
                                                -(__v4sf) __C),
                    (__v4sf) __C);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask3_fmsubadd_ps(__m256 __A, __m256 __B, __m256 __C, __mmask8 __U)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddsubps256 ((__v8sf) __A,
                                                   (__v8sf) __B,
                                                   -(__v8sf) __C),
                    (__v8sf) __C);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_fnmadd_pd(__m128d __A, __mmask8 __U, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd ((__v2df) __A,
                                             -(__v2df) __B,
                                             (__v2df) __C),
                    (__v2df) __A);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_fnmadd_pd(__m256d __A, __mmask8 __U, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 ((__v4df) __A,
                                                -(__v4df) __B,
                                                (__v4df) __C),
                    (__v4df) __A);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_fnmadd_ps(__m128 __A, __mmask8 __U, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps ((__v4sf) __A,
                                             -(__v4sf) __B,
                                             (__v4sf) __C),
                    (__v4sf) __A);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_fnmadd_ps(__m256 __A, __mmask8 __U, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 ((__v8sf) __A,
                                                -(__v8sf) __B,
                                                (__v8sf) __C),
                    (__v8sf) __A);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_fnmsub_pd(__m128d __A, __mmask8 __U, __m128d __B, __m128d __C)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd ((__v2df) __A,
                                             -(__v2df) __B,
                                             -(__v2df) __C),
                    (__v2df) __A);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask3_fnmsub_pd(__m128d __A, __m128d __B, __m128d __C, __mmask8 __U)
{
  return (__m128d) __builtin_ia32_selectpd_128((__mmask8) __U,
                    __builtin_ia32_vfmaddpd ((__v2df) __A,
                                             -(__v2df) __B,
                                             -(__v2df) __C),
                    (__v2df) __C);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_fnmsub_pd(__m256d __A, __mmask8 __U, __m256d __B, __m256d __C)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 ((__v4df) __A,
                                                -(__v4df) __B,
                                                -(__v4df) __C),
                    (__v4df) __A);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask3_fnmsub_pd(__m256d __A, __m256d __B, __m256d __C, __mmask8 __U)
{
  return (__m256d) __builtin_ia32_selectpd_256((__mmask8) __U,
                    __builtin_ia32_vfmaddpd256 ((__v4df) __A,
                                                -(__v4df) __B,
                                                -(__v4df) __C),
                    (__v4df) __C);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_fnmsub_ps(__m128 __A, __mmask8 __U, __m128 __B, __m128 __C)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps ((__v4sf) __A,
                                             -(__v4sf) __B,
                                             -(__v4sf) __C),
                    (__v4sf) __A);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask3_fnmsub_ps(__m128 __A, __m128 __B, __m128 __C, __mmask8 __U)
{
  return (__m128) __builtin_ia32_selectps_128((__mmask8) __U,
                    __builtin_ia32_vfmaddps ((__v4sf) __A,
                                             -(__v4sf) __B,
                                             -(__v4sf) __C),
                    (__v4sf) __C);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_fnmsub_ps(__m256 __A, __mmask8 __U, __m256 __B, __m256 __C)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 ((__v8sf) __A,
                                                -(__v8sf) __B,
                                                -(__v8sf) __C),
                    (__v8sf) __A);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask3_fnmsub_ps(__m256 __A, __m256 __B, __m256 __C, __mmask8 __U)
{
  return (__m256) __builtin_ia32_selectps_256((__mmask8) __U,
                    __builtin_ia32_vfmaddps256 ((__v8sf) __A,
                                                -(__v8sf) __B,
                                                -(__v8sf) __C),
                    (__v8sf) __C);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_add_pd(__m128d __W, __mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_add_pd(__A, __B),
                                              (__v2df)__W);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_add_pd(__mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_add_pd(__A, __B),
                                              (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_add_pd(__m256d __W, __mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_add_pd(__A, __B),
                                              (__v4df)__W);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_add_pd(__mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_add_pd(__A, __B),
                                              (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_add_ps(__m128 __W, __mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_add_ps(__A, __B),
                                             (__v4sf)__W);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_add_ps(__mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_add_ps(__A, __B),
                                             (__v4sf)_mm_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_add_ps(__m256 __W, __mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_add_ps(__A, __B),
                                             (__v8sf)__W);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_add_ps(__mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_add_ps(__A, __B),
                                             (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_blend_epi32 (__mmask8 __U, __m128i __A, __m128i __W) {
  return (__m128i) __builtin_ia32_selectd_128 ((__mmask8) __U,
                (__v4si) __W,
                (__v4si) __A);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_blend_epi32 (__mmask8 __U, __m256i __A, __m256i __W) {
  return (__m256i) __builtin_ia32_selectd_256 ((__mmask8) __U,
                (__v8si) __W,
                (__v8si) __A);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_blend_pd (__mmask8 __U, __m128d __A, __m128d __W) {
  return (__m128d) __builtin_ia32_selectpd_128 ((__mmask8) __U,
                 (__v2df) __W,
                 (__v2df) __A);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_blend_pd (__mmask8 __U, __m256d __A, __m256d __W) {
  return (__m256d) __builtin_ia32_selectpd_256 ((__mmask8) __U,
                 (__v4df) __W,
                 (__v4df) __A);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_blend_ps (__mmask8 __U, __m128 __A, __m128 __W) {
  return (__m128) __builtin_ia32_selectps_128 ((__mmask8) __U,
                (__v4sf) __W,
                (__v4sf) __A);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_blend_ps (__mmask8 __U, __m256 __A, __m256 __W) {
  return (__m256) __builtin_ia32_selectps_256 ((__mmask8) __U,
                (__v8sf) __W,
                (__v8sf) __A);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_blend_epi64 (__mmask8 __U, __m128i __A, __m128i __W) {
  return (__m128i) __builtin_ia32_selectq_128 ((__mmask8) __U,
                (__v2di) __W,
                (__v2di) __A);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_blend_epi64 (__mmask8 __U, __m256i __A, __m256i __W) {
  return (__m256i) __builtin_ia32_selectq_256 ((__mmask8) __U,
                (__v4di) __W,
                (__v4di) __A);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_compress_pd (__m128d __W, __mmask8 __U, __m128d __A) {
  return (__m128d) __builtin_ia32_compressdf128_mask ((__v2df) __A,
                  (__v2df) __W,
                  (__mmask8) __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_compress_pd (__mmask8 __U, __m128d __A) {
  return (__m128d) __builtin_ia32_compressdf128_mask ((__v2df) __A,
                  (__v2df)
                  _mm_setzero_pd (),
                  (__mmask8) __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_compress_pd (__m256d __W, __mmask8 __U, __m256d __A) {
  return (__m256d) __builtin_ia32_compressdf256_mask ((__v4df) __A,
                  (__v4df) __W,
                  (__mmask8) __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_compress_pd (__mmask8 __U, __m256d __A) {
  return (__m256d) __builtin_ia32_compressdf256_mask ((__v4df) __A,
                  (__v4df)
                  _mm256_setzero_pd (),
                  (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_compress_epi64 (__m128i __W, __mmask8 __U, __m128i __A) {
  return (__m128i) __builtin_ia32_compressdi128_mask ((__v2di) __A,
                  (__v2di) __W,
                  (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_compress_epi64 (__mmask8 __U, __m128i __A) {
  return (__m128i) __builtin_ia32_compressdi128_mask ((__v2di) __A,
                  (__v2di)
                  _mm_setzero_si128 (),
                  (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_compress_epi64 (__m256i __W, __mmask8 __U, __m256i __A) {
  return (__m256i) __builtin_ia32_compressdi256_mask ((__v4di) __A,
                  (__v4di) __W,
                  (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_compress_epi64 (__mmask8 __U, __m256i __A) {
  return (__m256i) __builtin_ia32_compressdi256_mask ((__v4di) __A,
                  (__v4di)
                  _mm256_setzero_si256 (),
                  (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_compress_ps (__m128 __W, __mmask8 __U, __m128 __A) {
  return (__m128) __builtin_ia32_compresssf128_mask ((__v4sf) __A,
                 (__v4sf) __W,
                 (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_compress_ps (__mmask8 __U, __m128 __A) {
  return (__m128) __builtin_ia32_compresssf128_mask ((__v4sf) __A,
                 (__v4sf)
                 _mm_setzero_ps (),
                 (__mmask8) __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_compress_ps (__m256 __W, __mmask8 __U, __m256 __A) {
  return (__m256) __builtin_ia32_compresssf256_mask ((__v8sf) __A,
                 (__v8sf) __W,
                 (__mmask8) __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_compress_ps (__mmask8 __U, __m256 __A) {
  return (__m256) __builtin_ia32_compresssf256_mask ((__v8sf) __A,
                 (__v8sf)
                 _mm256_setzero_ps (),
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_compress_epi32 (__m128i __W, __mmask8 __U, __m128i __A) {
  return (__m128i) __builtin_ia32_compresssi128_mask ((__v4si) __A,
                  (__v4si) __W,
                  (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_compress_epi32 (__mmask8 __U, __m128i __A) {
  return (__m128i) __builtin_ia32_compresssi128_mask ((__v4si) __A,
                  (__v4si)
                  _mm_setzero_si128 (),
                  (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_compress_epi32 (__m256i __W, __mmask8 __U, __m256i __A) {
  return (__m256i) __builtin_ia32_compresssi256_mask ((__v8si) __A,
                  (__v8si) __W,
                  (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_compress_epi32 (__mmask8 __U, __m256i __A) {
  return (__m256i) __builtin_ia32_compresssi256_mask ((__v8si) __A,
                  (__v8si)
                  _mm256_setzero_si256 (),
                  (__mmask8) __U);
}

static __inline__ void __DEFAULT_FN_ATTRS128
_mm_mask_compressstoreu_pd (void *__P, __mmask8 __U, __m128d __A) {
  __builtin_ia32_compressstoredf128_mask ((__v2df *) __P,
            (__v2df) __A,
            (__mmask8) __U);
}

static __inline__ void __DEFAULT_FN_ATTRS256
_mm256_mask_compressstoreu_pd (void *__P, __mmask8 __U, __m256d __A) {
  __builtin_ia32_compressstoredf256_mask ((__v4df *) __P,
            (__v4df) __A,
            (__mmask8) __U);
}

static __inline__ void __DEFAULT_FN_ATTRS128
_mm_mask_compressstoreu_epi64 (void *__P, __mmask8 __U, __m128i __A) {
  __builtin_ia32_compressstoredi128_mask ((__v2di *) __P,
            (__v2di) __A,
            (__mmask8) __U);
}

static __inline__ void __DEFAULT_FN_ATTRS256
_mm256_mask_compressstoreu_epi64 (void *__P, __mmask8 __U, __m256i __A) {
  __builtin_ia32_compressstoredi256_mask ((__v4di *) __P,
            (__v4di) __A,
            (__mmask8) __U);
}

static __inline__ void __DEFAULT_FN_ATTRS128
_mm_mask_compressstoreu_ps (void *__P, __mmask8 __U, __m128 __A) {
  __builtin_ia32_compressstoresf128_mask ((__v4sf *) __P,
            (__v4sf) __A,
            (__mmask8) __U);
}

static __inline__ void __DEFAULT_FN_ATTRS256
_mm256_mask_compressstoreu_ps (void *__P, __mmask8 __U, __m256 __A) {
  __builtin_ia32_compressstoresf256_mask ((__v8sf *) __P,
            (__v8sf) __A,
            (__mmask8) __U);
}

static __inline__ void __DEFAULT_FN_ATTRS128
_mm_mask_compressstoreu_epi32 (void *__P, __mmask8 __U, __m128i __A) {
  __builtin_ia32_compressstoresi128_mask ((__v4si *) __P,
            (__v4si) __A,
            (__mmask8) __U);
}

static __inline__ void __DEFAULT_FN_ATTRS256
_mm256_mask_compressstoreu_epi32 (void *__P, __mmask8 __U, __m256i __A) {
  __builtin_ia32_compressstoresi256_mask ((__v8si *) __P,
            (__v8si) __A,
            (__mmask8) __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_cvtepi32_pd (__m128d __W, __mmask8 __U, __m128i __A) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8) __U,
                                              (__v2df)_mm_cvtepi32_pd(__A),
                                              (__v2df)__W);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_cvtepi32_pd (__mmask8 __U, __m128i __A) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8) __U,
                                              (__v2df)_mm_cvtepi32_pd(__A),
                                              (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_cvtepi32_pd (__m256d __W, __mmask8 __U, __m128i __A) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8) __U,
                                              (__v4df)_mm256_cvtepi32_pd(__A),
                                              (__v4df)__W);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtepi32_pd (__mmask8 __U, __m128i __A) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8) __U,
                                              (__v4df)_mm256_cvtepi32_pd(__A),
                                              (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_cvtepi32_ps (__m128 __W, __mmask8 __U, __m128i __A) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_cvtepi32_ps(__A),
                                             (__v4sf)__W);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_cvtepi32_ps (__mmask8 __U, __m128i __A) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_cvtepi32_ps(__A),
                                             (__v4sf)_mm_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_cvtepi32_ps (__m256 __W, __mmask8 __U, __m256i __A) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_cvtepi32_ps(__A),
                                             (__v8sf)__W);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtepi32_ps (__mmask8 __U, __m256i __A) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_cvtepi32_ps(__A),
                                             (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_cvtpd_epi32 (__m128i __W, __mmask8 __U, __m128d __A) {
  return (__m128i) __builtin_ia32_cvtpd2dq128_mask ((__v2df) __A,
                (__v4si) __W,
                (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_cvtpd_epi32 (__mmask8 __U, __m128d __A) {
  return (__m128i) __builtin_ia32_cvtpd2dq128_mask ((__v2df) __A,
                (__v4si)
                _mm_setzero_si128 (),
                (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_mask_cvtpd_epi32 (__m128i __W, __mmask8 __U, __m256d __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm256_cvtpd_epi32(__A),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtpd_epi32 (__mmask8 __U, __m256d __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm256_cvtpd_epi32(__A),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_cvtpd_ps (__m128 __W, __mmask8 __U, __m128d __A) {
  return (__m128) __builtin_ia32_cvtpd2ps_mask ((__v2df) __A,
            (__v4sf) __W,
            (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_cvtpd_ps (__mmask8 __U, __m128d __A) {
  return (__m128) __builtin_ia32_cvtpd2ps_mask ((__v2df) __A,
            (__v4sf)
            _mm_setzero_ps (),
            (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS256
_mm256_mask_cvtpd_ps (__m128 __W, __mmask8 __U, __m256d __A) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm256_cvtpd_ps(__A),
                                             (__v4sf)__W);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtpd_ps (__mmask8 __U, __m256d __A) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm256_cvtpd_ps(__A),
                                             (__v4sf)_mm_setzero_ps());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_cvtpd_epu32 (__m128d __A) {
  return (__m128i) __builtin_ia32_cvtpd2udq128_mask ((__v2df) __A,
                 (__v4si)
                 _mm_setzero_si128 (),
                 (__mmask8) -1);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_cvtpd_epu32 (__m128i __W, __mmask8 __U, __m128d __A) {
  return (__m128i) __builtin_ia32_cvtpd2udq128_mask ((__v2df) __A,
                 (__v4si) __W,
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_cvtpd_epu32 (__mmask8 __U, __m128d __A) {
  return (__m128i) __builtin_ia32_cvtpd2udq128_mask ((__v2df) __A,
                 (__v4si)
                 _mm_setzero_si128 (),
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_cvtpd_epu32 (__m256d __A) {
  return (__m128i) __builtin_ia32_cvtpd2udq256_mask ((__v4df) __A,
                 (__v4si)
                 _mm_setzero_si128 (),
                 (__mmask8) -1);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_mask_cvtpd_epu32 (__m128i __W, __mmask8 __U, __m256d __A) {
  return (__m128i) __builtin_ia32_cvtpd2udq256_mask ((__v4df) __A,
                 (__v4si) __W,
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtpd_epu32 (__mmask8 __U, __m256d __A) {
  return (__m128i) __builtin_ia32_cvtpd2udq256_mask ((__v4df) __A,
                 (__v4si)
                 _mm_setzero_si128 (),
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_cvtps_epi32 (__m128i __W, __mmask8 __U, __m128 __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_cvtps_epi32(__A),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_cvtps_epi32 (__mmask8 __U, __m128 __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_cvtps_epi32(__A),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_cvtps_epi32 (__m256i __W, __mmask8 __U, __m256 __A) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_cvtps_epi32(__A),
                                             (__v8si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtps_epi32 (__mmask8 __U, __m256 __A) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_cvtps_epi32(__A),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_cvtps_pd (__m128d __W, __mmask8 __U, __m128 __A) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_cvtps_pd(__A),
                                              (__v2df)__W);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_cvtps_pd (__mmask8 __U, __m128 __A) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_cvtps_pd(__A),
                                              (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_cvtps_pd (__m256d __W, __mmask8 __U, __m128 __A) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_cvtps_pd(__A),
                                              (__v4df)__W);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtps_pd (__mmask8 __U, __m128 __A) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_cvtps_pd(__A),
                                              (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_cvtps_epu32 (__m128 __A) {
  return (__m128i) __builtin_ia32_cvtps2udq128_mask ((__v4sf) __A,
                 (__v4si)
                 _mm_setzero_si128 (),
                 (__mmask8) -1);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_cvtps_epu32 (__m128i __W, __mmask8 __U, __m128 __A) {
  return (__m128i) __builtin_ia32_cvtps2udq128_mask ((__v4sf) __A,
                 (__v4si) __W,
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_cvtps_epu32 (__mmask8 __U, __m128 __A) {
  return (__m128i) __builtin_ia32_cvtps2udq128_mask ((__v4sf) __A,
                 (__v4si)
                 _mm_setzero_si128 (),
                 (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_cvtps_epu32 (__m256 __A) {
  return (__m256i) __builtin_ia32_cvtps2udq256_mask ((__v8sf) __A,
                 (__v8si)
                 _mm256_setzero_si256 (),
                 (__mmask8) -1);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_cvtps_epu32 (__m256i __W, __mmask8 __U, __m256 __A) {
  return (__m256i) __builtin_ia32_cvtps2udq256_mask ((__v8sf) __A,
                 (__v8si) __W,
                 (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtps_epu32 (__mmask8 __U, __m256 __A) {
  return (__m256i) __builtin_ia32_cvtps2udq256_mask ((__v8sf) __A,
                 (__v8si)
                 _mm256_setzero_si256 (),
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_cvttpd_epi32 (__m128i __W, __mmask8 __U, __m128d __A) {
  return (__m128i) __builtin_ia32_cvttpd2dq128_mask ((__v2df) __A,
                 (__v4si) __W,
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_cvttpd_epi32 (__mmask8 __U, __m128d __A) {
  return (__m128i) __builtin_ia32_cvttpd2dq128_mask ((__v2df) __A,
                 (__v4si)
                 _mm_setzero_si128 (),
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_mask_cvttpd_epi32 (__m128i __W, __mmask8 __U, __m256d __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm256_cvttpd_epi32(__A),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_maskz_cvttpd_epi32 (__mmask8 __U, __m256d __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm256_cvttpd_epi32(__A),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_cvttpd_epu32 (__m128d __A) {
  return (__m128i) __builtin_ia32_cvttpd2udq128_mask ((__v2df) __A,
                  (__v4si)
                  _mm_setzero_si128 (),
                  (__mmask8) -1);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_cvttpd_epu32 (__m128i __W, __mmask8 __U, __m128d __A) {
  return (__m128i) __builtin_ia32_cvttpd2udq128_mask ((__v2df) __A,
                  (__v4si) __W,
                  (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_cvttpd_epu32 (__mmask8 __U, __m128d __A) {
  return (__m128i) __builtin_ia32_cvttpd2udq128_mask ((__v2df) __A,
                  (__v4si)
                  _mm_setzero_si128 (),
                  (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_cvttpd_epu32 (__m256d __A) {
  return (__m128i) __builtin_ia32_cvttpd2udq256_mask ((__v4df) __A,
                  (__v4si)
                  _mm_setzero_si128 (),
                  (__mmask8) -1);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_mask_cvttpd_epu32 (__m128i __W, __mmask8 __U, __m256d __A) {
  return (__m128i) __builtin_ia32_cvttpd2udq256_mask ((__v4df) __A,
                  (__v4si) __W,
                  (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS256
_mm256_maskz_cvttpd_epu32 (__mmask8 __U, __m256d __A) {
  return (__m128i) __builtin_ia32_cvttpd2udq256_mask ((__v4df) __A,
                  (__v4si)
                  _mm_setzero_si128 (),
                  (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_cvttps_epi32 (__m128i __W, __mmask8 __U, __m128 __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_cvttps_epi32(__A),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_cvttps_epi32 (__mmask8 __U, __m128 __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_cvttps_epi32(__A),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_cvttps_epi32 (__m256i __W, __mmask8 __U, __m256 __A) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_cvttps_epi32(__A),
                                             (__v8si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_cvttps_epi32 (__mmask8 __U, __m256 __A) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_cvttps_epi32(__A),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_cvttps_epu32 (__m128 __A) {
  return (__m128i) __builtin_ia32_cvttps2udq128_mask ((__v4sf) __A,
                  (__v4si)
                  _mm_setzero_si128 (),
                  (__mmask8) -1);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_cvttps_epu32 (__m128i __W, __mmask8 __U, __m128 __A) {
  return (__m128i) __builtin_ia32_cvttps2udq128_mask ((__v4sf) __A,
                  (__v4si) __W,
                  (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_cvttps_epu32 (__mmask8 __U, __m128 __A) {
  return (__m128i) __builtin_ia32_cvttps2udq128_mask ((__v4sf) __A,
                  (__v4si)
                  _mm_setzero_si128 (),
                  (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_cvttps_epu32 (__m256 __A) {
  return (__m256i) __builtin_ia32_cvttps2udq256_mask ((__v8sf) __A,
                  (__v8si)
                  _mm256_setzero_si256 (),
                  (__mmask8) -1);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_cvttps_epu32 (__m256i __W, __mmask8 __U, __m256 __A) {
  return (__m256i) __builtin_ia32_cvttps2udq256_mask ((__v8sf) __A,
                  (__v8si) __W,
                  (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_cvttps_epu32 (__mmask8 __U, __m256 __A) {
  return (__m256i) __builtin_ia32_cvttps2udq256_mask ((__v8sf) __A,
                  (__v8si)
                  _mm256_setzero_si256 (),
                  (__mmask8) __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_cvtepu32_pd (__m128i __A) {
  return (__m128d) __builtin_convertvector(
      __builtin_shufflevector((__v4su)__A, (__v4su)__A, 0, 1), __v2df);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_cvtepu32_pd (__m128d __W, __mmask8 __U, __m128i __A) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8) __U,
                                              (__v2df)_mm_cvtepu32_pd(__A),
                                              (__v2df)__W);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_cvtepu32_pd (__mmask8 __U, __m128i __A) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8) __U,
                                              (__v2df)_mm_cvtepu32_pd(__A),
                                              (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_cvtepu32_pd (__m128i __A) {
  return (__m256d)__builtin_convertvector((__v4su)__A, __v4df);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_cvtepu32_pd (__m256d __W, __mmask8 __U, __m128i __A) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8) __U,
                                              (__v4df)_mm256_cvtepu32_pd(__A),
                                              (__v4df)__W);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtepu32_pd (__mmask8 __U, __m128i __A) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8) __U,
                                              (__v4df)_mm256_cvtepu32_pd(__A),
                                              (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_cvtepu32_ps (__m128i __A) {
  return (__m128)__builtin_convertvector((__v4su)__A, __v4sf);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_cvtepu32_ps (__m128 __W, __mmask8 __U, __m128i __A) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_cvtepu32_ps(__A),
                                             (__v4sf)__W);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_cvtepu32_ps (__mmask8 __U, __m128i __A) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_cvtepu32_ps(__A),
                                             (__v4sf)_mm_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_cvtepu32_ps (__m256i __A) {
  return (__m256)__builtin_convertvector((__v8su)__A, __v8sf);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_cvtepu32_ps (__m256 __W, __mmask8 __U, __m256i __A) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_cvtepu32_ps(__A),
                                             (__v8sf)__W);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_cvtepu32_ps (__mmask8 __U, __m256i __A) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_cvtepu32_ps(__A),
                                             (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_div_pd(__m128d __W, __mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_div_pd(__A, __B),
                                              (__v2df)__W);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_div_pd(__mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_div_pd(__A, __B),
                                              (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_div_pd(__m256d __W, __mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_div_pd(__A, __B),
                                              (__v4df)__W);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_div_pd(__mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_div_pd(__A, __B),
                                              (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_div_ps(__m128 __W, __mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_div_ps(__A, __B),
                                             (__v4sf)__W);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_div_ps(__mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_div_ps(__A, __B),
                                             (__v4sf)_mm_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_div_ps(__m256 __W, __mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_div_ps(__A, __B),
                                             (__v8sf)__W);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_div_ps(__mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_div_ps(__A, __B),
                                             (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_expand_pd (__m128d __W, __mmask8 __U, __m128d __A) {
  return (__m128d) __builtin_ia32_expanddf128_mask ((__v2df) __A,
                (__v2df) __W,
                (__mmask8) __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_expand_pd (__mmask8 __U, __m128d __A) {
  return (__m128d) __builtin_ia32_expanddf128_mask ((__v2df) __A,
                 (__v2df)
                 _mm_setzero_pd (),
                 (__mmask8) __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_expand_pd (__m256d __W, __mmask8 __U, __m256d __A) {
  return (__m256d) __builtin_ia32_expanddf256_mask ((__v4df) __A,
                (__v4df) __W,
                (__mmask8) __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_expand_pd (__mmask8 __U, __m256d __A) {
  return (__m256d) __builtin_ia32_expanddf256_mask ((__v4df) __A,
                 (__v4df)
                 _mm256_setzero_pd (),
                 (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_expand_epi64 (__m128i __W, __mmask8 __U, __m128i __A) {
  return (__m128i) __builtin_ia32_expanddi128_mask ((__v2di) __A,
                (__v2di) __W,
                (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_expand_epi64 (__mmask8 __U, __m128i __A) {
  return (__m128i) __builtin_ia32_expanddi128_mask ((__v2di) __A,
                 (__v2di)
                 _mm_setzero_si128 (),
                 (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_expand_epi64 (__m256i __W, __mmask8 __U, __m256i __A) {
  return (__m256i) __builtin_ia32_expanddi256_mask ((__v4di) __A,
                (__v4di) __W,
                (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_expand_epi64 (__mmask8 __U, __m256i __A) {
  return (__m256i) __builtin_ia32_expanddi256_mask ((__v4di) __A,
                 (__v4di)
                 _mm256_setzero_si256 (),
                 (__mmask8) __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_expandloadu_pd (__m128d __W, __mmask8 __U, void const *__P) {
  return (__m128d) __builtin_ia32_expandloaddf128_mask ((const __v2df *) __P,
              (__v2df) __W,
              (__mmask8)
              __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_expandloadu_pd (__mmask8 __U, void const *__P) {
  return (__m128d) __builtin_ia32_expandloaddf128_mask ((const __v2df *) __P,
               (__v2df)
               _mm_setzero_pd (),
               (__mmask8)
               __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_expandloadu_pd (__m256d __W, __mmask8 __U, void const *__P) {
  return (__m256d) __builtin_ia32_expandloaddf256_mask ((const __v4df *) __P,
              (__v4df) __W,
              (__mmask8)
              __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_expandloadu_pd (__mmask8 __U, void const *__P) {
  return (__m256d) __builtin_ia32_expandloaddf256_mask ((const __v4df *) __P,
               (__v4df)
               _mm256_setzero_pd (),
               (__mmask8)
               __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_expandloadu_epi64 (__m128i __W, __mmask8 __U, void const *__P) {
  return (__m128i) __builtin_ia32_expandloaddi128_mask ((const __v2di *) __P,
              (__v2di) __W,
              (__mmask8)
              __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_expandloadu_epi64 (__mmask8 __U, void const *__P) {
  return (__m128i) __builtin_ia32_expandloaddi128_mask ((const __v2di *) __P,
               (__v2di)
               _mm_setzero_si128 (),
               (__mmask8)
               __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_expandloadu_epi64 (__m256i __W, __mmask8 __U,
             void const *__P) {
  return (__m256i) __builtin_ia32_expandloaddi256_mask ((const __v4di *) __P,
              (__v4di) __W,
              (__mmask8)
              __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_expandloadu_epi64 (__mmask8 __U, void const *__P) {
  return (__m256i) __builtin_ia32_expandloaddi256_mask ((const __v4di *) __P,
               (__v4di)
               _mm256_setzero_si256 (),
               (__mmask8)
               __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_expandloadu_ps (__m128 __W, __mmask8 __U, void const *__P) {
  return (__m128) __builtin_ia32_expandloadsf128_mask ((const __v4sf *) __P,
                   (__v4sf) __W,
                   (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_expandloadu_ps (__mmask8 __U, void const *__P) {
  return (__m128) __builtin_ia32_expandloadsf128_mask ((const __v4sf *) __P,
              (__v4sf)
              _mm_setzero_ps (),
              (__mmask8)
              __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_expandloadu_ps (__m256 __W, __mmask8 __U, void const *__P) {
  return (__m256) __builtin_ia32_expandloadsf256_mask ((const __v8sf *) __P,
                   (__v8sf) __W,
                   (__mmask8) __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_expandloadu_ps (__mmask8 __U, void const *__P) {
  return (__m256) __builtin_ia32_expandloadsf256_mask ((const __v8sf *) __P,
              (__v8sf)
              _mm256_setzero_ps (),
              (__mmask8)
              __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_expandloadu_epi32 (__m128i __W, __mmask8 __U, void const *__P) {
  return (__m128i) __builtin_ia32_expandloadsi128_mask ((const __v4si *) __P,
              (__v4si) __W,
              (__mmask8)
              __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_expandloadu_epi32 (__mmask8 __U, void const *__P) {
  return (__m128i) __builtin_ia32_expandloadsi128_mask ((const __v4si *) __P,
               (__v4si)
               _mm_setzero_si128 (),
               (__mmask8)     __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_expandloadu_epi32 (__m256i __W, __mmask8 __U,
             void const *__P) {
  return (__m256i) __builtin_ia32_expandloadsi256_mask ((const __v8si *) __P,
              (__v8si) __W,
              (__mmask8)
              __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_expandloadu_epi32 (__mmask8 __U, void const *__P) {
  return (__m256i) __builtin_ia32_expandloadsi256_mask ((const __v8si *) __P,
               (__v8si)
               _mm256_setzero_si256 (),
               (__mmask8)
               __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_expand_ps (__m128 __W, __mmask8 __U, __m128 __A) {
  return (__m128) __builtin_ia32_expandsf128_mask ((__v4sf) __A,
               (__v4sf) __W,
               (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_expand_ps (__mmask8 __U, __m128 __A) {
  return (__m128) __builtin_ia32_expandsf128_mask ((__v4sf) __A,
                (__v4sf)
                _mm_setzero_ps (),
                (__mmask8) __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_expand_ps (__m256 __W, __mmask8 __U, __m256 __A) {
  return (__m256) __builtin_ia32_expandsf256_mask ((__v8sf) __A,
               (__v8sf) __W,
               (__mmask8) __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_expand_ps (__mmask8 __U, __m256 __A) {
  return (__m256) __builtin_ia32_expandsf256_mask ((__v8sf) __A,
                (__v8sf)
                _mm256_setzero_ps (),
                (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_expand_epi32 (__m128i __W, __mmask8 __U, __m128i __A) {
  return (__m128i) __builtin_ia32_expandsi128_mask ((__v4si) __A,
                (__v4si) __W,
                (__mmask8) __U);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_expand_epi32 (__mmask8 __U, __m128i __A) {
  return (__m128i) __builtin_ia32_expandsi128_mask ((__v4si) __A,
                 (__v4si)
                 _mm_setzero_si128 (),
                 (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_expand_epi32 (__m256i __W, __mmask8 __U, __m256i __A) {
  return (__m256i) __builtin_ia32_expandsi256_mask ((__v8si) __A,
                (__v8si) __W,
                (__mmask8) __U);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_expand_epi32 (__mmask8 __U, __m256i __A) {
  return (__m256i) __builtin_ia32_expandsi256_mask ((__v8si) __A,
                 (__v8si)
                 _mm256_setzero_si256 (),
                 (__mmask8) __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_getexp_pd (__m128d __A) {
  return (__m128d) __builtin_ia32_getexppd128_mask ((__v2df) __A,
                (__v2df)
                _mm_setzero_pd (),
                (__mmask8) -1);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_getexp_pd (__m128d __W, __mmask8 __U, __m128d __A) {
  return (__m128d) __builtin_ia32_getexppd128_mask ((__v2df) __A,
                (__v2df) __W,
                (__mmask8) __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_getexp_pd (__mmask8 __U, __m128d __A) {
  return (__m128d) __builtin_ia32_getexppd128_mask ((__v2df) __A,
                (__v2df)
                _mm_setzero_pd (),
                (__mmask8) __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_getexp_pd (__m256d __A) {
  return (__m256d) __builtin_ia32_getexppd256_mask ((__v4df) __A,
                (__v4df)
                _mm256_setzero_pd (),
                (__mmask8) -1);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_getexp_pd (__m256d __W, __mmask8 __U, __m256d __A) {
  return (__m256d) __builtin_ia32_getexppd256_mask ((__v4df) __A,
                (__v4df) __W,
                (__mmask8) __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_getexp_pd (__mmask8 __U, __m256d __A) {
  return (__m256d) __builtin_ia32_getexppd256_mask ((__v4df) __A,
                (__v4df)
                _mm256_setzero_pd (),
                (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_getexp_ps (__m128 __A) {
  return (__m128) __builtin_ia32_getexpps128_mask ((__v4sf) __A,
               (__v4sf)
               _mm_setzero_ps (),
               (__mmask8) -1);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_getexp_ps (__m128 __W, __mmask8 __U, __m128 __A) {
  return (__m128) __builtin_ia32_getexpps128_mask ((__v4sf) __A,
               (__v4sf) __W,
               (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_getexp_ps (__mmask8 __U, __m128 __A) {
  return (__m128) __builtin_ia32_getexpps128_mask ((__v4sf) __A,
               (__v4sf)
               _mm_setzero_ps (),
               (__mmask8) __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_getexp_ps (__m256 __A) {
  return (__m256) __builtin_ia32_getexpps256_mask ((__v8sf) __A,
               (__v8sf)
               _mm256_setzero_ps (),
               (__mmask8) -1);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_getexp_ps (__m256 __W, __mmask8 __U, __m256 __A) {
  return (__m256) __builtin_ia32_getexpps256_mask ((__v8sf) __A,
               (__v8sf) __W,
               (__mmask8) __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_getexp_ps (__mmask8 __U, __m256 __A) {
  return (__m256) __builtin_ia32_getexpps256_mask ((__v8sf) __A,
               (__v8sf)
               _mm256_setzero_ps (),
               (__mmask8) __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_max_pd(__m128d __W, __mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_max_pd(__A, __B),
                                              (__v2df)__W);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_max_pd(__mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_max_pd(__A, __B),
                                              (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_max_pd(__m256d __W, __mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_max_pd(__A, __B),
                                              (__v4df)__W);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_max_pd(__mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_max_pd(__A, __B),
                                              (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_max_ps(__m128 __W, __mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_max_ps(__A, __B),
                                             (__v4sf)__W);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_max_ps(__mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_max_ps(__A, __B),
                                             (__v4sf)_mm_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_max_ps(__m256 __W, __mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_max_ps(__A, __B),
                                             (__v8sf)__W);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_max_ps(__mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_max_ps(__A, __B),
                                             (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_min_pd(__m128d __W, __mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_min_pd(__A, __B),
                                              (__v2df)__W);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_min_pd(__mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_min_pd(__A, __B),
                                              (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_min_pd(__m256d __W, __mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_min_pd(__A, __B),
                                              (__v4df)__W);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_min_pd(__mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_min_pd(__A, __B),
                                              (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_min_ps(__m128 __W, __mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_min_ps(__A, __B),
                                             (__v4sf)__W);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_min_ps(__mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_min_ps(__A, __B),
                                             (__v4sf)_mm_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_min_ps(__m256 __W, __mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_min_ps(__A, __B),
                                             (__v8sf)__W);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_min_ps(__mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_min_ps(__A, __B),
                                             (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_mul_pd(__m128d __W, __mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_mul_pd(__A, __B),
                                              (__v2df)__W);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_mul_pd(__mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                              (__v2df)_mm_mul_pd(__A, __B),
                                              (__v2df)_mm_setzero_pd());
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_mul_pd(__m256d __W, __mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_mul_pd(__A, __B),
                                              (__v4df)__W);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_mul_pd(__mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                              (__v4df)_mm256_mul_pd(__A, __B),
                                              (__v4df)_mm256_setzero_pd());
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_mul_ps(__m128 __W, __mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_mul_ps(__A, __B),
                                             (__v4sf)__W);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_mul_ps(__mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                             (__v4sf)_mm_mul_ps(__A, __B),
                                             (__v4sf)_mm_setzero_ps());
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_mul_ps(__m256 __W, __mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_mul_ps(__A, __B),
                                             (__v8sf)__W);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_mul_ps(__mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                             (__v8sf)_mm256_mul_ps(__A, __B),
                                             (__v8sf)_mm256_setzero_ps());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_abs_epi32(__m128i __W, __mmask8 __U, __m128i __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_abs_epi32(__A),
                                             (__v4si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_abs_epi32(__mmask8 __U, __m128i __A) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                             (__v4si)_mm_abs_epi32(__A),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_abs_epi32(__m256i __W, __mmask8 __U, __m256i __A) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_abs_epi32(__A),
                                             (__v8si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_abs_epi32(__mmask8 __U, __m256i __A) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                             (__v8si)_mm256_abs_epi32(__A),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_abs_epi64 (__m128i __A) {
  return (__m128i)__builtin_elementwise_abs((__v2di)__A);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_abs_epi64 (__m128i __W, __mmask8 __U, __m128i __A) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_abs_epi64(__A),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_abs_epi64 (__mmask8 __U, __m128i __A) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                             (__v2di)_mm_abs_epi64(__A),
                                             (__v2di)_mm_setzero_si128());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_abs_epi64 (__m256i __A) {
  return (__m256i)__builtin_elementwise_abs((__v4di)__A);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_abs_epi64 (__m256i __W, __mmask8 __U, __m256i __A) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                             (__v4di)_mm256_abs_epi64(__A),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_abs_epi64 (__mmask8 __U, __m256i __A) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                             (__v4di)_mm256_abs_epi64(__A),
                                             (__v4di)_mm256_setzero_si256());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_max_epi32(__mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_max_epi32(__A, __B),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_max_epi32(__m128i __W, __mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_max_epi32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_max_epi32(__mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_max_epi32(__A, __B),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_max_epi32(__m256i __W, __mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_max_epi32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_max_epi64 (__m128i __A, __m128i __B) {
  return (__m128i)__builtin_elementwise_max((__v2di)__A, (__v2di)__B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_max_epi64 (__mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_max_epi64(__A, __B),
                                             (__v2di)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_max_epi64 (__m128i __W, __mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_max_epi64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_max_epi64 (__m256i __A, __m256i __B) {
  return (__m256i)__builtin_elementwise_max((__v4di)__A, (__v4di)__B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_max_epi64 (__mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_max_epi64(__A, __B),
                                             (__v4di)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_max_epi64 (__m256i __W, __mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_max_epi64(__A, __B),
                                             (__v4di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_max_epu32(__mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_max_epu32(__A, __B),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_max_epu32(__m128i __W, __mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_max_epu32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_max_epu32(__mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_max_epu32(__A, __B),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_max_epu32(__m256i __W, __mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_max_epu32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_max_epu64 (__m128i __A, __m128i __B) {
  return (__m128i)__builtin_elementwise_max((__v2du)__A, (__v2du)__B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_max_epu64 (__mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_max_epu64(__A, __B),
                                             (__v2di)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_max_epu64 (__m128i __W, __mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_max_epu64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_max_epu64 (__m256i __A, __m256i __B) {
  return (__m256i)__builtin_elementwise_max((__v4du)__A, (__v4du)__B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_max_epu64 (__mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_max_epu64(__A, __B),
                                             (__v4di)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_max_epu64 (__m256i __W, __mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_max_epu64(__A, __B),
                                             (__v4di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_min_epi32(__mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_min_epi32(__A, __B),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_min_epi32(__m128i __W, __mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_min_epi32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_min_epi32(__mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_min_epi32(__A, __B),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_min_epi32(__m256i __W, __mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_min_epi32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_min_epi64 (__m128i __A, __m128i __B) {
  return (__m128i)__builtin_elementwise_min((__v2di)__A, (__v2di)__B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_min_epi64 (__m128i __W, __mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_min_epi64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_min_epi64 (__mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_min_epi64(__A, __B),
                                             (__v2di)_mm_setzero_si128());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_min_epi64 (__m256i __A, __m256i __B) {
  return (__m256i)__builtin_elementwise_min((__v4di)__A, (__v4di)__B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_min_epi64 (__m256i __W, __mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_min_epi64(__A, __B),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_min_epi64 (__mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_min_epi64(__A, __B),
                                             (__v4di)_mm256_setzero_si256());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_min_epu32(__mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_min_epu32(__A, __B),
                                             (__v4si)_mm_setzero_si128());
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_min_epu32(__m128i __W, __mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectd_128((__mmask8)__M,
                                             (__v4si)_mm_min_epu32(__A, __B),
                                             (__v4si)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_min_epu32(__mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_min_epu32(__A, __B),
                                             (__v8si)_mm256_setzero_si256());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_min_epu32(__m256i __W, __mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectd_256((__mmask8)__M,
                                             (__v8si)_mm256_min_epu32(__A, __B),
                                             (__v8si)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_min_epu64 (__m128i __A, __m128i __B) {
  return (__m128i)__builtin_elementwise_min((__v2du)__A, (__v2du)__B);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_mask_min_epu64 (__m128i __W, __mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_min_epu64(__A, __B),
                                             (__v2di)__W);
}

static __inline__ __m128i __DEFAULT_FN_ATTRS128
_mm_maskz_min_epu64 (__mmask8 __M, __m128i __A, __m128i __B) {
  return (__m128i)__builtin_ia32_selectq_128((__mmask8)__M,
                                             (__v2di)_mm_min_epu64(__A, __B),
                                             (__v2di)_mm_setzero_si128());
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_min_epu64 (__m256i __A, __m256i __B) {
  return (__m256i)__builtin_elementwise_min((__v4du)__A, (__v4du)__B);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_mask_min_epu64 (__m256i __W, __mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_min_epu64(__A, __B),
                                             (__v4di)__W);
}

static __inline__ __m256i __DEFAULT_FN_ATTRS256
_mm256_maskz_min_epu64 (__mmask8 __M, __m256i __A, __m256i __B) {
  return (__m256i)__builtin_ia32_selectq_256((__mmask8)__M,
                                             (__v4di)_mm256_min_epu64(__A, __B),
                                             (__v4di)_mm256_setzero_si256());
}

#define _mm_roundscale_pd(A, imm) \
  ((__m128d)__builtin_ia32_rndscalepd_128_mask((__v2df)(__m128d)(A), \
                                               (int)(imm), \
                                               (__v2df)_mm_setzero_pd(), \
                                               (__mmask8)-1))


#define _mm_mask_roundscale_pd(W, U, A, imm) \
  ((__m128d)__builtin_ia32_rndscalepd_128_mask((__v2df)(__m128d)(A), \
                                               (int)(imm), \
                                               (__v2df)(__m128d)(W), \
                                               (__mmask8)(U)))


#define _mm_maskz_roundscale_pd(U, A, imm) \
  ((__m128d)__builtin_ia32_rndscalepd_128_mask((__v2df)(__m128d)(A), \
                                               (int)(imm), \
                                               (__v2df)_mm_setzero_pd(), \
                                               (__mmask8)(U)))


#define _mm256_roundscale_pd(A, imm) \
  ((__m256d)__builtin_ia32_rndscalepd_256_mask((__v4df)(__m256d)(A), \
                                               (int)(imm), \
                                               (__v4df)_mm256_setzero_pd(), \
                                               (__mmask8)-1))


#define _mm256_mask_roundscale_pd(W, U, A, imm) \
  ((__m256d)__builtin_ia32_rndscalepd_256_mask((__v4df)(__m256d)(A), \
                                               (int)(imm), \
                                               (__v4df)(__m256d)(W), \
                                               (__mmask8)(U)))


#define _mm256_maskz_roundscale_pd(U, A, imm)  \
  ((__m256d)__builtin_ia32_rndscalepd_256_mask((__v4df)(__m256d)(A), \
                                               (int)(imm), \
                                               (__v4df)_mm256_setzero_pd(), \
                                               (__mmask8)(U)))

#define _mm_roundscale_ps(A, imm)  \
  ((__m128)__builtin_ia32_rndscaleps_128_mask((__v4sf)(__m128)(A), (int)(imm), \
                                              (__v4sf)_mm_setzero_ps(), \
                                              (__mmask8)-1))


#define _mm_mask_roundscale_ps(W, U, A, imm)  \
  ((__m128)__builtin_ia32_rndscaleps_128_mask((__v4sf)(__m128)(A), (int)(imm), \
                                              (__v4sf)(__m128)(W), \
                                              (__mmask8)(U)))


#define _mm_maskz_roundscale_ps(U, A, imm)  \
  ((__m128)__builtin_ia32_rndscaleps_128_mask((__v4sf)(__m128)(A), (int)(imm), \
                                              (__v4sf)_mm_setzero_ps(), \
                                              (__mmask8)(U)))

#define _mm256_roundscale_ps(A, imm)  \
  ((__m256)__builtin_ia32_rndscaleps_256_mask((__v8sf)(__m256)(A), (int)(imm), \
                                              (__v8sf)_mm256_setzero_ps(), \
                                              (__mmask8)-1))

#define _mm256_mask_roundscale_ps(W, U, A, imm)  \
  ((__m256)__builtin_ia32_rndscaleps_256_mask((__v8sf)(__m256)(A), (int)(imm), \
                                              (__v8sf)(__m256)(W), \
                                              (__mmask8)(U)))


#define _mm256_maskz_roundscale_ps(U, A, imm)  \
  ((__m256)__builtin_ia32_rndscaleps_256_mask((__v8sf)(__m256)(A), (int)(imm), \
                                              (__v8sf)_mm256_setzero_ps(), \
                                              (__mmask8)(U)))

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_scalef_pd (__m128d __A, __m128d __B) {
  return (__m128d) __builtin_ia32_scalefpd128_mask ((__v2df) __A,
                (__v2df) __B,
                (__v2df)
                _mm_setzero_pd (),
                (__mmask8) -1);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_mask_scalef_pd (__m128d __W, __mmask8 __U, __m128d __A,
        __m128d __B) {
  return (__m128d) __builtin_ia32_scalefpd128_mask ((__v2df) __A,
                (__v2df) __B,
                (__v2df) __W,
                (__mmask8) __U);
}

static __inline__ __m128d __DEFAULT_FN_ATTRS128
_mm_maskz_scalef_pd (__mmask8 __U, __m128d __A, __m128d __B) {
  return (__m128d) __builtin_ia32_scalefpd128_mask ((__v2df) __A,
                (__v2df) __B,
                (__v2df)
                _mm_setzero_pd (),
                (__mmask8) __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_scalef_pd (__m256d __A, __m256d __B) {
  return (__m256d) __builtin_ia32_scalefpd256_mask ((__v4df) __A,
                (__v4df) __B,
                (__v4df)
                _mm256_setzero_pd (),
                (__mmask8) -1);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_mask_scalef_pd (__m256d __W, __mmask8 __U, __m256d __A,
           __m256d __B) {
  return (__m256d) __builtin_ia32_scalefpd256_mask ((__v4df) __A,
                (__v4df) __B,
                (__v4df) __W,
                (__mmask8) __U);
}

static __inline__ __m256d __DEFAULT_FN_ATTRS256
_mm256_maskz_scalef_pd (__mmask8 __U, __m256d __A, __m256d __B) {
  return (__m256d) __builtin_ia32_scalefpd256_mask ((__v4df) __A,
                (__v4df) __B,
                (__v4df)
                _mm256_setzero_pd (),
                (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_scalef_ps (__m128 __A, __m128 __B) {
  return (__m128) __builtin_ia32_scalefps128_mask ((__v4sf) __A,
               (__v4sf) __B,
               (__v4sf)
               _mm_setzero_ps (),
               (__mmask8) -1);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_mask_scalef_ps (__m128 __W, __mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128) __builtin_ia32_scalefps128_mask ((__v4sf) __A,
               (__v4sf) __B,
               (__v4sf) __W,
               (__mmask8) __U);
}

static __inline__ __m128 __DEFAULT_FN_ATTRS128
_mm_maskz_scalef_ps (__mmask8 __U, __m128 __A, __m128 __B) {
  return (__m128) __builtin_ia32_scalefps128_mask ((__v4sf) __A,
               (__v4sf) __B,
               (__v4sf)
               _mm_setzero_ps (),
               (__mmask8) __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_scalef_ps (__m256 __A, __m256 __B) {
  return (__m256) __builtin_ia32_scalefps256_mask ((__v8sf) __A,
               (__v8sf) __B,
               (__v8sf)
               _mm256_setzero_ps (),
               (__mmask8) -1);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_mask_scalef_ps (__m256 __W, __mmask8 __U, __m256 __A,
           __m256 __B) {
  return (__m256) __builtin_ia32_scalefps256_mask ((__v8sf) __A,
               (__v8sf) __B,
               (__v8sf) __W,
               (__mmask8) __U);
}

static __inline__ __m256 __DEFAULT_FN_ATTRS256
_mm256_maskz_scalef_ps (__mmask8 __U, __m256 __A, __m256 __B) {
  return (__m256) __builtin_ia32_scalefps256_mask ((__v8sf) __A,
               (__v8sf) __B,
               (__v8sf)
               _mm256_setzero_ps (),
               (__mmask8) __U);
}

#define _mm_i64scatter_pd(addr, index, v1, scale) \
  __builtin_ia32_scatterdiv2df((void *)(addr), (__mmask8)-1, \
                               (__v2di)(__m128i)(index), \
                               (__v2df)(__m128d)(v1), (int)(scale))

#define _mm_mask_i64scatter_pd(addr, mask, index, v1, scale) \
  __builtin_ia32_scatterdiv2df((void *)(addr), (__mmask8)(mask), \
                               (__v2di)(__m128i)(index), \
                               (__v2df)(__m128d)(v1), (int)(scale))

#define _mm_i64scatter_epi64(addr, index, v1, scale) \
  __builtin_ia32_scatterdiv2di((void *)(addr), (__mmask8)-1, \
                               (__v2di)(__m128i)(index), \
                               (__v2di)(__m128i)(v1), (int)(scale))

#define _mm_mask_i64scatter_epi64(addr, mask, index, v1, scale) \
  __builtin_ia32_scatterdiv2di((void *)(addr), (__mmask8)(mask), \
                               (__v2di)(__m128i)(index), \
                               (__v2di)(__m128i)(v1), (int)(scale))

#define _mm256_i64scatter_pd(addr, index, v1, scale) \
  __builtin_ia32_scatterdiv4df((void *)(addr), (__mmask8)-1, \
                               (__v4di)(__m256i)(index), \
                               (__v4df)(__m256d)(v1), (int)(scale))

#define _mm256_mask_i64scatter_pd(addr, mask, index, v1, scale) \
  __builtin_ia32_scatterdiv4df((void *)(addr), (__mmask8)(mask), \
                               (__v4di)(__m256i)(index), \
                               (__v4df)(__m256d)(v1), (int)(scale))

#define _mm256_i64scatter_epi64(addr, index, v1, scale) \
  __builtin_ia32_scatterdiv4di((void *)(addr), (__mmask8)-1, \
                               (__v4di)(__m256i)(index), \
                               (__v4di)(__m256i)(v1), (int)(scale))

#define _mm256_mask_i64scatter_epi64(addr, mask, index, v1, scale) \
  __builtin_ia32_scatterdiv4di((void *)(addr), (__mmask8)(mask), \
                               (__v4di)(__m256i)(index), \
                               (__v4di)(__m256i)(v1), (int)(scale))

#define _mm_i64scatter_ps(addr, index, v1, scale) \
  __builtin_ia32_scatterdiv4sf((void *)(addr), (__mmask8)-1, \
                               (__v2di)(__m128i)(index), (__v4sf)(__m128)(v1), \
                               (int)(scale))

#define _mm_mask_i64scatter_ps(addr, mask, index, v1, scale) \
  __builtin_ia32_scatterdiv4sf((void *)(addr), (__mmask8)(mask), \
                               (__v2di)(__m128i)(index), (__v4sf)(__m128)(v1), \
                               (int)(scale))

#define _mm_i64scatter_epi32(addr, index, v1, scale) \
  __builtin_ia32_scatterdiv4si((void *)(addr), (__mmask8)-1, \
                               (__v2di)(__m128i)(index), \
                               (__v4si)(__m128i)(v1), (int)(scale))

#define _mm_mask_i64scatter_epi32(addr, mask, index, v1, scale) \
  __builtin_ia32_scatterdiv4si((void *)(addr), (__mmask8)(mask), \
                               (__v2di)(__m128i)(index), \
                               (__v4si)(__m128i)(v1), (int)(scale))

#define _mm256_i64scatter_ps(addr, index, v1, scale) \
  __builtin_ia32_scatterdiv8sf((void *)(addr), (__mmask8)-1, \
                               (__v4di)(__m256i)(index), (__v4sf)(__m128)(v1), \
                               (int)(scale))

#define _mm256_mask_i64scatter_ps(addr, mask, index, v1, scale) \
  __builtin_ia32_scatterdiv8sf((void *)(addr), (__mmask8)(mask), \
                               (__v4di)(__m256i)(index), (__v4sf)(__m128)(v1), \
                               (int)(scale))

#define _mm256_i64scatter_epi32(addr, index, v1, scale) \
  __builtin_ia32_scatterdiv8si((void *)(addr), (__mmask8)-1, \
                               (__v4di)(__m256i)(index), \
                               (__v4si)(__m128i)(v1), (int)(scale))

#define _mm256_mask_i64scatter_epi32(addr, mask, index, v1, scale) \
  __builtin_ia32_scatterdiv8si((void *)(addr), (__mmask8)(mask), \
                               (__v4di)(__m256i)(index), \
                               (__v4si)(__m128i)(v1), (int)(scale))

#define _mm_i32scatter_pd(addr, index, v1, scale) \
  __builtin_ia32_scattersiv2df((void *)(addr), (__mmask8)-1, \
                               (__v4si)(__m128i)(index), \
                               (__v2df)(__m128d)(v1), (int)(scale))

#define _mm_mask_i32scatter_pd(addr, mask, index, v1, scale) \
    __builtin_ia32_scattersiv2df((void *)(addr), (__mmask8)(mask), \
                                 (__v4si)(__m128i)(index), \
                                 (__v2df)(__m128d)(v1), (int)(scale))

#define _mm_i32scatter_epi64(addr, index, v1, scale) \
    __builtin_ia32_scattersiv2di((void *)(addr), (__mmask8)-1, \
                                 (__v4si)(__m128i)(index), \
                                 (__v2di)(__m128i)(v1), (int)(scale))

#define _mm_mask_i32scatter_epi64(addr, mask, index, v1, scale) \
    __builtin_ia32_scattersiv2di((void *)(addr), (__mmask8)(mask), \
                                 (__v4si)(__m128i)(index), \
                                 (__v2di)(__m128i)(v1), (int)(scale))

#define _mm256_i32scatter_pd(addr, index, v1, scale) \
    __builtin_ia32_scattersiv4df((void *)(addr), (__mmask8)-1, \
                                 (__v4si)(__m128i)(index), \
                                 (__v4df)(__m256d)(v1), (int)(scale))

#define _mm256_mask_i32scatter_pd(addr, mask, index, v1, scale) \
    __builtin_ia32_scattersiv4df((void *)(addr), (__mmask8)(mask), \
                                 (__v4si)(__m128i)(index), \
                                 (__v4df)(__m256d)(v1), (int)(scale))

#define _mm256_i32scatter_epi64(addr, index, v1, scale) \
    __builtin_ia32_scattersiv4di((void *)(addr), (__mmask8)-1, \
                                 (__v4si)(__m128i)(index), \
                                 (__v4di)(__m256i)(v1), (int)(scale))

#define _mm256_mask_i32scatter_epi64(addr, mask, index, v1, scale) \
    __builtin_ia32_scattersiv4di((void *)(addr), (__mmask8)(mask), \
                                 (__v4si)(__m128i)(index), \
                                 (__v4di)(__m256i)(v1), (int)(scale))

#define _mm_i32scatter_ps(addr, index, v1, scale) \
    __builtin_ia32_scattersiv4sf((void *)(addr), (__mmask8)-1, \
                                 (__v4si)(__m128i)(index), (__v4sf)(__m128)(v1), \
                                 (int)(scale))

#define _mm_mask_i32scatter_ps(addr, mask, index, v1, scale) \
    __builtin_ia32_scattersiv4sf((void *)(addr), (__mmask8)(mask), \
                                 (__v4si)(__m128i)(index), (__v4sf)(__m128)(v1), \
                                 (int)(scale))

#define _mm_i32scatter_epi32(addr, index, v1, scale) \
    __builtin_ia32_scattersiv4si((void *)(addr), (__mmask8)-1, \
                                 (__v4si)(__m128i)(index), \
                                 (__v4si)(__m128i)(v1), (int)(scale))

#define _mm_mask_i32scatter_epi32(addr, mask, index, v1, scale) \
    __builtin_ia32_scattersiv4si((void *)(addr), (__mmask8)(mask), \
                                 (__v4si)(__m128i)(index), \
                                 (__v4si)(__m128i)(v1), (int)(scale))

#define _mm256_i32scatter_ps(addr, index, v1, scale) \
    __builtin_ia32_scattersiv8sf((void *)(addr), (__mmask8)-1, \
                                 (__v8si)(__m256i)(index), (__v8sf)(__m256)(v1), \
                                 (int)(scale))

#define _mm256_mask_i32scatter_ps(addr, mask, index, v1, scale) \
    __builtin_ia32_scattersiv8sf((void *)(addr), (__mmask8)(mask), \
                                 (__v8si)(__m256i)(index), (__v8sf)(__m256)(v1), \
                                 (int)(scale))

#define _mm256_i32scatter_epi32(addr, index, v1, scale) \
    __builtin_ia32_scattersiv8si((void *)(addr), (__mmask8)-1, \
                                 (__v8si)(__m256i)(index), \
                                 (__v8si)(__m256i)(v1), (int)(scale))

#define _mm256_mask_i32scatter_epi32(addr, mask, index, v1, scale) \
    __builtin_ia32_scattersiv8si((void *)(addr), (__mmask8)(mask), \
                                 (__v8si)(__m256i)(index), \
                                 (__v8si)(__m256i)(v1), (int)(scale))

  static __inline__ __m128d __DEFAULT_FN_ATTRS128
  _mm_mask_sqrt_pd(__m128d __W, __mmask8 __U, __m128d __A) {
    return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                                (__v2df)_mm_sqrt_pd(__A),
                                                (__v2df)__W);
  }

  static __inline__ __m128d __DEFAULT_FN_ATTRS128
  _mm_maskz_sqrt_pd(__mmask8 __U, __m128d __A) {
    return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                                (__v2df)_mm_sqrt_pd(__A),
                                                (__v2df)_mm_setzero_pd());
  }

  static __inline__ __m256d __DEFAULT_FN_ATTRS256
  _mm256_mask_sqrt_pd(__m256d __W, __mmask8 __U, __m256d __A) {
    return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                                (__v4df)_mm256_sqrt_pd(__A),
                                                (__v4df)__W);
  }

  static __inline__ __m256d __DEFAULT_FN_ATTRS256
  _mm256_maskz_sqrt_pd(__mmask8 __U, __m256d __A) {
    return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                                (__v4df)_mm256_sqrt_pd(__A),
                                                (__v4df)_mm256_setzero_pd());
  }

  static __inline__ __m128 __DEFAULT_FN_ATTRS128
  _mm_mask_sqrt_ps(__m128 __W, __mmask8 __U, __m128 __A) {
    return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                               (__v4sf)_mm_sqrt_ps(__A),
                                               (__v4sf)__W);
  }

  static __inline__ __m128 __DEFAULT_FN_ATTRS128
  _mm_maskz_sqrt_ps(__mmask8 __U, __m128 __A) {
    return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                               (__v4sf)_mm_sqrt_ps(__A),
                                               (__v4sf)_mm_setzero_ps());
  }

  static __inline__ __m256 __DEFAULT_FN_ATTRS256
  _mm256_mask_sqrt_ps(__m256 __W, __mmask8 __U, __m256 __A) {
    return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                               (__v8sf)_mm256_sqrt_ps(__A),
                                               (__v8sf)__W);
  }

  static __inline__ __m256 __DEFAULT_FN_ATTRS256
  _mm256_maskz_sqrt_ps(__mmask8 __U, __m256 __A) {
    return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                               (__v8sf)_mm256_sqrt_ps(__A),
                                               (__v8sf)_mm256_setzero_ps());
  }

  static __inline__ __m128d __DEFAULT_FN_ATTRS128
  _mm_mask_sub_pd(__m128d __W, __mmask8 __U, __m128d __A, __m128d __B) {
    return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                                (__v2df)_mm_sub_pd(__A, __B),
                                                (__v2df)__W);
  }

  static __inline__ __m128d __DEFAULT_FN_ATTRS128
  _mm_maskz_sub_pd(__mmask8 __U, __m128d __A, __m128d __B) {
    return (__m128d)__builtin_ia32_selectpd_128((__mmask8)__U,
                                                (__v2df)_mm_sub_pd(__A, __B),
                                                (__v2df)_mm_setzero_pd());
  }

  static __inline__ __m256d __DEFAULT_FN_ATTRS256
  _mm256_mask_sub_pd(__m256d __W, __mmask8 __U, __m256d __A, __m256d __B) {
    return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                                (__v4df)_mm256_sub_pd(__A, __B),
                                                (__v4df)__W);
  }

  static __inline__ __m256d __DEFAULT_FN_ATTRS256
  _mm256_maskz_sub_pd(__mmask8 __U, __m256d __A, __m256d __B) {
    return (__m256d)__builtin_ia32_selectpd_256((__mmask8)__U,
                                                (__v4df)_mm256_sub_pd(__A, __B),
                                                (__v4df)_mm256_setzero_pd());
  }

  static __inline__ __m128 __DEFAULT_FN_ATTRS128
  _mm_mask_sub_ps(__m128 __W, __mmask8 __U, __m128 __A, __m128 __B) {
    return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                               (__v4sf)_mm_sub_ps(__A, __B),
                                               (__v4sf)__W);
  }

  static __inline__ __m128 __DEFAULT_FN_ATTRS128
  _mm_maskz_sub_ps(__mmask8 __U, __m128 __A, __m128 __B) {
    return (__m128)__builtin_ia32_selectps_128((__mmask8)__U,
                                               (__v4sf)_mm_sub_ps(__A, __B),
                                               (__v4sf)_mm_setzero_ps());
  }

  static __inline__ __m256 __DEFAULT_FN_ATTRS256
  _mm256_mask_sub_ps(__m256 __W, __mmask8 __U, __m256 __A, __m256 __B) {
    return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                               (__v8sf)_mm256_sub_ps(__A, __B),
                                               (__v8sf)__W);
  }

  static __inline__ __m256 __DEFAULT_FN_ATTRS256
  _mm256_maskz_sub_ps(__mmask8 __U, __m256 __A, __m256 __B) {
    return (__m256)__builtin_ia32_selectps_256((__mmask8)__U,
                                               (__v8sf)_mm256_sub_ps(__A, __B),
                                               (__v8sf)_mm256_setzero_ps());
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_permutex2var_epi32(__m128i __A, __m128i __I, __m128i __B) {
    return (__m128i)__builtin_ia32_vpermi2vard128((__v4si) __A, (__v4si)__I,
                                                  (__v4si)__B);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_permutex2var_epi32(__m128i __A, __mmask8 __U, __m128i __I,
                              __m128i __B) {
    return (__m128i)__builtin_ia32_selectd_128(__U,
                                    (__v4si)_mm_permutex2var_epi32(__A, __I, __B),
                                    (__v4si)__A);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask2_permutex2var_epi32(__m128i __A, __m128i __I, __mmask8 __U,
                               __m128i __B) {
    return (__m128i)__builtin_ia32_selectd_128(__U,
                                    (__v4si)_mm_permutex2var_epi32(__A, __I, __B),
                                    (__v4si)__I);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_maskz_permutex2var_epi32(__mmask8 __U, __m128i __A, __m128i __I,
                               __m128i __B) {
    return (__m128i)__builtin_ia32_selectd_128(__U,
                                    (__v4si)_mm_permutex2var_epi32(__A, __I, __B),
                                    (__v4si)_mm_setzero_si128());
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_permutex2var_epi32(__m256i __A, __m256i __I, __m256i __B) {
    return (__m256i)__builtin_ia32_vpermi2vard256((__v8si)__A, (__v8si) __I,
                                                  (__v8si) __B);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask_permutex2var_epi32(__m256i __A, __mmask8 __U, __m256i __I,
                                 __m256i __B) {
    return (__m256i)__builtin_ia32_selectd_256(__U,
                                 (__v8si)_mm256_permutex2var_epi32(__A, __I, __B),
                                 (__v8si)__A);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask2_permutex2var_epi32(__m256i __A, __m256i __I, __mmask8 __U,
                                  __m256i __B) {
    return (__m256i)__builtin_ia32_selectd_256(__U,
                                 (__v8si)_mm256_permutex2var_epi32(__A, __I, __B),
                                 (__v8si)__I);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_maskz_permutex2var_epi32(__mmask8 __U, __m256i __A, __m256i __I,
                                  __m256i __B) {
    return (__m256i)__builtin_ia32_selectd_256(__U,
                                 (__v8si)_mm256_permutex2var_epi32(__A, __I, __B),
                                 (__v8si)_mm256_setzero_si256());
  }

  static __inline__ __m128d __DEFAULT_FN_ATTRS128
  _mm_permutex2var_pd(__m128d __A, __m128i __I, __m128d __B) {
    return (__m128d)__builtin_ia32_vpermi2varpd128((__v2df)__A, (__v2di)__I,
                                                   (__v2df)__B);
  }

  static __inline__ __m128d __DEFAULT_FN_ATTRS128
  _mm_mask_permutex2var_pd(__m128d __A, __mmask8 __U, __m128i __I, __m128d __B) {
    return (__m128d)__builtin_ia32_selectpd_128(__U,
                                       (__v2df)_mm_permutex2var_pd(__A, __I, __B),
                                       (__v2df)__A);
  }

  static __inline__ __m128d __DEFAULT_FN_ATTRS128
  _mm_mask2_permutex2var_pd(__m128d __A, __m128i __I, __mmask8 __U, __m128d __B) {
    return (__m128d)__builtin_ia32_selectpd_128(__U,
                                       (__v2df)_mm_permutex2var_pd(__A, __I, __B),
                                       (__v2df)(__m128d)__I);
  }

  static __inline__ __m128d __DEFAULT_FN_ATTRS128
  _mm_maskz_permutex2var_pd(__mmask8 __U, __m128d __A, __m128i __I, __m128d __B) {
    return (__m128d)__builtin_ia32_selectpd_128(__U,
                                       (__v2df)_mm_permutex2var_pd(__A, __I, __B),
                                       (__v2df)_mm_setzero_pd());
  }

  static __inline__ __m256d __DEFAULT_FN_ATTRS256
  _mm256_permutex2var_pd(__m256d __A, __m256i __I, __m256d __B) {
    return (__m256d)__builtin_ia32_vpermi2varpd256((__v4df)__A, (__v4di)__I,
                                                   (__v4df)__B);
  }

  static __inline__ __m256d __DEFAULT_FN_ATTRS256
  _mm256_mask_permutex2var_pd(__m256d __A, __mmask8 __U, __m256i __I,
                              __m256d __B) {
    return (__m256d)__builtin_ia32_selectpd_256(__U,
                                    (__v4df)_mm256_permutex2var_pd(__A, __I, __B),
                                    (__v4df)__A);
  }

  static __inline__ __m256d __DEFAULT_FN_ATTRS256
  _mm256_mask2_permutex2var_pd(__m256d __A, __m256i __I, __mmask8 __U,
                               __m256d __B) {
    return (__m256d)__builtin_ia32_selectpd_256(__U,
                                    (__v4df)_mm256_permutex2var_pd(__A, __I, __B),
                                    (__v4df)(__m256d)__I);
  }

  static __inline__ __m256d __DEFAULT_FN_ATTRS256
  _mm256_maskz_permutex2var_pd(__mmask8 __U, __m256d __A, __m256i __I,
                               __m256d __B) {
    return (__m256d)__builtin_ia32_selectpd_256(__U,
                                    (__v4df)_mm256_permutex2var_pd(__A, __I, __B),
                                    (__v4df)_mm256_setzero_pd());
  }

  static __inline__ __m128 __DEFAULT_FN_ATTRS128
  _mm_permutex2var_ps(__m128 __A, __m128i __I, __m128 __B) {
    return (__m128)__builtin_ia32_vpermi2varps128((__v4sf)__A, (__v4si)__I,
                                                  (__v4sf)__B);
  }

  static __inline__ __m128 __DEFAULT_FN_ATTRS128
  _mm_mask_permutex2var_ps(__m128 __A, __mmask8 __U, __m128i __I, __m128 __B) {
    return (__m128)__builtin_ia32_selectps_128(__U,
                                       (__v4sf)_mm_permutex2var_ps(__A, __I, __B),
                                       (__v4sf)__A);
  }

  static __inline__ __m128 __DEFAULT_FN_ATTRS128
  _mm_mask2_permutex2var_ps(__m128 __A, __m128i __I, __mmask8 __U, __m128 __B) {
    return (__m128)__builtin_ia32_selectps_128(__U,
                                       (__v4sf)_mm_permutex2var_ps(__A, __I, __B),
                                       (__v4sf)(__m128)__I);
  }

  static __inline__ __m128 __DEFAULT_FN_ATTRS128
  _mm_maskz_permutex2var_ps(__mmask8 __U, __m128 __A, __m128i __I, __m128 __B) {
    return (__m128)__builtin_ia32_selectps_128(__U,
                                       (__v4sf)_mm_permutex2var_ps(__A, __I, __B),
                                       (__v4sf)_mm_setzero_ps());
  }

  static __inline__ __m256 __DEFAULT_FN_ATTRS256
  _mm256_permutex2var_ps(__m256 __A, __m256i __I, __m256 __B) {
    return (__m256)__builtin_ia32_vpermi2varps256((__v8sf)__A, (__v8si)__I,
                                                  (__v8sf) __B);
  }

  static __inline__ __m256 __DEFAULT_FN_ATTRS256
  _mm256_mask_permutex2var_ps(__m256 __A, __mmask8 __U, __m256i __I, __m256 __B) {
    return (__m256)__builtin_ia32_selectps_256(__U,
                                    (__v8sf)_mm256_permutex2var_ps(__A, __I, __B),
                                    (__v8sf)__A);
  }

  static __inline__ __m256 __DEFAULT_FN_ATTRS256
  _mm256_mask2_permutex2var_ps(__m256 __A, __m256i __I, __mmask8 __U,
                               __m256 __B) {
    return (__m256)__builtin_ia32_selectps_256(__U,
                                    (__v8sf)_mm256_permutex2var_ps(__A, __I, __B),
                                    (__v8sf)(__m256)__I);
  }

  static __inline__ __m256 __DEFAULT_FN_ATTRS256
  _mm256_maskz_permutex2var_ps(__mmask8 __U, __m256 __A, __m256i __I,
                               __m256 __B) {
    return (__m256)__builtin_ia32_selectps_256(__U,
                                    (__v8sf)_mm256_permutex2var_ps(__A, __I, __B),
                                    (__v8sf)_mm256_setzero_ps());
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_permutex2var_epi64(__m128i __A, __m128i __I, __m128i __B) {
    return (__m128i)__builtin_ia32_vpermi2varq128((__v2di)__A, (__v2di)__I,
                                                  (__v2di)__B);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_permutex2var_epi64(__m128i __A, __mmask8 __U, __m128i __I,
                              __m128i __B) {
    return (__m128i)__builtin_ia32_selectq_128(__U,
                                    (__v2di)_mm_permutex2var_epi64(__A, __I, __B),
                                    (__v2di)__A);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask2_permutex2var_epi64(__m128i __A, __m128i __I, __mmask8 __U,
                               __m128i __B) {
    return (__m128i)__builtin_ia32_selectq_128(__U,
                                    (__v2di)_mm_permutex2var_epi64(__A, __I, __B),
                                    (__v2di)__I);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_maskz_permutex2var_epi64(__mmask8 __U, __m128i __A, __m128i __I,
                               __m128i __B) {
    return (__m128i)__builtin_ia32_selectq_128(__U,
                                    (__v2di)_mm_permutex2var_epi64(__A, __I, __B),
                                    (__v2di)_mm_setzero_si128());
  }


  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_permutex2var_epi64(__m256i __A, __m256i __I, __m256i __B) {
    return (__m256i)__builtin_ia32_vpermi2varq256((__v4di)__A, (__v4di) __I,
                                                  (__v4di) __B);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask_permutex2var_epi64(__m256i __A, __mmask8 __U, __m256i __I,
                                 __m256i __B) {
    return (__m256i)__builtin_ia32_selectq_256(__U,
                                 (__v4di)_mm256_permutex2var_epi64(__A, __I, __B),
                                 (__v4di)__A);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask2_permutex2var_epi64(__m256i __A, __m256i __I, __mmask8 __U,
                                  __m256i __B) {
    return (__m256i)__builtin_ia32_selectq_256(__U,
                                 (__v4di)_mm256_permutex2var_epi64(__A, __I, __B),
                                 (__v4di)__I);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_maskz_permutex2var_epi64(__mmask8 __U, __m256i __A, __m256i __I,
                                  __m256i __B) {
    return (__m256i)__builtin_ia32_selectq_256(__U,
                                 (__v4di)_mm256_permutex2var_epi64(__A, __I, __B),
                                 (__v4di)_mm256_setzero_si256());
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_cvtepi8_epi32(__m128i __W, __mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                               (__v4si)_mm_cvtepi8_epi32(__A),
                                               (__v4si)__W);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_maskz_cvtepi8_epi32(__mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                               (__v4si)_mm_cvtepi8_epi32(__A),
                                               (__v4si)_mm_setzero_si128());
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask_cvtepi8_epi32 (__m256i __W, __mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                               (__v8si)_mm256_cvtepi8_epi32(__A),
                                               (__v8si)__W);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_maskz_cvtepi8_epi32 (__mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                               (__v8si)_mm256_cvtepi8_epi32(__A),
                                               (__v8si)_mm256_setzero_si256());
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_cvtepi8_epi64(__m128i __W, __mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                               (__v2di)_mm_cvtepi8_epi64(__A),
                                               (__v2di)__W);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_maskz_cvtepi8_epi64(__mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                               (__v2di)_mm_cvtepi8_epi64(__A),
                                               (__v2di)_mm_setzero_si128());
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask_cvtepi8_epi64(__m256i __W, __mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                               (__v4di)_mm256_cvtepi8_epi64(__A),
                                               (__v4di)__W);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_maskz_cvtepi8_epi64(__mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                               (__v4di)_mm256_cvtepi8_epi64(__A),
                                               (__v4di)_mm256_setzero_si256());
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_cvtepi32_epi64(__m128i __W, __mmask8 __U, __m128i __X)
  {
    return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                               (__v2di)_mm_cvtepi32_epi64(__X),
                                               (__v2di)__W);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_maskz_cvtepi32_epi64(__mmask8 __U, __m128i __X)
  {
    return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                               (__v2di)_mm_cvtepi32_epi64(__X),
                                               (__v2di)_mm_setzero_si128());
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask_cvtepi32_epi64(__m256i __W, __mmask8 __U, __m128i __X)
  {
    return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                               (__v4di)_mm256_cvtepi32_epi64(__X),
                                               (__v4di)__W);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_maskz_cvtepi32_epi64(__mmask8 __U, __m128i __X)
  {
    return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                               (__v4di)_mm256_cvtepi32_epi64(__X),
                                               (__v4di)_mm256_setzero_si256());
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_cvtepi16_epi32(__m128i __W, __mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                               (__v4si)_mm_cvtepi16_epi32(__A),
                                               (__v4si)__W);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_maskz_cvtepi16_epi32(__mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                               (__v4si)_mm_cvtepi16_epi32(__A),
                                               (__v4si)_mm_setzero_si128());
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask_cvtepi16_epi32(__m256i __W, __mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                               (__v8si)_mm256_cvtepi16_epi32(__A),
                                               (__v8si)__W);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_maskz_cvtepi16_epi32 (__mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                               (__v8si)_mm256_cvtepi16_epi32(__A),
                                               (__v8si)_mm256_setzero_si256());
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_cvtepi16_epi64(__m128i __W, __mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                               (__v2di)_mm_cvtepi16_epi64(__A),
                                               (__v2di)__W);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_maskz_cvtepi16_epi64(__mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                               (__v2di)_mm_cvtepi16_epi64(__A),
                                               (__v2di)_mm_setzero_si128());
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask_cvtepi16_epi64(__m256i __W, __mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                               (__v4di)_mm256_cvtepi16_epi64(__A),
                                               (__v4di)__W);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_maskz_cvtepi16_epi64(__mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                               (__v4di)_mm256_cvtepi16_epi64(__A),
                                               (__v4di)_mm256_setzero_si256());
  }


  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_cvtepu8_epi32(__m128i __W, __mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                               (__v4si)_mm_cvtepu8_epi32(__A),
                                               (__v4si)__W);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_maskz_cvtepu8_epi32(__mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectd_128((__mmask8)__U,
                                               (__v4si)_mm_cvtepu8_epi32(__A),
                                               (__v4si)_mm_setzero_si128());
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask_cvtepu8_epi32(__m256i __W, __mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                               (__v8si)_mm256_cvtepu8_epi32(__A),
                                               (__v8si)__W);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_maskz_cvtepu8_epi32(__mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectd_256((__mmask8)__U,
                                               (__v8si)_mm256_cvtepu8_epi32(__A),
                                               (__v8si)_mm256_setzero_si256());
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_cvtepu8_epi64(__m128i __W, __mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                               (__v2di)_mm_cvtepu8_epi64(__A),
                                               (__v2di)__W);
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_maskz_cvtepu8_epi64(__mmask8 __U, __m128i __A)
  {
    return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,
                                               (__v2di)_mm_cvtepu8_epi64(__A),
                                               (__v2di)_mm_setzero_si128());
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_mask_cvtepu8_epi64(__m256i __W, __mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                               (__v4di)_mm256_cvtepu8_epi64(__A),
                                               (__v4di)__W);
  }

  static __inline__ __m256i __DEFAULT_FN_ATTRS256
  _mm256_maskz_cvtepu8_epi64 (__mmask8 __U, __m128i __A)
  {
    return (__m256i)__builtin_ia32_selectq_256((__mmask8)__U,
                                               (__v4di)_mm256_cvtepu8_epi64(__A),
                                               (__v4di)_mm256_setzero_si256());
  }

  static __inline__ __m128i __DEFAULT_FN_ATTRS128
  _mm_mask_cvtepu32_epi64(__m128i __W, __mmask8 __U, __m128i __X)
  {
    return (__m128i)__builtin_ia32_selectq_128((__mmask8)__U,