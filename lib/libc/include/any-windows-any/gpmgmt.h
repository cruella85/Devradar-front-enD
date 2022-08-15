/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error This stub requires an updated version of <rpcndr.h>
#endif

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif

#ifndef __gpmgmt_h__
#define __gpmgmt_h__

#ifndef __IGPM_FWD_DEFINED__
#define __IGPM_FWD_DEFINED__
typedef struct IGPM IGPM;
#endif

#ifndef __IGPMDomain_FWD_DEFINED__
#define __IGPMDomain_FWD_DEFINED__
typedef struct IGPMDomain IGPMDomain;
#endif

#ifndef __IGPMBackupDir_FWD_DEFINED__
#define __IGPMBackupDir_FWD_DEFINED__
typedef struct IGPMBackupDir IGPMBackupDir;
#endif

#ifndef __IGPMSitesContainer_FWD_DEFINED__
#define __IGPMSitesContainer_FWD_DEFINED__
typedef struct IGPMSitesContainer IGPMSitesContainer;
#endif

#ifndef __IGPMSearchCriteria_FWD_DEFINED__
#define __IGPMSearchCriteria_FWD_DEFINED__
typedef struct IGPMSearchCriteria IGPMSearchCriteria;
#endif

#ifndef __IGPMTrustee_FWD_DEFINED__
#define __IGPMTrustee_FWD_DEFINED__
typedef struct IGPMTrustee IGPMTrustee;
#endif

#ifndef __IGPMPermission_FWD_DEFINED__
#define __IGPMPermission_FWD_DEFINED__
typedef struct IGPMPermission IGPMPermission;
#endif

#ifndef __IGPMSecurityInfo_FWD_DEFINED__
#define __IGPMSecurityInfo_FWD_DEFINED__
typedef struct IGPMSecurityInfo IGPMSecurityInfo;
#endif

#ifndef __IGPMBackup_FWD_DEFINED__
#define __IGPMBackup_FWD_DEFINED__
typedef struct IGPMBackup IGPMBackup;
#endif

#ifndef __IGPMBackupCollection_FWD_DEFINED__
#define __IGPMBackupCollection_FWD_DEFINED__
typedef struct IGPMBackupCollection IGPMBackupCollection;
#endif

#ifndef __IGPMSOM_FWD_DEFINED__
#define __IGPMSOM_FWD_DEFINED__
typedef struct IGPMSOM IGPMSOM;
#endif

#ifndef __IGPMSOMCollection_FWD_DEFINED__
#define __IGPMSOMCollection_FWD_DEFINED__
typedef struct IGPMSOMCollection IGPMSOMCollection;
#endif

#ifndef __IGPMWMIFilter_FWD_DEFINED__
#define __IGPMWMIFilter_FWD_DEFINED__
typedef struct IGPMWMIFilter IGPMWMIFilter;
#endif

#ifndef __IGPMWMIFilterCollection_FWD_DEFINED__
#define __IGPMWMIFilterCollection_FWD_DEFINED__
typedef struct IGPMWMIFilterCollection IGPMWMIFilterCollection;
#endif

#ifndef __IGPMRSOP_FWD_DEFINED__
#define __IGPMRSOP_FWD_DEFINED__
typedef struct IGPMRSOP IGPMRSOP;
#endif

#ifndef __IGPMGPO_FWD_DEFINED__
#define __IGPMGPO_FWD_DEFINED__
typedef struct IGPMGPO IGPMGPO;
#endif

#ifndef __IGPMGPOCollection_FWD_DEFINED__
#define __IGPMGPOCollection_FWD_DEFINED__
typedef struct IGPMGPOCollection IGPMGPOCollection;
#endif

#ifndef __IGPMGPOLink_FWD_DEFINED__
#define __IGPMGPOLink_FWD_DEFINED__
typedef struct IGPMGPOLink IGPMGPOLink;
#endif

#ifndef __IGPMGPOLinksCollection_FWD_DEFINED__
#define __IGPMGPOLinksCollection_FWD_DEFINED__
typedef struct IGPMGPOLinksCollection IGPMGPOLinksCollection;
#endif

#ifndef __IGPMCSECollection_FWD_DEFINED__
#define __IGPMCSECollection_FWD_DEFINED__
typedef struct IGPMCSECollection IGPMCSECollection;
#endif

#ifndef __IGPMClientSideExtension_FWD_DEFINED__
#define __IGPMClientSideExtension_FWD_DEFINED__
typedef struct IGPMClientSideExtension IGPMClientSideExtension;
#endif

#ifndef __IGPMAsyncCancel_FWD_DEFINED__
#define __IGPMAsyncCancel_FWD_DEFINED__
typedef struct IGPMAsyncCancel IGPMAsyncCancel;
#endif

#ifndef __IGPMAsyncProgress_FWD_DEFINED__
#define __IGPMAsyncProgress_FWD_DEFINED__
typedef struct IGPMAsyncProgress IGPMAsyncProgress;
#endif

#ifndef __IGPMStatusMsgCollection_FWD_DEFINED__
#define __IGPMStatusMsgCollection_FWD_DEFINED__
typedef struct IGPMStatusMsgCollection IGPMStatusMsgCollection;
#endif

#ifndef __IGPMStatusMessage_FWD_DEFINED__
#define __IGPMStatusMessage_FWD_DEFINED__
typedef struct IGPMStatusMessage IGPMStatusMessage;
#endif

#ifndef __IGPMConstants_FWD_DEFINED__
#define __IGPMConstants_FWD_DEFINED__
typedef struct IGPMConstants IGPMConstants;
#endif

#ifndef __IGPMResult_FWD_DEFINED__
#define __IGPMResult_FWD_DEFINED__
typedef struct IGPMResult IGPMResult;
#endif

#ifndef __IGPMMapEntryCollection_FWD_DEFINED__
#define __IGPMMapEntryCollection_FWD_DEFINED__
typedef struct IGPMMapEntryCollection IGPMMapEntryCollection;
#endif

#ifndef __IGPMMapEntry_FWD_DEFINED__
#define __IGPMMapEntry_FWD_DEFINED__
typedef struct IGPMMapEntry IGPMMapEntry;
#endif

#ifndef __IGPMMigrationTable_FWD_DEFINED__
#define __IGPMMigrationTable_FWD_DEFINED__
typedef struct IGPMMigrationTable IGPMMigrationTable;
#endif

#ifndef __GPM_FWD_DEFINED__
#define __GPM_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPM GPM;
#else
typedef struct GPM GPM;
#endif
#endif

#ifndef __GPMDomain_FWD_DEFINED__
#define __GPMDomain_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMDomain GPMDomain;
#else
typedef struct GPMDomain GPMDomain;
#endif
#endif

#ifndef __GPMSitesContainer_FWD_DEFINED__
#define __GPMSitesContainer_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMSitesContainer GPMSitesContainer;
#else
typedef struct GPMSitesContainer GPMSitesContainer;
#endif
#endif

#ifndef __GPMBackupDir_FWD_DEFINED__
#define __GPMBackupDir_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMBackupDir GPMBackupDir;
#else
typedef struct GPMBackupDir GPMBackupDir;
#endif
#endif

#ifndef __GPMSOM_FWD_DEFINED__
#define __GPMSOM_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMSOM GPMSOM;
#else
typedef struct GPMSOM GPMSOM;
#endif
#endif

#ifndef __GPMSearchCriteria_FWD_DEFINED__
#define __GPMSearchCriteria_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMSearchCriteria GPMSearchCriteria;
#else
typedef struct GPMSearchCriteria GPMSearchCriteria;
#endif
#endif

#ifndef __GPMPermission_FWD_DEFINED__
#define __GPMPermission_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMPermission GPMPermission;
#else
typedef struct GPMPermission GPMPermission;
#endif
#endif

#ifndef __GPMSecurityInfo_FWD_DEFINED__
#define __GPMSecurityInfo_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMSecurityInfo GPMSecurityInfo;
#else
typedef struct GPMSecurityInfo GPMSecurityInfo;
#endif
#endif

#ifndef __GPMBackup_FWD_DEFINED__
#define __GPMBackup_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMBackup GPMBackup;
#else
typedef struct GPMBackup GPMBackup;
#endif
#endif

#ifndef __GPMBackupCollection_FWD_DEFINED__
#define __GPMBackupCollection_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMBackupCollection GPMBackupCollection;
#else
typedef struct GPMBackupCollection GPMBackupCollection;
#endif
#endif

#ifndef __GPMSOMCollection_FWD_DEFINED__
#define __GPMSOMCollection_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMSOMCollection GPMSOMCollection;
#else
typedef struct GPMSOMCollection GPMSOMCollection;
#endif
#endif

#ifndef __GPMWMIFilter_FWD_DEFINED__
#define __GPMWMIFilter_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMWMIFilter GPMWMIFilter;
#else
typedef struct GPMWMIFilter GPMWMIFilter;
#endif
#endif

#ifndef __GPMWMIFilterCollection_FWD_DEFINED__
#define __GPMWMIFilterCollection_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMWMIFilterCollection GPMWMIFilterCollection;
#else
typedef struct GPMWMIFilterCollection GPMWMIFilterCollection;
#endif
#endif

#ifndef __GPMRSOP_FWD_DEFINED__
#define __GPMRSOP_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMRSOP GPMRSOP;
#else
typedef struct GPMRSOP GPMRSOP;
#endif
#endif

#ifndef __GPMGPO_FWD_DEFINED__
#define __GPMGPO_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMGPO GPMGPO;
#else
typedef struct GPMGPO GPMGPO;
#endif
#endif

#ifndef __GPMGPOCollection_FWD_DEFINED__
#define __GPMGPOCollection_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMGPOCollection GPMGPOCollection;
#else
typedef struct GPMGPOCollection GPMGPOCollection;
#endif
#endif

#ifndef __GPMGPOLink_FWD_DEFINED__
#define __GPMGPOLink_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMGPOLink GPMGPOLink;
#else
typedef struct GPMGPOLink GPMGPOLink;
#endif
#endif

#ifndef __GPMGPOLinksCollection_FWD_DEFINED__
#define __GPMGPOLinksCollection_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMGPOLinksCollection GPMGPOLinksCollection;
#else
typedef struct GPMGPOLinksCollection GPMGPOLinksCollection;
#endif
#endif

#ifndef __GPMAsyncCancel_FWD_DEFINED__
#define __GPMAsyncCancel_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMAsyncCancel GPMAsyncCancel;
#else
typedef struct GPMAsyncCancel GPMAsyncCancel;
#endif
#endif

#ifndef __GPMStatusMsgCollection_FWD_DEFINED__
#define __GPMStatusMsgCollection_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMStatusMsgCollection GPMStatusMsgCollection;
#else
typedef struct GPMStatusMsgCollection GPMStatusMsgCollection;
#endif
#endif

#ifndef __GPMStatusMessage_FWD_DEFINED__
#define __GPMStatusMessage_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMStatusMessage GPMStatusMessage;
#else
typedef struct GPMStatusMessage GPMStatusMessage;
#endif
#endif

#ifndef __GPMEnum_FWD_DEFINED__
#define __GPMEnum_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMEnum GPMEnum;
#else
typedef struct GPMEnum GPMEnum;
#endif
#endif

#ifndef __GPMTrustee_FWD_DEFINED__
#define __GPMTrustee_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMTrustee GPMTrustee;
#else
typedef struct GPMTrustee GPMTrustee;
#endif
#endif

#ifndef __GPMClientSideExtension_FWD_DEFINED__
#define __GPMClientSideExtension_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMClientSideExtension GPMClientSideExtension;
#else
typedef struct GPMClientSideExtension GPMClientSideExtension;
#endif
#endif

#ifndef __GPMCSECollection_FWD_DEFINED__
#define __GPMCSECollection_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMCSECollection GPMCSECollection;
#else
typedef struct GPMCSECollection GPMCSECollection;
#endif
#endif

#ifndef __GPMConstants_FWD_DEFINED__
#define __GPMConstants_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMConstants GPMConstants;
#else
typedef struct GPMConstants GPMConstants;
#endif
#endif

#ifndef __GPMResult_FWD_DEFINED__
#define __GPMResult_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMResult GPMResult;
#else
typedef struct GPMResult GPMResult;
#endif
#endif

#ifndef __GPMMapEntryCollection_FWD_DEFINED__
#define __GPMMapEntryCollection_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMMapEntryCollection GPMMapEntryCollection;
#else
typedef struct GPMMapEntryCollection GPMMapEntryCollection;
#endif
#endif

#ifndef __GPMMapEntry_FWD_DEFINED__
#define __GPMMapEntry_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMMapEntry GPMMapEntry;
#else
typedef struct GPMMapEntry GPMMapEntry;
#endif
#endif

#ifndef __GPMMigrationTable_FWD_DEFINED__
#define __GPMMigrationTable_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPMMigrationTable GPMMigrationTable;
#else
typedef struct GPMMigrationTable GPMMigrationTable;
#endif
#endif

#ifndef __GPOReportProvider_FWD_DEFINED__
#define __GPOReportProvider_FWD_DEFINED__
#ifdef __cplusplus
typedef class GPOReportProvider GPOReportProvider;
#else
typedef struct GPOReportProvider GPOReportProvider;
#endif
#endif

#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef __MIDL_user_allocate_free_DEFINED__
#define __MIDL_user_allocate_free_DEFINED__
  void *__RPC_API MIDL_user_allocate(size_t);
  void __RPC_API MIDL_user_free(void *);
#endif

  typedef enum __MIDL___MIDL_itf_gpmgmt_0000_0001 {
    rsopUnknown = 0,rsopPlanning,rsopLogging
  } GPMRSOPMode;

  typedef enum __MIDL___MIDL_itf_gpmgmt_0000_0002 {
    permGPOApply = 0x10000,permGPORead = 0x10100,permGPOEdit = 0x10101,permGPOEditSecurityAndDelete = 0x10102,permGPOCustom = 0x10103,
    permWMIFilterEdit = 0x20000,permWMIFilterFullControl = 0x20001,permWMIFilterCustom = 0x20002,permSOMLink = 0x1c0000,permSOMLogging = 0x180100,
    permSOMPlanning = 0x180200,permSOMWMICreate = 0x100300,permSOMWMIFullControl = 0x100301,permSOMGPOCreate = 0x100400,
    permStarterGPORead = 0x30500,permStarterGPOEdit = 0x30501,permStarterGPOFullControl = 0x30502,permStarterGPOCustom = 0x30503,
    permSOMStarterGPOCreate = 0x100500
  } GPMPermissionType;

  typedef enum __MIDL___MIDL_itf_gpmgmt_0000_0003 {
    gpoPermissions = 0,gpoEffectivePermissions,gpoDisplayName,gpoWMIFilter,
    gpoID,gpoComputerExtensions,gpoUserExtensions,somLinks,gpoDomain,
    backupMostRecent
  } GPMSearchProperty;

  typedef enum __MIDL___MIDL_itf_gpmgmt_0000_0004 {
    opEquals = 0,opContains,opNotContains,opNotEquals
  } GPMSearchOperation;

  typedef enum __MIDL___MIDL_itf_gpmgmt_0000_0005 {
    repXML = 0,repHTML = repXML + 1
  } GPMReportType;

  typedef enum __MIDL___MIDL_itf_gpmgmt_0000_0006 {
    typeUser = 0,typeComputer,typeLocalGroup,typeGlobalGroup,
    typeUniversalGroup,typeUNCPath,typeUnknown
  } GPMEntryType;

  typedef enum __MIDL___MIDL_itf_gpmgmt_0000_0007 {
    opDestinationSameAsSource = 0,opDestinationNone,opDestinationByRelativeName,
    opDestinationSet
  } GPMDestinationOption;

