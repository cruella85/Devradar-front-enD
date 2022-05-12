
/*** Autogenerated by WIDL 7.0 from include/tsvirtualchannels.idl - Do not edit ***/

#ifdef _WIN32
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif
#include <rpc.h>
#include <rpcndr.h>
#endif

#ifndef COM_NO_WINDOWS_H
#include <windows.h>
#include <ole2.h>
#endif

#ifndef __tsvirtualchannels_h__
#define __tsvirtualchannels_h__

/* Forward declarations */

#ifndef __IWTSPlugin_FWD_DEFINED__
#define __IWTSPlugin_FWD_DEFINED__
typedef interface IWTSPlugin IWTSPlugin;
#ifdef __cplusplus
interface IWTSPlugin;
#endif /* __cplusplus */
#endif

#ifndef __IWTSListener_FWD_DEFINED__
#define __IWTSListener_FWD_DEFINED__
typedef interface IWTSListener IWTSListener;
#ifdef __cplusplus
interface IWTSListener;
#endif /* __cplusplus */
#endif

#ifndef __IWTSListenerCallback_FWD_DEFINED__
#define __IWTSListenerCallback_FWD_DEFINED__
typedef interface IWTSListenerCallback IWTSListenerCallback;
#ifdef __cplusplus
interface IWTSListenerCallback;
#endif /* __cplusplus */
#endif

#ifndef __IWTSVirtualChannelCallback_FWD_DEFINED__
#define __IWTSVirtualChannelCallback_FWD_DEFINED__
typedef interface IWTSVirtualChannelCallback IWTSVirtualChannelCallback;
#ifdef __cplusplus
interface IWTSVirtualChannelCallback;
#endif /* __cplusplus */
#endif

#ifndef __IWTSVirtualChannelManager_FWD_DEFINED__
#define __IWTSVirtualChannelManager_FWD_DEFINED__
typedef interface IWTSVirtualChannelManager IWTSVirtualChannelManager;
#ifdef __cplusplus
interface IWTSVirtualChannelManager;
#endif /* __cplusplus */
#endif

#ifndef __IWTSVirtualChannel_FWD_DEFINED__
#define __IWTSVirtualChannel_FWD_DEFINED__
typedef interface IWTSVirtualChannel IWTSVirtualChannel;
#ifdef __cplusplus
interface IWTSVirtualChannel;
#endif /* __cplusplus */
#endif

#ifndef __IWTSPluginServiceProvider_FWD_DEFINED__
#define __IWTSPluginServiceProvider_FWD_DEFINED__
typedef interface IWTSPluginServiceProvider IWTSPluginServiceProvider;
#ifdef __cplusplus
interface IWTSPluginServiceProvider;
#endif /* __cplusplus */
#endif

#ifndef __IWTSBitmapRenderer_FWD_DEFINED__
#define __IWTSBitmapRenderer_FWD_DEFINED__
typedef interface IWTSBitmapRenderer IWTSBitmapRenderer;
#ifdef __cplusplus
interface IWTSBitmapRenderer;
#endif /* __cplusplus */
#endif

#ifndef __IWTSBitmapRendererCallback_FWD_DEFINED__
#define __IWTSBitmapRendererCallback_FWD_DEFINED__
typedef interface IWTSBitmapRendererCallback IWTSBitmapRendererCallback;
#ifdef __cplusplus
interface IWTSBitmapRendererCallback;
#endif /* __cplusplus */
#endif

#ifndef __IWTSBitmapRenderService_FWD_DEFINED__
#define __IWTSBitmapRenderService_FWD_DEFINED__
typedef interface IWTSBitmapRenderService IWTSBitmapRenderService;
#ifdef __cplusplus
interface IWTSBitmapRenderService;
#endif /* __cplusplus */
#endif

/* Headers for imported files */

#include <unknwn.h>
#include <oaidl.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <winapifamily.h>
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#ifndef __IWTSPlugin_FWD_DEFINED__
#define __IWTSPlugin_FWD_DEFINED__
typedef interface IWTSPlugin IWTSPlugin;
#ifdef __cplusplus
interface IWTSPlugin;
#endif /* __cplusplus */
#endif

#ifndef __IWTSListener_FWD_DEFINED__
#define __IWTSListener_FWD_DEFINED__
typedef interface IWTSListener IWTSListener;
#ifdef __cplusplus
interface IWTSListener;
#endif /* __cplusplus */
#endif

