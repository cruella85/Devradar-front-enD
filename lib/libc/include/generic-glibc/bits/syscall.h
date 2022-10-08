/* Generated at libc build time from syscall list.  */
/* The system call list corresponds to kernel 5.13.  */

#ifndef _SYSCALL_H
# error "Never use <bits/syscall.h> directly; include <sys/syscall.h> instead."
#endif

#define __GLIBC_LINUX_VERSION_CODE 331008

#ifdef __NR_FAST_atomic_update
# define SYS_FAST_atomic_update __NR_FAST_atomic_update
#endif

#ifdef __NR_FAST_cmpxchg
# define SYS_FAST_cmpxchg __NR_FAST_cmpxchg
#endif

#ifdef __NR_FAST_cmpxchg64
# define SYS_FAST_cmpxchg64 __NR_FAST_cmpxchg64
#endif

#ifdef __NR__llseek
# define SYS__llseek __NR__llseek
#endif

#ifdef __NR__newselect
# define SYS__newselect __NR__newselect
#endif

#ifdef __NR__sysctl
# define SYS__sysctl __NR__sysctl
#endif

#ifdef __NR_accept
# define SYS_accept __NR_accept
#endif

#ifdef __NR_accept4
# define SYS_accept4 __NR_accept4
#endif

#ifdef __NR_access
# define SYS_access __NR_access
#endif

#ifdef __NR_acct
# define SYS_acct __NR_acct
#endif

#ifdef __NR_acl_get
# define SYS_acl_get __NR_acl_get
#endif

#ifdef __NR_acl_set
# define SYS_acl_set __NR_acl_set
#endif

#ifdef __NR_add_key
# define SYS_add_key __NR_add_key
#endif

#ifdef __NR_adjtimex
# define SYS_adjtimex __NR_adjtimex
#endif

#ifdef __NR_afs_syscall
# define SYS_afs_syscall __NR_afs_syscall
#endif

#ifdef __NR_alarm
# define SYS_alarm __NR_alarm
#endif

#ifdef __NR_alloc_hugepages
# define SYS_alloc_hugepages __NR_alloc_hugepages
#endif

#ifdef __NR_arc_gettls
# define SYS_arc_gettls __NR_arc_gettls
#endif

#ifdef __NR_arc_settls
# define SYS_arc_settls __NR_arc_settls
#endif

#ifdef __NR_arc_usr_cmpxchg
# define SYS_arc_usr_cmpxchg __NR_arc_usr_cmpxchg
#endif

#ifdef __NR_arch_prctl
# define SYS_arch_prctl __NR_arch_prctl
#endif

#ifdef __NR_arm_fadvise64_64
# define SYS_arm_fadvise64_64 __NR_arm_fadvise64_64
#endif

#ifdef __NR_arm_sync_file_range
# define SYS_arm_sync_file_range __NR_arm_sync_file_range
#endif

#ifdef __NR_atomic_barrier
# define SYS_atomic_barrier __NR_atomic_barrier
#endif

#ifdef __NR_atomic_cmpxchg_32
# define SYS_atomic_cmpxchg_32 __NR_atomic_cmpxchg_32
#endif

#ifdef __NR_attrctl
# define SYS_attrctl __NR_attrctl
#endif

#ifdef __NR_bdflush
# define SYS_bdflush __NR_bdflush
#endif

#ifdef __NR_bind
# define SYS_bind __NR_bind
#endif

#ifdef __NR_bpf
# define SYS_bpf __NR_bpf
#endif

#ifdef __NR_break
# define SYS_break __NR_break
#endif

#ifdef __NR_breakpoint
# define SYS_breakpoint __NR_breakpoint
#endif

#ifdef __NR_brk
# define SYS_brk __NR_brk
#endif

#ifdef __NR_cachectl
# define SYS_cachectl __NR_cachectl
#endif

#ifdef __NR_cacheflush
# define SYS_cacheflush __NR_cacheflush
#endif

#ifdef __NR_capget
# define SYS_capget __NR_capget
#endif

#ifdef __NR_capset
# define SYS_capset __NR_capset
#endif

#ifdef __NR_chdir
# define SYS_chdir __NR_chdir
#endif

#ifdef __NR_chmod
# define SYS_chmod __NR_chmod
#endif

#ifdef __NR_chown
# define SYS_chown __NR_chown
#endif

#ifdef __NR_chown32
# define SYS_chown32 __NR_chown32
#endif

