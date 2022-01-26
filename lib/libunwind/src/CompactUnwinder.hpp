
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
//  Does runtime stack unwinding using compact unwind encodings.
//
//===----------------------------------------------------------------------===//

#ifndef __COMPACT_UNWINDER_HPP__
#define __COMPACT_UNWINDER_HPP__

#include <stdint.h>
#include <stdlib.h>

#include <libunwind.h>
#include <mach-o/compact_unwind_encoding.h>

#include "Registers.hpp"

#define EXTRACT_BITS(value, mask)                                              \
  ((value >> __builtin_ctz(mask)) & (((1 << __builtin_popcount(mask))) - 1))

namespace libunwind {

#if defined(_LIBUNWIND_TARGET_I386)
/// CompactUnwinder_x86 uses a compact unwind info to virtually "step" (aka
/// unwind) by modifying a Registers_x86 register set
template <typename A>
class CompactUnwinder_x86 {
public:

  static int stepWithCompactEncoding(compact_unwind_encoding_t info,
                                     uint32_t functionStart, A &addressSpace,
                                     Registers_x86 &registers);

private:
  typename A::pint_t pint_t;

  static void frameUnwind(A &addressSpace, Registers_x86 &registers);
  static void framelessUnwind(A &addressSpace,
                              typename A::pint_t returnAddressLocation,
                              Registers_x86 &registers);
  static int
      stepWithCompactEncodingEBPFrame(compact_unwind_encoding_t compactEncoding,
                                      uint32_t functionStart, A &addressSpace,
                                      Registers_x86 &registers);
  static int stepWithCompactEncodingFrameless(
      compact_unwind_encoding_t compactEncoding, uint32_t functionStart,
      A &addressSpace, Registers_x86 &registers, bool indirectStackSize);
};

template <typename A>
int CompactUnwinder_x86<A>::stepWithCompactEncoding(
    compact_unwind_encoding_t compactEncoding, uint32_t functionStart,
    A &addressSpace, Registers_x86 &registers) {
  switch (compactEncoding & UNWIND_X86_MODE_MASK) {
  case UNWIND_X86_MODE_EBP_FRAME:
    return stepWithCompactEncodingEBPFrame(compactEncoding, functionStart,
                                           addressSpace, registers);
  case UNWIND_X86_MODE_STACK_IMMD:
    return stepWithCompactEncodingFrameless(compactEncoding, functionStart,
                                            addressSpace, registers, false);
  case UNWIND_X86_MODE_STACK_IND:
    return stepWithCompactEncodingFrameless(compactEncoding, functionStart,
                                            addressSpace, registers, true);
  }
  _LIBUNWIND_ABORT("invalid compact unwind encoding");
}

template <typename A>
int CompactUnwinder_x86<A>::stepWithCompactEncodingEBPFrame(
    compact_unwind_encoding_t compactEncoding, uint32_t functionStart,
    A &addressSpace, Registers_x86 &registers) {
  uint32_t savedRegistersOffset =
      EXTRACT_BITS(compactEncoding, UNWIND_X86_EBP_FRAME_OFFSET);
  uint32_t savedRegistersLocations =
      EXTRACT_BITS(compactEncoding, UNWIND_X86_EBP_FRAME_REGISTERS);

  uint32_t savedRegisters = registers.getEBP() - 4 * savedRegistersOffset;
  for (int i = 0; i < 5; ++i) {
    switch (savedRegistersLocations & 0x7) {
    case UNWIND_X86_REG_NONE:
      // no register saved in this slot
      break;
    case UNWIND_X86_REG_EBX:
      registers.setEBX(addressSpace.get32(savedRegisters));
      break;
    case UNWIND_X86_REG_ECX:
      registers.setECX(addressSpace.get32(savedRegisters));
      break;
    case UNWIND_X86_REG_EDX:
      registers.setEDX(addressSpace.get32(savedRegisters));
      break;
    case UNWIND_X86_REG_EDI:
      registers.setEDI(addressSpace.get32(savedRegisters));
      break;
    case UNWIND_X86_REG_ESI:
      registers.setESI(addressSpace.get32(savedRegisters));
      break;
    default:
      (void)functionStart;
      _LIBUNWIND_DEBUG_LOG("bad register for EBP frame, encoding=%08X for  "
                           "function starting at 0x%X",
                            compactEncoding, functionStart);
      _LIBUNWIND_ABORT("invalid compact unwind encoding");
    }
    savedRegisters += 4;
    savedRegistersLocations = (savedRegistersLocations >> 3);
  }
  frameUnwind(addressSpace, registers);
  return UNW_STEP_SUCCESS;
}

template <typename A>
int CompactUnwinder_x86<A>::stepWithCompactEncodingFrameless(
    compact_unwind_encoding_t encoding, uint32_t functionStart,
    A &addressSpace, Registers_x86 &registers, bool indirectStackSize) {
  uint32_t stackSizeEncoded =
      EXTRACT_BITS(encoding, UNWIND_X86_FRAMELESS_STACK_SIZE);
  uint32_t stackAdjust =
      EXTRACT_BITS(encoding, UNWIND_X86_FRAMELESS_STACK_ADJUST);
  uint32_t regCount =
      EXTRACT_BITS(encoding, UNWIND_X86_FRAMELESS_STACK_REG_COUNT);
  uint32_t permutation =
      EXTRACT_BITS(encoding, UNWIND_X86_FRAMELESS_STACK_REG_PERMUTATION);
  uint32_t stackSize = stackSizeEncoded * 4;
  if (indirectStackSize) {
    // stack size is encoded in subl $xxx,%esp instruction
    uint32_t subl = addressSpace.get32(functionStart + stackSizeEncoded);
    stackSize = subl + 4 * stackAdjust;
  }
  // decompress permutation
  uint32_t permunreg[6];
  switch (regCount) {
  case 6:
    permunreg[0] = permutation / 120;
    permutation -= (permunreg[0] * 120);
    permunreg[1] = permutation / 24;
    permutation -= (permunreg[1] * 24);
    permunreg[2] = permutation / 6;
    permutation -= (permunreg[2] * 6);
    permunreg[3] = permutation / 2;
    permutation -= (permunreg[3] * 2);
    permunreg[4] = permutation;
    permunreg[5] = 0;
    break;
  case 5:
    permunreg[0] = permutation / 120;
    permutation -= (permunreg[0] * 120);
    permunreg[1] = permutation / 24;
    permutation -= (permunreg[1] * 24);
    permunreg[2] = permutation / 6;
    permutation -= (permunreg[2] * 6);