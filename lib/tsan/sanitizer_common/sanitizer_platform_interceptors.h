//===-- sanitizer_platform_interceptors.h -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines macro telling whether sanitizer tools can/should intercept
// given library functions on a given platform.
//
//===----------------------------------------------------------------------===//
#ifndef SANITIZER_PLATFORM_INTERCEPTORS_H
#define SANITIZER_PLATFORM_INTERCEPTORS_H

#include "sanitizer_glibc_version.h"
#include "sanitizer_internal_defs.h"
#include "san