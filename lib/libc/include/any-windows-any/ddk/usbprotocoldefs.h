#pragma once

#include <pshpack1.h>

#define USB_UnConnected_Device_Address		0
#define USB_UNCONNECTED_ADDRESS(address)	(USB_UnConnected_Device_Address == (address))
#define USB_CONNECTED_ADDRESS(address)		(USB_UnConnected_Device_Address != (address))

#define PID_OUT					1
#define PID_IN					9
#define PID_SOF					5
#define PID_SETUP				13

#define PID_DATA0				3
#define PID_DATA1				11
#define PID_DATA2				7
#define PID_MDATA				15

#define USB_ACK					2
#define USB_NAK					10
#define USB_STALL				14
#define USB_NYET				6

#define USB_PRE					12
#define USB_ERR					12
#define USB_SPLIT				8
#define USB_PING				4

#define USB_TIMEOUT				0

#define USB_SPEC				0x0200
#define HID_SPEC				0x0101

#define USB_20_SPEC				0x0200
#define USB_11_SPEC				0x0110
#define USB_10_SPEC				0x0100

#define HID_MAX_PACKET_SIZE0			0x08
#define MICROSOFT_VENDOR_ID			0x045E
#define HID_DEVICE_RELEASE			0x0100

#define HID_MAX_PACKET_SIZE			0x0008
#define HID_POLLING_INTERVAL			0x0A
#define MAX_POLLING_INTERVAL			0xFF

#define USB_DEFAULT_KEYBOARD_PRODUCT_ID		0x000B
#define USB_DEFAULT_MOUSE_PRODUCT_ID		0x0040

#define DEVICE_DESCRIPTOR			0x01
#define CONFIGURATION_DESCRIPTOR		0x02
#define STRING_DESCRIPTOR			0x03
#define INTERFACE_DESCRIPTOR			0x04
#define ENDPOINT_DESCRIPTOR			0x05
#define QUALIFIER_DESCRIPTOR			0x06
#define OTHER_SPEED_DESCRIPTOR			0x07
#define INTERFACE_POWER_DESCRIPTOR		0x08

#define HID_DESCRIPTOR				0x21
#define REPORT_DESCRIPTOR			0x22
#define PHYSICAL_DESCRIPTOR			0x23
#define HUB_DESCRIPTOR				0x29

#define USB_DESCRIPTOR_TYPE_STD			0
#define USB_DESCRIPTOR_TYPE_CLASS		1
#define USB_DESCRIPTOR_TYPE_VENDOR		2
#define USB_DESCRIPTOR_TYPE_RESERVED		3

#define DIR_HOST_TO_DEVICE			0
#define DIR_DEVICE_TO_HOST			1

#define TYPE_STANDARD				0
#define TYPE_CLASS				1
#define TYPE_VENDOR				2
#define TYPE_RESERVED				3

#define RCPT_DEVICE				0
#define RCPT_INTERFACE				1
#define RCPT_ENDPOINT				2
#define RCPT_OTHER				3
#define RCPT_PORT				4
#define RCPT_RPIPE				5

#if !defined(MIDL_PASS)
#define USB_MAKE_REQUEST_TYPE(direction, type, recipient)		\
		(BYTE)( ((BYTE)direction << 7) |			\
			((BYTE)type << 5) | ((BYTE)recipient & 0x07) )
#endif

#define GET_STATUS				0
#define CLEAR_FEATURE				1
#define SET_FEATURE				3
#define SET_ADDRESS				5
#define GET_DESCRIPTOR				6
#define SET_DESCRIPTOR				7
#define GET_CONFIGURATION			8
#define SET_CONFIGURATION			9
#define GET_INTERFACE				10
#define SET_INTERFACE				11
#define SYNCH_FRAME				12

#define USB_BULK_ONLY_MASS_STG_RESET		0xFF
#define USB_BULK_ONLY_MASS_STG_GET_MAX_LUN	0xFE

#define GET_REPORT				0x01
#define GET_IDLE				0x02
#define GET_PROTOCOL				0x03
#define SET_REPORT				0x09
#define SET_IDLE				0x0A
#define SET_PROTOCOL				0x0B

