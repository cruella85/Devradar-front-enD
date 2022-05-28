
/*
 * wdm.h
 *
 * Windows NT WDM Driver Developer Kit
 *
 * This file is part of the ReactOS DDK package.
 *
 * Contributors:
 *   Amine Khaldi
 *   Timo Kreuzer (timo.kreuzer@reactos.org)
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
#pragma once

#ifndef _WDMDDK_
#define _WDMDDK_

#define WDM_MAJORVERSION        0x06
#define WDM_MINORVERSION        0x00

/* Included via ntddk.h? */
#ifndef _NTDDK_
#define _NTDDK_
#define _WDM_INCLUDED_
#define _DDK_DRIVER_
#define NO_INTERLOCKED_INTRINSICS
#endif /* _NTDDK_ */

/* Dependencies */
#define NT_INCLUDED
#include <excpt.h>
#include <ntdef.h>
#include <ntstatus.h>
#include <ntiologc.h>

#ifndef GUID_DEFINED
#include <guiddef.h>
#endif

#ifdef _MAC
#ifndef _INC_STRING
#include <string.h>
#endif /* _INC_STRING */
#else
#include <string.h>
#endif /* _MAC */

#ifndef _KTMTYPES_
typedef GUID UOW, *PUOW;
#endif

typedef GUID *PGUID;

#if (NTDDI_VERSION >= NTDDI_WINXP)
#include <dpfilter.h>
#endif

#include "intrin.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_NTHALDLL_) && !defined(_BLDR_)
#define NTHALAPI DECLSPEC_IMPORT
#else
#define NTHALAPI
#endif

/* For ReactOS */
#if !defined(_NTOSKRNL_) && !defined(_BLDR_)
#define NTKERNELAPI DECLSPEC_IMPORT
#else
#define NTKERNELAPI
#endif

#if defined(_X86_) && !defined(_NTHAL_)
#define _DECL_HAL_KE_IMPORT  DECLSPEC_IMPORT
#elif defined(_X86_)
#define _DECL_HAL_KE_IMPORT
#else
#define _DECL_HAL_KE_IMPORT NTKERNELAPI
#endif

#if defined(_WIN64)
#define POINTER_ALIGNMENT DECLSPEC_ALIGN(8)
#else
#define POINTER_ALIGNMENT
#endif

#if defined(_MSC_VER)
/* Disable some warnings */
#pragma warning(disable:4115) /* Named type definition in parentheses */
#pragma warning(disable:4201) /* Nameless unions and structs */
#pragma warning(disable:4214) /* Bit fields of other types than int */
#pragma warning(disable:4820) /* Padding added, due to alignemnet requirement */

/* Indicate if #pragma alloc_text() is supported */
#if defined(_M_IX86) || defined(_M_AMD64) || defined(_M_IA64)
#define ALLOC_PRAGMA 1
#endif

/* Indicate if #pragma data_seg() is supported */
#if defined(_M_IX86) || defined(_M_AMD64)
#define ALLOC_DATA_PRAGMA 1
#endif

#endif /* _MSC_VER */

#if defined(_WIN64)
#if !defined(USE_DMA_MACROS) && !defined(_NTHAL_)
#define USE_DMA_MACROS
#endif
#if !defined(NO_LEGACY_DRIVERS) && !defined(__REACTOS__)
#define NO_LEGACY_DRIVERS
#endif
#endif /* defined(_WIN64) */

/* Forward declarations */
struct _IRP;
struct _MDL;
struct _KAPC;
struct _KDPC;
struct _FILE_OBJECT;
struct _DMA_ADAPTER;
struct _DEVICE_OBJECT;
struct _DRIVER_OBJECT;
struct _IO_STATUS_BLOCK;
struct _DEVICE_DESCRIPTION;
struct _SCATTER_GATHER_LIST;
struct _DRIVE_LAYOUT_INFORMATION;
struct _COMPRESSED_DATA_INFO;
struct _IO_RESOURCE_DESCRIPTOR;

/* Structures not exposed to drivers */
typedef struct _OBJECT_TYPE *POBJECT_TYPE;
typedef struct _HAL_DISPATCH_TABLE *PHAL_DISPATCH_TABLE;
typedef struct _HAL_PRIVATE_DISPATCH_TABLE *PHAL_PRIVATE_DISPATCH_TABLE;
typedef struct _CALLBACK_OBJECT *PCALLBACK_OBJECT;
typedef struct _EPROCESS *PEPROCESS;
typedef struct _ETHREAD *PETHREAD;
typedef struct _IO_TIMER *PIO_TIMER;
typedef struct _KINTERRUPT *PKINTERRUPT;
typedef struct _KPROCESS *PKPROCESS;
typedef struct _KTHREAD *PKTHREAD, *PRKTHREAD;
typedef struct _CONTEXT *PCONTEXT;

#if defined(USE_DMA_MACROS) && !defined(_NTHAL_)
typedef struct _DMA_ADAPTER *PADAPTER_OBJECT;
#elif defined(_WDM_INCLUDED_)
typedef struct _DMA_ADAPTER *PADAPTER_OBJECT;
#else
typedef struct _ADAPTER_OBJECT *PADAPTER_OBJECT; 
#endif

#ifndef DEFINE_GUIDEX
#ifdef _MSC_VER
#define DEFINE_GUIDEX(name) EXTERN_C const CDECL GUID name
#else
#define DEFINE_GUIDEX(name) EXTERN_C const GUID name
#endif
#endif /* DEFINE_GUIDEX */

#ifndef STATICGUIDOF
#define STATICGUIDOF(guid) STATIC_##guid
#endif

/* GUID Comparison */
#ifndef __IID_ALIGNED__
#define __IID_ALIGNED__
#ifdef __cplusplus
inline int IsEqualGUIDAligned(REFGUID guid1, REFGUID guid2)
{
    return ( (*(PLONGLONG)(&guid1) == *(PLONGLONG)(&guid2)) && 
             (*((PLONGLONG)(&guid1) + 1) == *((PLONGLONG)(&guid2) + 1)) );
}
#else
#define IsEqualGUIDAligned(guid1, guid2) \
           ( (*(PLONGLONG)(guid1) == *(PLONGLONG)(guid2)) && \
             (*((PLONGLONG)(guid1) + 1) == *((PLONGLONG)(guid2) + 1)) )
#endif /* __cplusplus */
#endif /* !__IID_ALIGNED__ */


/******************************************************************************
 *                           INTERLOCKED Functions                            *
 ******************************************************************************/
//
// Intrinsics (note: taken from our winnt.h)
// FIXME: 64-bit
//
#if defined(__GNUC__)

static __inline__ BOOLEAN
InterlockedBitTestAndSet(
  IN LONG volatile *Base,
  IN LONG Bit)
{
#if defined(_M_IX86)
  LONG OldBit;
  __asm__ __volatile__("lock "
                       "btsl %2,%1\n\t"
                       "sbbl %0,%0\n\t"
                       :"=r" (OldBit),"+m" (*Base)
                       :"Ir" (Bit)
                       : "memory");
  return OldBit;
#else
  return (_InterlockedOr(Base, 1 << Bit) >> Bit) & 1;
#endif
}

static __inline__ BOOLEAN
InterlockedBitTestAndReset(
  IN LONG volatile *Base,
  IN LONG Bit)
{
#if defined(_M_IX86)
  LONG OldBit;
  __asm__ __volatile__("lock "
                       "btrl %2,%1\n\t"
                       "sbbl %0,%0\n\t"
                       :"=r" (OldBit),"+m" (*Base)
                       :"Ir" (Bit)
                       : "memory");
  return OldBit;
#else
  return (_InterlockedAnd(Base, ~(1 << Bit)) >> Bit) & 1;
#endif
}

#endif /* defined(__GNUC__) */

#define BitScanForward _BitScanForward
#define BitScanReverse _BitScanReverse
#define BitTest _bittest
#define BitTestAndComplement _bittestandcomplement
#define BitTestAndSet _bittestandset
#define BitTestAndReset _bittestandreset
#define InterlockedBitTestAndSet _interlockedbittestandset
#define InterlockedBitTestAndReset _interlockedbittestandreset

#ifdef _M_AMD64
#define BitTest64 _bittest64
#define BitTestAndComplement64 _bittestandcomplement64
#define BitTestAndSet64 _bittestandset64
#define BitTestAndReset64 _bittestandreset64
#define InterlockedBitTestAndSet64 _interlockedbittestandset64
#define InterlockedBitTestAndReset64 _interlockedbittestandreset64
#endif

#if !defined(__INTERLOCKED_DECLARED)
#define __INTERLOCKED_DECLARED

#if defined (_X86_)
#if defined(NO_INTERLOCKED_INTRINSICS)
NTKERNELAPI
LONG
FASTCALL
InterlockedIncrement(
  IN OUT LONG volatile *Addend);

NTKERNELAPI
LONG
FASTCALL
InterlockedDecrement(
  IN OUT LONG volatile *Addend);

NTKERNELAPI
LONG
FASTCALL
InterlockedCompareExchange(
  IN OUT LONG volatile *Destination,
  IN LONG Exchange,
  IN LONG Comparand);

NTKERNELAPI
LONG
FASTCALL
InterlockedExchange(
  IN OUT LONG volatile *Destination,
  IN LONG Value);

NTKERNELAPI
LONG
FASTCALL
InterlockedExchangeAdd(
  IN OUT LONG volatile *Addend,
  IN LONG  Value);

#else /* !defined(NO_INTERLOCKED_INTRINSICS) */

#define InterlockedExchange _InterlockedExchange
#define InterlockedIncrement _InterlockedIncrement
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedExchangeAdd _InterlockedExchangeAdd
#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedOr _InterlockedOr
#define InterlockedAnd _InterlockedAnd
#define InterlockedXor _InterlockedXor

#endif /* !defined(NO_INTERLOCKED_INTRINSICS) */

#endif /* defined (_X86_) */

#if !defined (_WIN64)
/*
 * PVOID
 * InterlockedExchangePointer(
 *   IN OUT PVOID volatile  *Target,
 *   IN PVOID  Value)
 */
#define InterlockedExchangePointer(Target, Value) \
  ((PVOID) InterlockedExchange((PLONG) Target, (LONG) Value))

/*
 * PVOID
 * InterlockedCompareExchangePointer(
 *   IN OUT PVOID  *Destination,
 *   IN PVOID  Exchange,
 *   IN PVOID  Comparand)
 */
#define InterlockedCompareExchangePointer(Destination, Exchange, Comparand) \
  ((PVOID) InterlockedCompareExchange((PLONG) Destination, (LONG) Exchange, (LONG) Comparand))

#define InterlockedExchangeAddSizeT(a, b) InterlockedExchangeAdd((LONG *)a, b)
#define InterlockedIncrementSizeT(a) InterlockedIncrement((LONG *)a)
#define InterlockedDecrementSizeT(a) InterlockedDecrement((LONG *)a)

#endif // !defined (_WIN64)

#if defined (_M_AMD64)

#define InterlockedExchangeAddSizeT(a, b) InterlockedExchangeAdd64((LONGLONG *)a, (LONGLONG)b)
#define InterlockedIncrementSizeT(a) InterlockedIncrement64((LONGLONG *)a)
#define InterlockedDecrementSizeT(a) InterlockedDecrement64((LONGLONG *)a)
#define InterlockedAnd _InterlockedAnd
#define InterlockedOr _InterlockedOr
#define InterlockedXor _InterlockedXor
#define InterlockedIncrement _InterlockedIncrement
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedAdd _InterlockedAdd
#define InterlockedExchange _InterlockedExchange
#define InterlockedExchangeAdd _InterlockedExchangeAdd
#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedAnd64 _InterlockedAnd64
#define InterlockedOr64 _InterlockedOr64
#define InterlockedXor64 _InterlockedXor64
#define InterlockedIncrement64 _InterlockedIncrement64
#define InterlockedDecrement64 _InterlockedDecrement64
#define InterlockedAdd64 _InterlockedAdd64
#define InterlockedExchange64 _InterlockedExchange64
#define InterlockedExchangeAdd64 _InterlockedExchangeAdd64
#define InterlockedCompareExchange64 _InterlockedCompareExchange64
#define InterlockedCompareExchangePointer _InterlockedCompareExchangePointer
#define InterlockedExchangePointer _InterlockedExchangePointer
#define InterlockedBitTestAndSet64 _interlockedbittestandset64
#define InterlockedBitTestAndReset64 _interlockedbittestandreset64

#endif // _M_AMD64

#endif /* !__INTERLOCKED_DECLARED */


/******************************************************************************
 *                           Runtime Library Types                            *
 ******************************************************************************/

#define RTL_REGISTRY_ABSOLUTE             0
#define RTL_REGISTRY_SERVICES             1
#define RTL_REGISTRY_CONTROL              2
#define RTL_REGISTRY_WINDOWS_NT           3
#define RTL_REGISTRY_DEVICEMAP            4
#define RTL_REGISTRY_USER                 5
#define RTL_REGISTRY_MAXIMUM              6
#define RTL_REGISTRY_HANDLE               0x40000000
#define RTL_REGISTRY_OPTIONAL             0x80000000

/* RTL_QUERY_REGISTRY_TABLE.Flags */
#define RTL_QUERY_REGISTRY_SUBKEY         0x00000001
#define RTL_QUERY_REGISTRY_TOPKEY         0x00000002
#define RTL_QUERY_REGISTRY_REQUIRED       0x00000004
#define RTL_QUERY_REGISTRY_NOVALUE        0x00000008
#define RTL_QUERY_REGISTRY_NOEXPAND       0x00000010
#define RTL_QUERY_REGISTRY_DIRECT         0x00000020
#define RTL_QUERY_REGISTRY_DELETE         0x00000040

#define HASH_STRING_ALGORITHM_DEFAULT     0
#define HASH_STRING_ALGORITHM_X65599      1
#define HASH_STRING_ALGORITHM_INVALID     0xffffffff

typedef struct _RTL_BITMAP {
  ULONG SizeOfBitMap;
  PULONG Buffer;
} RTL_BITMAP, *PRTL_BITMAP;

typedef struct _RTL_BITMAP_RUN {
  ULONG StartingIndex;
  ULONG NumberOfBits;
} RTL_BITMAP_RUN, *PRTL_BITMAP_RUN;

typedef NTSTATUS
(NTAPI *PRTL_QUERY_REGISTRY_ROUTINE)(
  IN PWSTR ValueName,
  IN ULONG ValueType,
  IN PVOID ValueData,
  IN ULONG ValueLength,
  IN PVOID Context,
  IN PVOID EntryContext);

typedef struct _RTL_QUERY_REGISTRY_TABLE {
  PRTL_QUERY_REGISTRY_ROUTINE QueryRoutine;
  ULONG Flags;
  PCWSTR Name;
  PVOID EntryContext;
  ULONG DefaultType;
  PVOID DefaultData;
  ULONG DefaultLength;
} RTL_QUERY_REGISTRY_TABLE, *PRTL_QUERY_REGISTRY_TABLE;

typedef struct _TIME_FIELDS {
  CSHORT Year;
  CSHORT Month;
  CSHORT Day;
  CSHORT Hour;
  CSHORT Minute;
  CSHORT Second;
  CSHORT Milliseconds;
  CSHORT Weekday;
} TIME_FIELDS, *PTIME_FIELDS;

/* Slist Header */
#ifndef _SLIST_HEADER_
#define _SLIST_HEADER_

#if defined(_WIN64)

typedef struct DECLSPEC_ALIGN(16) _SLIST_ENTRY {
  struct _SLIST_ENTRY *Next;
} SLIST_ENTRY, *PSLIST_ENTRY;

typedef struct _SLIST_ENTRY32 {
  ULONG Next;
} SLIST_ENTRY32, *PSLIST_ENTRY32;

typedef union DECLSPEC_ALIGN(16) _SLIST_HEADER {
  _ANONYMOUS_STRUCT struct {
    ULONGLONG Alignment;
    ULONGLONG Region;
  } DUMMYSTRUCTNAME;
  struct {
    ULONGLONG Depth:16;
    ULONGLONG Sequence:9;
    ULONGLONG NextEntry:39;
    ULONGLONG HeaderType:1;
    ULONGLONG Init:1;
    ULONGLONG Reserved:59;
    ULONGLONG Region:3;
  } Header8;
  struct {
    ULONGLONG Depth:16;
    ULONGLONG Sequence:48;
    ULONGLONG HeaderType:1;
    ULONGLONG Init:1;
    ULONGLONG Reserved:2;
    ULONGLONG NextEntry:60;
  } Header16;
  struct {
    ULONGLONG Depth:16;
    ULONGLONG Sequence:48;
    ULONGLONG HeaderType:1;
    ULONGLONG Reserved:3;
    ULONGLONG NextEntry:60;
  } HeaderX64;
} SLIST_HEADER, *PSLIST_HEADER;

typedef union _SLIST_HEADER32 {
  ULONGLONG Alignment;
  _ANONYMOUS_STRUCT struct {
    SLIST_ENTRY32 Next;
    USHORT Depth;
    USHORT Sequence;
  } DUMMYSTRUCTNAME;
} SLIST_HEADER32, *PSLIST_HEADER32;

#else

#define SLIST_ENTRY SINGLE_LIST_ENTRY
#define _SLIST_ENTRY _SINGLE_LIST_ENTRY
#define PSLIST_ENTRY PSINGLE_LIST_ENTRY

typedef SLIST_ENTRY SLIST_ENTRY32, *PSLIST_ENTRY32;

typedef union _SLIST_HEADER {
  ULONGLONG Alignment;
  _ANONYMOUS_STRUCT struct {
    SLIST_ENTRY Next;
    USHORT Depth;
    USHORT Sequence;
  } DUMMYSTRUCTNAME;
} SLIST_HEADER, *PSLIST_HEADER;

typedef SLIST_HEADER SLIST_HEADER32, *PSLIST_HEADER32;

#endif /* defined(_WIN64) */

#endif /* _SLIST_HEADER_ */

/* MS definition is broken! */
extern BOOLEAN NTSYSAPI NlsMbCodePageTag;
extern BOOLEAN NTSYSAPI NlsMbOemCodePageTag;
#define NLS_MB_CODE_PAGE_TAG NlsMbCodePageTag
#define NLS_MB_OEM_CODE_PAGE_TAG NlsMbOemCodePageTag

#define SHORT_LEAST_SIGNIFICANT_BIT       0
#define SHORT_MOST_SIGNIFICANT_BIT        1

#define LONG_LEAST_SIGNIFICANT_BIT        0
#define LONG_3RD_MOST_SIGNIFICANT_BIT     1
#define LONG_2ND_MOST_SIGNIFICANT_BIT     2
#define LONG_MOST_SIGNIFICANT_BIT         3

#define RTLVERLIB_DDI(x) Wdmlib##x

typedef BOOLEAN
(*PFN_RTL_IS_NTDDI_VERSION_AVAILABLE)(
  IN ULONG Version);

typedef BOOLEAN
(*PFN_RTL_IS_SERVICE_PACK_VERSION_INSTALLED)(
  IN ULONG Version);

/******************************************************************************
 *                              Kernel Types                                  *
 ******************************************************************************/

typedef UCHAR KIRQL, *PKIRQL;
typedef CCHAR KPROCESSOR_MODE;
typedef LONG KPRIORITY;

typedef enum _MODE {
  KernelMode,
  UserMode,
  MaximumMode
} MODE;

#define CACHE_FULLY_ASSOCIATIVE 0xFF
#define MAXIMUM_SUSPEND_COUNT   MAXCHAR

#define EVENT_QUERY_STATE (0x0001)
#define EVENT_MODIFY_STATE (0x0002)
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3)

#define LTP_PC_SMT 0x1

#if (NTDDI_VERSION < NTDDI_WIN7) || defined(_X86_) || !defined(NT_PROCESSOR_GROUPS)
#define SINGLE_GROUP_LEGACY_API        1
#endif

#define SEMAPHORE_QUERY_STATE (0x0001)
#define SEMAPHORE_MODIFY_STATE (0x0002)
#define SEMAPHORE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3)

typedef enum _LOGICAL_PROCESSOR_RELATIONSHIP {
  RelationProcessorCore,
  RelationNumaNode,
  RelationCache,
  RelationProcessorPackage,
  RelationGroup,
  RelationAll = 0xffff
} LOGICAL_PROCESSOR_RELATIONSHIP;

typedef enum _PROCESSOR_CACHE_TYPE {
  CacheUnified,
  CacheInstruction,
  CacheData,
  CacheTrace
} PROCESSOR_CACHE_TYPE;

typedef struct _CACHE_DESCRIPTOR {
  UCHAR Level;
  UCHAR Associativity;
  USHORT LineSize;
  ULONG Size;
  PROCESSOR_CACHE_TYPE Type;
} CACHE_DESCRIPTOR, *PCACHE_DESCRIPTOR;

typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION {
  ULONG_PTR ProcessorMask;
  LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
  _ANONYMOUS_UNION union {
    struct {
      UCHAR Flags;
    } ProcessorCore;
    struct {
      ULONG NodeNumber;
    } NumaNode;
    CACHE_DESCRIPTOR Cache;
    ULONGLONG Reserved[2];
  } DUMMYUNIONNAME;
} SYSTEM_LOGICAL_PROCESSOR_INFORMATION, *PSYSTEM_LOGICAL_PROCESSOR_INFORMATION;

typedef struct _PROCESSOR_RELATIONSHIP {
  UCHAR Flags;
  UCHAR Reserved[21];
  USHORT GroupCount;
  GROUP_AFFINITY GroupMask[ANYSIZE_ARRAY];
} PROCESSOR_RELATIONSHIP, *PPROCESSOR_RELATIONSHIP;

typedef struct _NUMA_NODE_RELATIONSHIP {
  ULONG NodeNumber;
  UCHAR Reserved[20];
  GROUP_AFFINITY GroupMask;
} NUMA_NODE_RELATIONSHIP, *PNUMA_NODE_RELATIONSHIP;

typedef struct _CACHE_RELATIONSHIP {
  UCHAR Level;
  UCHAR Associativity;
  USHORT LineSize;
  ULONG CacheSize;
  PROCESSOR_CACHE_TYPE Type;
  UCHAR Reserved[20];
  GROUP_AFFINITY GroupMask;
} CACHE_RELATIONSHIP, *PCACHE_RELATIONSHIP;

typedef struct _PROCESSOR_GROUP_INFO {
  UCHAR MaximumProcessorCount;
  UCHAR ActiveProcessorCount;
  UCHAR Reserved[38];
  KAFFINITY ActiveProcessorMask;
} PROCESSOR_GROUP_INFO, *PPROCESSOR_GROUP_INFO;

typedef struct _GROUP_RELATIONSHIP {
  USHORT MaximumGroupCount;
  USHORT ActiveGroupCount;
  UCHAR Reserved[20];
  PROCESSOR_GROUP_INFO GroupInfo[ANYSIZE_ARRAY];
} GROUP_RELATIONSHIP, *PGROUP_RELATIONSHIP;

typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX {
  LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
  ULONG Size;
  _ANONYMOUS_UNION union {
    PROCESSOR_RELATIONSHIP Processor;
    NUMA_NODE_RELATIONSHIP NumaNode;
    CACHE_RELATIONSHIP Cache;
    GROUP_RELATIONSHIP Group;
  } DUMMYUNIONNAME;
} SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, *PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX;

/* Processor features */
#define PF_FLOATING_POINT_PRECISION_ERRATA  0
#define PF_FLOATING_POINT_EMULATED          1
#define PF_COMPARE_EXCHANGE_DOUBLE          2
#define PF_MMX_INSTRUCTIONS_AVAILABLE       3
#define PF_PPC_MOVEMEM_64BIT_OK             4
#define PF_ALPHA_BYTE_INSTRUCTIONS          5
#define PF_XMMI_INSTRUCTIONS_AVAILABLE      6
#define PF_3DNOW_INSTRUCTIONS_AVAILABLE     7
#define PF_RDTSC_INSTRUCTION_AVAILABLE      8
#define PF_PAE_ENABLED                      9
#define PF_XMMI64_INSTRUCTIONS_AVAILABLE   10
#define PF_SSE_DAZ_MODE_AVAILABLE          11
#define PF_NX_ENABLED                      12
#define PF_SSE3_INSTRUCTIONS_AVAILABLE     13
#define PF_COMPARE_EXCHANGE128             14
#define PF_COMPARE64_EXCHANGE128           15
#define PF_CHANNELS_ENABLED                16
#define PF_XSAVE_ENABLED                   17

#define MAXIMUM_WAIT_OBJECTS              64

#define ASSERT_APC(Object) NT_ASSERT((Object)->Type == ApcObject)

#define ASSERT_DPC(Object) \
    ASSERT(((Object)->Type == 0) || \
           ((Object)->Type == DpcObject) || \
           ((Object)->Type == ThreadedDpcObject))

#define ASSERT_GATE(object) \
    NT_ASSERT((((object)->Header.Type & KOBJECT_TYPE_MASK) == GateObject) || \
              (((object)->Header.Type & KOBJECT_TYPE_MASK) == EventSynchronizationObject))

#define ASSERT_DEVICE_QUEUE(Object) \
    NT_ASSERT((Object)->Type == DeviceQueueObject)

#define ASSERT_TIMER(E) \
    NT_ASSERT(((E)->Header.Type == TimerNotificationObject) || \
              ((E)->Header.Type == TimerSynchronizationObject))

#define ASSERT_MUTANT(E) \
    NT_ASSERT((E)->Header.Type == MutantObject)

#define ASSERT_SEMAPHORE(E) \
    NT_ASSERT((E)->Header.Type == SemaphoreObject)

#define ASSERT_EVENT(E) \
    NT_ASSERT(((E)->Header.Type == NotificationEvent) || \
              ((E)->Header.Type == SynchronizationEvent))

#define DPC_NORMAL 0
#define DPC_THREADED 1

#define GM_LOCK_BIT          0x1
#define GM_LOCK_BIT_V        0x0
#define GM_LOCK_WAITER_WOKEN 0x2
#define GM_LOCK_WAITER_INC   0x4

#define LOCK_QUEUE_WAIT_BIT               0
#define LOCK_QUEUE_OWNER_BIT              1

#define LOCK_QUEUE_WAIT                   1
#define LOCK_QUEUE_OWNER                  2
#define LOCK_QUEUE_TIMER_LOCK_SHIFT       4
#define LOCK_QUEUE_TIMER_TABLE_LOCKS (1 << (8 - LOCK_QUEUE_TIMER_LOCK_SHIFT))

#define PROCESSOR_FEATURE_MAX 64

#define DBG_STATUS_CONTROL_C              1
#define DBG_STATUS_SYSRQ                  2
#define DBG_STATUS_BUGCHECK_FIRST         3
#define DBG_STATUS_BUGCHECK_SECOND        4
#define DBG_STATUS_FATAL                  5
#define DBG_STATUS_DEBUG_CONTROL          6
#define DBG_STATUS_WORKER                 7

#if defined(_WIN64)
#define MAXIMUM_PROC_PER_GROUP 64
#else
#define MAXIMUM_PROC_PER_GROUP 32
#endif
#define MAXIMUM_PROCESSORS          MAXIMUM_PROC_PER_GROUP

/* Exception Records */
#define EXCEPTION_NONCONTINUABLE     1
#define EXCEPTION_MAXIMUM_PARAMETERS 15

