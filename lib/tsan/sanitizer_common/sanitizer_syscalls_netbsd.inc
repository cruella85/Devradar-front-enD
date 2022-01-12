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
        POST_READ(addr->piod_addr, a