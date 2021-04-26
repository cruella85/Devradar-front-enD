/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifdef DEFINE_GUID

#ifndef FAR
#define FAR
#endif

DEFINE_GUID(ScsiRawInterfaceGuid,0x53f56309,0xb6bf,0x11d0,0x94,0xf2,0x00,0xa0,0xc9,0x1e,0xfb,0x8b);
DEFINE_GUID(WmiScsiAddressGuid,0x53f5630f,0xb6bf,0x11d0,0x94,0xf2,0x00,0xa0,0xc9,0x1e,0xfb,0x8b);
#endif /* DEFINE_GUID */

#ifndef _NTDDSCSIH_
#define _NTDDSCSIH_

#ifdef __cplusplus
extern "C" {
#endif

#define IOCTL_SCSI_BASE		FILE_DEVICE_CONTROLLER

#define DD_SCSI_DEVICE_NAME	"\\Device\\ScsiPort"
#define DD_SCSI_DEVICE_NAME_U  L"\\Device\\ScsiPort"

#define IOCTL_SCSI_PASS_THROUGH CTL_CODE(IOCTL_SCSI_BASE,0x0401,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SCSI_MINIPORT CTL_CODE(IOCTL_SCSI_BASE,0x0402,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SCSI_GET_INQUIRY_DATA CTL_CODE(IOCTL_SCSI_BASE,0x0403,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_SCSI_GET_CAPABILITIES CTL_CODE(IOCTL_SCSI_BASE,0x0404,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_SCSI_PASS_THROUGH_DIRECT CTL_CODE(IOCTL_SCSI_BASE,0x0405,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SCSI_GET_ADDRESS CTL_CODE(IOCTL_SCSI_BASE,0x0406,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_SCSI_RESCAN_BUS CTL_CODE(IOCTL_SCSI_BASE,0x0407,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_SCSI_GET_DUMP_POINTERS CTL_CODE(IOCTL_SCSI_BASE,0x0408,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_SCSI_FREE_DUMP_POINTERS CTL_CODE(IOCTL_SCSI_BASE,0x0409,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_IDE_PASS_THROUGH CTL_CODE(IOCTL_SCSI_BASE,0x040a,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ATA_PASS_THROUGH CTL_CODE(IOCTL_SCSI_BASE,0x040b,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ATA_PASS_THROUGH_DIRECT CTL_CODE(IOCTL_SCSI_BASE,0x040c,METHOD_BUFFERED,FILE_READ_ACCESS | FILE_WRITE_ACCESS)

  typedef struct _SCSI_PASS_THROUGH {
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    ULONG_PTR DataBufferOffset;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
  }SCSI_PASS_THROUGH,*PSCSI_PASS_THROUGH;

  typedef struct _SCSI_PASS_THROUGH_DIRECT {
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    PVOID DataBuffer;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
  }SCSI_PASS_THROUGH_DIRECT,*PSCSI_PASS_THROUGH_DIRECT;

#if defined(_WIN64)
  typedef struct _SCSI_PASS_THROUGH32 {
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    ULONG32 DataBufferOffset;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
  } SCSI_PASS_THROUGH32,*PSCSI_PASS_THROUGH32;

  typedef struct _SCSI_PASS_THROUGH_DIRECT32 {
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    VOID *DataBuffer;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
  } SCSI_PASS_THROUGH_DIRECT32,*PSCSI_PASS_THROUGH_DIRECT32;
#endif /* _WIN64 */

  typedef struct _ATA_PASS_THROUGH_EX {
    USHORT Length;
    USHORT AtaFlags;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR ReservedAsUchar;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    ULONG ReservedAsUlong;
    ULONG_PTR DataBufferOffset;
    UCHAR PreviousTaskFile[8];
    UCHAR CurrentTaskFile[8];
  } ATA_PASS_THROUGH_EX,*PATA_PASS_THROUGH_EX;

  typedef struct _ATA_PASS_THROUGH_DIRECT {
    USHORT Length;
    USHORT AtaFlags;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR ReservedAsUchar;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    ULONG ReservedAsUlong;
    PVOID DataBuffer;
    UCHAR PreviousTaskFile[8];
    UCHAR CurrentTaskFile[8];
  } ATA_PASS_THROUGH_DIRECT,*PATA_PASS_THROUGH_DIRECT;

#if defined(_WIN64)

  typedef struct _ATA_PASS_THROUGH_EX32 {
    USHORT Length;
    USHORT AtaFlags;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR ReservedAsUchar;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    ULONG ReservedAsUlong;
    ULONG32 DataBufferOffset;
    UCHAR PreviousTaskFile[8];
    UCHAR CurrentTaskFile[8];
  } ATA_PASS_THROUGH_EX32,*PATA_PASS_THROUGH_EX32;

  