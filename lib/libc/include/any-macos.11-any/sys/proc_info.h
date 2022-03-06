/*
 * Copyright (c) 2005-2020 Apple Computer, Inc. All rights reserved.
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

#ifndef _SYS_PROC_INFO_H
#define _SYS_PROC_INFO_H

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/kern_control.h>
#include <sys/event.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <mach/machine.h>
#include <uuid/uuid.h>


__BEGIN_DECLS


#define PROC_ALL_PIDS           1
#define PROC_PGRP_ONLY          2
#define PROC_TTY_ONLY           3
#define PROC_UID_ONLY           4
#define PROC_RUID_ONLY          5
#define PROC_PPID_ONLY          6
#define PROC_KDBG_ONLY          7

struct proc_bsdinfo {
	uint32_t                pbi_flags;              /* 64bit; emulated etc */
	uint32_t                pbi_status;
	uint32_t                pbi_xstatus;
	uint32_t                pbi_pid;
	uint32_t                pbi_ppid;
	uid_t                   pbi_uid;
	gid_t                   pbi_gid;
	uid_t                   pbi_ruid;
	gid_t                   pbi_rgid;
	uid_t                   pbi_svuid;
	gid_t                   pbi_svgid;
	uint32_t                rfu_1;                  /* reserved */
	char                    pbi_comm[MAXCOMLEN];
	char                    pbi_name[2 * MAXCOMLEN];  /* empty if no name is registered */
	uint32_t                pbi_nfiles;
	uint32_t                pbi_pgid;
	uint32_t                pbi_pjobc;
	uint32_t                e_tdev;                 /* controlling tty dev */
	uint32_t                e_tpgid;                /* tty process group id */
	int32_t                 pbi_nice;
	uint64_t                pbi_start_tvsec;
	uint64_t                pbi_start_tvusec;
};


struct proc_bsdshortinfo {
	uint32_t                pbsi_pid;               /* process id */
	uint32_t                pbsi_ppid;              /* process parent id */
	uint32_t                pbsi_pgid;              /* process perp id */
	uint32_t                pbsi_status;            /* p_stat value, SZOMB, SRUN, etc */
	char                    pbsi_comm[MAXCOMLEN];   /* upto 16 characters of process name */
	uint32_t                pbsi_flags;              /* 64bit; emulated etc */
	uid_t                   pbsi_uid;               /* current uid on process */
	gid_t                   pbsi_gid;               /* current gid on process */
	uid_t                   pbsi_ruid;              /* current ruid on process */
	gid_t                   pbsi_rgid;              /* current tgid on process */
	uid_t                   pbsi_svuid;             /* current svuid on process */
	gid_t                   pbsi_svgid;             /* current svgid on process */
	uint32_t                pbsi_rfu;               /* reserved for future use*/
};




/* pbi_flags values */
#define PROC_FLAG_SYSTEM        1       /*  System process */
#define PROC_FLAG_TRACED        2       /* process currently being traced, possibly by gdb */
#define PROC_FLAG_INEXIT        4       /* process is working its way in exit() */
#define PROC_FLAG_PPWAIT        8
#define PROC_FLAG_LP64          0x10    /* 64bit process */
#define PROC_FLAG_SLEADER       0x20    /* The process is the session leader */
#define PROC_FLAG_CTTY          0x40    /* process has a control tty */
#define PROC_FLAG_CONTROLT      0x80    /* Has a controlling terminal */
#define PROC_FLAG_THCWD         0x100   /* process has a thread with cwd */
/* process control bits for resource starvation */
#define PROC_FLAG_PC_THROTTLE   0x200   /* In resource starvation situations, this process is to be throttled */
#define PROC_FLAG_PC_SUSP       0x400   /* In resource starvation situations, this process is to be suspended */
#define PROC_FLAG_PC_KILL       0x600   /* In resource starvation situations, this process is to be terminated */
#define PROC_FLAG_PC_MASK       0x600
/* process action bits for resource starvation */
#define PROC_FLAG_PA_THROTTLE   0x800   /* The process is currently throttled due to resource starvation */
#define PROC_FLAG_PA_SUSP       0x1000  /* The process is currently suspended due to resource starvation */
#define PROC_FLAG_PSUGID        0x2000   /* process has set privileges since last exec */
#define PROC_FLAG_EXEC          0x4000   /* process has called exec  */


struct proc_taskinfo {
	uint64_t                pti_virtual_size;       /* virtual memory size (bytes) */
	uint64_t                pti_resident_size;      /* resident memory size (bytes) */
	uint64_t                pti_total_user;         /* total time */
	uint64_t                pti_total_system;
	uint64_t                pti_threads_user;       /* existing threads only */
	uint64_t                pti_threads_system;
	int32_t                 pti_policy;             /* default policy for new threads */
	int32_t                 pti_faults;             /* number of page faults */
	int32_t                 pti_pageins;            /* number of actual pageins */
	int32_t                 pti_cow_faults;         /* number of copy-on-write faults */
	int32_t                 pti_messages_sent;      /* number of messages sent */
	int32_t                 pti_messages_received;  /* number of messages received */
	int32_t                 pti_syscalls_mach;      /* number of mach system calls */
	int32_t                 pti_syscalls_unix;      /* number of unix system calls */
	int32_t                 pti_csw;                /* number of context switches */
	int32_t                 pti_threadnum;          /* number of threads in the task */
	int32_t                 pti_numrunning;         /* number of running threads */
	int32_t                 pti_priority;           /* task priority*/
};

struct proc_taskallinfo {
	struct proc_bsdinfo     pbsd;
	struct proc_taskinfo    ptinfo;
};

#define MAXTHREADNAMESIZE 64

struct proc_threadinfo {
	uint64_t                pth_user_time;          /* user run time */
	uint64_t                pth_system_time;        /* system run time */
	int32_t                 pth_cpu_usage;          /* scaled cpu usage percentage */
	int32_t                 pth_policy