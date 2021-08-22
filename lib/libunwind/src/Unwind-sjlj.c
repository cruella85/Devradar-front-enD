//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
//  Implements setjump-longjump based C++ exceptions
//
//===----------------------------------------------------------------------===//

#include <unwind.h>

#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"

/// With SJLJ based exceptions, any function that has a catch clause or needs to
/// do any clean up when an exception propagates through it, needs to call
/// \c _Unwind_SjLj_Register at the start of the function and
/// \c _Unwind_SjLj_Unregister at the end.  The register function is called with
/// the address of a block of memory in the function's stack frame.  The runtime
/// keeps a linked list (stack) of these blocks - one per thread.  The calling
/// function also sets the personality and lsda fields of the block.

#if defined(_LIBUNWIND_BUILD_SJLJ_APIS)

struct _Unwind_FunctionContext {
  // next function in stack of handlers
  struct _Unwind_FunctionContext *prev;

#if defined(__ve__)
  // VE requires to store 64 bit pointers in the buffer for SjLj execption.
  // We expand the size of values defined here.  This size must be matched
  // to the size returned by TargetMachine::getSjLjDataSize().

  // set by calling function before registering to be the landing pad
  uint64_t                        resumeLocation;

  // set by personality handler to be parameters passed to landing pad function
  uint64_t                        resumeParameters[4];
#else
  // set by calling function before registering to be the landing pad
  uint32_t                        resumeLocation;

  // set by personality handler to be parameters passed to landing pad function
  uint32_t                        resumeParameters[4];
#endif

  // set by calling function before registering
  _Unwind_Personality_Fn personality;          // arm offset=24
  uintptr_t                       lsda;        // arm offset=28

  // variable length array, contains registers to restore
  // 0 = r7, 1 = pc, 2 = sp
  void                           *jbuf[];
};

#if defined(_LIBUNWIND_HAS_NO_THREADS)
# define _LIBUNWIND_THREAD_LOCAL
#else
# if __STDC_VERSION__ >= 201112L
#  define _LIBUNWIND_THREAD_LOCAL _Thread_local
# elif defined(_MSC_VER)
#  define _LIBUNWIND_THREAD_LOCAL __declspec(thread)
# elif defined(__GNUC__) || defined(__clang__)
#  define _LIBUNWIND_THREAD_LOCAL __thread
# else
#  error Unable to create thread local storage
# endif
#endif


#if !defined(FOR_DYLD)

#if defined(__APPLE__)
#include <System/pthread_machdep.h>
#else
static _LIBUNWIND_THREAD_LOCAL struct _Unwind_FunctionContext *stack = NULL;
#endif

static struct _Unwind_FunctionContext *__Unwind_SjLj_GetTopOfFunctionStack() {
#if defined(__APPLE__)
  return _pthread_getspecific_direct(__PTK_LIBC_DYLD_Unwind_SjLj_Key);
#else
  return stack;
#endif
}

static void
__Unwind_SjLj_SetTopOfFunctionStack(struct _Unwind_FunctionContext *fc) {
#if defined(__APPLE__)
  _pthread_setspecific_direct(__PTK_LIBC_DYLD_Unwind_SjLj_Key, fc);
#else
  stack = fc;
#endif
}

#endif


/// Called at start of each function that catches exceptions
_LIBUNWIND_EXPORT void
_Unwind_SjLj_Register(struct _Unwind_FunctionContext *fc) {
  fc->prev = __Unwind_SjLj_GetTopOfFunctionStack();
  __Unwind_SjLj_SetTopOfFunctionStack(fc);
}


/// Called at end of each function that catches exceptions
_LIBUNWIND_EXPORT void
_Unwind_SjLj_Unregister(struct _Unwind_FunctionContext *fc) {
  __Unwind_SjLj_SetTopOfFunctionStack(fc->prev);
}


static _Unwind_Reason_Code
unwind_phase1(struct _Unwind_Exception *exception_object) {
  _Unwind_FunctionContext_t c = __Unwind_SjLj_GetTopOfFunctionStack();
  _LIBUNWIND_TRACE_UNWINDING("unwind_phase1: initial function-context=%p",
                             (void *)c);

  // walk each frame looking for a place to stop
  for (bool handlerNotFound = true; handlerNotFound; c = c->prev) {

    // check for no more frames
    if (c == NULL) {
      _LIBUNWIND_TRACE_UNWINDING("unwind_phase1(ex_ojb=%p): reached "
                                 "bottom => _URC_END_OF_STACK",
                                 (void *)exception_object);
      return _URC_END_OF_STACK;
    }

    _LIBUNWIND_TRACE_UNWINDING("unwind_phase1: function-context=%p", (void *)c);
    // if there is a personality routine, ask it if it will want to stop at this
    // frame
    if (c->personality != NULL) {
      _LIBUNWIND_TRACE_UNWINDING("unwind_phase1(ex_ojb=%p): calling "
                                 "personality function %p",
                                 (void *)exception_object,
                                 (void *)c->personality);
      _Unwind_Reason_Code personalityResult = (*c->personality)(
          1, _UA_SEARCH_PHASE, exception_object->exception_class,
          exception_object, (struct _Unwind_Context *)c);
      switch (personalityResult) {
      case _URC_HANDLER_FOUND:
        // found a catch clause or locals that need destructing in this frame
        // stop search and remember function context
        handlerNotFound = false;
        exception_object->private_2 = (uintptr_t) c;
        _LIBUNWIND_TRACE_UNWINDING("unwind_phase1(ex_ojb=%p): "
                                   "_URC_HANDLER_FOUND",
                                   (void *)exception_object);
        return _URC_NO_REASON;

      case _URC_CONTINUE_UNWIND:
        _LIBUNWIND_TRACE_UNWINDING("unwind_phase1(ex_ojb=%p): "
                                   "_URC_CONTINUE_UNWIND",
                                   (void *)exception_object);
        // continue unwinding
        break;

      default:
        // something went wrong
        _LIBUNWIND_TRACE_UNWINDING(
            "unwind_phase1(ex_ojb=%p): _URC_FATAL_PHASE1_ERROR",
            (void *)exception_object);
        return _URC_FATAL_PHASE1_ERROR;
      }
    }
  }
  return _URC_NO_REASON;
}


static _Unwind_Reason_Code
unwind_phase2(struct _Unwind_Exception *exception_object) {
  _LIBUNWIND_TRACE_UNWINDING("unwind_phase2(ex_ojb=%p)",
                             (void *)exception_object);

  // walk each frame until we reach where search phase said to stop
  _Unwind_FunctionContext_t c = __Unwind_SjLj_GetTopOfFunctionStack();
  while (true) {
    _LIBUNWIND_TRACE_UNWINDING("unwind_phase2s(ex_ojb=%p): context=%p",
                               (void *)exception_object, (void *)c);

    // check for no more frames
    if (c == NULL) {
      _LIBUNWIND_TRACE_UNWINDING(
          "unwind_phase2(ex_ojb=%p): __unw_step() reached "
          "bottom => _URC_END_OF_STACK",
          (void *)exception_object);
      return _URC_END_OF_S