#define ADD_MMC_IE				20
#define REMOVE_MMC_IE				21
#define SET_NUM_DNTS				22
#define SET_CLUSTER_ID				23
#define SET_DEVICE_INFO				24
#define GET_TIME				25
#define SET_STREAM_INDEX			26
#define SET_WUSB_MAS				27
#define WUSB_CH_STOP				28

#define EXEC_RC_CMD				40

#define TIME_ADJ				0x01
#define TIME_BPST				0x02
#define TIME_WUSB				0x03

#define HID_REPORT_TYPE_INPUT			0x01
#define HID_REPORT_TYPE_OUTPUT			0x02
#define HID_REPORT_TYPE_FEATURE			0x03

#define HID_PROTOCOL_TYPE_BOOT			0x00
#define HID_PROTOCOL_TYPE_REPORT		0x01

#define HUB_DEVICE_PROTOCOL_1X			0
#define HUB_DEVICE_PROTOCOL_SINGLE_TT		1
#define HUB_DEVICE_PROTOCOL_MULTI_TT		2

#define HUB_INTERFACE_PROTOCOL_1X				0
#define HUB_INTERFACE_PROTOCOL_SINGLE_TT			0
#define HUB_INTERFACE_PROTOCOL_MULTI_TT_IN_SINGLE_TT_MODE	1
#define HUB_INTERFACE_PROTOCOL_MULTI_TT_IN_MULTI_TT_MODE	2

#define CLEAR_TT_BUFFER				8
#define RESET_TT				9
#define GET_TT_STATE				10
#define STOP_TT					11

#define C_HUB_LOCAL_POWER			0
#define C_HUB_OVER_CURRENT			1
#define PORT_CONNECTION				0
#define PORT_ENABLE				1
#define PORT_SUSPEND				2
#define PORT_OVER_CURRENT			3
#define PORT_RESET				4
#define PORT_POWER				8
#define PORT_LOW_SPEED				9
#define C_PORT_CONNECTION			16
#define C_PORT_ENABLE				17
#define C_PORT_SUSPEND				18
#define C_PORT_OVER_CURRENT			19
#define C_PORT_RESET				20
#define PORT_TEST				21
#define PORT_INDICATOR				22

#define USBSETUPSIZE				8
#define USBINREQUEST				128

#define BM_GET_DEVICE				128
#define BM_GET_INTERFACE			129
#define BM_GET_ENDPOINT				130

#define BM_SET_DEVICE				0
#define BM_SET_INTERFACE			1
#define BM_SET_ENDPOINT				2

#define HALT_ENDPOINT				0
#define REMOTE_WAKEUP				1
#define TEST_MODE				2

#define DEVICE_DESCRIPTION_TYPE			0x100
#define QUALIFIER_DESCRIPTION_TYPE		0x600
#define OTHER_SPEED_DESCRIPTION_TYPE		0x700
#define CONFIG_DESCRIPTION_TYPE			0x200
#define STRING_DESCRIPTION_TYPE			0x300
#define MSOS_DESCRIPTION_TYPE			0x3EE

#define CONFIG_BUS_POWERED			0x80
#define CONFIG_SELF_POWERED			0x40
#define CONFIG_REMOTE_WAKEUP			0x20

#define USB_WA_MULTIFUNCTION			0x02
#define USB_WA_PROTOCOL				0x01
#define USB_RADIO_CONTROL			0x2

#define USB_HID_CLASS_CODE			0x03
#define USB_MASS_STORAGE_CLASS_CODE		0x08
#define USB_HUB_CLASS_CODE			0x09
#define USB_MISCELLANEOUS			0xEF
#define USB_WIRELESS_WA				0xE0

#define BOOT_INTERFACE_SUBCLASS			0x01
#define COMMON_CLASS				0x02
#define USB_RF_CONTROL				0x01

#define PROTOCOL_NONE				0x00
#define PROTOCOL_KEYBOARD			0x01
#define PROTOCOL_MOUSE				0x02

