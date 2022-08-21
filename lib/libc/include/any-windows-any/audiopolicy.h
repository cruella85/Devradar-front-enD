/*** Autogenerated by WIDL 7.0 from include/audiopolicy.idl - Do not edit ***/

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

#ifndef __audiopolicy_h__
#define __audiopolicy_h__

/* Forward declarations */

#ifndef __IAudioSessionEvents_FWD_DEFINED__
#define __IAudioSessionEvents_FWD_DEFINED__
typedef interface IAudioSessionEvents IAudioSessionEvents;
#ifdef __cplusplus
interface IAudioSessionEvents;
#endif /* __cplusplus */
#endif

#ifndef __IAudioSessionControl_FWD_DEFINED__
#define __IAudioSessionControl_FWD_DEFINED__
typedef interface IAudioSessionControl IAudioSessionControl;
#ifdef __cplusplus
interface IAudioSessionControl;
#endif /* __cplusplus */
#endif

#ifndef __IAudioSessionControl2_FWD_DEFINED__
#define __IAudioSessionControl2_FWD_DEFINED__
typedef interface IAudioSessionControl2 IAudioSessionControl2;
#ifdef __cplusplus
interface IAudioSessionControl2;
#endif /* __cplusplus */
#endif

#ifndef __IAudioSessionManager_FWD_DEFINED__
#define __IAudioSessionManager_FWD_DEFINED__
typedef interface IAudioSessionManager IAudioSessionManager;
#ifdef __cplusplus
interface IAudioSessionManager;
#endif /* __cplusplus */
#endif

#ifndef __IAudioVolumeDuckNotification_FWD_DEFINED__
#define __IAudioVolumeDuckNotification_FWD_DEFINED__
typedef interface IAudioVolumeDuckNotification IAudioVolumeDuckNotification;
#ifdef __cplusplus
interface IAudioVolumeDuckNotification;
#endif /* __cplusplus */
#endif

#ifndef __IAudioSessionNotification_FWD_DEFINED__
#define __IAudioSessionNotification_FWD_DEFINED__
typedef interface IAudioSessionNotification IAudioSessionNotification;
#ifdef __cplusplus
interface IAudioSessionNotification;
#endif /* __cplusplus */
#endif

#ifndef __IAudioSessionEnumerator_FWD_DEFINED__
#define __IAudioSessionEnumerator_FWD_DEFINED__
typedef interface IAudioSessionEnumerator IAudioSessionEnumerator;
#ifdef __cplusplus
interface IAudioSessionEnumerator;
#endif /* __cplusplus */
#endif

#ifndef __IAudioSessionManager2_FWD_DEFINED__
#define __IAudioSessionManager2_FWD_DEFINED__
typedef interface IAudioSessionManager2 IAudioSessionManager2;
#ifdef __cplusplus
interface IAudioSessionManager2;
#endif /* __cplusplus */
#endif

/* Headers for imported files */

#include <oaidl.h>
#include <ocidl.h>
#include <propidl.h>
#include <audiosessiontypes.h>
#include <audioclient.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <winapifamily.h>


#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
typedef enum AudioSessionDisconnectReason {
    DisconnectReasonDeviceRemoval = 0,
    DisconnectReasonServerShutdown = 1,
    DisconnectReasonFormatChanged = 2,
    DisconnectReasonSessionLogoff = 3,
    DisconnectReasonSessionDisconnected = 4,
    DisconnectReasonExclusiveModeOverride = 5
} AudioSessionDisconnectReason;

/*****************************************************************************
 * IAudioSessionEvents interface
 */
#ifndef __IAudioSessionEvents_INTERFACE_DEFINED__
#define __IAudioSessionEvents_INTERFACE_DEFINED__

