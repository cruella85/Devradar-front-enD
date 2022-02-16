/*! @header
 *  This header defines fixed size vector types with relaxed alignment. For 
 *  each vector type defined by <simd/vector_types.h> that is not a 1- or 3-
 *  element vector, there is a corresponding type defined by this header that
 *  requires only the alignment matching that of the underlying scalar type.
 *
 *  These types should be used to access buffers that may not be sufficiently
 *  aligned to allow them to be accessed using the "normal" simd vector types.
 *  As an example of this usage, suppose that you want to load a vector of
 *  four floats from an array of floats. The type simd_float4 has sixteen byte
 *  alignment, whereas an array of floats has only four byte alignment.
 *  Thus, naively casting a pointer into the array to (simd_float4 *) would 
 *  invoke undefined behavior, and likely produce an alignment fault at
 *  runtime. Instead, use the corresponding packed type to load from the array:
 *
 *  <pre>
 *  @textblock
 *  simd_float4 vector = *(simd_packed_float4 *)&array[i];
 *  // do something with vector ...
 *  @/textblock
 *  </pre>
 *
 *  It's important to note that the packed_ types are only needed to work with
 *  memory; once the data is loaded, we simply operate on it as usual using
 *  the simd_float4 type, as illustrated above.
 *
 *  @copyright 2014-2017 Apple, Inc. All rights reserved.
 *  @unsorted                                                                 */

#ifndef SIMD_PACKED_TYPES
#define SIMD_PACKED_TYPES

# include <simd/vector_types.h>
# if SIMD_COMPILER_HAS_REQUIRED_FEATURES
/*! @abstract A vector of two 8-bit signed (twos-complement) integers with
 *  relaxed alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::char2. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(2),__aligned__(1))) char simd_packed_char2;

/*! @abstract A vector of four 8-bit signed (twos-complement) integers with
 *  relaxed alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::char4. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(4),__aligned__(1))) char simd_packed_char4;

/*! @abstract A vector of eight 8-bit signed (twos-complement) integers with
 *  relaxed alignment.
 *  @description In C++ this type is also available as simd::packed::char8.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(8),__aligned__(1))) char simd_packed_char8;

/*! @abstract A vector of sixteen 8-bit signed (twos-complement) integers
 *  with relaxed alignment.
 *  @description In C++ this type is also available as simd::packed::char16.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(16),__aligned__(1))) char simd_packed_char16;

/*! @abstract A vector of thirty-two 8-bit signed (twos-complement) integers
 *  with relaxed alignment.
 *  @description In C++ this type is also available as simd::packed::char32.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(32),__aligned__(1))) char simd_packed_char32;

/*! @abstract A vector of sixty-four 8-bit signed (twos-complement) integers
 *  with relaxed alignment.
 *  @description In C++ this type is also available as simd::packed::char64.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(64),__aligned__(1))) char simd_packed_char64;

/*! @abstract A vector of two 8-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::uchar2. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(2),__aligned__(1))) unsigned char simd_packed_uchar2;

/*! @abstract A vector of four 8-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::uchar4. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(4),__aligned__(1))) unsigned char simd_packed_uchar4;

/*! @abstract A vector of eight 8-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as simd::packed::uchar8.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(8),__aligned__(1))) unsigned char simd_packed_uchar8;

/*! @abstract A vector of sixteen 8-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::uchar16. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
typedef __attribute__((__ext_vector_type__(16),__aligned__(1))) unsigned char simd_packed_uchar16;

/*! @abstract A vector of thirty-two 8-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::uchar32. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
typedef __attribute__((__ext_vector_type__(32),__aligned__(1))) unsigned char simd_packed_uchar32;

/*! @abstract A vector of sixty-four 8-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::uchar64. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
typedef __attribute__((__ext_vector_type__(64),__aligned__(1))) unsigned char simd_packed_uchar64;

/*! @abstract A vector of two 16-bit signed (twos-complement) integers with
 *  relaxed alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::short2. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(2),__aligned__(2))) short simd_packed_short2;

/*! @abstract A vector of four 16-bit signed (twos-complement) integers with
 *  relaxed alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::short4. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(4),__aligned__(2))) short simd_packed_short4;

/*! @abstract A vector of eight 16-bit signed (twos-complement) integers
 *  with relaxed alignment.
 *  @description In C++ this type is also available as simd::packed::short8.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(8),__aligned__(2))) short simd_packed_short8;

/*! @abstract A vector of sixteen 16-bit signed (twos-complement) integers
 *  with relaxed alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::short16. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
typedef __attribute__((__ext_vector_type__(16),__aligned__(2))) short simd_packed_short16;

/*! @abstract A vector of thirty-two 16-bit signed (twos-complement)
 *  integers with relaxed alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::short32. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
typedef __attribute__((__ext_vector_type__(32),__aligned__(2))) short simd_packed_short32;

/*! @abstract A vector of two 16-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::ushort2. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(2),__aligned__(2))) unsigned short simd_packed_ushort2;

/*! @abstract A vector of four 16-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::ushort4. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(4),__aligned__(2))) unsigned short simd_packed_ushort4;

/*! @abstract A vector of eight 16-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::ushort8. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
typedef __attribute__((__ext_vector_type__(8),__aligned__(2))) unsigned short simd_packed_ushort8;

/*! @abstract A vector of sixteen 16-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::ushort16. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
typedef __attribute__((__ext_vector_type__(16),__aligned__(2))) unsigned short simd_packed_ushort16;

/*! @abstract A vector of thirty-two 16-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::ushort32. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
typedef __attribute__((__ext_vector_type__(32),__aligned__(2))) unsigned short simd_packed_ushort32;

/*! @abstract A vector of two 32-bit signed (twos-complement) integers with
 *  relaxed alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::int2. The alignment of this type is that of the underlying
 *  scalar element type, so you can use it to load or store from an array of
 *  that type.                                                                */
