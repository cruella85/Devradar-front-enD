/*
 * Header for the Direct Rendering Manager
 *
 * Author: Rickard E. (Rik) Faith <faith@valinux.com>
 *
 * Acknowledgments:
 * Dec 1999, Richard Henderson <rth@twiddle.net>, move to generic cmpxchg.
 */

/*
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _DRM_H_
#define _DRM_H_

#if   defined(__linux__)

#include <linux/types.h>
#include <asm/ioctl.h>
typedef unsigned int drm_handle_t;

#else /* One of the BSDs */

#include <stdint.h>
#include <sys/ioccom.h>
#include <sys/types.h>
typedef int8_t   __s8;
typedef uint8_t  __u8;
typedef int16_t  __s16;
typedef uint16_t __u16;
typedef int32_t  __s32;
typedef uint32_t __u32;
typedef int64_t  __s64;
typedef uint64_t __u64;
typedef size_t   __kernel_size_t;
typedef unsigned long drm_handle_t;

#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define DRM_NAME	"drm"	  /**< Name in kernel, /dev, and /proc */
#define DRM_MIN_ORDER	5	  /**< At least 2^5 bytes = 32 bytes */
#define DRM_MAX_ORDER	22	  /**< Up to 2^22 bytes = 4MB */
#define DRM_RAM_PERCENT 10	  /**< How much system ram can we lock? */

#define _DRM_LOCK_HELD	0x80000000U /**< Hardware lock is held */
#define _DRM_LOCK_CONT	0x40000000U /**< Hardware lock is contended */
#define _DRM_LOCK_IS_HELD(lock)	   ((lock) & _DRM_LOCK_HELD)
#define _DRM_LOCK_IS_CONT(lock)	   ((lock) & _DRM_LOCK_CONT)
#define _DRM_LOCKING_CONTEXT(lock) ((lock) & ~(_DRM_LOCK_HELD|_DRM_LOCK_CONT))

typedef unsigned int drm_context_t;
typedef unsigned int drm_drawable_t;
typedef unsigned int drm_magic_t;

/*
 * Cliprect.
 *
 * \warning: If you change this structure, make sure you change
 * XF86DRIClipRectRec in the server as well
 *
 * \note KW: Actually it's illegal to change either for
 * backwards-compatibility reasons.
 */
struct drm_clip_rect {
	unsigned short x1;
	unsigned short y1;
	unsigned short x2;
	unsigned short y2;
};

/*
 * Drawable information.
 */
struct drm_drawable_info {
	unsigned int num_rects;
	struct drm_clip_rect *rects;
};

/*
 * Texture region,
 */
struct drm_tex_region {
	unsigned char next;
	unsigned char prev;
	unsigned char in_use;
	unsigned char padding;
	unsigned int age;
};

/*
 * Hardware lock.
 *
 * The lock structure is a simple cache-line aligned integer.  To avoid
 * processor bus contention on a multiprocessor system, there should not be any
 * other data stored in the same cache line.
 */
struct drm_hw_lock {
	__volatile__ unsigned int lock;		/**< lock variable */
	char padding[60];			/**< Pad to cache line */
};

/*
 * DRM_IOCTL_VERSION ioctl argument type.
 *
 * \sa drmGetVersion().
 */
struct drm_version {
	int version_major;	  /**< Major version */
	int version_minor;	  /**< Minor version */
	int version_patchlevel;	  /**< Patch level */
	__kernel_size_t name_len;	  /**< Length of name buffer */
	char *name;	  /**< Name of driver */
	__kernel_size_t date_len;	  /**< Length of date buffer */
	char *date;	  /**< User-space buffer to hold date */
	__kernel_size_t desc_len;	  /**< Length of desc buffer */
	char *desc;	  /**< User-space buffer to hold desc */
};

/*
 * DRM_IOCTL_GET_UNIQUE ioctl argument type.
 *
 * \sa drmGetBusid() and drmSetBusId().
 */
struct drm_unique {
	__kernel_size_t unique_len;	  /**< Length of unique */
	char *unique;	  /**< Unique name for driver instantiation */
};

struct drm_list {
	int count;		  /**< Length of user-space structures */
	struct drm_version *version;
};

struct drm_block {
	int unused;
};

/*
 * DRM_IOCTL_CONTROL ioctl argument type.
 *
 * \sa drmCtlInstHandler() and drmCtlUninstHandler().
 */
