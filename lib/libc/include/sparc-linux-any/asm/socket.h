
/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _ASM_SOCKET_H
#define _ASM_SOCKET_H

#include <linux/posix_types.h>
#include <asm/sockios.h>

/* For setsockopt(2) */
#define SOL_SOCKET	0xffff

#define SO_DEBUG	0x0001
#define SO_PASSCRED	0x0002
#define SO_REUSEADDR	0x0004
#define SO_KEEPALIVE	0x0008
#define SO_DONTROUTE	0x0010
#define SO_BROADCAST	0x0020
#define SO_PEERCRED	0x0040
#define SO_LINGER	0x0080
#define SO_OOBINLINE	0x0100
#define SO_REUSEPORT	0x0200
#define SO_BSDCOMPAT    0x0400
#define SO_RCVLOWAT     0x0800
#define SO_SNDLOWAT     0x1000
#define SO_RCVTIMEO_OLD     0x2000
#define SO_SNDTIMEO_OLD     0x4000
#define SO_ACCEPTCONN	0x8000

#define SO_SNDBUF	0x1001
#define SO_RCVBUF	0x1002