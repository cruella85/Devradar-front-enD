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

#if defined(_LIBUNWIND_TARGET_PPC64)
#define PPC64_ELFV1_R2_LOAD_INST_ENCODING 0xe8410028u // ld r2,40(r1)
#define PPC64_ELFV1_R2_OFFSET 40
#define PPC64_ELFV2_R2_LOAD_INST_ENCODING 0xe8410018u // ld r2,24(r1)
#define PPC64_ELFV2_R2_OFFSET 24
      // If the instruction at return address is a TOC (r2) restore,
      // then r2 was saved and needs to be restored.
      // ELFv2 ABI specifies that the TOC Pointer must be saved at SP + 24,
      // while in ELFv1 ABI it is saved at SP + 40.
      if (R::getArch() == REGISTERS_PPC64 && returnAddress != 0) {
        pint_t sp = newRegisters.getRegister(UNW_REG_SP);
        pint_t r2 = 0;
        switch (addressSpace.get32(returnAddress)) {
        case PPC64_ELFV1_R2_LOAD_INST_ENCODING:
          r2 = addressSpace.get64(sp + PPC64_ELFV1_R2_OFFSET);
          break;
        case PPC64_ELFV2_R2_LOAD_INST_ENCODING:
          r2 = addressSpace.get64(sp + PPC64_ELFV2_R2_OFFSET);
          break;
        }
        if (r2)
          newRegisters.setRegister(UNW_PPC64_R2, r2);
      }
#endif

      // Return address is address after call site instruction, so setting IP to
      // that does simualates a return.
      newRegisters.setIP(returnAddress);

      // Simulate the step by replacing the register set with the new ones.
      registers = newRegisters;

      return UNW_STEP_SUCCESS;
    }
  }
  return UNW_EBADFRAME;
}

