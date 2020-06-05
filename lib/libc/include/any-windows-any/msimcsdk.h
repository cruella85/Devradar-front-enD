/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __msimcsdk_h__
#define __msimcsdk_h__

#ifndef __IMSIMHost_FWD_DEFINED__
#define __IMSIMHost_FWD_DEFINED__
typedef struct IMSIMHost IMSIMHost;
#endif

#ifndef __DMSIMHostEvents_FWD_DEFINED__
#define __DMSIMHostEvents_FWD_DEFINED__
typedef struct DMSIMHostEvents DMSIMHostEvents;
#endif

#ifndef __IMSIMWindow_FWD_DEFINED__
#define __IMSIMWindow_FWD_DEFINED__
typedef struct IMSIMWindow IMSIMWindow;
#endif

#ifndef __DMSIMWindowEvents_FWD_DEFINED__
#define __DMSIMWindowEvents_FWD_DEFINED__
typedef struct DMSIMWindowEvents DMSIMWindowEvents;
#endif

#ifndef __IIMService_FWD_DEFINED__
#define __IIMService_FWD_DEFINED__
typedef struct IIMService IIMService;
#endif

#ifndef __DIMServiceEvents_FWD_DEFINED__
#define __DIMServiceEvents_FWD_DEFINED__
typedef struct DIMServiceEvents DIMServiceEvents;
#endif

#ifndef __IIMContact_FWD_DEFINED__
#define __IIMContact_FWD_DEFINED__
typedef struct IIMContact IIMContact;
#endif

#ifndef __IIMContacts_FWD_DEFINED__
#define __IIMContacts_FWD_DEFINED__
typedef struct IIMContacts IIMContacts;
#endif

#ifndef __IIMSession_FWD_DEFINED__
#define __IIMSession_FWD_DEFINED__
typedef struct IIMSession IIMSession;
#endif

#ifndef __IIMSessions_FWD_DEFINED__
#define __IIMSessions_FWD_DEFINED__
typedef struct IIMSessions IIMSessions;
#endif

#ifndef __MSIMHost_FWD_DEFINED__
#define __MSIMHost_FWD_DEFINED__
#ifdef __cplusplus
typedef class MSIMHost MSIMHost;
#else
typedef struct MSIMHost MSIMHost;
#endif
#endif

#ifndef __MSIMService_FWD_DEFINED__
#define __MSIMService_FWD_DEFINED__
#ifdef __cplusplus
typedef class MSIMService MSIMService;
#else
typedef struct MSIMService MSIMService;
#endif
#endif

#ifndef __MSIMWindow_FWD_DEFINED__
#define __MSIMWindow_FWD_DEFINED__
#ifdef __cplusplus
typedef class MSIMWindow MSIMWindow;
#else
typedef struct MSIMWindow MSIMWindow;
#endif
#endif

#ifndef __MSIMHostOption_FWD_DEFINED__
#define __MSIMHostOption_FWD_DEFINED__
#ifdef __cplusplus
typedef class MSIMHostOption MSIMHostOption;
#else
typedef struct MSIMHostOption MSIMHostOption;
#endif
#endif

#ifndef __MSIMHostProfiles_FWD_DEFINED__
#define __MSIMHostProfiles_FWD_DEFINED__
#ifdef __cplusplus
typedef class MSIMHostProfiles MSIMHostProfiles;
#else
typedef struct MSIMHostProfiles MSIMHostProfiles;
#endif
#endif

#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MIDL_user_allocate_free_DEFINED__
#define __MIDL_user_allocate_free_DEFINED__
  void *__RPC_API MIDL_user_allocate(size_t);
  void __RPC_API MIDL_user_free(void *);
#endif

