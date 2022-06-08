/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/* atm.h - general ATM declarations */
 
/* Written 1995-2000 by Werner Almesberger, EPFL LRC/ICA */
 

/*
 * WARNING: User-space programs should not #include <linux/atm.h> directly.
 *          Instead, #include <atm.h>
 */

#ifndef _LINUX_ATM_H
#define _LINUX_ATM_H

/*
 * BEGIN_xx and END_xx markers are used for automatic generation of
 * documentation. Do not change them.
 */


#include <linux/atmapi.h>
#include <linux/atmsap.h>
#include <linux/atmioc.h>
#include <linux/types.h>


/* general ATM constants */
#define ATM_CELL_SIZE		    53	/* ATM cell size incl. header */
#define ATM_CELL_PAYLOAD	    48	/* ATM payload size */
#define ATM_AAL0_SDU		    52	/* AAL0 SDU size */
#define ATM_MAX_AAL34_PDU	 65535	/* maximum AAL3/4 PDU payload */
#define ATM_AAL5_TRAILER	     8	/* AAL5 trailer size */
#define ATM_MAX_AAL5_PDU	 65535	/* maximum AAL5 PDU payload */
#define ATM_MAX_CDV		  9999	/* maximum (default) CDV */
#define ATM_NOT_RSV_VCI		    32	/* first non-reserved VCI value */

#define ATM_MAX_VPI		   255	/* maximum VPI at the UNI */
#define ATM_MAX_VPI_NNI		  4096	/* maximum VPI at the NNI */
#define ATM_MAX_VCI		 65535	/* maximum VCI */


/* "protcol" values for the socket system call */
#define ATM_NO_AAL	0		/* AAL not specified */
#define ATM_AAL0	13		/* "raw" ATM cells */
#define ATM_AAL1	1		/* AAL1 (CBR) */
#define ATM_AAL2	2		/* AAL2 (VBR) */
#define ATM_AAL34	3		/* AAL3/4 (data) */
#define ATM_AAL5	5		/* AAL5 (data) */

/*
 * socket option name coding functions
 *
 * Note that __SO_ENCODE and __SO_LEVEL are somewhat a hack since the
 * << 22 only reserves 9 bits for the level.  On some architectures
 * SOL_SOCKET is 0xFFFF, so that's a bit of a problem
 */

#define __SO_ENCODE(l,n,t)	((((l) & 0x1FF) << 22) | ((n) << 16) | \
				sizeof(t))
#define __SO_LEVEL_MATCH(c,m)	(((c) >> 22) == ((m) & 0x1FF))
#define __SO_NUMBER(c)		(((c) >> 16) & 0x3f)
#define __SO_SIZE(c)		((c) & 0x3fff)

/*
 * ATM layer
 */

#define SO_SETCLP	__SO_ENCODE(SOL_ATM,0,int)
			    /* set CLP bit value - TODO */
#define SO_CIRANGE	__SO_ENCODE(SOL_ATM,1,struct atm_cirange)
			    /* connection identifier range; socket must be
			       bound or connected */
#define SO_ATMQOS	__SO_ENCODE(SOL_ATM,2,struct atm_qos)
			    /* Quality of Service setting */
#define SO_ATMSAP	__SO_ENCODE(SOL_ATM,3,struct atm_sap)
			    /* Service Access Point */
#define SO_ATMPVC	__SO_ENCODE(SOL_ATM,4,struct sockaddr_atmpvc)
			    /* "PVC" address (also for SVCs); get only */
#define SO_MULTIPOINT	__SO_ENCODE(SOL_ATM, 5, int)
			    /* make this vc a p2mp */


/*
 * Note @@@: since the socket layers don't really distinguish the control and
 * the data plane but generally seems to be data plane-centric, any layer is
 * about equally wrong for the SAP. If you have a better idea about this,
 * please speak up ...
 */


/* ATM cell header (for AAL0) */

/* BEGIN_CH */
#define ATM_HDR_GFC_MASK	0xf0000000
#define ATM_HDR_GFC_SHIFT	28
#define ATM_HDR_VPI_MASK	0x0ff00000
#define ATM_HDR_VPI_SHIFT	20
#define ATM_HDR_VCI_MASK	0x000ffff0
#define ATM_HDR_VCI_SHIFT	4
#define ATM_HDR_PTI_MASK	0x0000000e
#define ATM_HDR_PTI_SHIFT	1
#define ATM_HDR_CLP		0x00000001
/* END_CH */


/* PTI codings */

/* BEGIN_PTI */
#define ATM_PTI_US0	0  /* user data cell, congestion not exp, SDU-type 0 */
#define ATM_PTI_US1	1  /* user data cell, congestion not exp, SDU-type 1 */
#define ATM_PTI_UCES0	2  /* user data cell, cong. experienced, SDU-type 0 */
#define ATM_PTI_UCES1	3  /* user data cell, cong. experienced, SDU-type 1 */
#define ATM_PTI_SEGF5	4  /* segment OAM F5 flow related cell */
#define ATM_PTI_E2EF5	5  /* end-to-end OAM F5 flow related cell */
#define ATM_PTI_RSV_RM	6  /* reserved for traffic control/resource mgmt */
#define ATM_PTI_RSV	7  /* reserved */
/* END_PTI */


/*
 * The following items should stay in linux/atm.h, which should be linked to
 * netatm/atm.h
 */

/* Traffic description */

#define ATM_NONE	0		/* no traffic */
#define ATM_UBR		1
#define ATM_CBR		2
#define ATM_VBR		3
#define ATM_ABR		4
#define ATM_ANYCLASS	5		/* compatible with everything */

#define ATM_MAX_PCR	-1		/* maximum available PCR */

struct atm_trafprm {
	unsigned char	traffic_class;	/* traffic class (ATM_UBR, ...) */
	int		max_pcr;	/* maximum PCR in cells per second */
	int		pcr;		/* desired PCR in cells per second */
	int		min_pcr;	/* minimum PCR in cells per second */
	int		max_cdv;	/* maximum CDV in microseconds */
	int		max_sdu;	/* maximum SDU in bytes */
        /* extra params for ABR */
        unsigned int 	icr;         	/* Initial Cell Rate (24-bit) */
        unsigned int	tbe;		/* Transient Buffer Exposure (24-bit) */ 
        unsigned int 	frtt : 24;	/* Fixed Round Trip Time (24-bit) */
        unsigned int 	rif  : 4;       /* Rate Increment Factor (4-bit) */
        unsigned int 	rdf  : 4;       /* Rate Decrease Factor (4-bit) */
        unsigned int nrm_pres  :1;      /* nrm present bit */
        unsigned int trm_pres  :1;     	/* rm present bit */
        unsigned int adtf_pres :1;     	/* adtf present bit */
        unsigned int cdf_pres  :1;    	/* cdf present bit*/
        unsigned int nrm       :3;     	/* Max # of Cells for each forward RM cell (3-bit) */
        unsigned int trm       :3;    	/* Time between forward RM cells (3-bit) */    
	unsigned int 