#define EXCEPTION_DIVIDED_BY_ZERO       0
#define EXCEPTION_DEBUG                 1
#define EXCEPTION_NMI                   2
#define EXCEPTION_INT3                  3
#define EXCEPTION_BOUND_CHECK           5
#define EXCEPTION_INVALID_OPCODE        6
#define EXCEPTION_NPX_NOT_AVAILABLE     7
#define EXCEPTION_DOUBLE_FAULT          8
#define EXCEPTION_NPX_OVERRUN           9
#define EXCEPTION_INVALID_TSS           0x0A
#define EXCEPTION_SEGMENT_NOT_PRESENT   0x0B
#define EXCEPTION_STACK_FAULT           0x0C
#define EXCEPTION_GP_FAULT              0x0D
#define EXCEPTION_RESERVED_TRAP         0x0F
#define EXCEPTION_NPX_ERROR             0x010
#define EXCEPTION_ALIGNMENT_CHECK       0x011

typedef struct _EXCEPTION_RECORD {
  NTSTATUS ExceptionCode;
  ULONG ExceptionFlags;
  struct _EXCEPTION_RECORD *ExceptionRecord;
  PVOID ExceptionAddress;
  ULONG NumberParameters;
  ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_RECORD32 {
  NTSTATUS ExceptionCode;
  ULONG ExceptionFlags;
  ULONG ExceptionRecord;
  ULONG ExceptionAddress;
  ULONG NumberParameters;
  ULONG ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD32, *PEXCEPTION_RECORD32;

typedef struct _EXCEPTION_RECORD64 {
  NTSTATUS ExceptionCode;
  ULONG ExceptionFlags;
  ULONG64 ExceptionRecord;
  ULONG64 ExceptionAddress;
  ULONG NumberParameters;
  ULONG __unusedAlignment;
  ULONG64 ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD64, *PEXCEPTION_RECORD64;

typedef struct _EXCEPTION_POINTERS {
  PEXCEPTION_RECORD ExceptionRecord;
  PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

typedef enum _KBUGCHECK_CALLBACK_REASON {
  KbCallbackInvalid,
  KbCallbackReserved1,
  KbCallbackSecondaryDumpData,
  KbCallbackDumpIo,
  KbCallbackAddPages,
  KbCallbackSecondaryMultiPartDumpData,
  KbCallbackRemovePages,
  KbCallbackTriageDumpData
} KBUGCHECK_CALLBACK_REASON;

struct _KBUGCHECK_REASON_CALLBACK_RECORD;

typedef VOID
(NTAPI KBUGCHECK_REASON_CALLBACK_ROUTINE)(
  IN KBUGCHECK_CALLBACK_REASON Reason,
  IN struct _KBUGCHECK_REASON_CALLBACK_RECORD *Record,
  IN OUT PVOID ReasonSpecificData,
  IN ULONG ReasonSpecificDataLength);
typedef KBUGCHECK_REASON_CALLBACK_ROUTINE *PKBUGCHECK_REASON_CALLBACK_ROUTINE;

typedef struct _KBUGCHECK_ADD_PAGES {
  IN OUT PVOID Context;
  IN OUT ULONG Flags;
  IN ULONG BugCheckCode;
  OUT ULONG_PTR Address;
  OUT ULONG_PTR Count;
} KBUGCHECK_ADD_PAGES, *PKBUGCHECK_ADD_PAGES;

typedef struct _KBUGCHECK_SECONDARY_DUMP_DATA {
  IN PVOID InBuffer;
  IN ULONG InBufferLength;
  IN ULONG MaximumAllowed;
  OUT GUID Guid;
  OUT PVOID OutBuffer;
  OUT ULONG OutBufferLength;
} KBUGCHECK_SECONDARY_DUMP_DATA, *PKBUGCHECK_SECONDARY_DUMP_DATA;

typedef enum _KBUGCHECK_DUMP_IO_TYPE {
  KbDumpIoInvalid,
  KbDumpIoHeader,
  KbDumpIoBody,
  KbDumpIoSecondaryData,
  KbDumpIoComplete
} KBUGCHECK_DUMP_IO_TYPE;

typedef struct _KBUGCHECK_DUMP_IO {
  IN ULONG64 Offset;
  IN PVOID Buffer;
  IN ULONG BufferLength;
  IN KBUGCHECK_DUMP_IO_TYPE Type;
} KBUGCHECK_DUMP_IO, *PKBUGCHECK_DUMP_IO;

#define KB_ADD_PAGES_FLAG_VIRTUAL_ADDRESS         0x00000001UL
#define KB_ADD_PAGES_FLAG_PHYSICAL_ADDRESS        0x00000002UL
#define KB_ADD_PAGES_FLAG_ADDITIONAL_RANGES_EXIST 0x80000000UL

typedef struct _KBUGCHECK_REASON_CALLBACK_RECORD {
  LIST_ENTRY Entry;
  PKBUGCHECK_REASON_CALLBACK_ROUTINE CallbackRoutine;
  PUCHAR Component;
  ULONG_PTR Checksum;
  KBUGCHECK_CALLBACK_REASON Reason;
  UCHAR State;
} KBUGCHECK_REASON_CALLBACK_RECORD, *PKBUGCHECK_REASON_CALLBACK_RECORD;

typedef enum _KBUGCHECK_BUFFER_DUMP_STATE {
  BufferEmpty,
  BufferInserted,
  BufferStarted,
  BufferFinished,
  BufferIncomplete
} KBUGCHECK_BUFFER_DUMP_STATE;

typedef VOID
(NTAPI KBUGCHECK_CALLBACK_ROUTINE)(
  IN PVOID Buffer,
  IN ULONG Length);
typedef KBUGCHECK_CALLBACK_ROUTINE *PKBUGCHECK_CALLBACK_ROUTINE;

typedef struct _KBUGCHECK_CALLBACK_RECORD {
  LIST_ENTRY Entry;
  PKBUGCHECK_CALLBACK_ROUTINE CallbackRoutine;
  PVOID Buffer;
  ULONG Length;
  PUCHAR Component;
  ULONG_PTR Checksum;
  UCHAR State;
} KBUGCHECK_CALLBACK_RECORD, *PKBUGCHECK_CALLBACK_RECORD;

typedef BOOLEAN
(NTAPI NMI_CALLBACK)(
  IN PVOID Context,
  IN BOOLEAN Handled);
typedef NMI_CALLBACK *PNMI_CALLBACK;

typedef enum _KE_PROCESSOR_CHANGE_NOTIFY_STATE {
  KeProcessorAddStartNotify = 0,
  KeProcessorAddCompleteNotify,
  KeProcessorAddFailureNotify
} KE_PROCESSOR_CHANGE_NOTIFY_STATE;

typedef struct _KE_PROCESSOR_CHANGE_NOTIFY_CONTEXT {
  KE_PROCESSOR_CHANGE_NOTIFY_STATE State;
  ULONG NtNumber;
  NTSTATUS Status;
#if (NTDDI_VERSION >= NTDDI_WIN7)
  PROCESSOR_NUMBER ProcNumber;
#endif
} KE_PROCESSOR_CHANGE_NOTIFY_CONTEXT, *PKE_PROCESSOR_CHANGE_NOTIFY_CONTEXT;

typedef VOID
(NTAPI PROCESSOR_CALLBACK_FUNCTION)(
  IN PVOID CallbackContext,
  IN PKE_PROCESSOR_CHANGE_NOTIFY_CONTEXT ChangeContext,
  IN OUT PNTSTATUS OperationStatus);
typedef PROCESSOR_CALLBACK_FUNCTION *PPROCESSOR_CALLBACK_FUNCTION;

#define KE_PROCESSOR_CHANGE_ADD_EXISTING         1

#define INVALID_PROCESSOR_INDEX     0xffffffff

typedef enum _KINTERRUPT_POLARITY {
  InterruptPolarityUnknown,
  InterruptActiveHigh,
  InterruptRisingEdge = InterruptActiveHigh,
  InterruptActiveLow,
  InterruptFallingEdge = InterruptActiveLow,
#if NTDDI_VERSION >= NTDDI_WIN8
  InterruptActiveBoth,
#endif
#if NTDDI_VERSION >= NTDDI_WINBLUE
  InterruptActiveBothTriggerLow = InterruptActiveBoth,
  InterruptActiveBothTriggerHigh
#endif
} KINTERRUPT_POLARITY, *PKINTERRUPT_POLARITY;

typedef enum _KPROFILE_SOURCE {
  ProfileTime,
  ProfileAlignmentFixup,
  ProfileTotalIssues,
  ProfilePipelineDry,
  ProfileLoadInstructions,
  ProfilePipelineFrozen,
  ProfileBranchInstructions,
  ProfileTotalNonissues,
  ProfileDcacheMisses,
  ProfileIcacheMisses,
  ProfileCacheMisses,
  ProfileBranchMispredictions,
  ProfileStoreInstructions,
  ProfileFpInstructions,
  ProfileIntegerInstructions,
  Profile2Issue,
  Profile3Issue,
  Profile4Issue,
  ProfileSpecialInstructions,
  ProfileTotalCycles,
  ProfileIcacheIssues,
  ProfileDcacheAccesses,
  ProfileMemoryBarrierCycles,
  ProfileLoadLinkedIssues,
  ProfileMaximum
} KPROFILE_SOURCE;

typedef enum _KWAIT_REASON {
  Executive,
  FreePage,
  PageIn,
  PoolAllocation,
  DelayExecution,
  Suspended,
  UserRequest,
  WrExecutive,
  WrFreePage,
  WrPageIn,
  WrPoolAllocation,
  WrDelayExecution,
  WrSuspended,
  WrUserRequest,
  WrSpare0,
  WrQueue,
  WrLpcReceive,
  WrLpcReply,
  WrVirtualMemory,
  WrPageOut,
  WrRendezvous,
  WrKeyedEvent,
  WrTerminated,
  WrProcessInSwap,
  WrCpuRateControl,
  WrCalloutStack,
  WrKernel,
  WrResource,
  WrPushLock,
  WrMutex,
  WrQuantumEnd,
  WrDispatchInt,
  WrPreempted,
  WrYieldExecution,
  WrFastMutex,
  WrGuardedMutex,
  WrRundown,
  WrAlertByThreadId,
  WrDeferredPreempt,
  WrPhysicalFault,
  MaximumWaitReason
} KWAIT_REASON;

typedef struct _KWAIT_BLOCK {
  LIST_ENTRY WaitListEntry;
  struct _KTHREAD *Thread;
  PVOID Object;
  struct _KWAIT_BLOCK *NextWaitBlock;
  USHORT WaitKey;
  UCHAR WaitType;
  volatile UCHAR BlockState;
#if defined(_WIN64)
  LONG SpareLong;
#endif
} KWAIT_BLOCK, *PKWAIT_BLOCK, *PRKWAIT_BLOCK;

typedef enum _KINTERRUPT_MODE {
  LevelSensitive,
  Latched
} KINTERRUPT_MODE;

#define THREAD_WAIT_OBJECTS 3

typedef VOID
(NTAPI KSTART_ROUTINE)(
  IN PVOID StartContext);
typedef KSTART_ROUTINE *PKSTART_ROUTINE;

typedef VOID
(NTAPI *PKINTERRUPT_ROUTINE)(
  VOID);

typedef BOOLEAN
(NTAPI KSERVICE_ROUTINE)(
  IN struct _KINTERRUPT *Interrupt,
  IN PVOID ServiceContext);
typedef KSERVICE_ROUTINE *PKSERVICE_ROUTINE;

typedef BOOLEAN
(NTAPI KMESSAGE_SERVICE_ROUTINE)(
  IN struct _KINTERRUPT *Interrupt,
  IN PVOID ServiceContext,
  IN ULONG MessageID);
typedef KMESSAGE_SERVICE_ROUTINE *PKMESSAGE_SERVICE_ROUTINE;

typedef enum _KD_OPTION {
  KD_OPTION_SET_BLOCK_ENABLE,
} KD_OPTION;

typedef VOID
(NTAPI *PKNORMAL_ROUTINE)(
  IN PVOID NormalContext OPTIONAL,
  IN PVOID SystemArgument1 OPTIONAL,
  IN PVOID SystemArgument2 OPTIONAL);

typedef VOID
(NTAPI *PKRUNDOWN_ROUTINE)(
  IN struct _KAPC *Apc);

typedef VOID
(NTAPI *PKKERNEL_ROUTINE)(
  IN struct _KAPC *Apc,
  IN OUT PKNORMAL_ROUTINE *NormalRoutine OPTIONAL,
  IN OUT PVOID *NormalContext OPTIONAL,
  IN OUT PVOID *SystemArgument1 OPTIONAL,
  IN OUT PVOID *SystemArgument2 OPTIONAL);

typedef struct _KAPC {
  UCHAR Type;
  UCHAR SpareByte0;
  UCHAR Size;
  UCHAR SpareByte1;
  ULONG SpareLong0;
  struct _KTHREAD *Thread;
  LIST_ENTRY ApcListEntry;
  PKKERNEL_ROUTINE KernelRoutine;
  PKRUNDOWN_ROUTINE RundownRoutine;
  PKNORMAL_ROUTINE NormalRoutine;
  PVOID NormalContext;
  PVOID SystemArgument1;
  PVOID SystemArgument2;
  CCHAR ApcStateIndex;
  KPROCESSOR_MODE ApcMode;
  BOOLEAN Inserted;
} KAPC, *PKAPC, *RESTRICTED_POINTER PRKAPC;

#define KAPC_OFFSET_TO_SPARE_BYTE0 FIELD_OFFSET(KAPC, SpareByte0)
#define KAPC_OFFSET_TO_SPARE_BYTE1 FIELD_OFFSET(KAPC, SpareByte1)
#define KAPC_OFFSET_TO_SPARE_LONG FIELD_OFFSET(KAPC, SpareLong0)
#define KAPC_OFFSET_TO_SYSTEMARGUMENT1 FIELD_OFFSET(KAPC, SystemArgument1)
#define KAPC_OFFSET_TO_SYSTEMARGUMENT2 FIELD_OFFSET(KAPC, SystemArgument2)
#define KAPC_OFFSET_TO_APCSTATEINDEX FIELD_OFFSET(KAPC, ApcStateIndex)
#define KAPC_ACTUAL_LENGTH (FIELD_OFFSET(KAPC, Inserted) + sizeof(BOOLEAN))

typedef struct _KDEVICE_QUEUE_ENTRY {
  LIST_ENTRY DeviceListEntry;
  ULONG SortKey;
  BOOLEAN Inserted;
} KDEVICE_QUEUE_ENTRY, *PKDEVICE_QUEUE_ENTRY,
*RESTRICTED_POINTER PRKDEVICE_QUEUE_ENTRY;

typedef PVOID PKIPI_CONTEXT;

typedef VOID
(NTAPI *PKIPI_WORKER)(
  IN OUT PKIPI_CONTEXT PacketContext,
  IN PVOID Parameter1 OPTIONAL,
  IN PVOID Parameter2 OPTIONAL,
  IN PVOID Parameter3 OPTIONAL);

typedef struct _KIPI_COUNTS {
  ULONG Freeze;
  ULONG Packet;
  ULONG DPC;
  ULONG APC;
  ULONG FlushSingleTb;
  ULONG FlushMultipleTb;
  ULONG FlushEntireTb;
  ULONG GenericCall;
  ULONG ChangeColor;
  ULONG SweepDcache;
  ULONG SweepIcache;
  ULONG SweepIcacheRange;
  ULONG FlushIoBuffers;
  ULONG GratuitousDPC;
} KIPI_COUNTS, *PKIPI_COUNTS;

typedef ULONG_PTR
(NTAPI KIPI_BROADCAST_WORKER)(
  IN ULONG_PTR Argument);
typedef KIPI_BROADCAST_WORKER *PKIPI_BROADCAST_WORKER;

typedef ULONG_PTR KSPIN_LOCK, *PKSPIN_LOCK;

typedef struct _KSPIN_LOCK_QUEUE {
  struct _KSPIN_LOCK_QUEUE *volatile Next;
  PKSPIN_LOCK volatile Lock;
} KSPIN_LOCK_QUEUE, *PKSPIN_LOCK_QUEUE;

typedef struct _KLOCK_QUEUE_HANDLE {
  KSPIN_LOCK_QUEUE LockQueue;
  KIRQL OldIrql;
} KLOCK_QUEUE_HANDLE, *PKLOCK_QUEUE_HANDLE;

#if defined(_AMD64_)

typedef ULONG64 KSPIN_LOCK_QUEUE_NUMBER;

#define LockQueueUnusedSpare0 0
#define LockQueueUnusedSpare1 1
#define LockQueueUnusedSpare2 2
#define LockQueueUnusedSpare3 3
#define LockQueueVacbLock 4
#define LockQueueMasterLock 5
#define LockQueueNonPagedPoolLock 6
#define LockQueueIoCancelLock 7
#define LockQueueUnusedSpare8 8
#define LockQueueIoVpbLock 9
#define LockQueueIoDatabaseLock 10
#define LockQueueIoCompletionLock 11
#define LockQueueNtfsStructLock 12
#define LockQueueAfdWorkQueueLock 13
#define LockQueueBcbLock 14
#define LockQueueUnusedSpare15 15
#define LockQueueUnusedSpare16 16
#define LockQueueMaximumLock 17

#else

typedef enum _KSPIN_LOCK_QUEUE_NUMBER {
  LockQueueUnusedSpare0,
  LockQueueUnusedSpare1,
  LockQueueUnusedSpare2,
  LockQueueUnusedSpare3,
  LockQueueVacbLock,
  LockQueueMasterLock,
  LockQueueNonPagedPoolLock,
  LockQueueIoCancelLock,
  LockQueueUnusedSpare8,
  LockQueueIoVpbLock,
  LockQueueIoDatabaseLock,
  LockQueueIoCompletionLock,
  LockQueueNtfsStructLock,
  LockQueueAfdWorkQueueLock,
  LockQueueBcbLock,
  LockQueueUnusedSpare15,
  LockQueueUnusedSpare16,
  LockQueueMaximumLock = LockQueueUnusedSpare16 + 1
} KSPIN_LOCK_QUEUE_NUMBER, *PKSPIN_LOCK_QUEUE_NUMBER;

#endif /* defined(_AMD64_) */

typedef VOID
(NTAPI KDEFERRED_ROUTINE)(
  IN struct _KDPC *Dpc,
  IN PVOID DeferredContext OPTIONAL,
  IN PVOID SystemArgument1 OPTIONAL,
  IN PVOID SystemArgument2 OPTIONAL);
typedef KDEFERRED_ROUTINE *PKDEFERRED_ROUTINE;

typedef enum _KDPC_IMPORTANCE {
  LowImportance,
  MediumImportance,
  HighImportance,
  MediumHighImportance
} KDPC_IMPORTANCE;

typedef struct _KDPC {
  UCHAR Type;
  UCHAR Importance;
  volatile USHORT Number;
  LIST_ENTRY DpcListEntry;
  PKDEFERRED_ROUTINE DeferredRoutine;
  PVOID DeferredContext;
  PVOID SystemArgument1;
  PVOID SystemArgument2;
  volatile PVOID DpcData;
} KDPC, *PKDPC, *RESTRICTED_POINTER PRKDPC;

typedef struct _KDPC_WATCHDOG_INFORMATION {
  ULONG DpcTimeLimit;
  ULONG DpcTimeCount;
  ULONG DpcWatchdogLimit;
  ULONG DpcWatchdogCount;
  ULONG Reserved;
} KDPC_WATCHDOG_INFORMATION, *PKDPC_WATCHDOG_INFORMATION;

typedef struct _KDEVICE_QUEUE {
  CSHORT Type;
  CSHORT Size;
  LIST_ENTRY DeviceListHead;
  KSPIN_LOCK Lock;
# if defined(_AMD64_)
  _ANONYMOUS_UNION union {
    BOOLEAN Busy;
    _ANONYMOUS_STRUCT struct {
      LONG64 Reserved:8;
      LONG64 Hint:56;
    } DUMMYSTRUCTNAME;
  } DUMMYUNIONNAME;
# else
  BOOLEAN Busy;
# endif
} KDEVICE_QUEUE, *PKDEVICE_QUEUE, *RESTRICTED_POINTER PRKDEVICE_QUEUE;

#define TIMER_EXPIRED_INDEX_BITS        6
#define TIMER_PROCESSOR_INDEX_BITS      5

typedef struct _DISPATCHER_HEADER {
  _ANONYMOUS_UNION union {
    _ANONYMOUS_STRUCT struct {
      UCHAR Type;
      _ANONYMOUS_UNION union {
        _ANONYMOUS_UNION union {
          UCHAR TimerControlFlags;
          _ANONYMOUS_STRUCT struct {
            UCHAR Absolute:1;
            UCHAR Coalescable:1;
            UCHAR KeepShifting:1;
            UCHAR EncodedTolerableDelay:5;
          } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;
        UCHAR Abandoned;
#if (NTDDI_VERSION < NTDDI_WIN7)
        UCHAR NpxIrql;
#endif
        BOOLEAN Signalling;
      } DUMMYUNIONNAME;
      _ANONYMOUS_UNION union {
        _ANONYMOUS_UNION union {
          UCHAR ThreadControlFlags;
          _ANONYMOUS_STRUCT struct {
            UCHAR CpuThrottled:1;
            UCHAR CycleProfiling:1;
            UCHAR CounterProfiling:1;
            UCHAR Reserved:5;
          } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;
        UCHAR Size;
        UCHAR Hand;
      } DUMMYUNIONNAME2;
      _ANONYMOUS_UNION union {
#if (NTDDI_VERSION >= NTDDI_WIN7)
        _ANONYMOUS_UNION union {
          UCHAR TimerMiscFlags;
          _ANONYMOUS_STRUCT struct {
#if !defined(_X86_)
            UCHAR Index:TIMER_EXPIRED_INDEX_BITS;
#else
            UCHAR Index:1;
            UCHAR Processor:TIMER_PROCESSOR_INDEX_BITS;
#endif
            UCHAR Inserted:1;
            volatile UCHAR Expired:1;
          } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;
#else
        /* Pre Win7 compatibility fix to latest WDK */
        UCHAR Inserted;
#endif
        _ANONYMOUS_UNION union {
          BOOLEAN DebugActive;
          _ANONYMOUS_STRUCT struct {
            BOOLEAN ActiveDR7:1;
            BOOLEAN Instrumented:1;
            BOOLEAN Reserved2:4;
            BOOLEAN UmsScheduled:1;
            BOOLEAN UmsPrimary:1;
          } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME; /* should probably be DUMMYUNIONNAME2, but this is what WDK says */
        BOOLEAN DpcActive;
      } DUMMYUNIONNAME3;
    } DUMMYSTRUCTNAME;
    volatile LONG Lock;
  } DUMMYUNIONNAME;
  LONG SignalState;
  LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER, *PDISPATCHER_HEADER;

typedef struct _KEVENT {
  DISPATCHER_HEADER Header;
} KEVENT, *PKEVENT, *RESTRICTED_POINTER PRKEVENT;

typedef struct _KSEMAPHORE {
  DISPATCHER_HEADER Header;
  LONG Limit;
} KSEMAPHORE, *PKSEMAPHORE, *RESTRICTED_POINTER PRKSEMAPHORE;

#define KSEMAPHORE_ACTUAL_LENGTH (FIELD_OFFSET(KSEMAPHORE, Limit) + sizeof(LONG))

typedef struct _KGATE {
  DISPATCHER_HEADER Header;
} KGATE, *PKGATE, *RESTRICTED_POINTER PRKGATE;

typedef struct _KGUARDED_MUTEX {
  volatile LONG Count;
  PKTHREAD Owner;
  ULONG Contention;
  KGATE Gate;
  _ANONYMOUS_UNION union {
    _ANONYMOUS_STRUCT struct {
      SHORT KernelApcDisable;
      SHORT SpecialApcDisable;
    } DUMMYSTRUCTNAME;
    ULONG CombinedApcDisable;
  } DUMMYUNIONNAME;
} KGUARDED_MUTEX, *PKGUARDED_MUTEX;

typedef struct _KMUTANT {
  DISPATCHER_HEADER Header;
  LIST_ENTRY MutantListEntry;
  struct _KTHREAD *RESTRICTED_POINTER OwnerThread;
  BOOLEAN Abandoned;
  UCHAR ApcDisable;
} KMUTANT, *PKMUTANT, *RESTRICTED_POINTER PRKMUTANT, KMUTEX, *PKMUTEX, *RESTRICTED_POINTER PRKMUTEX;

#define TIMER_TABLE_SIZE 512
#define TIMER_TABLE_SHIFT 9

typedef struct _KTIMER {
  DISPATCHER_HEADER Header;
  ULARGE_INTEGER DueTime;
  LIST_ENTRY TimerListEntry;
  struct _KDPC *Dpc;
# if !defined(_X86_)
  ULONG Processor;
# endif
  ULONG Period;
} KTIMER, *PKTIMER, *RESTRICTED_POINTER PRKTIMER;

typedef enum _LOCK_OPERATION {
  IoReadAccess,
  IoWriteAccess,
  IoModifyAccess
} LOCK_OPERATION;

#define KTIMER_ACTUAL_LENGTH (FIELD_OFFSET(KTIMER, Period) + sizeof(LONG))

typedef BOOLEAN (NTAPI KSYNCHRONIZE_ROUTINE)(PVOID SynchronizeContext);
typedef KSYNCHRONIZE_ROUTINE *PKSYNCHRONIZE_ROUTINE;

typedef enum _POOL_TYPE {
  NonPagedPool,
  NonPagedPoolExecute = NonPagedPool,
  PagedPool,
  NonPagedPoolMustSucceed,
  DontUseThisType,
  NonPagedPoolCacheAligned,
  PagedPoolCacheAligned,
  NonPagedPoolCacheAlignedMustS,
  MaxPoolType,
  NonPagedPoolBase = 0,
  NonPagedPoolBaseMustSucceed = 2,
  NonPagedPoolBaseCacheAligned = 4,
  NonPagedPoolBaseCacheAlignedMustS = 6,
  NonPagedPoolSession = 32,
  PagedPoolSession,
  NonPagedPoolMustSucceedSession,
  DontUseThisTypeSession,
  NonPagedPoolCacheAlignedSession,
  PagedPoolCacheAlignedSession,
  NonPagedPoolCacheAlignedMustSSession,
  NonPagedPoolNx = 512,
  NonPagedPoolNxCacheAligned = 516,
  NonPagedPoolSessionNx = 544,
} POOL_TYPE;

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE {
  StandardDesign,
  NEC98x86,
  EndAlternatives
} ALTERNATIVE_ARCHITECTURE_TYPE;

#ifndef _X86_

#ifndef IsNEC_98
#define IsNEC_98 (FALSE)
#endif

#ifndef IsNotNEC_98
#define IsNotNEC_98 (TRUE)
#endif

#ifndef SetNEC_98
#define SetNEC_98
#endif

#ifndef SetNotNEC_98
#define SetNotNEC_98
#endif

#endif

typedef struct _KSYSTEM_TIME {
  ULONG LowPart;
  LONG High1Time;
  LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;

typedef struct DECLSPEC_ALIGN(16) _M128A {
  ULONGLONG Low;
  LONGLONG High;
} M128A, *PM128A;

typedef struct DECLSPEC_ALIGN(16) _XSAVE_FORMAT {
  USHORT ControlWord;
  USHORT StatusWord;
  UCHAR TagWord;
  UCHAR Reserved1;
  USHORT ErrorOpcode;
  ULONG ErrorOffset;
  USHORT ErrorSelector;
  USHORT Reserved2;
  ULONG DataOffset;
  USHORT DataSelector;
  USHORT Reserved3;
  ULONG MxCsr;
  ULONG MxCsr_Mask;
  M128A FloatRegisters[8];
#if defined(_WIN64)
  M128A XmmRegisters[16];
  UCHAR Reserved4[96];
#else
  M128A XmmRegisters[8];
  UCHAR Reserved4[192];
  ULONG StackControl[7];
  ULONG Cr0NpxState;
#endif
} XSAVE_FORMAT, *PXSAVE_FORMAT;

typedef struct DECLSPEC_ALIGN(8) _XSAVE_AREA_HEADER {
  ULONG64 Mask;
  ULONG64 Reserved[7];
} XSAVE_AREA_HEADER, *PXSAVE_AREA_HEADER;

typedef struct DECLSPEC_ALIGN(16) _XSAVE_AREA {
  XSAVE_FORMAT LegacyState;
  XSAVE_AREA_HEADER Header;
} XSAVE_AREA, *PXSAVE_AREA;

typedef struct _XSTATE_CONTEXT {
  ULONG64 Mask;
  ULONG Length;
  ULONG Reserved1;
  PXSAVE_AREA Area;
#if defined(_X86_)
  ULONG Reserved2;
#endif
  PVOID Buffer;
#if defined(_X86_)
  ULONG Reserved3;
#endif
} XSTATE_CONTEXT, *PXSTATE_CONTEXT;

typedef struct _XSTATE_SAVE {
#if defined(_AMD64_)
  struct _XSTATE_SAVE* Prev;
  struct _KTHREAD* Thread;
  UCHAR Level;
  XSTATE_CONTEXT XStateContext;
#elif defined(_IA64_)
  ULONG Dummy;
#elif defined(_X86_)
  _ANONYMOUS_UNION union {
    _ANONYMOUS_STRUCT struct {
      LONG64 Reserved1;
      ULONG Reserved2;
      struct _XSTATE_SAVE* Prev;
      PXSAVE_AREA Reserved3;
      struct _KTHREAD* Thread;
      PVOID Reserved4;
      UCHAR Level;
    } DUMMYSTRUCTNAME;
    XSTATE_CONTEXT XStateContext;
  } DUMMYUNIONNAME;
#endif
} XSTATE_SAVE, *PXSTATE_SAVE;

#ifdef _X86_

#define MAXIMUM_SUPPORTED_EXTENSION  512

#if !defined(__midl) && !defined(MIDL_PASS)
C_ASSERT(sizeof(XSAVE_FORMAT) == MAXIMUM_SUPPORTED_EXTENSION);
#endif

#endif /* _X86_ */

#define XSAVE_ALIGN                    64
#define MINIMAL_XSTATE_AREA_LENGTH     sizeof(XSAVE_AREA)

#if !defined(__midl) && !defined(MIDL_PASS)
C_ASSERT((sizeof(XSAVE_FORMAT) & (XSAVE_ALIGN - 1)) == 0);
C_ASSERT((FIELD_OFFSET(XSAVE_AREA, Header) & (XSAVE_ALIGN - 1)) == 0);
C_ASSERT(MINIMAL_XSTATE_AREA_LENGTH == 512 + 64);
#endif

typedef struct _CONTEXT_CHUNK {
  LONG Offset;
  ULONG Length;
} CONTEXT_CHUNK, *PCONTEXT_CHUNK;

typedef struct _CONTEXT_EX {
  CONTEXT_CHUNK All;
  CONTEXT_CHUNK Legacy;
  CONTEXT_CHUNK XState;
} CONTEXT_EX, *PCONTEXT_EX;

#define CONTEXT_EX_LENGTH         ALIGN_UP_BY(sizeof(CONTEXT_EX), STACK_ALIGN)

#if (NTDDI_VERSION >= NTDDI_VISTA)
extern NTSYSAPI volatile CCHAR KeNumberProcessors;
#elif (NTDDI_VERSION >= NTDDI_WINXP)
extern NTSYSAPI CCHAR KeNumberProcessors;
#else
extern PCCHAR KeNumberProcessors;
#endif


/******************************************************************************
 *                         Memory manager Types                               *
 ******************************************************************************/

#if (NTDDI_VERSION >= NTDDI_WIN2K)
typedef ULONG NODE_REQUIREMENT;
#define MM_ANY_NODE_OK                           0x80000000
#endif

#define MM_DONT_ZERO_ALLOCATION                  0x00000001
#define MM_ALLOCATE_FROM_LOCAL_NODE_ONLY         0x00000002
#define MM_ALLOCATE_FULLY_REQUIRED               0x00000004
#define MM_ALLOCATE_NO_WAIT                      0x00000008
#define MM_ALLOCATE_PREFER_CONTIGUOUS            0x00000010
#define MM_ALLOCATE_REQUIRE_CONTIGUOUS_CHUNKS    0x00000020

#define MDL_MAPPED_TO_SYSTEM_VA     0x0001
#define MDL_PAGES_LOCKED            0x0002
#define MDL_SOURCE_IS_NONPAGED_POOL 0x0004
#define MDL_ALLOCATED_FIXED_SIZE    0x0008
#define MDL_PARTIAL                 0x0010
#define MDL_PARTIAL_HAS_BEEN_MAPPED 0x0020
#define MDL_IO_PAGE_READ            0x0040
#define MDL_WRITE_OPERATION         0x0080
#define MDL_PARENT_MAPPED_SYSTEM_VA 0x0100
#define MDL_FREE_EXTRA_PTES         0x0200
#define MDL_DESCRIBES_AWE           0x0400
#define MDL_IO_SPACE                0x0800
#define MDL_NETWORK_HEADER          0x1000
#define MDL_MAPPING_CAN_FAIL        0x2000
#define MDL_ALLOCATED_MUST_SUCCEED  0x4000
#define MDL_INTERNAL                0x8000

#define MDL_MAPPING_FLAGS (MDL_MAPPED_TO_SYSTEM_VA     | \
                           MDL_PAGES_LOCKED            | \
                           MDL_SOURCE_IS_NONPAGED_POOL | \
                           MDL_PARTIAL_HAS_BEEN_MAPPED | \
                           MDL_PARENT_MAPPED_SYSTEM_VA | \
                           MDL_SYSTEM_VA               | \
                           MDL_IO_SPACE)

#define FLUSH_MULTIPLE_MAXIMUM       32

/* Section access rights */
#define SECTION_QUERY                0x0001
#define SECTION_MAP_WRITE            0x0002
#define SECTION_MAP_READ             0x0004
#define SECTION_MAP_EXECUTE          0x0008
#define SECTION_EXTEND_SIZE          0x0010
#define SECTION_MAP_EXECUTE_EXPLICIT 0x0020

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY| \
                            SECTION_MAP_WRITE |                     \
                            SECTION_MAP_READ |                      \
                            SECTION_MAP_EXECUTE |                   \
                            SECTION_EXTEND_SIZE)

#define SESSION_QUERY_ACCESS         0x0001
#define SESSION_MODIFY_ACCESS        0x0002

#define SESSION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED |  \
                            SESSION_QUERY_ACCESS     |  \
                            SESSION_MODIFY_ACCESS)

#define SEGMENT_ALL_ACCESS SECTION_ALL_ACCESS

#define PAGE_NOACCESS          0x01
#define PAGE_READONLY          0x02
#define PAGE_READWRITE         0x04
#define PAGE_WRITECOPY         0x08
#define PAGE_EXECUTE           0x10
#define PAGE_EXECUTE_READ      0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD            0x100
#define PAGE_NOCACHE          0x200
#define PAGE_WRITECOMBINE     0x400

#define MEM_COMMIT           0x1000
#define MEM_RESERVE          0x2000
#define MEM_DECOMMIT         0x4000
#define MEM_RELEASE          0x8000
#define MEM_FREE            0x10000
#define MEM_PRIVATE         0x20000
#define MEM_MAPPED          0x40000
#define MEM_RESET           0x80000
#define MEM_TOP_DOWN       0x100000
#define MEM_LARGE_PAGES  0x20000000
#define MEM_4MB_PAGES    0x80000000

#define SEC_RESERVE       0x4000000
#define SEC_COMMIT        0x8000000
#define SEC_LARGE_PAGES  0x80000000

/* Section map options */
typedef enum _SECTION_INHERIT {
  ViewShare = 1,
  ViewUnmap = 2
} SECTION_INHERIT;

typedef ULONG PFN_COUNT;
typedef LONG_PTR SPFN_NUMBER, *PSPFN_NUMBER;
typedef ULONG_PTR PFN_NUMBER, *PPFN_NUMBER;

typedef struct _MDL {
  struct _MDL *Next;
  CSHORT Size;
  CSHORT MdlFlags;
  struct _EPROCESS *Process;
  PVOID MappedSystemVa;
  PVOID StartVa;
  ULONG ByteCount;
  ULONG ByteOffset;
} MDL, *PMDL;
typedef MDL *PMDLX;

typedef enum _MEMORY_CACHING_TYPE_ORIG {
  MmFrameBufferCached = 2
} MEMORY_CACHING_TYPE_ORIG;

typedef enum _MEMORY_CACHING_TYPE {
  MmNonCached = FALSE,
  MmCached = TRUE,
  MmWriteCombined = MmFrameBufferCached,
  MmHardwareCoherentCached,
  MmNonCachedUnordered,
  MmUSWCCached,
  MmMaximumCacheType,
  MmNotMapped = -1
} MEMORY_CACHING_TYPE;

typedef enum _MM_PAGE_PRIORITY {
  LowPagePriority,
  NormalPagePriority = 16,
  HighPagePriority = 32
} MM_PAGE_PRIORITY;

typedef enum _MM_SYSTEM_SIZE {
  MmSmallSystem,
  MmMediumSystem,
  MmLargeSystem
} MM_SYSTEMSIZE;

extern NTKERNELAPI BOOLEAN Mm64BitPhysicalAddress;
extern PVOID MmBadPointer;


/******************************************************************************
 *                            Executive Types                                 *
 ******************************************************************************/
#define EX_RUNDOWN_ACTIVE                 0x1
#define EX_RUNDOWN_COUNT_SHIFT            0x1
#define EX_RUNDOWN_COUNT_INC              (1 << EX_RUNDOWN_COUNT_SHIFT)

typedef struct _FAST_MUTEX {
  volatile LONG Count;
  PKTHREAD Owner;
  ULONG Contention;
  KEVENT Event;
  ULONG OldIrql;
} FAST_MUTEX, *PFAST_MUTEX;

typedef enum _SUITE_TYPE {
  SmallBusiness,
  Enterprise,
  BackOffice,
  CommunicationServer,
  TerminalServer,
  SmallBusinessRestricted,
  EmbeddedNT,
  DataCenter,
  SingleUserTS,
  Personal,
  Blade,
  EmbeddedRestricted,
  SecurityAppliance,
  StorageServer,
  ComputeServer,
  WHServer,
  MaxSuiteType
} SUITE_TYPE;

typedef enum _EX_POOL_PRIORITY {
  LowPoolPriority,
  LowPoolPrioritySpecialPoolOverrun = 8,
  LowPoolPrioritySpecialPoolUnderrun = 9,
  NormalPoolPriority = 16,
  NormalPoolPrioritySpecialPoolOverrun = 24,
  NormalPoolPrioritySpecialPoolUnderrun = 25,
  HighPoolPriority = 32,
  HighPoolPrioritySpecialPoolOverrun = 40,
  HighPoolPrioritySpecialPoolUnderrun = 41
} EX_POOL_PRIORITY;

#if !defined(_WIN64) && (defined(_NTDDK_) || defined(_NTIFS_) || defined(_NDIS_))
#define LOOKASIDE_ALIGN
#else
#define LOOKASIDE_ALIGN DECLSPEC_CACHEALIGN
#endif

typedef struct _LOOKASIDE_LIST_EX *PLOOKASIDE_LIST_EX;

typedef PVOID
(NTAPI *PALLOCATE_FUNCTION)(
  IN POOL_TYPE PoolType,
  IN SIZE_T NumberOfBytes,
  IN ULONG Tag);

typedef PVOID
(NTAPI *PALLOCATE_FUNCTION_EX)(
  IN POOL_TYPE PoolType,
  IN SIZE_T NumberOfBytes,
  IN ULONG Tag,
  IN OUT PLOOKASIDE_LIST_EX Lookaside);

typedef VOID
(NTAPI *PFREE_FUNCTION)(
  IN PVOID Buffer);

typedef VOID
(NTAPI *PFREE_FUNCTION_EX)(
  IN PVOID Buffer,
  IN OUT PLOOKASIDE_LIST_EX Lookaside);

typedef VOID
(NTAPI CALLBACK_FUNCTION)(
  IN PVOID CallbackContext OPTIONAL,
  IN PVOID Argument1 OPTIONAL,
  IN PVOID Argument2 OPTIONAL);
typedef CALLBACK_FUNCTION *PCALLBACK_FUNCTION;

#define GENERAL_LOOKASIDE_LAYOUT                \
    _ANONYMOUS_UNION union {                    \
        SLIST_HEADER ListHead;                  \
        SINGLE_LIST_ENTRY SingleListHead;       \
    } DUMMYUNIONNAME;                           \
    USHORT Depth;                               \
    USHORT MaximumDepth;                        \
    ULONG TotalAllocates;                       \
    _ANONYMOUS_UNION union {                    \
        ULONG AllocateMisses;                   \
        ULONG AllocateHits;                     \
    } DUMMYUNIONNAME2;                          \
    ULONG TotalFrees;                           \
    _ANONYMOUS_UNION union {                    \
        ULONG FreeMisses;                       \
        ULONG FreeHits;                         \
    } DUMMYUNIONNAME3;                          \
    POOL_TYPE Type;                             \
    ULONG Tag;                                  \
    ULONG Size;                                 \
    _ANONYMOUS_UNION union {                    \
        PALLOCATE_FUNCTION_EX AllocateEx;       \
        PALLOCATE_FUNCTION Allocate;            \
    } DUMMYUNIONNAME4;                          \
    _ANONYMOUS_UNION union {                    \
        PFREE_FUNCTION_EX FreeEx;               \
        PFREE_FUNCTION Free;                    \
    } DUMMYUNIONNAME5;                          \
    LIST_ENTRY ListEntry;                       \
    ULONG LastTotalAllocates;                   \
    _ANONYMOUS_UNION union {                    \
        ULONG LastAllocateMisses;               \
        ULONG LastAllocateHits;                 \
    } DUMMYUNIONNAME6;                          \
    ULONG Future[2];

typedef struct LOOKASIDE_ALIGN _GENERAL_LOOKASIDE {
  GENERAL_LOOKASIDE_LAYOUT
} GENERAL_LOOKASIDE, *PGENERAL_LOOKASIDE;

typedef struct _GENERAL_LOOKASIDE_POOL {
  GENERAL_LOOKASIDE_LAYOUT
} GENERAL_LOOKASIDE_POOL, *PGENERAL_LOOKASIDE_POOL;

#define LOOKASIDE_CHECK(f)  \
    C_ASSERT(FIELD_OFFSET(GENERAL_LOOKASIDE,f) == FIELD_OFFSET(GENERAL_LOOKASIDE_POOL,f))

LOOKASIDE_CHECK(TotalFrees);
LOOKASIDE_CHECK(Tag);
LOOKASIDE_CHECK(Future);

typedef struct _PAGED_LOOKASIDE_LIST {
  GENERAL_LOOKASIDE L;
#if !defined(_AMD64_) && !defined(_IA64_)
  FAST_MUTEX Lock__ObsoleteButDoNotDelete;
#endif
} PAGED_LOOKASIDE_LIST, *PPAGED_LOOKASIDE_LIST;

typedef struct LOOKASIDE_ALIGN _NPAGED_LOOKASIDE_LIST {
  GENERAL_LOOKASIDE L;
#if !defined(_AMD64_) && !defined(_IA64_)
  KSPIN_LOCK Lock__ObsoleteButDoNotDelete;
#endif
} NPAGED_LOOKASIDE_LIST, *PNPAGED_LOOKASIDE_LIST;

#define LOOKASIDE_MINIMUM_BLOCK_SIZE (RTL_SIZEOF_THROUGH_FIELD (SLIST_ENTRY, Next))

typedef struct _LOOKASIDE_LIST_EX {
  GENERAL_LOOKASIDE_POOL L;
} LOOKASIDE_LIST_EX;

#if (NTDDI_VERSION >= NTDDI_VISTA)

#define EX_LOOKASIDE_LIST_EX_FLAGS_RAISE_ON_FAIL 0x00000001UL
#define EX_LOOKASIDE_LIST_EX_FLAGS_FAIL_NO_RAISE 0x00000002UL

#define EX_MAXIMUM_LOOKASIDE_DEPTH_BASE          256
#define EX_MAXIMUM_LOOKASIDE_DEPTH_LIMIT         1024

#endif /* (NTDDI_VERSION >= NTDDI_VISTA) */

typedef struct _EX_RUNDOWN_REF {
  _ANONYMOUS_UNION union {
    volatile ULONG_PTR Count;
    volatile PVOID Ptr;
  } DUMMYUNIONNAME;
} EX_RUNDOWN_REF, *PEX_RUNDOWN_REF;

typedef struct _EX_RUNDOWN_REF_CACHE_AWARE *PEX_RUNDOWN_REF_CACHE_AWARE;

typedef enum _WORK_QUEUE_TYPE {
  CriticalWorkQueue,
  DelayedWorkQueue,
  HyperCriticalWorkQueue,
  NormalWorkQueue,
  BackgroundWorkQueue,
  RealTimeWorkQueue,
  SuperCriticalWorkQueue,
  MaximumWorkQueue,
  CustomPriorityWorkQueue = 32
} WORK_QUEUE_TYPE;

typedef VOID
(NTAPI WORKER_THREAD_ROUTINE)(
  IN PVOID Parameter);
typedef WORKER_THREAD_ROUTINE *PWORKER_THREAD_ROUTINE;

typedef struct _WORK_QUEUE_ITEM {
  LIST_ENTRY List;
  PWORKER_THREAD_ROUTINE WorkerRoutine;
  volatile PVOID Parameter;
} WORK_QUEUE_ITEM, *PWORK_QUEUE_ITEM;

typedef ULONG_PTR ERESOURCE_THREAD, *PERESOURCE_THREAD;

typedef struct _OWNER_ENTRY {
  ERESOURCE_THREAD OwnerThread;
  _ANONYMOUS_UNION union {
    _ANONYMOUS_STRUCT struct {
      ULONG IoPriorityBoosted:1;
      ULONG OwnerReferenced:1;
      ULONG OwnerCount:30;
    } DUMMYSTRUCTNAME;
    ULONG TableSize;
  } DUMMYUNIONNAME;
} OWNER_ENTRY, *POWNER_ENTRY;

typedef struct _ERESOURCE {
  LIST_ENTRY SystemResourcesList;
  POWNER_ENTRY OwnerTable;
  SHORT ActiveCount;
  USHORT Flag;
  volatile PKSEMAPHORE SharedWaiters;
  volatile PKEVENT ExclusiveWaiters;
  OWNER_ENTRY OwnerEntry;
  ULONG ActiveEntries;
  ULONG ContentionCount;
  ULONG NumberOfSharedWaiters;
  ULONG NumberOfExclusiveWaiters;
#if defined(_WIN64)
  PVOID Reserved2;
#endif
  _ANONYMOUS_UNION union {
    PVOID Address;
    ULONG_PTR CreatorBackTraceIndex;
  } DUMMYUNIONNAME;
  KSPIN_LOCK SpinLock;
} ERESOURCE, *PERESOURCE;

/* ERESOURCE.Flag */
#define ResourceNeverExclusive            0x0010
#define ResourceReleaseByOtherThread      0x0020
#define ResourceOwnedExclusive            0x0080

#define RESOURCE_HASH_TABLE_SIZE          64

typedef struct _RESOURCE_HASH_ENTRY {
  LIST_ENTRY ListEntry;
  PVOID Address;
  ULONG ContentionCount;
  ULONG Number;
} RESOURCE_HASH_ENTRY, *PRESOURCE_HASH_ENTRY;

typedef struct _RESOURCE_PERFORMANCE_DATA {
  ULONG ActiveResourceCount;
  ULONG TotalResourceCount;
  ULONG ExclusiveAcquire;
  ULONG SharedFirstLevel;
  ULONG SharedSecondLevel;
  ULONG StarveFirstLevel;
  ULONG StarveSecondLevel;
  ULONG WaitForExclusive;
  ULONG OwnerTableExpands;
  ULONG MaximumTableExpand;
  LIST_ENTRY HashTable[RESOURCE_HASH_TABLE_SIZE];
} RESOURCE_PERFORMANCE_DATA, *PRESOURCE_PERFORMANCE_DATA;

/* Global debug flag */
#if DEVL
extern ULONG NtGlobalFlag;
#define IF_NTOS_DEBUG(FlagName) if (NtGlobalFlag & (FLG_##FlagName))
#else
#define IF_NTOS_DEBUG(FlagName) if(FALSE)
#endif

/******************************************************************************
 *                            Security Manager Types                          *
 ******************************************************************************/

/* Simple types */
typedef PVOID PSECURITY_DESCRIPTOR;
typedef ULONG SECURITY_INFORMATION, *PSECURITY_INFORMATION;
typedef ULONG ACCESS_MASK, *PACCESS_MASK;
typedef PVOID PACCESS_TOKEN;
typedef PVOID PSID;

#define DELETE                           0x00010000L
#define READ_CONTROL                     0x00020000L
#define WRITE_DAC                        0x00040000L
#define WRITE_OWNER                      0x00080000L
#define SYNCHRONIZE                      0x00100000L
#define STANDARD_RIGHTS_REQUIRED         0x000F0000L
#define STANDARD_RIGHTS_READ             READ_CONTROL
#define STANDARD_RIGHTS_WRITE            READ_CONTROL
#define STANDARD_RIGHTS_EXECUTE          READ_CONTROL
#define STANDARD_RIGHTS_ALL              0x001F0000L
#define SPECIFIC_RIGHTS_ALL              0x0000FFFFL
#define ACCESS_SYSTEM_SECURITY           0x01000000L
#define MAXIMUM_ALLOWED                  0x02000000L
#define GENERIC_READ                     0x80000000L
#define GENERIC_WRITE                    0x40000000L
#define GENERIC_EXECUTE                  0x20000000L
#define GENERIC_ALL                      0x10000000L

typedef struct _GENERIC_MAPPING {
  ACCESS_MASK GenericRead;
  ACCESS_MASK GenericWrite;
  ACCESS_MASK GenericExecute;
  ACCESS_MASK GenericAll;
} GENERIC_MAPPING, *PGENERIC_MAPPING;

#define ACL_REVISION                      2
#define ACL_REVISION_DS                   4

#define ACL_REVISION1                     1
#define ACL_REVISION2                     2
#define ACL_REVISION3                     3
#define ACL_REVISION4                     4
#define MIN_ACL_REVISION                  ACL_REVISION2
#define MAX_ACL_REVISION                  ACL_REVISION4

typedef struct _ACL {
  UCHAR AclRevision;
  UCHAR Sbz1;
  USHORT AclSize;
  USHORT AceCount;
  USHORT Sbz2;
} ACL, *PACL;

/* Current security descriptor revision value */
#define SECURITY_DESCRIPTOR_REVISION     (1)
#define SECURITY_DESCRIPTOR_REVISION1    (1)

/* Privilege attributes */
#define SE_PRIVILEGE_ENABLED_BY_DEFAULT (0x00000001L)
#define SE_PRIVILEGE_ENABLED            (0x00000002L)
#define SE_PRIVILEGE_REMOVED            (0X00000004L)
#define SE_PRIVILEGE_USED_FOR_ACCESS    (0x80000000L)

#define SE_PRIVILEGE_VALID_ATTRIBUTES   (SE_PRIVILEGE_ENABLED_BY_DEFAULT | \
                                         SE_PRIVILEGE_ENABLED            | \
                                         SE_PRIVILEGE_REMOVED            | \
                                         SE_PRIVILEGE_USED_FOR_ACCESS)

#include <pshpack4.h>
typedef struct _LUID_AND_ATTRIBUTES {
  LUID Luid;
  ULONG Attributes;
} LUID_AND_ATTRIBUTES, *PLUID_AND_ATTRIBUTES;
#include <poppack.h>

typedef LUID_AND_ATTRIBUTES LUID_AND_ATTRIBUTES_ARRAY[ANYSIZE_ARRAY];
typedef LUID_AND_ATTRIBUTES_ARRAY *PLUID_AND_ATTRIBUTES_ARRAY;

/* Privilege sets */
#define PRIVILEGE_SET_ALL_NECESSARY (1)

typedef struct _PRIVILEGE_SET {
  ULONG PrivilegeCount;
  ULONG Control;
  LUID_AND_ATTRIBUTES Privilege[ANYSIZE_ARRAY];
} PRIVILEGE_SET,*PPRIVILEGE_SET;

typedef enum _SECURITY_IMPERSONATION_LEVEL {
  SecurityAnonymous,
  SecurityIdentification,
  SecurityImpersonation,
  SecurityDelegation
} SECURITY_IMPERSONATION_LEVEL, * PSECURITY_IMPERSONATION_LEVEL;

#define SECURITY_MAX_IMPERSONATION_LEVEL SecurityDelegation
#define SECURITY_MIN_IMPERSONATION_LEVEL SecurityAnonymous
#define DEFAULT_IMPERSONATION_LEVEL SecurityImpersonation
#define VALID_IMPERSONATION_LEVEL(Level) (((Level) >= SECURITY_MIN_IMPERSONATION_LEVEL) && ((Level) <= SECURITY_MAX_IMPERSONATION_LEVEL))

#define SECURITY_DYNAMIC_TRACKING (TRUE)
#define SECURITY_STATIC_TRACKING (FALSE)

typedef BOOLEAN SECURITY_CONTEXT_TRACKING_MODE, *PSECURITY_CONTEXT_TRACKING_MODE;

typedef struct _SECURITY_QUALITY_OF_SERVICE {
  ULONG Length;
  SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
  SECURITY_CONTEXT_TRACKING_MODE ContextTrackingMode;
  BOOLEAN EffectiveOnly;
} SECURITY_QUALITY_OF_SERVICE, *PSECURITY_QUALITY_OF_SERVICE;

typedef struct _SE_IMPERSONATION_STATE {
  PACCESS_TOKEN Token;
  BOOLEAN CopyOnOpen;
  BOOLEAN EffectiveOnly;
  SECURITY_IMPERSONATION_LEVEL Level;
} SE_IMPERSONATION_STATE, *PSE_IMPERSONATION_STATE;

#define OWNER_SECURITY_INFORMATION       (0x00000001L)
#define GROUP_SECURITY_INFORMATION       (0x00000002L)
#define DACL_SECURITY_INFORMATION        (0x00000004L)
#define SACL_SECURITY_INFORMATION        (0x00000008L)
#define LABEL_SECURITY_INFORMATION       (0x00000010L)

#define PROTECTED_DACL_SECURITY_INFORMATION     (0x80000000L)
#define PROTECTED_SACL_SECURITY_INFORMATION     (0x40000000L)
#define UNPROTECTED_DACL_SECURITY_INFORMATION   (0x20000000L)
#define UNPROTECTED_SACL_SECURITY_INFORMATION   (0x10000000L)

typedef enum _SECURITY_OPERATION_CODE {
  SetSecurityDescriptor,
  QuerySecurityDescriptor,
  DeleteSecurityDescriptor,
  AssignSecurityDescriptor
} SECURITY_OPERATION_CODE, *PSECURITY_OPERATION_CODE;

#define INITIAL_PRIVILEGE_COUNT           3

typedef struct _INITIAL_PRIVILEGE_SET {
  ULONG PrivilegeCount;
  ULONG Control;
  LUID_AND_ATTRIBUTES Privilege[INITIAL_PRIVILEGE_COUNT];
} INITIAL_PRIVILEGE_SET, * PINITIAL_PRIVILEGE_SET;

#define SE_MIN_WELL_KNOWN_PRIVILEGE         2
#define SE_CREATE_TOKEN_PRIVILEGE           2
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE     3
#define SE_LOCK_MEMORY_PRIVILEGE            4
#define SE_INCREASE_QUOTA_PRIVILEGE         5
#define SE_MACHINE_ACCOUNT_PRIVILEGE        6
#define SE_TCB_PRIVILEGE                    7
#define SE_SECURITY_PRIVILEGE               8
#define SE_TAKE_OWNERSHIP_PRIVILEGE         9
#define SE_LOAD_DRIVER_PRIVILEGE            10
#define SE_SYSTEM_PROFILE_PRIVILEGE         11
#define SE_SYSTEMTIME_PRIVILEGE             12
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE    13
#define SE_INC_BASE_PRIORITY_PRIVILEGE      14
#define SE_CREATE_PAGEFILE_PRIVILEGE        15
#define SE_CREATE_PERMANENT_PRIVILEGE       16
#define SE_BACKUP_PRIVILEGE                 17
#define SE_RESTORE_PRIVILEGE                18
#define SE_SHUTDOWN_PRIVILEGE               19
#define SE_DEBUG_PRIVILEGE                  20
#define SE_AUDIT_PRIVILEGE                  21
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE     22
#define SE_CHANGE_NOTIFY_PRIVILEGE          23
#define SE_REMOTE_SHUTDOWN_PRIVILEGE        24
#define SE_UNDOCK_PRIVILEGE                 25
#define SE_SYNC_AGENT_PRIVILEGE             26
#define SE_ENABLE_DELEGATION_PRIVILEGE      27
#define SE_MANAGE_VOLUME_PRIVILEGE          28
#define SE_IMPERSONATE_PRIVILEGE            29
#define SE_CREATE_GLOBAL_PRIVILEGE          30
#define SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE 31
#define SE_RELABEL_PRIVILEGE                32
#define SE_INC_WORKING_SET_PRIVILEGE        33
#define SE_TIME_ZONE_PRIVILEGE              34
#define SE_CREATE_SYMBOLIC_LINK_PRIVILEGE   35
#define SE_MAX_WELL_KNOWN_PRIVILEGE         SE_CREATE_SYMBOLIC_LINK_PRIVILEGE

typedef struct _SECURITY_SUBJECT_CONTEXT {
  PACCESS_TOKEN ClientToken;
  SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
  PACCESS_TOKEN PrimaryToken;
  PVOID ProcessAuditId;
} SECURITY_SUBJECT_CONTEXT, *PSECURITY_SUBJECT_CONTEXT;

typedef struct _ACCESS_STATE {
  LUID OperationID;
  BOOLEAN SecurityEvaluated;
  BOOLEAN GenerateAudit;
  BOOLEAN GenerateOnClose;
  BOOLEAN PrivilegesAllocated;
  ULONG Flags;
  ACCESS_MASK RemainingDesiredAccess;
  ACCESS_MASK PreviouslyGrantedAccess;
  ACCESS_MASK OriginalDesiredAccess;
  SECURITY_SUBJECT_CONTEXT SubjectSecurityContext;
  PSECURITY_DESCRIPTOR SecurityDescriptor;
  PVOID AuxData;
  union {
    INITIAL_PRIVILEGE_SET InitialPrivilegeSet;
    PRIVILEGE_SET PrivilegeSet;
  } Privileges;
  BOOLEAN AuditPrivileges;
  UNICODE_STRING ObjectName;
  UNICODE_STRING ObjectTypeName;
} ACCESS_STATE, *PACCESS_STATE;

typedef VOID
(NTAPI *PNTFS_DEREF_EXPORTED_SECURITY_DESCRIPTOR)(
  IN PVOID Vcb,
  IN PSECURITY_DESCRIPTOR SecurityDescriptor);

#ifndef _NTLSA_IFS_

#ifndef _NTLSA_AUDIT_
#define _NTLSA_AUDIT_

#define SE_MAX_AUDIT_PARAMETERS 32
#define SE_MAX_GENERIC_AUDIT_PARAMETERS 28

#define SE_ADT_OBJECT_ONLY 0x1

#define SE_ADT_PARAMETERS_SELF_RELATIVE    0x00000001
#define SE_ADT_PARAMETERS_SEND_TO_LSA      0x00000002
#define SE_ADT_PARAMETER_EXTENSIBLE_AUDIT  0x00000004
#define SE_ADT_PARAMETER_GENERIC_AUDIT     0x00000008
#define SE_ADT_PARAMETER_WRITE_SYNCHRONOUS 0x00000010

#define LSAP_SE_ADT_PARAMETER_ARRAY_TRUE_SIZE(Parameters) \
  ( sizeof(SE_ADT_PARAMETER_ARRAY) - sizeof(SE_ADT_PARAMETER_ARRAY_ENTRY) * \
    (SE_MAX_AUDIT_PARAMETERS - Parameters->ParameterCount) )

typedef enum _SE_ADT_PARAMETER_TYPE {
  SeAdtParmTypeNone = 0,
  SeAdtParmTypeString,
  SeAdtParmTypeFileSpec,
  SeAdtParmTypeUlong,
  SeAdtParmTypeSid,
  SeAdtParmTypeLogonId,
  SeAdtParmTypeNoLogonId,
  SeAdtParmTypeAccessMask,
  SeAdtParmTypePrivs,
  SeAdtParmTypeObjectTypes,
  SeAdtParmTypeHexUlong,
  SeAdtParmTypePtr,
  SeAdtParmTypeTime,
  SeAdtParmTypeGuid,
  SeAdtParmTypeLuid,
  SeAdtParmTypeHexInt64,
  SeAdtParmTypeStringList,
  SeAdtParmTypeSidList,
  SeAdtParmTypeDuration,
  SeAdtParmTypeUserAccountControl,
  SeAdtParmTypeNoUac,
  SeAdtParmTypeMessage,
  SeAdtParmTypeDateTime,
  SeAdtParmTypeSockAddr,
  SeAdtParmTypeSD,
  SeAdtParmTypeLogonHours,
  SeAdtParmTypeLogonIdNoSid,
  SeAdtParmTypeUlongNoConv,
  SeAdtParmTypeSockAddrNoPort,
  SeAdtParmTypeAccessReason,
  SeAdtParmTypeStagingReason,
  SeAdtParmTypeResourceAttribute,
  SeAdtParmTypeClaims,
  SeAdtParmTypeLogonIdAsSid,
  SeAdtParmTypeMultiSzString,
  SeAdtParmTypeLogonIdEx
} SE_ADT_PARAMETER_TYPE, *PSE_ADT_PARAMETER_TYPE;

typedef struct _SE_ADT_OBJECT_TYPE {
  GUID ObjectType;
  USHORT Flags;
  USHORT Level;
  ACCESS_MASK AccessMask;
} SE_ADT_OBJECT_TYPE, *PSE_ADT_OBJECT_TYPE;

typedef struct _SE_ADT_PARAMETER_ARRAY_ENTRY {
  SE_ADT_PARAMETER_TYPE Type;
  ULONG Length;
  ULONG_PTR Data[2];
  PVOID Address;
} SE_ADT_PARAMETER_ARRAY_ENTRY, *PSE_ADT_PARAMETER_ARRAY_ENTRY;

typedef struct _SE_ADT_ACCESS_REASON {
  ACCESS_MASK AccessMask;
  ULONG AccessReasons[32];
  ULONG ObjectTypeIndex;
  ULONG AccessGranted;
  PSECURITY_DESCRIPTOR SecurityDescriptor;
} SE_ADT_ACCESS_REASON, *PSE_ADT_ACCESS_REASON;

typedef struct _SE_ADT_PARAMETER_ARRAY {
  ULONG CategoryId;
  ULONG AuditId;
  ULONG ParameterCount;
  ULONG Length;
  USHORT FlatSubCategoryId;
  USHORT Type;
  ULONG Flags;
  SE_ADT_PARAMETER_ARRAY_ENTRY Parameters[ SE_MAX_AUDIT_PARAMETERS ];
} SE_ADT_PARAMETER_ARRAY, *PSE_ADT_PARAMETER_ARRAY;

#endif /* !_NTLSA_AUDIT_ */
#endif /* !_NTLSA_IFS_ */

/******************************************************************************
 *                            Power Management Support Types                  *
 ******************************************************************************/

#ifndef _PO_DDK_
#define _PO_DDK_

#define PO_CB_SYSTEM_POWER_POLICY                0
#define PO_CB_AC_STATUS                          1
#define PO_CB_BUTTON_COLLISION                   2
#define PO_CB_SYSTEM_STATE_LOCK                  3
#define PO_CB_LID_SWITCH_STATE                   4
#define PO_CB_PROCESSOR_POWER_POLICY             5

/* Power States/Levels */
typedef enum _SYSTEM_POWER_STATE {
  PowerSystemUnspecified = 0,
  PowerSystemWorking,
  PowerSystemSleeping1,
  PowerSystemSleeping2,
  PowerSystemSleeping3,
  PowerSystemHibernate,
  PowerSystemShutdown,
  PowerSystemMaximum
} SYSTEM_POWER_STATE, *PSYSTEM_POWER_STATE;

#define POWER_SYSTEM_MAXIMUM PowerSystemMaximum

typedef enum _POWER_INFORMATION_LEVEL {
  SystemPowerPolicyAc,
  SystemPowerPolicyDc,
  VerifySystemPolicyAc,
  VerifySystemPolicyDc,
  SystemPowerCapabilities,
  SystemBatteryState,
  SystemPowerStateHandler,
  ProcessorStateHandler,
  SystemPowerPolicyCurrent,
  AdministratorPowerPolicy,
  SystemReserveHiberFile,
  ProcessorInformation,
  SystemPowerInformation,
  ProcessorStateHandler2,
  LastWakeTime,
  LastSleepTime,
  SystemExecutionState,
  SystemPowerStateNotifyHandler,
  ProcessorPowerPolicyAc,
  ProcessorPowerPolicyDc,
  VerifyProcessorPowerPolicyAc,
  VerifyProcessorPowerPolicyDc,
  ProcessorPowerPolicyCurrent,
  SystemPowerStateLogging,
  SystemPowerLoggingEntry,
  SetPowerSettingValue,
  NotifyUserPowerSetting,
  PowerInformationLevelUnused0,
  SystemMonitorHiberBootPowerOff,
  SystemVideoState,
  TraceApplicationPowerMessage,
  TraceApplicationPowerMessageEnd,
  ProcessorPerfStates,
  ProcessorIdleStates,
  ProcessorCap,
  SystemWakeSource,
  SystemHiberFileInformation,
  TraceServicePowerMessage,
  ProcessorLoad,
  PowerShutdownNotification,
  MonitorCapabilities,
  SessionPowerInit,
  SessionDisplayState,
  PowerRequestCreate,
  PowerRequestAction,
  GetPowerRequestList,
  ProcessorInformationEx,
  NotifyUserModeLegacyPowerEvent,
  GroupPark,
  ProcessorIdleDomains,
  WakeTimerList,
  SystemHiberFileSize,
  ProcessorIdleStatesHv,
  ProcessorPerfStatesHv,
  ProcessorPerfCapHv,
  ProcessorSetIdle,
  LogicalProcessorIdling,
  UserPresence,
  PowerSettingNotificationName,
  GetPowerSettingValue,
  IdleResiliency,
  SessionRITState,
  SessionConnectNotification,
  SessionPowerCleanup,
  SessionLockState,
  SystemHiberbootState,
  PlatformInformation,
  PdcInvocation,
  MonitorInvocation,
  FirmwareTableInformationRegistered,
  SetShutdownSelectedTime,
  SuspendResumeInvocation,
  PlmPowerRequestCreate,
  ScreenOff,
  CsDeviceNotification,
  PlatformRole,
  LastResumePerformance,
  DisplayBurst,
  ExitLatencySamplingPercentage,
  RegisterSpmPowerSettings,
  PlatformIdleStates,
  ProcessorIdleVeto,
  PlatformIdleVeto,
  SystemBatteryStatePrecise,
  ThermalEvent,
  PowerRequestActionInternal,
  BatteryDeviceState,
  PowerInformationInternal,
  ThermalStandby,
  SystemHiberFileType,
  PhysicalPowerButtonPress,
  QueryPotentialDripsConstraint,
  EnergyTrackerCreate,
  EnergyTrackerQuery,
  UpdateBlackBoxRecorder,
  SessionAllowExternalDmaDevices,
  PowerInformationLevelMaximum
} POWER_INFORMATION_LEVEL;

typedef enum {
  PowerActionNone = 0,
  PowerActionReserved,
  PowerActionSleep,
  PowerActionHibernate,
  PowerActionShutdown,
  PowerActionShutdownReset,
  PowerActionShutdownOff,
  PowerActionWarmEject,
  PowerActionDisplayOff
} POWER_ACTION, *PPOWER_ACTION;

typedef enum _DEVICE_POWER_STATE {
  PowerDeviceUnspecified = 0,
  PowerDeviceD0,
  PowerDeviceD1,
  PowerDeviceD2,
  PowerDeviceD3,
  PowerDeviceMaximum
} DEVICE_POWER_STATE, *PDEVICE_POWER_STATE;

typedef enum _MONITOR_DISPLAY_STATE {
  PowerMonitorOff = 0,
  PowerMonitorOn,
  PowerMonitorDim
} MONITOR_DISPLAY_STATE, *PMONITOR_DISPLAY_STATE;

typedef union _POWER_STATE {
  SYSTEM_POWER_STATE SystemState;
  DEVICE_POWER_STATE DeviceState;
} POWER_STATE, *PPOWER_STATE;

typedef enum _POWER_STATE_TYPE {
  SystemPowerState = 0,
  DevicePowerState
} POWER_STATE_TYPE, *PPOWER_STATE_TYPE;

#if (NTDDI_VERSION >= NTDDI_VISTA)
typedef struct _SYSTEM_POWER_STATE_CONTEXT {
  _ANONYMOUS_UNION union {
    _ANONYMOUS_STRUCT struct {
      ULONG Reserved1:8;
      ULONG TargetSystemState:4;
      ULONG EffectiveSystemState:4;
      ULONG CurrentSystemState:4;
      ULONG IgnoreHibernationPath:1;
      ULONG PseudoTransition:1;
      ULONG Reserved2:10;
    } DUMMYSTRUCTNAME;
    ULONG ContextAsUlong;
  } DUMMYUNIONNAME;
} SYSTEM_POWER_STATE_CONTEXT, *PSYSTEM_POWER_STATE_CONTEXT;
#endif

#if (NTDDI_VERSION >= NTDDI_WIN7)
typedef struct _COUNTED_REASON_CONTEXT {
  ULONG Version;
  ULONG Flags;
  _ANONYMOUS_UNION union {
    _ANONYMOUS_STRUCT struct {
      UNICODE_STRING ResourceFileName;
      USHORT ResourceReasonId;
      ULONG StringCount;
      PUNICODE_STRING ReasonStrings;
    } DUMMYSTRUCTNAME;
    UNICODE_STRING SimpleString;
  } DUMMYUNIONNAME;
} COUNTED_REASON_CONTEXT, *PCOUNTED_REASON_CONTEXT;
#endif

#define IOCTL_QUERY_DEVICE_POWER_STATE  \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x0, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_SET_DEVICE_WAKE           \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x1, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_CANCEL_DEVICE_WAKE        \
        CTL_CODE(FILE_DEVICE_BATTERY, 0x2, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define ES_SYSTEM_REQUIRED                       0x00000001
#define ES_DISPLAY_REQUIRED                      0x00000002
#define ES_USER_PRESENT                          0x00000004
#define ES_CONTINUOUS                            0x80000000

typedef ULONG EXECUTION_STATE, *PEXECUTION_STATE;

typedef enum {
  LT_DONT_CARE,
  LT_LOWEST_LATENCY
} LATENCY_TIME;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN7)
#define DIAGNOSTIC_REASON_VERSION                0
#define DIAGNOSTIC_REASON_SIMPLE_STRING          0x00000001
#define DIAGNOSTIC_REASON_DETAILED_STRING        0x00000002
#define DIAGNOSTIC_REASON_NOT_SPECIFIED          0x80000000
#define DIAGNOSTIC_REASON_INVALID_FLAGS          (~0x80000003)
#endif

#define POWER_REQUEST_CONTEXT_VERSION            0
#define POWER_REQUEST_CONTEXT_SIMPLE_STRING      0x00000001
#define POWER_REQUEST_CONTEXT_DETAILED_STRING    0x00000002

#define PowerRequestMaximum                      3

typedef enum _POWER_REQUEST_TYPE {
  PowerRequestDisplayRequired,
  PowerRequestSystemRequired,
  PowerRequestAwayModeRequired,
  PowerRequestExecutionRequired
} POWER_REQUEST_TYPE, *PPOWER_REQUEST_TYPE;

#if (NTDDI_VERSION >= NTDDI_WINXP)

#define PDCAP_D0_SUPPORTED                       0x00000001
#define PDCAP_D1_SUPPORTED                       0x00000002
#define PDCAP_D2_SUPPORTED                       0x00000004
#define PDCAP_D3_SUPPORTED                       0x00000008
#define PDCAP_WAKE_FROM_D0_SUPPORTED             0x00000010
#define PDCAP_WAKE_FROM_D1_SUPPORTED             0x00000020
#define PDCAP_WAKE_FROM_D2_SUPPORTED             0x00000040
#define PDCAP_WAKE_FROM_D3_SUPPORTED             0x00000080
#define PDCAP_WARM_EJECT_SUPPORTED               0x00000100

typedef struct CM_Power_Data_s {
  ULONG PD_Size;
  DEVICE_POWER_STATE PD_MostRecentPowerState;
  ULONG PD_Capabilities;
  ULONG PD_D1Latency;
  ULONG PD_D2Latency;
  ULONG PD_D3Latency;
  DEVICE_POWER_STATE PD_PowerStateMapping[PowerSystemMaximum];
  SYSTEM_POWER_STATE PD_DeepestSystemWake;
} CM_POWER_DATA, *PCM_POWER_DATA;

#endif /* (NTDDI_VERSION >= NTDDI_WINXP) */

typedef enum _SYSTEM_POWER_CONDITION {
  PoAc,
  PoDc,
  PoHot,
  PoConditionMaximum
} SYSTEM_POWER_CONDITION;

typedef struct _SET_POWER_SETTING_VALUE {
  ULONG Version;
  GUID Guid;
  SYSTEM_POWER_CONDITION PowerCondition;
  ULONG DataLength;
  UCHAR Data[ANYSIZE_ARRAY];
} SET_POWER_SETTING_VALUE, *PSET_POWER_SETTING_VALUE;

#define POWER_SETTING_VALUE_VERSION              (0x1)

typedef struct _NOTIFY_USER_POWER_SETTING {
  GUID Guid;
} NOTIFY_USER_POWER_SETTING, *PNOTIFY_USER_POWER_SETTING;

typedef struct _APPLICATIONLAUNCH_SETTING_VALUE {
  LARGE_INTEGER ActivationTime;
  ULONG Flags;
  ULONG ButtonInstanceID;
} APPLICATIONLAUNCH_SETTING_VALUE, *PAPPLICATIONLAUNCH_SETTING_VALUE;

typedef enum _POWER_PLATFORM_ROLE {
  PlatformRoleUnspecified = 0,
  PlatformRoleDesktop,
  PlatformRoleMobile,
  PlatformRoleWorkstation,
  PlatformRoleEnterpriseServer,
  PlatformRoleSOHOServer,
  PlatformRoleAppliancePC,
  PlatformRolePerformanceServer,
  PlatformRoleSlate,
  PlatformRoleMaximum
} POWER_PLATFORM_ROLE;

#if (NTDDI_VERSION >= NTDDI_WINXP) || !defined(_BATCLASS_)
typedef struct {
  ULONG Granularity;
  ULONG Capacity;
} BATTERY_REPORTING_SCALE, *PBATTERY_REPORTING_SCALE;
#endif /* (NTDDI_VERSION >= NTDDI_WINXP) || !defined(_BATCLASS_) */

#endif /* !_PO_DDK_ */

#define CORE_PARKING_POLICY_CHANGE_IDEAL         0
#define CORE_PARKING_POLICY_CHANGE_SINGLE        1
#define CORE_PARKING_POLICY_CHANGE_ROCKET        2
#define CORE_PARKING_POLICY_CHANGE_MAX           CORE_PARKING_POLICY_CHANGE_ROCKET

DEFINE_GUID(GUID_MAX_POWER_SAVINGS, 0xA1841308, 0x3541, 0x4FAB, 0xBC, 0x81, 0xF7, 0x15, 0x56, 0xF2, 0x0B, 0x4A);
DEFINE_GUID(GUID_MIN_POWER_SAVINGS, 0x8C5E7FDA, 0xE8BF, 0x4A96, 0x9A, 0x85, 0xA6, 0xE2, 0x3A, 0x8C, 0x63, 0x5C);
DEFINE_GUID(GUID_TYPICAL_POWER_SAVINGS, 0x381B4222, 0xF694, 0x41F0, 0x96, 0x85, 0xFF, 0x5B, 0xB2, 0x60, 0xDF, 0x2E);
DEFINE_GUID(NO_SUBGROUP_GUID, 0xFEA3413E, 0x7E05, 0x4911, 0x9A, 0x71, 0x70, 0x03, 0x31, 0xF1, 0xC2, 0x94);
DEFINE_GUID(ALL_POWERSCHEMES_GUID, 0x68A1E95E, 0x13EA, 0x41E1, 0x80, 0x11, 0x0C, 0x49, 0x6C, 0xA4, 0x90, 0xB0);
DEFINE_GUID(GUID_POWERSCHEME_PERSONALITY, 0x245D8541, 0x3943, 0x4422, 0xB0, 0x25, 0x13, 0xA7, 0x84, 0xF6, 0x79, 0xB7);
DEFINE_GUID(GUID_ACTIVE_POWERSCHEME, 0x31F9F286, 0x5084, 0x42FE, 0xB7, 0x20, 0x2B, 0x02, 0x64, 0x99, 0x37, 0x63);
DEFINE_GUID(GUID_VIDEO_SUBGROUP, 0x7516B95F, 0xF776, 0x4464, 0x8C, 0x53, 0x06, 0x16, 0x7F, 0x40, 0xCC, 0x99);
DEFINE_GUID(GUID_VIDEO_POWERDOWN_TIMEOUT, 0x3C0BC021, 0xC8A8, 0x4E07, 0xA9, 0x73, 0x6B, 0x14, 0xCB, 0xCB, 0x2B, 0x7E);
DEFINE_GUID(GUID_VIDEO_ANNOYANCE_TIMEOUT, 0x82DBCF2D, 0xCD67, 0x40C5, 0xBF, 0xDC, 0x9F, 0x1A, 0x5C, 0xCD, 0x46, 0x63);
DEFINE_GUID(GUID_VIDEO_ADAPTIVE_PERCENT_INCREASE, 0xEED904DF, 0xB142, 0x4183, 0xB1, 0x0B, 0x5A, 0x11, 0x97, 0xA3, 0x78, 0x64);
DEFINE_GUID(GUID_VIDEO_DIM_TIMEOUT, 0x17aaa29b, 0x8b43, 0x4b94, 0xaa, 0xfe, 0x35, 0xf6, 0x4d, 0xaa, 0xf1, 0xee);
DEFINE_GUID(GUID_VIDEO_ADAPTIVE_POWERDOWN, 0x90959D22, 0xD6A1, 0x49B9, 0xAF, 0x93, 0xBC, 0xE8, 0x85, 0xAD, 0x33, 0x5B);
DEFINE_GUID(GUID_MONITOR_POWER_ON, 0x02731015, 0x4510, 0x4526, 0x99, 0xE6, 0xE5, 0xA1, 0x7E, 0xBD, 0x1A, 0xEA);
DEFINE_GUID(GUID_DEVICE_POWER_POLICY_VIDEO_BRIGHTNESS, 0xaded5e82L, 0xb909, 0x4619, 0x99, 0x49, 0xf5, 0xd7, 0x1d, 0xac, 0x0b, 0xcb);
DEFINE_GUID(GUID_DEVICE_POWER_POLICY_VIDEO_DIM_BRIGHTNESS, 0xf1fbfde2, 0xa960, 0x4165, 0x9f, 0x88, 0x50, 0x66, 0x79, 0x11, 0xce, 0x96);
DEFINE_GUID(GUID_VIDEO_CURRENT_MONITOR_BRIGHTNESS, 0x8ffee2c6, 0x2d01, 0x46be, 0xad, 0xb9, 0x39, 0x8a, 0xdd, 0xc5, 0xb4, 0xff);
DEFINE_GUID(GUID_VIDEO_ADAPTIVE_DISPLAY_BRIGHTNESS, 0xFBD9AA66, 0x9553, 0x4097, 0xBA, 0x44, 0xED, 0x6E, 0x9D, 0x65, 0xEA, 0xB8);
DEFINE_GUID(GUID_SESSION_DISPLAY_STATE, 0x73A5E93A, 0x5BB1, 0x4F93, 0x89, 0x5B, 0xDB, 0xD0, 0xDA, 0x85, 0x59, 0x67);
DEFINE_GUID(GUID_CONSOLE_DISPLAY_STATE, 0x6fe69556, 0x704a, 0x47a0, 0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47);
DEFINE_GUID(GUID_ALLOW_DISPLAY_REQUIRED, 0xA9CEB8DA, 0xCD46, 0x44FB, 0xA9, 0x8B, 0x02, 0xAF, 0x69, 0xDE, 0x46, 0x23);
DEFINE_GUID(GUID_DISK_SUBGROUP, 0x0012EE47, 0x9041, 0x4B5D, 0x9B, 0x77, 0x53, 0x5F, 0xBA, 0x8B, 0x14, 0x42);
DEFINE_GUID(GUID_DISK_POWERDOWN_TIMEOUT, 0x6738E2C4, 0xE8A5, 0x4A42, 0xB1, 0x6A, 0xE0, 0x40, 0xE7, 0x69, 0x75, 0x6E);
DEFINE_GUID(GUID_DISK_BURST_IGNORE_THRESHOLD, 0x80e3c60e, 0xbb94, 0x4ad8, 0xbb, 0xe0, 0x0d, 0x31, 0x95, 0xef, 0xc6, 0x63);
DEFINE_GUID(GUID_DISK_ADAPTIVE_POWERDOWN, 0x396A32E1, 0x499A, 0x40B2, 0x91, 0x24, 0xA9, 0x6A, 0xFE, 0x70, 0x76, 0x67);
DEFINE_GUID(GUID_SLEEP_SUBGROUP, 0x238C9FA8, 0x0AAD, 0x41ED, 0x83, 0xF4, 0x97, 0xBE, 0x24, 0x2C, 0x8F, 0x20);
DEFINE_GUID(GUID_SLEEP_IDLE_THRESHOLD, 0x81cd32e0, 0x7833, 0x44f3, 0x87, 0x37, 0x70, 0x81, 0xf3, 0x8d, 0x1f, 0x70);
DEFINE_GUID(GUID_STANDBY_TIMEOUT, 0x29F6C1DB, 0x86DA, 0x48C5, 0x9F, 0xDB, 0xF2, 0xB6, 0x7B, 0x1F, 0x44, 0xDA);
DEFINE_GUID(GUID_UNATTEND_SLEEP_TIMEOUT, 0x7bc4a2f9, 0xd8fc, 0x4469, 0xb0, 0x7b, 0x33, 0xeb, 0x78, 0x5a, 0xac, 0xa0);
DEFINE_GUID(GUID_HIBERNATE_TIMEOUT, 0x9D7815A6, 0x7EE4, 0x497E, 0x88, 0x88, 0x51, 0x5A, 0x05, 0xF0, 0x23, 0x64);
DEFINE_GUID(GUID_HIBERNATE_FASTS4_POLICY, 0x94AC6D29, 0x73CE, 0x41A6, 0x80, 0x9F, 0x63, 0x63, 0xBA, 0x21, 0xB4, 0x7E);
DEFINE_GUID(GUID_CRITICAL_POWER_TRANSITION,  0xB7A27025, 0xE569, 0x46c2, 0xA5, 0x04, 0x2B, 0x96, 0xCA, 0xD2, 0x25, 0xA1);
DEFINE_GUID(GUID_SYSTEM_AWAYMODE, 0x98A7F580, 0x01F7, 0x48AA, 0x9C, 0x0F, 0x44, 0x35, 0x2C, 0x29, 0xE5, 0xC0);
DEFINE_GUID(GUID_ALLOW_AWAYMODE, 0x25dfa149, 0x5dd1, 0x4736, 0xb5, 0xab, 0xe8, 0xa3, 0x7b, 0x5b, 0x81, 0x87);
DEFINE_GUID(GUID_ALLOW_STANDBY_STATES, 0xabfc2519, 0x3608, 0x4c2a, 0x94, 0xea, 0x17, 0x1b, 0x0e, 0xd5, 0x46, 0xab);
DEFINE_GUID(GUID_ALLOW_RTC_WAKE, 0xBD3B718A, 0x0680, 0x4D9D, 0x8A, 0xB2, 0xE1, 0xD2, 0xB4, 0xAC, 0x80, 0x6D);
DEFINE_GUID(GUID_ALLOW_SYSTEM_REQUIRED, 0xA4B195F5, 0x8225, 0x47D8, 0x80, 0x12, 0x9D, 0x41, 0x36, 0x97, 0x86, 0xE2);
DEFINE_GUID(GUID_SYSTEM_BUTTON_SUBGROUP, 0x4F971E89, 0xEEBD, 0x4455, 0xA8, 0xDE, 0x9E, 0x59, 0x04, 0x0E, 0x73, 0x47);
DEFINE_GUID(GUID_POWERBUTTON_ACTION, 0x7648EFA3, 0xDD9C, 0x4E3E, 0xB5, 0x66, 0x50, 0xF9, 0x29, 0x38, 0x62, 0x80);
DEFINE_GUID(GUID_POWERBUTTON_ACTION_FLAGS, 0x857E7FAC, 0x034B, 0x4704, 0xAB, 0xB1, 0xBC, 0xA5, 0x4A, 0xA3, 0x14, 0x78);
DEFINE_GUID(GUID_SLEEPBUTTON_ACTION, 0x96996BC0, 0xAD50, 0x47EC, 0x92, 0x3B, 0x6F, 0x41, 0x87, 0x4D, 0xD9, 0xEB);
DEFINE_GUID(GUID_SLEEPBUTTON_ACTION_FLAGS, 0x2A160AB1, 0xB69D, 0x4743, 0xB7, 0x18, 0xBF, 0x14, 0x41, 0xD5, 0xE4, 0x93);
DEFINE_GUID(GUID_USERINTERFACEBUTTON_ACTION, 0xA7066653, 0x8D6C, 0x40A8, 0x91, 0x0E, 0xA1, 0xF5, 0x4B, 0x84, 0xC7, 0xE5);
DEFINE_GUID(GUID_LIDCLOSE_ACTION, 0x5CA83367, 0x6E45, 0x459F, 0xA2, 0x7B, 0x47, 0x6B, 0x1D, 0x01, 0xC9, 0x36);
DEFINE_GUID(GUID_LIDCLOSE_ACTION_FLAGS, 0x97E969AC, 0x0D6C, 0x4D08, 0x92, 0x7C, 0xD7, 0xBD, 0x7A, 0xD7, 0x85, 0x7B);
DEFINE_GUID(GUID_LIDOPEN_POWERSTATE, 0x99FF10E7, 0x23B1, 0x4C07, 0xA9, 0xD1, 0x5C, 0x32, 0x06, 0xD7, 0x41, 0xB4);
DEFINE_GUID(GUID_BATTERY_SUBGROUP, 0xE73A048D, 0xBF27, 0x4F12, 0x97, 0x31, 0x8B, 0x20, 0x76, 0xE8, 0x89, 0x1F);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_ACTION_0, 0x637EA02F, 0xBBCB, 0x4015, 0x8E, 0x2C, 0xA1, 0xC7, 0xB9, 0xC0, 0xB5, 0x46);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_LEVEL_0, 0x9A66D8D7, 0x4FF7, 0x4EF9, 0xB5, 0xA2, 0x5A, 0x32, 0x6C, 0xA2, 0xA4, 0x69);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_FLAGS_0, 0x5dbb7c9f, 0x38e9, 0x40d2, 0x97, 0x49, 0x4f, 0x8a, 0x0e, 0x9f, 0x64, 0x0f);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_ACTION_1, 0xD8742DCB, 0x3E6A, 0x4B3C, 0xB3, 0xFE, 0x37, 0x46, 0x23, 0xCD, 0xCF, 0x06);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_LEVEL_1, 0x8183BA9A, 0xE910, 0x48DA, 0x87, 0x69, 0x14, 0xAE, 0x6D, 0xC1, 0x17, 0x0A);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_FLAGS_1, 0xbcded951, 0x187b, 0x4d05, 0xbc, 0xcc, 0xf7, 0xe5, 0x19, 0x60, 0xc2, 0x58);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_ACTION_2, 0x421CBA38, 0x1A8E, 0x4881, 0xAC, 0x89, 0xE3, 0x3A, 0x8B, 0x04, 0xEC, 0xE4);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_LEVEL_2, 0x07A07CA2, 0xADAF, 0x40D7, 0xB0, 0x77, 0x53, 0x3A, 0xAD, 0xED, 0x1B, 0xFA);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_FLAGS_2, 0x7fd2f0c4, 0xfeb7, 0x4da3, 0x81, 0x17, 0xe3, 0xfb, 0xed, 0xc4, 0x65, 0x82);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_ACTION_3, 0x80472613, 0x9780, 0x455E, 0xB3, 0x08, 0x72, 0xD3, 0x00, 0x3C, 0xF2, 0xF8);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_LEVEL_3, 0x58AFD5A6, 0xC2DD, 0x47D2, 0x9F, 0xBF, 0xEF, 0x70, 0xCC, 0x5C, 0x59, 0x65);
DEFINE_GUID(GUID_BATTERY_DISCHARGE_FLAGS_3, 0x73613ccf, 0xdbfa, 0x4279, 0x83, 0x56, 0x49, 0x35, 0xf6, 0xbf, 0x62, 0xf3);
DEFINE_GUID(GUID_PROCESSOR_SETTINGS_SUBGROUP, 0x54533251, 0x82BE, 0x4824, 0x96, 0xC1, 0x47, 0xB6, 0x0B, 0x74, 0x0D, 0x00);
DEFINE_GUID(GUID_PROCESSOR_THROTTLE_POLICY, 0x57027304, 0x4AF6, 0x4104, 0x92, 0x60, 0xE3, 0xD9, 0x52, 0x48, 0xFC, 0x36);
DEFINE_GUID(GUID_PROCESSOR_THROTTLE_MAXIMUM, 0xBC5038F7, 0x23E0, 0x4960, 0x96, 0xDA, 0x33, 0xAB, 0xAF, 0x59, 0x35, 0xEC);
DEFINE_GUID(GUID_PROCESSOR_THROTTLE_MINIMUM, 0x893DEE8E, 0x2BEF, 0x41E0, 0x89, 0xC6, 0xB5, 0x5D, 0x09, 0x29, 0x96, 0x4C);
DEFINE_GUID(GUID_PROCESSOR_ALLOW_THROTTLING, 0x3b04d4fd, 0x1cc7, 0x4f23, 0xab, 0x1c, 0xd1, 0x33, 0x78, 0x19, 0xc4, 0xbb);
DEFINE_GUID(GUID_PROCESSOR_IDLESTATE_POLICY, 0x68f262a7, 0xf621, 0x4069, 0xb9, 0xa5, 0x48, 0x74, 0x16, 0x9b, 0xe2, 0x3c);
DEFINE_GUID(GUID_PROCESSOR_PERFSTATE_POLICY, 0xBBDC3814, 0x18E9, 0x4463, 0x8A, 0x55, 0xD1, 0x97, 0x32, 0x7C, 0x45, 0xC0);
DEFINE_GUID(GUID_PROCESSOR_PERF_INCREASE_THRESHOLD, 0x06cadf0e, 0x64ed, 0x448a, 0x89, 0x27, 0xce, 0x7b, 0xf9, 0x0e, 0xb3, 0x5d);
DEFINE_GUID(GUID_PROCESSOR_PERF_DECREASE_THRESHOLD, 0x12a0ab44, 0xfe28, 0x4fa9, 0xb3, 0xbd, 0x4b, 0x64, 0xf4, 0x49, 0x60, 0xa6);
DEFINE_GUID(GUID_PROCESSOR_PERF_INCREASE_POLICY, 0x465e1f50, 0xb610, 0x473a, 0xab, 0x58, 0x0, 0xd1, 0x7, 0x7d, 0xc4, 0x18);
DEFINE_GUID(GUID_PROCESSOR_PERF_DECREASE_POLICY, 0x40fbefc7, 0x2e9d, 0x4d25, 0xa1, 0x85, 0xc, 0xfd, 0x85, 0x74, 0xba, 0xc6);
DEFINE_GUID(GUID_PROCESSOR_PERF_INCREASE_TIME, 0x984cf492, 0x3bed, 0x4488, 0xa8, 0xf9, 0x42, 0x86, 0xc9, 0x7b, 0xf5, 0xaa);
DEFINE_GUID(GUID_PROCESSOR_PERF_DECREASE_TIME, 0xd8edeb9b, 0x95cf, 0x4f95, 0xa7, 0x3c, 0xb0, 0x61, 0x97, 0x36, 0x93, 0xc8);
DEFINE_GUID(GUID_PROCESSOR_PERF_TIME_CHECK, 0x4d2b0152, 0x7d5c, 0x498b, 0x88, 0xe2, 0x34, 0x34, 0x53, 0x92, 0xa2, 0xc5);
DEFINE_GUID(GUID_PROCESSOR_PERF_BOOST_POLICY, 0x45bcc044, 0xd885, 0x43e2, 0x86, 0x5, 0xee, 0xe, 0xc6, 0xe9, 0x6b, 0x59);
DEFINE_GUID(GUID_PROCESSOR_IDLE_ALLOW_SCALING, 0x6c2993b0, 0x8f48, 0x481f, 0xbc, 0xc6, 0x0, 0xdd, 0x27, 0x42, 0xaa, 0x6);
DEFINE_GUID(GUID_PROCESSOR_IDLE_DISABLE, 0x5d76a2ca, 0xe8c0, 0x402f, 0xa1, 0x33, 0x21, 0x58, 0x49, 0x2d, 0x58, 0xad);
DEFINE_GUID(GUID_PROCESSOR_IDLE_TIME_CHECK, 0xc4581c31, 0x89ab, 0x4597, 0x8e, 0x2b, 0x9c, 0x9c, 0xab, 0x44, 0xe, 0x6b);
DEFINE_GUID(GUID_PROCESSOR_IDLE_DEMOTE_THRESHOLD, 0x4b92d758, 0x5a24, 0x4851, 0xa4, 0x70, 0x81, 0x5d, 0x78, 0xae, 0xe1, 0x19);
DEFINE_GUID(GUID_PROCESSOR_IDLE_PROMOTE_THRESHOLD, 0x7b224883, 0xb3cc, 0x4d79, 0x81, 0x9f, 0x83, 0x74, 0x15, 0x2c, 0xbe, 0x7c);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_INCREASE_THRESHOLD, 0xdf142941, 0x20f3, 0x4edf, 0x9a, 0x4a, 0x9c, 0x83, 0xd3, 0xd7, 0x17, 0xd1);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_DECREASE_THRESHOLD, 0x68dd2f27, 0xa4ce, 0x4e11, 0x84, 0x87, 0x37, 0x94, 0xe4, 0x13, 0x5d, 0xfa);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_INCREASE_POLICY, 0xc7be0679, 0x2817, 0x4d69, 0x9d, 0x02, 0x51, 0x9a, 0x53, 0x7e, 0xd0, 0xc6);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_DECREASE_POLICY, 0x71021b41, 0xc749, 0x4d21, 0xbe, 0x74, 0xa0, 0x0f, 0x33, 0x5d, 0x58, 0x2b);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_MAX_CORES, 0xea062031, 0x0e34, 0x4ff1, 0x9b, 0x6d, 0xeb, 0x10, 0x59, 0x33, 0x40, 0x28);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_MIN_CORES, 0x0cc5b647, 0xc1df, 0x4637, 0x89, 0x1a, 0xde, 0xc3, 0x5c, 0x31, 0x85, 0x83);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_INCREASE_TIME, 0x2ddd5a84, 0x5a71, 0x437e, 0x91, 0x2a, 0xdb, 0x0b, 0x8c, 0x78, 0x87, 0x32);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_DECREASE_TIME, 0xdfd10d17, 0xd5eb, 0x45dd, 0x87, 0x7a, 0x9a, 0x34, 0xdd, 0xd1, 0x5c, 0x82);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_AFFINITY_HISTORY_DECREASE_FACTOR, 0x8f7b45e3, 0xc393, 0x480a, 0x87, 0x8c, 0xf6, 0x7a, 0xc3, 0xd0, 0x70, 0x82);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_AFFINITY_HISTORY_THRESHOLD, 0x5b33697b, 0xe89d, 0x4d38, 0xaa, 0x46, 0x9e, 0x7d, 0xfb, 0x7c, 0xd2, 0xf9);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_AFFINITY_WEIGHTING, 0xe70867f1, 0xfa2f, 0x4f4e, 0xae, 0xa1, 0x4d, 0x8a, 0x0b, 0xa2, 0x3b, 0x20);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_OVER_UTILIZATION_HISTORY_DECREASE_FACTOR, 0x1299023c, 0xbc28, 0x4f0a, 0x81, 0xec, 0xd3, 0x29, 0x5a, 0x8d, 0x81, 0x5d);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_OVER_UTILIZATION_HISTORY_THRESHOLD, 0x9ac18e92, 0xaa3c, 0x4e27, 0xb3, 0x07, 0x01, 0xae, 0x37, 0x30, 0x71, 0x29);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_OVER_UTILIZATION_WEIGHTING, 0x8809c2d8, 0xb155, 0x42d4, 0xbc, 0xda, 0x0d, 0x34, 0x56, 0x51, 0xb1, 0xdb);
DEFINE_GUID(GUID_PROCESSOR_CORE_PARKING_OVER_UTILIZATION_THRESHOLD, 0x943c8cb6, 0x6f93, 0x4227, 0xad, 0x87, 0xe9, 0xa3, 0xfe, 0xec, 0x08, 0xd1);
DEFINE_GUID(GUID_PROCESSOR_PARKING_CORE_OVERRIDE, 0xa55612aa, 0xf624, 0x42c6, 0xa4, 0x43, 0x73, 0x97, 0xd0, 0x64, 0xc0, 0x4f);
DEFINE_GUID(GUID_PROCESSOR_PARKING_PERF_STATE, 0x447235c7, 0x6a8d, 0x4cc0, 0x8e, 0x24, 0x9e, 0xaf, 0x70, 0xb9, 0x6e, 0x2b);
DEFINE_GUID(GUID_PROCESSOR_PERF_HISTORY, 0x7d24baa7, 0x0b84, 0x480f, 0x84, 0x0c, 0x1b, 0x07, 0x43, 0xc0, 0x0f, 0x5f);
DEFINE_GUID(GUID_SYSTEM_COOLING_POLICY, 0x94D3A615, 0xA899, 0x4AC5, 0xAE, 0x2B, 0xE4, 0xD8, 0xF6, 0x34, 0x36, 0x7F);
DEFINE_GUID(GUID_LOCK_CONSOLE_ON_WAKE, 0x0E796BDB, 0x100D, 0x47D6, 0xA2, 0xD5, 0xF7, 0xD2, 0xDA, 0xA5, 0x1F, 0x51);
DEFINE_GUID(GUID_DEVICE_IDLE_POLICY, 0x4faab71a, 0x92e5, 0x4726, 0xb5, 0x31, 0x22, 0x45, 0x59, 0x67, 0x2d, 0x19);
DEFINE_GUID(GUID_ACDC_POWER_SOURCE, 0x5D3E9A59, 0xE9D5, 0x4B00, 0xA6, 0xBD, 0xFF, 0x34, 0xFF, 0x51, 0x65, 0x48);
DEFINE_GUID(GUID_LIDSWITCH_STATE_CHANGE,  0xBA3E0F4D, 0xB817, 0x4094, 0xA2, 0xD1, 0xD5, 0x63, 0x79, 0xE6, 0xA0, 0xF3);
DEFINE_GUID(GUID_BATTERY_PERCENTAGE_REMAINING, 0xA7AD8041, 0xB45A, 0x4CAE, 0x87, 0xA3, 0xEE, 0xCB, 0xB4, 0x68, 0xA9, 0xE1);
DEFINE_GUID(GUID_IDLE_BACKGROUND_TASK, 0x515C31D8, 0xF734, 0x163D, 0xA0, 0xFD, 0x11, 0xA0, 0x8C, 0x91, 0xE8, 0xF1);
DEFINE_GUID(GUID_BACKGROUND_TASK_NOTIFICATION, 0xCF23F240, 0x2A54, 0x48D8, 0xB1, 0x14, 0xDE, 0x15, 0x18, 0xFF, 0x05, 0x2E);
DEFINE_GUID(GUID_APPLAUNCH_BUTTON, 0x1A689231, 0x7399, 0x4E9A, 0x8F, 0x99, 0xB7, 0x1F, 0x99, 0x9D, 0xB3, 0xFA);
DEFINE_GUID(GUID_PCIEXPRESS_SETTINGS_SUBGROUP, 0x501a4d13, 0x42af,0x4429, 0x9f, 0xd1, 0xa8, 0x21, 0x8c, 0x26, 0x8e, 0x20);
DEFINE_GUID(GUID_PCIEXPRESS_ASPM_POLICY, 0xee12f906, 0xd277, 0x404b, 0xb6, 0xda, 0xe5, 0xfa, 0x1a, 0x57, 0x6d, 0xf5);
DEFINE_GUID(GUID_ENABLE_SWITCH_FORCED_SHUTDOWN, 0x833a6b62, 0xdfa4, 0x46d1, 0x82, 0xf8, 0xe0, 0x9e, 0x34, 0xd0, 0x29, 0xd6);