#define EP_OUT					0
#define EP_IN					1

#define MAKE_ENDPOINT_ADDRESS(num, dir)				\
		( ((BYTE)(dir) << 7) | ((BYTE)(num) & 0x0F) )

#define ENDPOINT_TYPE				0x03
#define CONTROL_ENDPOINT			0
#define ISOCHRONOUS_ENDPOINT			1
#define BULK_ENDPOINT				2
#define INTERRUPT_ENDPOINT			3

typedef union _USBDESCRIPTORTYPE {
  BYTE Byte;
#if !defined(MIDL_PASS)
  struct Bits {
    BYTE Descriptor:5;
    BYTE Type:2;
    BYTE Reserved:1;
  } Bits;
#endif
} USBDESCRIPTORTYPE;

typedef union _USBCONFIGATTRIBS {
  BYTE Byte;
#if !defined(MIDL_PASS)
  struct Bits {
    BYTE bReserved0_4:5;
    BYTE bRemoteWakeup:1;
    BYTE bSelfPowered:1;
    BYTE bReserved7:1;
  } Bits;
#endif
} USBCONFIGATTRIBS;

typedef union _USBREQUESTTYPE {
  BYTE Byte;
#if !defined(MIDL_PASS)
  struct Bits {
    BYTE Recipient:5;
    BYTE Type:2;
    BYTE Direction:1;
    } Bits;
#endif
} USBREQUESTTYPE;

#if !defined(MIDL_PASS)
C_ASSERT((sizeof(USBREQUESTTYPE) == sizeof(BYTE)));
#endif

typedef struct _USBSETUPREQUEST {
  USBREQUESTTYPE bmRequestType;
  BYTE bRequest;
  SHORT sSetupValue;
  SHORT sSetupIndex;
  SHORT sSetupLength;
} USBSETUPREQUEST;

#if !defined(MIDL_PASS)

typedef struct _USBDEVICEDESC {
  BYTE bLength;
  BYTE bDescriptorType;
  USHORT usUSB;
  BYTE bDeviceClass;
  BYTE bDeviceSubClass;
  BYTE bProtocol;
  BYTE bMaxPacket0;
  USHORT usVendor;
  USHORT usProduct;
  USHORT usDeviceNumber;
  BYTE bManufacturer;
  BYTE bProductDesc;
  BYTE bSerialNumber;
  BYTE bNumConfigs;
} USBDEVICEDESC;

typedef struct _USBCONFIGDESC {
  BYTE bLength;
  BYTE bDescriptorType;
  USHORT usTotalLength;
  BYTE bNumInterfaces;
  BYTE bConfigValue;
  BYTE bConfig;
  BYTE bAttributes;
  BYTE bMaxPower;
} USBCONFIGDESC;


typedef struct _USBINTERFACEDESC {
  BYTE bLength;
  BYTE bDescriptorType;
  BYTE bInterfaceNumber;
  BYTE bAlternateSetting;
  BYTE bNumEndpoints;
  BYTE bClass;
  BYTE bSubClass;
  BYTE bProtocol;
  BYTE bDescription;
} USBINTERFACEDESC;

#define ENDPOINT_DIRECTION_OUT			0
#define ENDPOINT_DIRECTION_IN			1

typedef union _USBENDPOINTADDRESS {
  BYTE Byte;
  struct Bits {
    BYTE Number:4;
    BYTE Reserved:3;
    BYTE Direction:1;
  } Bits;
} USBENDPOINTADDRESS;

C_ASSERT((sizeof(USBENDPOINTADDRESS) == sizeof(BYTE)));

#define USB_TRANSFER_TYPE_CONTROL		0
#define USB_TRANSFER_TYPE_ISOCH			1
#define USB_TRANSFER_TYPE_BULK			2
#define USB_TRANSFER_TYPE_INTERRUPT		3

#define USB_SYNC_TYPE_NONE			0
#define USB_SYNC_TYPE_ASYNC			1
#define USB_SYNC_TYPE_ADAPTIVE			2
#define USB_SYNC_TYPE