#ifndef __IWTSListenerCallback_FWD_DEFINED__
#define __IWTSListenerCallback_FWD_DEFINED__
typedef interface IWTSListenerCallback IWTSListenerCallback;
#ifdef __cplusplus
interface IWTSListenerCallback;
#endif /* __cplusplus */
#endif

#ifndef __IWTSVirtualChannelCallback_FWD_DEFINED__
#define __IWTSVirtualChannelCallback_FWD_DEFINED__
typedef interface IWTSVirtualChannelCallback IWTSVirtualChannelCallback;
#ifdef __cplusplus
interface IWTSVirtualChannelCallback;
#endif /* __cplusplus */
#endif

#ifndef __IWTSVirtualChannelManager_FWD_DEFINED__
#define __IWTSVirtualChannelManager_FWD_DEFINED__
typedef interface IWTSVirtualChannelManager IWTSVirtualChannelManager;
#ifdef __cplusplus
interface IWTSVirtualChannelManager;
#endif /* __cplusplus */
#endif

#ifndef __IWTSVirtualChannel_FWD_DEFINED__
#define __IWTSVirtualChannel_FWD_DEFINED__
typedef interface IWTSVirtualChannel IWTSVirtualChannel;
#ifdef __cplusplus
interface IWTSVirtualChannel;
#endif /* __cplusplus */
#endif

#define WTS_PROPERTY_DEFAULT_CONFIG L"DefaultConfig"
#define E_MAPPEDRENDERER_SHUTDOWN HRESULT_FROM_WIN32(ERROR_INVALID_STATE)
#define E_DUPLICATE_WINDOW_HINT HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)
/*****************************************************************************
 * IWTSPlugin interface
 */
#ifndef __IWTSPlugin_INTERFACE_DEFINED__
#define __IWTSPlugin_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSPlugin, 0xa1230201, 0x1439, 0x4e62, 0xa4,0x14, 0x19,0x0d,0x0a,0xc3,0xd4,0x0e);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a1230201-1439-4e62-a414-190d0ac3d40e")
IWTSPlugin : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Initialize(
        IWTSVirtualChannelManager *pChannelMgr) = 0;

    virtual HRESULT STDMETHODCALLTYPE Connected(
        ) = 0;

    virtual HRESULT STDMETHODCALLTYPE Disconnected(
        DWORD dwDisconnectCode) = 0;

    virtual HRESULT STDMETHODCALLTYPE Terminated(
        ) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSPlugin, 0xa1230201, 0x1439, 0x4e62, 0xa4,0x14, 0x19,0x0d,0x0a,0xc3,0xd4,0x0e)
#endif
#else
typedef struct IWTSPluginVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSPlugin *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSPlugin *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSPlugin *This);

    /*** IWTSPlugin methods ***/
    HRESULT (STDMETHODCALLTYPE *Initialize)(
        IWTSPlugin *This,
        IWTSVirtualChannelManager *pChannelMgr);

    HRESULT (STDMETHODCALLTYPE *Connected)(
        IWTSPlugin *This);

    HRESULT (STDMETHODCALLTYPE *Disconnected)(
        IWTSPlugin *This,
        DWORD dwDisconnectCode);

    HRESULT (STDMETHODCALLTYPE *Terminated)(
        IWTSPlugin *This);

    END_INTERFACE
} IWTSPluginVtbl;

