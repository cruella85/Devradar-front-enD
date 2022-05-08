/* SPDX-License-Identifier: LGPL-2.1+ WITH Linux-syscall-note */
/*
 * osd.h - DEPRECATED On Screen Display API
 *
 * NOTE: should not be used on future drivers
 *
 * Copyright (C) 2001 Ralph  Metzler <ralph@convergence.de>
 *                  & Marcus Metzler <marcus@convergence.de>
 *                    for convergence integrated media GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Lesser Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DVBOSD_H_
#define _DVBOSD_H_



typedef enum {
	/* All functions return -2 on "not open" */
	OSD_Close = 1,	/* () */
	/*
	 * Disables OSD and releases the buffers
	 * returns 0 on success
	 */
	OSD_Open,	/* (x0,y0,x1,y1,BitPerPixel[2/4/8](color&0x0F),mix[0..15](color&0xF0)) */
	/*
	 * Opens OSD with this size and bit depth
	 * returns 0 on success, -1 on DRAM allocation error, -2 on "already open"
	 */
	OSD_Show,	/* () */
	/*
	 * enables OSD mode
	 * returns 0 on success
	 */
	OSD_Hide,	/* () */
	/*
	 * disables OSD mode
	 * returns 0 on success
	 */
	OSD_Clear,	/* () */
	/*
	 * Sets all pixel to color 0
	 * returns 0 on success
	 */
	OSD_Fill,	/* (color) */
	/*
	 * Sets all pixel to color <col>
	 * returns 0 on success
	 */
	OSD_SetColor,	/* (color,R{x0},G{y0},B{x1},opacity{y1}) */
	/*
	 * set palette entry <num> to <r,g,b>, <mix> and <trans> apply
	 * R,G,B: 0..255
	 * R=Red, G=Green, B=Blue
	 * opacity=0:      pixel opacity 0% (only video pixel shows)
	 * opacity=1..254: pixel opacity as specified in header
	 * opacity=255:    pixel opacity 100% (only OSD pixel shows)
	 * returns 0 on success, -1 on error
	 */
	OSD_SetPalette,	/* (firstcolor{color},lastcolor{x0},data) */
	/*
	 * Set a number of entries in the palette
	 * sets the entries "firstcolor" through "lastcolor" from the array "data"
	 * data