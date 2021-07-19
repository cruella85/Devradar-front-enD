/**
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER within this package.
 */

#include <apiset.h>
#include <apisetcconv.h>

#include <rpc.h>
#include <rpcndr.h>

#ifndef DECLSPEC_NOTHROW
#define DECLSPEC_NOTHROW
#endif

#if (NTDDI_VERSION >= 0x06000000 && !defined (_WIN32_WINNT))
#define _WIN32_WINNT 0x0600
#endif

#if (NTDDI_VERSION >= 0x05020000 && !defined (_WIN32_WINNT))
#define _WIN32_WINNT 0x0502
#endif

#if (NTDDI_VERSION >= 0x05010000 && !defined (_WIN32_WINNT))
#define _WIN32_WINNT 0x0501
#endif

#ifndef _COMBASEAPI_H_
#define _COMBASEAPI_H_

#include <pshpack8.h>

#ifdef _OLE32_
#define WINOLEAPI STDAPI
#define WINOLEAPI_(type) STDAPI_(type)
#else
#define WINOLEAPI EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define WINOLEAPI_(type) EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif

#if WINAPI_FAMILY_PARTITION (WINAPI_PARTITION_APP)
#if defined (__cplusplus) && !defined (CINTERFACE)

#ifdef COM_STDMETHOD_CAN_THROW
#define COM_DECLSPEC_NOTHROW
#else
#define COM_DECLSPEC_NOTHROW DECLSPEC_NOTHROW
#endif

#define __STRUCT__ struct
#undef interface
#define interface __STRUCT__
#define STDMETHOD(method) virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type, method) virtual COM_DECLSPEC_NOTHROW type STDMETHODCALLTYPE method
#define STDMETHODV(method) virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODVCALLTYPE method
#define STDMETHODV_(type, method) virtual COM_DECLSPEC_NOTHROW type STDMETHODVCALLTYPE method
#define PURE = 0
#define THIS_
#define THIS void
#define DECLARE_INTERFACE(iface) interface DECLSPEC_NOVTABLE iface
#define DECLARE_INTERFACE_(iface, baseiface) interface DECLSPEC_NOVTABLE iface : public baseiface
#define DECLARE_INTERFACE_IID(iface, iid) interface DECLSPEC_UUID (iid) DECLSPEC_NOVTABLE iface
#define DECLARE_INTERFACE_IID_(iface, baseiface, iid) interface DECLSPEC_UUID (iid) DECLSPEC_NOVTABLE iface : public baseiface

#define IFACEMETHOD(method) STDMETHOD (method)
#define IFACEMETHOD_(type, method) /*override*/ STDMETHOD_(type, method)
#define IFACEMETHODV(method) STDMETHODV (method)
#define IFACEMETHODV_(type, method) STDMETHODV_(type, method)

#ifndef BEGIN_INTERFACE
#define BEGIN_INTERFACE
#define END_INTERFACE
#endif

interface IUnknown;

extern "C++" {
  template<typename T> void **IID_PPV_ARGS_Helper (T **pp) {
    static_cast<IUnknown *> (*pp);
    return reinterpret_cast<void **> (pp);
  }
}

#define IID_PPV_ARGS(ppType) __uuidof (**(ppType)), IID_PPV_ARGS_Helper (ppType)
#else
#undef interface
#define interface struct

#define STDMETHOD(method) HRESULT (STDMETHODCALLTYPE *method)
#define STDMETHOD_(type, method) type (STDMETHODCALLTYPE *method)
#define STDMETHODV(method) HRESULT (STDMETHODVCALLTYPE *method)
#define STDMETHODV_(type, method) type (STDMETHODVCALLTYPE *method)

#define IFACEMETHOD(method) STDMETHOD (method)
#define IFACEMETHOD_(type, method) /*override*/ STDMETHOD_(type, method)
#define IFACEMETHODV(method) STDMETHODV (method)
#define IFACEMETHODV_(type, method) /*override*/ STDMETHODV_(type, method)

