//===-- sanitizer_errno.h ---------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is shared between sanitizers run-time libraries.
//
// Defines errno to avoid including errno.h and its dependencies into sensitive
// files (e.g. interceptors are not supposed to include any system headers).
// It's ok to use errno.h directly when your file already depend on other system
// includes though.
//
//===------------------------------------------------------------------