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
#define IGPMDomain_RestoreGPO(This,pIGPMBackup,lDCFlags,pvarGPMProgress,pvarGPMCancel,ppIGPMResult) (This)->lpVtbl->RestoreGPO(This,pIGPMBackup,lDCFlags,pvarGPMP