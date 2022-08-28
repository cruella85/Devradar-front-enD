//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
//  Processor specific interpretation of DWARF unwind info.
//
//===----------------------------------------------------------------------===//

#ifndef __DWARF_INSTRUCTIONS_HPP__
#define __DWARF_INSTRUCTIONS_HPP__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dwarf2.h"
#include "Registers.hpp"
#include "DwarfParser.hpp"
#include "config.h"


namespace libunwind {


/// DwarfInstructions maps abtract DWARF unwind instructions to a particular
/// architecture
template <typename A, typename R>
class DwarfInstructions {
public:
  typedef typename A::pint_t pint_t;
  typedef typename A::sint_t sint_t;

  static int stepWithDwarf(A &addressSpace, pint_t pc, pint_t fdeStart,
                           R &registers, bool &isSignalFrame);

private:

  enum {
    DW_X86_64_RET_ADDR = 16
  };

  enum {
    DW_X86_RET_ADDR = 8
  };

  typedef typename CFI_Parser<A>::RegisterLocation  RegisterLocation;
  typedef typename CFI_Parser<A>::PrologInfo        PrologInfo;
  typedef typename CFI_Parser<A>::FDE_Info          FDE_Info;
  typedef typename CFI_Parser<A>::CIE_Info          CIE_Info;

  static pint_t evaluateExpression(pint_t expression, A &addressSpace,
                                   const R &registers,
                                   pint_t initialStackValue);
  static pint_t getSavedRegister(A &addressSpace, const R &registers,
                                 pint_t cfa, const RegisterLocation &savedReg);
  static double getSavedFloatRegister(A &addressSpace, const R &registers,
                                  pint_t cfa, const RegisterLocation &savedReg);
  static v128 getSavedVectorRegister(A &addressSpace, const R &registers,
                                  pint_t cfa, const RegisterLocation &savedReg);

  static pint_t getCFA(A &addressSpace, const PrologInfo &prolog,
                       const R &registers) {
    if (prolog.cfaRegister != 0)
      return (pint_t)((sint_t)registers.getRegister((int)prolog.cfaRegister) +
             prolog.cfaRegisterOffset);
    if (prolog.cfaExpression != 0)
      return evaluateExpression((pint_t)prolog.cfaExpression, addressSpace, 
                                registers, 0);
    assert(0 && "getCFA(): unknown location");
    __builtin_unreachable();
  }
#if defined(_LIBUNWIND_TARGET_AARCH64)
  static bool getRA_SIGN_STATE(A &addressSpace, R registers, pint_t cfa,
                               PrologInfo &prolog);
#endif
};

template <typename R>
auto getSparcWCookie(const R &r, int) -> decltype(r.getWCookie()) {
  return r.getWCookie();
}
template <typename R> uint64_t getSparcWCookie(const R &, long) {
  return 0;
}

template <typename A, typename R>
typename A::pint_t DwarfInstructions<A, R>::getSavedRegister(
    A &addressSpace, const R &registers, pint_t cfa,
    const RegisterLocation &savedReg) {
  switch (