#define PERFSTATE_POLICY_CHANGE_IDEAL            0
#define PERFSTATE_POLICY_CHANGE_SINGLE           1
#define PERFSTATE_POLICY_CHANGE_ROCKET           2
#define PERFSTATE_POLICY_CHANGE_MAX              PERFSTATE_POLICY_CHANGE_ROCKET

#define PROCESSOR_PERF_BOOST_POLICY_DISABLED     0
#define PROCESSOR_PERF_BOOST_POLICY_MAX          100

#define POWER_DEVICE_IDLE_POLICY_PERFORMANCE     0
#define POWER_DEVICE_IDLE_POLICY_CONSERVATIVE    1

typedef VOID
(NTAPI REQUEST_POWER_COMPLETE)(
  IN struct _DEVICE_OBJECT *DeviceObject,
  IN UCHAR MinorFunction,
  IN POWER_STATE PowerState,
  IN PVOID Context,
  IN struct _IO_STATUS_BLOCK *IoStatus);
typedef REQUEST_POWER_COMPLETE *PREQUEST_POWER_COMPLETE;

typedef
NTSTATUS
(NTAPI POWER_SETTING_CALLBACK)(
  IN LPCGUID SettingGuid,
  IN PVOID Value,
  IN ULONG ValueLength,
  IN OUT PVOID Context OPTIONAL);
