/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 *  S390 version
 *    Copyright IBM Corp. 1999, 2000
 */

#ifndef _ASM_S390_SIGCONTEXT_H
#define _ASM_S390_SIGCONTEXT_H


#include <linux/types.h>

#define __NUM_GPRS		16
#define __NUM_FPRS		16
#define __NUM_ACRS		16
#define __NUM_VXRS		32
#define __NUM_VXRS_LOW		16
#define __NUM_VXRS_HIGH		16

#ifndef __s390x__

/* Has to be at least _NSIG_WORDS from asm/signal.h */
#define _SIGCONTEXT_NSIG	64
#define _SIGCONTEXT_NSIG_BPW	32
/* Size of stack frame allocated when calling signal handler. */
#define __SIGNAL_FRAMESIZE	96

#else /* __s390x__ */

/* Has to be at least _NSIG_WORDS from asm/signal.h */
#define _SIGCONTEXT_NSIG	64
#define _SIGCONTEXT_NSIG_BPW	64 
/* Size of stack frame allocated when calling signal handler. */
#define __SIGNAL_FRAMESIZE	160

#endif /* __s390x__ */

#define _SIGCONTEXT_NSIG_WORDS	(_SIGCONTEXT_NSIG / _SIGCONTEXT_NSIG_BPW)
#define _SIGMASK_COPY_SIZE	(sizeof(unsigned long)*_SIGCONTEXT_NSIG_WORDS)

typedef struct 
{
        unsigned long mask;
        unsigned long addr;
} __