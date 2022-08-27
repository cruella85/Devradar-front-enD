
/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef __WINEVT_H__
#define __WINEVT_H__

#include <winapifamily.h>

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

#if (_WIN32_WINNT >= 0x0600)

#ifdef __cplusplus
extern "C" {
#endif

#define EVT_READ_ACCESS 0x1
#define EVT_WRITE_ACCESS 0x2
#define EVT_CLEAR_ACCESS 0x4
#define EVT_ALL_ACCESS 0x7

typedef enum _EVT_CHANNEL_CLOCK_TYPE {
  EvtChannelClockTypeSystemTime   = 0,
  EvtChannelClockTypeQPC          = 1 
} EVT_CHANNEL_CLOCK_TYPE;

typedef enum _EVT_CHANNEL_CONFIG_PROPERTY_ID {
  EvtChannelConfigEnabled                 = 0,
  EvtChannelConfigIsolation               = 1,
  EvtChannelConfigType                    = 2,
  EvtChannelConfigOwningPublisher         = 3,
  EvtChannelConfigClassicEventlog         = 4,
  EvtChannelConfigAccess                  = 5,
  EvtChannelLoggingConfigRetention        = 6,
  EvtChannelLoggingConfigAutoBackup       = 7,
  EvtChannelLoggingConfigMaxSize          = 8,
  EvtChannelLoggingConfigLogFilePath      = 9,
  EvtChannelPublishingConfigLevel         = 10,
  EvtChannelPublishingConfigKeywords      = 11,
  EvtChannelPublishingConfigControlGuid   = 12,
  EvtChannelPublishingConfigBufferSize    = 13,
  EvtChannelPublishingConfigMinBuffers    = 14,
  EvtChannelPublishingConfigMaxBuffers    = 15,
  EvtChannelPublishingConfigLatency       = 16,
  EvtChannelPublishingConfigClockType     = 17,
  EvtChannelPublishingConfigSidType       = 18,
  EvtChannelPublisherList                 = 19,
  EvtChannelPublishingConfigFileMax       = 20,
  EvtChannelConfigPropertyIdEND           = 21 
} EVT_CHANNEL_CONFIG_PROPERTY_ID;

typedef enum _EVT_CHANNEL_ISOLATION_TYPE {
  EvtChannelIsolationTypeApplication   = 0,
  EvtChannelIsolationTypeSystem        = 1,
  EvtChannelIsolationTypeCustom        = 2 
} EVT_CHANNEL_ISOLATION_TYPE;

typedef enum _EVT_CHANNEL_REFERENCE_FLAGS {
  EvtChannelReferenceImported   = 0x1 
} EVT_CHANNEL_REFERENCE_FLAGS;

typedef enum _EVT_CHANNEL_SID_TYPE {
  EvtChannelSidTypeNone         = 0,
  EvtChannelSidTypePublishing   = 1 
} EVT_CHANNEL_SID_TYPE;

typedef enum _EVT_CHANNEL_TYPE {
  EvtChannelTypeAdmin         = 0,
  EvtChannelTypeOperational   = 1,
  EvtChannelTypeAnalytic      = 2,
  EvtChannelTypeDebug         = 3 
} EVT_CHANNEL_TYPE;

typedef enum _EVT_EVENT_METADATA_PROPERTY_ID {
  EventMetadataEventID            = 0,
  EventMetadataEventVersion       = 1,
  EventMetadataEventChannel       = 2,
  EventMetadataEventLevel         = 3,
  EventMetadataEventOpcode        = 4,
  EventMetadataEventTask          = 5,
  EventMetadataEventKeyword       = 6,
  EventMetadataEventMessageID     = 7,
  EventMetadataEventTemplate      = 8,
  EvtEventMetadataPropertyIdEND   = 9 
} EVT_EVENT_METADATA_PROPERTY_ID;

typedef enum _EVT_EVENT_PROPERTY_ID {
  EvtEventQueryIDs        = 0,
  EvtEventPath            = 1,
  EvtEventPropertyIdEND   = 2 
} EVT_EVENT_PROPERTY_ID;

typedef enum _EVT_EXPORTLOG_FLAGS {
  EvtExportLogChannelPath           = 0x1,
  EvtExportLogFilePath              = 0x2,
  EvtExportLogTolerateQueryErrors   = 0x1000 
} EVT_EXPORTLOG_FLAGS;

typedef enum _EVT_FORMAT_MESSAGE_FLAGS {
  EvtFormatMessageEvent      = 1,
  EvtFormatMessageLevel      = 2,
  EvtFormatMessageTask       = 3,
  EvtFormatMessageOpcode     = 4,
  EvtFormatMessageKeyword    = 5,
  EvtFormatMessageChannel    = 6,
  EvtFormatMessageProvider   = 7,
  EvtFormatMessageId         = 8,
  EvtFormatMessageXml        = 9 
} EVT_FORMAT_MESSAGE_FLAGS;

typedef enum _EVT_LOG_PROPERTY_ID {
  EvtLogCreationTime         = 0,
  EvtLogLastAccessTime       = 1,
  EvtLogLastWriteTime        = 2,
  EvtLogFileSize             = 3,
  EvtLogAttributes           = 4,
  EvtLogNumberOfLogRecords   = 5,
  EvtLogOldestRecordNumber   = 6,
  EvtLogFull                 = 7 
} EVT_LOG_PROPERTY_ID;

typedef enum _EVT_LOGIN_CLASS {
  EvtRpcLogin   = 1 
} EVT_LOGIN_CLASS;

typedef enum _EVT_OPEN_LOG_FLAGS {
  EvtOpenChannelPath   = 0x1,
  EvtOpenFilePath      = 0x2 
} EVT_OPEN_LOG_FLAGS;

typedef enum _EVT_PUBLISHER_METADATA_PROPERTY_ID {