typedef POWER_SETTING_CALLBACK *PPOWER_SETTING_CALLBACK;

DECLARE_HANDLE(POHANDLE);

#define PO_FX_VERSION_V1 1
#define PO_FX_VERSION_V2 2
#define PO_FX_VERSION PO_FX_VERSION_V2

typedef void (NTAPI PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK)(void *context, ULONG component);
typedef PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK *PPO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK;

typedef void (NTAPI PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK)(void *context, ULONG component);
typedef PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK *PPO_FX_COMPONENT_IDLE_CONDITION_CALLBACK;

typedef void (NTAPI PO_FX_COMPONENT_IDLE_STATE_CALLBACK)(void *context, ULONG component, ULONG state);
typedef PO_FX_COMPONENT_IDLE_STATE_CALLBACK *PPO_FX_COMPONENT_IDLE_STATE_CALLBACK;

typedef NTSTATUS (NTAPI PO_FX_POWER_CONTROL_CALLBACK)(void *context, const GUID *code, void *in, SIZE_T in_size, void *out, SIZE_T out_size, SIZE_T *ret_size);
typedef PO_FX_POWER_CONTROL_CALLBACK *PPO_FX_POWER_CONTROL_CALLBACK;

typedef struct _PO_FX_COMPONENT_IDLE_STATE
{
    ULONGLONG TransitionLatency;
    ULONGLONG ResidencyRequirement;
    ULONG NominalPower;
} PO_FX_COMPONENT_IDLE_STATE, *PPO_FX_COMPONENT_IDLE_STATE;

