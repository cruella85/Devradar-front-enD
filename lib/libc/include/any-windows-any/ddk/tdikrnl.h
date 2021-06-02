/*
 * tdikrnl.h
 *
 * TDI kernel mode definitions
 *
 * This file is part of the w32api package.
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
 */

#ifndef __TDIKRNL_H
#define __TDIKRNL_H

#include "tdi.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_TDI_)
#define TDIKRNLAPI
#else
#define TDIKRNLAPI DECLSPEC_IMPORT
#endif


typedef struct _TDI_REQUEST_KERNEL {
  ULONG  RequestFlags;
  PTDI_CONNECTION_INFORMATION  RequestConnectionInformation;
  PTDI_CONNECTION_INFORMATION  ReturnConnectionInformation;
  PVOID  RequestSpecific;
} TDI_REQUEST_KERNEL, *PTDI_REQUEST_KERNEL;

/* Request codes */
#define TDI_ASSOCIATE_ADDRESS             0x01
#define TDI_DISASSOCIATE_ADDRESS          0x02
#define TDI_CONNECT                       0x03
#define TDI_LISTEN                        0x04
#define TDI_ACCEPT                        0x05
#define TDI_DISCONNECT                    0x06
#define TDI_SEND                          0x07
#define TDI_RECEIVE                       0x08
#define TDI_SEND_DATAGRAM                 0x09
#define TDI_RECEIVE_DATAGRAM              0x0A
#define TDI_SET_EVENT_HANDLER             0x0B
#define TDI_QUERY_INFORMATION             0x0C
#define TDI_SET_INFORMATION               0x0D
#define TDI_ACTION                        0x0E

#define TDI_DIRECT_SEND                   0x27
#define TDI_DIRECT_SEND_DATAGRAM          0x29

#define TDI_TRANSPORT_ADDRESS_FILE        1
#define TDI_CONNECTION_FILE               2
#define TDI_CONTROL_CHANNEL_FILE          3

/* Internal TDI IOCTLS */
#define IOCTL_TDI_QUERY_DIRECT_SEND_HANDLER   _TDI_CONTROL_CODE(0x80, METHOD_NEITHER)
#define IOCTL_TDI_QUERY_DIRECT_SENDDG_HANDLER _TDI_CONTROL_CODE(0x81, METHOD_NEITHER)

/* TdiAssociateAddress */
typedef struct _TDI_REQUEST_KERNEL_ASSOCIATE {
  HANDLE  AddressHandle;
} TDI_REQUEST_KERNEL_ASSOCIATE, *PTDI_REQUEST_KERNEL_ASSOCIATE;

/* TdiDisassociateAddress */
typedef TDI_REQUEST_KERNEL TDI_REQUEST_KERNEL_DISASSOCIATE,
  *PTDI_REQUEST_KERNEL_DISASSOCIATE;

/* TdiAccept */
typedef struct _TDI_REQUEST_KERNEL_ACCEPT {
  PTDI_CONNECTION_INFORMATION  RequestConnectionInformation;
  PTDI_CONNECTION_INFORMATION  ReturnConnectionInformation;
} TDI_REQUEST_KERNEL_ACCEPT, *PTDI_REQUEST_KERNEL_ACCEPT;

/* TdiConnect */
typedef TDI_REQUEST_KERNEL TDI_REQUEST_KERNEL_CONNECT,
  *PTDI_REQUEST_KERNEL_CONNECT;

/* TdiDisconnect */
typedef TDI_REQUEST_KERNEL TDI_REQUEST_KERNEL_DISCONNECT,
  *PTDI_REQUEST_KERNEL_DISCONNECT;

/* TdiListen */
typedef TDI_REQUEST_KERNEL TDI_REQUEST_KERNEL_LISTEN,
  *PTDI_REQUEST_KERNEL_LISTEN;

/* TdiReceive */
typedef struct _TDI_REQUEST_KERNEL_RECEIVE {
  ULONG  ReceiveLength;
  ULONG  ReceiveFlags;
} TDI_REQUEST_KERNEL_RECEIVE, *PTDI_REQUEST_KERNEL_RECEIVE;

