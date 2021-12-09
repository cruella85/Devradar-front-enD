/* SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note */
/*
 * Copyright © International Business Machines Corp., 2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Artem Bityutskiy (Битюцкий Артём)
 */

#ifndef __UBI_USER_H__
#define __UBI_USER_H__

#include <linux/types.h>

/*
 * UBI device creation (the same as MTD device attachment)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * MTD devices may be attached using %UBI_IOCATT ioctl command of the UBI
 * control device. The caller has to properly fill and pass
 * &struct ubi_attach_req object - UBI will attach the MTD device specified in
 * the request and return the newly created UBI device number as the ioctl
 * return value.
 *
 * UBI device deletion (the same as MTD device detachment)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * An UBI device maybe deleted with %UBI_IOCDET ioctl command of the UBI
 * control device.
 *
 * UBI volume creation
 * ~~~~~~~~~~~~~~~~~~~
 *
 * UBI volumes are created via the %UBI_IOCMKVOL ioctl command of UBI character
 * device. A &struct ubi_mkvol_req object has to be properly filled and a
 * pointer to it has to be passed to the ioctl.
 *
 * UBI volume deletion
 * ~~~~~~~~~~~~~~~~~~~
 *
 * To delete a volume, the %UBI_IOCRMVOL ioctl command of the UBI character
 * device should be used. A pointer to the 32-bit volume ID hast to be passed
 * to the ioctl.
 *
 * UBI volume re-size
 * ~~~~~~~~~~~~~~~~~~
 *
 * To re-size a volume, the %UBI_IOCRSVOL ioctl command of the UBI character
 * device should be used. A &struct ubi_rsvol_req object has to be properly
 * filled and a pointer to it has to be passed to the ioctl.
 *
 * UBI volumes re-name
 * ~~~~~~~~~~~~~~~~~~~
 *
 * To re-name several volumes atomically at one go, the %UBI_IOCRNVOL command
 * of the UBI character device should be used. A &struct ubi_rnvol_req object
 * has to be properly filled and a pointer to it has to be passed to the ioctl.
 *
 * UBI volume update
 * ~~~~~~~~~~~~~~~~~
 *
 * Volume update should be done via the %UBI_IOCVOLUP ioctl command of the
 * corresponding UBI volume character device. A pointer to a 64-bit update
 * size should be passed to the ioctl. After this, UBI expects user to write
 * this number of bytes to the volume character device. The update is finished
 * when the claimed number of bytes is passed. So, the volume update sequence
 * is something like:
 *
 * fd = open("/dev/my_volume");
 * ioctl(fd, UBI_IOCVOLUP, &image_size);
 * write(fd, buf, image_size);
 * close(fd);
 *
 * Logical eraseblock erase
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To erase a logical eraseblock, the %UBI_IOCEBER ioctl command of the
 * corresponding UBI volume character device should be used. This command
 * unmaps the requested logical eraseblock, makes sure the corresponding
 * physical eraseblock is successfully erased, and returns.
 *
 * Atomic logical eraseblock change
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Atomic logical eraseblock change operation is called using the %UBI_IOCEBCH
 * ioctl command of the corresponding UBI volume character device. A pointer to
 * a &struct ubi_leb_change_req object has to be passed to the ioctl. Then the
 * user is expected to write the requested amount of bytes (similarly to what
 * should be done in case of the "volume update" ioctl).
 *
 * Logical eraseblock map
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * To map a logical eraseblock to a physical eraseblock, the %UBI_IOCEBMAP
 * ioctl command should be used. A pointer to a &struct ubi_map_req object is
 * expected to be passed. The ioctl maps the requested logical eraseblock to
 * a physical eraseblock and returns.  Only non-mapped logical eraseblocks can
 * be mapped. If the logical eraseblock specified in the request is already
 * mapped to a physical eraseblock, the ioctl fails and returns error.
 *
 * Logical eraseblock unmap
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To unmap a logical eraseblock to a physical eraseblock, the %UBI_IOCEBUNMAP
 * ioctl command should be used. The ioctl unmaps the logical eraseblocks,
 * schedules corresponding physical eraseblock for erasure, and returns. Unlike
 * the "LEB erase" command, it does not wait for the physical eraseblock being
 * erased. Note, the side effect of this is that if an unclean reboot happens
 * after the unmap ioctl returns, you may find the LEB mapped again to the same
 * physical eraseblock after the UBI is run again.
 *
 * Check if logical eraseblock is mapped
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To check if a logical eraseblock is mapped to a physical eraseblock, the
 * %UBI_IOCEBISMAP ioctl command should be used. It returns %0 if the LEB is
 * not mapped, and %1