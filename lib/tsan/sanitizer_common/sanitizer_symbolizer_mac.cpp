//===-- sanitizer_symbolizer_mac.cpp --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is shared between various sanitizers' runtime libraries.
//
// Implementation of Mac-specific "atos" symbolizer.
//===----------------------------------------------------------------------===//

#include "sanitizer_platform.h"
#if SANITIZER_MAC

#include "sanitizer_allocator_internal.h"
#include "sanitizer_mac.h"
#include "sanitizer_symbolizer_mac.h"

#include <dlfcn.h>
#include <errno.h>
#include <mach/mach.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <util.h>

namespace __sanitizer {

bool DlAddrSymbolizer::SymbolizePC(uptr addr, SymbolizedStack *stack) {
  Dl_info info;
  int result = dladdr((const void *)addr, &info);
  if (!result) return false;

  // Compute offset if possible. `dladdr()` doesn't always ensure that `addr >=
  // sym_addr` so only compute the offset when this holds. Failure to find the
  // function offset is not treated as a failure because it might still be
  // possible to get the symbol name.
  uptr sym_addr = reinterpret_cast<uptr>(info.dli_saddr);
  if (addr >= sym_addr) {
    stack->info.function_o