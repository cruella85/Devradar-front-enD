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
	int32_t                 pth_policy;             /* scheduling policy in effect */
	int32_t                 pth_run_state;          /* run state (see below) */
	int32_t                 pth_flags;              /* various flags (see below) */
	int32_t                 pth_sleep_time;         /* number of seconds that thread */
	int32_t                 pth_curpri;             /* cur priority*/
	int32_t                 pth_priority;           /*  priority*/
	int32_t                 pth_maxpriority;        /* max priority*/
	char                    pth_name[MAXTHREADNAMESIZE];    /* thread name, if any */
};

struct proc_regioninfo {
	uint32_t                pri_protection;
	uint32_t                pri_max_protection;
	uint32_t                pri_inheritance;
	uint32_t                pri_flags;              /* shared, external pager, is submap */
	uint64_t                pri_offset;
	uint32_t                pri_behavior;
	uint32_t                pri_user_wired_count;
	uint32_t                pri_user_tag;
	uint32_t                pri_pages_resident;
	uint32_t                pri_pages_shared_now_private;
	uint32_t                pri_pages_swapped_out;
	uint32_t                pri_pages_dirtied;
	uint32_t                pri_ref_count;
	uint32_t                pri_shadow_depth;
	uint32_t                pri_share_mode;
	uint32_t                pri_private_pages_resident;
	uint32_t                pri_shared_pages_resident;
	uint32_t                pri_obj_id;
	uint32_t                pri_depth;
	uint64_t                pri_address;
	uint64_t                pri_size;
};

#define PROC_REGION_SUBMAP      1
#define PROC_REGION_SHARED      2

#define SM_COW             1
#define SM_PRIVATE         2
#define SM_EMPTY           3
#define SM_SHARED          4
#define SM_TRUESHARED      5
#define SM_PRIVATE_ALIASED 6
#define SM_SHARED_ALIASED  7
#define SM_LARGE_PAGE      8


/*
 *	Thread run states (state field).
 */

#define TH_STATE_RUNNING        1       /* thread is running normally */
#define TH_STATE_STOPPED        2       /* thread is stopped */
#define TH_STATE_WAITING        3       /* thread is waiting normally */
#define TH_STATE_UNINTERRUPTIBLE 4      /* thread is in an uninterruptible
	                                 *  wait */
#define TH_STATE_HALTED         5       /* thread is halted at a
	                                 *  clean point */

/*
 *	Thread flags (flags field).
 */
#define TH_FLAGS_SWAPPED        0x1     /* thread is swapped out */
#define TH_FLAGS_IDLE           0x2     /* thread is an idle thread */


struct proc_workqueueinfo {
	uint32_t        pwq_nthreads;           /* total number of workqueue threads */
	uint32_t        pwq_runthreads;         /* total number of running workqueue threads */
	uint32_t        pwq_blockedthreads;     /* total number of blocked workqueue threads */
	uint32_t        pwq_state;
};

/*
 *	workqueue state (pwq_state field)
 */
#define WQ_EXCEEDED_CONSTRAINED_THREAD_LIMIT 0x1
#define WQ_EXCEEDED_TOTAL_THREAD_LIMIT 0x2
#define WQ_FLAGS_AVAILABLE 0x4

struct proc_fileinfo {
	uint32_t                fi_openflags;
	uint32_t                fi_status;
	off_t                   fi_offset;
	int32_t                 fi_type;
	uint32_t                fi_guardflags;
};

/* stats flags in proc_fileinfo */
#define PROC_FP_SHARED  1       /* shared by more than one fd */
#define PROC_FP_CLEXEC  2       /* close on exec */
#define PROC_FP_GUARDED 4       /* guarded fd */
#define PROC_FP_CLFORK  8       /* close on fork */

#define PROC_FI_GUARD_CLOSE             (1u << 0)
#define PROC_FI_GUARD_DUP               (1u << 1)
#define PROC_FI_GUARD_SOCKET_IPC        (1u << 2)
#define PROC_FI_GUARD_FILEPORT          (1u << 3)

struct proc_exitreasonbasicinfo {
	uint32_t                        beri_namespace;
	uint64_t                        beri_code;
	uint64_t                        beri_flags;
	uint32_t                        beri_reason_buf_size;
} __attribute__((packed));

struct proc_exitreasoninfo {
	uint32_t                        eri_namespace;
	uint64_t                        eri_code;
	uint64_t                        eri_flags;
	uint32_t                        eri_reason_buf_size;
	uint64_t                        eri_kcd_buf;
} __attribute__((packed));

/*
 * A copy of stat64 with static sized fields.
 */
