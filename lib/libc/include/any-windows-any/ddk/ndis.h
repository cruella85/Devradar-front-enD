/*
 * ndis.h
 *
 * Network Device Interface Specification definitions
 *
 * This file is part of the ReactOS DDK package.
 *
 * Contributors:
 *   Created by Casper S. Hornstrup <chorns@users.sourceforge.net>
 *
 * THIS SOFTWARE IS NOT COPYRIGHTED
 *
 * This source code is offered for use in the public domain. You may
 * use, modify or distribute it freely.
 *
 * This code is distributed in the hope that it will be useful but
 * WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 * DISCLAIMED. This includes but is not limited to warranties of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * DEFINES: i386                 - Target platform is i386
 *          NDIS_WRAPPER         - Define only for NDIS library
 *          NDIS_MINIPORT_DRIVER - Define only for NDIS miniport drivers
 *          NDIS40               - Use NDIS 4.0 structures by default
 *          NDIS50               - Use NDIS 5.0 structures by default
 *          NDIS51               - Use NDIS 5.1 structures by default
 *          NDIS40_MINIPORT      - Building NDIS 4.0 miniport driver
 *          NDIS50_MINIPORT      - Building NDIS 5.0 miniport driver
 *          NDIS51_MINIPORT      - Building NDIS 5.1 miniport driver
 */

#ifndef _NDIS_
#define _NDIS_

#ifndef NDIS_WDM
#define NDIS_WDM 0
#endif

#include "ntddk.h"
#include "netpnp.h"
#include "ntstatus.h"
#include "netevent.h"
#include <qos.h>

typedef int NDIS_STATUS, *PNDIS_STATUS;

#include "ntddndis.h"

#if !defined(_WINDEF_H)
typedef unsigned int UINT, *PUINT;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NET_PNP__
#define __NET_PNP__

typedef enum _NET_DEVICE_POWER_STATE {
  NetDeviceStateUnspecified = 0,
  NetDeviceStateD0,
  NetDeviceStateD1,
  NetDeviceStateD2,
  NetDeviceStateD3,
  NetDeviceStateMaximum
} NET_DEVICE_POWER_STATE, *PNET_DEVICE_POWER_STATE;

typedef enum _NET_PNP_EVENT_CODE {
  NetEventSetPower,
  NetEventQueryPower,
  NetEventQueryRemoveDevice,
  NetEventCancelRemoveDevice,
  NetEventReconfigure,
  NetEventBindList,
  NetEventBindsComplete,
  NetEventPnPCapabilities,
  NetEventPause,
  NetEventRestart,
  NetEventPortActivation,
  NetEventPortDeactivation,
  NetEventIMReEnableDevice,
  NetEventMaximum
} NET_PNP_EVENT_CODE, *PNET_PNP_EVENT_CODE;

typedef struct _NET_PNP_EVENT {
  NET_PNP_EVENT_CODE NetEvent;
  PVOID Buffer;
  ULONG BufferLength;
  ULONG_PTR NdisReserved[4];
  ULONG_PTR TransportReserved[4];
  ULONG_PTR TdiReserved[4];
  ULONG_PTR TdiClientReserved[4];
} NET_PNP_EVENT, *PNET_PNP_EVENT;

#endif /* __NET_PNP__ */

#if !defined(NDIS_WRAPPER)

#if (defined(NDIS_MINIPORT_MAJOR_VERSION) ||  \
    (defined(NDIS_MINIPORT_MINOR_VERSION)) || \
    (defined(NDIS_PROTOCOL_MAJOR_VERSION)) || \
    (defined(NDIS_PROTOCOL_MINOR_VERSION)) || \
    (defined(NDIS_FILTER_MAJOR_VERSION)) ||   \
    (defined(NDIS_FILTER_MINOR_VERSION)))
#error "Driver should not redefine NDIS reserved macros"
#endif

#if defined(NDIS_MINIPORT_DRIVER)