/* TdiReceiveDatagram */
typedef struct _TDI_REQUEST_KERNEL_RECEIVEDG {
  ULONG  ReceiveLength;
  PTDI_CONNECTION_INFORMATION  ReceiveDatagramInformation;
  PTDI_CONNECTION_INFORMATION  ReturnDatagramInformation;
  ULONG  ReceiveFlags;
} TDI_REQUEST_KERNEL_RECEIVEDG, *PTDI_REQUEST_KERNEL_RECEIVEDG;

/* TdiSend */
typedef struct _TDI_REQUEST_KERNEL_SEND {
  ULONG  SendLength;
  ULONG  SendFlags;
} TDI_REQUEST_KERNEL_SEND, *PTDI_REQUEST_KERNEL_SEND;

/* TdiSendDatagram */
typedef struct _TDI_REQUEST_KERNEL_SENDDG {
  ULONG  SendLength;
  PTDI_CONNECTION_INFORMATION  SendDatagramInformation;
} TDI_REQUEST_KERNEL_SENDDG, *PTDI_REQUEST_KERNEL_SENDDG;

/* TdiSetEventHandler */
typedef struct _TDI_REQUEST_KERNEL_SET_EVENT {
  LONG  EventType;
  PVOID  EventHandler;
  PVOID  EventContext;
} TDI_REQUEST_KERNEL_SET_EVENT, *PTDI_REQUEST_KERNEL_SET_EVENT;

/* TdiQueryInformation */
typedef struct _TDI_REQUEST_KERNEL_QUERY_INFO {
  LONG  QueryType;
  PTDI_CONNECTION_INFORMATION  RequestConnectionInformation;
} TDI_REQUEST_KERNEL_QUERY_INFORMATION, *PTDI_REQUEST_KERNEL_QUERY_INFORMATION;

/* TdiSetInformation */
typedef struct _TDI_REQUEST_KERNEL_SET_INFO {
  LONG  SetType;
  PTDI_CONNECTION_INFORMATION  RequestConnectionInformation;
} TDI_REQUEST_KERNEL_SET_INFORMATION, *PTDI_REQUEST_KERNEL_SET_INFORMATION;


/* Event types */
#define TDI_EVENT_CONNECT                   0
#define TDI_EVENT_DISCONNECT                1
#define TDI_EVENT_ERROR                     2
#define TDI_EVENT_RECEIVE                   3
#define TDI_EVENT_RECEIVE_DATAGRAM          4
#define TDI_EVENT_RECEIVE_EXPEDITED         5
#define TDI_EVENT_SEND_POSSIBLE             6
#define TDI_EVENT_CHAINED_RECEIVE           7
#define TDI_EVENT_CHAINED_RECEIVE_DATAGRAM  8
#define TDI_EVENT_CHAINED_RECEIVE_EXPEDITED 9
#define TDI_EVENT_ERROR_EX                  10

typedef NTSTATUS
(NTAPI *PTDI_IND_CONNECT)(
  IN PVOID  TdiEventContext,
  IN LONG  RemoteAddressLength,
  IN PVOID  RemoteAddress,
  IN LONG  UserDataLength,
  IN PVOID  UserData,
  IN LONG  OptionsLength,
  IN PVOID  Options,
  OUT CONNECTION_CONTEXT  *ConnectionContext,
  OUT PIRP  *AcceptIrp);

TDIKRNLAPI
NTSTATUS
NTAPI
TdiDefaultConnectHandler(
  IN PVOID  TdiEventContext,
  IN LONG  RemoteAddressLength,
  IN PVOID  RemoteAddress,
  IN LONG  UserDataLength,
  IN PVOID  UserData,
  IN LONG  OptionsLength,
  IN PVOID  Options,
  OUT CONNECTION_CONTEXT *ConnectionContext,
  OUT PIRP  *AcceptIrp);

typedef NTSTATUS
(NTAPI *PTDI_IND_DISCONNECT)(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN LONG  DisconnectDataLength,
  IN PVOID  DisconnectData,
  IN LONG  DisconnectInformationLength,
  IN PVOID  DisconnectInformation,
  IN ULONG  DisconnectFlags);

