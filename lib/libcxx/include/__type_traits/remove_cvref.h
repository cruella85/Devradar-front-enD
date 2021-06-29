//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___TYPE_TRAITS_REMOVE_CVREF_H
#define _LIBCPP___TYPE_TRAITS_REMOVE_CVREF_H

#include <__config>
#include <__type_traits/is_same.h>
#include <__type_traits/remove_cv.h>
#include <__type_traits/remove_reference.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp>
using __uncvref_t _LIBCPP_NODEBUG = typename remove_cv<typename remove_reference<_Tp>::type>::type;

template <class _Tp, class _Up>
struct __is_same_uncvref : _IsSame<__uncvref_t<_Tp>, __uncvref_t<_Up> > {};