struct drm_control {
	enum {
		DRM_ADD_COMMAND,
		DRM_RM_COMMAND,
		DRM_INST_HANDLER,
		DRM_UNINST_HANDLER
	} func;
	int irq;
};

/*
 * Type of memory to map.
 */
enum drm_map_type {
	_DRM_FRAME_BUFFER = 0,	  /**< WC (no caching), no core dump */
	_DRM_REGISTERS = 1,	  /**< no caching, no core dump */
	_DRM_SHM = 2,		  /**< shared, cached */
	_DRM_AGP = 3,		  /**< AGP/GART */
	_DRM_SCATTER_GATHER = 4,  /**< Scatter/gather memory for PCI DMA */
	_DRM_CONSISTENT = 5	  /**< Consistent memory for PCI DMA */
};

/*
 * Memory mapping flags.
 */
enum drm_map_flags {
	_DRM_RESTRICTED = 0x01,	     /**< Cannot be mapped to user-virtual */
	_DRM_READ_ONLY = 0x02,
	_DRM_LOCKED = 0x04,	     /**< shared, cached, locked */
	_DRM_KERNEL = 0x08,	     /**< kernel requires access */
	_DRM_WRITE_COMBINING = 0x10, /**< use write-combining if available */
	_DRM_CONTAINS_LOCK = 0x20,   /**< SHM page that contains lock */
	_DRM_REMOVABLE = 0x40,	     /**< Removable mapping */
	_DRM_DRIVER = 0x80	     /**< Managed by driver */
};

struct drm_ctx_priv_map {
	unsigned int ctx_id;	 /**< Context requesting private mapping */
	void *handle;		 /**< Handle of map */
};

/*
 * DRM_IOCTL_GET_MAP, DRM_IOCTL_ADD_MAP and DRM_IOCTL_RM_MAP ioctls
 * argument type.
 *
 * \sa drmAddMap().
 */
struct drm_map {
	unsigned long offset;	 /**< Requested physical address (0 for SAREA)*/
	unsigned long size;	 /**< Requested physical size (bytes) */
	enum drm_map_type type;	 /**< Type of memory to map */
	enum drm_map_flags flags;	 /**< Flags */
	void *handle;		 /**< User-space: "Handle" to pass to mmap() */
				 /**< Kernel-space: kernel-virtual address */
	int mtrr;		 /**< MTRR slot used */
	/*   Private data */
};

/*
 * DRM_IOCTL_GET_CLIENT ioctl argument type.
 */
struct drm_client {
	int idx;		/**< Which client desired? */
	int auth;		/**< Is client authenticated? */
	unsigned long pid;	/**< Process ID */
	unsigned long uid;	/**< User ID */
	unsigned long magic;	/**< Magic */
	unsigned long iocs;	/**< Ioctl count */
};

enum drm_stat_type {
	_DRM_STAT_LOCK,
	_DRM_STAT_OPENS,
	_DRM_STAT_CLOSES,
	_DRM_STAT_IOCTLS,
	_DRM_STAT_LOCKS,
	_DRM_STAT_UNLOCKS,
	_DRM_STAT_VALUE,	/**< Generic value */
	_DRM_STAT_BYTE,		/**< Generic byte counter (1024bytes/K) */
	_DRM_STAT_COUNT,	/**< Generic non-byte counter (1000/k) */

	_DRM_STAT_IRQ,		/**< IRQ */
	_DRM_STAT_PRIMARY,	/**< Primary DMA bytes */
	_DRM_STAT_SECONDARY,	/**< Secondary DMA bytes */
	_DRM_STAT_DMA,		/**< DMA */
	_DRM_STAT_SPECIAL,	/**< Special DMA (e.g., priority or polled) */
	_DRM_STAT_MISSED	/**< Missed DMA opportunity */
	    /* Add to the *END* of the list */
};

/*
 * DRM_IOCTL_GET_STATS ioctl argument type.
 */
struct drm_stats {
	unsigned long count;
	struct {
		unsigned long value;
		enum drm_stat_type type;
	} data[15];
};

/*
 * Hardware locking flags.
 */