#if defined(NDIS620_MINIPORT)
#define NDIS_MINIPORT_MAJOR_VERSION 6
#define NDIS_MINIPORT_MINOR_VERSION 20
#elif defined(NDIS61_MINIPORT)
#define NDIS_MINIPORT_MAJOR_VERSION 6
#define NDIS_MINIPORT_MINOR_VERSION 1
#elif defined(NDIS60_MINIPORT)
#define NDIS_MINIPORT_MAJOR_VERSION 6
#define NDIS_MINIPORT_MINOR_VERSION 0
#elif defined(NDIS51_MINIPORT)
#define NDIS_MINIPORT_MAJOR_VERSION 5
#define NDIS_MINIPORT_MINOR_VERSION 1
#elif defined(NDIS50_MINIPORT)
#define NDIS_MINIPORT_MAJOR_VERSION 5
#define NDIS_MINIPORT_MINOR_VERSION 0
#else
#error "Only NDIS miniport drivers with version >= 5 are supported"
#endif

#if ((NDIS_MINIPORT_MAJOR_VERSION == 6) &&    \
       (NDIS_MINIPORT_MINOR_VERSION != 20) && \
       (NDIS_MINIPORT_MINOR_VERSION != 1) &&  \
       (NDIS_MINIPORT_MINOR_VERSION != 0))
#error "Invalid miniport major/minor version combination"
#elif ((NDIS_MINIPORT_MAJOR_VERSION == 5) &&  \
       (NDIS_MINIPORT_MINOR_VERSION != 1) &&  \
       (NDIS_MINIPORT_MINOR_VERSION != 0))
#error "Invalid miniport major/minor version combination"
#endif

#if  (NDIS_MINIPORT_MAJOR_VERSION == 6) && \
     ((NDIS_MINIPORT_MINOR_VERSION == 20 && NTDDI_VERSION < NTDDI_WIN7)  || \
      (NDIS_MINIPORT_MINOR_VERSION == 1 && NTDDI_VERSION < NTDDI_VISTA) || \
      (NDIS_MINIPORT_MINOR_VERSION == 0 && NTDDI_VERSION < NTDDI_VISTA))
#error "Wrong NDIS/DDI version"
#elif ((NDIS_MINIPORT_MAJOR_VERSION == 5) && \
       (((NDIS_MINIPORT_MINOR_VERSION == 1) && (NTDDI_VERSION < NTDDI_WINXP)) || \
         ((NDIS_MINIPORT_MINOR_VERSION == 0) && (NTDDI_VERSION < NTDDI_WIN2K))))
#error "Wrong NDIS/DDI version"
#endif


#endif /* defined(NDIS_MINIPORT_DRIVER) */

#if defined(NDIS30)
#error "Only NDIS Protocol drivers version 4 or later are supported"
#endif

#if defined(NDIS620)
#define NDIS_PROTOCOL_MAJOR_VERSION 6
#define NDIS_PROTOCOL_MINOR_VERSION 20
#define NDIS_FILTER_MAJOR_VERSION 6
#define NDIS_FILTER_MINOR_VERSION 20
#elif defined(NDIS61)
#define NDIS_PROTOCOL_MAJOR_VERSION 6
#define NDIS_PROTOCOL_MINOR_VERSION 1
#define NDIS_FILTER_MAJOR_VERSION 6
#define NDIS_FILTER_MINOR_VERSION 1
#elif defined(NDIS60)
#define NDIS_PROTOCOL_MAJOR_VERSION 6
#define NDIS_PROTOCOL_MINOR_VERSION 0
#define NDIS_FILTER_MAJOR_VERSION 6
#define NDIS_FILTER_MINOR_VERSION 0
#elif defined(NDIS51)
#define NDIS_PROTOCOL_MAJOR_VERSION 5
#define NDIS_PROTOCOL_MINOR_VERSION 1
#elif defined(NDIS50)
#define NDIS_PROTOCOL_MAJOR_VERSION 5
#define NDIS_PROTOCOL_MINOR_VERSION 0
#elif defined(NDIS40)
#define NDIS_PROTOCOL_MAJOR_VERSION 4
#define NDIS_PROTOCOL_MINOR_VERSION 0
#endif

