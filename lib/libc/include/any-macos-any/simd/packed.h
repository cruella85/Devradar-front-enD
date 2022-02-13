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
 *  simd_float4 vector = *(simd_packed_float4 *)&array