typedef struct _PO_FX_COMPONENT_V1
{
    GUID Id;
    ULONG IdleStateCount;
    ULONG DeepestWakeableIdleState;
    PO_FX_COMPONENT_IDLE_STATE *IdleStates;
} PO_FX_COMPONENT_V1, *PPO_FX_COMPONENT_V1;

typedef struct _PO_FX_COMPONENT_V2
{
    GUID Id;
    ULONGLONG Flags;
    ULONG DeepestWakeableIdleState;
    ULONG IdleStateCount;
    PO_FX_COMPONENT_IDLE_STATE *IdleStates;
    ULONG ProviderCount;
    ULONG *Providers;
} PO_FX_COMPONENT_V2, *PPO_FX_COMPONENT_V2;

#if PO_FX_VERSION == PO_FX_VERSION_V1
typedef PO_FX_COMPONENT_V1 PO_FX_COMPONENT, *PPO_FX_COMPONENT;
#else
typedef PO_FX_COMPONENT_V2 PO_FX_COMPONENT, *PPO_FX_COMPONENT;
#endif

/******************************************************************************
 *                            Configuration Manager Types                     *
 ******************************************************************************/

/* Resource list definitions */
typedef int CM_RESOURCE_TYPE;

#define CmResourceTypeNull              0
#define CmResourceTypePort              1
#define CmResourceTypeInterrupt         2
#define CmResourceTypeMemory            3
#define CmResourceTypeDma               4
#define CmResourceTypeDeviceSpecific    5
#define CmResourceTypeBusNumber         6
#define CmResourceTypeNonArbitrated     128
#define CmResourceTypeConfigData        128
#define CmResourceTypeDevicePrivate     129
#define CmResourceTypePcCardConfig      130
#define CmResourceTypeMfCardConfig      131

/* KEY_VALUE_Xxx.Type */
#define REG_NONE                           0
#define REG_SZ                             1
#define REG_EXPAND_SZ                      2
#define REG_BINARY                         3
#define REG_DWORD                          4
#define REG_DWORD_LITTLE_ENDIAN            4
#define REG_DWORD_BIG_ENDIAN               5
#define REG_LINK                           6
#define REG_MULTI_SZ                       7
#define REG_RESOURCE_LIST                  8
#define REG_FULL_RESOURCE_DESCRIPTOR       9
#define REG_RESOURCE_REQUIREMENTS_LIST     10
#define REG_QWORD                          11
#define REG_QWORD_LITTLE_ENDIAN            11

/* Registry Access Rights */
#define KEY_QUERY_VALUE         (0x0001)
#define KEY_SET_VALUE           (0x0002)
#define KEY_CREATE_SUB_KEY      (0x0004)
#define KEY_ENUMERATE_SUB_KEYS  (0x0008)
#define KEY_NOTIFY              (0x0010)
#define KEY_CREATE_LINK         (0x0020)
#define KEY_WOW64_32KEY         (0x0200)
#define KEY_WOW64_64KEY         (0x0100)
#define KEY_WOW64_RES           (0x0300)

#define KEY_READ                ((STANDARD_RIGHTS_READ       |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY)                 \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_WRITE               ((STANDARD_RIGHTS_WRITE      |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY)         \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_EXECUTE             ((KEY_READ)                   \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_ALL_ACCESS          ((STANDARD_RIGHTS_ALL        |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY         |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY                 |\
                                  KEY_CREATE_LINK)            \
                                  &                           \
                                 (~SYNCHRONIZE))

/* Registry Open/Create Options */
#define REG_OPTION_RESERVED         (0x00000000L)
#define REG_OPTION_NON_VOLATILE     (0x00000000L)
#define REG_OPTION_VOLATILE         (0x00000001L)
#define REG_OPTION_CREATE_LINK      (0x00000002L)
#define REG_OPTION_BACKUP_RESTORE   (0x00000004L)
#define REG_OPTION_OPEN_LINK        (0x00000008L)

#define REG_LEGAL_OPTION            \
                (REG_OPTION_RESERVED            |\
                 REG_OPTION_NON_VOLATILE        |\
                 REG_OPTION_VOLATILE            |\
                 REG_OPTION_CREATE_LINK         |\
                 REG_OPTION_BACKUP_RESTORE      |\
                 REG_OPTION_OPEN_LINK)

#define REG_OPEN_LEGAL_OPTION       \
                (REG_OPTION_RESERVED            |\
                 REG_OPTION_BACKUP_RESTORE      |\
                 REG_OPTION_OPEN_LINK)

#define REG_STANDARD_FORMAT            1
#define REG_LATEST_FORMAT              2
#define REG_NO_COMPRESSION             4

/* Key creation/open disposition */
#define REG_CREATED_NEW_KEY         (0x00000001L)
#define REG_OPENED_EXISTING_KEY     (0x00000002L)

/* Key restore & hive load flags */
#define REG_WHOLE_HIVE_VOLATILE         (0x00000001L)
#define REG_REFRESH_HIVE                (0x00000002L)
#define REG_NO_LAZY_FLUSH               (0x00000004L)
#define REG_FORCE_RESTORE               (0x00000008L)
#define REG_APP_HIVE                    (0x00000010L)
#define REG_PROCESS_PRIVATE             (0x00000020L)
#define REG_START_JOURNAL               (0x00000040L)
#define REG_HIVE_EXACT_FILE_GROWTH      (0x00000080L)
#define REG_HIVE_NO_RM                  (0x00000100L)
#define REG_HIVE_SINGLE_LOG             (0x00000200L)
#define REG_BOOT_HIVE                   (0x00000400L)

/* Unload Flags */
#define REG_FORCE_UNLOAD            1

/* Notify Filter Values */
#define REG_NOTIFY_CHANGE_NAME          (0x00000001L)
#define REG_NOTIFY_CHANGE_ATTRIBUTES    (0x00000002L)
#define REG_NOTIFY_CHANGE_LAST_SET      (0x00000004L)
#define REG_NOTIFY_CHANGE_SECURITY      (0x00000008L)

#define REG_LEGAL_CHANGE_FILTER                 \
                (REG_NOTIFY_CHANGE_NAME          |\
                 REG_NOTIFY_CHANGE_ATTRIBUTES    |\
                 REG_NOTIFY_CHANGE_LAST_SET      |\
                 REG_NOTIFY_CHANGE_SECURITY)