#define MSIM_DISPID_ONLOGONRESULT 0x0E00
#define MSIM_DISPID_ONLOGOFF 0x0E01
#define MSIM_DISPID_ONLISTADDRESULT 0x0E02
#define MSIM_DISPID_ONLISTREMOVERESULT 0x0E03
#define MSIM_DISPID_ONFRIENDLYNAMECHANGERESULT 0x0E04
#define MSIM_DISPID_ONCONTACTSTATECHANGED 0x0E05
#define MSIM_DISPID_ONTEXTRECEIVED 0x0E06
#define MSIM_DISPID_ONLOCALFRIENDLYNAMECHANGERESULT 0x0E07
#define MSIM_DISPID_ONLOCALSTATECHANGERESULT 0x0E08
#define MSIM_DISPID_ONSENDRESULT 0x0E09
#define MSIM_DISPID_ONFINDRESULT 0x0E0A
#define MSIM_DISPID_ONSESSIONSTATECHANGE 0x0E0B
#define MSIM_DISPID_ONNEWSESSIONMEMBER 0x0E0C
#define MSIM_DISPID_ONSESSIONMEMBERLEAVE 0x0E0D
#define MSIM_DISPID_ONNEWSESSIONREQUEST 0x0E0F
#define MSIM_DISPID_ONINVITECONTACT 0x0E10
#define MSIM_DISPID_ONAPPSHUTDOWN 0x0E12
#define MSIM_DISPID_ON_NM_INVITERECEIVED 0x0E13
#define MSIM_DISPID_ON_NM_ACCEPTED 0x0E14
#define MSIM_DISPID_ON_NM_CANCELLED 0x0E15
#define MSIMWND_DISPID_ONMOVE 0x00E0
#define MSIMWND_DISPID_ONCLOSE 0x00E1
#define MSIMWND_DISPID_ONRESIZE 0x00E2
#define MSIMWND_DISPID_ONSHOW 0x00E3
#define MSIMWND_DISPID_ONFOCUS 0x00E4
#define MSIMHOSTEVENTS_DISPID_ONDOUBLECLICK 0xD
#define MSIMHOSTEVENTS_DISPID_ONSHUTDOWN 0xE
#define MSIMHOSTEVENTS_DISPID_ONCLICKUSERNOTIFY 0xF

  typedef enum __MIDL___MIDL_itf_msimcsdk_0000_0001 {
    IM_E_CONNECT = 0x81000300 + 0x1,IM_E_INVALID_SERVER_NAME = 0x81000300 + 0x2,IM_E_INVALID_PASSWORD = 0x81000300 + 0x3,
    IM_E_ALREADY_LOGGED_ON = 0x81000300 + 0x4,IM_E_SERVER_VERSION = 0x81000300 + 0x5,IM_E_LOGON_TIMEOUT = 0x81000300 + 0x6,
    IM_E_LIST_FULL = 0x81000300 + 0x7,IM_E_AI_REJECT = 0x81000300 + 0x8,IM_E_AI_REJECT_NOT_INST = 0x81000300 + 0x9,
    IM_E_USER_NOT_FOUND = 0x81000300 + 0xa,IM_E_ALREADY_IN_LIST = 0x81000300 + 0xb,IM_E_DISCONNECTED = 0x81000300 + 0xc,
    IM_E_UNEXPECTED = 0x81000300 + 0xd,IM_E_SERVER_TOO_BUSY = 0x81000300 + 0xe,IM_E_INVALID_AUTH_PACKAGES = 0x81000300 + 0xf,
    IM_E_NEWER_CLIENT_AVAILABLE = 0x81000300 + 0x10,IM_E_AI_TIMEOUT = 0x81000300 + 0x11,IM_E_CANCEL = 0x81000300 + 0x12,
    IM_E_TOO_MANY_MATCHES = 0x81000300 + 0x13,IM_E_SERVER_UNAVAILABLE = 0x81000300 + 0x14,IM_E_LOGON_UI_ACTIVE = 0x81000300 + 0x15,
    IM_E_OPTION_UI_ACTIVE = 0x81000300 + 0x16,IM_E_CONTACT_UI_ACTIVE = 0x81000300 + 0x17,IM_E_LOGGED_ON = 0x81000300 + 0x19,
    IM_E_CONNECT_PROXY = 0x81000300 + 0x1a,IM_E_PROXY_AUTH = 0x81000300 + 0x1b,IM_E_PROXY_AUTH_TYPE = 0x81000300 + 0x1c,
    IM_E_INVALID_PROXY_NAME = 0x81000300 + 0x1d,IM_E_NOT_PRIMARY_SERVICE = 0x81000300 + 0x20,IM_E_TOO_MANY_SESSIONS = 0x81000300 + 0x21,
    IM_E_TOO_MANY_MESSAGES = 0x81000300 + 0x22,IM_E_REMOTE_LOGIN = 0x81000300 + 0x23,IM_E_INVALID_FRIENDLY_NAME = 0x81000300 + 0x24,
    IM_E_SESSION_FULL = 0x81000300 + 0x25,IM_E_NOT_ALLOWING_NEW_USERS = 0x81000300 + 0x26,IM_E_INVALID_DOMAIN = 0x81000300 + 0x27,
    IM_E_TCP_ERROR = 0x81000300 + 0x28,IM_E_SESSION_TIMEOUT = 0x81000300 + 0x29,IM_E_MULTIPOINT_SESSION_BEGIN_TIMEOUT = 0x81000300 + 0x2a,
    IM_E_MULTIPOINT_SESSION_END_TIMEOUT = 0x81000300 + 0x2b,IM_E_REVERSE_LIST_FULL = 0x81000300 + 0x2c,IM_E_SERVER_ERROR = 0x81000300 + 0x2d,
    IM_E_SYSTEM_CONFIG = 0x81000300 + 0x2e,IM_E_NO_DIRECTORY = 0x81000300 + 0x2f,IM_E_USER_CANCELED_LOGON = 0x81000300 + 0x50,
    IM_E_ALREADY_EXISTS = 0x81000300 + 0x51,IM_E_DOES_NOT_EXIST = 0x81000300 + 0x52,IM_S_LOGGED_ON = 0x1000300 + 0x19,
    IM_S_ALREADY_IN_THE_MODE = 0x1000300 + 0x1
  } IM_RESULTS;

  typedef enum __MIDL___MIDL_itf_msimcsdk_0000_0002 {
    IM_MSG_TYPE_NO_RESULT = 0,IM_MSG_TYPE_ERRORS_ONLY = 1,IM_MSG_TYPE_ALL_RESULTS = 2
  } IM_MSG_TYPE;

  typedef enum __MIDL___MIDL_itf_msimcsdk_0000_0003 {
    IM_INVITE_TYPE_REQUEST_LAUNCH = 0x1,IM_INVITE_TYPE_REQUEST_IP = 0x4,IM_INVITE_TYPE_PROVIDE_IP = 0x8
  } IM_INVITE_FLAGS;

  typedef enum __MIDL___MIDL_itf_msimcsdk_0000_0004 {
    IM_STATE_UNKNOWN = 0,IM_STATE_OFFLINE = 0x1,IM_STATE_ONLINE = 0x2,IM_STATE_INVISIBLE = 0x6,IM_STATE_BUSY = 0xa,IM_STATE_BE_RIGHT_BACK = 0xe,IM_STATE_IDLE = 0x12,IM_STATE_AWAY = 0x22,IM_STATE_ON_THE_PHONE = 0x32,IM_STATE_OUT_TO_LUNCH = 0x42,IM_STATE_LOCAL_FINDING_SERVER = 0x100,IM_STATE_LOCAL_CONNECTING_TO_SERVER = 0x200,IM_STATE_LOCAL_SYNCHRONIZING_WITH_SERVER = 0x300,IM_STATE_LOCAL_DISCONNECTING_FROM_SERVER = 0x400
  } IM_STATE;

  typedef enum __MIDL___MIDL_itf_msimcsdk_0000_0005 {
    IM_SSTATE_DISCONNECTED = 0,IM_SSTATE_CONNECTING = 1,IM_SSTATE_CONNECTED = 2,IM_SSTATE_DISCONNECTING = 3,IM_SSTATE_ERROR = 4
  } IM_SSTATE;

  typedef enum __MIDL___MIDL_itf_msimcsdk_0000_0006 {
    MSIM_LIST_CONTACT = 0x1,MSIM_LIST_ALLOW = 0x2,MSIM_LIST_BLOCK = 0x4,MSIM_LIST_REVERSE = 0x8,MSIM_LIST_NOREF = 0x10,
    MSIM_LIST_SAVE = 0x20,MSIM_LIST_SYSTEM = 0x80
  } MSIM_LIST_TYPE;

  typedef enum __MIDL___MIDL_itf_msimcsdk_0000_0007 {
    MSIMWND_WS_OVERLAPPED = 0,MSIMWND_WS_TOOL = 1,MSIMWND_WS_POPUP = 2,MSIMWND_WS_DIALOG = 3,MSIMWND_WS_SIZEBOX = 4
  } MSIMWND_STYLES;

  typedef enum __MIDL___MIDL_itf_msimcsdk_0000_0008 {
    MSIMWND_SIZE_MAXHIDE = 1,MSIMWND_SIZE_MAXIMIZED = 2,MSIMWND_SIZE_MAXSHOW = 3,MSIMWND_SIZE_MINIMIZED = 4,MSIMWND_SIZE_RESTORED = 5
  } MSIMWND_SIZE_TYPE;