enum drm_lock_flags {
	_DRM_LOCK_READY = 0x01,	     /**< Wait until hardware is ready for DMA */
	_DRM_LOCK_QUIESCENT = 0x02,  /**< Wait until hardware quiescent */
	_DRM_LOCK_FLUSH = 0x04,	     /**< Flush this context's DMA queue first */
	_DRM_LOCK_FLUSH_ALL = 0x08,  /**< Flush all DMA queues first */
	/* These *HALT* flags aren't supported yet
	   -- they will be used to support the
	   full-screen DGA-like mode. */
	_DRM_HALT_ALL_QUEUES = 0x10, /**< Halt all current and future queues */
	_DRM_HALT_CUR_QUEUES = 0x20  /**< Halt all current queues */
};

/*
 * DRM_IOCTL_LOCK, DRM_IOCTL_UNLOCK and DRM_IOCTL_FINISH ioctl argument type.
 *
 * \sa drmGetLock() and drmUnlock().
 */
struct drm_lock {
	int context;
	enum drm_lock_flags flags;
};

/*
 * DMA flags
 *
 * \warning
 * These values \e must match xf86drm.h.
 *
 * \sa drm_dma.
 */
enum drm_dma_flags {
	/* Flags for DMA buffer dispatch */
	_DRM_DMA_BLOCK = 0x01,	      /**<
				       * Block until buffer dispatched.
				       *
				       * \note The buffer may not yet have
				       * been processed by the hardware --
				       * getting a hardware lock with the
				       * hardware quiescent will ensure
				       * that the buffer has been
				       * processed.
				       */
	_DRM_DMA_WHILE_LOCKED = 0x02, /**< Dispatch while lock held */
	_DRM_DMA_PRIORITY = 0x04,     /**< High priority dispatch */

	/* Flags for DMA buffer request */
	_DRM_DMA_WAIT = 0x10,	      /**< Wait for free buffers */
	_DRM_DMA_SMALLER_OK = 0x20,   /**< Smaller-than-requested buffers OK */
	_DRM_DMA_LARGER_OK = 0x40     /**< Larger-than-requested buffers OK */
};

/*
 * DRM_IOCTL_ADD_BUFS and DRM_IOCTL_MARK_BUFS ioctl argument type.
 *
 * \sa drmAddBufs().
 */
struct drm_buf_desc {
	int count;		 /**< Number of buffers of this size */
	int size;		 /**< Size in bytes */
	int low_mark;		 /**< Low water mark */
	int high_mark;		 /**< High water mark */
	enum {
		_DRM_PAGE_ALIGN = 0x01,	/**< Align on page boundaries for DMA */
		_DRM_AGP_BUFFER = 0x02,	/**< Buffer is in AGP space */
		_DRM_SG_BUFFER = 0x04,	/**< Scatter/gather memory buffer */
		_DRM_FB_BUFFER = 0x08,	/**< Buffer is in frame buffer */
		_DRM_PCI_BUFFER_RO = 0x10 /**< Map PCI DMA buffer read-only */
	} flags;
	unsigned long agp_start; /**<
				  * Start address of where the AGP buffers are
				  * in the AGP aperture
				  */
};

/*
 * DRM_IOCTL_INFO_BUFS ioctl argument type.
 */
struct drm_buf_info {
	int count;		/**< Entries in list */
	struct drm_buf_desc *list;
};

/*
 * DRM_IOCTL_FREE_BUFS ioctl argument type.
 */
struct drm_buf_free {
	int count;
	int *list;
};

/*
 * Buffer information
 *
 * \sa drm_buf_map.
 */
struct drm_buf_pub {
	int idx;		       /**< Index into the master buffer list */
	int total;		       /**< Buffer size */
	int used;		       /**< Amount of buffer in use (for DMA) */
	void *address;	       /**< Address of buffer */
};

/*
 * DRM_IOCTL_MAP_BUFS ioctl argument type.
 */
struct drm_buf_map {
	int count;		/**< Length of the buffer list */
#ifdef __cplusplus
	void *virt;
#else
	void *virtual;		/**< Mmap'd area in user-virtual */
#endif
	struct drm_buf_pub *list;	/**< Buffer information */
};

/*
 * DRM_IOCTL_DMA ioctl argument type.
 *
 * Indices here refer to the offset into the buffer list in drm_buf_get.
 *
 * \sa drmDMA().
 */