TDIKRNLAPI
NTSTATUS
NTAPI
TdiDefaultDisconnectHandler(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN LONG  DisconnectDataLength,
  IN PVOID  DisconnectData,
  IN LONG  DisconnectInformationLength,
  IN PVOID  DisconnectInformation,
  IN ULONG  DisconnectFlags);

typedef NTSTATUS
(NTAPI *PTDI_IND_ERROR)(
  IN PVOID  TdiEventContext,
  IN NTSTATUS  Status);

typedef NTSTATUS
(NTAPI *PTDI_IND_ERROR_EX)(
  IN PVOID  TdiEventContext,
  IN NTSTATUS  Status,
  IN PVOID  Buffer);

TDIKRNLAPI
NTSTATUS
NTAPI
TdiDefaultErrorHandler(
  IN PVOID  TdiEventContext,
  IN NTSTATUS  Status);

typedef NTSTATUS
(NTAPI *PTDI_IND_RECEIVE)(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN ULONG  ReceiveFlags,
  IN ULONG  BytesIndicated,
  IN ULONG  BytesAvailable,
  OUT ULONG  *BytesTaken,
  IN PVOID  Tsdu,
  OUT PIRP  *IoRequestPacket);

TDIKRNLAPI
NTSTATUS
NTAPI
TdiDefaultReceiveHandler(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN ULONG  ReceiveFlags,
  IN ULONG  BytesIndicated,
  IN ULONG  BytesAvailable,
  OUT ULONG  *BytesTaken,
  IN PVOID  Tsdu,
  OUT PIRP  *IoRequestPacket);

typedef NTSTATUS
(NTAPI *PTDI_IND_RECEIVE_DATAGRAM)(
  IN PVOID  TdiEventContext,
  IN LONG  SourceAddressLength,
  IN PVOID  SourceAddress,
  IN LONG  OptionsLength,
  IN PVOID  Options,
  IN ULONG  ReceiveDatagramFlags,
  IN ULONG  BytesIndicated,
  IN ULONG  BytesAvailable,
  OUT ULONG  *BytesTaken,
  IN PVOID  Tsdu,
  OUT PIRP  *IoRequestPacket);

TDIKRNLAPI
NTSTATUS NTAPI
TdiDefaultRcvDatagramHandler(
  IN PVOID  TdiEventContext,
  IN LONG  SourceAddressLength,
  IN PVOID  SourceAddress,
  IN LONG  OptionsLength,
  IN PVOID  Options,
  IN ULONG  ReceiveDatagramFlags,
  IN ULONG  BytesIndicated,
  IN ULONG  BytesAvailable,
  OUT ULONG  *BytesTaken,
  IN PVOID  Tsdu,
  OUT PIRP  *IoRequestPacket);

typedef NTSTATUS
(NTAPI *PTDI_IND_RECEIVE_EXPEDITED)(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN ULONG  ReceiveFlags,
  IN ULONG  BytesIndicated,
  IN ULONG  BytesAvailable,
  OUT ULONG  *BytesTaken,
  IN PVOID  Tsdu,
  OUT PIRP  *IoRequestPacket);

TDIKRNLAPI
NTSTATUS
NTAPI
TdiDefaultRcvExpeditedHandler(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN ULONG  ReceiveFlags,
  IN ULONG  BytesIndicated,
  IN ULONG  BytesAvailable,
  OUT ULONG  *BytesTaken,
  IN PVOID  Tsdu,
  OUT PIRP  *IoRequestPacket);

typedef NTSTATUS
(NTAPI *PTDI_IND_CHAINED_RECEIVE)(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN ULONG  ReceiveFlags,
  IN ULONG  ReceiveLength,
  IN ULONG  StartingOffset,
  IN PMDL  Tsdu,
  IN PVOID  TsduDescriptor);

TDIKRNLAPI
NTSTATUS
NTAPI
TdiDefaultChainedReceiveHandler(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN ULONG  ReceiveFlags,
  IN ULONG  ReceiveLength,
  IN ULONG  StartingOffset,
  IN PMDL  Tsdu,
  IN PVOID  TsduDescriptor);