#define MSIM_LIST_CONTACT 0x00000001
#define MSIM_LIST_ALLOW 0x00000002
#define MSIM_LIST_BLOCK 0x00000004
#define MSIM_LIST_REVERSE 0x00000008
#define MSIM_LIST_NOREF 0x00000010
#define MSIM_LIST_SAVE 0x00000020
#define MSIM_LIST_SYSTEM 0x00000080
#define MSIM_LIST_CONTACT_STR L"$$Messenger\\Contact"
#define MSIM_LIST_ALLOW_STR L"$$Messenger\\Allow"
#define MSIM_LIST_BLOCK_STR L"$$Messenger\\Block"
#define MSIM_LIST_REVERSE_STR L"$$Messenger\\Reverse"

  extern RPC_IF_HANDLE __MIDL_itf_msimcsdk_0000_v0_0_c_ifspec;
  extern RPC_IF_HANDLE __MIDL_itf_msimcsdk_0000_v0_0_s_ifspec;

#ifndef __MSIMCliSDKLib_LIBRARY_DEFINED__
#define __MSIMCliSDKLib_LIBRARY_DEFINED__
  EXTERN_C const IID LIBID_MSIMCliSDKLib;
#ifndef __IMSIMHost_INTERFACE_DEFINED__
#define __IMSIMHost_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IMSIMHost;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IMSIMHost : public IDispatch {
  public:
    virtual HRESULT WINAPI CreateContext(VARIANT Profile,VARIANT Flags,IDispatch **ppInterface) = 0;
    virtual HRESULT WINAPI ShowOptions(void) = 0;
    virtual HRESULT WINAPI get_Profiles(IDispatch **pProfile) = 0;
    virtual HRESULT WINAPI HostWindow(BSTR bstrControl,__LONG32 lStyle,VARIANT_BOOL fShowOnTaskbar,IDispatch **ppMSIMWnd) = 0;
    virtual HRESULT WINAPI CreateProfile(BSTR bstrProfile,IDispatch **ppProfile) = 0;
    virtual HRESULT WINAPI PopupMessage(BSTR bstrMessage,__LONG32 nTimeout,VARIANT_BOOL fClick,__LONG32 *plCookie) = 0;
    virtual HRESULT WINAPI HostWindowEx(BSTR bstrControl,__LONG32 lStyle,__LONG32 lExStyle,IStream *pStream,IMSIMWindow **ppMSIMWindow,IUnknown **ppUnk,REFIID iidAdvise,IUnknown *punkSink) = 0;
  };