struct drm_dma {
	int context;			  /**< Context handle */
	int send_count;			  /**< Number of buffers to send */
	int *send_indices;	  /**< List of handles to buffers */
	int *send_sizes;		  /**< Lengths of data to send */
	enum drm_dma_flags flags;	  /**< Flags */
	int request_count;		  /**< Number of buffers requested */
	int request_size;		  /**< Desired size for buffers */
	int *request_indices;	  /**< Buffer information */
	int *request_sizes;
	int granted_count;		  /**< Number of buffers granted */
};

enum drm_ctx_flags {
	_DRM_CONTEXT_PRESERVED = 0x01,
	_DRM_CONTEXT_2DONLY = 0x02
};

/*
 * DRM_IOCTL_ADD_CTX ioctl argument type.
 *
 * \sa drmCreateContext() and drmDestroyContext().
 */
struct drm_ctx {
	drm_context_t handle;
	enum drm_ctx_flags flags;
};

/*
 * DRM_IOCTL_RES_CTX ioctl argument type.
 */
struct drm_ctx_res {
	int count;
	struct drm_ctx *contexts;
};

/*
 * DRM_IOCTL_ADD_DRAW and DRM_IOCTL_RM_DRAW ioctl argument type.
 */
struct drm_draw {
	drm_drawable_t handle;
};

/*
 * DRM_IOCTL_UPDATE_DRAW ioctl argument type.
 */
typedef enum {
	DRM_DRAWABLE_CLIPRECTS
} drm_drawable_info_type_t;

struct drm_update_draw {
	drm_drawable_t handle;
	unsigned int type;
	unsigned int num;
	unsigned long long data;
};

/*
 * DRM_IOCTL_GET_MAGIC and DRM_IOCTL_AUTH_MAGIC ioctl argument type.
 */
struct drm_auth {
	drm_magic_t magic;
};

/*
 * DRM_IOCTL_IRQ_BUSID ioctl argument type.
 *
 * \sa drmGetInterruptFromBusID().
 */
struct drm_irq_busid {
	int irq;	/**< IRQ number */
	int busnum;	/**< bus number */
	int devnum;	/**< device number */
	int funcnum;	/**< function number */
};

enum drm_vblank_seq_type {
	_DRM_VBLANK_ABSOLUTE = 0x0,	/**< Wait for specific vblank sequence number */
	_DRM_VBLANK_RELATIVE = 0x1,	/**< Wait for given number of vblanks */
	/* bits 1-6 are reserved for high crtcs */
	_DRM_VBLANK_HIGH_CRTC_MASK = 0x0000003e,
	_DRM_VBLANK_EVENT = 0x4000000,   /**< Send event instead of blocking */
	_DRM_VBLANK_FLIP = 0x8000000,   /**< Scheduled buffer swap should flip */
	_DRM_VBLANK_NEXTONMISS = 0x10000000,	/**< If missed, wait for next vblank */
	_DRM_VBLANK_SECONDARY = 0x20000000,	/**< Secondary display controller */
	_DRM_VBLANK_SIGNAL = 0x40000000	/**< Send signal instead of blocking, unsupported */
};
#define _DRM_VBLANK_HIGH_CRTC_SHIFT 1

#define _DRM_VBLANK_TYPES_MASK (_DRM_VBLANK_ABSOLUTE | _DRM_VBLANK_RELATIVE)
#define _DRM_VBLANK_FLAGS_MASK (_DRM_VBLANK_EVENT | _DRM_VBLANK_SIGNAL | \
				_DRM_VBLANK_SECONDARY | _DRM_VBLANK_NEXTONMISS)

struct drm_wait_vblank_request {
	enum drm_vblank_seq_type type;
	unsigned int sequence;
	unsigned long signal;
};

struct drm_wait_vblank_reply {
	enum drm_vblank_seq_type type;
	unsigned int sequence;
	long tval_sec;
	long tval_usec;
};

/*
 * DRM_IOCTL_WAIT_VBLANK ioctl argument type.
 *
 * \sa drmWaitVBlank().
 */
union drm_wait_vblank {
	struct drm_wait_vblank_request request;
	struct drm_wait_vblank_reply reply;
};

#define _DRM_PRE_MODESET 1
#define _DRM_POST_MODESET 2

/*
 * DRM_IOCTL_MODESET_CTL ioctl argument type
 *
 * \sa drmModesetCtl().
 */
struct drm_modeset_ctl {
	__u32 crtc;
	__u32 cmd;
};