#define GPM_USE_PDC (0)
#define GPM_USE_ANYDC (1)
#define GPM_DONOTUSE_W2KDC (2)

#define GPM_DONOT_VALIDATEDC (1)

#define GPM_MIGRATIONTABLE_ONLY (0x1)
#define GPM_PROCESS_SECURITY (0x2)

#define RSOP_NO_COMPUTER (0x10000)
#define RSOP_NO_USER (0x20000)
#define RSOP_PLANNING_ASSUME_SLOW_LINK (0x1)
#define RSOP_PLANNING_ASSUME_LOOPBACK_MERGE (0x2)
#define RSOP_PLANNING_ASSUME_LOOPBACK_REPLACE (0x4)
#define RSOP_PLANNING_ASSUME_USER_WQLFILTER_TRUE (0x8)
#define RSOP_PLANNING_ASSUME_COMP_WQLFILTER_TRUE (0x10)

  extern RPC_IF_HANDLE __MIDL_itf_gpmgmt_0000_v0_0_c_ifspec;
  extern RPC_IF_HANDLE __MIDL_itf_gpmgmt_0000_v0_0_s_ifspec;

#ifndef __IGPM_INTERFACE_DEFINED__
#define __IGPM_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPM;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPM : public IDispatch {
  public:
    virtual HRESULT WINAPI GetDomain(BSTR bstrDomain,BSTR bstrDomainController,__LONG32 lDCFlags,IGPMDomain **pIGPMDomain) = 0;
    virtual HRESULT WINAPI GetBackupDir(BSTR bstrBackupDir,IGPMBackupDir **pIGPMBackupDir) = 0;
    virtual HRESULT WINAPI GetSitesContainer(BSTR bstrForest,BSTR bstrDomain,BSTR bstrDomainController,__LONG32 lDCFlags,IGPMSitesContainer **ppIGPMSitesContainer) = 0;
    virtual HRESULT WINAPI GetRSOP(GPMRSOPMode gpmRSoPMode,BSTR bstrNamespace,__LONG32 lFlags,IGPMRSOP **ppIGPMRSOP) = 0;
    virtual HRESULT WINAPI CreatePermission(BSTR bstrTrustee,GPMPermissionType perm,VARIANT_BOOL bInheritable,IGPMPermission **ppPerm) = 0;
    virtual HRESULT WINAPI CreateSearchCriteria(IGPMSearchCriteria **ppIGPMSearchCriteria) = 0;
    virtual HRESULT WINAPI CreateTrustee(BSTR bstrTrustee,IGPMTrustee **ppIGPMTrustee) = 0;
    virtual HRESULT WINAPI GetClientSideExtensions(IGPMCSECollection **ppIGPMCSECollection) = 0;
    virtual HRESULT WINAPI GetConstants(IGPMConstants **ppIGPMConstants) = 0;
    virtual HRESULT WINAPI GetMigrationTable(BSTR bstrMigrationTablePath,IGPMMigrationTable **ppMigrationTable) = 0;
    virtual HRESULT WINAPI CreateMigrationTable(IGPMMigrationTable **ppMigrationTable) = 0;
    virtual HRESULT WINAPI InitializeReporting(BSTR bstrAdmPath) = 0;
  };
#else
  typedef struct IGPMVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPM *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPM *This);
      ULONG (WINAPI *Release)(IGPM *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPM *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPM *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPM *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPM *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *GetDomain)(IGPM *This,BSTR bstrDomain,BSTR bstrDomainController,__LONG32 lDCFlags,IGPMDomain **pIGPMDomain);
      HRESULT (WINAPI *GetBackupDir)(IGPM *This,BSTR bstrBackupDir,IGPMBackupDir **pIGPMBackupDir);
      HRESULT (WINAPI *GetSitesContainer)(IGPM *This,BSTR bstrForest,BSTR bstrDomain,BSTR bstrDomainController,__LONG32 lDCFlags,IGPMSitesContainer **ppIGPMSitesContainer);
      HRESULT (WINAPI *GetRSOP)(IGPM *This,GPMRSOPMode gpmRSoPMode,BSTR bstrNamespace,__LONG32 lFlags,IGPMRSOP **ppIGPMRSOP);
      HRESULT (WINAPI *CreatePermission)(IGPM *This,BSTR bstrTrustee,GPMPermissionType perm,VARIANT_BOOL bInheritable,IGPMPermission **ppPerm);
      HRESULT (WINAPI *CreateSearchCriteria)(IGPM *This,IGPMSearchCriteria **ppIGPMSearchCriteria);
      HRESULT (WINAPI *CreateTrustee)(IGPM *This,BSTR bstrTrustee,IGPMTrustee **ppIGPMTrustee);
      HRESULT (WINAPI *GetClientSideExtensions)(IGPM *This,IGPMCSECollection **ppIGPMCSECollection);
      HRESULT (WINAPI *GetConstants)(IGPM *This,IGPMConstants **ppIGPMConstants);
      HRESULT (WINAPI *GetMigrationTable)(IGPM *This,BSTR bstrMigrationTablePath,IGPMMigrationTable **ppMigrationTable);
      HRESULT (WINAPI *CreateMigrationTable)(IGPM *This,IGPMMigrationTable **ppMigrationTable);
      HRESULT (WINAPI *InitializeReporting)(IGPM *This,BSTR bstrAdmPath);
    END_INTERFACE
  } IGPMVtbl;
  struct IGPM {
    CONST_VTBL struct IGPMVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPM_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPM_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPM_Release(This) (This)->lpVtbl->Release(This)
#define IGPM_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPM_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPM_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPM_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPM_GetDomain(This,bstrDomain,bstrDomainController,lDCFlags,pIGPMDomain) (This)->lpVtbl->GetDomain(This,bstrDomain,bstrDomainController,lDCFlags,pIGPMDomain)
#define IGPM_GetBackupDir(This,bstrBackupDir,pIGPMBackupDir) (This)->lpVtbl->GetBackupDir(This,bstrBackupDir,pIGPMBackupDir)
#define IGPM_GetSitesContainer(This,bstrForest,bstrDomain,bstrDomainController,lDCFlags,ppIGPMSitesContainer) (This)->lpVtbl->GetSitesContainer(This,bstrForest,bstrDomain,bstrDomainController,lDCFlags,ppIGPMSitesContainer)
#define IGPM_GetRSOP(This,gpmRSoPMode,bstrNamespace,lFlags,ppIGPMRSOP) (This)->lpVtbl->GetRSOP(This,gpmRSoPMode,bstrNamespace,lFlags,ppIGPMRSOP)
#define IGPM_CreatePermission(This,bstrTrustee,perm,bInheritable,ppPerm) (This)->lpVtbl->CreatePermission(This,bstrTrustee,perm,bInheritable,ppPerm)
#define IGPM_CreateSearchCriteria(This,ppIGPMSearchCriteria) (This)->lpVtbl->CreateSearchCriteria(This,ppIGPMSearchCriteria)
#define IGPM_CreateTrustee(This,bstrTrustee,ppIGPMTrustee) (This)->lpVtbl->CreateTrustee(This,bstrTrustee,ppIGPMTrustee)
#define IGPM_GetClientSideExtensions(This,ppIGPMCSECollection) (This)->lpVtbl->GetClientSideExtensions(This,ppIGPMCSECollection)
#define IGPM_GetConstants(This,ppIGPMConstants) (This)->lpVtbl->GetConstants(This,ppIGPMConstants)
#define IGPM_GetMigrationTable(This,bstrMigrationTablePath,ppMigrationTable) (This)->lpVtbl->GetMigrationTable(This,bstrMigrationTablePath,ppMigrationTable)
#define IGPM_CreateMigrationTable(This,ppMigrationTable) (This)->lpVtbl->CreateMigrationTable(This,ppMigrationTable)
#define IGPM_InitializeReporting(This,bstrAdmPath) (This)->lpVtbl->InitializeReporting(This,bstrAdmPath)
#endif
#endif
  HRESULT WINAPI IGPM_GetDomain_Proxy(IGPM *This,BSTR bstrDomain,BSTR bstrDomainController,__LONG32 lDCFlags,IGPMDomain **pIGPMDomain);
  void __RPC_STUB IGPM_GetDomain_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_GetBackupDir_Proxy(IGPM *This,BSTR bstrBackupDir,IGPMBackupDir **pIGPMBackupDir);
  void __RPC_STUB IGPM_GetBackupDir_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_GetSitesContainer_Proxy(IGPM *This,BSTR bstrForest,BSTR bstrDomain,BSTR bstrDomainController,__LONG32 lDCFlags,IGPMSitesContainer **ppIGPMSitesContainer);
  void __RPC_STUB IGPM_GetSitesContainer_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_GetRSOP_Proxy(IGPM *This,GPMRSOPMode gpmRSoPMode,BSTR bstrNamespace,__LONG32 lFlags,IGPMRSOP **ppIGPMRSOP);
  void __RPC_STUB IGPM_GetRSOP_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_CreatePermission_Proxy(IGPM *This,BSTR bstrTrustee,GPMPermissionType perm,VARIANT_BOOL bInheritable,IGPMPermission **ppPerm);
  void __RPC_STUB IGPM_CreatePermission_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_CreateSearchCriteria_Proxy(IGPM *This,IGPMSearchCriteria **ppIGPMSearchCriteria);
  void __RPC_STUB IGPM_CreateSearchCriteria_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_CreateTrustee_Proxy(IGPM *This,BSTR bstrTrustee,IGPMTrustee **ppIGPMTrustee);
  void __RPC_STUB IGPM_CreateTrustee_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_GetClientSideExtensions_Proxy(IGPM *This,IGPMCSECollection **ppIGPMCSECollection);
  void __RPC_STUB IGPM_GetClientSideExtensions_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_GetConstants_Proxy(IGPM *This,IGPMConstants **ppIGPMConstants);
  void __RPC_STUB IGPM_GetConstants_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_GetMigrationTable_Proxy(IGPM *This,BSTR bstrMigrationTablePath,IGPMMigrationTable **ppMigrationTable);
  void __RPC_STUB IGPM_GetMigrationTable_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_CreateMigrationTable_Proxy(IGPM *This,IGPMMigrationTable **ppMigrationTable);
  void __RPC_STUB IGPM_CreateMigrationTable_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPM_InitializeReporting_Proxy(IGPM *This,BSTR bstrAdmPath);
  void __RPC_STUB IGPM_InitializeReporting_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMDomain_INTERFACE_DEFINED__
#define __IGPMDomain_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMDomain;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMDomain : public IDispatch {
  public:
    virtual HRESULT WINAPI get_DomainController(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_Domain(BSTR *pVal) = 0;
    virtual HRESULT WINAPI CreateGPO(IGPMGPO **ppNewGPO) = 0;
    virtual HRESULT WINAPI GetGPO(BSTR bstrGuid,IGPMGPO **ppGPO) = 0;
    virtual HRESULT WINAPI SearchGPOs(IGPMSearchCriteria *pIGPMSearchCriteria,IGPMGPOCollection **ppIGPMGPOCollection) = 0;
    virtual HRESULT WINAPI RestoreGPO(IGPMBackup *pIGPMBackup,__LONG32 lDCFlags,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult) = 0;
    virtual HRESULT WINAPI GetSOM(BSTR bstrPath,IGPMSOM **ppSOM) = 0;
    virtual HRESULT WINAPI SearchSOMs(IGPMSearchCriteria *pIGPMSearchCriteria,IGPMSOMCollection **ppIGPMSOMCollection) = 0;
    virtual HRESULT WINAPI GetWMIFilter(BSTR bstrPath,IGPMWMIFilter **ppWMIFilter) = 0;
    virtual HRESULT WINAPI SearchWMIFilters(IGPMSearchCriteria *pIGPMSearchCriteria,IGPMWMIFilterCollection **ppIGPMWMIFilterCollection) = 0;
  };
#else
  typedef struct IGPMDomainVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMDomain *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMDomain *This);
      ULONG (WINAPI *Release)(IGPMDomain *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMDomain *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMDomain *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMDomain *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMDomain *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_DomainController)(IGPMDomain *This,BSTR *pVal);
      HRESULT (WINAPI *get_Domain)(IGPMDomain *This,BSTR *pVal);
      HRESULT (WINAPI *CreateGPO)(IGPMDomain *This,IGPMGPO **ppNewGPO);
      HRESULT (WINAPI *GetGPO)(IGPMDomain *This,BSTR bstrGuid,IGPMGPO **ppGPO);
      HRESULT (WINAPI *SearchGPOs)(IGPMDomain *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMGPOCollection **ppIGPMGPOCollection);
      HRESULT (WINAPI *RestoreGPO)(IGPMDomain *This,IGPMBackup *pIGPMBackup,__LONG32 lDCFlags,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
      HRESULT (WINAPI *GetSOM)(IGPMDomain *This,BSTR bstrPath,IGPMSOM **ppSOM);
      HRESULT (WINAPI *SearchSOMs)(IGPMDomain *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMSOMCollection **ppIGPMSOMCollection);
      HRESULT (WINAPI *GetWMIFilter)(IGPMDomain *This,BSTR bstrPath,IGPMWMIFilter **ppWMIFilter);
      HRESULT (WINAPI *SearchWMIFilters)(IGPMDomain *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMWMIFilterCollection **ppIGPMWMIFilterCollection);
    END_INTERFACE
  } IGPMDomainVtbl;
  struct IGPMDomain {
    CONST_VTBL struct IGPMDomainVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMDomain_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMDomain_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMDomain_Release(This) (This)->lpVtbl->Release(This)
#define IGPMDomain_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMDomain_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMDomain_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMDomain_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMDomain_get_DomainController(This,pVal) (This)->lpVtbl->get_DomainController(This,pVal)
#define IGPMDomain_get_Domain(This,pVal) (This)->lpVtbl->get_Domain(This,pVal)
#define IGPMDomain_CreateGPO(This,ppNewGPO) (This)->lpVtbl->CreateGPO(This,ppNewGPO)
#define IGPMDomain_GetGPO(This,bstrGuid,ppGPO) (This)->lpVtbl->GetGPO(This,bstrGuid,ppGPO)
#define IGPMDomain_SearchGPOs(This,pIGPMSearchCriteria,ppIGPMGPOCollection) (This)->lpVtbl->SearchGPOs(This,pIGPMSearchCriteria,ppIGPMGPOCollection)
#define IGPMDomain_RestoreGPO(This,pIGPMBackup,lDCFlags,pvarGPMProgress,pvarGPMCancel,ppIGPMResult) (This)->lpVtbl->RestoreGPO(This,pIGPMBackup,lDCFlags,pvarGPMProgress,pvarGPMCancel,ppIGPMResult)
#define IGPMDomain_GetSOM(This,bstrPath,ppSOM) (This)->lpVtbl->GetSOM(This,bstrPath,ppSOM)
#define IGPMDomain_SearchSOMs(This,pIGPMSearchCriteria,ppIGPMSOMCollection) (This)->lpVtbl->SearchSOMs(This,pIGPMSearchCriteria,ppIGPMSOMCollection)
#define IGPMDomain_GetWMIFilter(This,bstrPath,ppWMIFilter) (This)->lpVtbl->GetWMIFilter(This,bstrPath,ppWMIFilter)
#define IGPMDomain_SearchWMIFilters(This,pIGPMSearchCriteria,ppIGPMWMIFilterCollection) (This)->lpVtbl->SearchWMIFilters(This,pIGPMSearchCriteria,ppIGPMWMIFilterCollection)
#endif
#endif
  HRESULT WINAPI IGPMDomain_get_DomainController_Proxy(IGPMDomain *This,BSTR *pVal);
  void __RPC_STUB IGPMDomain_get_DomainController_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMDomain_get_Domain_Proxy(IGPMDomain *This,BSTR *pVal);
  void __RPC_STUB IGPMDomain_get_Domain_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMDomain_CreateGPO_Proxy(IGPMDomain *This,IGPMGPO **ppNewGPO);
  void __RPC_STUB IGPMDomain_CreateGPO_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMDomain_GetGPO_Proxy(IGPMDomain *This,BSTR bstrGuid,IGPMGPO **ppGPO);
  void __RPC_STUB IGPMDomain_GetGPO_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMDomain_SearchGPOs_Proxy(IGPMDomain *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMGPOCollection **ppIGPMGPOCollection);
  void __RPC_STUB IGPMDomain_SearchGPOs_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMDomain_RestoreGPO_Proxy(IGPMDomain *This,IGPMBackup *pIGPMBackup,__LONG32 lDCFlags,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMDomain_RestoreGPO_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMDomain_GetSOM_Proxy(IGPMDomain *This,BSTR bstrPath,IGPMSOM **ppSOM);
  void __RPC_STUB IGPMDomain_GetSOM_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMDomain_SearchSOMs_Proxy(IGPMDomain *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMSOMCollection **ppIGPMSOMCollection);
  void __RPC_STUB IGPMDomain_SearchSOMs_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMDomain_GetWMIFilter_Proxy(IGPMDomain *This,BSTR bstrPath,IGPMWMIFilter **ppWMIFilter);
  void __RPC_STUB IGPMDomain_GetWMIFilter_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMDomain_SearchWMIFilters_Proxy(IGPMDomain *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMWMIFilterCollection **ppIGPMWMIFilterCollection);
  void __RPC_STUB IGPMDomain_SearchWMIFilters_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMBackupDir_INTERFACE_DEFINED__
#define __IGPMBackupDir_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMBackupDir;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMBackupDir : public IDispatch {
  public:
    virtual HRESULT WINAPI get_BackupDirectory(BSTR *pVal) = 0;
    virtual HRESULT WINAPI GetBackup(BSTR bstrID,IGPMBackup **ppBackup) = 0;
    virtual HRESULT WINAPI SearchBackups(IGPMSearchCriteria *pIGPMSearchCriteria,IGPMBackupCollection **ppIGPMBackupCollection) = 0;
  };
#else
  typedef struct IGPMBackupDirVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMBackupDir *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMBackupDir *This);
      ULONG (WINAPI *Release)(IGPMBackupDir *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMBackupDir *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMBackupDir *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMBackupDir *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMBackupDir *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_BackupDirectory)(IGPMBackupDir *This,BSTR *pVal);
      HRESULT (WINAPI *GetBackup)(IGPMBackupDir *This,BSTR bstrID,IGPMBackup **ppBackup);
      HRESULT (WINAPI *SearchBackups)(IGPMBackupDir *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMBackupCollection **ppIGPMBackupCollection);
    END_INTERFACE
  } IGPMBackupDirVtbl;
  struct IGPMBackupDir {
    CONST_VTBL struct IGPMBackupDirVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMBackupDir_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMBackupDir_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMBackupDir_Release(This) (This)->lpVtbl->Release(This)
#define IGPMBackupDir_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMBackupDir_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMBackupDir_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMBackupDir_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMBackupDir_get_BackupDirectory(This,pVal) (This)->lpVtbl->get_BackupDirectory(This,pVal)
#define IGPMBackupDir_GetBackup(This,bstrID,ppBackup) (This)->lpVtbl->GetBackup(This,bstrID,ppBackup)
#define IGPMBackupDir_SearchBackups(This,pIGPMSearchCriteria,ppIGPMBackupCollection) (This)->lpVtbl->SearchBackups(This,pIGPMSearchCriteria,ppIGPMBackupCollection)
#endif
#endif
  HRESULT WINAPI IGPMBackupDir_get_BackupDirectory_Proxy(IGPMBackupDir *This,BSTR *pVal);
  void __RPC_STUB IGPMBackupDir_get_BackupDirectory_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackupDir_GetBackup_Proxy(IGPMBackupDir *This,BSTR bstrID,IGPMBackup **ppBackup);
  void __RPC_STUB IGPMBackupDir_GetBackup_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackupDir_SearchBackups_Proxy(IGPMBackupDir *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMBackupCollection **ppIGPMBackupCollection);
  void __RPC_STUB IGPMBackupDir_SearchBackups_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMSitesContainer_INTERFACE_DEFINED__
#define __IGPMSitesContainer_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMSitesContainer;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMSitesContainer : public IDispatch {
  public:
    virtual HRESULT WINAPI get_DomainController(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_Domain(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_Forest(BSTR *pVal) = 0;
    virtual HRESULT WINAPI GetSite(BSTR bstrSiteName,IGPMSOM **ppSOM) = 0;
    virtual HRESULT WINAPI SearchSites(IGPMSearchCriteria *pIGPMSearchCriteria,IGPMSOMCollection **ppIGPMSOMCollection) = 0;
  };
#else
  typedef struct IGPMSitesContainerVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMSitesContainer *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMSitesContainer *This);
      ULONG (WINAPI *Release)(IGPMSitesContainer *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMSitesContainer *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMSitesContainer *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMSitesContainer *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMSitesContainer *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_DomainController)(IGPMSitesContainer *This,BSTR *pVal);
      HRESULT (WINAPI *get_Domain)(IGPMSitesContainer *This,BSTR *pVal);
      HRESULT (WINAPI *get_Forest)(IGPMSitesContainer *This,BSTR *pVal);
      HRESULT (WINAPI *GetSite)(IGPMSitesContainer *This,BSTR bstrSiteName,IGPMSOM **ppSOM);
      HRESULT (WINAPI *SearchSites)(IGPMSitesContainer *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMSOMCollection **ppIGPMSOMCollection);
    END_INTERFACE
  } IGPMSitesContainerVtbl;
  struct IGPMSitesContainer {
    CONST_VTBL struct IGPMSitesContainerVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMSitesContainer_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMSitesContainer_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMSitesContainer_Release(This) (This)->lpVtbl->Release(This)
#define IGPMSitesContainer_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMSitesContainer_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMSitesContainer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMSitesContainer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMSitesContainer_get_DomainController(This,pVal) (This)->lpVtbl->get_DomainController(This,pVal)
#define IGPMSitesContainer_get_Domain(This,pVal) (This)->lpVtbl->get_Domain(This,pVal)
#define IGPMSitesContainer_get_Forest(This,pVal) (This)->lpVtbl->get_Forest(This,pVal)
#define IGPMSitesContainer_GetSite(This,bstrSiteName,ppSOM) (This)->lpVtbl->GetSite(This,bstrSiteName,ppSOM)
#define IGPMSitesContainer_SearchSites(This,pIGPMSearchCriteria,ppIGPMSOMCollection) (This)->lpVtbl->SearchSites(This,pIGPMSearchCriteria,ppIGPMSOMCollection)
#endif
#endif
  HRESULT WINAPI IGPMSitesContainer_get_DomainController_Proxy(IGPMSitesContainer *This,BSTR *pVal);
  void __RPC_STUB IGPMSitesContainer_get_DomainController_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSitesContainer_get_Domain_Proxy(IGPMSitesContainer *This,BSTR *pVal);
  void __RPC_STUB IGPMSitesContainer_get_Domain_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSitesContainer_get_Forest_Proxy(IGPMSitesContainer *This,BSTR *pVal);
  void __RPC_STUB IGPMSitesContainer_get_Forest_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSitesContainer_GetSite_Proxy(IGPMSitesContainer *This,BSTR bstrSiteName,IGPMSOM **ppSOM);
  void __RPC_STUB IGPMSitesContainer_GetSite_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSitesContainer_SearchSites_Proxy(IGPMSitesContainer *This,IGPMSearchCriteria *pIGPMSearchCriteria,IGPMSOMCollection **ppIGPMSOMCollection);
  void __RPC_STUB IGPMSitesContainer_SearchSites_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMSearchCriteria_INTERFACE_DEFINED__
#define __IGPMSearchCriteria_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMSearchCriteria;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMSearchCriteria : public IDispatch {
  public:
    virtual HRESULT WINAPI Add(GPMSearchProperty searchProperty,GPMSearchOperation searchOperation,VARIANT varValue) = 0;
  };