interface IWTSPlugin {
    CONST_VTBL IWTSPluginVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSPlugin_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSPlugin_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSPlugin_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSPlugin methods ***/
#define IWTSPlugin_Initialize(This,pChannelMgr) (This)->lpVtbl->Initialize(This,pChannelMgr)
#define IWTSPlugin_Connected(This) (This)->lpVtbl->Connected(This)
#define IWTSPlugin_Disconnected(This,dwDisconnectCode) (This)->lpVtbl->Disconnected(This,dwDisconnectCode)
#define IWTSPlugin_Terminated(This) (This)->lpVtbl->Terminated(This)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSPlugin_QueryInterface(IWTSPlugin* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSPlugin_AddRef(IWTSPlugin* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSPlugin_Release(IWTSPlugin* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSPlugin methods ***/
static FORCEINLINE HRESULT IWTSPlugin_Initialize(IWTSPlugin* This,IWTSVirtualChannelManager *pChannelMgr) {
    return This->lpVtbl->Initialize(This,pChannelMgr);
}
static FORCEINLINE HRESULT IWTSPlugin_Connected(IWTSPlugin* This) {
    return This->lpVtbl->Connected(This);
}
static FORCEINLINE HRESULT IWTSPlugin_Disconnected(IWTSPlugin* This,DWORD dwDisconnectCode) {
    return This->lpVtbl->Disconnected(This,dwDisconnectCode);
}
static FORCEINLINE HRESULT IWTSPlugin_Terminated(IWTSPlugin* This) {
    return This->lpVtbl->Terminated(This);
}
#endif
#endif

#endif


#endif  /* __IWTSPlugin_INTERFACE_DEFINED__ */

/*****************************************************************************
 * IWTSListener interface
 */
#ifndef __IWTSListener_INTERFACE_DEFINED__
#define __IWTSListener_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSListener, 0xa1230206, 0x9a39, 0x4d58, 0x86,0x74, 0xcd,0xb4,0xdf,0xf4,0xe7,0x3b);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a1230206-9a39-4d58-8674-cdb4dff4e73b")
IWTSListener : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetConfiguration(
        IPropertyBag **ppPropertyBag) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSListener, 0xa1230206, 0x9a39, 0x4d58, 0x86,0x74, 0xcd,0xb4,0xdf,0xf4,0xe7,0x3b)
#endif
#else
typedef struct IWTSListenerVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSListener *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSListener *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSListener *This);

    /*** IWTSListener methods ***/
    HRESULT (STDMETHODCALLTYPE *GetConfiguration)(
        IWTSListener *This,
        IPropertyBag **ppPropertyBag);

    END_INTERFACE
} IWTSListenerVtbl;

interface IWTSListener {
    CONST_VTBL IWTSListenerVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSListener_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSListener_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSListener_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSListener methods ***/
#define IWTSListener_GetConfiguration(This,ppPropertyBag) (This)->lpVtbl->GetConfiguration(This,ppPropertyBag)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSListener_QueryInterface(IWTSListener* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSListener_AddRef(IWTSListener* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSListener_Release(IWTSListener* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSListener methods ***/
static FORCEINLINE HRESULT IWTSListener_GetConfiguration(IWTSListener* This,IPropertyBag **ppPropertyBag) {
    return This->lpVtbl->GetConfiguration(This,ppPropertyBag);
}
#endif
#endif

#endif


#endif  /* __IWTSListener_INTERFACE_DEFINED__ */

/*****************************************************************************
 * IWTSListenerCallback interface
 */
#ifndef __IWTSListenerCallback_INTERFACE_DEFINED__
#define __IWTSListenerCallback_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSListenerCallback, 0xa1230203, 0xd6a7, 0x11d8, 0xb9,0xfd, 0x00,0x0b,0xdb,0xd1,0xf1,0x98);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a1230203-d6a7-11d8-b9fd-000bdbd1f198")
IWTSListenerCallback : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE OnNewChannelConnection(
        IWTSVirtualChannel *pChannel,
        BSTR data,
        WINBOOL *pbAccept,
        IWTSVirtualChannelCallback **ppCallback) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSListenerCallback, 0xa1230203, 0xd6a7, 0x11d8, 0xb9,0xfd, 0x00,0x0b,0xdb,0xd1,0xf1,0x98)
#endif
#else
typedef struct IWTSListenerCallbackVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSListenerCallback *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSListenerCallback *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSListenerCallback *This);

    /*** IWTSListenerCallback methods ***/
    HRESULT (STDMETHODCALLTYPE *OnNewChannelConnection)(
        IWTSListenerCallback *This,
        IWTSVirtualChannel *pChannel,
        BSTR data,
        WINBOOL *pbAccept,
        IWTSVirtualChannelCallback **ppCallback);

    END_INTERFACE
} IWTSListenerCallbackVtbl;

