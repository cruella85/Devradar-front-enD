//===-- sanitizer_syscalls_netbsd.inc ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Common syscalls handlers for tools like AddressSanitizer,
// ThreadSanitizer, MemorySanitizer, etc.
//
// This file should be included into the tool's interceptor file,
// which has to define it's own macros:
//   COMMON_SYSCALL_PRE_READ_RANGE
//          Called in prehook for regions that will be read by the kernel and
//          must be initialized.
//   COMMON_SYSCALL_PRE_WRITE_RANGE
//          Called in prehook for regions that will be written to by the kernel
//          and must be addressable. The actual write range may be smaller than
//          reported in the prehook. See POST_WRITE_RANGE.
//   COMMON_SYSCALL_POST_READ_RANGE
//          Called in posthook for regions that were read by the kernel. Does
//          not make much sense.
//   COMMON_SYSCALL_POST_WRITE_RANGE
//          Called in posthook for regions that were written to by the kernel
//          and are now initialized.
//   COMMON_SYSCALL_ACQUIRE(addr)
//          Acquire memory visibility from addr.
//   COMMON_SYSCALL_RELEASE(addr)
//          Release memory visibility to addr.
//   COMMON_SYSCALL_FD_CLOSE(fd)
//          Called before closing file descriptor fd.
//   COMMON_SYSCALL_FD_ACQUIRE(fd)
//          Acquire memory visibility from fd.
//   COMMON_SYSCALL_FD_RELEASE(fd)
//          Release memory visibility to fd.
//   COMMON_SYSCALL_PRE_FORK()
//          Called before fork syscall.
//   COMMON_SYSCALL_POST_FORK(long long res)
//          Called after fork syscall.
//
// DO NOT EDIT! THIS FILE HAS BEEN GENERATED!
//
// Generated with: generate_netbsd_syscalls.awk
// Generated date: 2020-09-10
// Generated from: syscalls.master,v 1.306 2020/08/14 00:53:16 riastradh Exp
//
//===----------------------------------------------------------------------===//

#include "sanitizer_platform.h"
#if SANITIZER_NETBSD

#include "sanitizer_libc.h"

#define PRE_SYSCALL(name)                                                      \
  SANITIZER_INTERFACE_ATTRIBUTE void __sanitizer_syscall_pre_impl_##name
#define PRE_READ(p, s) COMMON_SYSCALL_PRE_READ_RANGE(p, s)
#define PRE_WRITE(p, s) COMMON_SYSCALL_PRE_WRITE_RANGE(p, s)

#define POST_SYSCALL(name)                                                     \
  SANITIZER_INTERFACE_ATTRIBUTE void __sanitizer_syscall_post_impl_##name
#define POST_READ(p, s) COMMON_SYSCALL_POST_READ_RANGE(p, s)
#define POST_WRITE(p, s) COMMON_SYSCALL_POST_WRITE_RANGE(p, s)

#ifndef COMMON_SYSCALL_ACQUIRE
#define COMMON_SYSCALL_ACQUIRE(addr) ((void)(addr))
#endif

#ifndef COMMON_SYSCALL_RELEASE
#define COMMON_SYSCALL_RELEASE(addr) ((void)(addr))
#endif

#ifndef COMMON_SYSCALL_FD_CLOSE
#define COMMON_SYSCALL_FD_CLOSE(fd) ((void)(fd))
#endif

#ifndef COMMON_SYSCALL_FD_ACQUIRE
#define COMMON_SYSCALL_FD_ACQUIRE(fd) ((void)(fd))
#endif

#ifndef COMMON_SYSCALL_FD_RELEASE
#define COMMON_SYSCALL_FD_RELEASE(fd) ((void)(fd))
#endif

#ifndef COMMON_SYSCALL_PRE_FORK
#define COMMON_SYSCALL_PRE_FORK()                                              \
  {}
#endif

#ifndef COMMON_SYSCALL_POST_FORK
#define COMMON_SYSCALL_POST_FORK(res)                                          \
  {}
#endif

// FIXME: do some kind of PRE_READ for all syscall arguments (int(s) and such).