DEFINE_GUID(IID_IAudioSessionEvents, 0x24918acc, 0x64b3, 0x37c1, 0x8c,0xa9, 0x74,0xa6,0x6e,0x99,0x57,0xa8);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("24918acc-64b3-37c1-8ca9-74a66e9957a8")
IAudioSessionEvents : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(
        LPCWSTR NewDisplayName,
        LPCGUID EventContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnIconPathChanged(
        LPCWSTR NewIconPath,
        LPCGUID EventContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
        float NewVolume,
        WINBOOL NewMute,
        LPCGUID EventContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(
        DWORD ChannelCount,
        float NewChannelVolumeArray[],
        DWORD ChangedChannel,
        LPCGUID EventContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(
        LPCGUID NewGroupingParam,
        LPCGUID EventContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnStateChanged(
        AudioSessionState NewState) = 0;

    virtual HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
        AudioSessionDisconnectReason DisconnectReason) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IAudioSessionEvents, 0x24918acc, 0x64b3, 0x37c1, 0x8c,0xa9, 0x74,0xa6,0x6e,0x99,0x57,0xa8)
#endif
#else
typedef struct IAudioSessionEventsVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IAudioSessionEvents *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IAudioSessionEvents *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IAudioSessionEvents *This);

    /*** IAudioSessionEvents methods ***/
    HRESULT (STDMETHODCALLTYPE *OnDisplayNameChanged)(
        IAudioSessionEvents *This,
        LPCWSTR NewDisplayName,
        LPCGUID EventContext);

    HRESULT (STDMETHODCALLTYPE *OnIconPathChanged)(
        IAudioSessionEvents *This,
        LPCWSTR NewIconPath,
        LPCGUID EventContext);

    HRESULT (STDMETHODCALLTYPE *OnSimpleVolumeChanged)(
        IAudioSessionEvents *This,
        float NewVolume,
        WINBOOL NewMute,
        LPCGUID EventContext);

    HRESULT (STDMETHODCALLTYPE *OnChannelVolumeChanged)(
        IAudioSessionEvents *This,
        DWORD ChannelCount,
        float NewChannelVolumeArray[],
        DWORD ChangedChannel,
        LPCGUID EventContext);

    HRESULT (STDMETHODCALLTYPE *OnGroupingParamChanged)(
        IAudioSessionEvents *This,
        LPCGUID NewGroupingParam,
        LPCGUID EventContext);

    HRESULT (STDMETHODCALLTYPE *OnStateChanged)(
        IAudioSessionEvents *This,
        AudioSessionState NewState);

    HRESULT (STDMETHODCALLTYPE *OnSessionDisconnected)(
        IAudioSessionEvents *This,
        AudioSessionDisconnectReason DisconnectReason);

    END_INTERFACE
} IAudioSessionEventsVtbl;

interface IAudioSessionEvents {
    CONST_VTBL IAudioSessionEventsVtbl* lpVtbl;
};

#ifdef COBJMACROS
#ifndef WIDL_C_INLINE_WRAPPERS
/*** IUnknown methods ***/
#define IAudioSessionEvents_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAudioSessionEvents_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IAudioSessionEvents_Release(This) (This)->lpVtbl->Release(This)
/*** IAudioSessionEvents methods ***/
#define IAudioSessionEvents_OnDisplayNameChanged(This,NewDisplayName,EventContext) (This)->lpVtbl->OnDisplayNameChanged(This,NewDisplayName,EventContext)
#define IAudioSessionEvents_OnIconPathChanged(This,NewIconPath,EventContext) (This)->lpVtbl->OnIconPathChanged(This,NewIconPath,EventContext)
#define IAudioSessionEvents_OnSimpleVolumeChanged(This,NewVolume,NewMute,EventContext) (This)->lpVtbl->OnSimpleVolumeChanged(This,NewVolume,NewMute,EventContext)
#define IAudioSessionEvents_OnChannelVolumeChanged(This,ChannelCount,NewChannelVolumeArray,ChangedChannel,EventContext) (This)->lpVtbl->OnChannelVolumeChanged(This,ChannelCount,NewChannelVolumeArray,ChangedChannel,EventContext)
#define IAudioSessionEvents_OnGroupingParamChanged(This,NewGroupingParam,EventContext) (This)->lpVtbl->OnGroupingParamChanged(This,NewGroupingParam,EventContext)
#define IAudioSessionEvents_OnStateChanged(This,NewState) (This)->lpVtbl->OnStateChanged(This,NewState)
#define IAudioSessionEvents_OnSessionDisconnected(This,DisconnectReason) (This)->lpVtbl->OnSessionDisconnected(This,DisconnectReason)
#else
/*** IUnknown methods ***/
static FORCEINLINE HRESULT IAudioSessionEvents_QueryInterface(IAudioSessionEvents* This,REFIID riid,void **ppvObject) {
    return This->lpVtbl->QueryInterface(This,riid,ppvObject);
}
static FORCEINLINE ULONG IAudioSessionEvents_AddRef(IAudioSessionEvents* This) {
    return This->lpVtbl->AddRef(This);
}
static FORCEINLINE ULONG IAudioSessionEvents_Release(IAudioSessionEvents* This) {
    return This->lpVtbl->Release(This);
}
/*** IAudioSessionEvents methods ***/
static FORCEINLINE HRESULT IAudioSessionEvents_OnDisplayNameChanged(IAudioSessionEvents* This,LPCWSTR NewDisplayName,LPCGUID EventContext) {
    return This->lpVtbl->OnDisplayNameChanged(This,NewDisplayName,EventContext);
}
static FORCEINLINE HRESULT IAudioSessionEvents_OnIconPathChanged(IAudioSessionEvents* This,LPCWSTR NewIconPath,LPCGUID EventContext) {
    return This->lpVtbl->OnIconPathChanged(This,NewIconPath,EventContext);
}
static FORCEINLINE HRESULT IAudioSessionEvents_OnSimpleVolumeChanged(IAudioSessionEvents* This,float NewVolume,WINBOOL NewMute,LPCGUID EventContext) {
    return This->lpVtbl->OnSimpleVolumeChanged(This,NewVolume,NewMute,EventContext);
}
static FORCEINLINE HRESULT IAudioSessionEvents_OnChannelVolumeChanged(IAudioSessionEvents* This,DWORD ChannelCount,float NewChannelVolumeArray[],DWORD ChangedChannel,LPCGUID EventContext) {
    return This->lpVtbl->OnChannelVolumeChanged(This,ChannelCount,NewChannelVolumeArray,ChangedChannel,EventContext);
}
static FORCEINLINE HRESULT IAudioSessionEvents_OnGroupingParamChanged(IAudioSessionEvents* This,LPCGUID NewGroupingParam,LPCGUID EventContext) {
    return This->lpVtbl->OnGroupingParamChanged(This,NewGroupingParam,EventContext);
}
static FORCEINLINE HRESULT IAudioSessionEvents_OnStateChanged(IAudioSessionEvents* This,AudioSessionState NewState) {
    return This->lpVtbl->OnStateChanged(This,NewState);
}
static FORCEINLINE HRESULT IAudioSessionEvents_OnSessionDisconnected(IAudioSessionEvents* This,AudioSessionDisconnectReason DisconnectReason) {
    return This->lpVtbl->OnSessionDisconnected(This,DisconnectReason);
}
#endif
#endif