#if !defined(NDIS_MINIPORT_DRIVER) && !defined(NDIS_PROTOCOL_MAJOR_VERSION)
#define NDIS40
#define NDIS_PROTOCOL_MAJOR_VERSION 4
#define NDIS_PROTOCOL_MINOR_VERSION 0
#endif

#if defined(NDIS_FILTER_MAJOR_VERSION)

#if ((NDIS_FILTER_MAJOR_VERSION == 6) &&  \
     (NDIS_FILTER_MINOR_VERSION != 20) && \
     (NDIS_FILTER_MINOR_VERSION != 1) &&  \
     (NDIS_FILTER_MINOR_VERSION != 0))
#error "Invalid Filter version"
#endif

#endif /* defined(NDIS_FILTER_MAJOR_VERSION) */


#if defined(NDIS_PROTOCOL_MAJOR_VERSION)

#if ((NDIS_PROTOCOL_MAJOR_VERSION == 6) &&  \
     (NDIS_PROTOCOL_MINOR_VERSION != 20) && \
     (NDIS_PROTOCOL_MINOR_VERSION != 1) &&  \
     (NDIS_PROTOCOL_MINOR_VERSION != 0))
#error "Invalid Protocol version"
#elif ((NDIS_PROTOCOL_MAJOR_VERSION == 5) && \
     (NDIS_PROTOCOL_MINOR_VERSION != 1) && (NDIS_PROTOCOL_MINOR_VERSION != 0))
#error "Invalid Protocol version"
#elif ((NDIS_PROTOCOL_MAJOR_VERSION == 4) && (NDIS_PROTOCOL_MINOR_VERSION != 0))
#error "Invalid Protocol major/minor version"
#endif

#if ((NDIS_PROTOCOL_MAJOR_VERSION == 6) && (NTDDI_VERSION < NTDDI_VISTA))
#error "Wrong NDIS/DDI version"
#endif

#endif /* defined(NDIS_PROTOCOL_MAJOR_VERSION) */

#endif /* !defined(NDIS_WRAPPER) */

#if !defined(NDIS_LEGACY_MINIPORT)

#if ((defined(NDIS_MINIPORT_DRIVER) && (NDIS_MINIPORT_MAJOR_VERSION < 6)) || NDIS_WRAPPER)
#define NDIS_LEGACY_MINIPORT    1
#else
#define NDIS_LEGACY_MINIPORT    0
#endif

#endif /* !defined(NDIS_LEGACY_MINIPORT) */

#if !defined(NDIS_LEGACY_PROTOCOL)

#if ((defined(NDIS_PROTOCOL_MAJOR_VERSION) && (NDIS_PROTOCOL_MAJOR_VERSION < 6)) || NDIS_WRAPPER)
#define NDIS_LEGACY_PROTOCOL    1
#else
#define NDIS_LEGACY_PROTOCOL    0
#endif

#endif /* !defined(NDIS_LEGACY_PROTOCOL) */

#if !defined(NDIS_LEGACY_DRIVER)

#if (NDIS_LEGACY_MINIPORT || NDIS_LEGACY_PROTOCOL || NDIS_WRAPPER)
#define NDIS_LEGACY_DRIVER      1
#else
#define NDIS_LEGACY_DRIVER      0
#endif

#endif /* !defined(NDIS_LEGACY_DRIVER) */

#if !defined(NDIS_SUPPORT_NDIS6)

#if ((defined (NDIS_MINIPORT_MAJOR_VERSION) && (NDIS_MINIPORT_MAJOR_VERSION >= 6)) || \
     (defined (NDIS60)) || NDIS_WRAPPER)
#define NDIS_SUPPORT_NDIS6      1
#else
#define NDIS_SUPPORT_NDIS6      0
#endif

#endif /* !defined(NDIS_SUPPORT_NDIS6) */

#if !defined(NDIS_SUPPORT_NDIS61)
#if  (((defined (NDIS_MINIPORT_MAJOR_VERSION) && (NDIS_MINIPORT_MAJOR_VERSION >= 6)) && \
       (defined (NDIS_MINIPORT_MINOR_VERSION) && (NDIS_MINIPORT_MINOR_VERSION >= 1))) || \
      (defined (NDIS61)) || NDIS_WRAPPER)