interface IWTSListenerCallback {
    CONST_VTBL IWTSListenerCallbackVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSListenerCallback_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSListenerCallback_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSListenerCallback_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSListenerCallback methods ***/
#define IWTSListenerCallback_OnNewChannelConnection(This,pChannel,data,pbAccept,ppCallback) (This)->lpVtbl->OnNewChannelConnection(This,pChannel,data,pbAccept,ppCallback)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSListenerCallback_QueryInterface(IWTSListenerCallback* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSListenerCallback_AddRef(IWTSListenerCallback* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSListenerCallback_Release(IWTSListenerCallback* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSListenerCallback methods ***/
static FORCEINLINE HRESULT IWTSListenerCallback_OnNewChannelConnection(IWTSListenerCallback* This,IWTSVirtualChannel *pChannel,BSTR data,WINBOOL *pbAccept,IWTSVirtualChannelCallback **ppCallback) {
    return This->lpVtbl->OnNewChannelConnection(This,pChannel,data,pbAccept,ppCallback);
}
#endif
#endif

#endif


#endif  /* __IWTSListenerCallback_INTERFACE_DEFINED__ */

/*****************************************************************************
 * IWTSVirtualChannelCallback interface
 */
#ifndef __IWTSVirtualChannelCallback_INTERFACE_DEFINED__
#define __IWTSVirtualChannelCallback_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSVirtualChannelCallback, 0xa1230204, 0xd6a7, 0x11d8, 0xb9,0xfd, 0x00,0x0b,0xdb,0xd1,0xf1,0x98);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a1230204-d6a7-11d8-b9fd-000bdbd1f198")
IWTSVirtualChannelCallback : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE OnDataReceived(
        ULONG cbSize,
        BYTE *pBuffer) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnClose(
        ) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSVirtualChannelCallback, 0xa1230204, 0xd6a7, 0x11d8, 0xb9,0xfd, 0x00,0x0b,0xdb,0xd1,0xf1,0x98)
#endif
#else
typedef struct IWTSVirtualChannelCallbackVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSVirtualChannelCallback *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSVirtualChannelCallback *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSVirtualChannelCallback *This);

    /*** IWTSVirtualChannelCallback methods ***/
    HRESULT (STDMETHODCALLTYPE *OnDataReceived)(
        IWTSVirtualChannelCallback *This,
        ULONG cbSize,
        BYTE *pBuffer);

    HRESULT (STDMETHODCALLTYPE *OnClose)(
        IWTSVirtualChannelCallback *This);

    END_INTERFACE
} IWTSVirtualChannelCallbackVtbl;

interface IWTSVirtualChannelCallback {
    CONST_VTBL IWTSVirtualChannelCallbackVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSVirtualChannelCallback_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSVirtualChannelCallback_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSVirtualChannelCallback_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSVirtualChannelCallback methods ***/
#define IWTSVirtualChannelCallback_OnDataReceived(This,cbSize,pBuffer) (This)->lpVtbl->OnDataReceived(This,cbSize,pBuffer)
#define IWTSVirtualChannelCallback_OnClose(This) (This)->lpVtbl->OnClose(This)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSVirtualChannelCallback_QueryInterface(IWTSVirtualChannelCallback* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSVirtualChannelCallback_AddRef(IWTSVirtualChannelCallback* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSVirtualChannelCallback_Release(IWTSVirtualChannelCallback* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSVirtualChannelCallback methods ***/
static FORCEINLINE HRESULT IWTSVirtualChannelCallback_OnDataReceived(IWTSVirtualChannelCallback* This,ULONG cbSize,BYTE *pBuffer) {
    return This->lpVtbl->OnDataReceived(This,cbSize,pBuffer);
}
static FORCEINLINE HRESULT IWTSVirtualChannelCallback_OnClose(IWTSVirtualChannelCallback* This) {
    return This->lpVtbl->OnClose(This);
}
#endif
#endif

#endif


#endif  /* __IWTSVirtualChannelCallback_INTERFACE_DEFINED__ */

#define TS_VC_LISTENER_STATIC_CHANNEL 0x00000001
/*****************************************************************************
 * IWTSVirtualChannelManager interface
 */
#ifndef __IWTSVirtualChannelManager_INTERFACE_DEFINED__
#define __IWTSVirtualChannelManager_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSVirtualChannelManager, 0xa1230205, 0xd6a7, 0x11d8, 0xb9,0xfd, 0x00,0x0b,0xdb,0xd1,0xf1,0x98);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a1230205-d6a7-11d8-b9fd-000bdbd1f198")
IWTSVirtualChannelManager : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE CreateListener(
        const char *pszChannelName,
        ULONG uFlags,
        IWTSListenerCallback *pListenerCallback,
        IWTSListener **ppListener) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSVirtualChannelManager, 0xa1230205, 0xd6a7, 0x11d8, 0xb9,0xfd, 0x00,0x0b,0xdb,0xd1,0xf1,0x98)