#ifndef BEGIN_INTERFACE
#define BEGIN_INTERFACE
#define END_INTERFACE
#endif

#define PURE
#define THIS_ INTERFACE *This,
#define THIS INTERFACE *This
#ifdef CONST_VTABLE
#undef CONST_VTBL
#define CONST_VTBL const
#define DECLARE_INTERFACE(iface) typedef interface iface { const struct iface##Vtbl *lpVtbl; } iface; typedef const struct iface##Vtbl iface##Vtbl; const struct iface##Vtbl
#else
#undef CONST_VTBL
#define CONST_VTBL
#define DECLARE_INTERFACE(iface) typedef interface iface { struct iface##Vtbl *lpVtbl; } iface; typedef struct iface##Vtbl iface##Vtbl; struct iface##Vtbl
#endif
#define DECLARE_INTERFACE_(iface, baseiface) DECLARE_INTERFACE (iface)
#define DECLARE_INTERFACE_IID(iface, iid) DECLARE_INTERFACE (iface)
#define DECLARE_INTERFACE_IID_(iface, baseiface, iid) DECLARE_INTERFACE_ (iface, baseiface)
#endif

#ifndef FARSTRUCT
#define FARSTRUCT
#endif

#ifndef HUGEP
#define HUGEP
#endif

#include <stdlib.h>

#define LISet32(li, v) ((li).HighPart = ((LONG) (v)) < 0 ? -1 : 0,(li).LowPart = (v))
#define ULISet32(li, v) ((li).HighPart = 0,(li).LowPart = (v))

#define CLSCTX_INPROC (CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER)
#define CLSCTX_ALL (CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER)
#define CLSCTX_SERVER (CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER)

typedef enum tagREGCLS {
  REGCLS_SINGLEUSE = 0,
  REGCLS_MULTIPLEUSE = 1,
  REGCLS_MULTI_SEPARATE = 2,
  REGCLS_SUSPENDED = 4,
  REGCLS_SURROGATE = 8
} REGCLS;

typedef interface IRpcStubBuffer IRpcStubBuffer;
typedef interface IRpcChannelBuffer IRpcChannelBuffer;

typedef enum tagCOINITBASE {
  COINITBASE_MULTITHREADED = 0x0,
} COINITBASE;

#include <wtypesbase.h>
#include <unknwnbase.h>
#include <objidlbase.h>
#include <guiddef.h>

#ifndef INITGUID
#include <cguid.h>
#endif
#endif

#if WINAPI_FAMILY_PARTITION (WINAPI_PARTITION_APP)
#if NTDDI_VERSION >= 0x06020000
typedef struct tagServerInformation {
  DWORD dwServerPid;
  DWORD dwServerTid;
  UINT64 ui64ServerAddress;
} ServerInformation,*PServerInformation;

DECLARE_HANDLE (CO_MTA_USAGE_COOKIE);
#endif
WINOLEAPI CreateStreamOnHGlobal (HGLOBAL hGlobal, WINBOOL fDeleteOnRelease, LPSTREAM *ppstm);
WINOLEAPI GetHGlobalFromStream (LPSTREAM pstm, HGLOBAL *phglobal);
WINOLEAPI_(void) CoUninitialize (void);
WINOLEAPI CoInitializeEx (LPVOID pvReserved, DWORD dwCoInit);
WINOLEAPI CoGetCurrentLogicalThreadId (GUID *pguid);
WINOLEAPI CoGetContextToken (ULONG_PTR *pToken);
#if NTDDI_VERSION >= 0x06010000
WINOLEAPI CoGetApartmentType (APTTYPE *pAptType, APTTYPEQUALIFIER *pAptQualifier);
#endif
WINOLEAPI CoGetObjectContext (REFIID riid, LPVOID *ppv);
WINOLEAPI CoRegisterClassObject (REFCLSID rclsid, LPUNKNOWN pUnk, DWORD dwClsContext, DWORD flags, LPDWORD lpdwRegister);
WINOLEAPI C