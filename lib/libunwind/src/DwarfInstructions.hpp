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
  switch (savedReg.location) {
  case CFI_Parser<A>::kRegisterInCFA:
    return (pint_t)addressSpace.getRegister(cfa + (pint_t)savedReg.value);

  case CFI_Parser<A>::kRegisterInCFADecrypt: // sparc64 specific
    return (pint_t)(addressSpace.getP(cfa + (pint_t)savedReg.value) ^
           getSparcWCookie(registers, 0));

  case CFI_Parser<A>::kRegisterAtExpression:
    return (pint_t)addressSpace.getRegister(evaluateExpression(
        (pint_t)savedReg.value, addressSpace, registers, cfa));

  case CFI_Parser<A>::kRegisterIsExpression:
    return evaluateExpression((pint_t)savedReg.value, addressSpace,
                              registers, cfa);

  case CFI_Parser<A>::kRegisterInRegister:
    return registers.getRegister((int)savedReg.value);
  case CFI_Parser<A>::kRegisterUndefined:
    return 0;
  case CFI_Parser<A>::kRegisterUnused:
  case CFI_Parser<A>::kRegisterOffsetFromCFA:
    // FIX ME
    break;
  }
  _LIBUNWIND_ABORT("unsupported restore location for register");
}

template <typename A, typename R>
double DwarfInstructions<A, R>::getSavedFloatRegister(
    A &addressSpace, const R &registers, pint_t cfa,
    const RegisterLocation &savedReg) {
  switch (savedReg.location) {
  case CFI_Parser<A>::kRegisterInCFA:
    return addressSpace.getDouble(cfa + (pint_t)savedReg.value);

  case CFI_Parser<A>::kRegisterAtExpression:
    return addressSpace.getDouble(
        evaluateExpression((pint_t)savedReg.value, addressSpace,
                            registers, cfa));
  case CFI_Parser<A>::kRegisterUndefined:
    return 0.0;
  case CFI_Parser<A>::kRegisterInRegister:
#ifndef _LIBUNWIND_TARGET_ARM
    return registers.getFloatRegister((int)savedReg.value);
#endif
  case CFI_Parser<A>::kRegisterIsExpression:
  case CFI_Parser<A>::kRegisterUnused:
  case CFI_Parser<A>::kRegisterOffsetFromCFA:
  case CFI_Parser<A>::kRegisterInCFADecrypt:
    // FIX ME
    break;
  }
  _LIBUNWIND_ABORT("unsupported restore location for float register");
}

template <typename A, typename R>
v128 DwarfInstructions<A, R>::getSavedVectorRegister(
    A &addressSpace, const R &registers, pint_t cfa,
    const RegisterLocation &savedReg) {
  switch (savedReg.location) {
  case CFI_Parser<A>::kRegisterInCFA:
    return addressSpace.getVector(cfa + (pint_t)savedReg.value);

  case CFI_Parser<A>::kRegisterAtExpression:
    return addressSpace.getVector(
        evaluateExpression((pint_t)savedReg.value, addressSpace,
                            registers, cfa));

  case CFI_Parser<A>::kRegisterIsExpression:
  case CFI_Parser<A>::kRegisterUnused:
  case CFI_Parser<A>::kRegisterUndefined:
  case CFI_Parser<A>::kRegisterOffsetFromCFA:
  case CFI_Parser<A>::kRegisterInRegister:
  case CFI_Parser<A>::kRegisterInCFADecrypt:
    // FIX ME
    break;
  }
  _LIBUNWIND_ABORT("unsupported restore location for vector register");
}
#if defined(_LIBUNWIND_TARGET_AARCH64)
template <typename A, typename R>
bool DwarfInstructions<A, R>::getRA_SIGN_STATE(A &addressSpace, R registers,
                                               pint_t cfa, PrologInfo &prolog) {
  pint_t raSignState;
  auto regloc = prolog.savedRegisters[UNW_AARCH64_RA_SIGN_STATE];
  if (regloc.location == CFI_Parser<A>::kRegisterUnused)
    raSignState = static_cast<pint_t>(regloc.value);
  else
    raSignState = getSavedRegister(addressSpace, registers, cfa, regloc);

  // Only bit[0] is meaningful.
  return raSignState & 0x01;
}
#endif