#else
  typedef struct IGPMSearchCriteriaVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMSearchCriteria *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMSearchCriteria *This);
      ULONG (WINAPI *Release)(IGPMSearchCriteria *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMSearchCriteria *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMSearchCriteria *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMSearchCriteria *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMSearchCriteria *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *Add)(IGPMSearchCriteria *This,GPMSearchProperty searchProperty,GPMSearchOperation searchOperation,VARIANT varValue);
    END_INTERFACE
  } IGPMSearchCriteriaVtbl;
  struct IGPMSearchCriteria {
    CONST_VTBL struct IGPMSearchCriteriaVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMSearchCriteria_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMSearchCriteria_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMSearchCriteria_Release(This) (This)->lpVtbl->Release(This)
#define IGPMSearchCriteria_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMSearchCriteria_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMSearchCriteria_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMSearchCriteria_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMSearchCriteria_Add(This,searchProperty,searchOperation,varValue) (This)->lpVtbl->Add(This,searchProperty,searchOperation,varValue)
#endif
#endif
  HRESULT WINAPI IGPMSearchCriteria_Add_Proxy(IGPMSearchCriteria *This,GPMSearchProperty searchProperty,GPMSearchOperation searchOperation,VARIANT varValue);
  void __RPC_STUB IGPMSearchCriteria_Add_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMTrustee_INTERFACE_DEFINED__
#define __IGPMTrustee_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMTrustee;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMTrustee : public IDispatch {
  public:
    virtual HRESULT WINAPI get_TrusteeSid(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI get_TrusteeName(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI get_TrusteeDomain(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI get_TrusteeDSPath(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_TrusteeType(__LONG32 *lVal) = 0;
  };
#else
  typedef struct IGPMTrusteeVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMTrustee *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMTrustee *This);
      ULONG (WINAPI *Release)(IGPMTrustee *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMTrustee *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMTrustee *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMTrustee *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMTrustee *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_TrusteeSid)(IGPMTrustee *This,BSTR *bstrVal);
      HRESULT (WINAPI *get_TrusteeName)(IGPMTrustee *This,BSTR *bstrVal);
      HRESULT (WINAPI *get_TrusteeDomain)(IGPMTrustee *This,BSTR *bstrVal);
      HRESULT (WINAPI *get_TrusteeDSPath)(IGPMTrustee *This,BSTR *pVal);
      HRESULT (WINAPI *get_TrusteeType)(IGPMTrustee *This,__LONG32 *lVal);
    END_INTERFACE
  } IGPMTrusteeVtbl;
  struct IGPMTrustee {
    CONST_VTBL struct IGPMTrusteeVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMTrustee_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMTrustee_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMTrustee_Release(This) (This)->lpVtbl->Release(This)
#define IGPMTrustee_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMTrustee_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMTrustee_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMTrustee_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMTrustee_get_TrusteeSid(This,bstrVal) (This)->lpVtbl->get_TrusteeSid(This,bstrVal)
#define IGPMTrustee_get_TrusteeName(This,bstrVal) (This)->lpVtbl->get_TrusteeName(This,bstrVal)
#define IGPMTrustee_get_TrusteeDomain(This,bstrVal) (This)->lpVtbl->get_TrusteeDomain(This,bstrVal)
#define IGPMTrustee_get_TrusteeDSPath(This,pVal) (This)->lpVtbl->get_TrusteeDSPath(This,pVal)
#define IGPMTrustee_get_TrusteeType(This,lVal) (This)->lpVtbl->get_TrusteeType(This,lVal)
#endif
#endif
  HRESULT WINAPI IGPMTrustee_get_TrusteeSid_Proxy(IGPMTrustee *This,BSTR *bstrVal);
  void __RPC_STUB IGPMTrustee_get_TrusteeSid_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMTrustee_get_TrusteeName_Proxy(IGPMTrustee *This,BSTR *bstrVal);
  void __RPC_STUB IGPMTrustee_get_TrusteeName_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMTrustee_get_TrusteeDomain_Proxy(IGPMTrustee *This,BSTR *bstrVal);
  void __RPC_STUB IGPMTrustee_get_TrusteeDomain_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMTrustee_get_TrusteeDSPath_Proxy(IGPMTrustee *This,BSTR *pVal);
  void __RPC_STUB IGPMTrustee_get_TrusteeDSPath_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMTrustee_get_TrusteeType_Proxy(IGPMTrustee *This,__LONG32 *lVal);
  void __RPC_STUB IGPMTrustee_get_TrusteeType_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMPermission_INTERFACE_DEFINED__
#define __IGPMPermission_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMPermission;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMPermission : public IDispatch {
  public:
    virtual HRESULT WINAPI get_Inherited(VARIANT_BOOL *pVal) = 0;
    virtual HRESULT WINAPI get_Inheritable(VARIANT_BOOL *pVal) = 0;
    virtual HRESULT WINAPI get_Denied(VARIANT_BOOL *pVal) = 0;
    virtual HRESULT WINAPI get_Permission(GPMPermissionType *pVal) = 0;
    virtual HRESULT WINAPI get_Trustee(IGPMTrustee **ppIGPMTrustee) = 0;
  };
#else
  typedef struct IGPMPermissionVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMPermission *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMPermission *This);
      ULONG (WINAPI *Release)(IGPMPermission *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMPermission *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMPermission *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMPermission *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMPermission *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_Inherited)(IGPMPermission *This,VARIANT_BOOL *pVal);
      HRESULT (WINAPI *get_Inheritable)(IGPMPermission *This,VARIANT_BOOL *pVal);
      HRESULT (WINAPI *get_Denied)(IGPMPermission *This,VARIANT_BOOL *pVal);
      HRESULT (WINAPI *get_Permission)(IGPMPermission *This,GPMPermissionType *pVal);
      HRESULT (WINAPI *get_Trustee)(IGPMPermission *This,IGPMTrustee **ppIGPMTrustee);
    END_INTERFACE
  } IGPMPermissionVtbl;
  struct IGPMPermission {
    CONST_VTBL struct IGPMPermissionVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMPermission_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMPermission_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMPermission_Release(This) (This)->lpVtbl->Release(This)
#define IGPMPermission_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMPermission_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMPermission_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMPermission_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMPermission_get_Inherited(This,pVal) (This)->lpVtbl->get_Inherited(This,pVal)
#define IGPMPermission_get_Inheritable(This,pVal) (This)->lpVtbl->get_Inheritable(This,pVal)
#define IGPMPermission_get_Denied(This,pVal) (This)->lpVtbl->get_Denied(This,pVal)
#define IGPMPermission_get_Permission(This,pVal) (This)->lpVtbl->get_Permission(This,pVal)
#define IGPMPermission_get_Trustee(This,ppIGPMTrustee) (This)->lpVtbl->get_Trustee(This,ppIGPMTrustee)
#endif
#endif
  HRESULT WINAPI IGPMPermission_get_Inherited_Proxy(IGPMPermission *This,VARIANT_BOOL *pVal);
  void __RPC_STUB IGPMPermission_get_Inherited_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMPermission_get_Inheritable_Proxy(IGPMPermission *This,VARIANT_BOOL *pVal);
  void __RPC_STUB IGPMPermission_get_Inheritable_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMPermission_get_Denied_Proxy(IGPMPermission *This,VARIANT_BOOL *pVal);
  void __RPC_STUB IGPMPermission_get_Denied_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMPermission_get_Permission_Proxy(IGPMPermission *This,GPMPermissionType *pVal);
  void __RPC_STUB IGPMPermission_get_Permission_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMPermission_get_Trustee_Proxy(IGPMPermission *This,IGPMTrustee **ppIGPMTrustee);
  void __RPC_STUB IGPMPermission_get_Trustee_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMSecurityInfo_INTERFACE_DEFINED__
#define __IGPMSecurityInfo_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMSecurityInfo;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMSecurityInfo : public IDispatch {
  public:
    virtual HRESULT WINAPI get_Count(__LONG32 *pVal) = 0;
    virtual HRESULT WINAPI get_Item(__LONG32 lIndex,VARIANT *pVal) = 0;
    virtual HRESULT WINAPI get__NewEnum(IEnumVARIANT **ppEnum) = 0;
    virtual HRESULT WINAPI Add(IGPMPermission *pPerm) = 0;
    virtual HRESULT WINAPI Remove(IGPMPermission *pPerm) = 0;
    virtual HRESULT WINAPI RemoveTrustee(BSTR bstrTrustee) = 0;
  };
