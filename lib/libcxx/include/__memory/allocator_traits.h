// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___MEMORY_ALLOCATOR_TRAITS_H
#define _LIBCPP___MEMORY_ALLOCATOR_TRAITS_H

#include <__config>
#include <__memory/construct_at.h>
#include <__memory/pointer_traits.h>
#include <__utility/forward.h>
#include <limits>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#define _LIBCPP_ALLOCATOR_TRAITS_HAS_XXX(NAME, PROPERTY)                \
    template <class _Tp, class = void> struct NAME : false_type { };    \
    template <class _Tp>               struct NAME<_Tp, typename __void_t<typename _Tp:: PROPERTY >::type> : true_type { }

// __pointer
_LIBCPP_ALLOCATOR_TRAITS_HAS_XXX(__has_pointer, pointer);
template <class _Tp, class _Alloc,
          class _RawAlloc = typename remove_reference<_Alloc>::type,
          bool = __has_pointer<_RawAlloc>::value>
struct __pointer {
    using type _LIBCPP_NODEBUG = typename _RawAlloc::pointer;
};
template <class _Tp, class _Alloc, class _RawAlloc>
struct __pointer<_Tp, _Alloc, _RawAlloc, false> {
    using type _LIBCPP_NODEBUG = _Tp*;
};

// __const_pointer
_LIBCPP_ALLOCATOR_TRAITS_HAS_XXX(__has_const_pointer, const_pointer);
template <class _Tp, class _Ptr, class _Alloc,
          bool = __has_const_pointer<_Alloc>::value>
struct __const_pointer {
    using type _LIBCPP_NODEBUG = typename _Alloc::const_pointer;
};
template <class _Tp, class _Ptr, class _Alloc>
struct __const_pointer<_Tp, _Ptr, _Alloc, false> {
#ifdef _LIBCPP_CXX03_LANG
    using type = typename pointer_traits<_Ptr>::template rebind<const _Tp>::other;
#else
    using type _LIBCPP_NODEBUG = typename pointer_traits<_Ptr>::template rebind<const _Tp>;
#endif
};

// __void_pointer
_LIBCPP_ALLOCATOR_TRAITS_HAS_XXX(__has_void_pointer, void_pointer);
template <class _Ptr, class _Alloc,
          bool = __has_void_pointer<_Alloc>::value>
struct __void_pointer {
    using type _LIBCPP_NODEBUG = typename _Alloc::void_pointer;
};
template <class _Ptr, class _Alloc>
struct __void_pointer<_Ptr, _Alloc, false> {
#ifdef _LIBCPP_CXX03_LANG
    using type _LIBCPP_NODEBUG = typename pointer_traits<_Ptr>::template rebind<void>::other;
#else
    using type _LIBCPP_NODEBUG = typename pointer_traits<_Ptr>::template rebind<void>;
#endif
};

// __const_void_pointer
_LIBCPP_ALLOCATOR_TRAITS_HAS_XXX(__has_const_void_pointer, const_void_pointer);
template <class _Ptr, class _Alloc,
          bool = __has_const_void_pointer<_Alloc>::value>
struct __const_void_pointer {
    using type _LIBCPP_NODEBUG = typename _Alloc::const_void_pointer;
};
template <class _Ptr, class _Alloc>
struct __const_void_pointer<_Ptr, _Alloc, false> {
#ifdef _LIBCPP_CXX03_LANG
    using type _LIBCPP_NODEBUG = type