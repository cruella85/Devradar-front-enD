
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___FUNCTIONAL_BIND_H
#define _LIBCPP___FUNCTIONAL_BIND_H

#include <__config>
#include <__functional/invoke.h>
#include <__functional/weak_result_type.h>
#include <cstddef>
#include <tuple>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

template<class _Tp>
struct is_bind_expression : _If<
    _IsSame<_Tp, __uncvref_t<_Tp> >::value,
    false_type,
    is_bind_expression<__uncvref_t<_Tp> >
> {};

#if _LIBCPP_STD_VER > 14
template <class _Tp>
inline constexpr size_t is_bind_expression_v = is_bind_expression<_Tp>::value;
#endif

template<class _Tp>
struct is_placeholder : _If<
    _IsSame<_Tp, __uncvref_t<_Tp> >::value,
    integral_constant<int, 0>,
    is_placeholder<__uncvref_t<_Tp> >
> {};

#if _LIBCPP_STD_VER > 14
template <class _Tp>
inline constexpr size_t is_placeholder_v = is_placeholder<_Tp>::value;
#endif

namespace placeholders
{

template <int _Np> struct __ph {};

#if defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_BUILDING_LIBRARY)
_LIBCPP_FUNC_VIS extern const __ph<1>   _1;
_LIBCPP_FUNC_VIS extern const __ph<2>   _2;
_LIBCPP_FUNC_VIS extern const __ph<3>   _3;
_LIBCPP_FUNC_VIS extern const __ph<4>   _4;
_LIBCPP_FUNC_VIS extern const __ph<5>   _5;
_LIBCPP_FUNC_VIS extern const __ph<6>   _6;
_LIBCPP_FUNC_VIS extern const __ph<7>   _7;
_LIBCPP_FUNC_VIS extern const __ph<8>   _8;
_LIBCPP_FUNC_VIS extern const __ph<9>   _9;
_LIBCPP_FUNC_VIS extern const __ph<10> _10;
#else
/* inline */ constexpr __ph<1>   _1{};
/* inline */ constexpr __ph<2>   _2{};
/* inline */ constexpr __ph<3>   _3{};
/* inline */ constexpr __ph<4>   _4{};
/* inline */ constexpr __ph<5>   _5{};
/* inline */ constexpr __ph<6>   _6{};
/* inline */ constexpr __ph<7>   _7{};
/* inline */ constexpr __ph<8>   _8{};
/* inline */ constexpr __ph<9>   _9{};
/* inline */ constexpr __ph<10> _10{};
#endif // defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_BUILDING_LIBRARY)

} // namespace placeholders

template<int _Np>
struct is_placeholder<placeholders::__ph<_Np> >
    : public integral_constant<int, _Np> {};


#ifndef _LIBCPP_CXX03_LANG

template <class _Tp, class _Uj>
inline _LIBCPP_INLINE_VISIBILITY
_Tp&
__mu(reference_wrapper<_Tp> __t, _Uj&)
{
    return __t.get();
}

template <class _Ti, class ..._Uj, size_t ..._Indx>
inline _LIBCPP_INLINE_VISIBILITY
typename __invoke_of<_Ti&, _Uj...>::type
__mu_expand(_Ti& __ti, tuple<_Uj...>& __uj, __tuple_indices<_Indx...>)
{
    return __ti(_VSTD::forward<_Uj>(_VSTD::get<_Indx>(__uj))...);
}

template <class _Ti, class ..._Uj>
inline _LIBCPP_INLINE_VISIBILITY
typename __enable_if_t
<
    is_bind_expression<_Ti>::value,
    __invoke_of<_Ti&, _Uj...>
>::type
__mu(_Ti& __ti, tuple<_Uj...>& __uj)
{
    typedef typename __make_tuple_indices<sizeof...(_Uj)>::type __indices;
    return _VSTD::__mu_expand(__ti, __uj, __indices());
}

template <bool IsPh, class _Ti, class _Uj>
struct __mu_return2 {};

template <class _Ti, class _Uj>
struct __mu_return2<true, _Ti, _Uj>
{
    typedef typename tuple_element<is_placeholder<_Ti>::value - 1, _Uj>::type type;
};

template <class _Ti, class _Uj>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    0 < is_placeholder<_Ti>::value,
    typename __mu_return2<0 < is_placeholder<_Ti>::value, _Ti, _Uj>::type
>::type
__mu(_Ti&, _Uj& __uj)
{
    const size_t _Indx = is_placeholder<_Ti>::value - 1;
    return _VSTD::forward<typename tuple_element<_Indx, _Uj>::type>(_VSTD::get<_Indx>(__uj));
}

template <class _Ti, class _Uj>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    !is_bind_expression<_Ti>::value &&
    is_placeholder<_Ti>::value == 0 &&
    !__is_reference_wrapper<_Ti>::value,
    _Ti&
>::type
__mu(_Ti& __ti, _Uj&)
{
    return __ti;
}

template <class _Ti, bool IsReferenceWrapper, bool IsBindEx, bool IsPh,
          class _TupleUj>
struct __mu_return_impl;

template <bool _Invokable, class _Ti, class ..._Uj>
struct __mu_return_invokable  // false
{
    typedef __nat type;
};

template <class _Ti, class ..._Uj>
struct __mu_return_invokable<true, _Ti, _Uj...>
{
    typedef typename __invoke_of<_Ti&, _Uj...>::type type;
};

template <class _Ti, class ..._Uj>
struct __mu_return_impl<_Ti, false, true, false, tuple<_Uj...> >
    : public __mu_return_invokable<__invokable<_Ti&, _Uj...>::value, _Ti, _Uj...>
{
};

template <class _Ti, class _TupleUj>
struct __mu_return_impl<_Ti, false, false, true, _TupleUj>
{
    typedef typename tuple_element<is_placeholder<_Ti>::value - 1,
                                   _TupleUj>::type&& type;
};

template <class _Ti, class _TupleUj>
struct __mu_return_impl<_Ti, true, false, false, _TupleUj>
{
    typedef typename _Ti::type& type;
};

template <class _Ti, class _TupleUj>
struct __mu_return_impl<_Ti, false, false, false, _TupleUj>
{
    typedef _Ti& type;
};

template <class _Ti, class _TupleUj>
struct __mu_return
    : public __mu_return_impl<_Ti,
                              __is_reference_wrapper<_Ti>::value,
                              is_bind_expression<_Ti>::value,
                              0 < is_placeholder<_Ti>::value &&
                              is_placeholder<_Ti>::value <= tuple_size<_TupleUj>::value,
                              _TupleUj>
{
};

template <class _Fp, class _BoundArgs, class _TupleUj>
struct __is_valid_bind_return
{