typedef NTSTATUS
(NTAPI *PTDI_IND_CHAINED_RECEIVE_DATAGRAM)(
  IN PVOID  TdiEventContext,
  IN LONG  SourceAddressLength,
  IN PVOID  SourceAddress,
  IN LONG  OptionsLength,
  IN PVOID  Options,
  IN ULONG  ReceiveDatagramFlags,
  IN ULONG  ReceiveDatagramLength,
  IN ULONG  StartingOffset,
  IN PMDL  Tsdu,
  IN PVOID  TsduDescriptor);

TDIKRNLAPI
NTSTATUS
NTAPI
TdiDefaultChainedRcvDatagramHandler(
  IN PVOID  TdiEventContext,
  IN LONG  SourceAddressLength,
  IN PVOID  SourceAddress,
  IN LONG  OptionsLength,
  IN PVOID  Options,
  IN ULONG  ReceiveDatagramFlags,
  IN ULONG  ReceiveDatagramLength,
  IN ULONG  StartingOffset,
  IN PMDL  Tsdu,
  IN PVOID  TsduDescriptor);

typedef NTSTATUS
(NTAPI *PTDI_IND_CHAINED_RECEIVE_EXPEDITED)(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN ULONG  ReceiveFlags,
  IN ULONG  ReceiveLength,
  IN ULONG  StartingOffset,
  IN PMDL  Tsdu,
  IN PVOID  TsduDescriptor);

TDIKRNLAPI
NTSTATUS
NTAPI
TdiDefaultChainedRcvExpeditedHandler(
  IN PVOID  TdiEventContext,
  IN CONNECTION_CONTEXT  ConnectionContext,
  IN ULONG  ReceiveFlags,
  IN ULONG  ReceiveLength,
  IN ULONG  StartingOffset,
  IN PMDL  Tsdu,
  IN PVOID  TsduDescriptor);

typedef NTSTATUS
(NTAPI *PTDI_IND_SEND_POSSIBLE)(
  IN PVOID  TdiEventContext,
  IN PVOID  ConnectionContext,
  IN ULONG  BytesAvailable);

TDIKRNLAPI
NTSTATUS
NTAPI
TdiDefaultSendPossibleHandler(
  IN PVOID  TdiEventContext,
  IN PVOID  ConnectionContext,
  IN ULONG  BytesAvailable);



/* Macros and functions to build IRPs */

#define TdiBuildBaseIrp(                                                  \
  bIrp, bDevObj, bFileObj, bCompRoutine, bContxt, bIrpSp, bMinor)         \
{                                                                         \
  bIrpSp->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;                 \
  bIrpSp->MinorFunction = (bMinor);                                       \
  bIrpSp->DeviceObject  = (bDevObj);                                      \
  bIrpSp->FileObject    = (bFileObj);                                     \
  if (bCompRoutine)                                                       \
  {                                                                       \
    IoSetCompletionRoutine(bIrp, bCompRoutine, bContxt, TRUE, TRUE, TRUE);\
  }                                                                       \
  else                                                                    \
    IoSetCompletionRoutine(bIrp, NULL, NULL, FALSE, FALSE, FALSE);        \
}

/*
 * VOID
 * TdiBuildAccept(
 *   IN PIRP  Irp,
 *   IN PDEVICE_OBJECT  DevObj,
 *   IN PFILE_OBJECT  FileObj,
 *   IN PVOID  CompRoutine,
 *   IN PVOID  Contxt,
 *   IN PTDI_CONNECTION_INFORMATION  RequestConnectionInfo,
 *   OUT PTDI_CONNECTION_INFORMATION  ReturnConnectionInfo);
 */
#define TdiBuildAccept(                                             \
  Irp, DevObj, FileObj, CompRoutine, Contxt,                        \
  RequestConnectionInfo, ReturnConnectionInfo)                      \
{                                                                   \
  PTDI_REQUEST_KERNEL_ACCEPT _Request;                              \
  PIO_STACK_LOCATION _IrpSp;                                        \
                                                                    \
  _IrpSp = IoGetNextIrpStackLocation(Irp);                          \
                                                                    \
  TdiBuildBaseIrp(Irp, DevObj, FileObj, CompRoutine,                \
                  Contxt, _IrpSp, TDI_ACCEPT);                      \
                                                                    \
  _Request = (PTDI_REQUEST_KERNEL_ACCEPT)&_IrpSp->Parameters;       \
  _Request->RequestConnectionInformation = (RequestConnectionInfo); \
  _Request->ReturnConnectionInformation  = (ReturnConnectionInfo);  \
}

