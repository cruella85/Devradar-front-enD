/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef __MIPS_UAPI_ASM_UCONTEXT_H
#define __MIPS_UAPI_ASM_UCONTEXT_H

/**
 * struct extcontext - extended context header structure
 * @magic:	magic value identifying the type of extended context
 * @size:	the size in bytes of the enclosing structure
 *
 * Extended context structures provide context which does not fit within struct
 * sigcontext. They are placed sequentially in memory at the end of struct
 * ucontext and struct sigframe, with each extended context structure beginning
 * with a header defined by this struct. The type of context represented is
 * indicated by the magic field. Userland may check each extended context
 * structure against magic values that it recognises.