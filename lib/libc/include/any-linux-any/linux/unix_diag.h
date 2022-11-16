/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef __UNIX_DIAG_H__
#define __UNIX_DIAG_H__

#include <linux/types.h>

struct unix_diag_req {
	__u8	sdiag_family;
	__u8	sdiag_protocol;
	__u16	pad;
	__u32	udiag_states;
	__u32	udiag_ino;
	__u32	udiag_show;
	__u32	udiag_cookie[2];
};

#define UDIAG_SHOW_NAME		0x000000