/*
 * VOID
 * TdiBuildAction(
 *   IN PIRP  Irp,
 *   IN PDEVICE_OBJECT  DevObj,
 *   IN PFILE_OBJECT  FileObj,
 *   IN PVOID  CompRoutine,
 *   IN PVOID  Contxt,
 *   IN PMDL  MdlAddr);
 */
#define TdiBuildAction(                               \
  Irp, DevObj, FileObj, CompRoutine, Contxt, MdlAddr) \
{                                                     \
  PIO_STACK_LOCATION _IrpSp;                          \
                                                      \
  _IrpSp = IoGetNextIrpStackLocation(Irp);            \
                                                      \
  TdiBuildBaseIrp(Irp, DevObj, FileObj, CompRoutine,  \
                  Contxt, _IrpSp, TDI_ACTION);        \
                                                      \
  (Irp)->MdlAddress = (MdlAddr);                      \
}

/*
 * VOID
 * TdiBuildAssociateAddress(
 *   IN PIRP  Irp,
 *   IN PDEVICE_OBJECT  DevObj,
 *   IN PFILE_OBJECT  FileObj,
 *   IN PVOID  CompRoutine,
 *   IN PVOID  Contxt,
 *   IN HANDLE  AddrHandle);
 */
#define TdiBuildAssociateAddress(                                \
  Irp, DevObj, FileObj, CompRoutine, Contxt, AddrHandle)         \
{                                                                \
  PTDI_REQUEST_KERNEL_ASSOCIATE _Request;                        \
  PIO_STACK_LOCATION _IrpSp;                                     \
                                                                 \
  _IrpSp = IoGetNextIrpStackLocation(Irp);                       \
                                                                 \
  TdiBuildBaseIrp(Irp, DevObj, FileObj, CompRoutine,             \
                  Contxt, _IrpSp, TDI_ASSOCIATE_ADDRESS);        \
                                                                 \
  _Request = (PTDI_REQUEST_KERNEL_ASSOCIATE)&_IrpSp->Parameters; \
  _Request->AddressHandle = (HANDLE)(AddrHandle);                \
}

/*
 * VOID
 * TdiBuildConnect(
 *   IN PIRP  Irp,
 *   IN PDEVICE_OBJECT  DevObj,
 *   IN PFILE_OBJECT  FileObj,
 *   IN PVOID  CompRoutine,
 *   IN PVOID  Contxt,
 *   IN PLARGE_INTEGER  Time,
 *   IN PTDI_CONNECTION_INFORMATION  RequestConnectionInfo,
 *   OUT PTDI_CONNECTION_INFORMATION  ReturnConnectionInfo);
 */
#define TdiBuildConnect(                                            \
  Irp, DevObj, FileObj, CompRoutine, Contxt,                        \
  Time, RequestConnectionInfo, ReturnConnectionInfo)                \
{                                                                   \
  PTDI_REQUEST_KERNEL _Request;                                     \
  PIO_STACK_LOCATION _IrpSp;                                        \
                                                                    \
  _IrpSp = IoGetNextIrpStackLocation(Irp);                          \
                                                                    \
  TdiBuildBaseIrp(Irp, DevObj, FileObj, CompRoutine,                \
                  Contxt, _IrpSp, TDI_CONNECT);                     \
                                                                    \
  _Request = (PTDI_REQUEST_KERNEL)&_IrpSp->Parameters;              \
  _Request->RequestConnectionInformation = (RequestConnectionInfo); \
  _Request->ReturnConnectionInformation  = (ReturnConnectionInfo);  \
  _Request->RequestSpecific              = (PVOID)(Time);           \
}

/*
 * VOID
 * TdiBuildDisassociateAddress(
 *   IN PIRP  Irp,
 *   IN PDEVICE_OBJECT  DevObj,
 *   IN PFILE_OBJECT  FileObj,
 *   IN PVOID  CompRoutine,
 *   IN PVOID  Contxt);
 */
#define TdiBuildDisassociateAddress(                                \
  Irp, DevObj, FileObj, CompRoutine, Contxt)                        \
{                                    