template <typename A, typename R>
int DwarfInstructions<A, R>::stepWithDwarf(A &addressSpace, pint_t pc,
                                           pint_t fdeStart, R &registers,
                                           bool &isSignalFrame) {
  FDE_Info fdeInfo;
  CIE_Info cieInfo;
  if (CFI_Parser<A>::decodeFDE(addressSpace, fdeStart, &fdeInfo,
                               &cieInfo) == NULL) {
    PrologInfo prolog;
    if (CFI_Parser<A>::parseFDEInstructions(addressSpace, fdeInfo, cieInfo, pc,
                                            R::getArch(), &prolog)) {
      // get pointer to cfa (architecture specific)
      pint_t cfa = getCFA(addressSpace, prolog, registers);

       // restore registers that DWARF says were saved
      R newRegisters = registers;

      // Typically, the CFA is the stack pointer at the call site in
      // the previous frame. However, there are scenarios in which this is not
      // true. For example, if we switched to a new stack. In that case, the
      // value of the previous SP might be indicated by a CFI directive.
      //
      // We set the SP here to the CFA, allowing for it to be overridden
      // by a CFI directive later on.
      newRegisters.setSP(cfa);

      pint_t returnAddress = 0;
      constexpr int lastReg = R::lastDwarfRegNum();
      static_assert(static_cast<int>(CFI_Parser<A>::kMaxRegisterNumber) >=
                        lastReg,
                    "register range too large");
      assert(lastReg >= (int)cieInfo.returnAddressRegister &&
             "register range does not contain return address register");
      for (int i = 0; i <= lastReg; ++i) {
        if (prolog.savedRegisters[i].location !=
            CFI_Parser<A>::kRegisterUnused) {
          if (registers.validFloatRegister(i))
            newRegisters.setFloatRegister(
                i, getSavedFloatRegister(addressSpace, registers, cfa,
                                         prolog.savedRegisters[i]));
          else if (registers.validVectorRegister(i))
            newRegisters.setVectorRegister(
                i, getSavedVectorRegister(addressSpace, registers, cfa,
                                          prolog.savedRegisters[i]));
          else if (i == (int)cieInfo.returnAddressRegister)
            returnAddress = getSavedRegister(addressSpace, registers, cfa,
                                             prolog.savedRegisters[i]);
          else if (registers.validRegister(i))
            newRegisters.setRegister(
                i, getSavedRegister(addressSpace, registers, cfa,
                                    prolog.savedRegisters[i]));
          else
            return UNW_EBADREG;
        } else if (i == (int)cieInfo.returnAddressRegister) {
            // Leaf function keeps the return address in register and there is no
            // explicit intructions how to restore it.
            returnAddress = registers.getRegister(cieInfo.returnAddressRegister);
        }
      }

      isSignalFrame = cieInfo.isSignalFrame;

#if defined(_LIBUNWIND_TARGET_AARCH64)
      // If the target is aarch64 then the return address may have been signed
      // using the v8.3 pointer authentication extensions. The original
      // return address needs to be authenticated before the return address is
      // restored. autia1716 is used instead of autia as autia1716 assembles
      // to a NOP on pre-v8.3a architectures.
      if ((R::getArch() == REGISTERS_ARM64) &&
          getRA_SIGN_STATE(addressSpace, registers, cfa, prolog) &&
          returnAddress != 0) {
#if !defined(_LIBUNWIND_IS_NATIVE_ONLY)
        return UNW_ECROSSRASIGNING;
#else
        register unsigned long long x17 __asm("x17") = returnAddress;
        register unsigned long long x16 __asm("x16") = cfa;

        // These are the autia1716/autib1716 instructions. The hint instructions
        // are used here as gcc does not assemble autia1716/autib1716 for pre
        // armv8.3a targets.
        if (cieInfo.addressesSignedWithBKey)
          asm("hint 0xe" : "+r"(x17) : "r"(x16)); // autib1716
        else
          asm("hint 0xc" : "+r"(x17) : "r"(x16)); // autia1716
        returnAddress = x17;
#endif
      }
#endif

#if defined(_LIBUNWIND_IS_NATIVE_ONLY) && defined(_LIBUNWIND_TARGET_ARM) &&    \
    defined(__ARM_FEATURE_PAUTH)
      if ((R::getArch() == REGISTERS_ARM) &&
          prolog.savedRegisters[UNW_ARM_RA_AUTH_CODE].value) {
        pint_t pac =
            getSavedRegister(addressSpace, registers, cfa,
                             prolog.savedRegisters[UNW_ARM_RA_AUTH_CODE]);
        __asm__ __volatile__("autg %0, %1, %2"
                             :
                             : "r"(pac), "r"(returnAddress), "r"(cfa)
                             :);
      }
#endif

#if defined(_LIBUNWIND_TARGET_SPARC)
      if (R::getArch() == REGISTERS_SPARC) {
        // Skip call site instruction and delay slot
        returnAddress += 8;
        // Skip unimp instruction if function returns a struct
        if ((addressSpace.get32(returnAddress) & 0xC1C00000) == 0)
          returnAddress += 4;
      }
#endif

#if defined(_LIBUNWIND_TARGET_SPARC64)
      // Skip call site instruction and delay slot.
      if (R::getArch() == REGISTERS_SPARC64)
        returnAddress += 8;
#endif

#if defined(_LIBUNWIND_TARGET