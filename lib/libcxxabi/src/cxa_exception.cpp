
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
//  This file implements the "Exception Handling APIs"
//  https://itanium-cxx-abi.github.io/cxx-abi/abi-eh.html
//
//===----------------------------------------------------------------------===//

#include "cxxabi.h"

#include <exception>        // for std::terminate
#include <string.h>         // for memset
#include "cxa_exception.h"
#include "cxa_handlers.h"
#include "fallback_malloc.h"
#include "include/atomic_support.h" // from libc++

#if __has_feature(address_sanitizer)
#include <sanitizer/asan_interface.h>
#endif

// +---------------------------+-----------------------------+---------------+
// | __cxa_exception           | _Unwind_Exception CLNGC++\0 | thrown object |
// +---------------------------+-----------------------------+---------------+
//                                                           ^
//                                                           |
//   +-------------------------------------------------------+
//   |
// +---------------------------+-----------------------------+
// | __cxa_dependent_exception | _Unwind_Exception CLNGC++\1 |
// +---------------------------+-----------------------------+

namespace __cxxabiv1 {

//  Utility routines
static
inline
__cxa_exception*
cxa_exception_from_thrown_object(void* thrown_object)
{
    return static_cast<__cxa_exception*>(thrown_object) - 1;
}

// Note:  This is never called when exception_header is masquerading as a
//        __cxa_dependent_exception.
static
inline
void*
thrown_object_from_cxa_exception(__cxa_exception* exception_header)
{
    return static_cast<void*>(exception_header + 1);
}

//  Get the exception object from the unwind pointer.
//  Relies on the structure layout, where the unwind pointer is right in
//  front of the user's exception object
static
inline
__cxa_exception*
cxa_exception_from_exception_unwind_exception(_Unwind_Exception* unwind_exception)
{
    return cxa_exception_from_thrown_object(unwind_exception + 1 );
}

// Round s up to next multiple of a.
static inline
size_t aligned_allocation_size(size_t s, size_t a) {
    return (s + a - 1) & ~(a - 1);
}

static inline
size_t cxa_exception_size_from_exception_thrown_size(size_t size) {
    return aligned_allocation_size(size + sizeof (__cxa_exception),
                                   alignof(__cxa_exception));
}

void __setExceptionClass(_Unwind_Exception* unwind_exception, uint64_t newValue) {
    ::memcpy(&unwind_exception->exception_class, &newValue, sizeof(newValue));
}


static void setOurExceptionClass(_Unwind_Exception* unwind_exception) {
    __setExceptionClass(unwind_exception, kOurExceptionClass);
}

static void setDependentExceptionClass(_Unwind_Exception* unwind_exception) {
    __setExceptionClass(unwind_exception, kOurDependentExceptionClass);
}

//  Is it one of ours?
uint64_t __getExceptionClass(const _Unwind_Exception* unwind_exception) {
    // On x86 and some ARM unwinders, unwind_exception->exception_class is
    // a uint64_t. On other ARM unwinders, it is a char[8].
    // See: http://infocenter.arm.com/help/topic/com.arm.doc.ihi0038b/IHI0038B_ehabi.pdf
    // So we just copy it into a uint64_t to be sure.
    uint64_t exClass;
    ::memcpy(&exClass, &unwind_exception->exception_class, sizeof(exClass));
    return exClass;
}

bool __isOurExceptionClass(const _Unwind_Exception* unwind_exception) {
    return (__getExceptionClass(unwind_exception) & get_vendor_and_language) ==
           (kOurExceptionClass                    & get_vendor_and_language);
}

static bool isDependentException(_Unwind_Exception* unwind_exception) {
    return (__getExceptionClass(unwind_exception) & 0xFF) == 0x01;
}

//  This does not need to be atomic
static inline int incrementHandlerCount(__cxa_exception *exception) {
    return ++exception->handlerCount;
}

//  This does not need to be atomic
static inline  int decrementHandlerCount(__cxa_exception *exception) {
    return --exception->handlerCount;
}

/*
    If reason isn't _URC_FOREIGN_EXCEPTION_CAUGHT, then the terminateHandler
    stored in exc is called.  Otherwise the exceptionDestructor stored in
    exc is called, and then the memory for the exception is deallocated.

    This is never called for a __cxa_dependent_exception.
*/
static
void
exception_cleanup_func(_Unwind_Reason_Code reason, _Unwind_Exception* unwind_exception)
{
    __cxa_exception* exception_header = cxa_exception_from_exception_unwind_exception(unwind_exception);
    if (_URC_FOREIGN_EXCEPTION_CAUGHT != reason)
        std::__terminate(exception_header->terminateHandler);