/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 * omap3isp.h
 *
 * TI OMAP3 ISP - User-space API
 *
 * Copyright (C) 2010 Nokia Corporation
 * Copyright (C) 2009 Texas Instruments, Inc.
 *
 * Contacts: Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 *	     Sakari Ailus <sakari.ailus@iki.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef OMAP3_ISP_USER_H
#define OMAP3_ISP_USER_H

#include <linux/types.h>
#include <linux/videodev2.h>

/*
 * Private IOCTLs
 *
 * VIDIOC_OMAP3ISP_CCDC_CFG: Set CCDC configuration
 * VIDIOC_OMAP3ISP_PRV_CFG: Set preview engine configuration
 * VIDIOC_OMAP3ISP_AEWB_CFG: Set AEWB module configuration
 * VIDIOC_OMAP3ISP_HIST_CFG: Set histogram module configuration
 * VIDIOC_OMAP3ISP_AF_CFG: Set auto-focus module configuration
 * VIDIOC_OMAP3ISP_STAT_REQ: Read statistics (AEWB/AF/histogram) data
 * VIDIOC_OMAP3ISP_STAT_EN: Enable/disable a statistics module
 */

#define VIDIOC_OMAP3ISP_CCDC_CFG \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 1, struct omap3isp_ccdc_update_config)
#define VIDIOC_OMAP3ISP_PRV_CFG \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 2, struct omap3isp_prev_update_config)
#define VIDIOC_OMAP3ISP_AEWB_CFG \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 3, struct omap3isp_h3a_aewb_config)
#define VIDIOC_OMAP3ISP_HIST_CFG \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 4, struct omap3isp_hist_config)
#define VIDIOC_OMAP3ISP_AF_CFG \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 5, struct omap3isp_h3a_af_config)
#define VIDIOC_OMAP3ISP_STAT_REQ \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 6, struct omap3isp_stat_data)
#define VIDIOC_OMAP3ISP_STAT_REQ_TIME32 \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 6, struct omap3isp_stat_data_time32)
#define VIDIOC_OMAP3ISP_STAT_EN \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 7, unsigned long)

/*
 * Events
 *
 * V4L2_EVENT_OMAP3ISP_AEWB: AEWB statistics data ready
 * V4L2_EVENT_OMAP3ISP_AF: AF statistics data ready
 * V4L2_EVENT_OMAP3ISP_HIST: Histogram statistics data ready
 */

#define V4L2_EVENT_OMAP3ISP_CLASS	(V4L2_EVENT_PRIVATE_START | 0x100)
#define V4L2_EVENT_OMAP3ISP_AEWB	(V4L2_EVENT_OMAP3ISP_CLASS | 0x1)
#define V4L2_EVENT_OMAP3ISP_AF		(V4L2_EVENT_OMAP3ISP_CLASS | 0x2)
#define V4L2_EVENT_OMAP3ISP_HIST	(V4L2_EVENT_OMAP3ISP_CLASS | 0x3)

struct omap3isp_stat_event_status {
	__u32 frame_number;
	__u16 config_counter;
	__u8 buf_err;
};

/* AE/AWB related structures and flags*/

/* H3A Range Constants */
#define OMAP3ISP_AEWB_MAX_SATURATION_LIM	1023
#define OMAP3ISP_AEWB_MIN_WIN_H			2
#define OMAP3ISP_AEWB_MAX_WIN_H			256
#define OMAP3ISP_AEWB_MIN_WIN_W			6
#define OMAP3ISP_AEWB_MAX_WIN_W			256
#define OMAP3ISP_AEWB_MIN_WINVC			1
#define OMAP3ISP_AEWB_MIN_WINHC			1
#define OMAP3ISP_AEWB_MAX_WINVC			128
#define OMAP3ISP_AEWB_MAX_WINHC			36
#define OMAP3ISP_AEWB_MAX_WINSTART		4095
#define OMAP3ISP_AEWB_MIN_SUB_INC		2
#define OMAP3ISP_AEWB_MAX_SUB_INC		32
#define OMAP3ISP_AEWB_MAX_BUF_SIZE		83600

#define OMAP3ISP_AF_IIRSH_MIN			0
#define OMAP3ISP_AF_IIRSH_MAX			4095
#define OMAP3ISP_AF_PAXEL_HORIZONTAL_COUNT_MIN	1
#define OMAP3ISP_AF_PAXEL_HORIZONTAL_COUNT_MAX	36
#define OMAP3ISP_AF_PAXEL_VERTICAL_COUNT_MIN	1
#define OMAP3ISP_AF_PAXEL_VERTICAL_COUNT_MAX	128
#define OMAP3ISP_AF_PAXEL_INCREMENT_MIN		2
#define OMAP3ISP_AF_PAXEL_INCREMENT_MAX		32
#define OMAP3ISP_AF_PAXEL_HEIGHT_MIN		2
#define OMAP3ISP_AF_PAXEL_HEIGHT_MAX		256
#define OMAP3ISP_AF_PAXEL_WIDTH_MIN		16
#define OMAP3ISP_AF_PAXEL_WIDTH_MAX		256
#define OMAP3ISP_AF_PAXEL_HZSTART_MIN		1
#define OMAP3ISP_AF_PAXEL_HZSTART_MAX		4095
#define OMAP3ISP_AF_PAXEL_VTSTART_MIN		0
#define OMAP3ISP_AF_PAXEL_VTSTART_MAX		4095
#define OMAP3ISP_AF_THRESHOLD_MAX		255
#define OMAP3ISP_AF_COEF_MAX			4095
#define OMAP3ISP_AF_PAXEL_SIZE			48
#define OMAP3ISP_AF_MAX_BUF_SIZE		221184

