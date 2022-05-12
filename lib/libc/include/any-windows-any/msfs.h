
/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _MSFS_H_
#define _MSFS_H_

#include <ras.h>
#include <mapitags.h>

#define PR_CFG_SERVER_PATH PROP_TAG (PT_STRING8,0x6600)
#define PR_CFG_MAILBOX PROP_TAG (PT_STRING8,0x6601)

#define PR_CFG_PASSWORD PROP_TAG (PT_STRING8,PROP_ID_SECURE_MIN)
#define PR_CFG_CONN_TYPE PROP_TAG (PT_LONG,0x6603)
#define CFG_CONN_LAN 0
#define CFG_CONN_REMOTE 1
#define CFG_CONN_OFFLINE 2
#define CFG_CONN_AUTO 3
#define PR_CFG_SESSION_LOG PROP_TAG (PT_BOOLEAN,0x6604)
#define PR_CFG_SESSION_LOG_FILE PROP_TAG (PT_STRING8,0x6605)
#define PR_CFG_REMEMBER PROP_TAG (PT_BOOLEAN,0x6606)

#define PR_CFG_ENABLE_UPLOAD PROP_TAG (PT_BOOLEAN,0x6620)
#define PR_CFG_ENABLE_DOWNLOAD PROP_TAG (PT_BOOLEAN,0x6621)
#define PR_CFG_UPLOADTO PROP_TAG (PT_LONG,0x6622)
#define CFG_UPLOADTO_PCMAIL 0x00000001
#define CFG_UPLOADTO_PROFS 0x00000002
#define CFG_UPLOADTO_SNADS 0x00000004
#define CFG_UPLOADTO_MCI 0x00000008
#define CFG_UPLOADTO_X400 0x00000010
#define CFG_UPLOADTO_FAX 0x00000040
#define CFG_UPLOADTO_MHS 0x00000080
#define CFG_UPLOADTO_SMTP 0x00000100
#define CFG_UPLOADTO_OV 0x00000800
#define CFG_UPLOADTO_MACMAIL 0x00001000
#define CFG_UPLOADTO_ALL CFG_UPLOADTO_PCMAIL | CFG_UPLOADTO_PROFS | CFG_UPLOADTO_SNADS | CFG_UPLOADTO_MCI | CFG_UPLOADTO_X400 | CFG_UPLOADTO_FAX | CFG_UPLOADTO_MHS | CFG_UPLOADTO_SMTP | CFG_UPLOADTO_OV | CFG_UPLOADTO_MACMAIL
#define PR_CFG_NETBIOS_NTFY PROP_TAG (PT_BOOLEAN,0x6623)
#define PR_CFG_SPOOLER_POLL PROP_TAG (PT_STRING8,0x6624)
#define PR_CFG_GAL_ONLY PROP_TAG (PT_BOOLEAN,0x6625)

#define PR_CFG_LAN_HEADERS PROP_TAG (PT_BOOLEAN,0x6630)
#define PR_CFG_LAN_LOCAL_AB PROP_TAG (PT_BOOLEAN,0x6631)
#define PR_CFG_LAN_EXTERNAL_DELIVERY PROP_TAG (PT_BOOLEAN,0x6632)

#define PR_CFG_RAS_EXTERNAL_DELIVERY PROP_TAG (PT_BOOLEAN,0x6639)
#define PR_CFG_RAS_HEADERS PROP_TAG (PT_BOOLEAN,0x6640)
#define PR_CFG_RAS_LOCAL_AB PROP_TAG (PT_BOOLEAN,0x6641)
#define PR_CFG_RAS_INIT_ON_START PROP_TAG (PT_BOOLEAN,0x6642)
#define PR_CFG_RAS_TERM_ON_HDRS PROP_TAG (PT_BOOLEAN,0x6643)
#define PR_CFG_RAS_TERM_ON_XFER PROP_TAG (PT_BOOLEAN,0x6644)
#define PR_CFG_RAS_TERM_ON_EXIT PROP_TAG (PT_BOOLEAN,0x6645)
#define PR_CFG_RAS_PROFILE PROP_TAG (PT_STRING8,0x6646)
#define PR_CFG_RAS_CONFIRM PROP_TAG (PT_LONG,0x6647)
#define CFG_ALWAYS 0
#define CFG_ASK_FIRST 1
#define CFG_ASK_EVERY 2
#define PR_CFG_RAS_RETRYATTEMPTS PROP_TAG (PT_STRING8,0x6648)
#define PR_CFG_RAS_RETRYDELAY PROP_TAG (PT_STRING8,0x6649)

#define PR_CFG_LOCAL_HEADER PROP_TAG (PT_BOOLEAN,0x6680)

#define CFG_SS_MAX 16
#define CFG_SS_BASE_ID 0x6700
#define CFG_SS_MAX_ID CFG_SS_BASE_ID + CFG_SS_MAX - 1
#define SchedPropTag(n) PROP_TAG (PT_BINARY,CFG_SS_BASE_ID+(n))
#define PR_CFG_SCHED_SESS SchedPropTag(0)

typedef struct SchedSess {
  USHORT sSessType;
  USHORT sDayMask;
  FILETIME ftTime;
  FILETIME ftStart;
  ULONG ulFlags;
  TCHAR szPhoneEntry[RAS_MaxEntryName+1];
} SCHEDSESS,*LPSCHEDSESS;

#define CFG_SS_SUN 0x0001
#define CFG_SS_MON 0x0002
#define CFG_SS_TUE 0x0004
#define CFG_SS_WED 0x0008
#define CFG_SS_THU 0x0010
#define CFG_SS_FRI 0x0020
#define CFG_SS_SAT 0x0040

#define IsDayChecked(sDayMask,nDay) ((sDayMask) & (1<<(nDay)))

#define CFG_SS_EVERY 0
#define CFG_SS_WEEKLY 1
#define CFG_SS_ONCE 2
#define CFG_SS_NULLTYPE 3

#define PR_CFG_MIN PROP_TAG (PT_STRING8,0x6600)
#define PR_CFG_MAX SchedPropTag(CFG_SS_MAX-1)

#define PR_ASSIGNED_ACCESS PROP_TAG(PT_LONG,0x66ff)
#define PR_OWNER_NAME PROP_TAG(PT_STRING8,0x66fe)

#define SFSP_ACCESS_OWNER 0x8000

#define MSFS_UID_ABPROVIDER { 0x00,0x60,0x94,0x64,0x60,0x41,0xb8,0x01,0x08,0x00,0x2b,0x2b,0x8a,0x29,0x00,0x00 }
#define MSFS_UID_SFPROVIDER { 0x00,0xff,0xb8,0x64,0x60,0x41,0xb8,0x01,0x08,0x00,0x2b,0x2b,0x8a,0x29,0x00,0x00 }
#endif /* End _MSFS_H_ */