template <typename A, typename R>
typename A::pint_t
DwarfInstructions<A, R>::evaluateExpression(pint_t expression, A &addressSpace,
                                            const R &registers,
                                            pint_t initialStackValue) {
  const bool log = false;
  pint_t p = expression;
  pint_t expressionEnd = expression + 20; // temp, until len read
  pint_t length = (pint_t)addressSpace.getULEB128(p, expressionEnd);
  expressionEnd = p + length;
  if (log)
    fprintf(stderr, "evaluateExpression(): length=%" PRIu64 "\n",
            (uint64_t)length);
  pint_t stack[100];
  pint_t *sp = stack;
  *(++sp) = initialStackValue;

  while (p < expressionEnd) {
    if (log) {
      for (pint_t *t = sp; t > stack; --t) {
        fprintf(stderr, "sp[] = 0x%" PRIx64 "\n", (uint64_t)(*t));
      }
    }
    uint8_t opcode = addressSpace.get8(p++);
    sint_t svalue, svalue2;
    pint_t value;
    uint32_t reg;
    switch (opcode) {
    case DW_OP_addr:
      // push immediate address sized value
      value = addressSpace.getP(p);
      p += sizeof(pint_t);
      *(++sp) = value;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)value);
      break;

    case DW_OP_deref:
      // pop stack, dereference, push result
      value = *sp--;
      *(++sp) = addressSpace.getP(value);
      if (log)
        fprintf(stderr, "dereference 0x%" PRIx64 "\n", (uint64_t)value);
      break;

    case DW_OP_const1u:
      // push immediate 1 byte value
      value = addressSpace.get8(p);
      p += 1;
      *(++sp) = value;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)value);
      break;

    case DW_OP_const1s:
      // push immediate 1 byte signed value
      svalue = (int8_t) addressSpace.get8(p);
      p += 1;
      *(++sp) = (pint_t)svalue;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)svalue);
      break;

    case DW_OP_const2u:
      // push immediate 2 byte value
      value = addressSpace.get16(p);
      p += 2;
      *(++sp) = value;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)value);
      break;

    case DW_OP_const2s:
      // push immediate 2 byte signed value
      svalue = (int16_t) addressSpace.get16(p);
      p += 2;
      *(++sp) = (pint_t)svalue;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)svalue);
      break;

    case DW_OP_const4u:
      // push immediate 4 byte value
      value = addressSpace.get32(p);
      p += 4;
      *(++sp) = value;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)value);
      break;

    case DW_OP_const4s:
      // push immediate 4 byte signed value
      svalue = (int32_t)addressSpace.get32(p);
      p += 4;
      *(++sp) = (pint_t)svalue;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)svalue);
      break;

    case DW_OP_const8u:
      // push immediate 8 byte value
      value = (pint_t)addressSpace.get64(p);
      p += 8;
      *(++sp) = value;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)value);
      break;

    case DW_OP_const8s:
      // push immediate 8 byte signed value
      value = (pint_t)addressSpace.get64(p);
      p += 8;
      *(++sp) = value;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)value);
      break;

    case DW_OP_constu:
      // push immediate ULEB128 value
      value = (pint_t)addressSpace.getULEB128(p, expressionEnd);
      *(++sp) = value;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)value);
      break;

    case DW_OP_consts:
      // push immediate SLEB128 value
      svalue = (sint_t)addressSpace.getSLEB128(p, expressionEnd);
      *(++sp) = (pint_t)svalue;
      if (log)
        fprintf(stderr, "push 0x%" PRIx64 "\n", (uint64_t)svalue);
      break;

    case DW_OP_dup:
      // push top of stack
      value = *sp;
      *(++sp) = value;
      if (log)
        fprintf(stderr, "duplicate top of stack\n");
      break;

    case DW_OP_drop:
      // pop
      --sp;
      if (log)
        fprintf(stderr, "pop top of stack\n");
      break;

    case DW_OP_over:
      // dup second
      value = sp[-1];
      *(++sp) = value;
      if (log)
        fprintf(stderr, "duplicate second in stack\n");
      break;

    case DW_OP_pick:
      // pick from
      reg = addressSpace.get8(p);
      p += 1;
      value = sp[-(int)reg];
      *(++sp) = value;
      if (log)
        fprintf(stderr, "duplicate %d in stack\n", reg);
      break;

    case DW_OP_swap:
      // swap top two
      value = sp[0];
      sp[0] = sp[-1];
      sp[-1] = value;
      if (log)
        fprintf(stderr, "swap top of stack\n");
      break;

    case DW_OP_rot:
      // rotate top three
      value = sp[0];
      sp[0] = sp[-1];
      sp[-1] = sp[-2];
      sp[-2] = value;
      if (log)
        fprintf(stderr, "rotate top three of stack\n");
      break;

    case DW_OP_xderef:
      // pop stack, dereference, push result
      value = *sp--;
      *sp = *((pint_t*)value);
      if (log)
        fprintf(stderr, "x-dereference 0x%" PRIx64 "\n", (uint64_t)value);
      break;

    case DW_OP_abs:
      svalue = (sint_t)*sp;
      if (svalue < 0)
        *sp = (pint_t)(-svalue);
      if (log)
        fprintf(stderr, "abs\n");
      break;

    case DW_OP_and:
      value = *sp--;
      *sp &= value;
      if (log)
        fprintf(stderr, "and\n");
      break;

    case DW_OP_div:
      svalue = (sint_t)(*sp--);
      svalue2 = (sint_t)*sp;
      *sp = (pint_t)(svalue2 / svalue);
      if (log)
        fprintf(stderr, "div\n");
      break;

    case DW_OP_minus:
      value = *sp--;
      *sp = *sp - value;
      if (log)
        fprintf(stderr, "minus\n");
      break;

    case DW_OP_mod:
      svalue = (sint_t)(*sp--);
      svalue2 = (sint_t)*sp;
      *sp = (pint_t)(svalue2 % svalue);
      if (log)
        fprintf(stderr, "module\n");
      break;

    case DW_OP_mul:
      svalue = (sint_t)(*sp--);
      svalue2 = (sint_t)*sp;
      *sp = (pint_t)(svalue2 * svalue);
      if (log)
        fprintf(stderr, "mul\n");
      break;

    case DW_OP_neg:
      *sp = 0 - *sp;
      if (log)
        fprintf(stderr, "neg\n");
      break;

    case DW_OP_not:
      svalue = (sint_t)(*sp);
      *sp = (pint_t)(~svalue);
      if (log)
        fprintf(stderr, "not\n");
      break;

    case DW_OP_or:
      value = *sp--;
      *sp |= value;
      if (log)
        fprintf(stderr, "or\n");
      break;

    case DW_OP_plus:
      value = *sp--;
      *sp += value;
      if (log)
        fprintf(stderr, "plus\n");
      break;

    case DW_OP_plus_uconst:
      // pop stack, add uelb128 constant, push result
      *sp += static_cast<pint_t>(addressSpace.getULEB128(p, expressionEnd));
      if (log)
        fprintf(stderr, "add constant\n");
      break;

    case DW_OP_shl:
      value = *sp--;
      *sp = *sp << value;
      if (log)
        fprintf(stderr, "shift left\n");
      break;

    case DW_OP_shr:
      value = *sp--;
      *sp = *sp >> value;
      if (log)
        fprintf(stderr, "shift left\n");
      break;

    case DW_OP_shra:
      value = *sp--;
      svalue = (sint_t)*sp;
      *sp = (pint_t)(svalue >> value);
      if (log)
        fprintf(stderr, "shift left arithmetric\n");
      break;

    case DW_OP_xor:
      value = *sp--;
      *sp ^= 