#endif
#else
typedef struct IWTSVirtualChannelManagerVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSVirtualChannelManager *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSVirtualChannelManager *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSVirtualChannelManager *This);

    /*** IWTSVirtualChannelManager methods ***/
    HRESULT (STDMETHODCALLTYPE *CreateListener)(
        IWTSVirtualChannelManager *This,
        const char *pszChannelName,
        ULONG uFlags,
        IWTSListenerCallback *pListenerCallback,
        IWTSListener **ppListener);

    END_INTERFACE
} IWTSVirtualChannelManagerVtbl;

interface IWTSVirtualChannelManager {
    CONST_VTBL IWTSVirtualChannelManagerVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSVirtualChannelManager_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSVirtualChannelManager_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSVirtualChannelManager_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSVirtualChannelManager methods ***/
#define IWTSVirtualChannelManager_CreateListener(This,pszChannelName,uFlags,pListenerCallback,ppListener) (This)->lpVtbl->CreateListener(This,pszChannelName,uFlags,pListenerCallback,ppListener)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSVirtualChannelManager_QueryInterface(IWTSVirtualChannelManager* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSVirtualChannelManager_AddRef(IWTSVirtualChannelManager* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSVirtualChannelManager_Release(IWTSVirtualChannelManager* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSVirtualChannelManager methods ***/
static FORCEINLINE HRESULT IWTSVirtualChannelManager_CreateListener(IWTSVirtualChannelManager* This,const char *pszChannelName,ULONG uFlags,IWTSListenerCallback *pListenerCallback,IWTSListener **ppListener) {
    return This->lpVtbl->CreateListener(This,pszChannelName,uFlags,pListenerCallback,ppListener);
}
#endif
#endif

#endif


#endif  /* __IWTSVirtualChannelManager_INTERFACE_DEFINED__ */

/*****************************************************************************
 * IWTSVirtualChannel interface
 */
#ifndef __IWTSVirtualChannel_INTERFACE_DEFINED__
#define __IWTSVirtualChannel_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSVirtualChannel, 0xa1230207, 0xd6a7, 0x11d8, 0xb9,0xfd, 0x00,0x0b,0xdb,0xd1,0xf1,0x98);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a1230207-d6a7-11d8-b9fd-000bdbd1f198")
IWTSVirtualChannel : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Write(
        ULONG cbSize,
        BYTE *pBuffer,
        IUnknown *pReserved) = 0;

    virtual HRESULT STDMETHODCALLTYPE Close(
        ) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSVirtualChannel, 0xa1230207, 0xd6a7, 0x11d8, 0xb9,0xfd, 0x00,0x0b,0xdb,0xd1,0xf1,0x98)
#endif
#else
typedef struct IWTSVirtualChannelVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSVirtualChannel *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSVirtualChannel *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSVirtualChannel *This);

    /*** IWTSVirtualChannel methods ***/
    HRESULT (STDMETHODCALLTYPE *Write)(
        IWTSVirtualChannel *This,
        ULONG cbSize,
        BYTE *pBuffer,
        IUnknown *pReserved);

    HRESULT (STDMETHODCALLTYPE *Close)(
        IWTSVirtualChannel *This);

    END_INTERFACE
} IWTSVirtualChannelVtbl;