typedef __attribute__((__ext_vector_type__(2),__aligned__(4))) int simd_packed_int2;

/*! @abstract A vector of four 32-bit signed (twos-complement) integers with
 *  relaxed alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::int4. The alignment of this type is that of the underlying
 *  scalar element type, so you can use it to load or store from an array of
 *  that type.                                                                */
typedef __attribute__((__ext_vector_type__(4),__aligned__(4))) int simd_packed_int4;

/*! @abstract A vector of eight 32-bit signed (twos-complement) integers
 *  with relaxed alignment.
 *  @description In C++ this type is also available as simd::packed::int8.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(8),__aligned__(4))) int simd_packed_int8;

/*! @abstract A vector of sixteen 32-bit signed (twos-complement) integers
 *  with relaxed alignment.
 *  @description In C++ this type is also available as simd::packed::int16.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(16),__aligned__(4))) int simd_packed_int16;

/*! @abstract A vector of two 32-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::uint2. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(2),__aligned__(4))) unsigned int simd_packed_uint2;

/*! @abstract A vector of four 32-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::uint4. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(4),__aligned__(4))) unsigned int simd_packed_uint4;

/*! @abstract A vector of eight 32-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as simd::packed::uint8.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(8),__aligned__(4))) unsigned int simd_packed_uint8;

/*! @abstract A vector of sixteen 32-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as simd::packed::uint16.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(16),__aligned__(4))) unsigned int simd_packed_uint16;

/*! @abstract A vector of two 32-bit floating-point numbers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::float2. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(2),__aligned__(4))) float simd_packed_float2;

/*! @abstract A vector of four 32-bit floating-point numbers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::float4. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
typedef __attribute__((__ext_vector_type__(4),__aligned__(4))) float simd_packed_float4;

/*! @abstract A vector of eight 32-bit floating-point numbers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as simd::packed::float8.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
typedef __attribute__((__ext_vector_type__(8),__aligned__(4))) float simd_packed_float8;

/*! @abstract A vector of sixteen 32-bit floating-point numbers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::float16. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
typedef __attribute__((__ext_vector_type__(16),__aligned__(4))) float simd_packed_float16;

/*! @abstract A vector of two 64-bit signed (twos-complement) integers with
 *  relaxed alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::long2. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
#if defined __LP64__
typedef __attribute__((__ext_vector_type__(2),__aligned__(8))) simd_long1 simd_packed_long2;
#else
typedef __attribute__((__ext_vector_type__(2),__aligned__(4))) simd_long1 simd_packed_long2;
#endif

/*! @abstract A vector of four 64-bit signed (twos-complement) integers with
 *  relaxed alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::long4. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
#if defined __LP64__
typedef __attribute__((__ext_vector_type__(4),__aligned__(8))) simd_long1 simd_packed_long4;
#else
typedef __attribute__((__ext_vector_type__(4),__aligned__(4))) simd_long1 simd_packed_long4;
#endif

/*! @abstract A vector of eight 64-bit signed (twos-complement) integers
 *  with relaxed alignment.
 *  @description In C++ this type is also available as simd::packed::long8.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
#if defined __LP64__
typedef __attribute__((__ext_vector_type__(8),__aligned__(8))) simd_long1 simd_packed_long8;
#else
typedef __attribute__((__ext_vector_type__(8),__aligned__(4))) simd_long1 simd_packed_long8;
#endif

/*! @abstract A vector of two 64-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::ulong2. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
#if defined __LP64__
typedef __attribute__((__ext_vector_type__(2),__aligned__(8))) simd_ulong1 simd_packed_ulong2;
#else
typedef __attribute__((__ext_vector_type__(2),__aligned__(4))) simd_ulong1 simd_packed_ulong2;
#endif

/*! @abstract A vector of four 64-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::ulong4. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
#if defined __LP64__
typedef __attribute__((__ext_vector_type__(4),__aligned__(8))) simd_ulong1 simd_packed_ulong4;
#else
typedef __attribute__((__ext_vector_type__(4),__aligned__(4))) simd_ulong1 simd_packed_ulong4;
#endif

/*! @abstract A vector of eight 64-bit unsigned integers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as simd::packed::ulong8.
 *  This type is not available in Metal. The alignment of this type is only
 *  that of the underlying scalar element type, so you can use it to load or
 *  store from an array of that type.                                         */