#define NDIS_SUPPORT_NDIS61      1
#else
#define NDIS_SUPPORT_NDIS61      0
#endif
#endif /* !defined(NDIS_SUPPORT_NDIS61) */

#if !defined(NDIS_SUPPORT_NDIS620)

#if  (((defined (NDIS_MINIPORT_MAJOR_VERSION) && (NDIS_MINIPORT_MAJOR_VERSION >= 6)) && \
       (defined (NDIS_MINIPORT_MINOR_VERSION) && (NDIS_MINIPORT_MINOR_VERSION >= 20))) || \
      (defined (NDIS620)) || NDIS_WRAPPER)
#define NDIS_SUPPORT_NDIS620      1
#else
#define NDIS_SUPPORT_NDIS620      0
#endif

#endif /* !defined(NDIS_SUPPORT_NDIS620) */

#if (NDIS_SUPPORT_NDIS620)
#undef NDIS_SUPPORT_NDIS61
#define NDIS_SUPPORT_NDIS61 1
#endif

#if (NDIS_SUPPORT_NDIS61)
#undef NDIS_SUPPORT_NDIS6
#define NDIS_SUPPORT_NDIS6 1
#endif

#if defined(NDIS61_MINIPORT) || defined(NDIS60_MINIPORT) || defined(NDIS61) || \
    defined(NDIS60) || defined(NDIS_WRAPPER) || defined(NDIS_LEGACY_DRIVER)
#define NDIS_SUPPORT_60_COMPATIBLE_API      1
#else
#define NDIS_SUPPORT_60_COMPATIBLE_API      0
#endif

#if defined(NDIS_WRAPPER)
#define NDISAPI
#else
#define NDISAPI DECLSPEC_IMPORT
#endif

typedef PVOID QUEUED_CLOSE; //FIXME : Doesn't exist in public headers

typedef struct _X_FILTER FDDI_FILTER, *PFDDI_FILTER;
typedef struct _X_FILTER TR_FILTER, *PTR_FILTER;
typedef struct _X_FILTER NULL_FILTER, *PNULL_FILTER;

typedef struct _NDIS_MINIPORT_BLOCK NDIS_MINIPORT_BLOCK, *PNDIS_MINIPORT_BLOCK;

typedef struct _REFERENCE {
  KSPIN_LOCK SpinLock;
  USHORT ReferenceCount;
  BOOLEAN Closing;
} REFERENCE, *PREFERENCE;

/* NDIS base types */

typedef struct _NDIS_SPIN_LOCK {
  KSPIN_LOCK SpinLock;
  KIRQL OldIrql;
} NDIS_SPIN_LOCK, *PNDIS_SPIN_LOCK;

typedef struct _NDIS_EVENT {
  KEVENT Event;
} NDIS_EVENT, *PNDIS_EVENT;

typedef PVOID NDIS_HANDLE, *PNDIS_HANDLE;

typedef ANSI_STRING NDIS_ANSI_STRING, *PNDIS_ANSI_STRING;
typedef UNICODE_STRING NDIS_STRING, *PNDIS_STRING;

typedef MDL NDIS_BUFFER, *PNDIS_BUFFER;

