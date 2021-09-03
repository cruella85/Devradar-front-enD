/*===--- __clang_cuda_intrinsics.h - Device-side CUDA intrinsic wrappers ---===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */
#ifndef __CLANG_CUDA_INTRINSICS_H__
#define __CLANG_CUDA_INTRINSICS_H__
#ifndef __CUDA__
#error "This file is for CUDA compilation only."
#endif

// sm_30 intrinsics: __shfl_{up,down,xor}.

#define __SM_30_INTRINSICS_H__
#define __SM_30_INTRINSICS_HPP__

#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 300

#pragma push_macro("__MAKE_SHUFFLES")
#define __MAKE_SHUFFLES(__FnName, __IntIntrinsic, __FloatIntrinsic, __Mask,    \
                        __Type)                                                \
  inline __device__ int __FnName(int __val, __Type __offset,                   \
                                 int __width = warpSize) {                     \
    return __IntIntrinsic(__val, __offset,                                     \
                          ((warpSize - __width) << 8) | (__Mask));             \
  }                                                                            \
  inline __device__ float __FnName(float __val, __Type __offset,               \
                                   int __width = warpSize) {                   \
    return __FloatIntrinsic(__val, __offset,                                   \
                            ((warpSize - __width) << 8) | (__Mask));           \
  }                                                                            \
  inline __device__ unsigned int __FnName(unsigned int __val, __Type __offset, \
                                          int __width = warpSize) {            \
    return static_cast<unsigned int>(                                          \
        ::__FnName(static_cast<int>(__val), __offset, __width));               \
  }                                                                            \
  inline __device__ long long __FnName(long long __val, __Type __offset,       \
                                       int __width = warpSize) {               \
    struct __Bits {                                                            \
      int __a, __b;                                                            \
    };                                                                         \
    _Static_assert(sizeof(__val) == sizeof(__Bits));                           \
    _Static_assert(sizeof(__Bits) == 2 * sizeof(int));                         \
    __Bits __tmp;                                                              \
    memcpy(&__tmp, &__val, sizeof(__val));                                \
    __tmp.__a = ::__FnName(__tmp.__a, __offset, __width);                      \
    __tmp.__b = ::__FnName(__tmp.__b, __offset, __width);                      \
    long long __ret;                                                           \
    memcpy(&__ret, &__tmp, sizeof(__tmp));                                     \
    return __ret;                                                              \
  }                                                                            \
  inline __device__ long __FnName(long __val, __Type __offset,                 \
                                  int __width = warpSize) {                    \
    _Static_assert(sizeof(long) == sizeof(long long) ||                        \
                   sizeof(long) == sizeof(int));                               \
    if (sizeof(long) == sizeof(long long)) {                                   \
      return static_cast<long>(                                                \
          ::__FnName(static_cast<long long>(__val), __offset, __width));       \
    } else if (sizeof(long) == sizeof(int)) {                                  \
      return static_cast<long>(                                                \
          ::__FnName(static_cast<int>(__val), __offset, __width));             \
    }                                                                          \
  }                                                                            \
  inline __device__ unsigned long __FnName(                                    \
      unsigned long __val, __Type __offset, int __width = warpSize) {          \
    return static_cast<unsigned long>(                                         \
        ::__FnName(static_cast<long>(__val), __offset, __width));              \
  }                                                                            \
  inline __device__ unsigned long long __FnName(                               \
      unsigned long long __val, __Type __offset, int __width = warpSize) {     \
    return static_cast<unsigned long long>(                                    \
        ::__FnName(static_cast<long long>(__val), __offset, __width));         \
  }                                                                            \
  inline __device__ double __FnName(double __val, __Type __offset,             \
                                    int __width = warpSize) {                  \
    long long __tmp;                                                           \
    _Static_assert(sizeof(__tmp) == sizeof(__val));                            \
    memcpy(&__tmp, &__val, sizeof(__val));                                     \
    __tmp = ::__FnName(__tmp, __offset, __width);                              \
    double __ret;                                                              \
    memcpy(&__ret, &__tmp, sizeof(__ret));                                     \
    return __ret;                                                              \
  }

__MAKE_SHUFFLES(__shfl, __nvvm_shfl_idx_i32, __nvvm_shfl_idx_f32, 0x1f, int);
// We use 0 rather than 31 as our mask, because shfl.up applies to lanes >=
// maxLane.
__MAKE_SHUFFLES(__shfl_up, __nvvm_shfl_up_i32, __nvvm_shfl_up_f32, 0,
                unsigned int);
__MAKE_SHUFFLES(__shfl_down, __nvvm_shfl_down_i32, __nvvm_shfl_down_f32, 0x1f,
                unsigned int);
__MAKE_SHUFFLES(__shfl_xor, __nvvm_shfl_bfly_i32, __nvvm_shfl_bfly_f32, 0x1f,
                int);
#pragma pop_macro("__MAKE_SHUFFLES")

#endif // !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 300

