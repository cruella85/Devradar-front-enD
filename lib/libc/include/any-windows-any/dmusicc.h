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

/* GUIDs - property s