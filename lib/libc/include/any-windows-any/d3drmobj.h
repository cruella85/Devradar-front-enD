#undef INTERFACE
/*
 * Copyright (C) 2008 Vijay Kiran Kamuju
 * Copyright (C) 2010 Christian Costa
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __D3DRMOBJ_H__
#define __D3DRMOBJ_H__

#include <objbase.h>
#define VIRTUAL
#include <d3drmdef.h>
#include <d3d.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Direct3DRM object CLSIDs */

DEFINE_GUID(CLSID_CDirect3DRMDevice,                    0x4fa3568e, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMViewport,                  0x4fa3568f, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMFrame,                     0x4fa35690, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMMesh,                      0x4fa35691, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMMeshBuilder,               0x4fa35692, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMFace,                      0x4fa35693, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMLight,                     0x4fa35694, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMTexture,                   0x4fa35695, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMWrap,                      0x4fa35696, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMMaterial,                  0x4fa35697, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMAnimation,                 0x4fa35698, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMAnimationSet,              0x4fa35699, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMUserVisual,                0x4fa3569a, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMShadow,                    0x4fa3569b, 0x623f, 0x11cf, 0xac, 0x4a, 0x0, 0x0, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(CLSID_CDirect3DRMViewportInterpolator,      0xde9eaa1, 0x3b84, 0x11d0, 0x9b, 0x6d, 0x0, 0x0, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(CLSID_CDirect3DRMFrameInterpolator,         0xde9eaa2, 0x3b84, 0x11d0, 0x9b, 0x6d, 0x0, 0x0, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(CLSID_CDirect3DRMMeshInterpolator,          0xde9eaa3, 0x3b84, 0x11d0, 0x9b, 0x6d, 0x0, 0x0, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(CLSID_CDirect3DRMLightInterpolator,         0xde9eaa6, 0x3b84, 0x11d0, 0x9b, 0x6d, 0x0, 0x0, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(CLSID_CDirect3DRMMaterialInterpolator,      0xde9eaa7, 0x3b84, 0x11d0, 0x9b, 0x6d, 0x0, 0x0, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(CLSID_CDirect3DRMTextureInterpolator,       0xde9eaa8, 0x3b84, 0x11d0, 0x9b, 0x6d, 0x0, 0x0, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(CLSID_CDirect3DRMProgressiveMesh,           0x4516ec40, 0x8f20, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(CLSID_CDirect3DRMClippedVisual,             0x5434e72d, 0x6d66, 0x11d1, 0xbb, 0xb, 0x0, 0x0, 0xf8, 0x75, 0x86, 0x5a);

/* Direct3DRM object interface GUIDs */

DEFINE_GUID(IID_IDirect3DRMObject,          0xeb16cb00, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMObject2,         0x4516ec7c, 0x8f20, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(IID_IDirect3DRMDevice,          0xe9e19280, 0x6e05, 0x11cf, 0xac, 0x4a, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMDevice2,         0x4516ec78, 0x8f20, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(IID_IDirect3DRMDevice3,         0x549f498b, 0xbfeb, 0x11d1, 0x8e, 0xd8, 0x00, 0xa0, 0xc9, 0x67, 0xa4, 0x82);
DEFINE_GUID(IID_IDirect3DRMViewport,        0xeb16cb02, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMViewport2,       0x4a1b1be6, 0xbfed, 0x11d1, 0x8e, 0xd8, 0x00, 0xa0, 0xc9, 0x67, 0xa4, 0x82);
DEFINE_GUID(IID_IDirect3DRMFrame,           0xeb16cb03, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMFrame2,          0xc3dfbd60, 0x3988, 0x11d0, 0x9e, 0xc2, 0x00, 0x00, 0xc0, 0x29, 0x1a, 0xc3);
DEFINE_GUID(IID_IDirect3DRMFrame3,          0xff6b7f70, 0xa40e, 0x11d1, 0x91, 0xf9, 0x00, 0x00, 0xf8, 0x75, 0x8e, 0x66);
DEFINE_GUID(IID_IDirect3DRMVisual,          0xeb16cb04, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMMesh,            0xa3a80d01, 0x6e12, 0x11cf, 0xac, 0x4a, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMMeshBuilder,     0xa3a80d02, 0x6e12, 0x11cf, 0xac, 0x4a, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMMeshBuilder2,    0x4516ec77, 0x8f20, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(IID_IDirect3DRMMeshBuilder3,    0x4516ec82, 0x8f20, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(IID_IDirect3DRMFace,            0xeb16cb07, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMFace2,           0x4516ec81, 0x8f20, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(IID_IDirect3DRMLight,           0xeb16cb08, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMTexture,         0xeb16cb09, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMTexture2,        0x120f30c0, 0x1629, 0x11d0, 0x94, 0x1c, 0x00, 0x80, 0xc8, 0x0c, 0xfa, 0x7b);
DEFINE_GUID(IID_IDirect3DRMTexture3,        0xff6b7f73, 0xa40e, 0x11d1, 0x91, 0xf9, 0x00, 0x00, 0xf8, 0x75, 0x8e, 0x66);
DEFINE_GUID(IID_IDirect3DRMWrap,            0xeb16cb0a, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMMaterial,        0xeb16cb0b, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMMaterial2,       0xff6b7f75, 0xa40e, 0x11d1, 0x91, 0xf9, 0x00, 0x00, 0xf8, 0x75, 0x8e, 0x66);
DEFINE_GUID(IID_IDirect3DRMAnimation,       0xeb16cb0d, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMAnimation2,      0xff6b7f77, 0xa40e, 0x11d1, 0x91, 0xf9, 0x00, 0x00, 0xf8, 0x75, 0x8e, 0x66);
DEFINE_GUID(IID_IDirect3DRMAnimationSet,    0xeb16cb0e, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMAnimationSet2,   0xff6b7f79, 0xa40e, 0x11d1, 0x91, 0xf9, 0x00, 0x00, 0xf8, 0x75, 0x8e, 0x66);
DEFINE_GUID(IID_IDirect3DRMObjectArray,     0x242f6bc2, 0x3849, 0x11d0, 0x9b, 