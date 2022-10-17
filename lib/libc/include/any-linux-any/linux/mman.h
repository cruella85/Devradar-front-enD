/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _LINUX_MMAN_H
#define _LINUX_MMAN_H

#include <asm/mman.h>
#include <asm-generic/hugetlb_encode.h>

#define MREMAP_MAYMOVE		1
#define MREMAP_FIXED		2
#define MREMAP_DONTUNMAP	4

#define OVERCOMMIT_GUESS		0
#define OVERCOMMIT_ALWAYS		1
#define OVERCOMMIT_NEVER		2

#define MAP_SHARED	0x01		/* Share changes */
#define MAP_PRIVATE	0x02		/* Changes are private */
#define MAP_SHARED_VALIDATE 0x03	/* share + validate extension flags */

/*
 * Huge page size encoding when MAP_HUGETLB is specified, and a huge page
 * size other than the default i