/*
 * DRM_IOCTL_AGP_ENABLE ioctl argument type.
 *
 * \sa drmAgpEnable().
 */
struct drm_agp_mode {
	unsigned long mode;	/**< AGP mode */
};

/*
 * DRM_IOCTL_AGP_ALLOC and DRM_IOCTL_AGP_FREE ioctls argument type.
 *
 * \sa drmAgpAlloc() and drmAgpFree().
 */
struct drm_agp_buffer {
	unsigned long size;	/**< In bytes -- will round to page boundary */
	unsigned long handle;	/**< Used for binding / unbinding */
	unsigned long type;	/**< Type of memory to allocate */
	unsigned long physical;	/**< Physical used by i810 */
};

/*
 * DRM_IOCTL_AGP_BIND and DRM_IOCTL_AGP_UNBIND ioctls argument type.
 *
 * \sa drmAgpBind() and drmAgpUnbind().
 */
struct drm_agp_binding {
	unsigned long handle;	/**< From drm_agp_buffer */
	unsigned long offset;	/**< In bytes -- will round to page boundary */
};

/*
 * DRM_IOCTL_AGP_INFO ioctl argument type.
 *
 * \sa drmAgpVersionMajor(), drmAgpVersionMinor(), drmAgpGetMode(),
 * drmAgpBase(), drmAgpSize(), drmAgpMemoryUsed(), drmAgpMemoryAvail(),
 * drmAgpVendorId() and drmAgpDeviceId().
 */
struct drm_agp_info {
	int agp_version_major;
	int agp_version_minor;
	unsigned long mode;
	unsigned long aperture_base;	/* physical address */
	unsigned long aperture_size;	/* bytes */
	unsigned long memory_allowed;	/* bytes */
	unsigned long memory_used;

	/* PCI information */
	unsigned short id_vendor;
	unsigned short id_device;
};

/*
 * DRM_IOCTL_SG_ALLOC ioctl argument type.
 */
struct drm_scatter_gather {
	unsigned long size;	/**< In bytes -- will round to page boundary */
	unsigned long handle;	/**< Used for mapping / unmapping */
};

/*
 * DRM_IOCTL_SET_VERSION ioctl argument type.
 */
struct drm_set_version {
	int drm_di_major;
	int drm_di_minor;
	int drm_dd_major;
	int drm_dd_minor;
};

/* DRM_IOCTL_GEM_CLOSE ioctl argument type */
struct drm_gem_close {
	/** Handle of the object to be closed. */
	__u32 handle;
	__u32 pad;
};

/* DRM_IOCTL_GEM_FLINK ioctl argument type */
struct drm_gem_flink {
	/** Handle for the object being named */
	__u32 handle;

	/** Returned global name */
	__u32 name;
};

/* DRM_IOCTL_GEM_OPEN ioctl argument type */
struct drm_gem_open {
	/** Name of object being opened */
	__u32 name;

	/** Returned handle for the object */
	__u32 handle;

	/** Returned size of the object */
	__u64 size;
};

/**
 * DRM_CAP_DUMB_BUFFER
 *
 * If set to 1, the driver supports creating dumb buffers via the
 * &DRM_IOCTL_MODE_CREATE_DUMB ioctl.
 */
#define DRM_CAP_DUMB_BUFFER		0x1
/**
 * DRM_CAP_VBLANK_HIGH_CRTC
 *
 * If set to 1, the kernel supports specifying a :ref:`CRTC index<crtc_index>`
 * in the high bits of &drm_wait_vblank_request.type.
 *
 * Starting kernel version 2.6.39, this capability is always set to 1.
 */
#define DRM_CAP_VBLANK_HIGH_CRTC	0x2
/**
 * DRM_CAP_DUMB_PREFERRED_DEPTH
 *
 * The preferred bit depth for dumb buffers.
 *
 * The bit depth is the number of bits used to indicate the color of a single
 * pixel excluding any padding. This is different from the number of bits per
 * pixel. For instance, XRGB8888 has a bit depth of 24 but has 32 bits per
 * pixel.
 *
 * Note that this preference only applies to dumb buffers, it's irrelevant for
 * other types of buffers.
 */