#ifdef __NR_chroot
# define SYS_chroot __NR_chroot
#endif

#ifdef __NR_clock_adjtime
# define SYS_clock_adjtime __NR_clock_adjtime
#endif

#ifdef __NR_clock_adjtime64
# define SYS_clock_adjtime64 __NR_clock_adjtime64
#endif

#ifdef __NR_clock_getres
# define SYS_clock_getres __NR_clock_getres
#endif

#ifdef __NR_clock_getres_time64
# define SYS_clock_getres_time64 __NR_clock_getres_time64
#endif

#ifdef __NR_clock_gettime
# define SYS_clock_gettime __NR_clock_gettime
#endif

#ifdef __NR_clock_gettime64
# define SYS_clock_gettime64 __NR_clock_gettime64
#endif

#ifdef __NR_clock_nanosleep
# define SYS_clock_nanosleep __NR_clock_nanosleep
#endif

#ifdef __NR_clock_nanosleep_time64
# define SYS_clock_nanosleep_time64 __NR_clock_nanosleep_time64
#endif

#ifdef __NR_clock_settime
# define SYS_clock_settime __NR_clock_settime
#endif

#ifdef __NR_clock_settime64
# define SYS_clock_settime64 __NR_clock_settime64
#endif

#ifdef __NR_clone
# define SYS_clone __NR_clone
#endif

#ifdef __NR_clone2
# define SYS_clone2 __NR_clone2
#endif

#ifdef __NR_clone3
# define SYS_clone3 __NR_clone3
#endif

#ifdef __NR_close
# define SYS_close __NR_close
#endif

#ifdef __NR_close_range
# define SYS_close_range __NR_close_range
#endif

#ifdef __NR_cmpxchg_badaddr
# define SYS_cmpxchg_badaddr __NR_cmpxchg_badaddr
#endif

#ifdef __NR_connect
# define SYS_connect __NR_connect
#endif

#ifdef __NR_copy_file_range
# define SYS_copy_file_range __NR_copy_file_range
#endif

#ifdef __NR_creat
# define SYS_creat __NR_creat
#endif

#ifdef __NR_create_module
# define SYS_create_module __NR_create_module
#endif

#ifdef __NR_delete_module
# define SYS_delete_module __NR_delete_module
#endif

#ifdef __NR_dipc
# define SYS_dipc __NR_dipc
#endif

#ifdef __NR_dup
# define SYS_dup __NR_dup
#endif

#ifdef __NR_dup2
# define SYS_dup2 __NR_dup2
#endif

#ifdef __NR_dup3
# define SYS_dup3 __NR_dup3
#endif

#ifdef __NR_epoll_create
# define SYS_epoll_create __NR_epoll_create
#endif

#ifdef __NR_epoll_create1
# define SYS_epoll_create1 __NR_epoll_create1
#endif

#ifdef __NR_epoll_ctl
# define SYS_epoll_ctl __NR_epoll_ctl
#endif

#ifdef __NR_epoll_ctl_old
# define SYS_epoll_ctl_old __NR_epoll_ctl_old
#endif

#ifdef __NR_epoll_pwait
# define SYS_epoll_pwait __NR_epoll_pwait
#endif

#ifdef __NR_epoll_pwait2
# define SYS_epoll_pwait2 __NR_epoll_pwait2
#endif

#ifdef __NR_epoll_wait
# define SYS_epoll_wait __NR_epoll_wait
#endif

#ifdef __NR_epoll_wait_old
# define SYS_epoll_wait_old __NR_epoll_wait_old
#endif

#ifdef __NR_eventfd
# define SYS_eventfd __NR_eventfd
#endif

#ifdef __NR_eventfd2
# define SYS_eventfd2 __NR_eventfd2
#endif

#ifdef __NR_exec_with_loader
# define SYS_exec_with_loader __NR_exec_with_loader
#endif

#ifdef __NR_execv
# define SYS_execv __NR_execv
#endif

#ifdef __NR_execve
# define SYS_execve __NR_execve
#endif

#ifdef __NR_execveat
# define SYS_execveat __NR_execveat
#endif

#ifdef __NR_exit
# define SYS_exit __NR_exit
#endif

#ifdef __NR_exit_group
# define SYS_exit_group __NR_exit_group
#endif

#ifdef __NR_faccessat
# define SYS_faccessat __NR_faccessat
#endif