#else
  typedef struct IGPMSecurityInfoVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMSecurityInfo *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMSecurityInfo *This);
      ULONG (WINAPI *Release)(IGPMSecurityInfo *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMSecurityInfo *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMSecurityInfo *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMSecurityInfo *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMSecurityInfo *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_Count)(IGPMSecurityInfo *This,__LONG32 *pVal);
      HRESULT (WINAPI *get_Item)(IGPMSecurityInfo *This,__LONG32 lIndex,VARIANT *pVal);
      HRESULT (WINAPI *get__NewEnum)(IGPMSecurityInfo *This,IEnumVARIANT **ppEnum);
      HRESULT (WINAPI *Add)(IGPMSecurityInfo *This,IGPMPermission *pPerm);
      HRESULT (WINAPI *Remove)(IGPMSecurityInfo *This,IGPMPermission *pPerm);
      HRESULT (WINAPI *RemoveTrustee)(IGPMSecurityInfo *This,BSTR bstrTrustee);
    END_INTERFACE
  } IGPMSecurityInfoVtbl;
  struct IGPMSecurityInfo {
    CONST_VTBL struct IGPMSecurityInfoVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMSecurityInfo_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMSecurityInfo_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMSecurityInfo_Release(This) (This)->lpVtbl->Release(This)
#define IGPMSecurityInfo_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMSecurityInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMSecurityInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMSecurityInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMSecurityInfo_get_Count(This,pVal) (This)->lpVtbl->get_Count(This,pVal)
#define IGPMSecurityInfo_get_Item(This,lIndex,pVal) (This)->lpVtbl->get_Item(This,lIndex,pVal)
#define IGPMSecurityInfo_get__NewEnum(This,ppEnum) (This)->lpVtbl->get__NewEnum(This,ppEnum)
#define IGPMSecurityInfo_Add(This,pPerm) (This)->lpVtbl->Add(This,pPerm)
#define IGPMSecurityInfo_Remove(This,pPerm) (This)->lpVtbl->Remove(This,pPerm)
#define IGPMSecurityInfo_RemoveTrustee(This,bstrTrustee) (This)->lpVtbl->RemoveTrustee(This,bstrTrustee)
#endif
#endif
  HRESULT WINAPI IGPMSecurityInfo_get_Count_Proxy(IGPMSecurityInfo *This,__LONG32 *pVal);
  void __RPC_STUB IGPMSecurityInfo_get_Count_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSecurityInfo_get_Item_Proxy(IGPMSecurityInfo *This,__LONG32 lIndex,VARIANT *pVal);
  void __RPC_STUB IGPMSecurityInfo_get_Item_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSecurityInfo_get__NewEnum_Proxy(IGPMSecurityInfo *This,IEnumVARIANT **ppEnum);
  void __RPC_STUB IGPMSecurityInfo_get__NewEnum_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSecurityInfo_Add_Proxy(IGPMSecurityInfo *This,IGPMPermission *pPerm);
  void __RPC_STUB IGPMSecurityInfo_Add_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSecurityInfo_Remove_Proxy(IGPMSecurityInfo *This,IGPMPermission *pPerm);
  void __RPC_STUB IGPMSecurityInfo_Remove_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSecurityInfo_RemoveTrustee_Proxy(IGPMSecurityInfo *This,BSTR bstrTrustee);
  void __RPC_STUB IGPMSecurityInfo_RemoveTrustee_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMBackup_INTERFACE_DEFINED__
#define __IGPMBackup_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMBackup;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMBackup : public IDispatch {
  public:
    virtual HRESULT WINAPI get_ID(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_GPOID(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_GPODomain(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_GPODisplayName(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_Timestamp(DATE *pVal) = 0;
    virtual HRESULT WINAPI get_Comment(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_BackupDir(BSTR *pVal) = 0;
    virtual HRESULT WINAPI Delete(void) = 0;
    virtual HRESULT WINAPI GenerateReport(GPMReportType gpmReportType,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult) = 0;
    virtual HRESULT WINAPI GenerateReportToFile(GPMReportType gpmReportType,BSTR bstrTargetFilePath,IGPMResult **ppIGPMResult) = 0;
  };
#else
  typedef struct IGPMBackupVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMBackup *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMBackup *This);
      ULONG (WINAPI *Release)(IGPMBackup *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMBackup *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMBackup *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMBackup *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMBackup *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_ID)(IGPMBackup *This,BSTR *pVal);
      HRESULT (WINAPI *get_GPOID)(IGPMBackup *This,BSTR *pVal);
      HRESULT (WINAPI *get_GPODomain)(IGPMBackup *This,BSTR *pVal);
      HRESULT (WINAPI *get_GPODisplayName)(IGPMBackup *This,BSTR *pVal);
      HRESULT (WINAPI *get_Timestamp)(IGPMBackup *This,DATE *pVal);
      HRESULT (WINAPI *get_Comment)(IGPMBackup *This,BSTR *pVal);
      HRESULT (WINAPI *get_BackupDir)(IGPMBackup *This,BSTR *pVal);
      HRESULT (WINAPI *Delete)(IGPMBackup *This);
      HRESULT (WINAPI *GenerateReport)(IGPMBackup *This,GPMReportType gpmReportType,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
      HRESULT (WINAPI *GenerateReportToFile)(IGPMBackup *This,GPMReportType gpmReportType,BSTR bstrTargetFilePath,IGPMResult **ppIGPMResult);
    END_INTERFACE
  } IGPMBackupVtbl;
  struct IGPMBackup {
    CONST_VTBL struct IGPMBackupVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMBackup_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMBackup_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMBackup_Release(This) (This)->lpVtbl->Release(This)
#define IGPMBackup_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMBackup_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMBackup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMBackup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMBackup_get_ID(This,pVal) (This)->lpVtbl->get_ID(This,pVal)
#define IGPMBackup_get_GPOID(This,pVal) (This)->lpVtbl->get_GPOID(This,pVal)
#define IGPMBackup_get_GPODomain(This,pVal) (This)->lpVtbl->get_GPODomain(This,pVal)
#define IGPMBackup_get_GPODisplayName(This,pVal) (This)->lpVtbl->get_GPODisplayName(This,pVal)
#define IGPMBackup_get_Timestamp(This,pVal) (This)->lpVtbl->get_Timestamp(This,pVal)
#define IGPMBackup_get_Comment(This,pVal) (This)->lpVtbl->get_Comment(This,pVal)
#define IGPMBackup_get_BackupDir(This,pVal) (This)->lpVtbl->get_BackupDir(This,pVal)
#define IGPMBackup_Delete(This) (This)->lpVtbl->Delete(This)
#define IGPMBackup_GenerateReport(This,gpmReportType,pvarGPMProgress,pvarGPMCancel,ppIGPMResult) (This)->lpVtbl->GenerateReport(This,gpmReportType,pvarGPMProgress,pvarGPMCancel,ppIGPMResult)
#define IGPMBackup_GenerateReportToFile(This,gpmReportType,bstrTargetFilePath,ppIGPMResult) (This)->lpVtbl->GenerateReportToFile(This,gpmReportType,bstrTargetFilePath,ppIGPMResult)
#endif
#endif
  HRESULT WINAPI IGPMBackup_get_ID_Proxy(IGPMBackup *This,BSTR *pVal);
  void __RPC_STUB IGPMBackup_get_ID_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackup_get_GPOID_Proxy(IGPMBackup *This,BSTR *pVal);
  void __RPC_STUB IGPMBackup_get_GPOID_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackup_get_GPODomain_Proxy(IGPMBackup *This,BSTR *pVal);
  void __RPC_STUB IGPMBackup_get_GPODomain_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackup_get_GPODisplayName_Proxy(IGPMBackup *This,BSTR *pVal);
  void __RPC_STUB IGPMBackup_get_GPODisplayName_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackup_get_Timestamp_Proxy(IGPMBackup *This,DATE *pVal);
  void __RPC_STUB IGPMBackup_get_Timestamp_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackup_get_Comment_Proxy(IGPMBackup *This,BSTR *pVal);
  void __RPC_STUB IGPMBackup_get_Comment_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackup_get_BackupDir_Proxy(IGPMBackup *This,BSTR *pVal);
  void __RPC_STUB IGPMBackup_get_BackupDir_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackup_Delete_Proxy(IGPMBackup *This);
  void __RPC_STUB IGPMBackup_Delete_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackup_GenerateReport_Proxy(IGPMBackup *This,GPMReportType gpmReportType,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMBackup_GenerateReport_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackup_GenerateReportToFile_Proxy(IGPMBackup *This,GPMReportType gpmReportType,BSTR bstrTargetFilePath,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMBackup_GenerateReportToFile_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMBackupCollection_INTERFACE_DEFINED__
#define __IGPMBackupCollection_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMBackupCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMBackupCollection : public IDispatch {
  public:
    virtual HRESULT WINAPI get_Count(__LONG32 *pVal) = 0;
    virtual HRESULT WINAPI get_Item(__LONG32 lIndex,VARIANT *pVal) = 0;
    virtual HRESULT WINAPI get__NewEnum(IEnumVARIANT **ppIGPMBackup) = 0;
  };
#else
  typedef struct IGPMBackupCollectionVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMBackupCollection *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMBackupCollection *This);
      ULONG (WINAPI *Release)(IGPMBackupCollection *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMBackupCollection *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMBackupCollection *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMBackupCollection *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMBackupCollection *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_Count)(IGPMBackupCollection *This,__LONG32 *pVal);
      HRESULT (WINAPI *get_Item)(IGPMBackupCollection *This,__LONG32 lIndex,VARIANT *pVal);
      HRESULT (WINAPI *get__NewEnum)(IGPMBackupCollection *This,IEnumVARIANT **ppIGPMBackup);
    END_INTERFACE
  } IGPMBackupCollectionVtbl;
  struct IGPMBackupCollection {
    CONST_VTBL struct IGPMBackupCollectionVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMBackupCollection_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMBackupCollection_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMBackupCollection_Release(This) (This)->lpVtbl->Release(This)
#define IGPMBackupCollection_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMBackupCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMBackupCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMBackupCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMBackupCollection_get_Count(This,pVal) (This)->lpVtbl->get_Count(This,pVal)
#define IGPMBackupCollection_get_Item(This,lIndex,pVal) (This)->lpVtbl->get_Item(This,lIndex,pVal)
#define IGPMBackupCollection_get__NewEnum(This,ppIGPMBackup) (This)->lpVtbl->get__NewEnum(This,ppIGPMBackup)
#endif
#endif
  HRESULT WINAPI IGPMBackupCollection_get_Count_Proxy(IGPMBackupCollection *This,__LONG32 *pVal);
  void __RPC_STUB IGPMBackupCollection_get_Count_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackupCollection_get_Item_Proxy(IGPMBackupCollection *This,__LONG32 lIndex,VARIANT *pVal);
  void __RPC_STUB IGPMBackupCollection_get_Item_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMBackupCollection_get__NewEnum_Proxy(IGPMBackupCollection *This,IEnumVARIANT **ppIGPMBackup);
  void __RPC_STUB IGPMBackupCollection_get__NewEnum_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMSOM_INTERFACE_DEFINED__
#define __IGPMSOM_INTERFACE_DEFINED__
  typedef enum __MIDL_IGPMSOM_0001 {
    somSite = 0,somDomain,somOU
  } GPMSOMType;

  EXTERN_C const IID IID_IGPMSOM;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMSOM : public IDispatch {
  public:
    virtual HRESULT WINAPI get_GPOInheritanceBlocked(VARIANT_BOOL *pVal) = 0;
    virtual HRESULT WINAPI put_GPOInheritanceBlocked(VARIANT_BOOL newVal) = 0;
    virtual HRESULT WINAPI get_Name(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_Path(BSTR *pVal) = 0;
    virtual HRESULT WINAPI CreateGPOLink(__LONG32 lLinkPos,IGPMGPO *pGPO,IGPMGPOLink **ppNewGPOLink) = 0;
    virtual HRESULT WINAPI get_Type(GPMSOMType *pVal) = 0;
    virtual HRESULT WINAPI GetGPOLinks(IGPMGPOLinksCollection **ppGPOLinks) = 0;
    virtual HRESULT WINAPI GetInheritedGPOLinks(IGPMGPOLinksCollection **ppGPOLinks) = 0;
    virtual HRESULT WINAPI GetSecurityInfo(IGPMSecurityInfo **ppSecurityInfo) = 0;
    virtual HRESULT WINAPI SetSecurityInfo(IGPMSecurityInfo *pSecurityInfo) = 0;
  };
#else
  typedef struct IGPMSOMVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMSOM *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMSOM *This);
      ULONG (WINAPI *Release)(IGPMSOM *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMSOM *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMSOM *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMSOM *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMSOM *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_GPOInheritanceBlocked)(IGPMSOM *This,VARIANT_BOOL *pVal);
      HRESULT (WINAPI *put_GPOInheritanceBlocked)(IGPMSOM *This,VARIANT_BOOL newVal);
      HRESULT (WINAPI *get_Name)(IGPMSOM *This,BSTR *pVal);
      HRESULT (WINAPI *get_Path)(IGPMSOM *This,BSTR *pVal);
      HRESULT (WINAPI *CreateGPOLink)(IGPMSOM *This,__LONG32 lLinkPos,IGPMGPO *pGPO,IGPMGPOLink **ppNewGPOLink);
      HRESULT (WINAPI *get_Type)(IGPMSOM *This,GPMSOMType *pVal);
      HRESULT (WINAPI *GetGPOLinks)(IGPMSOM *This,IGPMGPOLinksCollection **ppGPOLinks);
      HRESULT (WINAPI *GetInheritedGPOLinks)(IGPMSOM *This,IGPMGPOLinksCollection **ppGPOLinks);
      HRESULT (WINAPI *GetSecurityInfo)(IGPMSOM *This,IGPMSecurityInfo **ppSecurityInfo);
      HRESULT (WINAPI *SetSecurityInfo)(IGPMSOM *This,IGPMSecurityInfo *pSecurityInfo);
    END_INTERFACE
  } IGPMSOMVtbl;
  struct IGPMSOM {
    CONST_VTBL struct IGPMSOMVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMSOM_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMSOM_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMSOM_Release(This) (This)->lpVtbl->Release(This)
#define IGPMSOM_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMSOM_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMSOM_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMSOM_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMSOM_get_GPOInheritanceBlocked(This,pVal) (This)->lpVtbl->get_GPOInheritanceBlocked(This,pVal)
#define IGPMSOM_put_GPOInheritanceBlocked(This,newVal) (This)->lpVtbl->put_GPOInheritanceBlocked(This,newVal)
#define IGPMSOM_get_Name(This,pVal) (This)->lpVtbl->get_Name(This,pVal)
#define IGPMSOM_get_Path(This,pVal) (This)->lpVtbl->get_Path(This,pVal)
#define IGPMSOM_CreateGPOLink(This,lLinkPos,pGPO,ppNewGPOLink) (This)->lpVtbl->CreateGPOLink(This,lLinkPos,pGPO,ppNewGPOLink)
#define IGPMSOM_get_Type(This,pVal) (This)->lpVtbl->get_Type(This,pVal)
#define IGPMSOM_GetGPOLinks(This,ppGPOLinks) (This)->lpVtbl->GetGPOLinks(This,ppGPOLinks)
#define IGPMSOM_GetInheritedGPOLinks(This,ppGPOLinks) (This)->lpVtbl->GetInheritedGPOLinks(This,ppGPOLinks)
#define IGPMSOM_GetSecurityInfo(This,ppSecurityInfo) (This)->lpVtbl->GetSecurityInfo(This,ppSecurityInfo)
#define IGPMSOM_SetSecurityInfo(This,pSecurityInfo) (This)->lpVtbl->SetSecurityInfo(This,pSecurityInfo)
#endif
#endif
  HRESULT WINAPI IGPMSOM_get_GPOInheritanceBlocked_Proxy(IGPMSOM *This,VARIANT_BOOL *pVal);
  void __RPC_STUB IGPMSOM_get_GPOInheritanceBlocked_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOM_put_GPOInheritanceBlocked_Proxy(IGPMSOM *This,VARIANT_BOOL newVal);
  void __RPC_STUB IGPMSOM_put_GPOInheritanceBlocked_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOM_get_Name_Proxy(IGPMSOM *This,BSTR *pVal);
  void __RPC_STUB IGPMSOM_get_Name_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOM_get_Path_Proxy(IGPMSOM *This,BSTR *pVal);
  void __RPC_STUB IGPMSOM_get_Path_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOM_CreateGPOLink_Proxy(IGPMSOM *This,__LONG32 lLinkPos,IGPMGPO *pGPO,IGPMGPOLink **ppNewGPOLink);
  void __RPC_STUB IGPMSOM_CreateGPOLink_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOM_get_Type_Proxy(IGPMSOM *This,GPMSOMType *pVal);
  void __RPC_STUB IGPMSOM_get_Type_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOM_GetGPOLinks_Proxy(IGPMSOM *This,IGPMGPOLinksCollection **ppGPOLinks);
  void __RPC_STUB IGPMSOM_GetGPOLinks_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOM_GetInheritedGPOLinks_Proxy(IGPMSOM *This,IGPMGPOLinksCollection **ppGPOLinks);
  void __RPC_STUB IGPMSOM_GetInheritedGPOLinks_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOM_GetSecurityInfo_Proxy(IGPMSOM *This,IGPMSecurityInfo **ppSecurityInfo);
  void __RPC_STUB IGPMSOM_GetSecurityInfo_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOM_SetSecurityInfo_Proxy(IGPMSOM *This,IGPMSecurityInfo *pSecurityInfo);
  void __RPC_STUB IGPMSOM_SetSecurityInfo_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMSOMCollection_INTERFACE_DEFINED__
#define __IGPMSOMCollection_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMSOMCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMSOMCollection : public IDispatch {
  public:
    virtual HRESULT WINAPI get_Count(__LONG32 *pVal) = 0;
    virtual HRESULT WINAPI get_Item(__LONG32 lIndex,VARIANT *pVal) = 0;
    virtual HRESULT WINAPI get__NewEnum(IEnumVARIANT **ppIGPMSOM) = 0;
  };
#else
  typedef struct IGPMSOMCollectionVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMSOMCollection *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMSOMCollection *This);
      ULONG (WINAPI *Release)(IGPMSOMCollection *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMSOMCollection *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMSOMCollection *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMSOMCollection *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMSOMCollection *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_Count)(IGPMSOMCollection *This,__LONG32 *pVal);
      HRESULT (WINAPI *get_Item)(IGPMSOMCollection *This,__LONG32 lIndex,VARIANT *pVal);
      HRESULT (WINAPI *get__NewEnum)(IGPMSOMCollection *This,IEnumVARIANT **ppIGPMSOM);
    END_INTERFACE
  } IGPMSOMCollectionVtbl;
  struct IGPMSOMCollection {
    CONST_VTBL struct IGPMSOMCollectionVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMSOMCollection_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMSOMCollection_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMSOMCollection_Release(This) (This)->lpVtbl->Release(This)
#define IGPMSOMCollection_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMSOMCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMSOMCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMSOMCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMSOMCollection_get_Count(This,pVal) (This)->lpVtbl->get_Count(This,pVal)
#define IGPMSOMCollection_get_Item(This,lIndex,pVal) (This)->lpVtbl->get_Item(This,lIndex,pVal)
#define IGPMSOMCollection_get__NewEnum(This,ppIGPMSOM) (This)->lpVtbl->get__NewEnum(This,ppIGPMSOM)
#endif
#endif
  HRESULT WINAPI IGPMSOMCollection_get_Count_Proxy(IGPMSOMCollection *This,__LONG32 *pVal);
  void __RPC_STUB IGPMSOMCollection_get_Count_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOMCollection_get_Item_Proxy(IGPMSOMCollection *This,__LONG32 lIndex,VARIANT *pVal);
  void __RPC_STUB IGPMSOMCollection_get_Item_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMSOMCollection_get__NewEnum_Proxy(IGPMSOMCollection *This,IEnumVARIANT **ppIGPMSOM);
  void __RPC_STUB IGPMSOMCollection_get__NewEnum_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMWMIFilter_INTERFACE_DEFINED__
#define __IGPMWMIFilter_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMWMIFilter;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMWMIFilter : public IDispatch {
  public:
    virtual HRESULT WINAPI get_Path(BSTR *pVal) = 0;
    virtual HRESULT WINAPI put_Name(BSTR newVal) = 0;
    virtual HRESULT WINAPI get_Name(BSTR *pVal) = 0;
    virtual HRESULT WINAPI put_Description(BSTR newVal) = 0;
    virtual HRESULT WINAPI get_Description(BSTR *pVal) = 0;
    virtual HRESULT WINAPI GetQueryList(VARIANT *pQryList) = 0;
    virtual HRESULT WINAPI GetSecurityInfo(IGPMSecurityInfo **ppSecurityInfo) = 0;
    virtual HRESULT WINAPI SetSecurityInfo(IGPMSecurityInfo *pSecurityInfo) = 0;
  };
#else
  typedef struct IGPMWMIFilterVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMWMIFilter *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMWMIFilter *This);
      ULONG (WINAPI *Release)(IGPMWMIFilter *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMWMIFilter *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMWMIFilter *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMWMIFilter *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMWMIFilter *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_Path)(IGPMWMIFilter *This,BSTR *pVal);
      HRESULT (WINAPI *put_Name)(IGPMWMIFilter *This,BSTR newVal);
      HRESULT (WINAPI *get_Name)(IGPMWMIFilter *This,BSTR *pVal);
      HRESULT (WINAPI *put_Description)(IGPMWMIFilter *This,BSTR newVal);
      HRESULT (WINAPI *get_Description)(IGPMWMIFilter *This,BSTR *pVal);
      HRESULT (WINAPI *GetQueryList)(IGPMWMIFilter *This,VARIANT *pQryList);
      HRESULT (WINAPI *GetSecurityInfo)(IGPMWMIFilter *This,IGPMSecurityInfo **ppSecurityInfo);
      HRESULT (WINAPI *SetSecurityInfo)(IGPMWMIFilter *This,IGPMSecurityInfo *pSecurityInfo);
    END_INTERFACE
  } IGPMWMIFilterVtbl;
  struct IGPMWMIFilter {
    CONST_VTBL struct IGPMWMIFilterVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMWMIFilter_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMWMIFilter_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMWMIFilter_Release(This) (This)->lpVtbl->Release(This)
#define IGPMWMIFilter_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMWMIFilter_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMWMIFilter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMWMIFilter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMWMIFilter_get_Path(This,pVal) (This)->lpVtbl->get_Path(This,pVal)
#define IGPMWMIFilter_put_Name(This,newVal) (This)->lpVtbl->put_Name(This,newVal)
#define IGPMWMIFilter_get_Name(This,pVal) (This)->lpVtbl->get_Name(This,pVal)
#define IGPMWMIFilter_put_Description(This,newVal) (This)->lpVtbl->put_Description(This,newVal)
#define IGPMWMIFilter_get_Description(This,pVal) (This)->lpVtbl->get_Description(This,pVal)
#define IGPMWMIFilter_GetQueryList(This,pQryList) (This)->lpVtbl->GetQueryList(This,pQryList)
#define IGPMWMIFilter_GetSecurityInfo(This,ppSecurityInfo) (This)->lpVtbl->GetSecurityInfo(This,ppSecurityInfo)
#define IGPMWMIFilter_SetSecurityInfo(This,pSecurityInfo) (This)->lpVtbl->SetSecurityInfo(This,pSecurityInfo)
#endif
#endif
  HRESULT WINAPI IGPMWMIFilter_get_Path_Proxy(IGPMWMIFilter *This,BSTR *pVal);
  void __RPC_STUB IGPMWMIFilter_get_Path_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMWMIFilter_put_Name_Proxy(IGPMWMIFilter *This,BSTR newVal);
  void __RPC_STUB IGPMWMIFilter_put_Name_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMWMIFilter_get_Name_Proxy(IGPMWMIFilter *This,BSTR *pVal);
  void __RPC_STUB IGPMWMIFilter_get_Name_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMWMIFilter_put_Description_Proxy(IGPMWMIFilter *This,BSTR newVal);
  void __RPC_STUB IGPMWMIFilter_put_Description_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMWMIFilter_get_Description_Proxy(IGPMWMIFilter *This,BSTR *pVal);
  void __RPC_STUB IGPMWMIFilter_get_Description_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMWMIFilter_GetQueryList_Proxy(IGPMWMIFilter *This,VARIANT *pQryList);
  void __RPC_STUB IGPMWMIFilter_GetQueryList_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMWMIFilter_GetSecurityInfo_Proxy(IGPMWMIFilter *This,IGPMSecurityInfo **ppSecurityInfo);
  void __RPC_STUB IGPMWMIFilter_GetSecurityInfo_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMWMIFilter_SetSecurityInfo_Proxy(IGPMWMIFilter *This,IGPMSecurityInfo *pSecurityInfo);
  void __RPC_STUB IGPMWMIFilter_SetSecurityInfo_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMWMIFilterCollection_INTERFACE_DEFINED__
#define __IGPMWMIFilterCollection_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMWMIFilterCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMWMIFilterCollection : public IDispatch {
  public:
    virtual HRESULT WINAPI get_Count(__LONG32 *pVal) = 0;
    virtual HRESULT WINAPI get_Item(__LONG32 lIndex,VARIANT *pVal) = 0;
    virtual HRESULT WINAPI get__NewEnum(IEnumVARIANT **pVal) = 0;
  };
