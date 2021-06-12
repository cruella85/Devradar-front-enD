#undef INTERFACE
/* DirectMusic Core API Stuff
 *
 * Copyright (C) 2003-2004 Rok Mandeljc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_DMUSIC_CORE_H
#define __WINE_DMUSIC_CORE_H

#include <windows.h>

#define COM_NO_WINDOWS_H
#include <objbase.h>
#include <mmsystem.h>

#include <dls1.h>
#include <dmerror.h>
#include <dmdls.h>
#include <dsound.h>
#include <dmusbuff.h>

#include <pshpack8.h>

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * Predeclare the interfaces
 */
/* CLSIDs */
DEFINE_GUID(CLSID_DirectMusic,                    0x636b9f10,0x0c7d,0x11d1,0x95,0xb2,0x00,0x20,0xaf,0xdc,0x74,0x21);
DEFINE_GUID(CLSID_DirectMusicCollection,          0x480ff4b0,0x28b2,0x11d1,0xbe,0xf7,0x00,0xc0,0x4f,0xbf,0x8f,0xef);
DEFINE_GUID(CLSID_DirectMusicSynth,               0x58c2b4d0,0x46e7,0x11d1,0x89,0xac,0x00,0xa0,0xc9,0x05,0x41,0x29);
	
/* IIDs */
DEFINE_GUID(IID_IDirectMusic,                     0x6536115a,0x7b2d,0x11d2,0xba,0x18,0x00,0x00,0xf8,0x75,0xac,0x12);
DEFINE_GUID(IID_IDirectMusic2,                    0x6fc2cae1,0xbc78,0x11d2,0xaf,0xa6,0x00,0xaa,0x00,0x24,0xd8,0xb6);
DEFINE_GUID(IID_IDirectMusic8,                    0x2d3629f7,0x813d,0x4939,0x85,0x08,0xf0,0x5c,0x6b,0x75,0xfd,0x97);
DEFINE_GUID(IID_IDirectMusicBuffer,               0xd2ac2878,0xb39b,0x11d1,0x87,0x04,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(IID_IDirectMusicCollection,           0xd2ac287c,0xb39b,0x11d1,0x87,0x04,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(IID_IDirectMusicDownload,             0xd2ac287b,0xb39b,0x11d1,0x87,0x04,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(IID_IDirectMusicDownloadedInstrument, 0xd2ac287e,0xb39b,0x11d1,0x87,0x04,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(IID_IDirectMusicInstrument,           0xd2ac287d,0xb39b,0x11d1,0x87,0x04,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(IID_IDirectMusicPort,                 0x08f2d8c9,0x37c2,0x11d2,0xb9,0xf9,0x00,0x00,0xf8,0x75,0xac,0x12);
DEFINE_GUID(IID_IDirectMusicPortDownload,         0xd2ac287a,0xb39b,0x11d1,0x87,0x04,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(IID_IDirectMusicThru,                 0xced153e7,0x3606,0x11d2,0xb9,0xf9,0x00,0x00,0xf8,0x75,0xac,0x12);

#define IID_IDirectMusicCollection8 IID_IDirectMusicCollection
#define IID_IDirectMusicDownload8 IID_IDirectMusicDownload
#define IID_IDirectMusicDownloadedInstrument8 IID_IDirectMusicDownloadedInstrument
#define IID_IDirectMusicInstrument8 IID_IDirectMusicInstrument
#define IID_IDirectMusicPort8 IID_IDirectMusicPort
#define IID_IDirectMusicPortDownload8 IID_IDirectMusicPortDownload
#define IID_IDirectMusicThru8 IID_IDirectMusicThru

/* GUIDs - property set */
DEFINE_GUID(GUID_DMUS_PROP_GM_Hardware,           0x178f2f24,0xc364,0x11d1,0xa7,0x60,0x00,0x00,0xf8,0x75,0xac,0x12);
DEFINE_GUID(GUID_DMUS_PROP_GS_Capable,            0x6496aba2,0x61b0,0x11d2,0xaf,0xa6,0x00,0xaa,0x00,0x24,0xd8,0xb6);
DEFINE_GUID(GUID_DMUS_PROP_GS_Hardware,           0x178f2f25,0xc364,0x11d1,0xa7,0x60,0x00,0x00,0xf8,0x75,0xac,0x12);
DEFINE_GUID(GUID_DMUS_PROP_DLS1,                  0x178f2f27,0xc364,0x11d1,0xa7,0x60,0x00,0x00,0xf8,0x75,0xac,0x12);
DEFINE_GUID(GUID_DMUS_PROP_DLS2,                  0xf14599e5,0x4689,0x11d2,0xaf,0xa6,0x00,0xaa,0x00,0x24,0xd8,0xb6);
DEFINE_GUID(GUID_DMUS_PROP_Effects,               0xcda8d611,0x684a,0x11d2,0x87,0x1e,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(GUID_DMUS_PROP_INSTRUMENT2,           0x865fd372,0x9f67,0x11d2,0x87,0x2a,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(GUID_DMUS_PROP_LegacyCaps,            0xcfa7cdc2,0x00a1,0x11d2,0xaa,0xd5,0x00,0x00,0xf8,0x75,0xac,0x12);
DEFINE_GUID(GUID_DMUS_PROP_MemorySize,            0x178f2f28,0xc364,0x11d1,0xa7,0x60,0x00,0x00,0xf8,0x75,0xac,0x12);
DEFINE_GUID(GUID_DMUS_PROP_SampleMemorySize,      0x178f2f28,0xc364,0x11d1,0xa7,0x60,0x00,0x00,0xf8,0x75,0xac,0x12);
DEFINE_GUID(GUID_DMUS_PROP_SamplePlaybackRate,    0x2a91f713,0xa4bf,0x11d2,0xbb,0xdf,0x00,0x60,0x08,0x33,0xdb,0xd8);
DEFINE_GUID(GUID_DMUS_PROP_SynthSink_DSOUND,      0x0aa97844,0xc877,0x11d1,0x87,0x0c,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(GUID_DMUS_PROP_SynthSink_WAVE,        0x0aa97845,0xc877,0x11d1,0x87,0x0c,0x00,0x60,0x08,0x93,0xb1,0xbd);
DEFINE_GUID(GUID_DMUS_PROP_Volume,                0xfedfae25,0xe46e,0x11d1,0xaa,0xce,0x00,0x00,0xf8,0x75,0xac,0x12);
DEFINE_GUID(GUID_DMUS_PROP_WavesReverb,           0x04cb5622,0x32e5,0x11d2,0xaf,0xa6,0x00,0xaa,0x00,0x24,0xd8,0xb6);
DEFINE_GUID(GUID_DMUS_PROP_WriteLatency,          0x268a0fa0,0x60f2,0x11d2,0xaf,0xa6,0x00,0xaa,0x00,0x24,0xd8,0xb6);
DEFINE_GUID(GUID_DMUS_PROP_WritePeriod,           0x268a0fa1,0x60f2,0x11d2,0xaf,0xa6,0x00,0xaa,0x00,0x24,0xd8,0xb6);
DEFINE_GUID(GUID_DMUS_PROP_XG_Capable,            0x6496aba1,0x61b0,0x11d2,0xaf,0xa6,0x00,0xaa,0x00,0x24,0xd8,0xb6);
DEFINE_GUID(GUID_DMUS_PROP_XG_Hardware,           0x178f2f26,0xc364,0x11d1,0xa7,0x60,0x00,0x00,0xf8,0x75,0xac,0x12);	

/* typedef definitions */
typedef struct IDirectMusic *LPDIRECTMUSIC;
typedef struct IDirectMusic8 *LPDIRECTMUSIC8;
typedef struct IDirectMusicBuffer *LPDIRECTMUSICBUFFER;
typedef struct IDirectMusicBuffer IDirectMusicBuffer8, *LPDIRECTMUSICBUFFER8;
typedef struct IDirectMusicInstrument *LPDIRECTMUSICINSTRUMENT;
typedef struct IDirectMusicInstrument IDirectMusicInstrument8, *LPDIRECTMUSICINSTRUMENT8;
typedef struct IDirectMusicDownloadedInstrument *LPDIRECTMUSICDOWNLOADEDINSTRUMENT;
typedef struct IDirectMusicDownloadedInstrument IDirectMusicDownloadedInstrument8, *LPDIRECTMUSICDOWNLOADEDINSTRUMENT8;
typedef struct IDirectMusicCollection *LPDIRECTMUSICCOLLECTION;
typedef struct IDirectMusicCollection IDirectMusicCollection8, *LPDIRECTMUSICCOLLECTION8;
typedef struct IDirectMusicDownload *LPDIRECTMUSICDOWNLOAD;
typedef struct IDirectMusicDownload IDirectMusicDownload8, *LPDIRECTMUSICDOWNLOAD8;
typedef struct IDirectMusicPortDownload *LPDIRECTMUSICPORTDOWNLOAD;
typedef struct IDirectMusicPortDownload IDirectMusicPortDownload8, *LPDIRECTMUSICPORTDOWNLOAD8;
typedef struct IDirectMusicPort *LPDIRECTMUSICPORT;
typedef struct IDirectMusicPort IDirectMusicPort8, *LPDIRECTMUSICPORT8;
typedef struct IDirectMusicThru *LPDIRECTMUSICTHRU;
typedef struct IDirectMusicThru IDirectMusicThru8, *LPDIRECTMUSICTHRU8;
typedef struct IReferenceClock *LPREFERENCECLOCK;


/*****************************************************************************
 * Typedef definitions
 */
typedef ULONGLONG    SAMPLE_TIME, *LPSAMPLE_TIME;
typedef ULONGLONG    SAMPLE_POSITION, *LPSAMPLE_POSITION;	


/*****************************************************************************
 * Flags
 */
#ifndef _DIRECTAUDIO_PRIORITIES_DEFINED_
#define _DIRECTAUDIO_PRIORITIES_DEFINED_

#define DAUD_CRITICAL_VOICE_PRIORITY 0xF0000000
#define DAUD_HIGH_VOICE_PRIORITY     0xC0000000
#define DAUD_STANDARD_VOICE_PRIORITY 0x80000000
#define DAUD_LOW_VOICE_PRIORITY      0x40000000
#define DAUD_PERSIST_VOICE_PRIORITY  0x10000000

#define DAUD_CHAN1_VOICE_PRIORITY_OFFSET  0x0000000E
#define DAUD_CHAN2_VOICE_PRIORITY_OFFSET  0x0000000D
#define DAUD_CHAN3_VOICE_PRIORITY_OFFSET  0x0000000C
#define DAUD_CHAN4_VOICE_PRIORITY_OFFSET  0x0000000B
#define DAUD_CHAN5_VOICE_PRIORITY_OFFSET  0x0000000A
#define DAUD_CHAN6_VOICE_PRIORITY_OFFSET  0x00000009
#define DAUD_CHAN7_VOICE_PRIORITY_OFFSET  0x00000008
#define DAUD_CHAN8_VOICE_PRIORITY_OFFSET  0x00000007
#define DAUD_CHAN9_VOICE_PRIORITY_OFFSET  0x00000006
#define DAUD_CHAN10_VOICE_PRIORITY_OFFSET 0x0000000F
#define DAUD_CHAN11_VOICE_PRIORITY_OFFSET 0x00000005
#define DAUD_CHAN12_VOICE_PRIORITY_OFFSET 0x00000004
#define DAUD_CHAN13_VOICE_PRIORITY_OFFSET 0x00000003
#define DAUD_CHAN14_VOICE_PRIORITY_OFFSET 0x00000002
#define DAUD_CHAN15_VOICE_PRIORITY_OFFSET 0x00000001
#define DAUD_CHAN16_VOICE_PRIORITY_OFFSET 0x00000000

#define DAUD_CHAN1_DEF_VOICE_PRIORITY  (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN1_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN2_DEF_VOICE_PRIORITY  (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN2_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN3_DEF_VOICE_PRIORITY  (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN3_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN4_DEF_VOICE_PRIORITY  (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN4_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN5_DEF_VOICE_PRIORITY  (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN5_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN6_DEF_VOICE_PRIORITY  (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN6_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN7_DEF_VOICE_PRIORITY  (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN7_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN8_DEF_VOICE_PRIORITY  (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN8_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN9_DEF_VOICE_PRIORITY  (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN9_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN10_DEF_VOICE_PRIORITY (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN10_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN11_DEF_VOICE_PRIORITY (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN11_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN12_DEF_VOICE_PRIORITY (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN12_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN13_DEF_VOICE_PRIORITY (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN13_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN14_DEF_VOICE_PRIORITY (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN14_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN15_DEF_VOICE_PRIORITY (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN15_VOICE_PRIORITY_OFFSET)
#define DAUD_CHAN16_DEF_VOICE_PRIORITY (DAUD_STANDARD_VOICE_PRIORITY | DAUD_CHAN16_VOICE_PRIORITY_OFFSET)
#endif  /* _DIRECTAUDIO_PRIORITIES_DEFINED_ */

#define DMUS_CLOCKF_GLOBAL 0x1

#define DMUS_EFFECT_NONE   0x0
#define DMUS_EFFECT_REVERB 0x1
#define DMUS_EFFECT_CHORUS 0x2
#define DMUS_EFFECT_DELAY  0x4
	
#define DMUS_MAX_DESCRIPTION 0x80
#define DMUS_MAX_DRIVER 0x80

#define DMUS_PC_INPUTCLASS  0x0
#define DMUS_PC_OUTPUTCLASS 0x1

#define DMUS_PC_DLS             0x00000001
#define DMUS_PC_EXTERNAL        0x00000002
#define DMUS_PC_SOFTWARESYNTH   0x00000004
#define DMUS_PC_MEMORYSIZEFIXED 0x00000008
#define DMUS_PC_GMINHARDWARE    0x00000010
#define DMUS_PC_GSINHARDWARE    0x00000020
#define DMUS_PC_XGINHARDWARE    0x00000040
#define DMUS_PC_DIRECTSOUND     0x00000080
#define DMUS_PC_SHAREABLE       0x00000100
#define DMUS_PC_DLS2            0x00000200
#define DMUS_PC_AUDIOPATH       0x00000400
#define DMUS_PC_WAVE            0x00000800
#define DMUS_PC_SYSTEMMEMORY    0x7FFFFFFF

#define DMUS_PORT_WINMM_DRIVER    0x0
#define DMUS_PORT_USER_MODE_SYNTH 0x1
#define DMUS_PORT_KERNEL_MODE     0x2

#define DMUS_PORT_FEATURE_AUDIOPATH     0x1
#define DMUS_PORT_FEATURE_STREAMING     0x2

#define DMUS_PORTPARAMS_VOICES           0x01
#define DMUS_PORTPARAMS_CHANNELGROUPS    0x02
#define DMUS_PORTPARAMS_AUDIOCHANNELS    0x04
#define DMUS_PORTPARAMS_SAMPLERATE       0x08
#define DMUS_PORTPARAMS_EFFECTS          0x20
#define DMUS_PORTPARAMS_SHARE            0x40
#define DMUS_PORTPARAMS_FEATURES         0x80

#define DMUS_VOLUME_MAX     2000
#define DMUS_VOLUME_MIN   -20000

#define DMUS_SYNTHSTATS_VOICES        0x01
#define DMUS_SYNTHSTATS_TOTAL_CPU     0x02
#define DMUS_SYNTHSTATS_CPU_PER_VOICE 0x04
#define DMUS_SYNTHSTATS_LOST_NOTES    0x08
#define DMUS_SYNTHSTATS_PEAK_VOLUME   0x10
#define DMUS_SYNTHSTATS_FREE_MEMORY   0x20
#define DMUS_SYNTHSTATS_SYSTEMMEMORY  DMUS_PC_SYSTEMMEMORY

#define DSBUSID_FIRST_SPKR_LOC        0x00000000
#define DSBUSID_FRONT_LEFT            0x00000000
#define DSBUSID_LEFT                  0x00000000
#define DSBUSID_FRONT_RIGHT           0x00000001
#define DSBUSID_RIGHT                 0x0000000