#define DRM_CAP_DUMB_PREFERRED_DEPTH	0x3
/**
 * DRM_CAP_DUMB_PREFER_SHADOW
 *
 * If set to 1, the driver prefers userspace to render to a shadow buffer
 * instead of directly rendering to a dumb buffer. For best speed, userspace
 * should do streaming ordered memory copies into the dumb buffer and never
 * read from it.
 *
 * Note that this preference only applies to dumb buffers, it's irrelevant for
 * other types of buffers.
 */
#define DRM_CAP_DUMB_PREFER_SHADOW	0x4
/**
 * DRM_CAP_PRIME
 *
 * Bitfield of supported PRIME sharing capabilities. See &DRM_PRIME_CAP_IMPORT
 * and &DRM_PRIME_CAP_EXPORT.
 *
 * PRIME buffers are exposed as dma-buf file descriptors. See
 * Documentation/gpu/drm-mm.rst, section "PRIME Buffer Sharing".
 */
#define DRM_CAP_PRIME			0x5
/**
 * DRM_PRIME_CAP_IMPORT
 *
 * If this bit is set in &DRM_CAP_PRIME, the driver supports importing PRIME
 * buffers via the &DRM_IOCTL_PRIME_FD_TO_HANDLE ioctl.
 */
#define  DRM_PRIME_CAP_IMPORT		0x1
/**
 * DRM_PRIME_CAP_EXPORT
 *
 * If this bit is set in &DRM_CAP_PRIME, the driver supports exporting PRIME
 * buffers via the &DRM_IOCTL_PRIME_HANDLE_TO_FD ioctl.
 */
#define  DRM_PRIME_CAP_EXPORT		0x2
/**
 * DRM_CAP_TIMESTAMP_MONOTONIC
 *
 * If set to 0, the kernel will report timestamps with ``CLOCK_REALTIME`` in
 * struct drm_event_vblank. If set to 1, the kernel will report timestamps with
 * ``CLOCK_MONOTONIC``. See ``clock_gettime(2)`` for the definition of these
 * clocks.
 *
 * Starting from kernel version 2.6.39, the default value for this capability
 * is 1. Starting kernel version 4.15, this capability is always set to 1.
 */
#define DRM_CAP_TIMESTAMP_MONOTONIC	0x6
/**
 * DRM_CAP_ASYNC_PAGE_FLIP
 *
 * If set to 1, the driver supports &DRM_MODE_PAGE_FLIP_ASYNC.
 */
#define DRM_CAP_ASYNC_PAGE_FLIP		0x7
/**
 * DRM_CAP_CURSOR_WIDTH
 *
 * The ``CURSOR_WIDTH`` and ``CURSOR_HEIGHT`` capabilities return a valid
 * width x height combination for the hardware cursor. The intention is that a
 * hardware agnostic userspace can query a cursor plane size to use.
 *
 * Note that the cross-driver contract is to merely return a valid size;
 * drivers are free to attach another meaning on top, eg. i915 returns the
 * maximum plane size.
 */
#define DRM_CAP_CURSOR_WIDTH		0x8
/**
 * DRM_CAP_CURSOR_HEIGHT
 *
 * See &DRM_CAP_CURSOR_WIDTH.
 */
#define DRM_CAP_CURSOR_HEIGHT		0x9
/**
 * DRM_CAP_ADDFB2_MODIFIERS
 *
 * If set to 1, the driver supports supplying modifiers in the
 * &DRM_IOCTL_MODE_ADDFB2 ioctl.
 */
#define DRM_CAP_ADDFB2_MODIFIERS	0x10
/**
 * DRM_CAP_PAGE_FLIP_TARGET
 *
 * If set to 1, the driver supports the &DRM_MODE_PAGE_FLIP_TARGET_ABSOLUTE and
 * &DRM_MODE_PAGE_FLIP_TARGET_RELATIVE flags in
 * &drm_mode_crtc_page_flip_target.flags for the &DRM_IOCTL_MODE_PAGE_FLIP
 * ioctl.
 */
#define DRM_CAP_PAGE_FLIP_TARGET	0x11
/**
 * DRM_CAP_CRTC_IN_VBLANK_EVENT
 *
 * If set to 1, the kernel supports reporting the CRTC ID in
 * &drm_event_vblank.crtc_id for the &DRM_EVENT_VBLANK and
 * &DRM_EVENT_FLIP_COMPLETE events.
 *
 * Starting kernel version 4.12, this capability is always set to 1.
 */
#define DRM_CAP_CRTC_IN_VBLANK_EVENT	0x12