#endif


#endif  /* __IAudioSessionEvents_INTERFACE_DEFINED__ */


/*****************************************************************************
 * IAudioSessionControl interface
 */
#ifndef __IAudioSessionControl_INTERFACE_DEFINED__
#define __IAudioSessionControl_INTERFACE_DEFINED__

DEFINE_GUID(IID_IAudioSessionControl, 0xf4b1a599, 0x7266, 0x4319, 0xa8,0xca, 0xe7,0x0a,0xcb,0x11,0xe8,0xcd);
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("f4b1a599-7266-4319-a8ca-e70acb11e8cd")
IAudioSessionControl : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetState(
        AudioSessionState *pRetVal) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(
        LPWSTR *pRetVal) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDisplayName(
        LPCWSTR Value,
        LPCGUID EventContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetIconPath(
        LPWSTR *pRetVal) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetIconPath(
        LPCWSTR Value,
        LPCGUID EventContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetGroupingParam(
        GUID *pRetVal) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetGroupingParam(
        LPCGUID Override,
        LPCGUID EventContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE RegisterAudioSessionNotification(
        IAudioSessionEvents *NewNotifications) = 0;

    virtual HRESULT STDMETHODCALLTYPE UnregisterAudioSessionNotification(
        IAudioSessionEvents *NewNotifications) = 0;

};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IAudioSessionControl, 0xf4b1a599, 0x7266, 0x4319, 0xa8,0xca, 0xe7,0x0a,0xcb,0x11,0xe8,0xcd)
#endif
#else
typedef struct IAudioSessionControlVtbl {
    BEGIN_INTERFACE

    /*** IUnknown methods ***/
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(
        IAudioSessionControl *This,
        REFIID riid,
        void **ppvObject);

    ULONG (STDMETHODCALLTYPE *AddRef)(
        IAudioSessionControl *This);

    ULONG (STDMETHODCALLTYPE *Release)(
        IAudioSessionControl *This);

    /*** IAudioSessionControl methods ***/
    HRESULT (STDMETHODCALLTYPE *GetState)(
        IAudioSessionControl *This,
        AudioSessionState *pRetVal);

    HRESULT (STDMETHODCALLTYPE *GetDisplayName)(
        IAudioSessionControl *This,
        LPWSTR *pRetVal);

    HRESULT (STDMETHODCALLTYPE *SetDisplayName)(
        IAudioSessionControl *This,
        LPCWSTR Value,
        LPCGUID EventContext);

    HRESULT (STDMETHODCALLTYPE *GetIconPath)(
        IAudioSessionControl *This,
        LPWSTR *pRetVal);

    HRESULT (STDMETHODCALLTYPE *SetIconPath)(
        IAudioSessionControl *This,
        LPCWSTR Value,
        LPCGUID EventContext);

    HRESULT (STDMETHODCALLTYPE *GetGroupingParam)(
        IAudioSessionControl *This,
        GUID *pRetVal);

    HRESULT (STDMETHODCALLTYPE *SetGroupingParam)(
        IAudioSessionControl *This,
        LPCGUID Override,
        LPCGUID EventContext);

    HRESULT (STDMETHODCALLTYPE *RegisterAudioSessionNotification)(
        IAudioSessionControl *This,
        IAudioSessionEvents *NewNotificat