/* NDIS_STATUS constants */
#define NDIS_STATUS_SUCCESS                     ((NDIS_STATUS)STATUS_SUCCESS)
#define NDIS_STATUS_PENDING                     ((NDIS_STATUS)STATUS_PENDING)
#define NDIS_STATUS_NOT_RECOGNIZED              ((NDIS_STATUS)0x00010001L)
#define NDIS_STATUS_NOT_COPIED                  ((NDIS_STATUS)0x00010002L)
#define NDIS_STATUS_NOT_ACCEPTED                ((NDIS_STATUS)0x00010003L)
#define NDIS_STATUS_CALL_ACTIVE                 ((NDIS_STATUS)0x00010007L)
#define NDIS_STATUS_INDICATION_REQUIRED         ((NDIS_STATUS)STATUS_NDIS_INDICATION_REQUIRED)
#define NDIS_STATUS_ONLINE                      ((NDIS_STATUS)0x40010003L)
#define NDIS_STATUS_RESET_START                 ((NDIS_STATUS)0x40010004L)
#define NDIS_STATUS_RESET_END                   ((NDIS_STATUS)0x40010005L)
#define NDIS_STATUS_RING_STATUS                 ((NDIS_STATUS)0x40010006L)
#define NDIS_STATUS_CLOSED                      ((NDIS_STATUS)0x40010007L)
#define NDIS_STATUS_WAN_LINE_UP                 ((NDIS_STATUS)0x40010008L)
#define NDIS_STATUS_WAN_LINE_DOWN               ((NDIS_STATUS)0x40010009L)
#define NDIS_STATUS_WAN_FRAGMENT                ((NDIS_STATUS)0x4001000AL)
#define NDIS_STATUS_MEDIA_CONNECT               ((NDIS_STATUS)0x4001000BL)
#define NDIS_STATUS_MEDIA_DISCONNECT            ((NDIS_STATUS)0x4001000CL)
#define NDIS_STATUS_HARDWARE_LINE_UP            ((NDIS_STATUS)0x4001000DL)
#define NDIS_STATUS_HARDWARE_LINE_DOWN          ((NDIS_STATUS)0x4001000EL)
#define NDIS_STATUS_INTERFACE_UP                ((NDIS_STATUS)0x4001000FL)
#define NDIS_STATUS_INTERFACE_DOWN              ((NDIS_STATUS)0x40010010L)
#define NDIS_STATUS_MEDIA_BUSY                  ((NDIS_STATUS)0x40010011L)
#define NDIS_STATUS_MEDIA_SPECIFIC_INDICATION   ((NDIS_STATUS)0x40010012L)
#define NDIS_STATUS_WW_INDICATION               NDIS_STATUS_MEDIA_SPECIFIC_INDICATION
#define NDIS_STATUS_LINK_SPEED_CHANGE           ((NDIS_STATUS)0x40010013L)
#define NDIS_STATUS_WAN_GET_STATS               ((NDIS_STATUS)0x40010014L)
#define NDIS_STATUS_WAN_CO_FRAGMENT             ((NDIS_STATUS)0x40010015L)
#define NDIS_STATUS_WAN_CO_LINKPARAMS           ((NDIS_STATUS)0x40010016L)
#if NDIS_SUPPORT_NDIS6
#define NDIS_STATUS_LINK_STATE                  ((NDIS_STATUS)0x40010017L)
#define NDIS_STATUS_NETWORK_CHANGE              ((NDIS_STATUS)0x40010018L)
#define NDIS_STATUS_MEDIA_SPECIFIC_INDICATION_EX ((NDIS_STATUS)0x40010019L)
#define NDIS_STATUS_PORT_STATE                  ((NDIS_STATUS)0x40010022L)
#define NDIS_STATUS_OPER_STATUS                 ((NDIS_STATUS)0x40010023L)
#define NDIS_STATUS_PACKET_FILTER               ((NDIS_STATUS)0x40010024L)
#endif /* NDIS_SUPPORT_NDIS6 */
#define NDIS_STATUS_WAN_CO_MTULINKPARAMS        ((NDIS_STATUS)0x40010025L)

#if NDIS_SUPPORT_NDIS6

#define NDIS_STATUS_IP_OPER_STATUS              ((NDIS_STATUS)0x40010026L)

#define NDIS_STATUS_OFFLOAD_PAUSE               ((NDIS_STATUS)0x40020001L)
#define NDIS_STATUS_UPLOAD_ALL                  ((NDIS_STATUS)0x40020002L)
#define NDIS_STATUS_OFFLOAD_RESUME              ((NDIS_STATUS)0x40020003L)
#define NDIS_STATUS_OFFLOAD_PARTIAL_SUCCESS     ((NDIS_STATUS)0x40020004L)
#define NDIS_STATUS_OFFLOAD_STATE_INVALID       ((NDIS_STATUS)0x40020005L)
#define NDIS_STATUS_TASK_OFFLOAD_CURRENT_CONFIG ((NDIS_STATUS)0x40020006L)
#define NDIS_STATUS_TASK_OFFLOAD_HARDWARE_CAPABILITIES ((NDIS_STATUS)0x40020007L)
#define NDIS_STATUS_OFFLOAD_ENCASPULATION_CHANGE ((NDIS_STATUS)0x40020008L)
#define NDIS_STATUS_TCP_CONNECTION_OFFLOAD_HARDWARE_CAPABILITIES ((NDIS_STATUS)0x4002000BL)