/**
 * struct omap3isp_h3a_aewb_config - AE AWB configuration reset values
 * saturation_limit: Saturation limit.
 * @win_height: Window Height. Range 2 - 256, even values only.
 * @win_width: Window Width. Range 6 - 256, even values only.
 * @ver_win_count: Vertical Window Count. Range 1 - 128.
 * @hor_win_count: Horizontal Window Count. Range 1 - 36.
 * @ver_win_start: Vertical Window Start. Range 0 - 4095.
 * @hor_win_start: Horizontal Window Start. Range 0 - 4095.
 * @blk_ver_win_start: Black Vertical Windows Start. Range 0 - 4095.
 * @blk_win_height: Black Window Height. Range 2 - 256, even values only.
 * @subsample_ver_inc: Subsample Vertical points increment Range 2 - 32, even
 *                     values only.
 * @subsample_hor_inc: Subsample Horizontal points increment Range 2 - 32, even
 *                     values only.
 * @alaw_enable: AEW ALAW EN flag.
 */
struct omap3isp_h3a_aewb_config {
	/*
	 * Common fields.
	 * They should be the first ones and must be in the same order as in
	 * ispstat_generic_config struct.
	 */
	__u32 buf_size;
	__u16 config_counter;

	/* Private fields */
	__u16 saturation_limit;
	__u16 win_height;
	__u16 win_width;
	__u16 ver_win_count;
	__u16 hor_win_count;
	__u16 ver_win_start;
	__u16 hor_win_start;
	__u16 blk_ver_win_start;
	__u16 blk_win_height;
	__u16 subsample_ver_inc;
	__u16 subsample_hor_inc;
	__u8 alaw_enable;
};

/**
 * struct omap3isp_stat_data - Statistic data sent to or received from user
 * @ts: Timestamp of returned framestats.
 * @buf: Pointer to pass to user.
 * @frame_number: Frame number of requested stats.
 * @cur_frame: Current frame number being processed.
 * @config_counter: Number of the configuration associated with the data.
 */
struct omap3isp_stat_data {
	struct timeval ts;
	void *buf;
	__u32 buf_size;
	__u16 frame_number;
	__u16 cur_frame;
	__u16 config_counter;
};


/* Histogram related structs */

/* Flags for number of bins */
#define OMAP3ISP_HIST_BINS_32		0
#define OMAP3ISP_HIST_BINS_64		1
#define OMAP3ISP_HIST_BINS_128		2
#define OMAP3ISP_HIST_BINS_256		3

/* Number of bins * 4 colors * 4-bytes word */
#define OMAP3ISP_HIST_MEM_SIZE_BINS(n)	((1 << ((n)+5))*4*4)

#define OMAP3ISP_HIST_MEM_SIZE		1024
#define OMAP3ISP_HIST_MIN_REGIONS	1
#define OMAP3ISP_HIST_MAX_REGIONS	4
#define OMAP3ISP_HIST_MAX_WB_GAIN	255
#define OMAP3ISP_HIST_MIN_WB_GAIN	0
#define OMAP3ISP_HIST_MAX_BIT_WIDTH	14
#define OMAP3ISP_HIST_MIN_BIT_WIDTH	8
#define OMAP3ISP_HIST_MAX_WG		4
#define OMAP3ISP_HIST_MAX_BUF_SIZE	4096

/* Source */
#define OMAP3ISP_HIST_SOURCE_CCDC	0
#define OMAP3ISP_HIST_SOURCE_MEM	1

/* CFA pattern */
#define OMAP3ISP_HIST_CFA_BAYER		0
#define OMAP3ISP_HIST_CFA_FOVEONX3	1

struct omap3isp_hist_region {
	__u16 h_start;
	__u16 h_end;
	__u16 v_start;
	__u16 v_end;
};

struct omap3isp_hist_config {
	/*
	 * Common fields.
	 * They should be the first ones and must be in the same order as in
	 * ispstat_generic_config struct.
	 */
	__u32 buf_size;
	__u16 config_counter;

	__u8 num_acc_frames;	/* Num of image frames to be processed and
				   accumulated for each histogram frame */
	__u16 hist_bins;	/* number of bins: 32, 64, 128, or 256 */
	__u8 cfa;		/* BAYER or FOVEON X3 */
	__u8 wg[OMAP3ISP_HIST_MAX_WG];	/* White Balance Gain */
	__u8 num_regions;	/* number of regions to be configured */
	struct omap3isp_hist_region region[OMAP3ISP_HIST_MAX_REGIONS];
};

/* Auto Focus related structs */

#define OMAP3ISP_AF_NUM_COEF		11

enum omap3isp_h3a_af_fvmode {
	OMAP3ISP_AF_MODE_SUMMED = 0,
	OMAP3ISP_AF_MODE_PEAK = 1
};

/* Red, Green, and blue pixel location in the AF windows */
enum omap3isp_h3a_af_rgbpos {
	OMAP3ISP_AF_GR_GB_BAYER = 0,	/* GR and GB as Bayer pattern */
	OMAP3ISP_AF_RG_GB_BAYER = 1,	/* RG and GB as Bayer pattern */
	OMAP3ISP_AF_GR_BG_BAYER = 2,	/* GR and BG as Bayer pattern */
	OMAP3ISP_AF_RG_BG_BAYER = 3,	/* RG and BG as Bayer pattern */
	OMAP3ISP_AF_GG_RB_CUSTOM = 4,	/* GG and RB as custom pattern */
	OMAP3ISP_AF_RB_GG_CUSTOM = 5	/* RB and GG as custom pattern */
};

/* Contains the information regarding the Horizontal Median Filter */
struct omap3isp_h3a_af_hmf {
	__u8 enable;	/* Status of Horizontal Median Filter */
	__u8 threshold;	/* Threshold