#if defined __LP64__
typedef __attribute__((__ext_vector_type__(8),__aligned__(8))) simd_ulong1 simd_packed_ulong8;
#else
typedef __attribute__((__ext_vector_type__(8),__aligned__(4))) simd_ulong1 simd_packed_ulong8;
#endif

/*! @abstract A vector of two 64-bit floating-point numbers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::double2. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
#if defined __LP64__
typedef __attribute__((__ext_vector_type__(2),__aligned__(8))) double simd_packed_double2;
#else
typedef __attribute__((__ext_vector_type__(2),__aligned__(4))) double simd_packed_double2;
#endif

/*! @abstract A vector of four 64-bit floating-point numbers with relaxed
 *  alignment.
 *  @description In C++ and Metal, this type is also available as
 *  simd::packed::double4. The alignment of this type is that of the
 *  underlying scalar element type, so you can use it to load or store from
 *  an array of that type.                                                    */
#if defined __LP64__
typedef __attribute__((__ext_vector_type__(4),__aligned__(8))) double simd_packed_double4;
#else
typedef __attribute__((__ext_vector_type__(4),__aligned__(4))) double simd_packed_double4;
#endif

/*! @abstract A vector of eight 64-bit floating-point numbers with relaxed
 *  alignment.
 *  @description In C++ this type is also available as
 *  simd::packed::double8. This type is not available in Metal. The
 *  alignment of this type is only that of the underlying scalar element
 *  type, so you can use it to load or store from an array of that type.      */