#ifdef __NR_faccessat2
# define SYS_faccessat2 __NR_faccessat2
#endif

#ifdef __NR_fadvise64
# define SYS_fadvise64 __NR_fadvise64
#endif

#ifdef __NR_fadvise64_64
# define SYS_fadvise64_64 __NR_fadvise64_64
#endif

#ifdef __NR_fallocate
# define SYS_fallocate __NR_fallocate
#endif

#ifdef __NR_fanotify_init
# define SYS_fanotify_init __NR_fanotify_init
#endif

#ifdef __NR_fanotify_mark
# define SYS_fanotify_mark __NR_fanotify_mark
#endif

#ifdef __NR_fchdir
# define SYS_fchdir __NR_fchdir
#endif

#ifdef __NR_fchmod
# define SYS_fchmod __NR_fchmod
#endif

#ifdef __NR_fchmodat
# define SYS_fchmodat __NR_fchmodat
#endif

#ifdef __NR_fchown
# define SYS_fchown __NR_fchown
#endif

#ifdef __NR_fchown32
# define SYS_fchown32 __NR_fchown32
#endif

#ifdef __NR_fchownat
# define SYS_fchownat __NR_fchownat
#endif

#ifdef __NR_fcntl
# define SYS_fcntl __NR_fcntl
#endif

#ifdef __NR_fcntl64
# define SYS_fcntl64 __NR_fcntl64
#endif

#ifdef __NR_fdatasync
# define SYS_fdatasync __NR_fdatasync
#endif

#ifdef __NR_fgetxattr
# define SYS_fgetxattr __NR_fgetxattr
#endif

#ifdef __NR_finit_module
# define SYS_finit_module __NR_finit_module
#endif

#ifdef __NR_flistxattr
# define SYS_flistxattr __NR_flistxattr
#endif

#ifdef __NR_flock
# define SYS_flock __NR_flock
#endif

#ifdef __NR_fork
# define SYS_fork __NR_fork
#endif

#ifdef __NR_fp_udfiex_crtl
# define SYS_fp_udfiex_crtl __NR_fp_udfiex_crtl
#endif

#ifdef __NR_free_hugepages
# define SYS_free_hugepages __NR_free_hugepages
#endif

#ifdef __NR_fremovexattr
# define SYS_fremovexattr __NR_fremovexattr
#endif

#ifdef __NR_fsconfig
# define SYS_fsconfig __NR_fsconfig
#endif

#ifdef __NR_fsetxattr
# define SYS_fsetxattr __NR_fsetxattr
#endif

#ifdef __NR_fsmount
# define SYS_fsmount __NR_fsmount
#endif

#ifdef __NR_fsopen
# define SYS_fsopen __NR_fsopen
#endif

#ifdef __NR_fspick
# define SYS_fspick __NR_fspick
#endif

#ifdef __NR_fstat
# define SYS_fstat __NR_fstat
#endif

#ifdef __NR_fstat64
# define SYS_fstat64 __NR_fstat64
#endif

#ifdef __NR_fstatat64
# define SYS_fstatat64 __NR_fstatat64
#endif

#ifdef __NR_fstatfs
# define SYS_fstatfs __NR_fstatfs
#endif

#ifdef __NR_fstatfs64
# define SYS_fstatfs64 __NR_fstatfs64
#endif

#ifdef __NR_fsync
# define SYS_fsync __NR_fsync
#endif

#ifdef __NR_ftime
# define SYS_ftime __NR_ftime
#endif

#ifdef __NR_ftruncate
# define SYS_ftruncate __NR_ftruncate
#endif

#ifdef __NR_ftruncate64
# define SYS_ftruncate64 __NR_ftruncate64
#endif

#ifdef __NR_futex
# define SYS_futex __NR_futex
#endif

#ifdef __NR_futex_time64
# define SYS_futex_time64 __NR_futex_time64
#endif

#ifdef __NR_futimesat
# define SYS_futimesat __NR_futimesat
#endif

#ifdef __NR_get_kernel_syms
# define SYS_get_kernel_syms __NR_get_kernel_syms
#endif

#ifdef __NR_get_mempolicy
# define SYS_get_mempolicy __NR_get_mempolicy
#endif

#ifdef __NR_get_robust_list
# define SYS_get_robust_list __NR_get_robust_list
#endif

#ifdef __NR_get_thread_area
# define SYS_get_thread_area __NR_get_thread_area
#endif

#ifdef __NR_get_tls
# define SYS_get