#include <pshpack4.h>
typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR {
  UCHAR Type;
  UCHAR ShareDisposition;
  USHORT Flags;
  union {
    struct {
      PHYSICAL_ADDRESS Start;
      ULONG Length;
    } Generic;
    struct {
      PHYSICAL_ADDRESS Start;
      ULONG Length;
    } Port;
    struct {
#if defined(NT_PROCESSOR_GROUPS)
      USHORT Level;
      USHORT Group;
#else
      ULONG Level;
#endif
      ULONG Vector;
      KAFFINITY Affinity;
    } Interrupt;
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    struct {
      _ANONYMOUS_UNION union {
        struct {
#if defined(NT_PROCESSOR_GROUPS)
          USHORT Group;
#else
          USHORT Reserved;
#endif
          USHORT MessageCount;
          ULONG Vector;
          KAFFINITY Affinity;
        } Raw;
        struct {
#if defined(NT_PROCESSOR_GROUPS)
          USHORT Level;
          USHORT Group;
#else
          ULONG Level;
#endif
          ULONG Vector;
          KAFFINITY Affinity;
        } Translated;
      } DUMMYUNIONNAME;
    } MessageInterrupt;
#endif
    struct {
      PHYSICAL_ADDRESS Start;
      ULONG Length;
    } Memory;
    struct {
      ULONG Channel;
      ULONG Port;
      ULONG Reserved1;
    } Dma;
    struct {
      ULONG Data[3];
    } DevicePrivate;
    struct {
      ULONG Start;
      ULONG Length;
      ULONG Reserved;
    } BusNumber;
    struct {
      ULONG DataSize;
      ULONG Reserved1;
      ULONG Reserved2;
    } DeviceSpecificData;
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    struct {
      PHYSICAL_ADDRESS Start;
      ULONG Length40;
    } Memory40;
    struct {
      PHYSICAL_ADDRESS Start;
      ULONG Length48;
    } Memory48;
    struct {
      PHYSICAL_ADDRESS Start;
      ULONG Length64;
    } Memory64;
#endif
  } u;
} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR;
#include <poppack.h>

/* CM_PARTIAL_RESOURCE_DESCRIPTOR.Type */
#define CmResourceTypeNull                0
#define CmResourceTypePort                1
#define CmResourceTypeInterrupt           2
#define CmResourceTypeMemory              3
#define CmResourceTypeDma                 4
#define CmResourceTypeDeviceSpecific      5
#define CmResourceTypeBusNumber           6
#define CmResourceTypeMemoryLarge         7
#define CmResourceTypeNonArbitrated       128
#define CmResourceTypeConfigData          128
#define CmResourceTypeDevicePrivate       129
#define CmResourceTypePcCardConfig        130
#define CmResourceTypeMfCardConfig        131

/* CM_PARTIAL_RESOURCE_DESCRIPTOR.ShareDisposition */
typedef enum _CM_SHARE_DISPOSITION {
  CmResourceShareUndetermined = 0,
  CmResourceShareDeviceExclusive,
  CmResourceShareDriverExclusive,
  CmResourceShareShared
} CM_SHARE_DISPOSITION;

/* CM_PARTIAL_RESOURCE_DESCRIPTOR.Flags if Type = CmResourceTypePort */
#define CM_RESOURCE_PORT_MEMORY           0x0000
#define CM_RESOURCE_PORT_IO               0x0001
#define CM_RESOURCE_PORT_10_BIT_DECODE    0x0004
#define CM_RESOURCE_PORT_12_BIT_DECODE    0x0008
#define CM_RESOURCE_PORT_16_BIT_DECODE    0x0010
#define CM_RESOURCE_PORT_POSITIVE_DECODE  0x0020
#define CM_RESOURCE_PORT_PASSIVE_DECODE   0x0040
#define CM_RESOURCE_PORT_WINDOW_DECODE    0x0080
#define CM_RESOURCE_PORT_BAR              0x0100

/* CM_PARTIAL_RESOURCE_DESCRIPTOR.Flags if Type = CmResourceTypeInterrupt */
#define CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE 0x0000
#define CM_RESOURCE_INTERRUPT_LATCHED         0x0001
#define CM_RESOURCE_INTERRUPT_MESSAGE         0x0002
#define CM_RESOURCE_INTERRUPT_POLICY_INCLUDED 0x0004

#define CM_RESOURCE_INTERRUPT_LEVEL_LATCHED_BITS 0x0001

#define CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN   ((ULONG)-2)

/* CM_PARTIAL_RESOURCE_DESCRIPTOR.Flags if Type = CmResourceTypeMemory */
#define CM_RESOURCE_MEMORY_READ_WRITE                    0x0000
#define CM_RESOURCE_MEMORY_READ_ONLY                     0x0001
#define CM_RESOURCE_MEMORY_WRITE_ONLY                    0x0002
#define CM_RESOURCE_MEMORY_WRITEABILITY_MASK             0x0003
#define CM_RESOURCE_MEMORY_PREFETCHABLE                  0x0004
#define CM_RESOURCE_MEMORY_COMBINEDWRITE                 0x0008
#define CM_RESOURCE_MEMORY_24                            0x0010
#define CM_RESOURCE_MEMORY_CACHEABLE                     0x0020
#define CM_RESOURCE_MEMORY_WINDOW_DECODE                 0x0040
#define CM_RESOURCE_MEMORY_BAR                           0x0080
#define CM_RESOURCE_MEMORY_COMPAT_FOR_INACCESSIBLE_RANGE 0x0100

#define CM_RESOURCE_MEMORY_LARGE                         0x0E00
#define CM_RESOURCE_MEMORY_LARGE_40                      0x0200
#define CM_RESOURCE_MEMORY_LARGE_48                      0x0400
#define CM_RESOURCE_MEMORY_LARGE_64                      0x0800

#define CM_RESOURCE_MEMORY_LARGE_40_MAXLEN               0x000000FFFFFFFF00
#define CM_RESOURCE_MEMORY_LARGE_48_MAXLEN               0x0000FFFFFFFF0000
#define CM_RESOURCE_MEMORY_LARGE_64_MAXLEN               0xFFFFFFFF00000000

/* CM_PARTIAL_RESOURCE_DESCRIPTOR.Flags if Type = CmResourceTypeDma */
#define CM_RESOURCE_DMA_8                 0x0000
#define CM_RESOURCE_DMA_16                0x0001
#define CM_RESOURCE_DMA_32                0x0002
#define CM_RESOURCE_DMA_8_AND_16          0x0004
#define CM_RESOURCE_DMA_BUS_MASTER        0x0008
#define CM_RESOURCE_DMA_TYPE_A            0x0010
#define CM_RESOURCE_DMA_TYPE_B            0x0020
#define CM_RESOURCE_DMA_TYPE_F            0x0040

typedef struct _DEVICE_FLAGS {
  ULONG Failed:1;
  ULONG ReadOnly:1;
  ULONG Removable:1;
  ULONG ConsoleIn:1;
  ULONG ConsoleOut:1;
  ULONG Input:1;
  ULONG Output:1;
} DEVICE_FLAGS, *PDEVICE_FLAGS;

typedef enum _INTERFACE_TYPE {
  InterfaceTypeUndefined = -1,
  Internal,
  Isa,
  Eisa,
  MicroChannel,
  TurboChannel,
  PCIBus,
  VMEBus,
  NuBus,
  PCMCIABus,
  CBus,
  MPIBus,
  MPSABus,
  ProcessorInternal,
  InternalPowerBus,
  PNPISABus,
  PNPBus,
  Vmcs,
  ACPIBus,
  MaximumInterfaceType
} INTERFACE_TYPE, *PINTERFACE_TYPE;

typedef struct _CM_COMPONENT_INFORMATION {
  DEVICE_FLAGS Flags;
  ULONG Version;
  ULONG Key;
  KAFFINITY AffinityMask;
} CM_COMPONENT_INFORMATION, *PCM_COMPONENT_INFORMATION;

typedef struct _CM_ROM_BLOCK {
  ULONG Address;
  ULONG Size;
} CM_ROM_BLOCK, *PCM_ROM_BLOCK;

typedef struct _CM_PARTIAL_RESOURCE_LIST {
  USHORT Version;
  USHORT Revision;
  ULONG Count;
  CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1];
} CM_PARTIAL_RESOURCE_LIST, *PCM_PARTIAL_RESOURCE_LIST;

typedef struct _CM_FULL_RESOURCE_DESCRIPTOR {
  INTERFACE_TYPE InterfaceType;
  ULONG BusNumber;
  CM_PARTIAL_RESOURCE_LIST PartialResourceList;
} CM_FULL_RESOURCE_DESCRIPTOR, *PCM_FULL_RESOURCE_DESCRIPTOR;

typedef struct _CM_RESOURCE_LIST {
  ULONG Count;
  CM_FULL_RESOURCE_DESCRIPTOR List[1];
} CM_RESOURCE_LIST, *PCM_RESOURCE_LIST;

typedef struct _PNP_BUS_INFORMATION {
  GUID BusTypeGuid;
  INTERFACE_TYPE LegacyBusType;
  ULONG BusNumber;
} PNP_BUS_INFORMATION, *PPNP_BUS_INFORMATION;

#include <pshpack1.h>

typedef struct _CM_INT13_DRIVE_PARAMETER {
  USHORT DriveSelect;
  ULONG MaxCylinders;
  USHORT SectorsPerTrack;
  USHORT MaxHeads;
  USHORT NumberDrives;
} CM_INT13_DRIVE_PARAMETER, *PCM_INT13_DRIVE_PARAMETER;

typedef struct _CM_MCA_POS_DATA {
  USHORT AdapterId;
  UCHAR PosData1;
  UCHAR PosData2;
  UCHAR PosData3;
  UCHAR PosData4;
} CM_MCA_POS_DATA, *PCM_MCA_POS_DATA;

typedef struct _CM_PNP_BIOS_DEVICE_NODE {
  USHORT Size;
  UCHAR Node;
  ULONG ProductId;
  UCHAR DeviceType[3];
  USHORT DeviceAttributes;
} CM_PNP_BIOS_DEVICE_NODE,*PCM_PNP_BIOS_DEVICE_NODE;

typedef struct _CM_PNP_BIOS_INSTALLATION_CHECK {
  UCHAR Signature[4];
  UCHAR Revision;
  UCHAR Length;
  USHORT ControlField;
  UCHAR Checksum;
  ULONG EventFlagAddress;
  USHORT RealModeEntryOffset;
  USHORT RealModeEntrySegment;
  USHORT ProtectedModeEntryOffset;
  ULONG ProtectedModeCodeBaseAddress;
  ULONG OemDeviceId;
  USHORT RealModeDataBaseAddress;
  ULONG ProtectedModeDataBaseAddress;
} CM_PNP_BIOS_INSTALLATION_CHECK, *PCM_PNP_BIOS_INSTALLATION_CHECK;

#include <poppack.h>

typedef struct _CM_DISK_GEOMETRY_DEVICE_DATA {
  ULONG BytesPerSector;
  ULONG NumberOfCylinders;
  ULONG SectorsPerTrack;
  ULONG NumberOfHeads;
} CM_DISK_GEOMETRY_DEVICE_DATA, *PCM_DISK_GEOMETRY_DEVICE_DATA;

typedef struct _CM_KEYBOARD_DEVICE_DATA {
  USHORT Version;
  USHORT Revision;
  UCHAR Type;
  UCHAR Subtype;
  USHORT KeyboardFlags;
} CM_KEYBOARD_DEVICE_DATA, *PCM_KEYBOARD_DEVICE_DATA;

typedef struct _CM_SCSI_DEVICE_DATA {
  USHORT Version;
  USHORT Revision;
  UCHAR HostIdentifier;
} CM_SCSI_DEVICE_DATA, *PCM_SCSI_DEVICE_DATA;

typedef struct _CM_VIDEO_DEVICE_DATA {
  USHORT Version;
  USHORT Revision;
  ULONG VideoClock;
} CM_VIDEO_DEVICE_DATA, *PCM_VIDEO_DEVICE_DATA;

typedef struct _CM_SONIC_DEVICE_DATA {
  USHORT Version;
  USHORT Revision;
  USHORT DataConfigurationRegister;
  UCHAR EthernetAddress[8];
} CM_SONIC_DEVICE_DATA, *PCM_SONIC_DEVICE_DATA;

typedef struct _CM_SERIAL_DEVICE_DATA {
  USHORT Version;
  USHORT Revision;
  ULONG BaudClock;
} CM_SERIAL_DEVICE_DATA, *PCM_SERIAL_DEVICE_DATA;

typedef struct _CM_MONITOR_DEVICE_DATA {
  USHORT Version;
  USHORT Revision;
  USHORT HorizontalScreenSize;
  USHORT VerticalScreenSize;
  USHORT HorizontalResolution;
  USHORT VerticalResolution;
  USHORT HorizontalDisplayTimeLow;
  USHORT HorizontalDisplayTime;
  USHORT HorizontalDisplayTimeHigh;
  USHORT HorizontalBackPorchLow;
  USHORT HorizontalBackPorch;
  USHORT HorizontalBackPorchHigh;
  USHORT HorizontalFrontPorchLow;
  USHORT HorizontalFrontPorch;
  USHORT HorizontalFrontPorchHigh;
  USHORT HorizontalSyncLow;
  USHORT HorizontalSync;
  USHORT HorizontalSyncHigh;
  USHORT VerticalBackPorchLow;
  USHORT VerticalBackPorch;
  USHORT VerticalBackPorchHigh;
  USHORT VerticalFrontPorchLow;
  USHORT VerticalFrontPorch;
  USHORT VerticalFrontPorchHigh;
  USHORT VerticalSyncLow;
  USHORT VerticalSync;
  USHORT VerticalSyncHigh;
} CM_MONITOR_DEVICE_DATA, *PCM_MONITOR_DEVICE_DATA;

typedef struct _CM_FLOPPY_DEVICE_DATA {
  USHORT Version;
  USHORT Revision;
  CHAR Size[8];
  ULONG MaxDensity;
  ULONG MountDensity;
  UCHAR StepRateHeadUnloadTime;
  UCHAR HeadLoadTime;
  UCHAR MotorOffTime;
  UCHAR SectorLengthCode;
  UCHAR SectorPerTrack;
  UCHAR ReadWriteGapLength;
  UCHAR DataTransferLength;
  UCHAR FormatGapLength;
  UCHAR FormatFillCharacter;
  UCHAR HeadSettleTime;
  UCHAR MotorSettleTime;
  UCHAR MaximumTrackValue;
  UCHAR DataTransferRate;
} CM_FLOPPY_DEVICE_DATA, *PCM_FLOPPY_DEVICE_DATA;

typedef enum _KEY_INFORMATION_CLASS {
  KeyBasicInformation,
  KeyNodeInformation,
  KeyFullInformation,
  KeyNameInformation,
  KeyCachedInformation,
  KeyFlagsInformation,
  KeyVirtualizationInformation,
  KeyHandleTagsInformation,
  KeyTrustInformation,
  KeyLayerInformation,
  MaxKeyInfoClass
} KEY_INFORMATION_CLASS;