#if CUDA_VERSION >= 9000
#if (!defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 300)
// __shfl_sync_* variants available in CUDA-9
#pragma push_macro("__MAKE_SYNC_SHUFFLES")
#define __MAKE_SYNC_SHUFFLES(__FnName, __IntIntrinsic, __FloatIntrinsic,       \
                             __Mask, __Type)                                   \
  inline __device__ int __FnName(unsigned int __mask, int __val,               \
                                 __Type __offset, int __width = warpSize) {    \
    return __IntIntrinsic(__mask, __val, __offset,                             \
                          ((warpSize - __width) << 8) | (__Mask));             \
  }                                                                            \
  inline __device__ float __FnName(unsigned int __mask, float __val,           \
                                   __Type __offset, int __width = warpSize) {  \
    return __FloatIntrinsic(__mask, __val, __offset,                           \
                            ((warpSize - __width) << 8) | (__Mask));           \
  }                                                                            \
  inline __device__ unsigned int __FnName(unsigned int __mask,                 \
                                          unsigned int __val, __Type __offset, \
                                          int __width = warpSize) {            \
    return static_cast<unsigned int>(                                          \
        ::__FnName(__mask, static_cast<int>(__val), __offset, __width));       \
  }                                                                            \
  inline __device__ long long __FnName(unsigned int __mask, long long __val,   \
                                       __Type __offset,                        \
                                       int __width = warpSize) {               \
    struct __Bits {                                                            \
      int __a, __b;                                                            \
    };                                                                         \
    _Static_assert(sizeof(__val) == sizeof(__Bits));                           \
    _Static_assert(sizeof(__Bits) == 2 * sizeof(int));                         \
    __Bits __tmp;                                                              \
    memcpy(&__tmp, &__val, sizeof(__val));                                     \
    __tmp.__a = ::__FnName(__mask, __tmp.__a, __offset, __width);              \
    __tmp.__b = ::__FnName(__mask, __tmp.__b, __offset, __width);              \
    long long __ret;                                                           \
    memcpy(&__ret, &__tmp, sizeof(__tmp));                                     \
    return __ret;                                                              \
  }                                                                            \
  inline __device__ unsigned long long __FnName(                               \
      unsigned int __mask, unsigned long long __val, __Type __offset,          \
      int __width = warpSize) {                                                \
    return static_cast<unsigned long long>(                                    \
        ::__FnName(__mask, static_cast<long long>(__val), __offset, __width)); \
  }                                                                            \
  inline __device__ long __FnName(unsigned int __mask, long __val,             \
                                  __Type __offset, int __width = warpSize) {   \
    _Static_assert(sizeof(long) == sizeof(long long) ||                        \
                   sizeof(long) == sizeof(int));                               \
    if (sizeof(long) == sizeof(long long)) {                                   \
      return static_cast<long>(::__FnName(                                     \
          __mask, static_cast<long long>(__val), __offset, __width));          \
    } else if (sizeof(long) == sizeof(int)) {                                  \
      return static_cast<long>(                                                \
          ::__FnName(__mask, static_cast<int>(__val), __offset, __width));     \
    }                                                                          \
  }                                                                            \
  inline __device__ unsigned long __FnName(                                    \
      unsigned int __mask, unsigned long __val, __Type __offset,               \
      int __width = warpSize) {                                                \
    return static_cast<unsigned long>(                                         \
        ::__FnName(__mask, static_cast<long>(__val), __offset, __width));      \
  }                                                                            \
  inline __device__ double __FnName(unsigned int __mask, double __val,         \
                                    __Type __offset, int __width = warpSize) { \
    long long __tmp;                                                           \
    _Static_assert(sizeof(__tmp) == sizeof(__val));                            \
    memcpy(&__tmp, &__val, sizeof(__val));                                     \
    __tmp = ::__FnName(__mask, __tmp, __offset, __width);                      \
    double __ret;                                                              \
    memcpy(&__ret, &__tmp, sizeof(__ret));                                     \
    return __ret;                                                              \
  }
__MAKE_SYNC_SHUFFLES(__shfl_sync, __nvvm_shfl_sync_idx_i32,
                     __nvvm_shfl_sync_idx_f32, 0x1f, int);
// We use 0 rather than 31 as our mask, because shfl.up applies to lanes >=
// maxLane.
__MAKE_SYNC_SHUFFLES(__shfl_up_sync, __nvvm_shfl_sync_up_i32,
                     __nvvm_shfl_sync_up_f32, 0, unsigned int);
__MAKE_SYNC_SHUFFLES(__shfl_down_sync, __nvvm_shfl_sync_down_i32,
                     __nvvm_shfl_sync_down_f32, 0x1f, unsigned int);
__MAKE_SYNC_SHUFFLES(__shfl_xor_sync, __nvvm_shfl_sync_bfly_i32,
                     __nvvm_shfl_sync_bfly_f32, 0x1f, int);
#pragma pop_macro("__MAKE_SYNC_SHUFFLES")

inline __device__ void __syncwarp(unsigned int mask = 0xffffffff) {
  return __nvvm_bar_warp_sync(mask);
}

inline __device__ void __barrier_sync(unsigned int id) {
  __nvvm_barrier_sync(id);
}

inline __device__ void __barrier_sync_count(unsigned int id,
                                            unsigned int count) {
  __nvvm_barrier_sync_cnt(id, count);
}

inline __device__ int __all_sync(unsigned int mask, int pred) {
  return __nvvm_vote_all_sync(mask, pred);
}

inline __device__ int __any_sync(unsigned int mask, int pred) {
  return __nvvm_vote_any_sync(mask, pred);
}

inline __device__ int __uni_sync(unsigned int mask, int pred) {
  return __nvvm_vote_uni_sync(mask, pred);
}

inline __device__ unsigned int __ballot_sync(unsigned int mask, int pred) {
  return __nvvm_vote_ballot_sync(mask, pred);
}

inline __device__ unsigned int __activemask() {
#if CUDA_VERSION < 9020
  return __nvvm_vote_ballot(1);
#else
  unsigned int mask;
  asm volatile("activemask.b32 %0;" : "=r"(mask));
  return mask;
#endif
}

inline __device__ unsigned int __fns(unsigned mask, unsigned base, int offset) {
  return __nvvm_fns(mask, base, offset);
}

#endif // !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 300

// Define __match* builtins CUDA-9 headers expect to see.
#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 700
inline __device__ unsigned int __match32_any_sync(unsigned int mask,
                                                  unsigned int value) {
  return __nvvm_match_any_sync_i32(mask, value);