#if defined __LP64__
typedef __attribute__((__ext_vector_type__(8),__aligned__(8))) double simd_packed_double8;
#else
typedef __attribute__((__ext_vector_type__(8),__aligned__(4))) double simd_packed_double8;
#endif

/*  MARK: C++ vector types                                                    */
#if defined __cplusplus
namespace simd {
  namespace packed {
    /*! @abstract A vector of two 8-bit signed (twos-complement) integers
     *  with relaxed alignment.
     *  @description In C or Objective-C, this type is available as
     *  simd_packed_char2. The alignment of this type is only that of the
     *  underlying scalar element type, so you can use it to load or store
     *  from an array of that type.                                           */
typedef ::simd_packed_char2 char2;
  
    /*! @abstract A vector of four 8-bit signed (twos-complement) integers
     *  with relaxed alignment.
     *  @description In C or Objective-C, this type is available as
     *  simd_packed_char4. The alignment of this type is only that of the
     *  underlying scalar element type, so you can use it to load or store
     *  from an array of that type.                                           */
typedef ::simd_packed_char4 char4;
  
    /*! @abstract A vector of eight 8-bit signed (twos-complement) integers
     *  with relaxed alignment.
     *  @description This type is not available in Metal. In C or
     *  Objective-C, this type is available as simd_packed_char8. The
     *  alignment of this type is only that of the underlying scalar element
     *  type, so you can use it to load or store from an array of that type.  */
typedef ::simd_packed_char8 char8;
  
    /*! @abstract A vector of sixteen 8-bit signed (twos-complement)
     *  integers with relaxed alignment.
     *  @description This type is not available in Metal. In C or
     *  Objective-C, this type is available as simd_packed_char16. The
     *  alignment of this type is only that of the underlying scalar element
     *  type, so you can use it to load or store from an array of that type.  */
typedef ::simd_packed_char16 char16;
  
    /*! @abstract A vector of thirty-two 8-bit signed (twos-complement)
     *  integers with relaxed alignment.
     *  @description This type is not available in Metal. In C or
     *  Objective-C, this type is available as simd_packed_char32. The
     *  alignment of this type is only that of the underlying scalar element
     *  type, so you can use it to load or store from an array of that type.  */
typedef ::simd_packed_char32 char32;
  
    /*! @abstract A vector of sixty-four 8-bit signed (twos-complement)
     *  integers with relaxed alignment.
     *  @description This type is not available in Metal. In C or
     *  Objective-C, this type is available as simd_packed_char64. The
     *  alignment of this type is only that of the underlying scalar element
     *  type, so you can use it to load or store from an array of that type.  */
typedef ::simd_packed_char64 char64;
  
    /*! @abstract A vector of two 8-bit unsigned integers with relaxed
     *  alignment.
     *  @description In C or Objective-C, this type is available as
     *  simd_packed_uchar2. The alignment of this type is only that of the
     *  underlying scalar element type, so you can use it to load or store
     *  from an array of that type.                                           */
typedef ::simd_packed_uchar2 uchar2;
  
    /*! @abstract A vector of four 8-bit unsigned integers with relaxed
     *  alignment.
     *  @description In C or Objective-C, this type is available as
     *  simd_packed_uchar4. The alignment of this type is only that of the
     *  underlying scalar element type, so you can use it to load or store
     *  from an array of that type.                                           */
typedef ::simd_packed_uchar4 uchar4;
  
    /*! @abstract A vector of eight 8-bit unsigned integers with relaxed
     *  alignment.
     *  @description This type is not available in Metal. In C or
     *  Objective-C, this type is available as simd_packed_uchar8. The
     *  alignment of this type is only that of the underlying scalar element
     *  type, so you can use it to load or store from an array of that type.  */
typedef ::simd_packed_uchar8 uchar8;
  
    /*! @abstract A vector of sixteen 8-bit unsigned integers with relaxed
     *  alignment.
     *  @description This type is not available in Metal. In C or
     *  Objective-C, this type is available as simd_packed_u