struct vinfo_stat {
	uint32_t        vst_dev;        /* [XSI] ID of device containing file */
	uint16_t        vst_mode;       /* [XSI] Mode of file (see below) */
	uint16_t        vst_nlink;      /* [XSI] Number of hard links */
	uint64_t        vst_ino;        /* [XSI] File serial number */
	uid_t           vst_uid;        /* [XSI] User ID of the file */
	gid_t           vst_gid;        /* [XSI] Group ID of the file */
	int64_t         vst_atime;      /* [XSI] Time of last access */
	int64_t         vst_atimensec;  /* nsec of last access */
	int64_t         vst_mtime;      /* [XSI] Last data modification time */
	int64_t         vst_mtimensec;  /* last data modification nsec */
	int64_t         vst_ctime;      /* [XSI] Time of last status change */
	int64_t         vst_ctimensec;  /* nsec of last status change */
	int64_t         vst_birthtime;  /*  File creation time(birth)  */
	int64_t         vst_birthtimensec;      /* nsec of File creation time */
	off_t           vst_size;       /* [XSI] file size, in bytes */
	int64_t         vst_blocks;     /* [XSI] blocks allocated for file */
	int32_t         vst_blksize;    /* [XSI] optimal blocksize for I/O */
	uint32_t        vst_flags;      /* user defined flags for file */
	uint32_t        vst_gen;        /* file generation number */
	uint32_t        vst_rdev;       /* [XSI] Device ID */
	int64_t         vst_qspare[2];  /* RESERVED: DO NOT USE! */
};

struct vnode_info {
	struct vinfo_stat       vi_stat;
	int                     vi_type;
	int                     vi_pad;
	fsid_t                  vi_fsid;
};

struct vnode_info_path {
	struct vnode_info       vip_vi;
	char                    vip_path[MAXPATHLEN];   /* tail end of it  */
};

struct vnode_fdinfo {
	struct proc_fileinfo    pfi;
	struct vnode_info       pvi;
};

struct vnode_fdinfowithpath {
	struct proc_fileinfo    pfi;
	struct vnode_info_path  pvip;
};

struct proc_regionwithpathinfo {
	struct proc_regioninfo  prp_prinfo;
	struct vnode_info_path  prp_vip;
};

struct proc_regionpath {
	uint64_t prpo_addr;
	uint64_t prpo_regionlength;
	char prpo_path[MAXPATHLEN];
};

struct proc_vnodepathinfo {
	struct vnode_info_path  pvi_cdir;
	struct vnode_info_path  pvi_rdir;
};

struct proc_threadwithpathinfo {
	struct proc_threadinfo  pt;
	struct vnode_info_path  pvip;
};

/*
 *  Socket
 */


/*
 * IPv4 and IPv6 Sockets
 */

#define INI_IPV4        0x1
#define INI_IPV6        0x2

struct in4in6_addr {
	u_int32_t               i46a_pad32[3];
	struct in_addr          i46a_addr4;
};

struct in_sockinfo {
	int                                     insi_fport;             /* foreign port */
	int                                     insi_lport;             /* local port */
	uint64_t                                insi_gencnt;            /* generation count of this instance */
	uint32_t                                insi_flags;             /* generic IP/datagram flags */
	uint32_t                                insi_flow;

	uint8_t                                 insi_vflag;             /* ini_IPV4 or ini_IPV6 */
	uint8_t                                 insi_ip_ttl;            /* time to live proto */
	uint32_t                                rfu_1;                  /* reserved */
	/* protocol dependent part */
	union {
		struct in4in6_addr      ina_46;
		struct in6_addr         ina_6;
	}                                       insi_faddr;             /* foreign host table entry */
	union {
		struct in4in6_addr      ina_46;
		struct in6_addr         ina_6;
	}                                       insi_laddr;             /* local host table entry */
	struct {
		u_char                  in4_tos;                        /* type of service */
	}                                       insi_v4;
	struct {
		uint8_t                 in6_hlim;
		int                     in6_cksum;
		u_short                 in6_ifindex;
		short                   in6_hops;
	}                                       insi_v6;
};

/*
 * TCP Sockets
 */

#define TSI_T_REXMT             0       /* retransmit */
#define TSI_T_PERSIST           1       /* retransmit persistence */
#define TSI_T_KEEP              2       /* keep alive */
#define TSI_T_2MSL              3       /* 2*msl quiet time timer */
#define TSI_T_NTIMERS           4

#define TSI_S_CLOSED            0       /* closed */
#define TSI_S_LISTEN            1       /* listening for connection */
#define TSI_S_SYN_SENT          2       /* active, have sent syn */
#define TSI_S_SYN_RECEIVED      3       /* have send and received syn */
#define