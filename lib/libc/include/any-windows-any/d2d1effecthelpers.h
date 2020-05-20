/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#ifndef _D2D1_EFFECT_HELPERS_H_
#define _D2D1_EFFECT_HELPERS_H_

#include <winapifamily.h>

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)

#include <d2d1effectauthor.h>

template<typename T>
T GetType(T t) {
    return t;
};

template<class C, typename P, typename I>
HRESULT DeducingValueSetter(HRESULT 