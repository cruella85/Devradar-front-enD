
/*
 * Copyright (c) 2000-2018 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 *
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/* Copyright 1995 NeXT Computer, Inc. All rights reserved. */
/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */

#ifndef _CDEFS_H_
#define _CDEFS_H_

#if defined(__cplusplus)
#define __BEGIN_DECLS   extern "C" {
#define __END_DECLS     }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

/* This SDK is designed to work with clang and specific versions of
 * gcc >= 4.0 with Apple's patch sets */
#if !defined(__GNUC__) || __GNUC__ < 4
#warning "Unsupported compiler detected"
#endif

/*
 * Compatibility with compilers and environments that don't support compiler
 * feature checking function-like macros.
 */
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif
#ifndef __has_include
#define __has_include(x) 0
#endif
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
#ifndef __has_extension
#define __has_extension(x) 0
#endif

/*
 * The __CONCAT macro is used to concatenate parts of symbol names, e.g.
 * with "#define OLD(foo) __CONCAT(old,foo)", OLD(foo) produces oldfoo.
 * The __CONCAT macro is a bit tricky -- make sure you don't put spaces
 * in between its arguments.  __CONCAT can also concatenate double-quoted
 * strings produced by the __STRING macro, but this only works with ANSI C.
 */
#if defined(__STDC__) || defined(__cplusplus)
#define __P(protos)     protos          /* full-blown ANSI C */
#define __CONCAT(x, y)   x ## y
#define __STRING(x)     #x

#define __const         const           /* define reserved names to standard */
#define __signed        signed
#define __volatile      volatile
#if defined(__cplusplus)
#define __inline        inline          /* convert to C++ keyword */
#else
#ifndef __GNUC__
#define __inline                        /* delete GCC keyword */
#endif /* !__GNUC__ */
#endif /* !__cplusplus */

#else   /* !(__STDC__ || __cplusplus) */
#define __P(protos)     ()              /* traditional C preprocessor */
#define __CONCAT(x, y)   x /**/ y
#define __STRING(x)     "x"

#ifndef __GNUC__
#define __const                         /* delete pseudo-ANSI C keywords */
#define __inline
#define __signed
#define __volatile
#endif  /* !__GNUC__ */

/*
 * In non-ANSI C environments, new programs will want ANSI-only C keywords
 * deleted from the program and old programs will want them left alone.
 * When using a compiler other than gcc, programs using the ANSI C keywords
 * const, inline etc. as normal identifiers should define -DNO_ANSI_KEYWORDS.
 * When using "gcc -traditional", we assume that this is the intent; if
 * __GNUC__ is defined but __STDC__ is not, we leave the new keywords alone.
 */
#ifndef NO_ANSI_KEYWORDS
#define const           __const                 /* convert ANSI C keywords */
#define inline          __inline
#define signed          __signed
#define volatile        __volatile
#endif /* !NO_ANSI_KEYWORDS */
#endif /* !(__STDC__ || __cplusplus) */

/*
 * __pure2 can be used for functions that are only a function of their scalar
 * arguments (meaning they can't dereference pointers).
 *
 * __stateful_pure can be used for functions that have no side effects,
 * but depend on the state of the memory.
 */
#define __dead2         __attribute__((__noreturn__))
#define __pure2         __attribute__((__const__))
#define __stateful_pure __attribute__((__pure__))

/* __unused denotes variables and functions that may not be used, preventing
 * the compiler from warning about it if not used.
 */
#define __unused        __attribute__((__unused__))

/* __used forces variables and functions to be included even if it appears
 * to the compiler that they are not used (and would thust be discarded).
 */
#define __used          __attribute__((__used__))

/* __cold marks code used for debugging or that is rarely taken
 * and tells the compiler to optimize for size and outline code.
 */
#if __has_attribute(cold)
#define __cold          __attribute__((__cold__))
#else
#define __cold
#endif

/* __exported denotes symbols that should be exported even when symbols
 * are hidden by default.
 * __exported_push/_exported_pop are pragmas used to delimit a range of
 *  symbols that should be exported even when symbols are hidden by default.
 */
#define __exported      __attribute__((__visibility__("default")))
#define __exported_push _Pragma("GCC visibility push(default)")