interface IWTSVirtualChannel {
    CONST_VTBL IWTSVirtualChannelVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSVirtualChannel_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSVirtualChannel_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSVirtualChannel_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSVirtualChannel methods ***/
#define IWTSVirtualChannel_Write(This,cbSize,pBuffer,pReserved) (This)->lpVtbl->Write(This,cbSize,pBuffer,pReserved)
#define IWTSVirtualChannel_Close(This) (This)->lpVtbl->Close(This)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSVirtualChannel_QueryInterface(IWTSVirtualChannel* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSVirtualChannel_AddRef(IWTSVirtualChannel* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSVirtualChannel_Release(IWTSVirtualChannel* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSVirtualChannel methods ***/
static FORCEINLINE HRESULT IWTSVirtualChannel_Write(IWTSVirtualChannel* This,ULONG cbSize,BYTE *pBuffer,IUnknown *pReserved) {
    return This->lpVtbl->Write(This,cbSize,pBuffer,pReserved);
}
static FORCEINLINE HRESULT IWTSVirtualChannel_Close(IWTSVirtualChannel* This) {
    return This->lpVtbl->Close(This);
}
#endif
#endif

#endif


#endif  /* __IWTSVirtualChannel_INTERFACE_DEFINED__ */

EXTERN_GUID( RDCLIENT_BITMAP_RENDER_SERVICE, 0xe4cc08cb, 0x942e, 0x4b19, 0x85, 0x4, 0xbd, 0x5a, 0x89, 0xa7, 0x47, 0xf5);
/*****************************************************************************
 * IWTSPluginServiceProvider interface
 */
#ifndef __IWTSPluginServiceProvider_INTERFACE_DEFINED__
#define __IWTSPluginServiceProvider_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSPluginServiceProvider, 0xd3e07363, 0x087c, 0x476c, 0x86,0xa7, 0xdb,0xb1,0x5f,0x46,0xdd,0xb4);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d3e07363-087c-476c-86a7-dbb15f46ddb4")
IWTSPluginServiceProvider : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetService(
        GUID ServiceId,
        IUnknown **ppunkObject) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSPluginServiceProvider, 0xd3e07363, 0x087c, 0x476c, 0x86,0xa7, 0xdb,0xb1,0x5f,0x46,0xdd,0xb4)
#endif
#else
typedef struct IWTSPluginServiceProviderVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSPluginServiceProvider *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSPluginServiceProvider *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSPluginServiceProvider *This);

    /*** IWTSPluginServiceProvider methods ***/
    HRESULT (STDMETHODCALLTYPE *GetService)(
        IWTSPluginServiceProvider *This,
        GUID ServiceId,
        IUnknown **ppunkObject);

    END_INTERFACE
} IWTSPluginServiceProviderVtbl;

interface IWTSPluginServiceProvider {
    CONST_VTBL IWTSPluginServiceProviderVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSPluginServiceProvider_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSPluginServiceProvider_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSPluginServiceProvider_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSPluginServiceProvider methods ***/
#define IWTSPluginServiceProvider_GetService(This,ServiceId,ppunkObject) (This)->lpVtbl->GetService(This,ServiceId,ppunkObject)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSPluginServiceProvider_QueryInterface(IWTSPluginServiceProvider* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSPluginServiceProvider_AddRef(IWTSPluginServiceProvider* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSPluginServiceProvider_Release(IWTSPluginServiceProvider* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSPluginServiceProvider methods ***/
static FORCEINLINE HRESULT IWTSPluginServiceProvider_GetService(IWTSPluginServiceProvider* This,GUID ServiceId,IUnknown **ppunkObject) {
    return This->lpVtbl->GetService(This,ServiceId,ppunkObject);
}
#endif
#endif

#endif


#endif  /* __IWTSPluginServiceProvider_INTERFACE_DEFINED__ */

typedef struct __BITMAP_RENDERER_STATISTICS {
    DWORD dwFramesDelivered;
    DWORD dwFramesDropped;
} BITMAP_RENDERER_STATISTICS;
typedef struct __BITMAP_RENDERER_STATISTICS *PBITMAP_RENDERER_STATISTICS;
/*****************************************************************************
 * IWTSBitmapRenderer interface
 */
#ifndef __IWTSBitmapRenderer_INTERFACE_DEFINED__
#define __IWTSBitmapRenderer_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSBitmapRenderer, 0x5b7acc97, 0xf3c9, 0x46f7, 0x8c,0x5b, 0xfa,0x68,0x5d,0x34,0x41,0xb1);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5b7acc97-f3c9-46f7-8c5b-fa685d3441b1")
IWTSBitmapRenderer : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Render(
        GUID imageFormat,
        DWORD dwWidth,
        DWORD dwHeight,
        LONG cbStride,
        DWORD cbImageBuffer,
        BYTE *pImageBuffer) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetRendererStatistics(
        BITMAP_RENDERER_STATISTICS *pStatistics) = 0;

    virtual HRESULT STDMETHODCALLTYPE RemoveMapping(
        ) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSBitmapRenderer, 0x5b7acc97, 0xf3c9, 0x46f7, 0x8c,0x5b, 0xfa,0x68,0x5d,0x34,0x41,0xb1)