extern "C" {
#define SYS_MAXSYSARGS 8
PRE_SYSCALL(syscall)(long long code_, long long args_[SYS_MAXSYSARGS]) {
  /* Nothing to do */
}
POST_SYSCALL(syscall)
(long long res, long long code_, long long args_[SYS_MAXSYSARGS]) {
  /* Nothing to do */
}
PRE_SYSCALL(exit)(long long rval_) { /* Nothing to do */ }
POST_SYSCALL(exit)(long long res, long long rval_) { /* Nothing to do */ }
PRE_SYSCALL(fork)(void) { COMMON_SYSCALL_PRE_FORK(); }
POST_SYSCALL(fork)(long long res) { COMMON_SYSCALL_POST_FORK(res); }
PRE_SYSCALL(read)(long long fd_, void *buf_, long long nbyte_) {
  if (buf_) {
    PRE_WRITE(buf_, nbyte_);
  }
}
POST_SYSCALL(read)(long long res, long long fd_, void *buf_, long long nbyte_) {
  if (res > 0) {
    POST_WRITE(buf_, res);
  }
}
PRE_SYSCALL(write)(long long fd_, void *buf_, long long nbyte_) {
  if (buf_) {
    PRE_READ(buf_, nbyte_);
  }
}
POST_SYSCALL(write)
(long long res, long long fd_, void *buf_, long long nbyte_) {
  if (res > 0) {
    POST_READ(buf_, res);
  }
}
PRE_SYSCALL(open)(void *path_, long long flags_, long long mode_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(open)
(long long res, void *path_, long long flags_, long long mode_) {
  if (res > 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(close)(long long fd_) { COMMON_SYSCALL_FD_CLOSE((int)fd_); }
POST_SYSCALL(close)(long long res, long long fd_) { /* Nothing to do */ }
PRE_SYSCALL(compat_50_wait4)
(long long pid_, void *status_, long long options_, void *rusage_) {
  /* TODO */
}
POST_SYSCALL(compat_50_wait4)
(long long res, long long pid_, void *status_, long long options_,
  void *rusage_) {
  /* TODO */
}
PRE_SYSCALL(compat_43_ocreat)(void *path_, long long mode_) { /* TODO */ }
POST_SYSCALL(compat_43_ocreat)(long long res, void *path_, long long mode_) {
  /* TODO */
}
PRE_SYSCALL(link)(void *path_, void *link_) {
  const char *path = (const char *)path_;
  const char *link = (const char *)link_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
  if (link) {
    PRE_READ(path, __sanitizer::internal_strlen(link) + 1);
  }
}
POST_SYSCALL(link)(long long res, void *path_, void *link_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    const char *link = (const char *)link_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
    if (link) {
      POST_READ(path, __sanitizer::internal_strlen(link) + 1);
    }
  }
}
PRE_SYSCALL(unlink)(void *path_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(unlink)(long long res, void *path_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
/* syscall 11 has been skipped */
PRE_SYSCALL(chdir)(void *path_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(chdir)(long long res, void *path_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(fchdir)(long long fd_) { /* Nothing to do */ }
POST_SYSCALL(fchdir)(long long res, long long fd_) { /* Nothing to do */ }
PRE_SYSCALL(compat_50_mknod)(void *path_, long long mode_, long long dev_) {
  /* TODO */
}
POST_SYSCALL(compat_50_mknod)
(long long res, void *path_, long long mode_, long long dev_) {
  /* TODO */
}
PRE_SYSCALL(chmod)(void *path_, long long mode_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(chmod)(long long res, void *path_, long long mode_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(chown)(void *path_, long long uid_, long long gid_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(chown)
(long long res, void *path_, long long uid_, long long gid_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(break)(void *nsize_) { /* Nothing to do */ }
POST_SYSCALL(break)(long long res, void *nsize_) { /* Nothing to do */ }
PRE_SYSCALL(compat_20_getfsstat)
(void *buf_, long long bufsize_, long long flags_) {
  /* TODO */
}
POST_SYSCALL(compat_20_getfsstat)
(long long res, void *buf_, long long bufsize_, long long flags_) {
  /* TODO */
}
PRE_SYSCALL(compat_43_olseek)
(long long fd_, long long offset_, long long whence_) {
  /* TODO */
}
POST_SYSCALL(compat_43_olseek)
(long long res, long long fd_, long long offset_, long long whence_) {
  /* TODO */
}
PRE_SYSCALL(getpid)(void) { /* Nothing to do */ }
POST_SYSCALL(getpid)(long long res) { /* Nothing to do */ }
PRE_SYSCALL(compat_40_mount)
(void *type_, void *path_, long long flags_, void *data_) {
  /* TODO */
}
POST_SYSCALL(compat_40_mount)
(long long res, void *type_, void *path_, long long flags_, void *data_) {
  /* TODO */
}
PRE_SYSCALL(unmount)(void *path_, long long flags_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(unmount)(long long res, void *path_, long long flags_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(setuid)(long long uid_) { /* Nothing to do */ }
POST_SYSCALL(setuid)(long long res, long long uid_) { /* Nothing to do */ }
PRE_SYSCALL(getuid)(void) { /* Nothing to do */ }
POST_SYSCALL(getuid)(long long res) { /* Nothing to do */ }
PRE_SYSCALL(geteuid)(void) { /* Nothing to do */ }
POST_SYSCALL(geteuid)(long long res) { /* Nothing to do */ }
PRE_SYSCALL(ptrace)
(long long req_, long long pid_, void *addr_, long long data_) {
  if (req_ == ptrace_pt_io) {
    struct __sanitizer_ptrace_io_desc *addr =
        (struct __sanitizer_ptrace_io_desc *)addr_;
    PRE_READ(addr, struct_ptrace_ptrace_io_desc_struct_sz);
    if (addr->piod_op == ptrace_piod_write_d ||
        addr->piod_op == ptrace_piod_write_i) {
      PRE_READ(addr->piod_addr, addr->piod_len);
    }
    if (addr->piod_op == ptrace_piod_read_d ||
        addr->piod_op == ptrace_piod_read_i ||
        addr->piod_op == ptrace_piod_read_auxv) {
      PRE_WRITE(addr->piod_addr, addr->piod_len);
    }
  } else if (req_ == ptrace_pt_lwpinfo) {
    struct __sanitizer_ptrace_lwpinfo *addr =
        (struct __sanitizer_ptrace_lwpinfo *)addr_;
    PRE_READ(&addr->pl_lwpid, sizeof(__sanitizer_lwpid_t));
    PRE_WRITE(addr, struct_ptrace_ptrace_lwpinfo_struct_sz);
  } else if (req_ == ptrace_pt_set_event_mask) {
    PRE_READ(addr_, struct_ptrace_ptrace_event_struct_sz);
  } else if (req_ == ptrace_pt_get_event_mask) {
    PRE_WRITE(addr_, struct_ptrace_ptrace_event_struct_sz);
  } else if (req_ == ptrace_pt_set_siginfo) {
    PRE_READ(addr_, struct_ptrace_ptrace_siginfo_struct_sz);
  } else if (req_ == ptrace_pt_get_siginfo) {
    PRE_WRITE(addr_, struct_ptrace_ptrace_siginfo_struct_sz);
  } else if (req_ == ptrace_pt_lwpstatus) {
    struct __sanitizer_ptrace_lwpstatus *addr =
        (struct __sanitizer_ptrace_lwpstatus *)addr_;
    PRE_READ(&addr->pl_lwpid, sizeof(__sanitizer_lwpid_t));
    PRE_WRITE(addr, struct_ptrace_ptrace_lwpstatus_struct_sz);
  } else if (req_ == ptrace_pt_lwpnext) {
    struct __sanitizer_ptrace_lwpstatus *addr =
        (struct __sanitizer_ptrace_lwpstatus *)addr_;
    PRE_READ(&addr->pl_lwpid, sizeof(__sanitizer_lwpid_t));
    PRE_WRITE(addr, struct_ptrace_ptrace_lwpstatus_struct_sz);
  } else if (req_ == ptrace_pt_setregs) {
    PRE_READ(addr_, struct_ptrace_reg_struct_sz);
  } else if (req_ == ptrace_pt_getregs) {
    PRE_WRITE(addr_, struct_ptrace_reg_struct_sz);
  } else if (req_ == ptrace_pt_setfpregs) {
    PRE_READ(addr_, struct_ptrace_fpreg_struct_sz);
  } else if (req_ == ptrace_pt_getfpregs) {
    PRE_WRITE(addr_, struct_ptrace_fpreg_struct_sz);
  } else if (req_ == ptrace_pt_setdbregs) {
    PRE_READ(addr_, struct_ptrace_dbreg_struct_sz);
  } else if (req_ == ptrace_pt_getdbregs) {
    PRE_WRITE(addr_, struct_ptrace_dbreg_struct_sz);
  }
}
POST_SYSCALL(ptrace)
(long long res, long long req_, long long pid_, void *addr_, long long data_) {
  if (res == 0) {
    if (req_ == ptrace_pt_io) {
      struct __sanitizer_ptrace_io_desc *addr =
          (struct __sanitizer_ptrace_io_desc *)addr_;
      POST_READ(addr, struct_ptrace_ptrace_io_desc_struct_sz);
      if (addr->piod_op == ptrace_piod_write_d ||
          addr->piod_op == ptrace_piod_write_i) {
        POST_READ(addr->piod_addr, addr->piod_len);
      }
      if (addr->piod_op == ptrace_piod_read_d ||
          addr->piod_op == ptrace_piod_read_i ||
          addr->piod_op == ptrace_piod_read_auxv) {
        POST_WRITE(addr->piod_addr, addr->piod_len);
      }
    } else if (req_ == ptrace_pt_lwpinfo) {
      struct __sanitizer_ptrace_lwpinfo *addr =
          (struct __sanitizer_ptrace_lwpinfo *)addr_;
      POST_READ(&addr->pl_lwpid, sizeof(__sanitizer_lwpid_t));
      POST_WRITE(addr, struct_ptrace_ptrace_lwpinfo_struct_sz);
    } else if (req_ == ptrace_pt_set_event_mask) {
      POST_READ(addr_, struct_ptrace_ptrace_event_struct_sz);
    } else if (req_ == ptrace_pt_get_event_mask) {
      POST_WRITE(addr_, struct_ptrace_ptrace_event_struct_sz);
    } else if (req_ == ptrace_pt_set_siginfo) {
      POST_READ(addr_, struct_ptrace_ptrace_siginfo_struct_sz);
    } else if (req_ == ptrace_pt_get_siginfo) {
      POST_WRITE(addr_, struct_ptrace_ptrace_siginfo_struct_sz);
    } else if (req_ == ptrace_pt_lwpstatus) {
      struct __sanitizer_ptrace_lwpstatus *addr =
          (struct __sanitizer_ptrace_lwpstatus *)addr_;
      POST_READ(&addr->pl_lwpid, sizeof(__sanitizer_lwpid_t));
      POST_WRITE(addr, struct_ptrace_ptrace_lwpstatus_struct_sz);
    } else if (req_ == ptrace_pt_lwpnext) {
      struct __sanitizer_ptrace_lwpstatus *addr =
          (struct __sanitizer_ptrace_lwpstatus *)addr_;
      POST_READ(&addr->pl_lwpid, sizeof(__sanitizer_lwpid_t));
      POST_WRITE(addr, struct_ptrace_ptrace_lwpstatus_struct_sz);
    } else if (req_ == ptrace_pt_setregs) {
      POST_READ(addr_, struct_ptrace_reg_struct_sz);
    } else if (req_ == ptrace_pt_getregs) {
      POST_WRITE(addr_, struct_ptrace_reg_struct_sz);
    } else if (req_ == ptrace_pt_setfpregs) {
      POST_READ(addr_, struct_ptrace_fpreg_struct_sz);
    } else if (req_ == ptrace_pt_getfpregs) {
      POST_WRITE(addr_, struct_ptrace_fpreg_struct_sz);
    } else if (req_ == ptrace_pt_setdbregs) {
      POST_READ(addr_, struct_ptrace_dbreg_struct_sz);
    } else if (req_ == ptrace_pt_getdbregs) {
      POST_WRITE(addr_, struct_ptrace_dbreg_struct_sz);
    }
  }
}
PRE_SYSCALL(recvmsg)(long long s_, void *msg_, long long flags_) {
  PRE_WRITE(msg_, sizeof(__sanitizer_msghdr));
}
POST_SYSCALL(recvmsg)
(long long res, long long s_, void *msg_, long long flags_) {
  if (res > 0) {
    POST_WRITE(msg_, sizeof(__sanitizer_msghdr));
  }
}
PRE_SYSCALL(sendmsg)(long long s_, void *msg_, long long flags_) {
  PRE_READ(msg_, sizeof(__sanitizer_msghdr));
}
POST_SYSCALL(sendmsg)
(long long res, long long s_, void *msg_, long long flags_) {
  if (res > 0) {
    POST_READ(msg_, sizeof(__sanitizer_msghdr));
  }
}
PRE_SYSCALL(recvfrom)
(long long s_, void *buf_, long long len_, long long flags_, void *from_,
  void *fromlenaddr_) {
  PRE_WRITE(buf_, len_);
  PRE_WRITE(from_, struct_sockaddr_sz);
  PRE_WRITE(fromlenaddr_, sizeof(__sanitizer_socklen_t));
}
POST_SYSCALL(recvfrom)
(long long res, long long s_, void *buf_, long long len_, long long flags_,
  void *from_, void *fromlenaddr_) {
  if (res >= 0) {
    POST_WRITE(buf_, res);
    POST_WRITE(from_, struct_sockaddr_sz);
    POST_WRITE(fromlenaddr_, sizeof(__sanitizer_socklen_t));
  }
}
PRE_SYSCALL(accept)(long long s_, void *name_, void *anamelen_) {
  PRE_WRITE(name_, struct_sockaddr_sz);
  PRE_WRITE(anamelen_, sizeof(__sanitizer_socklen_t));
}
POST_SYSCALL(accept)
(long long res, long long s_, void *name_, void *anamelen_) {
  if (res == 0) {
    POST_WRITE(name_, struct_sockaddr_sz);
    POST_WRITE(anamelen_, sizeof(__sanitizer_socklen_t));
  }
}
PRE_SYSCALL(getpeername)(long long fdes_, void *asa_, void *alen_) {
  PRE_WRITE(asa_, struct_sockaddr_sz);
  PRE_WRITE(alen_, sizeof(__sanitizer_socklen_t));
}
POST_SYSCALL(getpeername)
(long long res, long long fdes_, void *asa_, void *alen_) {
  if (res == 0) {
    POST_WRITE(asa_, struct_sockaddr_sz);
    POST_WRITE(alen_, sizeof(__sanitizer_socklen_t));
  }
}
PRE_SYSCALL(getsockname)(long long fdes_, void *asa_, void *alen_) {
  PRE_WRITE(asa_, struct_sockaddr_sz);
  PRE_WRITE(alen_, sizeof(__sanitizer_socklen_t));
}
POST_SYSCALL(getsockname)
(long long res, long long fdes_, void *asa_, void *alen_) {
  if (res == 0) {
    POST_WRITE(asa_, struct_sockaddr_sz);
    POST_WRITE(alen_, sizeof(__sanitizer_socklen_t));
  }
}
PRE_SYSCALL(access)(void *path_, long long flags_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(access)(long long res, void *path_, long long flags_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(chflags)(void *path_, long long flags_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(chflags)(long long res, void *path_, long long flags_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(fchflags)(long long fd_, long long flags_) { /* Nothing to do */ }
POST_SYSCALL(fchflags)(long long res, long long fd_, long long flags_) {
  /* Nothing to do */
}
PRE_SYSCALL(sync)(void) { /* Nothing to do */ }
POST_SYSCALL(sync)(long long res) { /* Nothing to do */ }
PRE_SYSCALL(kill)(long long pid_, long long signum_) { /* Nothing to do */ }
POST_SYSCALL(kill)(long long res, long long pid_, long long signum_) {
  /* Nothing to do */
}
PRE_SYSCALL(compat_43_stat43)(void *path_, void *ub_) { /* TODO */ }
POST_SYSCALL(compat_43_stat43)(long long res, void *path_, void *ub_) {
  /* TODO */
}
PRE_SYSCALL(getppid)(void) { /* Nothing to do */ }
POST_SYSCALL(getppid)(long long res) { /* Nothing to do */ }
PRE_SYSCALL(compat_43_lstat43)(void *path_, void *ub_) { /* TODO */ }
POST_SYSCALL(compat_43_lstat43)(long long res, void *path_, void *ub_) {
  /* TODO */
}
PRE_SYSCALL(dup)(long long fd_) { /* Nothing to do */ }
POST_SYSCALL(dup)(long long res, long long fd_) { /* Nothing to do */ }
PRE_SYSCALL(pipe)(void) {
  /* pipe returns two descriptors through two returned values */
}
POST_SYSCALL(pipe)(long long res) {
  /* pipe returns two descriptors through two returned values */
}
PRE_SYSCALL(getegid)(void) { /* Nothing to do */ }
POST_SYSCALL(getegid)(long long res) { /* Nothing to do */ }
PRE_SYSCALL(profil)
(void *samples_, long long size_, long long offset_, long long scale_) {
  if (samples_) {
    PRE_WRITE(samples_, size_);
  }
}
POST_SYSCALL(profil)
(long long res, void *samples_, long long size_, long long offset_,
  long long scale_) {
  if (res == 0) {
    if (samples_) {
      POST_WRITE(samples_, size_);
    }
  }
}
PRE_SYSCALL(ktrace)
(void *fname_, long long ops_, long long facs_, long long pid_) {
  const char *fname = (const char *)fname_;
  if (fname) {
    PRE_READ(fname, __sanitizer::internal_strlen(fname) + 1);
  }
}
POST_SYSCALL(ktrace)
(long long res, void *fname_, long long ops_, long long facs_, long long pid_) {
  const char *fname = (const char *)fname_;
  if (res == 0) {
    if (fname) {
      POST_READ(fname, __sanitizer::internal_strlen(fname) + 1);
    }
  }
}
PRE_SYSCALL(compat_13_sigaction13)(long long signum_, void *nsa_, void *osa_) {
  /* TODO */
}
POST_SYSCALL(compat_13_sigaction13)
(long long res, long long signum_, void *nsa_, void *osa_) {
  /* TODO */
}
PRE_SYSCALL(getgid)(void) { /* Nothing to do */ }
POST_SYSCALL(getgid)(long long res) { /* Nothing to do */ }
PRE_SYSCALL(compat_13_sigprocmask13)(long long how_, long long mask_) {
  /* TODO */
}
POST_SYSCALL(compat_13_sigprocmask13)
(long long res, long long how_, long long mask_) {
  /* TODO */
}
PRE_SYSCALL(__getlogin)(void *namebuf_, long long namelen_) {
  if (namebuf_) {
    PRE_WRITE(namebuf_, namelen_);
  }
}
POST_SYSCALL(__getlogin)(long long res, void *namebuf_, long long namelen_) {
  if (res == 0) {
    if (namebuf_) {
      POST_WRITE(namebuf_, namelen_);
    }
  }
}
PRE_SYSCALL(__setlogin)(void *namebuf_) {
  const char *namebuf = (const char *)namebuf_;
  if (namebuf) {
    PRE_READ(namebuf, __sanitizer::internal_strlen(namebuf) + 1);
  }
}
POST_SYSCALL(__setlogin)(long long res, void *namebuf_) {
  if (res == 0) {
    const char *namebuf = (const char *)namebuf_;
    if (namebuf) {
      POST_READ(namebuf, __sanitizer::internal_strlen(namebuf) + 1);
    }
  }
}
PRE_SYSCALL(acct)(void *path_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(acct)(long long res, void *path_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(compat_13_sigpending13)(void) { /* TODO */ }
POST_SYSCALL(compat_13_sigpending13)(long long res) { /* TODO */ }
PRE_SYSCALL(compat_13_sigaltstack13)(void *nss_, void *oss_) { /* TODO */ }
POST_SYSCALL(compat_13_sigaltstack13)(long long res, void *nss_, void *oss_) {
  /* TODO */
}
PRE_SYSCALL(ioctl)(long long fd_, long long com_, void *data_) {
  /* Nothing to do */
}
POST_SYSCALL(ioctl)(long long res, long long fd_, long long com_, void *data_) {
  /* Nothing to do */
}
PRE_SYSCALL(compat_12_oreboot)(long long opt_) { /* TODO */ }
POST_SYSCALL(compat_12_oreboot)(long long res, long long opt_) { /* TODO */ }
PRE_SYSCALL(revoke)(void *path_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(revoke)(long long res, void *path_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(symlink)(void *path_, void *link_) {
  const char *path = (const char *)path_;
  const char *link = (const char *)link_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
  if (link) {
    PRE_READ(link, __sanitizer::internal_strlen(link) + 1);
  }
}
POST_SYSCALL(symlink)(long long res, void *path_, void *link_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    const char *link = (const char *)link_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
    if (link) {
      POST_READ(link, __sanitizer::internal_strlen(link) + 1);
    }
  }
}
PRE_SYSCALL(readlink)(void *path_, void *buf_, long long count_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
  if (buf_) {
    PRE_WRITE(buf_, count_);
  }
}
POST_SYSCALL(readlink)
(long long res, void *path_, void *buf_, long long count_) {
  if (res > 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
    if (buf_) {
      PRE_WRITE(buf_, res);
    }
  }
}
PRE_SYSCALL(execve)(void *path_, void *argp_, void *envp_) {
  const char *path = (const char *)path_;
  char **argp = (char **)argp_;
  char **envp = (char **)envp_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
  if (argp && argp[0]) {
    char *a = argp[0];
    while (a++) {
      PRE_READ(a, __sanitizer::internal_strlen(a) + 1);
    }
  }
  if (envp && envp[0]) {
    char *e = envp[0];
    while (e++) {
      PRE_READ(e, __sanitizer::internal_strlen(e) + 1);
    }
  }
}
POST_SYSCALL(execve)(long long res, void *path_, void *argp_, void *envp_) {
  /* If we are here, something went wrong */
  const char *path = (const char *)path_;
  char **argp = (char **)argp_;
  char **envp = (char **)envp_;
  if (path) {
    POST_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
  if (argp && argp[0]) {
    char *a = argp[0];
    while (a++) {
      POST_READ(a, __sanitizer::internal_strlen(a) + 1);
    }
  }
  if (envp && envp[0]) {
    char *e = envp[0];
    while (e++) {
      POST_READ(e, __sanitizer::internal_strlen(e) + 1);
    }
  }
}
PRE_SYSCALL(umask)(long long newmask_) { /* Nothing to do */ }
POST_SYSCALL(umask)(long long res, long long newmask_) { /* Nothing to do */ }
PRE_SYSCALL(chroot)(void *path_) {
  const char *path = (const char *)path_;
  if (path) {
    PRE_READ(path, __sanitizer::internal_strlen(path) + 1);
  }
}
POST_SYSCALL(chroot)(long long res, void *path_) {
  if (res == 0) {
    const char *path = (const char *)path_;
    if (path) {
      POST_READ(path, __sanitizer::internal_strlen(path) + 1);
    }
  }
}
PRE_SYSCALL(compat_43_fstat43)(long long fd_, void *sb_) { /* TODO */ }
POST_SYSCALL(compat_43_fstat43)(long long res, long long fd_, void *sb_) {
  /* TODO */
}
PRE_SYSCALL(compat_43_ogetkerninfo)
(long long op_, void *where_, void *size_, long long arg_) {
  /* TODO */
}
POST_SYSCALL(compat_43_ogetkerninfo)
(long long res, long long op_, void *where_, void *size_, long long arg_) {
  /* TODO */
}
PRE_SYSCALL(compat_43_ogetpagesize)(void) { /* TODO */ }
POST_SYSCALL(compat_43_ogetpagesize)(long long res) { /* TODO */ }
PRE_SYSCALL(compat_12_msync)(void *addr_, long long len_) { /* TODO */ }
POST_SYSCALL(compat_12_msync)(long long res, void *addr_, long long len_) {
  /* TODO */
}
PRE_SYSCALL(vfork)(void) { /* Nothing to do */ }
POST_SYSCALL(vfork)(long long res) { /* Nothing to do */ }
/* syscall 67 has been skipped */
/* syscall 68 has been skipped */
/* syscall 69 has been skipped */
/* syscall 70 has been skipped */
PRE_SYSCALL(compat_43_ommap)
(void *addr_, long long len_, long long prot_, long long flags_, long long fd_,
  long long pos_) {
  /* TODO */
}
POST_SYSCALL(compat_43_ommap)
(long long res, void *addr_, long long len_, long long prot_, long long flags_,
  long long fd_, long long pos_) {
  /* TODO */
}
PRE_SYSCALL(vadvise)(long long anom_) { /* Nothing to do */ }
POST_SYSCALL(vadvise)(long long res, long long anom_) { /* Nothing to do */ }
PRE_SYSCALL(munmap)(void *addr_, long long len_) { /* Nothing to do */ }
POST_SYSCALL(munmap)(long long res, void *addr_, long long len_) {
  /* Nothing to do */
}
PRE_SYSCALL(mprotect)(void *addr_, long long len_, long long prot_) {
  /* Nothing to do */
}
POST_SYSCALL(mprotect)
(long long res, void *addr_, long long len_, long long prot_) {
  /* Nothing to do */
}
PRE_SYSCALL(madvise)(void *addr_, long long len_, long long behav_) {
  /* Nothing to do */
}
POST_SYSCALL(madvise)
(long long res, void *addr_, long long len_, long long behav_) {
  /* Nothing to do */
}
/* syscall 76 has been skipped */
/* syscall 77 has been skipped */
PRE_SYSCALL(mincore)(void *addr_, long long len_, void *vec_) {
  /* Nothing to do */
}
POST_SYSCALL(mincore)(long long res, void *addr_, long long len_, void *vec_) {
  /* Nothing to do */
}
PRE_SYSCALL(getgroups)(long long gidsetsize_, void *gidset_) {
  unsigned int *gidset = (unsigned int *)gidset_;
  if (gidset) {
    PRE_WRITE(gidset, sizeof(*gidset) * gidsetsize_);
  }
}
POST_SYSCALL(getgroups)(long long res, long long gidsetsize_, void *gidset_) {
  if (res == 0) {
    unsigned int *gidset = (unsigned int *)gidset_;
    if (gidset) {
      POST_WRITE(gidset, sizeof(*gidset) * gidsetsize_);
    }
  }
}
PRE_SYSCALL(setgroups)(long long gidsetsize_, void *gidset_) {
  unsigned int *gidset = (unsigned int *)gidset_;
  if (gidset) {
    PRE_READ(gidset, sizeof(*gidset) * gidsetsize_);
  }
}
POST_SYSCALL(setgroups)(long long res, long long gidsetsize_, void *gidset_) {
  if (res == 0) {
    unsigned int *gidset = (unsigned int *)gidset_;
    if (gidset) {
      POST_READ(gidset, sizeof(*gidset) * gidsetsize_);
    }
  }
}
PRE_SYSCALL(getpgrp)(void) { /* Nothing to do */ }
POST_SYSCALL(getpgrp)(long long res) { /* Nothing to do */ }
PRE_SYSCALL(setpgid)(long long pid_, long long pgid_) { /* Nothing to do */ }
POST_SYSCALL(setpgid)(long long res, long long pid_, long long pgid_) {
  /* Nothing to do */
}
PRE_SYSCALL(compat_50_setitimer)(long long which_, void *itv_, void *oitv_) {
  /* TODO */
}
POST_SYSCALL(compat_50_setitimer)
(long long res, long long which_, void *itv_, void *oitv_) {
  /* TODO */
}
PRE_SYSCALL(compat_43_owait)(void) { /* TODO */ }
POST_SYSCALL(compat_43_owait)(long long res) { /* TODO */ }
PRE_SYSCALL(compat_12_oswapon)(void *name_) { /* TODO */ }
POST_SYSCALL(compat_12_oswapon)(long long res, void *name_) { /* TODO */ }
PRE_SYSCALL(compat_50_getitimer)(long long which_, void *itv_) { /* TODO */ }
POST_SYSCALL(compat_50_getitimer)(long long res, long long which_, void *itv_) {
  /* TODO */
}
PRE_SYSCALL(compat_43_ogethostname)(void *hostname_, long long len_) {
  /* TODO */
}
POST_SYSCALL(compat_43_ogethostname)
(long long res, void *hostname_, long long len_) {
  /* TODO */
}
PRE_SYSCALL(compat_43_osethostname)(void *hostname_, long long len_) {
  /* TODO */
}
POST_SYSCALL(compat_43_osethostname)
(long long res, void *hostname_, long long len_) {
  /* TODO */
}
PRE_SYSCALL(compat_43_ogetdtablesize)(void) { /* TODO */ }
POST_SYSCALL(compat_43_ogetdtablesize)(long long res) { /* TODO */ }
PRE_SYSCALL(dup2)(long long from_, long long to_) { /* Nothing to do */ }
POST_SYSCALL(dup2)(long long res, long long from_, long long to_) {
  /* Nothing to do */
}
PRE_SYSCALL(getrandom)(void *buf_, long long buflen_, long long flags_) {
  /* TODO */
}
POST_SYSCALL(getrandom)
(long long res, void *buf_, long long buflen_, long long flags_) {
  /* TODO */
}
PRE_SYSCALL(fcntl)(long long fd_, long long cmd_, void *arg_) {
  /* Nothing to do */
}
POST_SYSCALL(fcntl)(long long res, long long fd_, long long cmd_, void *arg_) {
  /* Nothing to do */
