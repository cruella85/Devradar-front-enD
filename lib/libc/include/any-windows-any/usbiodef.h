
/*
 * usbiodef.h
 *
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER within this package.
 *
 * This file is based on the ReactOS PSDK file usbiodef.h header.
 * Original contributed by Casper S. Hornstrup <chorns@users.sourceforge.net>
 *
 * Added winapi-family check, Windows 8 additions by Kai Tietz.
 */

#ifndef __USBIODEF_H__
#define __USBIODEF_H__

#include <winapifamily.h>

#if WINAPI_FAMILY_PARTITION (WINAPI_PARTITION_DESKTOP)

#define USB_SUBMIT_URB 0
#define USB_RESET_PORT 1
#define USB_GET_ROOTHUB_PDO 3
#define USB_GET_PORT_STATUS 4
#define USB_ENABLE_PORT 5
#define USB_GET_HUB_COUNT 6
#define USB_CYCLE_PORT 7
#define USB_GET_HUB_NAME 8
#define USB_IDLE_NOTIFICATION 9
#define USB_RECORD_FAILURE 10

#define USB_GET_BUS_INFO 264
#define USB_GET_CONTROLLER_NAME 265
#define USB_GET_BUSGUID_INFO 266
#define USB_GET_PARENT_HUB_INFO 267
#define USB_GET_DEVICE_HANDLE 268
#define USB_GET_DEVICE_HANDLE_EX 269
#define USB_GET_TT_DEVICE_HANDLE 270
#define USB_GET_TOPOLOGY_ADDRESS 271
#define USB_IDLE_NOTIFICATION_EX 272
#define USB_REQ_GLOBAL_SUSPEND 273
#define USB_REQ_GLOBAL_RESUME 274
#define USB_GET_HUB_CONFIG_INFO 275

#define USB_REGISTER_COMPOSITE_DEVICE 0
#define USB_UNREGISTER_COMPOSITE_DEVICE 1
#define USB_REQUEST_REMOTE_WAKE_NOTIFICATION 2

#define HCD_GET_STATS_1 255
#define HCD_DIAGNOSTIC_MODE_ON 256
#define HCD_DIAGNOSTIC_MODE_OFF 257
#define HCD_GET_ROOT_HUB_NAME 258
#define HCD_GET_DRIVERKEY_NAME 265
#define HCD_GET_STATS_2 266
#define HCD_DISABLE_PORT 268
#define HCD_ENABLE_PORT 269
#define HCD_USER_REQUEST 270
#define HCD_TRACE_READ_REQUEST 275

#define USB_GET_NODE_INFORMATION 258
#define USB_GET_NODE_CONNECTION_INFORMATION 259
#define USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION 260
#define USB_GET_NODE_CONNECTION_NAME 261
#define USB_DIAG_IGNORE_HUBS_ON 262
#define USB_DIAG_IGNORE_HUBS_OFF 263
#define USB_GET_NODE_CONNECTION_DRIVERKEY_NAME 264
#define USB_GET_HUB_CAPABILITIES 271
#define USB_GET_NODE_CONNECTION_ATTRIBUTES 272
#define USB_HUB_CYCLE_PORT 273
#define USB_GET_NODE_CONNECTION_INFORMATION_EX 274
#define USB_RESET_HUB 275
#define USB_GET_HUB_CAPABILITIES_EX 276
#define USB_GET_HUB_INFORMATION_EX 277
#define USB_GET_PORT_CONNECTOR_PROPERTIES 278
#define USB_GET_NODE_CONNECTION_INFORMATION_EX_V2 279

#define GUID_CLASS_USBHUB GUID_DEVINTERFACE_USB_HUB
#define GUID_CLASS_USB_DEVICE GUID_DEVINTERFACE_USB_DEVICE
#define GUID_CLASS_USB_HOST_CONTROLLER GUID_DEVINTERFACE_USB_HOST_CONTROLLER

#define FILE_DEVICE_USB FILE_DEVICE_UNKNOWN
#define USB_CTL(id) CTL_CODE (FILE_DEVICE_USB,(id), METHOD_BUFFERED, FILE_ANY_ACCESS)
#define USB_KERNEL_CTL(id) CTL_CODE (FILE_DEVICE_USB,(id), METHOD_NEITHER, FILE_ANY_ACCESS)
#define USB_KERNEL_CTL_BUFFERED(id) CTL_CODE (FILE_DEVICE_USB,(id), METHOD_BUFFERED, FILE_ANY_ACCESS)

DEFINE_GUID (GUID_DEVINTERFACE_USB_HUB, 0xf18a0e88, 0xc30c, 0x11d0, 0x88, 0x15, 0x00, 0xa0, 0xc9, 0x06, 0xbe, 0xd8);
DEFINE_GUID (GUID_DEVINTERFACE_USB_DEVICE, 0xa5dcbf10, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed);
DEFINE_GUID (GUID_DEVINTERFACE_USB_HOST_CONTROLLER, 0x3abf6f2d, 0x71c4, 0x462a, 0x8a, 0x92, 0x1e, 0x68, 0x61, 0xe6, 0xaf, 0x27);
DEFINE_GUID (GUID_USB_WMI_STD_DATA, 0x4e623b20, 0xcb14, 0x11d1, 0xb3, 0x31, 0x00, 0xa0, 0xc9, 0x59, 0xbb, 0xd2);
DEFINE_GUID (GUID_USB_WMI_STD_NOTIFICATION, 0x4e623b20, 0xcb14, 0x11d1, 0xb3, 0x31, 0x00, 0xa0, 0xc9, 0x59, 0xbb, 0xd2);
#if _WIN32_WINNT >= 0x0600
DEFINE_GUID (GUID_USB_WMI_DEVICE_PERF_INFO, 0x66c1aa3c, 0x499f, 0x49a0, 0xa9, 0xa5, 0x61, 0xe2, 0x35, 0x9f, 0x64, 0x7);
DEFINE_GUID (GUID_USB_WMI_NODE_INFO, 0x9c179357, 0xdc7a, 0x4f41, 0xb6, 0x6b, 0x32, 0x3b, 0x9d, 0xdc, 0xb5, 0xb1);
DEFINE_GUID (GUID_USB_WMI_TRACING, 0x3a61881b, 0xb4e6, 0x4bf9, 0xae, 0xf, 0x3c, 0xd8, 0xf3, 0x94, 0xe5, 0x2f);
DEFINE_GUID (GUID_USB_TRANSFER_TRACING, 0x681eb8aa, 0x403d, 0x452c, 0x9f, 0x8a, 0xf0, 0x61, 0x6f, 0xac, 0x95, 0x40);
DEFINE_GUID (GUID_USB_PERFORMANCE_TRACING, 0xd5de77a6, 0x6ae9, 0x425c, 0xb1, 0xe2, 0xf5, 0x61, 0x5f, 0xd3, 0x48, 0xa9);
DEFINE_GUID (GUID_USB_WMI_SURPRISE_REMOVAL_NOTIFICATION, 0x9bbbf831, 0xa2f2, 0x43b4, 0x96, 0xd1, 0x86, 0x94, 0x4b, 0x59, 0x14, 0xb3);
#endif

typedef VOID (*USB_IDLE_CALLBACK) (PVOID Context);

typedef struct _USB_IDLE_CALLBACK_INFO {
  USB_IDLE_CALLBACK IdleCallback;
  PVOID IdleContext;
} USB_IDLE_CALLBACK_INFO,*PUSB_IDLE_CALLBACK_INFO;

#endif

#endif