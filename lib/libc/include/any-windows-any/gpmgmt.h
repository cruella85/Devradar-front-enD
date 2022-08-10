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
typedef struct GPMR