#else
  typedef struct IMSIMHostVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IMSIMHost *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IMSIMHost *This);
      ULONG (WINAPI *Release)(IMSIMHost *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IMSIMHost *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IMSIMHost *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IMSIMHost *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IMSIMHost *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *CreateContext)(IMSIMHost *This,VARIANT Profile,VARIANT Flags,IDispatch **ppInterface);
      HRESULT (WINAPI *ShowOptions)(IMSIMHost *This);
      HRESULT (WINAPI *get_Profiles)(IMSIMHost *This,IDispatch **pProfile);
      HRESULT (WINAPI *HostWindow)(IMSIMHost *This,BSTR bstrControl,__LONG32 lStyle,VARIANT_BOOL fShowOnTaskbar,IDispatch **ppMSIMWnd);
      HRESULT (WINAPI *CreateProfile)(IMSIMHost *This,BSTR bstrProfile,IDispatch **ppProfile);
      HRESULT (WINAPI *PopupMessage)(IMSIMHost *This,BSTR bstrMessage,__LONG32 nTimeout,VARIANT_BOOL fClick,__LONG32 *plCookie);
      HRESULT (WINAPI *HostWindowEx)(IMSIMHost *This,BSTR bstrControl,__LONG32 lStyle,__LONG32 lExStyle,IStream *pStream,IMSIMWindow **ppMSIMWindow,IUnknown **ppUnk,REFIID iidAdvise,IUnknown *punkSink);
    END_INTERFACE
  } IMSIMHostVtbl;
  struct IMSIMHost {
    CONST_VTBL struct IMSIMHostVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IMSIMHost_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IMSIMHost_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IMSIMHost_Release(This) (This)->lpVtbl->Release(This)
#define IMSIMHost_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IMSIMHost_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IMSIMHost_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IMSIMHost_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IMSIMHost_CreateContext(This,Profile,Flags,ppInterface) (This)->lpVtbl->CreateContext(This,Profile,Flags,ppInterface)
#define IMSIMHost_ShowOptions(This) (This)->lpVtbl->ShowOptions(This)
#define IMSIMHost_get_Profiles(This,pProfile) (This)->lpVtbl->get_Profiles(This,pProfile)
#define IMSIMHost_HostWindow(This,bstrControl,lStyle,fShowOnTaskbar,ppMSIMWnd) (This)->lpVtbl->HostWindow(This,bstrControl,lStyle,fShowOnTaskbar,ppMSIMWnd)
#define IMSIMHost_CreateProfile(This,bstrProfile,ppProfile) (This)->lpVtbl->CreateProfile(This,bstrProfile,ppProfile)
#define IMSIMHost_PopupMessage(This,bstrMessage,nTimeout,fClick,plCookie) (This)->lpVtbl->PopupMessage(This,bstrMessage,nTimeout,fClick,plCookie)
#define IMSIMHost_HostWindowEx(This,bstrControl,lStyle,lExStyle,pStream,ppMSIMWindow,ppUnk,iidAdvise,punkSink) (This)->lpVtbl->HostWindowEx(This,bstrControl,lStyle,lExStyle,pStream,ppMSIMWindow,ppUnk,iidAdvise,punkSink)
#endif
#endif
  HRESULT WINAPI IMSIMHost_CreateContext_Proxy(IMSIMHost *This,VARIANT Profile,VARIANT Flags,IDispatch **ppInterface);
  void __RPC_STUB IMSIMHost_CreateContext_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IMSIMHost_ShowOptions_Proxy(IMSIMHost *This);
  void __RPC_STUB IMSIMHost_ShowOptions_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IMSIMHost_get_Profiles_Proxy(IMSIMHost *This,IDispatch **pProfile);
  void __RPC_STUB IMSIMHost_get_Profiles_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IMSIMHost_HostWindow_Proxy(IMSIMHost *This,BSTR bstrControl,__LONG32 lStyle,VARIANT_BOOL fShowOnTaskbar,IDispatch **ppMSIMWnd);
  void __RPC_STUB IMSIMHost_HostWindow_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IMSIMHost_CreateProfile_Proxy(IMSIMHost *This,BSTR bstrProfile,IDispatch **ppProfile);
  void __RPC_STUB IMSIMHost_CreateProfile_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IMSIMHost_PopupMessage_Proxy(IMSIMHost *This,BSTR bstrMessage,__LONG32 nTimeout,VARIANT_BOOL fClick,__LONG32 *plCookie);
  void __RPC_STUB IMSIMHost_PopupMessage_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IMSIMHost_HostWindowEx_Proxy(IMSIMHost *This,BSTR bstrControl,__LONG32 lStyle,__LONG32 lExStyle,IStream *pStream,IMSIMWindow **ppMSIMWindow,IUnknown **ppUnk,REFIID iidAdvise,IUnknown *punkSink);
  void __RPC_STUB IMSIMHost_HostWindowEx_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __DMSIMHostEvents_DISPINTERFACE_DEFINED__
#define __DMSIMHostEvents_DISPINTERFACE_DEFINED__
  EXTERN_C const IID DIID_DMSIMHostEvents;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct DMSIMHostEvents : public IDispatch {
  };
#else
  typedef struct DMSIMHostEventsVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(DMSIMHostEvents *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(DMSIMHostEvents *This);
      ULONG (WINAPI *Release)(DMSIMHostEvents *This);
      HRESULT (WINAPI *GetTypeInfoCount)(DMSIMHostEvents *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(DMSIMHostEvents *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(DMSIMHostEvents *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(DMSIMHostEvents *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
    END_INTERFACE
  } DMSIMHostEventsVtbl;
  struct DMSIMHostEvents {
    CONST_VTBL struct DMSIMHostEventsVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define DMSIMHostEvents_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define DMSIMHostEvents_AddRef(This) (This)->lpVtbl->AddRef(This)
#define DMSIMHostEvents_Release(This) (This)->lpVtbl->Release(This)
#define DMSIMHostEvents_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define DMSIMHostEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define DMSIMHostEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define DMSIMHostEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#endif
#endif
#endif

#ifndef __IMSIMWindow_INTERFACE_DEFINED__
#define __IMSIMWindow_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IMSIMWindow;
#if defined(__cplusplus) && !defined(CINTERFACE)

  struct IMSIMWindow : public IDispatch {
  public:
    virtual HRESULT WINAPI get_Object(IDispatch **ppDisp) = 0;
    virtual HRESULT WINAPI Move(__LONG32 nX,__LONG32 nY,__LONG32 nWidth,__LONG32 nHeight) = 0;
    virtual HRESULT WINAPI Focus(void) = 0;
    virtual HRESULT WINAPI Show(void) = 0;
    virtual HRESULT WINAPI Hide(void) = 0;
    virtual HRESULT WINAPI get_Title(BSTR *pVal) = 0;
    virtual HRESULT WINAPI put_Title(BSTR newVal) = 0;
    virtual HRESULT WINAPI Close(void) = 0;
    virtual HRESULT WINAPI get_HasFocus(VARIANT_BOOL *pVal) = 0;
    virtual HRESULT WINAPI get_IsVisible(VARIANT_BOOL *pVal) = 0;
    virtual HRESULT WINAPI GetPosition(VARIANT *pvarX,VARIANT *pvarY,VARIANT *pvarWidth,VARIANT *pvarHeight) = 0;
    virtual HRESULT WINAPI get_TopMost(VARIANT_BOOL *pVal) = 0;
    virtual HRESULT WINAPI put_TopMost(VARIANT_BOOL newVal) = 0;
    virtual HRESU