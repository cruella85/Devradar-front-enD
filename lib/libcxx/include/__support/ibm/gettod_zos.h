// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_SUPPORT_IBM_GETTOD_ZOS_H
#define _LIBCPP_SUPPORT_IBM_GETTOD_ZOS_H

#include <time.h>

inline _LIBCPP_HIDE_FROM_ABI int
gettimeofdayMonotonic(struct timespec64* Output) {

  // The POSIX gettimeofday() function is not available on z/OS. Therefore,
  // we will call stcke and other hardware instructions in implement equivalent.
  // Note that nanoseconds alone will overflow when reaching new epoch in 2042.

  struct _t {
    uint64_t Hi;
    