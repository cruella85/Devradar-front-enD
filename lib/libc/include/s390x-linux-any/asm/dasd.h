/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 * Author(s)......: Holger Smolinski <Holger.Smolinski@de.ibm.com>
 * Bugreports.to..: <Linux390@de.ibm.com>
 * Copyright IBM Corp. 1999, 2000
 * EMC Symmetrix ioctl Copyright EMC Corporation, 2008
 * Author.........: Nigel Hislop <hislop_nigel@emc.com>
 *
 * This file is the interface of the DASD device driver, which is exported to user space
 * any future changes wrt the API will result in a change of the APIVERSION reported
 * to userspace by the DASDAPIVER-ioctl
 *
 */

#ifndef DASD_H
#define DASD_H
#include <linux/types.h>
#include <linux/ioctl.h>

#define DASD_IOCTL_LETTER 'D'

#define DASD_API_VERSION 6

/*
 * struct dasd_information2_t
 * represents any data about the device, which is visible to userspace.
 *  including foramt and featueres.
 */
typedef struct dasd_information2_t {
	unsigned int devno;	    /* S/390 devno */
	unsigned int real_devno;    /* for aliases */
	unsigned int schid;	    /* S/390 subchannel identifier */
	unsigned int cu_type  : 16; /* from SenseID */
	unsigned int cu_model :  8; /* from SenseID */
	unsigned int dev_type : 16; /* from SenseID */
	unsigned int dev_model : 8; /* from SenseID */
	unsigned int open_count;
	unsigned int req_queue_len;
	unsigned int chanq_len;     /* length of chanq */
	char type[4];		    /* from discipline.name, 'none' for unknown */
	unsigned int status;	    /* current device level */
	unsigned int label_block;   /* where to find the VOLSER */
	unsigned int FBA_layout;    /* fixed block size (like AIXVOL) */
	unsigned int characteristics_size;
	unsigned int confdata_size;
	char characteristics[64];   /* from read_device_characteristics */
	char configuration_data[256]; /* from read_configuration_data */
	unsigned int format;	      /* format info like formatted/cdl/ldl/... */
	unsigned int features;	      /* dasd features like 'ro',...		*/
	unsigned int reserved0;       /* reserved for further use ,...		*/
	unsigned int reserved1;       /* reserved for further use ,...		*/
	unsigned int reserved2;       /* reserved for further use ,...		*/
	unsigned int reserved3;       /* reserved for further use ,...		*/
	unsigned int reserved4;       /* reserved for further use ,...		*/
	unsigned int reserved5;       /* reserved for further use ,...		*/
	unsigned int reserved6;       /* reserved for further use ,...		*/
	unsigned int reserved7;       /* reserved for further use ,...		*/
} dasd_information2_t;

/*
 * values to be used for dasd_information_t.format
 * 0x00: NOT formatted
 * 0x01: Linux disc layout
 * 0x02: Common disc layout
 */
#define DASD_FORMAT_NONE 0
#define DASD_FORMAT_LDL  1
#define DASD_FORMAT_CDL  2
/*
 * values to be used for dasd_information_t.features
 * 0x100: default features
 * 0x001: readonly (ro)
 * 0x002: use diag discipline (diag)
 * 0x004: set the device initially online (internal use only)
 * 0x008: enable ERP related logging
 * 0x010: allow I/O to fail on lost paths
 * 0x020: allow I/O to fail when a lock was stolen
 * 0x040: give access to raw eckd data
 * 0x080: enable discard support
 * 0x100: enable autodisable for IFCC errors (default)
 */
#define DASD_FEATURE_READONLY	      0x001
#define DASD_FEATURE_USEDIAG	      0x002
#define DASD_FEATURE_INITIAL_ONLINE   0x004
#define DASD_FEATURE_ERPLOG	      0x008
#define DASD_FEATURE_FAILFAST	      0x010
#define DASD_FEATURE_FAILONSLCK       0x020
#define DASD_FEATURE_USERAW	      0x040
#define DASD_FEATURE_DISCARD	      0x080
#define DASD_FEATURE_PATH_AUTODISABLE 0x100
#define DASD_FEATURE_DEFAULT	      DASD_FEATURE_PATH_AUTODISABLE

#define DASD_PARTN_BITS 2

/*
 * struct dasd_information_t
 * represents any data about the data, which is visible to userspace
 */
typedef struct dasd_information_t {
	unsigned int devno;	    /* S/390 devno */
	unsigned int real_devno;    /* for aliases */
	unsigned int schid;	    /* S/390 subchannel identifier */
	unsigned int cu_type  : 16; /* from SenseID */
	unsigned int cu_model :  8; /* from SenseID */
	unsigned int dev_type : 16; /* from SenseID */
	unsigned int dev_model : 8; /* from SenseID */
	unsigned int open_count;
	unsigned int req_queue_len;
	unsigned int chanq_len;     /* length of chanq */
	char type[4];		    /* from discipline.name, 'none' for unknown */
	unsigned int status;	    /* current device level */
	unsigned int label_block;   /* where to find the VOLSER */
	unsigned int FBA_layout;    /* fixed block size (like AIXVOL) */
	unsigned int characteristics_size;
	unsigned int confdata_size;
	char characteristics[64];   /* from read_device_characteristics */
	char configuration_data[256]; /* from read_configuration_data */
} dasd_information_t;

/*
 * Read Subsystem Data - Performance Statistics
 */
typedef struct dasd_rssd_perf_stats_t {
	unsigned char  invalid:1;
	unsigned char  format:3;
	unsigned char  data_format:4;
	unsigned char  unit_address;
	unsigned short device_status;
	unsigned int   nr_read_normal;
	unsigned int   nr_read_normal_hits;
	unsigned int   nr_write_normal;
	unsigned int   nr_write_fast_normal_hits;
	unsigned int   nr_read_seq;
	unsigned int   nr_read_seq_hits;
	unsigned int   nr_write_seq;
	unsigned int   nr_write_fast_seq_hits;
	unsigned int   nr_read_cache;
	unsigned int   nr_read_cache_hits;
	unsigned int   nr_write_cache;
	unsigned int   nr_write_fast_cache_hits;
	unsigned int   nr_inhibit_cache;
	unsigned int   nr_bybass_cache;
	unsigned int   nr_seq_dasd_to_cache;
	unsigned int   nr_dasd_to_cache;
	unsigned int   nr_cache_to_dasd;
	unsigned int   nr_delayed_fast_write;
	unsigned int   nr_normal_fast_write;
	unsigned int   nr_seq_fast_write;
	unsigned int   nr_cache_miss;
	unsigned char  status2;
	unsigned int   nr_quick_write_promotes;
	unsigned char  reserved;
	unsigned short ssid;
	unsigned char  reseved2[96];
} __attribute__((packed)) dasd_rssd_perf_stats_t;

/*
 * struct profile_info_t
 * holds the profinling information
 */
typedef struct dasd_profile_info_t {
	unsigned int dasd_io_reqs;	 /* number of requests processed at all */
	unsigned int dasd_io