#endif
#else
typedef struct IWTSBitmapRendererVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSBitmapRenderer *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSBitmapRenderer *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSBitmapRenderer *This);

    /*** IWTSBitmapRenderer methods ***/
    HRESULT (STDMETHODCALLTYPE *Render)(
        IWTSBitmapRenderer *This,
        GUID imageFormat,
        DWORD dwWidth,
        DWORD dwHeight,
        LONG cbStride,
        DWORD cbImageBuffer,
        BYTE *pImageBuffer);

    HRESULT (STDMETHODCALLTYPE *GetRendererStatistics)(
        IWTSBitmapRenderer *This,
        BITMAP_RENDERER_STATISTICS *pStatistics);

    HRESULT (STDMETHODCALLTYPE *RemoveMapping)(
        IWTSBitmapRenderer *This);

    END_INTERFACE
} IWTSBitmapRendererVtbl;

interface IWTSBitmapRenderer {
    CONST_VTBL IWTSBitmapRendererVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSBitmapRenderer_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSBitmapRenderer_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSBitmapRenderer_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSBitmapRenderer methods ***/
#define IWTSBitmapRenderer_Render(This,imageFormat,dwWidth,dwHeight,cbStride,cbImageBuffer,pImageBuffer) (This)->lpVtbl->Render(This,imageFormat,dwWidth,dwHeight,cbStride,cbImageBuffer,pImageBuffer)
#define IWTSBitmapRenderer_GetRendererStatistics(This,pStatistics) (This)->lpVtbl->GetRendererStatistics(This,pStatistics)
#define IWTSBitmapRenderer_RemoveMapping(This) (This)->lpVtbl->RemoveMapping(This)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSBitmapRenderer_QueryInterface(IWTSBitmapRenderer* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSBitmapRenderer_AddRef(IWTSBitmapRenderer* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSBitmapRenderer_Release(IWTSBitmapRenderer* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSBitmapRenderer methods ***/
static FORCEINLINE HRESULT IWTSBitmapRenderer_Render(IWTSBitmapRenderer* This,GUID imageFormat,DWORD dwWidth,DWORD dwHeight,LONG cbStride,DWORD cbImageBuffer,BYTE *pImageBuffer) {
    return This->lpVtbl->Render(This,imageFormat,dwWidth,dwHeight,cbStride,cbImageBuffer,pImageBuffer);
}
static FORCEINLINE HRESULT IWTSBitmapRenderer_GetRendererStatistics(IWTSBitmapRenderer* This,BITMAP_RENDERER_STATISTICS *pStatistics) {
    return This->lpVtbl->GetRendererStatistics(This,pStatistics);
}
static FORCEINLINE HRESULT IWTSBitmapRenderer_RemoveMapping(IWTSBitmapRenderer* This) {
    return This->lpVtbl->RemoveMapping(This);
}
#endif
#endif

#endif


#endif  /* __IWTSBitmapRenderer_INTERFACE_DEFINED__ */

/*****************************************************************************
 * IWTSBitmapRendererCallback interface
 */
#ifndef __IWTSBitmapRendererCallback_INTERFACE_DEFINED__
#define __IWTSBitmapRendererCallback_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSBitmapRendererCallback, 0xd782928e, 0xfe4e, 0x4e77, 0xae,0x90, 0x9c,0xd0,0xb3,0xe3,0xb3,0x53);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d782928e-fe4e-4e77-ae90-9cd0b3e3b353")
IWTSBitmapRendererCallback : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE OnTargetSizeChanged(
        RECT rcNewSize) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSBitmapRendererCallback, 0xd782928e, 0xfe4e, 0x4e77, 0xae,0x90, 0x9c,0xd0,0xb3,0xe3,0xb3,0x53)
#endif
#else
typedef struct IWTSBitmapRendererCallbackVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSBitmapRendererCallback *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSBitmapRendererCallback *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSBitmapRendererCallback *This);

    /*** IWTSBitmapRendererCallback methods ***/
    HRESULT (STDMETHODCALLTYPE *OnTargetSizeChanged)(
        IWTSBitmapRendererCallback *This,
        RECT rcNewSize);

    END_INTERFACE
} IWTSBitmapRendererCallbackVtbl;