typedef struct _KEY_BASIC_INFORMATION {
  LARGE_INTEGER LastWriteTime;
  ULONG TitleIndex;
  ULONG NameLength;
  WCHAR Name[1];
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

typedef struct _KEY_CONTROL_FLAGS_INFORMATION {
  ULONG ControlFlags;
} KEY_CONTROL_FLAGS_INFORMATION, *PKEY_CONTROL_FLAGS_INFORMATION;

typedef struct _KEY_FULL_INFORMATION {
  LARGE_INTEGER LastWriteTime;
  ULONG TitleIndex;
  ULONG ClassOffset;
  ULONG ClassLength;
  ULONG SubKeys;
  ULONG MaxNameLen;
  ULONG MaxClassLen;
  ULONG Values;
  ULONG MaxValueNameLen;
  ULONG MaxValueDataLen;
  WCHAR Class[1];
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;

typedef struct _KEY_HANDLE_TAGS_INFORMATION {
  ULONG HandleTags;
} KEY_HANDLE_TAGS_INFORMATION, *PKEY_HANDLE_TAGS_INFORMATION;

typedef struct _KEY_NODE_INFORMATION {
  LARGE_INTEGER LastWriteTime;
  ULONG TitleIndex;
  ULONG ClassOffset;
  ULONG ClassLength;
  ULONG NameLength;
  WCHAR Name[1];
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

typedef enum _KEY_SET_INFORMATION_CLASS {
  KeyWriteTimeInformation,
  KeyWow64FlagsInformation,
  KeyControlFlagsInformation,
  KeySetVirtualizationInformation,
  KeySetDebugInformation,
  KeySetHandleTagsInformation,
  KeySetLayerInformation,
  MaxKeySetInfoClass
} KEY_SET_INFORMATION_CLASS;

typedef struct _KEY_SET_VIRTUALIZATION_INFORMATION {
  ULONG VirtualTarget:1;
  ULONG VirtualStore:1;
  ULONG VirtualSource:1;
  ULONG Reserved:29;
} KEY_SET_VIRTUALIZATION_INFORMATION, *PKEY_SET_VIRTUALIZATION_INFORMATION;

typedef struct _KEY_VALUE_BASIC_INFORMATION {
  ULONG TitleIndex;
  ULONG Type;
  ULONG NameLength;
  WCHAR Name[1];
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
  ULONG TitleIndex;
  ULONG Type;
  ULONG DataOffset;
  ULONG DataLength;
  ULONG NameLength;
  WCHAR Name[1];
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
  ULONG TitleIndex;
  ULONG Type;
  ULONG DataLength;
  UCHAR Data[1];
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION_ALIGN64 {
  ULONG Type;
  ULONG DataLength;
  UCHAR Data[1];
} KEY_VALUE_PARTIAL_INFORMATION_ALIGN64, *PKEY_VALUE_PARTIAL_INFORMATION_ALIGN64;

typedef struct _KEY_VALUE_ENTRY {
  PUNICODE_STRING ValueName;
  ULONG DataLength;
  ULONG DataOffset;
  ULONG Type;
} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
  KeyValueBasicInformation,
  KeyValueFullInformation,
  KeyValuePartialInformation,
  KeyValueFullInformationAlign64,
  KeyValuePartialInformationAlign64,
  KeyValueLayerInformation,
  MaxKeyValueInfoClass
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_WOW64_FLAGS_INFORMATION {
  ULONG UserFlags;
} KEY_WOW64_FLAGS_INFORMATION, *PKEY_WOW64_FLAGS_INFORMATION;

typedef struct _KEY_WRITE_TIME_INFORMATION {
  LARGE_INTEGER LastWriteTime;
} KEY_WRITE_TIME_INFORMATION, *PKEY_WRITE_TIME_INFORMATION;

typedef enum _REG_NOTIFY_CLASS {
  RegNtDeleteKey,
  RegNtPreDeleteKey = RegNtDeleteKey,
  RegNtSetValueKey,
  RegNtPreSetValueKey = RegNtSetValueKey,
  RegNtDeleteValueKey,
  RegNtPreDeleteValueKey = RegNtDeleteValueKey,
  RegNtSetInformationKey,
  RegNtPreSetInformationKey = RegNtSetInformationKey,
  RegNtRenameKey,
  RegNtPreRenameKey = RegNtRenameKey,
  RegNtEnumerateKey,
  RegNtPreEnumerateKey = RegNtEnumerateKey,
  RegNtEnumerateValueKey,
  RegNtPreEnumerateValueKey = RegNtEnumerateValueKey,
  RegNtQueryKey,
  RegNtPreQueryKey = RegNtQueryKey,
  RegNtQueryValueKey,
  RegNtPreQueryValueKey = RegNtQueryValueKey,
  RegNtQueryMultipleValueKey,
  RegNtPreQueryMultipleValueKey = RegNtQueryMultipleValueKey,
  RegNtPreCreateKey,
  RegNtPostCreateKey,
  RegNtPreOpenKey,
  RegNtPostOpenKey,
  RegNtKeyHandleClose,
  RegNtPreKeyHandleClose = RegNtKeyHandleClose,
  RegNtPostDeleteKey,
  RegNtPostSetValueKey,
  RegNtPostDeleteValueKey,
  RegNtPostSetInformationKey,
  RegNtPostRenameKey,
  RegNtPostEnumerateKey,
  RegNtPostEnumerateValueKey,
  RegNtPostQueryKey,
  RegNtPostQueryValueKey,
  RegNtPostQueryMultipleValueKey,
  RegNtPostKeyHandleClose,
  RegNtPreCreateKeyEx,
  RegNtPostCreateKeyEx,
  RegNtPreOpenKeyEx,
  RegNtPostOpenKeyEx,
  RegNtPreFlushKey,
  RegNtPostFlushKey,
  RegNtPreLoadKey,
  RegNtPostLoadKey,
  RegNtPreUnLoadKey,
  RegNtPostUnLoadKey,
  RegNtPreQueryKeySecurity,
  RegNtPostQueryKeySecurity,
  RegNtPreSetKeySecurity,
  RegNtPostSetKeySecurity,
  RegNtCallbackObjectContextCleanup,
  RegNtPreRestoreKey,
  RegNtPostRestoreKey,
  RegNtPreSaveKey,
  RegNtPostSaveKey,
  RegNtPreReplaceKey,
  RegNtPostReplaceKey,
  RegNtPreQueryKeyName,
  RegNtPostQueryKeyName,
  MaxRegNtNotifyClass
} REG_NOTIFY_CLASS, *PREG_NOTIFY_CLASS;

typedef NTSTATUS
(NTAPI EX_CALLBACK_FUNCTION)(
  IN PVOID CallbackContext,
  IN PVOID Argument1,
  IN PVOID Argument2);
typedef EX_CALLBACK_FUNCTION *PEX_CALLBACK_FUNCTION;

typedef struct _REG_DELETE_KEY_INFORMATION {
  PVOID Object;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_DELETE_KEY_INFORMATION, *PREG_DELETE_KEY_INFORMATION
#if (NTDDI_VERSION >= NTDDI_VISTA)
, REG_FLUSH_KEY_INFORMATION, *PREG_FLUSH_KEY_INFORMATION
#endif
;

typedef struct _REG_SET_VALUE_KEY_INFORMATION {
  PVOID Object;
  PUNICODE_STRING ValueName;
  ULONG TitleIndex;
  ULONG Type;
  PVOID Data;
  ULONG DataSize;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_SET_VALUE_KEY_INFORMATION, *PREG_SET_VALUE_KEY_INFORMATION;

typedef struct _REG_DELETE_VALUE_KEY_INFORMATION {
  PVOID Object;
  PUNICODE_STRING ValueName;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_DELETE_VALUE_KEY_INFORMATION, *PREG_DELETE_VALUE_KEY_INFORMATION;

typedef struct _REG_SET_INFORMATION_KEY_INFORMATION {
  PVOID Object;
  KEY_SET_INFORMATION_CLASS KeySetInformationClass;
  PVOID KeySetInformation;
  ULONG KeySetInformationLength;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_SET_INFORMATION_KEY_INFORMATION, *PREG_SET_INFORMATION_KEY_INFORMATION;

typedef struct _REG_ENUMERATE_KEY_INFORMATION {
  PVOID Object;
  ULONG Index;
  KEY_INFORMATION_CLASS KeyInformationClass;
  PVOID KeyInformation;
  ULONG Length;
  PULONG ResultLength;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_ENUMERATE_KEY_INFORMATION, *PREG_ENUMERATE_KEY_INFORMATION;

typedef struct _REG_ENUMERATE_VALUE_KEY_INFORMATION {
  PVOID Object;
  ULONG Index;
  KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass;
  PVOID KeyValueInformation;
  ULONG Length;
  PULONG ResultLength;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_ENUMERATE_VALUE_KEY_INFORMATION, *PREG_ENUMERATE_VALUE_KEY_INFORMATION;

typedef struct _REG_QUERY_KEY_INFORMATION {
  PVOID Object;
  KEY_INFORMATION_CLASS KeyInformationClass;
  PVOID KeyInformation;
  ULONG Length;
  PULONG ResultLength;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_QUERY_KEY_INFORMATION, *PREG_QUERY_KEY_INFORMATION;

typedef struct _REG_QUERY_VALUE_KEY_INFORMATION {
  PVOID Object;
  PUNICODE_STRING ValueName;
  KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass;
  PVOID KeyValueInformation;
  ULONG Length;
  PULONG ResultLength;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_QUERY_VALUE_KEY_INFORMATION, *PREG_QUERY_VALUE_KEY_INFORMATION;

typedef struct _REG_QUERY_MULTIPLE_VALUE_KEY_INFORMATION {
  PVOID Object;
  PKEY_VALUE_ENTRY ValueEntries;
  ULONG EntryCount;
  PVOID ValueBuffer;
  PULONG BufferLength;
  PULONG RequiredBufferLength;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_QUERY_MULTIPLE_VALUE_KEY_INFORMATION, *PREG_QUERY_MULTIPLE_VALUE_KEY_INFORMATION;

typedef struct _REG_RENAME_KEY_INFORMATION {
  PVOID Object;
  PUNICODE_STRING NewName;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_RENAME_KEY_INFORMATION, *PREG_RENAME_KEY_INFORMATION;

typedef struct _REG_CREATE_KEY_INFORMATION {
  PUNICODE_STRING CompleteName;
  PVOID RootObject;
  PVOID ObjectType;
  ULONG CreateOptions;
  PUNICODE_STRING Class;
  PVOID SecurityDescriptor;
  PVOID SecurityQualityOfService;
  ACCESS_MASK DesiredAccess;
  ACCESS_MASK GrantedAccess;
  PULONG Disposition;
  PVOID *ResultObject;
  PVOID CallContext;
  PVOID RootObjectContext;
  PVOID Transaction;
  PVOID Reserved;
} REG_CREATE_KEY_INFORMATION, REG_OPEN_KEY_INFORMATION,*PREG_CREATE_KEY_INFORMATION, *PREG_OPEN_KEY_INFORMATION;

typedef struct _REG_CREATE_KEY_INFORMATION_V1 {
  PUNICODE_STRING CompleteName;
  PVOID RootObject;
  PVOID ObjectType;
  ULONG Options;
  PUNICODE_STRING Class;
  PVOID SecurityDescriptor;
  PVOID SecurityQualityOfService;
  ACCESS_MASK DesiredAccess;
  ACCESS_MASK GrantedAccess;
  PULONG Disposition;
  PVOID *ResultObject;
  PVOID CallContext;
  PVOID RootObjectContext;
  PVOID Transaction;
  ULONG_PTR Version;
  PUNICODE_STRING RemainingName;
  ULONG Wow64Flags;
  ULONG Attributes;
  KPROCESSOR_MODE CheckAccessMode;
} REG_CREATE_KEY_INFORMATION_V1, REG_OPEN_KEY_INFORMATION_V1,*PREG_CREATE_KEY_INFORMATION_V1, *PREG_OPEN_KEY_INFORMATION_V1;

typedef struct _REG_PRE_CREATE_KEY_INFORMATION {
  PUNICODE_STRING CompleteName;
} REG_PRE_CREATE_KEY_INFORMATION, REG_PRE_OPEN_KEY_INFORMATION,*PREG_PRE_CREATE_KEY_INFORMATION, *PREG_PRE_OPEN_KEY_INFORMATION;

typedef struct _REG_POST_CREATE_KEY_INFORMATION {
  PUNICODE_STRING CompleteName;
  PVOID Object;
  NTSTATUS Status;
} REG_POST_CREATE_KEY_INFORMATION,REG_POST_OPEN_KEY_INFORMATION, *PREG_POST_CREATE_KEY_INFORMATION, *PREG_POST_OPEN_KEY_INFORMATION;

typedef struct _REG_POST_OPERATION_INFORMATION {
  PVOID Object;
  NTSTATUS Status;
  PVOID PreInformation;
  NTSTATUS ReturnStatus;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_POST_OPERATION_INFORMATION,*PREG_POST_OPERATION_INFORMATION;

typedef struct _REG_KEY_HANDLE_CLOSE_INFORMATION {
  PVOID Object;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_KEY_HANDLE_CLOSE_INFORMATION, *PREG_KEY_HANDLE_CLOSE_INFORMATION;

#if (NTDDI_VERSION >= NTDDI_VISTA)

typedef struct _REG_LOAD_KEY_INFORMATION {
  PVOID Object;
  PUNICODE_STRING KeyName;
  PUNICODE_STRING SourceFile;
  ULONG Flags;
  PVOID TrustClassObject;
  PVOID UserEvent;
  ACCESS_MASK DesiredAccess;
  PHANDLE RootHandle;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_LOAD_KEY_INFORMATION, *PREG_LOAD_KEY_INFORMATION;

typedef struct _REG_UNLOAD_KEY_INFORMATION {
  PVOID Object;
  PVOID UserEvent;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_UNLOAD_KEY_INFORMATION, *PREG_UNLOAD_KEY_INFORMATION;

typedef struct _REG_CALLBACK_CONTEXT_CLEANUP_INFORMATION {
  PVOID Object;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_CALLBACK_CONTEXT_CLEANUP_INFORMATION, *PREG_CALLBACK_CONTEXT_CLEANUP_INFORMATION;

typedef struct _REG_QUERY_KEY_SECURITY_INFORMATION {
  PVOID Object;
  PSECURITY_INFORMATION SecurityInformation;
  PSECURITY_DESCRIPTOR SecurityDescriptor;
  PULONG Length;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_QUERY_KEY_SECURITY_INFORMATION, *PREG_QUERY_KEY_SECURITY_INFORMATION;

typedef struct _REG_SET_KEY_SECURITY_INFORMATION {
  PVOID Object;
  PSECURITY_INFORMATION SecurityInformation;
  PSECURITY_DESCRIPTOR SecurityDescriptor;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_SET_KEY_SECURITY_INFORMATION, *PREG_SET_KEY_SECURITY_INFORMATION;

typedef struct _REG_RESTORE_KEY_INFORMATION {
  PVOID Object;
  HANDLE FileHandle;
  ULONG Flags;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_RESTORE_KEY_INFORMATION, *PREG_RESTORE_KEY_INFORMATION;

typedef struct _REG_SAVE_KEY_INFORMATION {
  PVOID Object;
  HANDLE FileHandle;
  ULONG Format;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_SAVE_KEY_INFORMATION, *PREG_SAVE_KEY_INFORMATION;

typedef struct _REG_REPLACE_KEY_INFORMATION {
  PVOID Object;
  PUNICODE_STRING OldFileName;
  PUNICODE_STRING NewFileName;
  PVOID CallContext;
  PVOID ObjectContext;
  PVOID Reserved;
} REG_REPLACE_KEY_INFORMATION, *PREG_REPLACE_KEY_INFORMATION;

#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#define SERVICE_KERNEL_DRIVER          0x00000001
#define SERVICE_FILE_SYSTEM_DRIVER     0x00000002
#define SERVICE_ADAPTER                0x00000004
#define SERVICE_RECOGNIZER_DRIVER      0x00000008

#define SERVICE_DRIVER                 (SERVICE_KERNEL_DRIVER | \
                                        SERVICE_FILE_SYSTEM_DRIVER | \
                                        SERVICE_RECOGNIZER_DRIVER)

#define SERVICE_WIN32_OWN_PROCESS      0x00000010
#define SERVICE_WIN32_SHARE_PROCESS    0x00000020
#define SERVICE_WIN32                  (SERVICE_WIN32_OWN_PROCESS | \
                                        SERVICE_WIN32_SHARE_PROCESS)

#define SERVICE_INTERACTIVE_PROCESS    0x00000100

#define SERVICE_TYPE_ALL               (SERVICE_WIN32  | \
                                        SERVICE_ADAPTER | \
                                        SERVICE_DRIVER  | \
                                        SERVICE_INTERACTIVE_PROCESS)

/* Service Start Types */
#define SERVICE_BOOT_START             0x00000000
#define SERVICE_SYSTEM_START           0x00000001
#define SERVICE_AUTO_START             0x00000002
#define SERVICE_DEMAND_START           0x00000003
#define SERVICE_DISABLED               0x00000004

#define SERVICE_ERROR_IGNORE           0x00000000
#define SERVICE_ERROR_NORMAL           0x00000001
#define SERVICE_ERROR_SEVERE           0x00000002
#define SERVICE_ERROR_CRITICAL         0x00000003

typedef enum _CM_SERVICE_NODE_TYPE {
  DriverType = SERVICE_KERNEL_DRIVER,
  FileSystemType = SERVICE_FILE_SYSTEM_DRIVER,
  Win32ServiceOwnProcess = SERVICE_WIN32_OWN_PROCESS,
  Win32ServiceShareProcess = SERVICE_WIN32_SHARE_PROCESS,
  AdapterType = SERVICE_ADAPTER,
  RecognizerType = SERVICE_RECOGNIZER_DRIVER
} SERVICE_NODE_TYPE;

typedef enum _CM_SERVICE_LOAD_TYPE {
  BootLoad = SERVICE_BOOT_START,
  SystemLoad = SERVICE_SYSTEM_START,
  AutoLoad = SERVICE_AUTO_START,
  DemandLoad = SERVICE_DEMAND_START,
  DisableLoad = SERVICE_DISABLED
} SERVICE_LOAD_TYPE;

typedef enum _CM_ERROR_CONTROL_TYPE {
  IgnoreError = SERVICE_ERROR_IGNORE,
  NormalError = SERVICE_ERROR_NORMAL,
  SevereError = SERVICE_ERROR_SEVERE,
  CriticalError = SERVICE_ERROR_CRITICAL
} SERVICE_ERROR_TYPE;

#define CM_SERVICE_NETWORK_BOOT_LOAD      0x00000001
#define CM_SERVICE_VIRTUAL_DISK_BOOT_LOAD 0x00000002
#define CM_SERVICE_USB_DISK_BOOT_LOAD     0x00000004

#define CM_SERVICE_VALID_PROMOTION_MASK (CM_SERVICE_NETWORK_BOOT_LOAD |       \
                                         CM_SERVICE_VIRTUAL_DISK_BOOT_LOAD |  \
                                         CM_SERVICE_USB_DISK_BOOT_LOAD)


/******************************************************************************
 *                         I/O Manager Types                                  *
 ******************************************************************************/

#define STATUS_CONTINUE_COMPLETION      STATUS_SUCCESS

#define CONNECT_FULLY_SPECIFIED         0x1
#define CONNECT_LINE_BASED              0x2
#define CONNECT_MESSAGE_BASED           0x3
#define CONNECT_FULLY_SPECIFIED_GROUP   0x4
#define CONNECT_CURRENT_VERSION         0x4

#define POOL_COLD_ALLOCATION                256
#define POOL_QUOTA_FAIL_INSTEAD_OF_RAISE    8
#define POOL_RAISE_IF_ALLOCATION_FAILURE    16

#define IO_TYPE_ADAPTER                 1
#define IO_TYPE_CONTROLLER              2
#define IO_TYPE_DEVICE                  3
#define IO_TYPE_DRIVER                  4
#define IO_TYPE_FILE                    5
#define IO_TYPE_IRP                     6
#define IO_TYPE_MASTER_ADAPTER          7
#define IO_TYPE_OPEN_PACKET             8
#define IO_TYPE_TIMER                   9
#define IO_TYPE_VPB                     10
#define IO_TYPE_ERROR_LOG               11
#define IO_TYPE_ERROR_MESSAGE           12
#define IO_TYPE_DEVICE_OBJECT_EXTENSION 13

#define IO_TYPE_CSQ_IRP_CONTEXT 1
#define IO_TYPE_CSQ 2
#define IO_TYPE_CSQ_EX 3

/* IO_RESOURCE_DESCRIPTOR.Option */
#define IO_RESOURCE_PREFERRED             0x01
#define IO_RESOURCE_DEFAULT               0x02
#define IO_RESOURCE_ALTERNATIVE           0x08

#define FILE_DEVICE_BEEP                  0x00000001
#define FILE_DEVICE_CD_ROM                0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM    0x00000003
#define FILE_DEVICE_CONTROLLER            0x00000004
#define FILE_DEVICE_DATALINK              0x00000005
#define FILE_DEVICE_DFS                   0x00000006
#define FILE_DEVICE_DISK                  0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM      0x00000008
#define FILE_DEVICE_FILE_SYSTEM           0x00000009
#define FILE_DEVICE_INPORT_PORT           0x0000000a
#define FILE_DEVICE_KEYBOARD              0x0000000b
#define FILE_DEVICE_MAILSLOT              0x0000000c
#define FILE_DEVICE_MIDI_IN               0x0000000d
#define FILE_DEVICE_MIDI_OUT              0x0000000e
#define FILE_DEVICE_MOUSE                 0x0000000f
#define FILE_DEVICE_MULTI_UNC_PROVIDER    0x00000010
#define FILE_DEVICE_NAMED_PIPE            0x00000011
#define FILE_DEVICE_NETWORK               0x00000012
#define FILE_DEVICE_NETWORK_BROWSER       0x00000013
#define FILE_DEVICE_NETWORK_FILE_SYSTEM   0x00000014
#define FILE_DEVICE_NULL                  0x00000015
#define FILE_DEVICE_PARALLEL_PORT         0x00000016
#define FILE_DEVICE_PHYSICAL_NETCARD      0x00000017
#define FILE_DEVICE_PRINTER               0x00000018
#define FILE_DEVICE_SCANNER               0x00000019
#define FILE_DEVICE_SERIAL_MOUSE_PORT     0x0000001a
#define FILE_DEVICE_SERIAL_PORT           0x0000001b
#define FILE_DEVICE_SCREEN                0x0000001c
#define FILE_DEVICE_SOUND                 0x0000001d
#define FILE_DEVICE_STREAMS               0x0000001e
#define FILE_DEVICE_TAPE                  0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM      0x00000020
#define FILE_DEVICE_TRANSPORT             0x00000021
#define FILE_DEVICE_UNKNOWN               0x00000022
#define FILE_DEVICE_VIDEO                 0x00000023
#define FILE_DEVICE_VIRTUAL_DISK          0x00000024
#define FILE_DEVICE_WAVE_IN               0x00000025
#define FILE_DEVICE_WAVE_OUT              0x00000026
#define FILE_DEVICE_8042_PORT             0x00000027
#define FILE_DEVICE_NETWORK_REDIRECTOR    0x00000028
#define FILE_DEVICE_BATTERY               0x00000029
#define FILE_DEVICE_BUS_EXTENDER          0x0000002a
#define FILE_DEVICE_MODEM                 0x0000002b
#define FILE_DEVICE_VDM                   0x0000002c
#define FILE_DEVICE_MASS_STORAGE          0x0000002d
#define FILE_DEVICE_SMB                   0x0000002e
#define FILE_DEVICE_KS                    0x0000002f
#define FILE_DEVICE_CHANGER               0x00000030
#define FILE_DEVICE_SMARTCARD             0x00000031
#define FILE_DEVICE_ACPI                  0x00000032
#define FILE_DEVICE_DVD                   0x00000033
#define FILE_DEVICE_FULLSCREEN_VIDEO      0x00000034
#define FILE_DEVICE_DFS_FILE_SYSTEM       0x00000035
#define FILE_DEVICE_DFS_VOLUME            0x00000036
#define FILE_DEVICE_SERENUM               0x00000037
#define FILE_DEVICE_TERMSRV               0x00000038
#define FILE_DEVICE_KSEC                  0x00000039
#define FILE_DEVICE_FIPS                  0x0000003A
#define FILE_DEVICE_INFINIBAND            0x0000003B
#define FILE_DEVICE_VMBUS                 0x0000003E
#define FILE_DEVICE_CRYPT_PROVIDER        0x0000003F
#define FILE_DEVICE_WPD                   0x00000040
#define FILE_DEVICE_BLUETOOTH             0x00000041
#define FILE_DEVICE_MT_COMPOSITE          0x00000042
#define FILE_DEVICE_MT_TRANSPORT          0x00000043
#define FILE_DEVICE_BIOMETRIC             0x00000044
#define FILE_DEVICE_PMI                   0x00000045

#if defined(NT_PROCESSOR_GROUPS)

typedef USHORT IRQ_DEVICE_POLICY, *PIRQ_DEVICE_POLICY;

typedef enum _IRQ_DEVICE_POLICY_USHORT {
  IrqPolicyMachineDefault = 0,
  IrqPolicyAllCloseProcessors = 1,
  IrqPolicyOneCloseProcessor = 2,
  IrqPolicyAllProcessorsInMachine = 3,
  IrqPolicyAllProcessorsInGroup = 3,
  IrqPolicySpecifiedProcessors = 4,
  IrqPolicySpreadMessagesAcrossAllProcessors = 5,
  IrqPolicyAllProcessorsInMachineWhenSteered = 6,
  IrqPolicyAllProcessorsInGroupWhenSteered = 6
};

#else /* defined(NT_PROCESSOR_GROUPS) */

typedef enum _IRQ_DEVICE_POLICY {
  IrqPolicyMachineDefault = 0,
  IrqPolicyAllCloseProcessors,
  IrqPolicyOneCloseProcessor,
  IrqPolicyAllProcessorsInMachine,
  IrqPolicySpecifiedProcessors,
  IrqPolicySpreadMessagesAcrossAllProcessors,
  IrqPolicyAllProcessorsInMachineWhenSteered
} IRQ_DEVICE_POLICY, *PIRQ_DEVICE_POLICY;

#endif

typedef enum _IRQ_PRIORITY {
  IrqPriorityUndefined = 0,
  IrqPriorityLow,
  IrqPriorityNormal,
  IrqPriorityHigh
} IRQ_PRIORITY, *PIRQ_PRIORITY;

typedef enum _IRQ_GROUP_POLICY {
  GroupAffinityAllGroupZero = 0,
  GroupAffinityDontCare
} IRQ_GROUP_POLICY, *PIRQ_GROUP_POLICY;

#define MAXIMUM_VOLUME_LABEL_LENGTH       (32 * sizeof(WCHAR))

typedef struct _OBJECT_HANDLE_INFORMATION {
  ULONG HandleAttributes;
  ACCESS_MASK GrantedAccess;
} OBJECT_HANDLE_INFORMATION, *POBJECT_HANDLE_INFORMATION;

typedef struct _CLIENT_ID {
  HANDLE UniqueProcess;
  HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _VPB {
  CSHORT Type;
  CSHORT Size;
  USHORT Flags;
  USHORT VolumeLabelLength;
  struct _DEVICE_OBJECT *DeviceObject;
  struct _DEVICE_OBJECT *RealDevice;
  ULONG SerialNumber;
  ULONG ReferenceCount;
  WCHAR VolumeLabel[MAXIMUM_VOLUME_LABEL_LENGTH / sizeof(WCHAR)];
} VPB, *PVPB;

typedef enum _IO_ALLOCATION_ACTION {
  KeepObject = 1,
  DeallocateObject,
  DeallocateObjectKeepRegisters
} IO_ALLOCATION_ACTION, *PIO_ALLOCATION_ACTION;

typedef IO_ALLOCATION_ACTION
(NTAPI DRIVER_CONTROL)(
  IN struct _DEVICE_OBJECT *DeviceObject,
  IN struct _IRP *Irp,
  IN PVOID MapRegisterBase,
  IN PVOID Context);
typedef DRIVER_CONTROL *PDRIVER_CONTROL;

typedef struct _WAIT_CONTEXT_BLOCK {
  KDEVICE_QUEUE_ENTRY WaitQueueEntry;
  PDRIVER_CONTROL DeviceRoutine;
  PVOID DeviceContext;
  ULONG NumberOfMapRegisters;
  PVOID DeviceObject;
  PVOID CurrentIrp;
  PKDPC BufferChainingDpc;
} WAIT_CONTEXT_BLOCK, *PWAIT_CONTEXT_BLOCK;

/* DEVICE_OBJECT.Flags */
#define DO_VERIFY_VOLUME                  0x00000002
#define DO_BUFFERED_IO                    0x00000004
#define DO_EXCLUSIVE                      0x00000008
#define DO_DIRECT_IO                      0x00000010
#define DO_MAP_IO_BUFFER                  0x00000020
#define DO_DEVICE_INITIALIZING            0x00000080
#define DO_SHUTDOWN_REGISTERED            0x00000800
#define DO_BUS_ENUMERATED_DEVICE          0x00001000
#define DO_POWER_PAGABLE                  0x00002000
#define DO_POWER_INRUSH                   0x00004000

/* DEVICE_OBJECT.Characteristics */
#define FILE_REMOVABLE_MEDIA              0x00000001
#define FILE_READ_ONLY_DEVICE             0x00000002
#define FILE_FLOPPY_DISKETTE              0x00000004
#define FILE_WRITE_ONCE_MEDIA             0x00000008
#define FILE_REMOTE_DEVICE                0x00000010
#define FILE_DEVICE_IS_MOUNTED            0x00000020
#define FILE_VIRTUAL_VOLUME               0x00000040
#define FILE_AUTOGENERATED_DEVICE_NAME    0x00000080
#define FILE_DEVICE_SECURE_OPEN           0x00000100
#define FILE_CHARACTERISTIC_PNP_DEVICE    0x00000800
#define FILE_CHARACTERISTIC_TS_DEVICE     0x00001000
#define FILE_CHARACTERISTIC_WEBDAV_DEVICE 0x00002000

/* DEVICE_OBJECT.AlignmentRequirement */
#define FILE_BYTE_ALIGNMENT             0x00000000
#define FILE_WORD_ALIGNMENT             0x00000001
#define FILE_LONG_ALIGNMENT             0x00000003
#define FILE_QUAD_ALIGNMENT             0x00000007
#define FILE_OCTA_ALIGNMENT             0x0000000f
#define FILE_32_BYTE_ALIGNMENT          0x0000001f
#define FILE_64_BYTE_ALIGNMENT          0x0000003f
#define FILE_128_BYTE_ALIGNMENT         0x0000007f
#define FILE_256_BYTE_ALIGNMENT         0x000000ff
#define FILE_512_BYTE_ALIGNMENT         0x000001ff

/* DEVICE_OBJECT.DeviceType */
#define DEVICE_TYPE ULONG

typedef struct _DEVICE_OBJECT {
  CSHORT Type;
  USHORT Size;
  LONG ReferenceCount;
  struct _DRIVER_OBJECT *DriverObject;
  struct _DEVICE_OBJECT *NextDevice;
  struct _DEVICE_OBJECT *AttachedDevice;
  struct _IRP *CurrentIrp;
  PIO_TIMER Timer;
  ULONG Flags;
  ULONG Characteristics;
  volatile PVPB Vpb;
  PVOID DeviceExtension;
  DEVICE_TYPE DeviceType;
  CCHAR StackSize;
  union {
    LIST_ENTRY ListEntry;
    WAIT_CONTEXT_BLOCK Wcb;
  } Queue;
  ULONG AlignmentRequirement;
  KDEVICE_QUEUE DeviceQueue;
  KDPC Dpc;
  ULONG ActiveThreadCount;
  PSECURITY_DESCRIPTOR SecurityDescriptor;
  KEVENT DeviceLock;
  USHORT SectorSize;
  USHORT Spare1;
  struct _DEVOBJ_EXTENSION *DeviceObjectExtension;
  PVOID Reserved;
} DEVICE_OBJECT, *PDEVICE_OBJECT;

typedef enum _IO_SESSION_STATE {
  IoSessionStateCreated = 1,
  IoSessionStateInitialized,
  IoSessionStateConnected,
  IoSessionStateDisconnected,
  IoSessionStateDisconnectedLoggedOn,
  IoSessionStateLoggedOn,
  IoSessionStateLoggedOff,
  IoSessionStateTerminated,
  IoSessionStateMax
} IO_SESSION_STATE, *PIO_SESSION_STATE;

typedef enum _IO_COMPLETION_ROUTINE_RESULT {
  ContinueCompletion = STATUS_CONTINUE_COMPLETION,
  StopCompletion = STATUS_MORE_PROCESSING_REQUIRED
} IO_COMPLETION_ROUTINE_RESULT, *PIO_COMPLETION_ROUTINE_RESULT;

typedef struct _IO_INTERRUPT_MESSAGE_INFO_ENTRY {
  PHYSICAL_ADDRESS MessageAddress;
  KAFFINITY TargetProcessorSet;
  PKINTERRUPT InterruptObject;
  ULONG MessageData;
  ULONG Vector;
  KIRQL Irql;
  KINTERRUPT_MODE Mode;
  KINTERRUPT_POLARITY Polarity;
} IO_INTERRUPT_MESSAGE_INFO_ENTRY, *PIO_INTERRUPT_MESSAGE_INFO_ENTRY;

typedef struct _IO_INTERRUPT_MESSAGE_INFO {
  KIRQL UnifiedIrql;
  ULONG MessageCount;
  IO_INTERRUPT_MESSAGE_INFO_ENTRY MessageInfo[1];
} IO_INTERRUPT_MESSAGE_INFO, *PIO_INTERRUPT_MESSAGE_INFO;

typedef struct _IO_CONNECT_INTERRUPT_FULLY_SPECIFIED_PARAMETERS {
  IN PDEVICE_OBJECT PhysicalDeviceObject;
  OUT PKINTERRUPT *InterruptObject;
  IN PKSERVICE_ROUTINE ServiceRoutine;
  IN PVOID ServiceContext;
  IN PKSPIN_LOCK SpinLock OPTIONAL;
  IN KIRQL SynchronizeIrql;
  IN BOOLEAN FloatingSave;
  IN BOOLEAN ShareVector;
  IN ULONG Vector;
  IN KIRQL Irql;
  IN KINTERRUPT_MODE InterruptMode;
  IN KAFFINITY ProcessorEnableMask;
  IN USHORT Group;
} IO_CONNECT_INTERRUPT_FULLY_SPECIFIED_PARAMETERS, *PIO_CONNECT_INTERRUPT_FULLY_SPECIFIED_PARAMETERS;

typedef struct _IO_CONNECT_INTERRUPT_LINE_BASED_PARAMETERS {
  IN PDEVICE_OBJECT PhysicalDeviceObject;
  OUT PKINTERRUPT *InterruptObject;
  IN PKSERVICE_ROUTINE ServiceRoutine;
  IN PVOID ServiceContext;
  IN PKSPIN_LOCK SpinLock OPTIONAL;
  IN KIRQL SynchronizeIrql OPTIONAL;
  IN BOOLEAN FloatingSave;
} IO_CONNECT_INTERRUPT_LINE_BASED_PARAMETERS, *PIO_CONNECT_INTERRUPT_LINE_BASED_PARAMETERS;

typedef struct _IO_CONNECT_INTERRUPT_MESSAGE_BASED_PARAMETERS {
  IN PDEVICE_OBJECT PhysicalDeviceObject;
  union {
    OUT PVOID *Generic;
    OUT PIO_INTERRUPT_MESSAGE_INFO *InterruptMessageTable;
    OUT PKINTERRUPT *InterruptObject;
  } ConnectionContext;
  IN PKMESSAGE_SERVICE_ROUTINE MessageServiceRoutine;
  IN PVOID ServiceContext;
  IN PKSPIN_LOCK SpinLock OPTIONAL;
  IN KIRQL SynchronizeIrql OPTIONAL;
  IN BOOLEAN FloatingSave;
  IN PKSERVICE_ROUTINE FallBackServiceRoutine OPTIONAL;
} IO_CONNECT_INTERRUPT_MESSAGE_BASED_PARAMETERS, *PIO_CONNECT_INTERRUPT_MESSAGE_BASED_PARAMETERS;

typedef struct _IO_CONNECT_INTERRUPT_PARAMETERS {
  IN OUT ULONG Version;
  _ANONYMOUS_UNION union {
    IO_CONNECT_INTERRUPT_FULLY_SPECIFIED_PARAMETERS FullySpecified;
    IO_CONNECT_INTERRUPT_LINE_BASED_PARAMETERS LineBased;
    IO_CONNECT_INTERRUPT_MESSAGE_BASED_PARAMETERS MessageBased;
  } DUMMYUNIONNAME;
} IO_CONNECT_INTERRUPT_PARAMETERS, *PIO_CONNECT_INTERRUPT_PARAMETERS;

typedef struct _IO_DISCONNECT_INTERRUPT_PARAMETERS {
  IN ULONG Version;
  union {
    IN PVOID Generic;
    IN PKINTERRUPT InterruptObject;
    IN PIO_INTERRUPT_MESSAGE_INFO InterruptMessageTable;
  } ConnectionContext;
} IO_DISCONNECT_INTERRUPT_PARAMETERS, *PIO_DISCONNECT_INTERRUPT_PARAMETERS;

typedef enum _IO_ACCESS_TYPE {
  ReadAccess,
  WriteAccess,
  ModifyAccess
} IO_ACCESS_TYPE;

typedef enum _IO_ACCESS_MODE {
  SequentialAccess,
  RandomAccess
} IO_ACCESS_MODE;

typedef enum _IO_CONTAINER_NOTIFICATION_CLASS {
  IoSessionStateNotification,
  IoMaxContainerNotificationClass
} IO_CONTAINER_NOTIFICATION_CLASS;

typedef struct _IO_SESSION_STATE_NOTIFICATION {
  ULONG Size;
  ULONG Flags;
  PVOID IoObject;
  ULONG EventMask;
  PVOID Context;
} IO_SESSION_STATE_NOTIFICATION, *PIO_SESSION_STATE_NOTIFICATION;

typedef enum _IO_CONTAINER_INFORMATION_CLASS {
  IoSessionStateInformation,
  IoMaxContainerInformationClass
} IO_CONTAINER_INFORMATION_CLASS;

typedef struct _IO_SESSION_STATE_INFORMATION {
  ULONG SessionId;
  IO_SESSION_STATE SessionState;
  BOOLEAN LocalSession;
} IO_SESSION_STATE_INFORMATION, *PIO_SESSION_STATE_INFORMATION;

#if (NTDDI_VERSION >= NTDDI_WIN7)

typedef NTSTATUS
(NTAPI *PIO_CONTAINER_NOTIFICATION_FUNCTION)(
  VOID);

typedef NTSTATUS
(NTAPI IO_SESSION_NOTIFICATION_FUNCTION)(
  IN PVOID SessionObject,
  IN PVOID IoObject,
  IN ULONG Event,
  IN PVOID Context,
  IN PVOID NotificationPayload,
  IN ULONG PayloadLength);

typedef IO_SESSION_NOTIFICATION_FUNCTION *PIO_SESSION_NOTIFICATION_FUNCTION;

#endif

typedef struct _IO_REMOVE_LOCK_TRACKING_BLOCK * PIO_REMOVE_LOCK_TRACKING_BLOCK;

typedef struct _IO_REMOVE_LOCK_COMMON_BLOCK {
  BOOLEAN Removed;
  BOOLEAN Reserved[3];
  volatile LONG IoCount;
  KEVENT RemoveEvent;
} IO_REMOVE_LOCK_COMMON_BLOCK;

typedef struct _IO_REMOVE_LOCK_DBG_BLOCK {
  LONG Signature;
  LONG HighWatermark;
  LONGLONG MaxLockedTicks;
  LONG AllocateTag;
  LIST_ENTRY LockList;
  KSPIN_LOCK Spin;
  volatile LONG LowMemoryCount;
  ULONG Reserved1[4];
  PVOID Reserved2;
  PIO_REMOVE_LOCK_TRACKING_BLOCK Blocks;
} IO_REMOVE_LOCK_DBG_BLOCK;

typedef struct _IO_REMOVE_LOCK {
  IO_REMOVE_LOCK_COMMON_BLOCK Common;
#if DBG
  IO_REMOVE_LOCK_DBG_BLOCK Dbg;
#endif
} IO_REMOVE_LOCK, *PIO_REMOVE_LOCK;

typedef struct _IO_WORKITEM *PIO_WORKITEM;

typedef VOID
(NTAPI IO_WORKITEM_ROUTINE)(
  IN PDEVICE_OBJECT DeviceObject,
  IN PVOID Context);
typedef IO_WORKITEM_ROUTINE *PIO_WORKITEM_ROUTINE;

typedef VOID
(NTAPI IO_WORKITEM_ROUTINE_EX)(
  IN PVOID IoObject,
  IN PVOID Context OPTIONAL,
  IN PIO_WORKITEM IoWorkItem);
typedef IO_WORKITEM_ROUTINE_EX *PIO_WORKITEM_ROUTINE_EX;

typedef struct _SHARE_ACCESS {
  ULONG OpenCount;
  ULONG Readers;
  ULONG Writers;
  ULONG Deleters;
  ULONG SharedRead;
  ULONG SharedWrite;
  ULONG SharedDelete;
} SHARE_ACCESS, *PSHARE_ACCESS;

/* While MS WDK uses inheritance in C++, we cannot do this with gcc, as
   inheritance, even from a struct renders the type non-POD. So we use
   this hack */
#define PCI_COMMON_HEADER_LAYOUT                \
  USHORT VendorID;                              \
  USHORT DeviceID;                              \
  USHORT Command;                               \
  USHORT Status;                                \
  UCHAR RevisionID;                             \
  UCHAR ProgIf;                                 \
  UCHAR SubClass;                               \
  UCHAR BaseClass;                              \
  UCHAR CacheLineSize;                          \
  UCHAR LatencyTimer;                           \
  UCHAR HeaderType;                             \
  UCHAR BIST;                                   \
  union {                                       \
    struct /* _PCI_HEADER_TYPE_0 */ {                 \
      ULONG BaseAddresses[PCI_TYPE0_ADDRESSES]; \
      ULONG CIS;                                \
      USHORT SubVendorID;                       \
      USHORT SubSystemID;                       \
      ULONG ROMBaseAddress;                     \
      UCHAR CapabilitiesPtr;                    \
      UCHAR Reserved1[3];                       \
      ULONG Reserved2;                          \
      UCHAR InterruptLine;                      \
      UCHAR InterruptPin;                       \
      UCHAR MinimumGrant;                       \
      UCHAR MaximumLatency;                     \
    } type0;                                    \
    struct /* _PCI_HEADER_TYPE_1 */ {                 \
      ULONG BaseAddresses[PCI_TYPE1_ADDRESSES]; \
      UCHAR PrimaryBus;                         \
      UCHAR SecondaryBus;                       \
      UCHAR SubordinateBus;                     \
      UCHAR SecondaryLatency;                   \
      UCHAR IOBase;                             \
      UCHAR IOLimit;                            \
      USHORT SecondaryStatus;                   \
      USHORT MemoryBase;                        \
      USHORT MemoryLimit;                       \
      USHORT PrefetchBase;                      \
      USHORT PrefetchLimit;                     \
      ULONG PrefetchBaseUpper32;                \
      ULONG PrefetchLimitUpper32;               \
      USHORT IOBaseUpper16;                     \
      USHORT IOLimitUpper16;                    \
      UCHAR CapabilitiesPtr;                    \
      UCHAR Reserved1[3];                       \
      ULONG ROMBaseAddress;                     \
      UCHAR InterruptLine;                      \
      UCHAR InterruptPin;                       \
      USHORT BridgeControl;                     \
    } type1;                                    \
    struct /* _PCI_HEADER_TYPE_2 */ {                 \
      ULONG SocketRegistersBaseAddress;         \
      UCHAR CapabilitiesPtr;                    \
      UCHAR Reserved;                           \
      USHORT SecondaryStatus;                   \
      UCHAR PrimaryBus;                         \
      UCHAR SecondaryBus;                       \
      UCHAR SubordinateBus;                     \
      UCHAR SecondaryLatency;                   \
      struct {                                  \
        ULONG Base;                             \
        ULONG Limit;                            \
      } Range[PCI_TYPE2_ADDRESSES-1];           \
      UCHAR InterruptLine;                      \
      UCHAR InterruptPin;                       \
      USHORT BridgeControl;                     \
    } type2;                                    \
  } u;

typedef enum _CREATE_FILE_TYPE {
  CreateFileTypeNone,
  CreateFileTypeNamedPipe,
  CreateFileTypeMailslot
} CREATE_FILE_TYPE;

typedef struct _NAMED_PIPE_CREATE_PARAMETERS {
  ULONG NamedPipeType;
  ULONG ReadMode;
  ULONG CompletionMode;
  ULONG MaximumInstances;
  ULONG InboundQuota;
  ULONG OutboundQuota;
  LARGE_INTEGER DefaultTimeout;
  BOOLEAN TimeoutSpecified;
} NAMED_PIPE_CREATE_PARAMETERS, *PNAMED_PIPE_CREATE_PARAMETERS;

typedef struct _MAILSLOT_CREATE_PARAMETERS {
  ULONG MailslotQuota;
  ULONG MaximumMessageSize;
  LARGE_INTEGER ReadTimeout;
  BOOLEAN TimeoutSpecified;
} MAILSLOT_CREATE_PARAMETERS, *PMAILSLOT_CREATE_PARAMETERS;

#define IO_FORCE_ACCESS_CHECK               0x001
#define IO_NO_PARAMETER_CHECKING            0x100

#define IO_REPARSE                      0x0
#define IO_REMOUNT                      0x1

typedef struct _IO_STATUS_BLOCK {
  _ANONYMOUS_UNION union {
    NTSTATUS Status;
    PVOID Pointer;
  } DUMMYUNIONNAME;
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

#if defined(_WIN64)
typedef struct _IO_STATUS_BLOCK32 {
  NTSTATUS Status;
  ULONG Information;
} IO_STATUS_BLOCK32, *PIO_STATUS_BLOCK32;
#endif

typedef VOID
(NTAPI *PIO_APC_ROUTINE)(
  IN PVOID ApcContext,
  IN PIO_STATUS_BLOCK IoStatusBlock,
  IN ULONG Reserved);

#define PIO_APC_ROUTINE_DEFINED

typedef enum _IO_SESSION_EVENT {
  IoSessionEventIgnore = 0,
  IoSessionEventCreated,
  IoSessionEventTerminated,
  IoSessionEventConnected,
  IoSessionEventDisconnected,
  IoSessionEventLogon,
  IoSessionEventLogoff,
  IoSessionEventMax
} IO_SESSION_EVENT, *PIO_SESSION_EVENT;

#define IO_SESSION_STATE_ALL_EVENTS        0xffffffff
#define IO_SESSION_STATE_CREATION_EVENT    0x00000001
#define IO_SESSION_STATE_TERMINATION_EVENT 0x00000002
#define IO_SESSION_STATE_CONNECT_EVENT     0x00000004
#define IO_SESSION_STATE_DISCONNECT_EVENT  0x00000008
#define IO_SESSION_STATE_LOGON_EVENT       0x00000010
#define IO_SESSION_STATE_LOGOFF_EVENT      0x00000020

#define IO_SESSION_STATE_VALID_EVENT_MASK  0x0000003f

#define IO_SESSION_MAX_PAYLOAD_SIZE        256L

typedef struct _IO_SESSION_CONNECT_INFO {
  ULONG SessionId;
  BOOLEAN LocalSession;
} IO_SESSION_CONNECT_INFO, *PIO_SESSION_CONNECT_INFO;

#define EVENT_INCREMENT                   1
#define IO_NO_INCREMENT                   0
#define IO_CD_ROM_INCREMENT               1
#define IO_DISK_INCREMENT                 1
#define IO_KEYBOARD_INCREMENT             6
#define IO_MAILSLOT_INCREMENT             2
#define IO_MOUSE_INCREMENT                6
#define IO_NAMED_PIPE_INCREMENT           2
#define IO_NETWORK_INCREMENT              2
#define IO_PARALLEL_INCREMENT             1
#define IO_SERIAL_INCREMENT               2
#define IO_SOUND_INCREMENT                8
#define IO_VIDEO_INCREMENT                1
#define SEMAPHORE_INCREMENT               1

#define MM_MAXIMUM_DISK_IO_SIZE          (0x10000)

typedef struct _BOOTDISK_INFORMATION {
  LONGLONG BootPartitionOffset;
  LONGLONG SystemPartitionOffset;
  ULONG BootDeviceSignature;
  ULONG SystemDeviceSignature;
} BOOTDISK_INFORMATION, *PBOOTDISK_INFORMATION;

typedef struct _BOOTDISK_INFORMATION_EX {
  LONGLONG BootPartitionOffset;
  LONGLONG SystemPartitionOffset;
  ULONG BootDeviceSignature;
  ULONG SystemDeviceSignature;
  GUID BootDeviceGuid;
  GUID SystemDeviceGuid;
  BOOLEAN BootDeviceIsGpt;
  BOOLEAN SystemDeviceIsGpt;
} BOOTDISK_INFORMATION_EX, *PBOOTDISK_INFORMATION_EX;

#if (NTDDI_VERSION >= NTDDI_WIN7)

typedef struct _LOADER_PARTITION_INFORMATION_EX {
  ULONG PartitionStyle;
  ULONG PartitionNumber;
  _ANONYMOUS_UNION union {
    ULONG Signature;
    GUID DeviceId;
  } DUMMYUNIONNAME;
  ULONG Flags;
} LOADER_PARTITION_INFORMATION_EX, *PLOADER_PARTITION_INFORMATION_EX;

typedef struct _BOOTDISK_INFORMATION_LITE {
  ULONG NumberEntries;
  LOADER_PARTITION_INFORMATION_EX Entries[1];
} BOOTDISK_INFORMATION_LITE, *PBOOTDISK_INFORMATION_LITE;

#else

#if (NTDDI_VERSION >= NTDDI_VISTA)
typedef struct _BOOTDISK_INFORMATION_LITE {
  ULONG BootDeviceSignature;
  ULONG SystemDeviceSignature;
  GUID BootDeviceGuid;
  GUID SystemDeviceGuid;
  BOOLEAN BootDeviceIsGpt;
  BOOLEAN SystemDeviceIsGpt;
} BOOTDISK_INFORMATION_LITE, *PBOOTDISK_INFORMATION_LITE;
#endif /* (NTDDI_VERSION >= NTDDI_VISTA) */

#endif /* (NTDDI_VERSION >= NTDDI_WIN7) */

#include <pshpack1.h>

typedef struct _EISA_MEMORY_TYPE {
  UCHAR ReadWrite:1;
  UCHAR Cached:1;
  UCHAR Reserved0:1;
  UCHAR Type:2;
  UCHAR Shared:1;
  UCHAR Reserved1:1;
  UCHAR MoreEntries:1;
} EISA_MEMORY_TYPE, *PEISA_MEMORY_TYPE;

typedef struct _EISA_MEMORY_CONFIGURATION {
  EISA_MEMORY_TYPE ConfigurationByte;
  UCHAR DataSize;
  USHORT AddressLowWord;
  UCHAR AddressHighByte;
  USHORT MemorySize;
} EISA_MEMORY_CONFIGURATION, *PEISA_MEMORY_CONFIGURATION;

typedef struct _EISA_IRQ_DESCRIPTOR {
  UCHAR Interrupt:4;
  UCHAR Reserved:1;
  UCHAR LevelTriggered:1;
  UCHAR Shared:1;
  UCHAR MoreEntries:1;
} EISA_IRQ_DESCRIPTOR, *PEISA_IRQ_DESCRIPTOR;

typedef struct _EISA_IRQ_CONFIGURATION {
  EISA_IRQ_DESCRIPTOR ConfigurationByte;
  UCHAR Reserved;
} EISA_IRQ_CONFIGURATION, *PEISA_IRQ_CONFIGURATION;

typedef struct _DMA_CONFIGURATION_BYTE0 {
  UCHAR Channel:3;
  UCHAR Reserved:3;
  UCHAR Shared:1;
  UCHAR MoreEntries:1;
} DMA_CONFIGURATION_BYTE0;

typedef struct _DMA_CONFIGURATION_BYTE1 {
  UCHAR Reserved0:2;
  UCHAR TransferSize:2;
  UCHAR Timing:2;
  UCHAR Reserved1:2;
} DMA_CONFIGURATION_BYTE1;

typedef struct _EISA_DMA_CONFIGURATION {
  DMA_CONFIGURATION_BYTE0 ConfigurationByte0;
  DMA_CONFIGURATION_BYTE1 ConfigurationByte1;
} EISA_DMA_CONFIGURATION, *PEISA_DMA_CONFIGURATION;

typedef struct _EISA_PORT_DESCRIPTOR {
  UCHAR NumberPorts:5;
  UCHAR Reserved:1;
  UCHAR Shared:1;
  UCHAR MoreEntries:1;
} EISA_PORT_DESCRIPTOR, *PEISA_PORT_DESCRIPTOR;

typedef struct _EISA_PORT_CONFIGURATION {
  EISA_PORT_DESCRIPTOR Configuration;
  USHORT PortAddress;
} EISA_PORT_CONFIGURATION, *PEISA_PORT_CONFIGURATION;

typedef struct _CM_EISA_SLOT_INFORMATION {
  UCHAR ReturnCode;
  UCHAR ReturnFlags;
  UCHAR MajorRevision;
  UCHAR MinorRevision;
  USHORT Checksum;
  UCHAR NumberFunctions;
  UCHAR FunctionInformation;
  ULONG CompressedId;
} CM_EISA_SLOT_INFORMATION, *PCM_EISA_SLOT_INFORMATION;

typedef struct _CM_EISA_FUNCTION_INFORMATION {
  ULONG CompressedId;
  UCHAR IdSlotFlags1;
  UCHAR IdSlotFlags2;
  UCHAR MinorRevision;
  UCHAR MajorRevision;
  UCHAR Selections[26];
  UCHAR FunctionFlags;
  UCHAR TypeString[80];
  EISA_MEMORY_CONFIGURATION EisaMemory[9];
  EISA_IRQ_CONFIGURATION EisaIrq[7];
  EISA_DMA_CONFIGURATION EisaDma[4];
  EISA_PORT_CONFIGURATION EisaPort[20];
  UCHAR InitializationData[60];
} CM_EISA_FUNCTION_INFORMATION, *PCM_EISA_FUNCTION_INFORMATION;

#include <poppack.h>

/* CM_EISA_FUNCTION_INFORMATION.FunctionFlags */

#define EISA_FUNCTION_ENABLED           0x80
#define EISA_FREE_FORM_DATA             0x40
#define EISA_HAS_PORT_INIT_ENTRY        0x20
#define EISA_HAS_PORT_RANGE             0x10
#define EISA_HAS_DMA_ENTRY              0x08
#define EISA_HAS_IRQ_ENTRY              0x04
#define EISA_HAS_MEMORY_ENTRY           0x02
#define EISA_HAS_TYPE_ENTRY             0x01
#define EISA_HAS_INFORMATION \
  (EISA_HAS_PORT_RANGE + EISA_HAS_DMA_ENTRY + EISA_HAS_IRQ_ENTRY \
  + EISA_HAS_MEMORY_ENTRY + EISA_HAS_TYPE_ENTRY)

#define EISA_MORE_ENTRIES               0x80
#define EISA_SYSTEM_MEMORY              0x00
#define EISA_MEMORY_TYPE_RAM            0x01

/* CM_EISA_SLOT_INFORMATION.ReturnCode */

#define EISA_INVALID_SLOT               0x80
#define EISA_INVALID_FUNCTION           0x81
#define EISA_INVALID_CONFIGURATION      0x82
#define EISA_EMPTY_SLOT                 0x83
#define EISA_INVALID_BIOS_CALL          0x86

/*
** Plug and Play structures
*/

typedef VOID
(NTAPI *PINTERFACE_REFERENCE)(
  PVOID Context);

typedef VOID
(NTAPI *PINTERFACE_DEREFERENCE)(
  PVOID Context);

typedef BOOLEAN
(NTAPI TRANSLATE_BUS_ADDRESS)(
  IN PVOID Context,
  IN PHYSICAL_ADDRESS BusAddress,
  IN ULONG Length,
  IN OUT PULONG AddressSpace,
  OUT PPHYSICAL_ADDRESS  TranslatedAddress);
typedef TRANSLATE_BUS_ADDRESS *PTRANSLATE_BUS_ADDRESS;

typedef struct _DMA_ADAPTER*
(NTAPI GET_DMA_ADAPTER)(
  IN PVOID Context,
  IN struct _DEVICE_DESCRIPTION *DeviceDescriptor,
  OUT PULONG NumberOfMapRegisters);
typedef GET_DMA_ADAPTER *PGET_DMA_ADAPTER;

typedef ULONG
(NTAPI GET_SET_DEVICE_DATA)(
  IN PVOID Context,
  IN ULONG DataType,
  IN PVOID Buffer,
  IN ULONG Offset,
  IN ULONG Length);
typedef GET_SET_DEVICE_DATA *PGET_SET_DEVICE_DATA;

typedef enum _DEVICE_INSTALL_STATE {
  InstallStateInstalled,
  InstallStateNeedsReinstall,
  InstallStateFailedInstall,
  InstallStateFinishInstall
} DEVICE_INSTALL_STATE, *PDEVICE_INSTALL_STATE;

typedef struct _LEGACY_BUS_INFORMATION {
  GUID BusTypeGuid;
  INTERFACE_TYPE LegacyBusType;
  ULONG BusNumber;
} LEGACY_BUS_INFORMATION, *PLEGACY_BUS_INFORMATION;

typedef enum _DEVICE_REMOVAL_POLICY {
  RemovalPolicyExpectNoRemoval = 1,
  RemovalPolicyExpectOrderlyRemoval = 2,
  RemovalPolicyExpectSurpriseRemoval = 3
} DEVICE_REMOVAL_POLICY, *PDEVICE_REMOVAL_POLICY;

typedef VOID
(NTAPI*PREENUMERATE_SELF)(
  IN PVOID Context);

typedef struct _REENUMERATE_SELF_INTERFACE_STANDARD {
  USHORT Size;
  USHORT Version;
  PVOID Context;
  PINTERFACE_REFERENCE InterfaceReference;
  PINTERFACE_DEREFERENCE InterfaceDereference;
  PREENUMERATE_SELF SurpriseRemoveAndReenumerateSelf;
} REENUMERATE_SELF_INTERFACE_STANDARD, *PREENUMERATE_SELF_INTERFACE_STANDARD;

typedef VOID
(NTAPI *PIO_DEVICE_EJECT_CALLBACK)(
  IN NTSTATUS Status,
  IN OUT PVOID Context OPTIONAL);

#define PCI_DEVICE_PRESENT_INTERFACE_VERSION     1

/* PCI_DEVICE_PRESENCE_PARAMETERS.Flags */
#define PCI_USE_SUBSYSTEM_IDS   0x00000001
#define PCI_USE_REVISION        0x00000002
#define PCI_USE_VENDEV_IDS      0x00000004
#define PCI_USE_CLASS_SUBCLASS  0x00000008
#define PCI_USE_PROGIF          0x00000010
#define PCI_USE_LOCAL_BUS       0x00000020
#define PCI_USE_LOCAL_DEVICE    0x00000040

typedef struct _PCI_DEVICE_PRESENCE_PARAMETERS {
  ULONG Size;
  ULONG Flags;
  USHORT VendorID;
  USHORT DeviceID;
  UCHAR RevisionID;
  USHORT SubVendorID;
  USHORT SubSystemID;
  UCHAR BaseClass;
  UCHAR SubClass;
  UCHAR ProgIf;
} PCI_DEVICE_PRESENCE_PARAMETERS, *PPCI_DEVICE_PRESENCE_PARAMETERS;

typedef BOOLEAN
(NTAPI PCI_IS_DEVICE_PRESENT)(
  IN USHORT VendorID,
  IN USHORT DeviceID,
  IN UCHAR RevisionID,
  IN USHORT SubVendorID,
  IN USHORT SubSystemID,
  IN ULONG Flags);
typedef PCI_IS_DEVICE_PRESENT *PPCI_IS_DEVICE_PRESENT;

typedef BOOLEAN
(NTAPI PCI_IS_DEVICE_PRESENT_EX)(
  IN PVOID Context,
  IN PPCI_DEVICE_PRESENCE_PARAMETERS Parameters);
typedef PCI_IS_DEVICE_PRESENT_EX *PPCI_IS_DEVICE_PRESENT_EX;

typedef struct _BUS_INTERFACE_STANDARD {
  USHORT Size;
  USHORT Version;
  PVOID Context;
  PINTERFACE_REFERENCE InterfaceReference;
  PINTERFACE_DEREFERENCE InterfaceDereference;
  PTRANSLATE_BUS_ADDRESS TranslateBusAddress;
  PGET_DMA_ADAPTER GetDmaAdapter;
  PGET_SET_DEVICE_DATA SetBusData;
  PGET_SET_DEVICE_DATA GetBusData;
} BUS_INTERFACE_STANDARD, *PBUS_INTERFACE_STANDARD;

typedef struct _PCI_DEVICE_PRESENT_INTERFACE {
  USHORT Size;
  USHORT Version;
  PVOID Context;
  PINTERFACE_REFERENCE InterfaceReference;
  PINTERFACE_DEREFERENCE InterfaceDereference;
  PPCI_IS_DEVICE_PRESENT IsDevicePresent;
  PPCI_IS_DEVICE_PRESENT_EX IsDevicePresentEx;
} PCI_DEVICE_PRESENT_INTERFACE, *PPCI_DEVICE_PRESENT_INTERFACE;

typedef struct _DEVICE_CAPABILITIES {
  USHORT Size;
  USHORT Version;
  ULONG DeviceD1:1;
  ULONG DeviceD2:1;
  ULONG LockSupported:1;
  ULONG EjectSupported:1;
  ULONG Removable:1;
  ULONG DockDevice:1;
  ULONG UniqueID:1;
  ULONG SilentInstall:1;
  ULONG RawDeviceOK:1;
  ULONG SurpriseRemovalOK:1;
  ULONG WakeFromD0:1;
  ULONG WakeFromD1:1;
  ULONG WakeFromD2:1;
  ULONG WakeFromD3:1;
  ULONG HardwareDisabled:1;
  ULONG NonDynamic:1;
  ULONG WarmEjectSupported:1;
  ULONG NoDisplayInUI:1;
  ULONG Reserved:14;
  ULONG Address;
  ULONG UINumber;
  DEVICE_POWER_STATE DeviceState[PowerSystemMaximum];
  SYSTEM_POWER_STATE SystemWake;
  DEVICE_POWER_STATE DeviceWake;
  ULONG D1Latency;
  ULONG D2Latency;
  ULONG D3Latency;
} DEVICE_CAPABILITIES, *PDEVICE_CAPABILITIES;

typedef struct _DEVICE_INTERFACE_CHANGE_NOTIFICATION {
  USHORT Version;
  USHORT Size;
  GUID Event;
  GUID InterfaceClassGuid;
  PUNICODE_STRING SymbolicLinkName;
} DEVICE_INTERFACE_CHANGE_NOTIFICATION, *PDEVICE_INTERFACE_CHANGE_NOTIFICATION;

typedef struct _HWPROFILE_CHANGE_NOTIFICATION {
  USHORT Version;
  USHORT Size;
  GUID Event;
} HWPROFILE_CHANGE_NOTIFICATION, *PHWPROFILE_CHANGE_NOTIFICATION;

#undef INTERFACE

typedef struct _INTERFACE {
  USHORT Size;
  USHORT Version;
  PVOID Context;
  PINTERFACE_REFERENCE InterfaceReference;
  PINTERFACE_DEREFERENCE InterfaceDereference;
} INTERFACE, *PINTERFACE;

typedef struct _PLUGPLAY_NOTIFICATION_HEADER {
  USHORT Version;
  USHORT Size;
  GUID Event;
} PLUGPLAY_NOTIFICATION_HEADER, *PPLUGPLAY_NOTIFICATION_HEADER;

typedef ULONG PNP_DEVICE_STATE, *PPNP_DEVICE_STATE;

/* PNP_DEVICE_STATE */

#define PNP_DEVICE_DISABLED                      0x00000001
#define PNP_DEVICE_DONT_DISPLAY_IN_UI            0x00000002
#define PNP_DEVICE_FAILED                        0x00000004
#define PNP_DEVICE_REMOVED                       0x00000008
#define PNP_DEVICE_RESOURCE_REQUIREMENTS_CHANGED 0x00000010
#define PNP_DEVICE_NOT_DISABLEABLE               0x00000020

typedef struct _TARGET_DEVICE_CUSTOM_NOTIFICATION {
  USHORT Version;
  USHORT Size;
  GUID Event;
  struct _FILE_OBJECT *FileObject;
  LONG NameBufferOffset;
  UCHAR CustomDataBuffer[1];
} TARGET_DEVICE_CUSTOM_NOTIFICATION, *PTARGET_DEVICE_CUSTOM_NOTIFICATION;

typedef struct _TARGET_DEVICE_REMOVAL_NOTIFICATION {
  USHORT Version;
  USHORT Size;
  GUID Event;
  struct _FILE_OBJECT *FileObject;
} TARGET_DEVICE_REMOVAL_NOTIFICATION, *PTARGET_DEVICE_REMOVAL_NOTIFICATION;

#if (NTDDI_VERSION >= NTDDI_VISTA)
#include <devpropdef.h>
#define PLUGPLAY_PROPERTY_PERSISTENT   0x00000001
#endif

#define PNP_REPLACE_NO_MAP             MAXLONGLONG

typedef NTSTATUS
(NTAPI *PREPLACE_MAP_MEMORY)(
  IN PHYSICAL_ADDRESS TargetPhysicalAddress,
  IN PHYSICAL_ADDRESS SparePhysicalAddress,
  IN OUT PLARGE_INTEGER NumberOfBytes,
  OUT PVOID *TargetAddress,
  OUT PVOID *SpareAddress);

typedef struct _PNP_REPLACE_MEMORY_LIST {
  ULONG AllocatedCount;
  ULONG Count;
  ULONGLONG TotalLength;
  struct {
    PHYSICAL_ADDRESS Address;
    ULONGLONG Length;
  } Ranges[ANYSIZE_ARRAY];
} PNP_REPLACE_MEMORY_LIST, *PPNP_REPLACE_MEMORY_LIST;

typedef struct _PNP_REPLACE_PROCESSOR_LIST {
  PKAFFINITY Affinity;
  ULONG GroupCount;
  ULONG AllocatedCount;
  ULONG Count;
  ULONG ApicIds[ANYSIZE_ARRAY];
} PNP_REPLACE_PROCESSOR_LIST, *PPNP_REPLACE_PROCESSOR_LIST;

typedef struct _PNP_REPLACE_PROCESSOR_LIST_V1 {
  KAFFINITY AffinityMask;
  ULONG AllocatedCount;
  ULONG Count;
  ULONG ApicIds[ANYSIZE_ARRAY];
} PNP_REPLACE_PROCESSOR_LIST_V1, *PPNP_REPLACE_PROCESSOR_LIST_V1;

#define PNP_REPLACE_PARAMETERS_VERSION           2

typedef struct _PNP_REPLACE_PARAMETERS {
  ULONG Size;
  ULONG Version;
  ULONG64 Target;
  ULONG64 Spare;
  PPNP_REPLACE_PROCESSOR_LIST TargetProcessors;
  PPNP_REPLACE_PROCESSOR_LIST SpareProcessors;
  PPNP_REPLACE_MEMORY_LIST TargetMemory;
  PPNP_REPLACE_MEMORY_LIST SpareMemory;
  PREPLACE_MAP_MEMORY MapMemory;
} PNP_REPLACE_PARAMETERS, *PPNP_REPLACE_PARAMETERS;

typedef VOID