#else
  typedef struct IGPMWMIFilterCollectionVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMWMIFilterCollection *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMWMIFilterCollection *This);
      ULONG (WINAPI *Release)(IGPMWMIFilterCollection *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMWMIFilterCollection *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMWMIFilterCollection *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMWMIFilterCollection *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMWMIFilterCollection *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_Count)(IGPMWMIFilterCollection *This,__LONG32 *pVal);
      HRESULT (WINAPI *get_Item)(IGPMWMIFilterCollection *This,__LONG32 lIndex,VARIANT *pVal);
      HRESULT (WINAPI *get__NewEnum)(IGPMWMIFilterCollection *This,IEnumVARIANT **pVal);
    END_INTERFACE
  } IGPMWMIFilterCollectionVtbl;
  struct IGPMWMIFilterCollection {
    CONST_VTBL struct IGPMWMIFilterCollectionVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMWMIFilterCollection_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMWMIFilterCollection_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMWMIFilterCollection_Release(This) (This)->lpVtbl->Release(This)
#define IGPMWMIFilterCollection_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMWMIFilterCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMWMIFilterCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMWMIFilterCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMWMIFilterCollection_get_Count(This,pVal) (This)->lpVtbl->get_Count(This,pVal)
#define IGPMWMIFilterCollection_get_Item(This,lIndex,pVal) (This)->lpVtbl->get_Item(This,lIndex,pVal)
#define IGPMWMIFilterCollection_get__NewEnum(This,pVal) (This)->lpVtbl->get__NewEnum(This,pVal)
#endif
#endif
  HRESULT WINAPI IGPMWMIFilterCollection_get_Count_Proxy(IGPMWMIFilterCollection *This,__LONG32 *pVal);
  void __RPC_STUB IGPMWMIFilterCollection_get_Count_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMWMIFilterCollection_get_Item_Proxy(IGPMWMIFilterCollection *This,__LONG32 lIndex,VARIANT *pVal);
  void __RPC_STUB IGPMWMIFilterCollection_get_Item_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMWMIFilterCollection_get__NewEnum_Proxy(IGPMWMIFilterCollection *This,IEnumVARIANT **pVal);
  void __RPC_STUB IGPMWMIFilterCollection_get__NewEnum_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMRSOP_INTERFACE_DEFINED__
#define __IGPMRSOP_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMRSOP;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMRSOP : public IDispatch {
  public:
    virtual HRESULT WINAPI get_Mode(GPMRSOPMode *pVal) = 0;
    virtual HRESULT WINAPI get_Namespace(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI put_LoggingComputer(BSTR bstrVal) = 0;
    virtual HRESULT WINAPI get_LoggingComputer(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI put_LoggingUser(BSTR bstrVal) = 0;
    virtual HRESULT WINAPI get_LoggingUser(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI put_LoggingFlags(__LONG32 lVal) = 0;
    virtual HRESULT WINAPI get_LoggingFlags(__LONG32 *lVal) = 0;
    virtual HRESULT WINAPI put_PlanningFlags(__LONG32 lVal) = 0;
    virtual HRESULT WINAPI get_PlanningFlags(__LONG32 *lVal) = 0;
    virtual HRESULT WINAPI put_PlanningDomainController(BSTR bstrVal) = 0;
    virtual HRESULT WINAPI get_PlanningDomainController(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI put_PlanningSiteName(BSTR bstrVal) = 0;
    virtual HRESULT WINAPI get_PlanningSiteName(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI put_PlanningUser(BSTR bstrVal) = 0;
    virtual HRESULT WINAPI get_PlanningUser(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI put_PlanningUserSOM(BSTR bstrVal) = 0;
    virtual HRESULT WINAPI get_PlanningUserSOM(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI put_PlanningUserWMIFilters(VARIANT varVal) = 0;
    virtual HRESULT WINAPI get_PlanningUserWMIFilters(VARIANT *varVal) = 0;
    virtual HRESULT WINAPI put_PlanningUserSecurityGroups(VARIANT varVal) = 0;
    virtual HRESULT WINAPI get_PlanningUserSecurityGroups(VARIANT *varVal) = 0;
    virtual HRESULT WINAPI put_PlanningComputer(BSTR bstrVal) = 0;
    virtual HRESULT WINAPI get_PlanningComputer(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI put_PlanningComputerSOM(BSTR bstrVal) = 0;
    virtual HRESULT WINAPI get_PlanningComputerSOM(BSTR *bstrVal) = 0;
    virtual HRESULT WINAPI put_PlanningComputerWMIFilters(VARIANT varVal) = 0;
    virtual HRESULT WINAPI get_PlanningComputerWMIFilters(VARIANT *varVal) = 0;
    virtual HRESULT WINAPI put_PlanningComputerSecurityGroups(VARIANT varVal) = 0;
    virtual HRESULT WINAPI get_PlanningComputerSecurityGroups(VARIANT *varVal) = 0;
    virtual HRESULT WINAPI LoggingEnumerateUsers(VARIANT *varVal) = 0;
    virtual HRESULT WINAPI CreateQueryResults(void) = 0;
    virtual HRESULT WINAPI ReleaseQueryResults(void) = 0;
    virtual HRESULT WINAPI GenerateReport(GPMReportType gpmReportType,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult) = 0;
    virtual HRESULT WINAPI GenerateReportToFile(GPMReportType gpmReportType,BSTR bstrTargetFilePath,IGPMResult **ppIGPMResult) = 0;
  };
#else
  typedef struct IGPMRSOPVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMRSOP *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMRSOP *This);
      ULONG (WINAPI *Release)(IGPMRSOP *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMRSOP *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMRSOP *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMRSOP *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMRSOP *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_Mode)(IGPMRSOP *This,GPMRSOPMode *pVal);
      HRESULT (WINAPI *get_Namespace)(IGPMRSOP *This,BSTR *bstrVal);
      HRESULT (WINAPI *put_LoggingComputer)(IGPMRSOP *This,BSTR bstrVal);
      HRESULT (WINAPI *get_LoggingComputer)(IGPMRSOP *This,BSTR *bstrVal);
      HRESULT (WINAPI *put_LoggingUser)(IGPMRSOP *This,BSTR bstrVal);
      HRESULT (WINAPI *get_LoggingUser)(IGPMRSOP *This,BSTR *bstrVal);
      HRESULT (WINAPI *put_LoggingFlags)(IGPMRSOP *This,__LONG32 lVal);
      HRESULT (WINAPI *get_LoggingFlags)(IGPMRSOP *This,__LONG32 *lVal);
      HRESULT (WINAPI *put_PlanningFlags)(IGPMRSOP *This,__LONG32 lVal);
      HRESULT (WINAPI *get_PlanningFlags)(IGPMRSOP *This,__LONG32 *lVal);
      HRESULT (WINAPI *put_PlanningDomainController)(IGPMRSOP *This,BSTR bstrVal);
      HRESULT (WINAPI *get_PlanningDomainController)(IGPMRSOP *This,BSTR *bstrVal);
      HRESULT (WINAPI *put_PlanningSiteName)(IGPMRSOP *This,BSTR bstrVal);
      HRESULT (WINAPI *get_PlanningSiteName)(IGPMRSOP *This,BSTR *bstrVal);
      HRESULT (WINAPI *put_PlanningUser)(IGPMRSOP *This,BSTR bstrVal);
      HRESULT (WINAPI *get_PlanningUser)(IGPMRSOP *This,BSTR *bstrVal);
      HRESULT (WINAPI *put_PlanningUserSOM)(IGPMRSOP *This,BSTR bstrVal);
      HRESULT (WINAPI *get_PlanningUserSOM)(IGPMRSOP *This,BSTR *bstrVal);
      HRESULT (WINAPI *put_PlanningUserWMIFilters)(IGPMRSOP *This,VARIANT varVal);
      HRESULT (WINAPI *get_PlanningUserWMIFilters)(IGPMRSOP *This,VARIANT *varVal);
      HRESULT (WINAPI *put_PlanningUserSecurityGroups)(IGPMRSOP *This,VARIANT varVal);
      HRESULT (WINAPI *get_PlanningUserSecurityGroups)(IGPMRSOP *This,VARIANT *varVal);
      HRESULT (WINAPI *put_PlanningComputer)(IGPMRSOP *This,BSTR bstrVal);
      HRESULT (WINAPI *get_PlanningComputer)(IGPMRSOP *This,BSTR *bstrVal);
      HRESULT (WINAPI *put_PlanningComputerSOM)(IGPMRSOP *This,BSTR bstrVal);
      HRESULT (WINAPI *get_PlanningComputerSOM)(IGPMRSOP *This,BSTR *bstrVal);
      HRESULT (WINAPI *put_PlanningComputerWMIFilters)(IGPMRSOP *This,VARIANT varVal);
      HRESULT (WINAPI *get_PlanningComputerWMIFilters)(IGPMRSOP *This,VARIANT *varVal);
      HRESULT (WINAPI *put_PlanningComputerSecurityGroups)(IGPMRSOP *This,VARIANT varVal);
      HRESULT (WINAPI *get_PlanningComputerSecurityGroups)(IGPMRSOP *This,VARIANT *varVal);
      HRESULT (WINAPI *LoggingEnumerateUsers)(IGPMRSOP *This,VARIANT *varVal);
      HRESULT (WINAPI *CreateQueryResults)(IGPMRSOP *This);
      HRESULT (WINAPI *ReleaseQueryResults)(IGPMRSOP *This);
      HRESULT (WINAPI *GenerateReport)(IGPMRSOP *This,GPMReportType gpmReportType,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
      HRESULT (WINAPI *GenerateReportToFile)(IGPMRSOP *This,GPMReportType gpmReportType,BSTR bstrTargetFilePath,IGPMResult **ppIGPMResult);
    END_INTERFACE
  } IGPMRSOPVtbl;
  struct IGPMRSOP {
    CONST_VTBL struct IGPMRSOPVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMRSOP_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMRSOP_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMRSOP_Release(This) (This)->lpVtbl->Release(This)
#define IGPMRSOP_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMRSOP_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMRSOP_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMRSOP_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMRSOP_get_Mode(This,pVal) (This)->lpVtbl->get_Mode(This,pVal)
#define IGPMRSOP_get_Namespace(This,bstrVal) (This)->lpVtbl->get_Namespace(This,bstrVal)
#define IGPMRSOP_put_LoggingComputer(This,bstrVal) (This)->lpVtbl->put_LoggingComputer(This,bstrVal)
#define IGPMRSOP_get_LoggingComputer(This,bstrVal) (This)->lpVtbl->get_LoggingComputer(This,bstrVal)
#define IGPMRSOP_put_LoggingUser(This,bstrVal) (This)->lpVtbl->put_LoggingUser(This,bstrVal)
#define IGPMRSOP_get_LoggingUser(This,bstrVal) (This)->lpVtbl->get_LoggingUser(This,bstrVal)
#define IGPMRSOP_put_LoggingFlags(This,lVal) (This)->lpVtbl->put_LoggingFlags(This,lVal)
#define IGPMRSOP_get_LoggingFlags(This,lVal) (This)->lpVtbl->get_LoggingFlags(This,lVal)
#define IGPMRSOP_put_PlanningFlags(This,lVal) (This)->lpVtbl->put_PlanningFlags(This,lVal)
#define IGPMRSOP_get_PlanningFlags(This,lVal) (This)->lpVtbl->get_PlanningFlags(This,lVal)
#define IGPMRSOP_put_PlanningDomainController(This,bstrVal) (This)->lpVtbl->put_PlanningDomainController(This,bstrVal)
#define IGPMRSOP_get_PlanningDomainController(This,bstrVal) (This)->lpVtbl->get_PlanningDomainController(This,bstrVal)
#define IGPMRSOP_put_PlanningSiteName(This,bstrVal) (This)->lpVtbl->put_PlanningSiteName(This,bstrVal)
#define IGPMRSOP_get_PlanningSiteName(This,bstrVal) (This)->lpVtbl->get_PlanningSiteName(This,bstrVal)
#define IGPMRSOP_put_PlanningUser(This,bstrVal) (This)->lpVtbl->put_PlanningUser(This,bstrVal)
#define IGPMRSOP_get_PlanningUser(This,bstrVal) (This)->lpVtbl->get_PlanningUser(This,bstrVal)
#define IGPMRSOP_put_PlanningUserSOM(This,bstrVal) (This)->lpVtbl->put_PlanningUserSOM(This,bstrVal)
#define IGPMRSOP_get_PlanningUserSOM(This,bstrVal) (This)->lpVtbl->get_PlanningUserSOM(This,bstrVal)
#define IGPMRSOP_put_PlanningUserWMIFilters(This,varVal) (This)->lpVtbl->put_PlanningUserWMIFilters(This,varVal)
#define IGPMRSOP_get_PlanningUserWMIFilters(This,varVal) (This)->lpVtbl->get_PlanningUserWMIFilters(This,varVal)
#define IGPMRSOP_put_PlanningUserSecurityGroups(This,varVal) (This)->lpVtbl->put_PlanningUserSecurityGroups(This,varVal)
#define IGPMRSOP_get_PlanningUserSecurityGroups(This,varVal) (This)->lpVtbl->get_PlanningUserSecurityGroups(This,varVal)
#define IGPMRSOP_put_PlanningComputer(This,bstrVal) (This)->lpVtbl->put_PlanningComputer(This,bstrVal)
#define IGPMRSOP_get_PlanningComputer(This,bstrVal) (This)->lpVtbl->get_PlanningComputer(This,bstrVal)
#define IGPMRSOP_put_PlanningComputerSOM(This,bstrVal) (This)->lpVtbl->put_PlanningComputerSOM(This,bstrVal)
#define IGPMRSOP_get_PlanningComputerSOM(This,bstrVal) (This)->lpVtbl->get_PlanningComputerSOM(This,bstrVal)
#define IGPMRSOP_put_PlanningComputerWMIFilters(This,varVal) (This)->lpVtbl->put_PlanningComputerWMIFilters(This,varVal)
#define IGPMRSOP_get_PlanningComputerWMIFilters(This,varVal) (This)->lpVtbl->get_PlanningComputerWMIFilters(This,varVal)
#define IGPMRSOP_put_PlanningComputerSecurityGroups(This,varVal) (This)->lpVtbl->put_PlanningComputerSecurityGroups(This,varVal)
#define IGPMRSOP_get_PlanningComputerSecurityGroups(This,varVal) (This)->lpVtbl->get_PlanningComputerSecurityGroups(This,varVal)
#define IGPMRSOP_LoggingEnumerateUsers(This,varVal) (This)->lpVtbl->LoggingEnumerateUsers(This,varVal)
#define IGPMRSOP_CreateQueryResults(This) (This)->lpVtbl->CreateQueryResults(This)
#define IGPMRSOP_ReleaseQueryResults(This) (This)->lpVtbl->ReleaseQueryResults(This)
#define IGPMRSOP_GenerateReport(This,gpmReportType,pvarGPMProgress,pvarGPMCancel,ppIGPMResult) (This)->lpVtbl->GenerateReport(This,gpmReportType,pvarGPMProgress,pvarGPMCancel,ppIGPMResult)
#define IGPMRSOP_GenerateReportToFile(This,gpmReportType,bstrTargetFilePath,ppIGPMResult) (This)->lpVtbl->GenerateReportToFile(This,gpmReportType,bstrTargetFilePath,ppIGPMResult)
#endif
#endif
  HRESULT WINAPI IGPMRSOP_get_Mode_Proxy(IGPMRSOP *This,GPMRSOPMode *pVal);
  void __RPC_STUB IGPMRSOP_get_Mode_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_Namespace_Proxy(IGPMRSOP *This,BSTR *bstrVal);
  void __RPC_STUB IGPMRSOP_get_Namespace_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_LoggingComputer_Proxy(IGPMRSOP *This,BSTR bstrVal);
  void __RPC_STUB IGPMRSOP_put_LoggingComputer_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_LoggingComputer_Proxy(IGPMRSOP *This,BSTR *bstrVal);
  void __RPC_STUB IGPMRSOP_get_LoggingComputer_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_LoggingUser_Proxy(IGPMRSOP *This,BSTR bstrVal);
  void __RPC_STUB IGPMRSOP_put_LoggingUser_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_LoggingUser_Proxy(IGPMRSOP *This,BSTR *bstrVal);
  void __RPC_STUB IGPMRSOP_get_LoggingUser_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_LoggingFlags_Proxy(IGPMRSOP *This,__LONG32 lVal);
  void __RPC_STUB IGPMRSOP_put_LoggingFlags_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_LoggingFlags_Proxy(IGPMRSOP *This,__LONG32 *lVal);
  void __RPC_STUB IGPMRSOP_get_LoggingFlags_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningFlags_Proxy(IGPMRSOP *This,__LONG32 lVal);
  void __RPC_STUB IGPMRSOP_put_PlanningFlags_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningFlags_Proxy(IGPMRSOP *This,__LONG32 *lVal);
  void __RPC_STUB IGPMRSOP_get_PlanningFlags_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningDomainController_Proxy(IGPMRSOP *This,BSTR bstrVal);
  void __RPC_STUB IGPMRSOP_put_PlanningDomainController_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningDomainController_Proxy(IGPMRSOP *This,BSTR *bstrVal);
  void __RPC_STUB IGPMRSOP_get_PlanningDomainController_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningSiteName_Proxy(IGPMRSOP *This,BSTR bstrVal);
  void __RPC_STUB IGPMRSOP_put_PlanningSiteName_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningSiteName_Proxy(IGPMRSOP *This,BSTR *bstrVal);
  void __RPC_STUB IGPMRSOP_get_PlanningSiteName_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningUser_Proxy(IGPMRSOP *This,BSTR bstrVal);
  void __RPC_STUB IGPMRSOP_put_PlanningUser_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningUser_Proxy(IGPMRSOP *This,BSTR *bstrVal);
  void __RPC_STUB IGPMRSOP_get_PlanningUser_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningUserSOM_Proxy(IGPMRSOP *This,BSTR bstrVal);
  void __RPC_STUB IGPMRSOP_put_PlanningUserSOM_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningUserSOM_Proxy(IGPMRSOP *This,BSTR *bstrVal);
  void __RPC_STUB IGPMRSOP_get_PlanningUserSOM_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningUserWMIFilters_Proxy(IGPMRSOP *This,VARIANT varVal);
  void __RPC_STUB IGPMRSOP_put_PlanningUserWMIFilters_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningUserWMIFilters_Proxy(IGPMRSOP *This,VARIANT *varVal);
  void __RPC_STUB IGPMRSOP_get_PlanningUserWMIFilters_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningUserSecurityGroups_Proxy(IGPMRSOP *This,VARIANT varVal);
  void __RPC_STUB IGPMRSOP_put_PlanningUserSecurityGroups_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningUserSecurityGroups_Proxy(IGPMRSOP *This,VARIANT *varVal);
  void __RPC_STUB IGPMRSOP_get_PlanningUserSecurityGroups_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningComputer_Proxy(IGPMRSOP *This,BSTR bstrVal);
  void __RPC_STUB IGPMRSOP_put_PlanningComputer_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningComputer_Proxy(IGPMRSOP *This,BSTR *bstrVal);
  void __RPC_STUB IGPMRSOP_get_PlanningComputer_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningComputerSOM_Proxy(IGPMRSOP *This,BSTR bstrVal);
  void __RPC_STUB IGPMRSOP_put_PlanningComputerSOM_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningComputerSOM_Proxy(IGPMRSOP *This,BSTR *bstrVal);
  void __RPC_STUB IGPMRSOP_get_PlanningComputerSOM_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningComputerWMIFilters_Proxy(IGPMRSOP *This,VARIANT varVal);
  void __RPC_STUB IGPMRSOP_put_PlanningComputerWMIFilters_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningComputerWMIFilters_Proxy(IGPMRSOP *This,VARIANT *varVal);
  void __RPC_STUB IGPMRSOP_get_PlanningComputerWMIFilters_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_put_PlanningComputerSecurityGroups_Proxy(IGPMRSOP *This,VARIANT varVal);
  void __RPC_STUB IGPMRSOP_put_PlanningComputerSecurityGroups_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_get_PlanningComputerSecurityGroups_Proxy(IGPMRSOP *This,VARIANT *varVal);
  void __RPC_STUB IGPMRSOP_get_PlanningComputerSecurityGroups_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_LoggingEnumerateUsers_Proxy(IGPMRSOP *This,VARIANT *varVal);
  void __RPC_STUB IGPMRSOP_LoggingEnumerateUsers_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_CreateQueryResults_Proxy(IGPMRSOP *This);
  void __RPC_STUB IGPMRSOP_CreateQueryResults_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_ReleaseQueryResults_Proxy(IGPMRSOP *This);
  void __RPC_STUB IGPMRSOP_ReleaseQueryResults_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_GenerateReport_Proxy(IGPMRSOP *This,GPMReportType gpmReportType,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMRSOP_GenerateReport_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMRSOP_GenerateReportToFile_Proxy(IGPMRSOP *This,GPMReportType gpmReportType,BSTR bstrTargetFilePath,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMRSOP_GenerateReportToFile_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMGPO_INTERFACE_DEFINED__
#define __IGPMGPO_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMGPO;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMGPO : public IDispatch {
  public:
    virtual HRESULT WINAPI get_DisplayName(BSTR *pVal) = 0;
    virtual HRESULT WINAPI put_DisplayName(BSTR newVal) = 0;
    virtual HRESULT WINAPI get_Path(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_ID(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_DomainName(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_CreationTime(DATE *pDate) = 0;
    virtual HRESULT WINAPI get_ModificationTime(DATE *pDate) = 0;
    virtual HRESULT WINAPI get_UserDSVersionNumber(__LONG32 *pVal) = 0;
    virtual HRESULT WINAPI get_ComputerDSVersionNumber(__LONG32 *pVal) = 0;
    virtual HRESULT WINAPI get_UserSysvolVersionNumber(__LONG32 *pVal) = 0;
    virtual HRESULT WINAPI get_ComputerSysvolVersionNumber(__LONG32 *pVal) = 0;
    virtual HRESULT WINAPI GetWMIFilter(IGPMWMIFilter **ppIGPMWMIFilter) = 0;
    virtual HRESULT WINAPI SetWMIFilter(IGPMWMIFilter *pIGPMWMIFilter) = 0;
    virtual HRESULT WINAPI SetUserEnabled(VARIANT_BOOL vbEnabled) = 0;
    virtual HRESULT WINAPI SetComputerEnabled(VARIANT_BOOL vbEnabled) = 0;
    virtual HRESULT WINAPI IsUserEnabled(VARIANT_BOOL *pvbEnabled) = 0;
    virtual HRESULT WINAPI IsComputerEnabled(VARIANT_BOOL *pvbEnabled) = 0;
    virtual HRESULT WINAPI GetSecurityInfo(IGPMSecurityInfo **ppSecurityInfo) = 0;
    virtual HRESULT WINAPI SetSecurityInfo(IGPMSecurityInfo *pSecurityInfo) = 0;
    virtual HRESULT WINAPI Delete(void) = 0;
    virtual HRESULT WINAPI Backup(BSTR bstrBackupDir,BSTR bstrComment,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult) = 0;
    virtual HRESULT WINAPI Import(__LONG32 lFlags,IGPMBackup *pIGPMBackup,VARIANT *pvarMigrationTable,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult) = 0;
    virtual HRESULT WINAPI GenerateReport(GPMReportType gpmReportType,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult) = 0;
    virtual HRESULT WINAPI GenerateReportToFile(GPMReportType gpmReportType,BSTR bstrTargetFilePath,IGPMResult **ppIGPMResult) = 0;
    virtual HRESULT WINAPI CopyTo(__LONG32 lFlags,IGPMDomain *pIGPMDomain,VARIANT *pvarNewDisplayName,VARIANT *pvarMigrationTable,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult) = 0;
    virtual HRESULT WINAPI SetSecurityDescriptor(__LONG32 lFlags,IDispatch *pSD) = 0;
    virtual HRESULT WINAPI GetSecurityDescriptor(__LONG32 lFlags,IDispatch **ppSD) = 0;
    virtual HRESULT WINAPI IsACLConsistent(VARIANT_BOOL *pvbConsistent) = 0;
    virtual HRESULT WINAPI MakeACLConsistent(void) = 0;
  };
#else
  typedef struct IGPMGPOVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMGPO *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMGPO *This);
      ULONG (WINAPI *Release)(IGPMGPO *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMGPO *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMGPO *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMGPO *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMGPO *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_DisplayName)(IGPMGPO *This,BSTR *pVal);
      HRESULT (WINAPI *put_DisplayName)(IGPMGPO *This,BSTR newVal);
      HRESULT (WINAPI *get_Path)(IGPMGPO *This,BSTR *pVal);
      HRESULT (WINAPI *get_ID)(IGPMGPO *This,BSTR *pVal);
      HRESULT (WINAPI *get_DomainName)(IGPMGPO *This,BSTR *pVal);
      HRESULT (WINAPI *get_CreationTime)(IGPMGPO *This,DATE *pDate);
      HRESULT (WINAPI *get_ModificationTime)(IGPMGPO *This,DATE *pDate);
      HRESULT (WINAPI *get_UserDSVersionNumber)(IGPMGPO *This,__LONG32 *pVal);
      HRESULT (WINAPI *get_ComputerDSVersionNumber)(IGPMGPO *This,__LONG32 *pVal);
      HRESULT (WINAPI *get_UserSysvolVersionNumber)(IGPMGPO *This,__LONG32 *pVal);
      HRESULT (WINAPI *get_ComputerSysvolVersionNumber)(IGPMGPO *This,__LONG32 *pVal);
      HRESULT (WINAPI *GetWMIFilter)(IGPMGPO *This,IGPMWMIFilter **ppIGPMWMIFilter);
      HRESULT (WINAPI *SetWMIFilter)(IGPMGPO *This,IGPMWMIFilter *pIGPMWMIFilter);
      HRESULT (WINAPI *SetUserEnabled)(IGPMGPO *This,VARIANT_BOOL vbEnabled);
      HRESULT (WINAPI *SetComputerEnabled)(IGPMGPO *This,VARIANT_BOOL vbEnabled);
      HRESULT (WINAPI *IsUserEnabled)(IGPMGPO *This,VARIANT_BOOL *pvbEnabled);
      HRESULT (WINAPI *IsComputerEnabled)(IGPMGPO *This,VARIANT_BOOL *pvbEnabled);
      HRESULT (WINAPI *GetSecurityInfo)(IGPMGPO *This,IGPMSecurityInfo **ppSecurityInfo);
      HRESULT (WINAPI *SetSecurityInfo)(IGPMGPO *This,IGPMSecurityInfo *pSecurityInfo);
      HRESULT (WINAPI *Delete)(IGPMGPO *This);
      HRESULT (WINAPI *Backup)(IGPMGPO *This,BSTR bstrBackupDir,BSTR bstrComment,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
      HRESULT (WINAPI *Import)(IGPMGPO *This,__LONG32 lFlags,IGPMBackup *pIGPMBackup,VARIANT *pvarMigrationTable,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
      HRESULT (WINAPI *GenerateReport)(IGPMGPO *This,GPMReportType gpmReportType,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
      HRESULT (WINAPI *GenerateReportToFile)(IGPMGPO *This,GPMReportType gpmReportType,BSTR bstrTargetFilePath,IGPMResult **ppIGPMResult);
      HRESULT (WINAPI *CopyTo)(IGPMGPO *This,__LONG32 lFlags,IGPMDomain *pIGPMDomain,VARIANT *pvarNewDisplayName,VARIANT *pvarMigrationTable,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
      HRESULT (WINAPI *SetSecurityDescriptor)(IGPMGPO *This,__LONG32 lFlags,IDispatch *pSD);
      HRESULT (WINAPI *GetSecurityDescriptor)(IGPMGPO *This,__LONG32 lFlags,IDispatch **ppSD);
      HRESULT (WINAPI *IsACLConsistent)(IGPMGPO *This,VARIANT_BOOL *pvbConsistent);
      HRESULT (WINAPI *MakeACLConsistent)(IGPMGPO *This);
    END_INTERFACE
  } IGPMGPOVtbl;
  struct IGPMGPO {
    CONST_VTBL struct IGPMGPOVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMGPO_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMGPO_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMGPO_Release(This) (This)->lpVtbl->Release(This)
#define IGPMGPO_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMGPO_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMGPO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMGPO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMGPO_get_DisplayName(This,pVal) (This)->lpVtbl->get_DisplayName(This,pVal)
#define IGPMGPO_put_DisplayName(This,newVal) (This)->lpVtbl->put_DisplayName(This,newVal)
#define IGPMGPO_get_Path(This,pVal) (This)->lpVtbl->get_Path(This,pVal)
#define IGPMGPO_get_ID(This,pVal) (This)->lpVtbl->get_ID(This,pVal)
#define IGPMGPO_get_DomainName(This,pVal) (This)->lpVtbl->get_DomainName(This,pVal)
#define IGPMGPO_get_CreationTime(This,pDate) (This)->lpVtbl->get_CreationTime(This,pDate)
#define IGPMGPO_get_ModificationTime(This,pDate) (This)->lpVtbl->get_ModificationTime(This,pDate)
#define IGPMGPO_get_UserDSVersionNumber(This,pVal) (This)->lpVtbl->get_UserDSVersionNumber(This,pVal)
#define IGPMGPO_get_ComputerDSVersionNumber(This,pVal) (This)->lpVtbl->get_ComputerDSVersionNumber(This,pVal)
#define IGPMGPO_get_UserSysvolVersionNumber(This,pVal) (This)->lpVtbl->get_UserSysvolVersionNumber(This,pVal)
#define IGPMGPO_get_ComputerSysvolVersionNumber(This,pVal) (This)->lpVtbl->get_ComputerSysvolVersionNumber(This,pVal)
#define IGPMGPO_GetWMIFilter(This,ppIGPMWMIFilter) (This)->lpVtbl->GetWMIFilter(This,ppIGPMWMIFilter)
#define IGPMGPO_SetWMIFilter(This,pIGPMWMIFilter) (This)->lpVtbl->SetWMIFilter(This,pIGPMWMIFilter)
#define IGPMGPO_SetUserEnabled(This,vbEnabled) (This)->lpVtbl->SetUserEnabled(This,vbEnabled)
#define IGPMGPO_SetComputerEnabled(This,vbEnabled) (This)->lpVtbl->SetComputerEnabled(This,vbEnabled)
#define IGPMGPO_IsUserEnabled(This,pvbEnabled) (This)->lpVtbl->IsUserEnabled(This,pvbEnabled)
#define IGPMGPO_IsComputerEnabled(This,pvbEnabled) (This)->lpVtbl->IsComputerEnabled(This,pvbEnabled)
#define IGPMGPO_GetSecurityInfo(This,ppSecurityInfo) (This)->lpVtbl->GetSecurityInfo(This,ppSecurityInfo)
#define IGPMGPO_SetSecurityInfo(This,pSecurityInfo) (This)->lpVtbl->SetSecurityInfo(This,pSecurityInfo)
#define IGPMGPO_Delete(This) (This)->lpVtbl->Delete(This)
#define IGPMGPO_Backup(This,bstrBackupDir,bstrComment,pvarGPMProgress,pvarGPMCancel,ppIGPMResult) (This)->lpVtbl->Backup(This,bstrBackupDir,bstrComment,pvarGPMProgress,pvarGPMCancel,ppIGPMResult)
#define IGPMGPO_Import(This,lFlags,pIGPMBackup,pvarMigrationTable,pvarGPMProgress,pvarGPMCancel,ppIGPMResult) (This)->lpVtbl->Import(This,lFlags,pIGPMBackup,pvarMigrationTable,pvarGPMProgress,pvarGPMCancel,ppIGPMResult)
#define IGPMGPO_GenerateReport(This,gpmReportType,pvarGPMProgress,pvarGPMCancel,ppIGPMResult) (This)->lpVtbl->GenerateReport(This,gpmReportType,pvarGPMProgress,pvarGPMCancel,ppIGPMResult)
#define IGPMGPO_GenerateReportToFile(This,gpmReportType,bstrTargetFilePath,ppIGPMResult) (This)->lpVtbl->GenerateReportToFile(This,gpmReportType,bstrTargetFilePath,ppIGPMResult)
#define IGPMGPO_CopyTo(This,lFlags,pIGPMDomain,pvarNewDisplayName,pvarMigrationTable,pvarGPMProgress,pvarGPMCancel,ppIGPMResult) (This)->lpVtbl->CopyTo(This,lFlags,pIGPMDomain,pvarNewDisplayName,pvarMigrationTable,pvarGPMProgress,pvarGPMCancel,ppIGPMResult)
#define IGPMGPO_SetSecurityDescriptor(This,lFlags,pSD) (This)->lpVtbl->SetSecurityDescriptor(This,lFlags,pSD)
#define IGPMGPO_GetSecurityDescriptor(This,lFlags,ppSD) (This)->lpVtbl->GetSecurityDescriptor(This,lFlags,ppSD)
#define IGPMGPO_IsACLConsistent(This,pvbConsistent) (This)->lpVtbl->IsACLConsistent(This,pvbConsistent)
#define IGPMGPO_MakeACLConsistent(This) (This)->lpVtbl->MakeACLConsistent(This)
#endif
#endif
  HRESULT WINAPI IGPMGPO_get_DisplayName_Proxy(IGPMGPO *This,BSTR *pVal);
  void __RPC_STUB IGPMGPO_get_DisplayName_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_put_DisplayName_Proxy(IGPMGPO *This,BSTR newVal);
  void __RPC_STUB IGPMGPO_put_DisplayName_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_get_Path_Proxy(IGPMGPO *This,BSTR *pVal);
  void __RPC_STUB IGPMGPO_get_Path_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_get_ID_Proxy(IGPMGPO *This,BSTR *pVal);
  void __RPC_STUB IGPMGPO_get_ID_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_get_DomainName_Proxy(IGPMGPO *This,BSTR *pVal);
  void __RPC_STUB IGPMGPO_get_DomainName_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_get_CreationTime_Proxy(IGPMGPO *This,DATE *pDate);
  void __RPC_STUB IGPMGPO_get_CreationTime_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_get_ModificationTime_Proxy(IGPMGPO *This,DATE *pDate);
  void __RPC_STUB IGPMGPO_get_ModificationTime_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_get_UserDSVersionNumber_Proxy(IGPMGPO *This,__LONG32 *pVal);
  void __RPC_STUB IGPMGPO_get_UserDSVersionNumber_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_get_ComputerDSVersionNumber_Proxy(IGPMGPO *This,__LONG32 *pVal);
  void __RPC_STUB IGPMGPO_get_ComputerDSVersionNumber_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_get_UserSysvolVersionNumber_Proxy(IGPMGPO *This,__LONG32 *pVal);
  void __RPC_STUB IGPMGPO_get_UserSysvolVersionNumber_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_get_ComputerSysvolVersionNumber_Proxy(IGPMGPO *This,__LONG32 *pVal);
  void __RPC_STUB IGPMGPO_get_ComputerSysvolVersionNumber_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_GetWMIFilter_Proxy(IGPMGPO *This,IGPMWMIFilter **ppIGPMWMIFilter);
  void __RPC_STUB IGPMGPO_GetWMIFilter_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_SetWMIFilter_Proxy(IGPMGPO *This,IGPMWMIFilter *pIGPMWMIFilter);
  void __RPC_STUB IGPMGPO_SetWMIFilter_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_SetUserEnabled_Proxy(IGPMGPO *This,VARIANT_BOOL vbEnabled);
  void __RPC_STUB IGPMGPO_SetUserEnabled_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_SetComputerEnabled_Proxy(IGPMGPO *This,VARIANT_BOOL vbEnabled);
  void __RPC_STUB IGPMGPO_SetComputerEnabled_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_IsUserEnabled_Proxy(IGPMGPO *This,VARIANT_BOOL *pvbEnabled);
  void __RPC_STUB IGPMGPO_IsUserEnabled_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_IsComputerEnabled_Proxy(IGPMGPO *This,VARIANT_BOOL *pvbEnabled);
  void __RPC_STUB IGPMGPO_IsComputerEnabled_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_GetSecurityInfo_Proxy(IGPMGPO *This,IGPMSecurityInfo **ppSecurityInfo);
  void __RPC_STUB IGPMGPO_GetSecurityInfo_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_SetSecurityInfo_Proxy(IGPMGPO *This,IGPMSecurityInfo *pSecurityInfo);
  void __RPC_STUB IGPMGPO_SetSecurityInfo_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_Delete_Proxy(IGPMGPO *This);
  void __RPC_STUB IGPMGPO_Delete_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_Backup_Proxy(IGPMGPO *This,BSTR bstrBackupDir,BSTR bstrComment,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMGPO_Backup_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_Import_Proxy(IGPMGPO *This,__LONG32 lFlags,IGPMBackup *pIGPMBackup,VARIANT *pvarMigrationTable,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMGPO_Import_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_GenerateReport_Proxy(IGPMGPO *This,GPMReportType gpmReportType,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMGPO_GenerateReport_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_GenerateReportToFile_Proxy(IGPMGPO *This,GPMReportType gpmReportType,BSTR bstrTargetFilePath,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMGPO_GenerateReportToFile_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_CopyTo_Proxy(IGPMGPO *This,__LONG32 lFlags,IGPMDomain *pIGPMDomain,VARIANT *pvarNewDisplayName,VARIANT *pvarMigrationTable,VARIANT *pvarGPMProgress,VARIANT *pvarGPMCancel,IGPMResult **ppIGPMResult);
  void __RPC_STUB IGPMGPO_CopyTo_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_SetSecurityDescriptor_Proxy(IGPMGPO *This,__LONG32 lFlags,IDispatch *pSD);
  void __RPC_STUB IGPMGPO_SetSecurityDescriptor_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_GetSecurityDescriptor_Proxy(IGPMGPO *This,__LONG32 lFlags,IDispatch **ppSD);
  void __RPC_STUB IGPMGPO_GetSecurityDescriptor_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_IsACLConsistent_Proxy(IGPMGPO *This,VARIANT_BOOL *pvbConsistent);
  void __RPC_STUB IGPMGPO_IsACLConsistent_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPO_MakeACLConsistent_Proxy(IGPMGPO *This);
  void __RPC_STUB IGPMGPO_MakeACLConsistent_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMGPOCollection_INTERFACE_DEFINED__
#define __IGPMGPOCollection_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMGPOCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMGPOCollection : public IDispatch {
  public:
    virtual HRESULT WINAPI get_Count(__LONG32 *pVal) = 0;
    virtual HRESULT WINAPI get_Item(__LONG32 lIndex,VARIANT *pVal) = 0;
    virtual HRESULT WINAPI get__NewEnum(IEnumVARIANT **ppIGPMGPOs) = 0;
  };
#else
  typedef struct IGPMGPOCollectionVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMGPOCollection *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMGPOCollection *This);
      ULONG (WINAPI *Release)(IGPMGPOCollection *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMGPOCollection *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMGPOCollection *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMGPOCollection *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMGPOCollection *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_Count)(IGPMGPOCollection *This,__LONG32 *pVal);
      HRESULT (WINAPI *get_Item)(IGPMGPOCollection *This,__LONG32 lIndex,VARIANT *pVal);
      HRESULT (WINAPI *get__NewEnum)(IGPMGPOCollection *This,IEnumVARIANT **ppIGPMGPOs);
    END_INTERFACE
  } IGPMGPOCollectionVtbl;
  struct IGPMGPOCollection {
    CONST_VTBL struct IGPMGPOCollectionVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMGPOCollection_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMGPOCollection_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMGPOCollection_Release(This) (This)->lpVtbl->Release(This)
#define IGPMGPOCollection_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMGPOCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMGPOCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMGPOCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMGPOCollection_get_Count(This,pVal) (This)->lpVtbl->get_Count(This,pVal)
#define IGPMGPOCollection_get_Item(This,lIndex,pVal) (This)->lpVtbl->get_Item(This,lIndex,pVal)
#define IGPMGPOCollection_get__NewEnum(This,ppIGPMGPOs) (This)->lpVtbl->get__NewEnum(This,ppIGPMGPOs)
#endif
#endif
  HRESULT WINAPI IGPMGPOCollection_get_Count_Proxy(IGPMGPOCollection *This,__LONG32 *pVal);
  void __RPC_STUB IGPMGPOCollection_get_Count_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPOCollection_get_Item_Proxy(IGPMGPOCollection *This,__LONG32 lIndex,VARIANT *pVal);
  void __RPC_STUB IGPMGPOCollection_get_Item_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPOCollection_get__NewEnum_Proxy(IGPMGPOCollection *This,IEnumVARIANT **ppIGPMGPOs);
  void __RPC_STUB IGPMGPOCollection_get__NewEnum_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
#endif

#ifndef __IGPMGPOLink_INTERFACE_DEFINED__
#define __IGPMGPOLink_INTERFACE_DEFINED__
  EXTERN_C const IID IID_IGPMGPOLink;
#if defined(__cplusplus) && !defined(CINTERFACE)
  struct IGPMGPOLink : public IDispatch {
  public:
    virtual HRESULT WINAPI get_GPOID(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_GPODomain(BSTR *pVal) = 0;
    virtual HRESULT WINAPI get_Enabled(VARIANT_BOOL *pVal) = 0;
    virtual HRESULT WINAPI put_Enabled(VARIANT_BOOL newVal) = 0;
    virtual HRESULT WINAPI get_Enforced(VARIANT_BOOL *pVal) = 0;
    virtual HRESULT WINAPI put_Enforced(VARIANT_BOOL newVal) = 0;
    virtual HRESULT WINAPI get_SOMLinkOrder(__LONG32 *lVal) = 0;
    virtual HRESULT WINAPI get_SOM(IGPMSOM **ppIGPMSOM) = 0;
    virtual HRESULT WINAPI Delete(void) = 0;
  };
#else
  typedef struct IGPMGPOLinkVtbl {
    BEGIN_INTERFACE
      HRESULT (WINAPI *QueryInterface)(IGPMGPOLink *This,REFIID riid,void **ppvObject);
      ULONG (WINAPI *AddRef)(IGPMGPOLink *This);
      ULONG (WINAPI *Release)(IGPMGPOLink *This);
      HRESULT (WINAPI *GetTypeInfoCount)(IGPMGPOLink *This,UINT *pctinfo);
      HRESULT (WINAPI *GetTypeInfo)(IGPMGPOLink *This,UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo);
      HRESULT (WINAPI *GetIDsOfNames)(IGPMGPOLink *This,REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
      HRESULT (WINAPI *Invoke)(IGPMGPOLink *This,DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
      HRESULT (WINAPI *get_GPOID)(IGPMGPOLink *This,BSTR *pVal);
      HRESULT (WINAPI *get_GPODomain)(IGPMGPOLink *This,BSTR *pVal);
      HRESULT (WINAPI *get_Enabled)(IGPMGPOLink *This,VARIANT_BOOL *pVal);
      HRESULT (WINAPI *put_Enabled)(IGPMGPOLink *This,VARIANT_BOOL newVal);
      HRESULT (WINAPI *get_Enforced)(IGPMGPOLink *This,VARIANT_BOOL *pVal);
      HRESULT (WINAPI *put_Enforced)(IGPMGPOLink *This,VARIANT_BOOL newVal);
      HRESULT (WINAPI *get_SOMLinkOrder)(IGPMGPOLink *This,__LONG32 *lVal);
      HRESULT (WINAPI *get_SOM)(IGPMGPOLink *This,IGPMSOM **ppIGPMSOM);
      HRESULT (WINAPI *Delete)(IGPMGPOLink *This);
    END_INTERFACE
  } IGPMGPOLinkVtbl;
  struct IGPMGPOLink {
    CONST_VTBL struct IGPMGPOLinkVtbl *lpVtbl;
  };
#ifdef COBJMACROS
#define IGPMGPOLink_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IGPMGPOLink_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IGPMGPOLink_Release(This) (This)->lpVtbl->Release(This)
#define IGPMGPOLink_GetTypeInfoCount(This,pctinfo) (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IGPMGPOLink_GetTypeInfo(This,iTInfo,lcid,ppTInfo) (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IGPMGPOLink_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IGPMGPOLink_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IGPMGPOLink_get_GPOID(This,pVal) (This)->lpVtbl->get_GPOID(This,pVal)
#define IGPMGPOLink_get_GPODomain(This,pVal) (This)->lpVtbl->get_GPODomain(This,pVal)
#define IGPMGPOLink_get_Enabled(This,pVal) (This)->lpVtbl->get_Enabled(This,pVal)
#define IGPMGPOLink_put_Enabled(This,newVal) (This)->lpVtbl->put_Enabled(This,newVal)
#define IGPMGPOLink_get_Enforced(This,pVal) (This)->lpVtbl->get_Enforced(This,pVal)
#define IGPMGPOLink_put_Enforced(This,newVal) (This)->lpVtbl->put_Enforced(This,newVal)
#define IGPMGPOLink_get_SOMLinkOrder(This,lVal) (This)->lpVtbl->get_SOMLinkOrder(This,lVal)
#define IGPMGPOLink_get_SOM(This,ppIGPMSOM) (This)->lpVtbl->get_SOM(This,ppIGPMSOM)
#define IGPMGPOLink_Delete(This) (This)->lpVtbl->Delete(This)
#endif
#endif
  HRESULT WINAPI IGPMGPOLink_get_GPOID_Proxy(IGPMGPOLink *This,BSTR *pVal);
  void __RPC_STUB IGPMGPOLink_get_GPOID_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPOLink_get_GPODomain_Proxy(IGPMGPOLink *This,BSTR *pVal);
  void __RPC_STUB IGPMGPOLink_get_GPODomain_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPOLink_get_Enabled_Proxy(IGPMGPOLink *This,VARIANT_BOOL *pVal);
  void __RPC_STUB IGPMGPOLink_get_Enabled_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPOLink_put_Enabled_Proxy(IGPMGPOLink *This,VARIANT_BOOL newVal);
  void __RPC_STUB IGPMGPOLink_put_Enabled_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPOLink_get_Enforced_Proxy(IGPMGPOLink *This,VARIANT_BOOL *pVal);
  void __RPC_STUB IGPMGPOLink_get_Enforced_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPOLink_put_Enforced_Proxy(IGPMGPOLink *This,VARIANT_BOOL newVal);
  void __RPC_STUB IGPMGPOLink_put_Enforced_Stub(IRpcStubBuffer *This,IRpcChannelBuffer *_pRpcChannelBuffer,PRPC_MESSAGE _pRpcMessage,DWORD *_pdwStubPhase);
  HRESULT WINAPI IGPMGPOLink_get_SOMLinkOrder_Proxy(IGPMGPOLink *This,__LONG32 *lVal);
  void __RPC_STUB IGPMGPOLink_get_SOMLinkOrder_Stub(IRpcStubBuf