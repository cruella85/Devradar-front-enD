/* r128_drm.h -- Public header for the r128 driver -*- linux-c -*-
 * Created: Wed Apr  5 19:24:19 2000 by kevin@precisioninsight.com
 */
/*
 * Copyright 2000 Precision Insight, Inc., Cedar Park, Texas.
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
 * PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Gareth Hughes <gareth@valinux.com>
 *    Kevin E. Martin <martin@valinux.com>
 */

#ifndef __R128_DRM_H__
#define __R128_DRM_H__

#include "drm.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* WARNING: If you change any of these defines, make sure to change the
 * defines in the X server file (r128_sarea.h)
 */
#ifndef __R128_SAREA_DEFINES__
#define __R128_SAREA_DEFINES__

/* What needs to be changed for the current vertex buffer?
 */
#define R128_UPLOAD_CONTEXT		0x001
#define R128_UPLOAD_SETUP		0x002
#define R128_UPLOAD_TEX0		0x004
#define R128_UPLOAD_TEX1		0x008
#define R128_UPLOAD_TEX0IMAGE