interface IWTSBitmapRendererCallback {
    CONST_VTBL IWTSBitmapRendererCallbackVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSBitmapRendererCallback_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSBitmapRendererCallback_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSBitmapRendererCallback_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSBitmapRendererCallback methods ***/
#define IWTSBitmapRendererCallback_OnTargetSizeChanged(This,rcNewSize) (This)->lpVtbl->OnTargetSizeChanged(This,rcNewSize)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSBitmapRendererCallback_QueryInterface(IWTSBitmapRendererCallback* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSBitmapRendererCallback_AddRef(IWTSBitmapRendererCallback* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSBitmapRendererCallback_Release(IWTSBitmapRendererCallback* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSBitmapRendererCallback methods ***/
static FORCEINLINE HRESULT IWTSBitmapRendererCallback_OnTargetSizeChanged(IWTSBitmapRendererCallback* This,RECT rcNewSize) {
    return This->lpVtbl->OnTargetSizeChanged(This,rcNewSize);
}
#endif
#endif

#endif


#endif  /* __IWTSBitmapRendererCallback_INTERFACE_DEFINED__ */

/*****************************************************************************
 * IWTSBitmapRenderService interface
 */
#ifndef __IWTSBitmapRenderService_INTERFACE_DEFINED__
#define __IWTSBitmapRenderService_INTERFACE_DEFINED__

DEFINE_GUID(IID_IWTSBitmapRenderService, 0xea326091, 0x05fe, 0x40c1, 0xb4,0x9c, 0x3d,0x2e,0xf4,0x62,0x6a,0x0e);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("ea326091-05fe-40c1-b49c-3d2ef4626a0e")
IWTSBitmapRenderService : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetMappedRenderer(
        UINT64 mappingId,
        IWTSBitmapRendererCallback *pMappedRendererCallback,
        IWTSBitmapRenderer **ppMappedRenderer) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IWTSBitmapRenderService, 0xea326091, 0x05fe, 0x40c1, 0xb4,0x9c, 0x3d,0x2e,0xf4,0x62,0x6a,0x0e)
#endif
#else
typedef struct IWTSBitmapRenderServiceVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IWTSBitmapRenderService *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IWTSBitmapRenderService *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IWTSBitmapRenderService *This);

    /*** IWTSBitmapRenderService methods ***/
    HRESULT (STDMETHODCALLTYPE *GetMappedRenderer)(
        IWTSBitmapRenderService *This,
        UINT64 mappingId,
        IWTSBitmapRendererCallback *pMappedRendererCallback,
        IWTSBitmapRenderer **ppMappedRenderer);

    END_INTERFACE
} IWTSBitmapRenderServiceVtbl;

interface IWTSBitmapRenderService {
    CONST_VTBL IWTSBitmapRenderServiceVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IWTSBitmapRenderService_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWTSBitmapRenderService_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IWTSBitmapRenderService_Release(This) (This)->lpVtbl->Release(This)
/*** IWTSBitmapRenderService methods ***/
#define IWTSBitmapRenderService_GetMappedRenderer(This,mappingId,pMappedRendererCallback,ppMappedRenderer) (This)->lpVtbl->GetMappedRenderer(This,mappingId,pMappedRendererCallback,ppMappedRenderer)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IWTSBitmapRenderService_QueryInterface(IWTSBitmapRenderService* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IWTSBitmapRenderService_AddRef(IWTSBitmapRenderService* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IWTSBitmapRenderService_Release(IWTSBitmapRenderService* This) {
    return This->lpVtbl->Release(This);
}
/*** IWTSBitmapRenderService methods ***/
static FORCEINLINE HRESULT IWTSBitmapRenderService_GetMappedRenderer(IWTSBitmapRenderService* This,UINT64 mappingId,IWTSBitmapRendererCallback *pMappedRendererCallback,IWTSBitmapRenderer **ppMappedRenderer) {
    return This->lpVtbl->GetMappedRenderer(This,mappingId,pMappedRendererCallback,ppMappedRenderer);
}
#endif
#endif

#endif


#endif  /* __IWTSBitmapRenderService_INTERFACE_DEFINED__ */

#endif /* WINAPI_PARTITION_DESKTOP */
/* Begin additional prototypes for all interfaces */

ULONG           __RPC_USER BSTR_UserSize     (ULONG *, ULONG, BSTR *);
unsigned char * __RPC_USER BSTR_UserMarshal  (ULONG *, unsigned char *, BSTR *);
unsigned char * __RPC_USER BSTR_UserUnmarshal(ULONG *, unsigned char *, BSTR *);
void            __RPC_USER BSTR_UserFree     (ULONG *, BSTR *);

/* End additional prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __tsvirtualchannels_h__ */