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
#defi