#if (NDIS_SUPPORT_NDIS61)
#define NDIS_STATUS_HD_SPLIT_CURRENT_CONFIG     ((NDIS_STATUS)0x4002000CL)
#endif

#if (NDIS_SUPPORT_NDIS620)
#define NDIS_STATUS_RECEIVE_QUEUE_STATE         ((NDIS_STATUS)0x4002000DL)
#endif

#define NDIS_STATUS_OFFLOAD_IM_RESERVED1        ((NDIS_STATUS)0x40020100L)
#define NDIS_STATUS_OFFLOAD_IM_RESERVED2        ((NDIS_STATUS)0x40020101L)
#define NDIS_STATUS_OFFLOAD_IM_RESERVED3        ((NDIS_STATUS)0x40020102L)

#define NDIS_STATUS_DOT11_SCAN_CONFIRM          ((NDIS_STATUS)0x40030000L)
#define NDIS_STATUS_DOT11_MPDU_MAX_LENGTH_CHANGED ((NDIS_STATUS)0x40030001L)
#define NDIS_STATUS_DOT11_ASSOCIATION_START     ((NDIS_STATUS)0x40030002L)
#define NDIS_STATUS_DOT11_ASSOCIATION_COMPLETION ((NDIS_STATUS)0x40030003L)
#define NDIS_STATUS_DOT11_CONNECTION_START      ((NDIS_STATUS)0x40030004L)
#define NDIS_STATUS_DOT11_CONNECTION_COMPLETION ((NDIS_STATUS)0x40030005L)
#define NDIS_STATUS_DOT11_ROAMING_START         ((NDIS_STATUS)0x40030006L)
#define NDIS_STATUS_DOT11_ROAMING_COMPLETION    ((NDIS_STATUS)0x40030007L)
#define NDIS_STATUS_DOT11_DISASSOCIATION        ((NDIS_STATUS)0x40030008L)
#define NDIS_STATUS_DOT11_TKIPMIC_FAILURE       ((NDIS_STATUS)0x40030009L)
#define NDIS_STATUS_DOT11_PMKID_CANDIDATE_LIST  ((NDIS_STATUS)0x4003000AL)
#define NDIS_STATUS_DOT11_PHY_STATE_CHANGED     ((NDIS_STATUS)0x4003000BL)
#define NDIS_STATUS_DOT11_LINK_QUALITY          ((NDIS_STATUS)0x4003000CL)
#define NDIS_STATUS_DOT11_INCOMING_ASSOC_STARTED ((NDIS_STATUS)0x4003000DL)
#define NDIS_STATUS_DOT11_INCOMING_ASSOC_REQUEST_RECEIVED ((NDIS_STATUS)0x4003000EL)
#define NDIS_STATUS_DOT11_INCOMING_ASSOC_COMPLETION ((NDIS_STATUS)0x4003000FL)
#define NDIS_STATUS_DOT11_STOP_AP               ((NDIS_STATUS)0x40030010L)
#define NDIS_STATUS_DOT11_PHY_FREQUENCY_ADOPTED ((NDIS_STATUS)0x40030011L)
#define NDIS_STATUS_DOT11_CAN_SUSTAIN_AP        ((NDIS_STATUS)0x40030012L)

#define NDIS_STATUS_WWAN_DEVICE_CAPS            ((NDIS_STATUS)0x40041000)
#define NDIS_STATUS_WWAN_READY_INFO             ((NDIS_STATUS)0x40041001)
#define NDIS_STATUS_WWAN_RADIO_STATE            ((NDIS_STATUS)0x40041002)
#define NDIS_STATUS_WWAN_PIN_INFO               ((NDIS_STATUS)0x40041003)
#define NDIS_STATUS_WWAN_PIN_LIST               ((NDIS_STATUS)0