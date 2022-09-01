/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 *  include/linux/userfaultfd.h
 *
 *  Copyright (C) 2007  Davide Libenzi <davidel@xmailserver.org>
 *  Copyright (C) 2015  Red Hat, Inc.
 *
 */

#ifndef _LINUX_USERFAULTFD_H
#define _LINUX_USERFAULTFD_H

#include <linux/types.h>

/*
 * If the UFFDIO_API is upgraded someday, the UFFDIO_UNREGISTER and
 * UFFDIO_WAKE ioctls should be defined as _IOW and not as _IOR.  In
 * userfaultfd.h we assumed the kernel was reading (instead _IOC_READ
 * means the userland is reading).
 */
#define UFFD_API ((__u64)0xAA)
#define UFFD_API_REGISTER_MODES (UFFDIO_REGISTER_MODE_MISSING |	\
				 UFFDIO_REGISTER_MODE_WP |	\
				 UFFDIO_REGISTER_MODE_MINOR)
#define UFFD_API_FEATURES (UFFD_FEATURE_PAGEFAULT_FLAG_WP |	\
			   UFFD_FEATURE_EVENT_FORK |		\
			   UFFD_FEATURE_EVENT_REMAP |		\
			   UFFD_FEATURE_EVENT_REMOVE |		\
			   UFFD_FEATURE_EVENT_UNMAP |		\
			   UFFD_FEATURE_MISSING_HUGETLBFS |	\
			   UFFD_FEATURE_MISSING_SHMEM |		\
			   UFFD_FEATURE_SIGBUS |		\
			   UFFD_FEATURE_THREAD_ID |		\
			   UFFD_FEATURE_MINOR_HUGETLBFS |	\
			   UFFD_FEATURE_MINOR_SHMEM)
#define UFFD_API_IOCTLS				\
	((__u64)1 << _UFFDIO_REGISTER |		\
	 (__u64)1 << _UFFDIO_UNREGISTER |	\
	 (__u64)1 << _UFFDIO_API)
#define UFFD_API_RANGE_IOCTLS			\
	((__u64)1 << _UFFDIO_WAKE |		\
	 (__u64)1 << _UFFDIO_COPY |		\
	 (__u64)1 << _UFFDIO_ZEROPAGE |		\
	 (__u64)1 << _UFFDIO_WRITEPROTECT |	\
	 (__u64)1 << _UFFDIO_CONTINUE)
#define UFFD_API_RANGE_IOCTLS_BASIC		\
	((__u64)1 << _UFFDIO_WAKE |		\
	 (__u64)1 << _UFFDIO_COPY |		\
	 (__u64)1 << _UFFDIO_CONTINUE)

/*
 * Valid ioctl command number range with this API is from 0x00 to
 * 0x3F.  UFFDIO_API is the fixed number, everything else can be
 * changed by implementing a different UFFD_API. If sticking to the
 * same UFFD_API more ioctl can be added and userland will be aware of
 * which ioctl the running kernel implements through the ioctl command
 * bitmask written by the UFFDIO_API.
 */
#define _UFFDIO_REGISTER		(0x00)
#define _UFFDIO_UNREGISTER		(0x01)
#define _UFFDIO_WAKE			(0x02)
#define _UFFDIO_COPY			(0x03)
#define _UFFDIO_ZEROPAGE		(0x04)
#define _UFFDIO_WRITEPROTECT		(0x06)
#define _UFFDIO_CONTINUE		(0x07)
#define _UFFDIO_API			(0x3F)

/* userfaultfd ioctl ids */
#define UFFDIO 0xAA
#define UFFDIO_API		_IOWR(UFFDIO, _UFFDIO_API,	\
				      struct uffdio_api)
#define UFFDIO_REGISTER		_IOWR(UFFDIO, _UFFDIO_REGISTER, \
				      struct uffdio_register)
#define UFFDIO_UNREGISTER	_IOR(UFFDIO, _UFFDIO_UNREGISTER,	\
				     struct uffdio_range)
#define UFFDIO_WAKE		_IOR(UFFDIO, _UFFDIO_WAKE,	\
				     struct uffdio_range)
#define UFFDIO_COPY		_IOWR(UFFDIO, _UFFDIO_COPY,	\
				      struct uffdio_copy)
#define UFFDIO_ZEROPAGE		_IOWR(UFFDIO, _UFFDIO_ZEROPAGE,	\
				      struct uffdio_zeropage)
#define UFFDIO_WRITEPROTECT	_IO