//===-- sanitizer_procmaps_common.cpp -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Information about the process mappings (common parts).
//===----------------------------------------------------------------------===//

#include "sanitizer_platform.h"

#if SANITIZER_FREEBSD || SANITIZER_LINUX || SANITIZER_NETBSD ||                \
    SANITIZER_SOLARIS

#include "sanitizer_common.h"
#include "sanitizer_placement_new.h"
#include "sanitizer_procmaps.h"

namespace __sanitizer {

static ProcSelfMapsBuff cached_proc_self_maps;
static StaticSpinMutex cache_lock;

static int TranslateDigit(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

// Parse a number and promote 'p' up to the first non-digit character.
static uptr ParseNumber(const char **p, int base) {
  uptr n = 0;
  int d;
  CHECK(base >= 2 && base <= 16);
  while ((d = TranslateDigit(**p)) >= 0 && d < base) {
    n = n * base + d;
    (*p)++;
  }
  return n;
}

bool IsDecimal(char c) {
  int d = TranslateDigit(c);
  return d >= 0 && d < 10;
}

uptr ParseDecimal(const char **p) {
  return ParseNumber(p, 10);
}

bool IsHex(char c) {
  int d = TranslateDigit(c);
  return d >= 0 && d < 16;
}

uptr ParseHex(const char **p) {
  return ParseNumber(p, 16);
}

void MemoryMappedSegment::AddAddressRanges(LoadedModule *module) {
  // data_ should be unused on this platform
  CHECK(!data_);
  module->addAddressRange(start, end, IsExecutable(), IsWritable());
}

MemoryMappingLayout::MemoryMappingLayout(bool cache_enabled) {
  // FIXME: in the future we may want to cache the mappings on demand only.
  if (cache_enabled)
    CacheMemoryMappings();

  // Read maps after the cache update to capture the maps/unmaps happening in
  // the process of updating.
  ReadProcMaps(&data_.proc_self_maps);
  if (cache_enabled && data_.proc_self_maps.mmaped_size == 0)
    LoadFromCache();

  Reset();
}

bool MemoryMappingLayout::Error() const {
  return data_.current == nullptr;
}

MemoryMappingLayout::~MemoryMappingLayout() {
  // Only unmap the buffer if it is different from the cached one. Otherwise
  // it will be unmapped when the cache is refreshed.
  if (data_.proc_self_maps.data != cached_proc_self_maps.data)
    UnmapOrDie(data_.pro