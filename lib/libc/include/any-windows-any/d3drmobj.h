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
DEFINE_GUID(IID_IDirect3DRMObjectArray,     0x242f6bc2, 0x3849, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(IID_IDirect3DRMDeviceArray,     0xeb16cb10, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMViewportArray,   0xeb16cb11, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMFrameArray,      0xeb16cb12, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMVisualArray,     0xeb16cb13, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMLightArray,      0xeb16cb14, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMPickedArray,     0xeb16cb16, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMFaceArray,       0xeb16cb17, 0xd271, 0x11ce, 0xac, 0x48, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMAnimationArray,  0xd5f1cae0, 0x4bd7, 0x11d1, 0xb9, 0x74, 0x00, 0x60, 0x08, 0x3e, 0x45, 0xf3);
DEFINE_GUID(IID_IDirect3DRMUserVisual,      0x59163de0, 0x6d43, 0x11cf, 0xac, 0x4a, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMShadow,          0xaf359780, 0x6ba3, 0x11cf, 0xac, 0x4a, 0x00, 0x00, 0xc0, 0x38, 0x25, 0xa1);
DEFINE_GUID(IID_IDirect3DRMShadow2,         0x86b44e25, 0x9c82, 0x11d1, 0xbb, 0x0b, 0x00, 0xa0, 0xc9, 0x81, 0xa0, 0xa6);
DEFINE_GUID(IID_IDirect3DRMInterpolator,    0x242f6bc1, 0x3849, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(IID_IDirect3DRMProgressiveMesh, 0x4516ec79, 0x8f20, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(IID_IDirect3DRMPicked2Array,    0x4516ec7b, 0x8f20, 0x11d0, 0x9b, 0x6d, 0x00, 0x00, 0xc0, 0x78, 0x1b, 0xc3);
DEFINE_GUID(IID_IDirect3DRMClippedVisual,   0x5434e733, 0x6d66, 0x11d1, 0xbb, 0x0b, 0x00, 0x00, 0xf8, 0x75, 0x86, 0x5a);

/*****************************************************************************
 * Predeclare the interfaces
 */

typedef struct IDirect3DRMObject          *LPDIRECT3DRMOBJECT, **LPLPDIRECT3DRMOBJECT;
typedef struct IDirect3DRMObject2         *LPDIRECT3DRMOBJECT2, **LPLPDIRECT3DRMOBJECT2;
typedef struct IDirect3DRMDevice          *LPDIRECT3DRMDEVICE, **LPLPDIRECT3DRMDEVICE;
typedef struct IDirect3DRMDevice2         *LPDIRECT3DRMDEVICE2, **LPLPDIRECT3DRMDEVICE2;
typedef struct IDirect3DRMDevice3         *LPDIRECT3DRMDEVICE3, **LPLPDIRECT3DRMDEVICE3;
typedef struct IDirect3DRMViewport        *LPDIRECT3DRMVIEWPORT, **LPLPDIRECT3DRMVIEWPORT;
typedef struct IDirect3DRMViewport2       *LPDIRECT3DRMVIEWPORT2, **LPLPDIRECT3DRMVIEWPORT2;
typedef struct IDirect3DRMFrame           *LPDIRECT3DRMFRAME, **LPLPDIRECT3DRMFRAME;
typedef struct IDirect3DRMFrame2          *LPDIRECT3DRMFRAME2, **LPLPDIRECT3DRMFRAME2;
typedef struct IDirect3DRMFrame3          *LPDIRECT3DRMFRAME3, **LPLPDIRECT3DRMFRAME3;
typedef struct IDirect3DRMVisual          *LPDIRECT3DRMVISUAL, **LPLPDIRECT3DRMVISUAL;
typedef struct IDirect3DRMMesh            *LPDIRECT3DRMMESH, **LPLPDIRECT3DRMMESH;
typedef struct IDirect3DRMMeshBuilder     *LPDIRECT3DRMMESHBUILDER, **LPLPDIRECT3DRMMESHBUILDER;
typedef struct IDirect3DRMMeshBuilder2    *LPDIRECT3DRMMESHBUILDER2, **LPLPDIRECT3DRMMESHBUILDER2;
typedef struct IDirect3DRMMeshBuilder3    *LPDIRECT3DRMMESHBUILDER3, **LPLPDIRECT3DRMMESHBUILDER3;
typedef struct IDirect3DRMFace            *LPDIRECT3DRMFACE, **LPLPDIRECT3DRMFACE;
typedef struct IDirect3DRMFace2           *LPDIRECT3DRMFACE2, **LPLPDIRECT3DRMFACE2;
typedef struct IDirect3DRMLight           *LPDIRECT3DRMLIGHT, **LPLPDIRECT3DRMLIGHT;
typedef struct IDirect3DRMTexture         *LPDIRECT3DRMTEXTURE, **LPLPDIRECT3DRMTEXTURE;
typedef struct IDirect3DRMTexture2        *LPDIRECT3DRMTEXTURE2, **LPLPDIRECT3DRMTEXTURE2;
typedef struct IDirect3DRMTexture3        *LPDIRECT3DRMTEXTURE3, **LPLPDIRECT3DRMTEXTURE3;
typedef struct IDirect3DRMWrap            *LPDIRECT3DRMWRAP, **LPLPDIRECT3DRMWRAP;
typedef struct IDirect3DRMMaterial        *LPDIRECT3DRMMATERIAL, **LPLPDIRECT3DRMMATERIAL;
typedef struct IDirect3DRMMaterial2       *LPDIRECT3DRMMATERIAL2, **LPLPDIRECT3DRMMATERIAL2;
typedef struct IDirect3DRMAnimation       *LPDIRECT3DRMANIMATION, **LPLPDIRECT3DRMANIMATION;
typedef struct IDirect3DRMAnimation2      *LPDIRECT3DRMANIMATION2, **LPLPDIRECT3DRMANIMATION2;
typedef struct IDirect3DRMAnimationSet    *LPDIRECT3DRMANIMATIONSET, **LPLPDIRECT3DRMANIMATIONSET;
typedef struct IDirect3DRMAnimationSet2   *LPDIRECT3DRMANIMATIONSET2, **LPLPDIRECT3DRMANIMATIONSET2;
typedef struct IDirect3DRMUserVisual      *LPDIRECT3DRMUSERVISUAL, **LPLPDIRECT3DRMUSERVISUAL;
typedef struct IDirect3DRMShadow          *LPDIRECT3DRMSHADOW, **LPLPDIRECT3DRMSHADOW;
typedef struct IDirect3DRMShadow2         *LPDIRECT3DRMSHADOW2, **LPLPDIRECT3DRMSHADOW2;
typedef struct IDirect3DRMArray           *LPDIRECT3DRMARRAY, **LPLPDIRECT3DRMARRAY;
typedef struct IDirect3DRMObjectArray     *LPDIRECT3DRMOBJECTARRAY, **LPLPDIRECT3DRMOBJECTARRAY;
typedef struct IDirect3DRMDeviceArray     *LPDIRECT3DRMDEVICEARRAY, **LPLPDIRECT3DRMDEVICEARRAY;
typedef struct IDirect3DRMFaceArray       *LPDIRECT3DRMFACEARRAY, **LPLPDIRECT3DRMFACEARRAY;
typedef struct IDirect3DRMViewportArray   *LPDIRECT3DRMVIEWPORTARRAY, **LPLPDIRECT3DRMVIEWPORTARRAY;
typedef struct IDirect3DRMFrameArray      *LPDIRECT3DRMFRAMEARRAY, **LPLPDIRECT3DRMFRAMEARRAY;
typedef struct IDirect3DRMAnimationArray  *LPDIRECT3DRMANIMATIONARRAY, **LPLPDIRECT3DRMANIMATIONARRAY;
typedef struct IDirect3DRMVisualArray     *LPDIRECT3DRMVISUALARRAY, **LPLPDIRECT3DRMVISUALARRAY;
typedef struct IDirect3DRMPickedArray     *LPDIRECT3DRMPICKEDARRAY, **LPLPDIRECT3DRMPICKEDARRAY;
typedef struct IDirect3DRMPicked2Array    *LPDIRECT3DRMPICKED2ARRAY, **LPLPDIRECT3DRMPICKED2ARRAY;
typedef struct IDirect3DRMLightArray      *LPDIRECT3DRMLIGHTARRAY, **LPLPDIRECT3DRMLIGHTARRAY;
typedef struct IDirect3DRMProgressiveMesh *LPDIRECT3DRMPROGRESSIVEMESH, **LPLPDIRECT3DRMPROGRESSIVEMESH;
typedef struct IDirect3DRMClippedVisual   *LPDIRECT3DRMCLIPPEDVISUAL, **LPLPDIRECT3DRMCLIPPEDVISUAL;

/* ********************************************************************
   Types and structures
   ******************************************************************** */

typedef void (__cdecl *D3DRMOBJECTCALLBACK)(struct IDirect3DRMObject *obj, void *arg);
typedef void (__cdecl *D3DRMFRAMEMOVECALLBACK)(struct IDirect3DRMFrame *frame, void *ctx, D3DVALUE delta);
typedef void (__cdecl *D3DRMFRAME3MOVECALLBACK)(struct IDirect3DRMFrame3 *frame, void *ctx, D3DVALUE delta);
typedef void (__cdecl *D3DRMUPDATECALLBACK)(struct IDirect3DRMDevice *device, void *ctx, int count, D3DRECT *rects);
typedef void (__cdecl *D3DRMDEVICE3UPDATECALLBACK)(struct IDirect3DRMDevice3 *device, void *ctx,
        int count, D3DRECT *rects);
typedef int (__cdecl *D3DRMUSERVISUALCALLBACK)(struct IDirect3DRMUserVisual *visual, void *ctx,
        D3DRMUSERVISUALREASON reason, struct IDirect3DRMDevice *device, struct IDirect3DRMViewport *viewport);
typedef HRESULT (__cdecl *D3DRMLOADTEXTURECALLBACK)(char *tex_name, void *arg, struct IDirect3DRMTexture **texture);
typedef HRESULT (__cdecl *D3DRMLOADTEXTURE3CALLBACK)(char *tex_name, void *arg, struct IDirect3DRMTexture3 **texture);
typedef void (__cdecl *D3DRMLOADCALLBACK)(struct IDirect3DRMObject *object, REFIID objectguid, void *arg);
typedef HRESULT (__cdecl *D3DRMDOWNSAMPLECALLBACK)(struct IDirect3DRMTexture3 *texture, void *ctx,
        IDirectDrawSurface *src_surface, IDirectDrawSurface *dst_surface);
typedef HRESULT (__cdecl *D3DRMVALIDATIONCALLBACK)(struct IDirect3DRMTexture3 *texture, void *ctx,
        DWORD flags, DWORD rect_count, RECT *rects);

typedef struct _D3DRMPICKDESC
{
    ULONG     ulFaceIdx;
    LONG      lGroupIdx;
    D3DVECTOR vPosition;
} D3DRMPICKDESC, *LPD3DRMPICKDESC;

typedef struct _D3DRMPICKDESC2
{
    ULONG     ulFaceIdx;
    LONG      lGroupIdx;
    D3DVECTOR vPosition;
    D3DVALUE  tu;
    D3DVALUE  tv;
    D3DVECTOR dvNormal;
    D3DCOLOR  dcColor;
} D3DRMPICKDESC2, *LPD3DRMPICKDESC2;

/*****************************************************************************
 * IDirect3DRMObject interface
 */
#ifdef WINE_NO_UNICODE_MACROS
#undef GetClassName
#endif
#define INTERFACE IDirect3DRMObject
DECLARE_INTERFACE_(IDirect3DRMObject,IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject methods ***/
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetAppData)(THIS_ DWORD data) PURE;
    STDMETHOD_(DWORD, GetAppData)(THIS) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(GetClassName)(THIS_ DWORD *size, char *name) PURE;
};
#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)
/*** IUnknown methods ***/
#define IDirect3DRMObject_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DRMObject_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirect3DRMObject_Release(p)                   (p)->lpVtbl->Release(p)
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMObject_Clone(p,a,b,c)               (p)->lpVtbl->Clone(p,a,b,c)
#define IDirect3DRMObject_AddDestroyCallback(p,a,b)    (p)->lpVtbl->AddDestroyCallback(p,a,b)
#define IDirect3DRMObject_DeleteDestroyCallback(p,a,b) (p)->lpVtbl->DeleteDestroyCallback(p,a,b)
#define IDirect3DRMObject_SetAppData(p,a)              (p)->lpVtbl->SetAppData(p,a)
#define IDirect3DRMObject_GetAppData(p)                (p)->lpVtbl->GetAppData(p)
#define IDirect3DRMObject_SetName(p,a)                 (p)->lpVtbl->SetName(p,a)
#define IDirect3DRMObject_GetName(p,a,b)               (p)->lpVtbl->GetName(p,a,b)
#define IDirect3DRMObject_GetClassName(p,a,b)          (p)->lpVtbl->GetClassName(p,a,b)
#else
/*** IUnknown methods ***/
#define IDirect3DRMObject_QueryInterface(p,a,b)        (p)->QueryInterface(a,b)
#define IDirect3DRMObject_AddRef(p)                    (p)->AddRef()
#define IDirect3DRMObject_Release(p)                   (p)->Release()
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMObject_Clone(p,a,b,c)               (p)->Clone(a,b,c)
#define IDirect3DRMObject_AddDestroyCallback(p,a,b)    (p)->AddDestroyCallback(a,b)
#define IDirect3DRMObject_DeleteDestroyCallback(p,a,b) (p)->DeleteDestroyCallback(a,b)
#define IDirect3DRMObject_SetAppData(p,a)              (p)->SetAppData(a)
#define IDirect3DRMObject_GetAppData(p)                (p)->GetAppData()
#define IDirect3DRMObject_SetName(p,a)                 (p)->SetName(a)
#define IDirect3DRMObject_GetName(p,a,b)               (p)->GetName(a,b)
#define IDirect3DRMObject_GetClassName(p,a,b)          (p)->GetClassName(a,b)
#endif

/*****************************************************************************
 * IDirect3DRMObject2 interface
 */
#ifdef WINE_NO_UNICODE_MACROS
#undef GetClassName
#endif
#define INTERFACE IDirect3DRMObject2
DECLARE_INTERFACE_(IDirect3DRMObject2,IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject2 methods ***/
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(GetClientData)(THIS_ DWORD id, void **data) PURE;
    STDMETHOD(GetDirect3DRM)(THIS_ struct IDirect3DRM **d3drm) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(SetClientData)(THIS_ DWORD id, void *data, DWORD flags) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetAge)(THIS_ DWORD flags, DWORD *age) PURE;
};
#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)
/*** IUnknown methods ***/
#define IDirect3DRMObject2_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DRMObject2_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirect3DRMObject2_Release(p)                   (p)->lpVtbl->Release(p)
/*** IDirect3DRMObject2 methods ***/
#define IDirect3DRMObject2_AddDestroyCallback(p,a,b)    (p)->lpVtbl->AddDestroyCallback(p,a,b)
#define IDirect3DRMObject2_Clone(p,a,b,c)               (p)->lpVtbl->Clone(p,a,b,c)
#define IDirect3DRMObject2_DeleteDestroyCallback(p,a,b) (p)->lpVtbl->DeleteDestroyCallback(p,a,b)
#define IDirect3DRMObject2_GetClientData(p,a,b)         (p)->lpVtbl->SetClientData(p,a,b)
#define IDirect3DRMObject2_GetDirect3DRM(p,a)           (p)->lpVtbl->GetDirect3DRM(p,a)
#define IDirect3DRMObject2_GetName(p,a,b)               (p)->lpVtbl->GetName(p,a,b)
#define IDirect3DRMObject2_SetClientData(p,a,b,c)       (p)->lpVtbl->SetClientData(p,a,b,c)
#define IDirect3DRMObject2_SetName(p,a)                 (p)->lpVtbl->SetName(p,a)
#define IDirect3DRMObject2_GetAge(p,a,b)                (p)->lpVtbl->GetAge(p,a,b)
#else
/*** IUnknown methods ***/
#define IDirect3DRMObject2_QueryInterface(p,a,b)        (p)->QueryInterface(a,b)
#define IDirect3DRMObject2_AddRef(p)                    (p)->AddRef()
#define IDirect3DRMObject2_Release(p)                   (p)->Release()
/*** IDirect3DRMObject2 methods ***/
#define IDirect3DRMObject2_AddDestroyCallback(p,a,b)    (p)->AddDestroyCallback(a,b)
#define IDirect3DRMObject2_Clone(p,a,b,c)               (p)->Clone(a,b,c)
#define IDirect3DRMObject2_DeleteDestroyCallback(p,a,b) (p)->DeleteDestroyCallback(a,b)
#define IDirect3DRMObject2_GetClientData(p,a,b)         (p)->SetClientData(a,b)
#define IDirect3DRMObject2_GetDirect3DRM(p,a)           (p)->GetDirect3DRM(a)
#define IDirect3DRMObject2_GetName(p,a,b)               (p)->GetName(a,b)
#define IDirect3DRMObject2_SetClientData(p,a,b,c)       (p)->SetClientData(a,b,c)
#define IDirect3DRMObject2_SetName(p,a)                 (p)->SetName(a)
#define IDirect3DRMObject2_GetAge(p,a,b)                (p)->GetAge(a,b)
#endif

/*****************************************************************************
 * IDirect3DRMVisual interface
 */
#define INTERFACE IDirect3DRMVisual
DECLARE_INTERFACE_(IDirect3DRMVisual,IDirect3DRMObject)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject methods ***/
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetAppData)(THIS_ DWORD data) PURE;
    STDMETHOD_(DWORD, GetAppData)(THIS) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(GetClassName)(THIS_ DWORD *size, char *name) PURE;
};
#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)
/*** IUnknown methods ***/
#define IDirect3DRMVisual_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DRMVisual_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirect3DRMVisual_Release(p)                   (p)->lpVtbl->Release(p)
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMVisual_Clone(p,a,b,c)               (p)->lpVtbl->Clone(p,a,b,c)
#define IDirect3DRMVisual_AddDestroyCallback(p,a,b)    (p)->lpVtbl->AddDestroyCallback(p,a,b)
#define IDirect3DRMVisual_DeleteDestroyCallback(p,a,b) (p)->lpVtbl->DeleteDestroyCallback(p,a,b)
#define IDirect3DRMVisual_SetAppData(p,a)              (p)->lpVtbl->SetAppData(p,a)
#define IDirect3DRMVisual_GetAppData(p)                (p)->lpVtbl->GetAppData(p)
#define IDirect3DRMVisual_SetName(p,a)                 (p)->lpVtbl->SetName(p,a)
#define IDirect3DRMVisual_GetName(p,a,b)               (p)->lpVtbl->GetName(p,a,b)
#define IDirect3DRMVisual_GetClassName(p,a,b)          (p)->lpVtbl->GetClassName(p,a,b)
#else
/*** IUnknown methods ***/
#define IDirect3DRMVisual_QueryInterface(p,a,b)        (p)->QueryInterface(a,b)
#define IDirect3DRMVisual_AddRef(p)                    (p)->AddRef()
#define IDirect3DRMVisual_Release(p)                   (p)->Release()
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMVisual_Clone(p,a,b,c)               (p)->Clone(a,b,c)
#define IDirect3DRMVisual_AddDestroyCallback(p,a,b)    (p)->AddDestroyCallback(a,b)
#define IDirect3DRMVisual_DeleteDestroyCallback(p,a,b) (p)->DeleteDestroyCallback(a,b)
#define IDirect3DRMVisual_SetAppData(p,a)              (p)->SetAppData(a)
#define IDirect3DRMVisual_GetAppData(p)                (p)->GetAppData()
#define IDirect3DRMVisual_SetName(p,a)                 (p)->SetName(a)
#define IDirect3DRMVisual_GetName(p,a,b)               (p)->GetName(a,b)
#define IDirect3DRMVisual_GetClassName(p,a,b)          (p)->GetClassName(a,b)
#endif

/*****************************************************************************
 * IDirect3DRMDevice interface
 */
#ifdef WINE_NO_UNICODE_MACROS
#undef GetClassName
#endif
#define INTERFACE IDirect3DRMDevice
DECLARE_INTERFACE_(IDirect3DRMDevice,IDirect3DRMObject)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject methods ***/
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetAppData)(THIS_ DWORD data) PURE;
    STDMETHOD_(DWORD, GetAppData)(THIS) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(GetClassName)(THIS_ DWORD *size, char *name) PURE;
    /*** IDirect3DRMDevice methods ***/
    STDMETHOD(Init)(THIS_ ULONG width, ULONG height) PURE;
    STDMETHOD(InitFromD3D)(THIS_ IDirect3D *d3d, IDirect3DDevice *d3d_device) PURE;
    STDMETHOD(InitFromClipper)(THIS_ IDirectDrawClipper *clipper, GUID *guid, int width, int height) PURE;
    STDMETHOD(Update)(THIS) PURE;
    STDMETHOD(AddUpdateCallback)(THIS_ D3DRMUPDATECALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteUpdateCallback)(THIS_ D3DRMUPDATECALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetBufferCount)(THIS_ DWORD) PURE;
    STDMETHOD_(DWORD, GetBufferCount)(THIS) PURE;
    STDMETHOD(SetDither)(THIS_ WINBOOL) PURE;
    STDMETHOD(SetShades)(THIS_ DWORD) PURE;
    STDMETHOD(SetQuality)(THIS_ D3DRMRENDERQUALITY) PURE;
    STDMETHOD(SetTextureQuality)(THIS_ D3DRMTEXTUREQUALITY) PURE;
    STDMETHOD(GetViewports)(THIS_ struct IDirect3DRMViewportArray **array) PURE;
    STDMETHOD_(WINBOOL, GetDither)(THIS) PURE;
    STDMETHOD_(DWORD, GetShades)(THIS) PURE;
    STDMETHOD_(DWORD, GetHeight)(THIS) PURE;
    STDMETHOD_(DWORD, GetWidth)(THIS) PURE;
    STDMETHOD_(DWORD, GetTrianglesDrawn)(THIS) PURE;
    STDMETHOD_(DWORD, GetWireframeOptions)(THIS) PURE;
    STDMETHOD_(D3DRMRENDERQUALITY, GetQuality)(THIS) PURE;
    STDMETHOD_(D3DCOLORMODEL, GetColorModel)(THIS) PURE;
    STDMETHOD_(D3DRMTEXTUREQUALITY, GetTextureQuality)(THIS) PURE;
    STDMETHOD(GetDirect3DDevice)(THIS_ IDirect3DDevice **d3d_device) PURE;
};
#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)
/*** IUnknown methods ***/
#define IDirect3DRMDevice_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DRMDevice_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirect3DRMDevice_Release(p)                   (p)->lpVtbl->Release(p)
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMDevice_Clone(p,a,b,c)               (p)->lpVtbl->Clone(p,a,b,c)
#define IDirect3DRMDevice_AddDestroyCallback(p,a,b)    (p)->lpVtbl->AddDestroyCallback(p,a,b)
#define IDirect3DRMDevice_DeleteDestroyCallback(p,a,b) (p)->lpVtbl->DeleteDestroyCallback(p,a,b)
#define IDirect3DRMDevice_SetAppData(p,a)              (p)->lpVtbl->SetAppData(p,a)
#define IDirect3DRMDevice_GetAppData(p)                (p)->lpVtbl->GetAppData(p)
#define IDirect3DRMDevice_SetName(p,a)                 (p)->lpVtbl->SetName(p,a)
#define IDirect3DRMDevice_GetName(p,a,b)               (p)->lpVtbl->GetName(p,a,b)
#define IDirect3DRMDevice_GetClassName(p,a,b)          (p)->lpVtbl->GetClassName(p,a,b)
/*** IDirect3DRMDevice methods ***/
#define IDirect3DRMDevice_Init(p,a,b)                  (p)->lpVtbl->Init(p,a,b)
#define IDirect3DRMDevice_InitFromD3D(p,a,b)           (p)->lpVtbl->InitFromD3D(p,a,b)
#define IDirect3DRMDevice_InitFromClipper(p,a,b,c,d)   (p)->lpVtbl->InitFromClipper(p,a,b,c,d)
#define IDirect3DRMDevice_Update(p)                    (p)->lpVtbl->Update(p)
#define IDirect3DRMDevice_AddUpdateCallback(p,a,b)     (p)->lpVtbl->AddUpdateCallback(p,a,b)
#define IDirect3DRMDevice_DeleteUpdateCallback(p,a,b)  (p)->lpVtbl->DeleteUpdateCallback(p,a,b)
#define IDirect3DRMDevice_SetBufferCount(p,a)          (p)->lpVtbl->SetBufferCount(p,a)
#define IDirect3DRMDevice_GetBufferCount(p)            (p)->lpVtbl->GetBufferCount(p)
#define IDirect3DRMDevice_SetDither(p,a)               (p)->lpVtbl->SetDither(p,a)
#define IDirect3DRMDevice_SetShades(p,a)               (p)->lpVtbl->SetShades(p,a)
#define IDirect3DRMDevice_SetQuality(p,a)              (p)->lpVtbl->SetQuality(p,a)
#define IDirect3DRMDevice_SetTextureQuality(p,a)       (p)->lpVtbl->SetTextureQuality(p,a)
#define IDirect3DRMDevice_GetViewports(p,a)            (p)->lpVtbl->GetViewports(p,a)
#define IDirect3DRMDevice_GetDither(p)                 (p)->lpVtbl->GetDither(p)
#define IDirect3DRMDevice_GetShades(p)                 (p)->lpVtbl->GetShades(p)
#define IDirect3DRMDevice_GetHeight(p)                 (p)->lpVtbl->GetHeight(p)
#define IDirect3DRMDevice_GetWidth(p)                  (p)->lpVtbl->GetWidth(p)
#define IDirect3DRMDevice_GetTrianglesDrawn(p)         (p)->lpVtbl->GetTrianglesDrawn(p)
#define IDirect3DRMDevice_GetWireframeOptions(p)       (p)->lpVtbl->GetWireframeOptions(p)
#define IDirect3DRMDevice_GetQuality(p)                (p)->lpVtbl->GetQuality(p)
#define IDirect3DRMDevice_GetColorModel(p)             (p)->lpVtbl->GetColorModel(p)
#define IDirect3DRMDevice_GetTextureQuality(p)         (p)->lpVtbl->GetTextureQuality(p)
#define IDirect3DRMDevice_GetDirect3DDevice(p,a)       (p)->lpVtbl->GetDirect3DDevice(p,a)
#else
/*** IUnknown methods ***/
#define IDirect3DRMDevice_QueryInterface(p,a,b)        (p)->QueryInterface(a,b)
#define IDirect3DRMDevice_AddRef(p)                    (p)->AddRef()
#define IDirect3DRMDevice_Release(p)                   (p)->Release()
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMDevice_Clone(p,a,b,c)               (p)->Clone(a,b,c)
#define IDirect3DRMDevice_AddDestroyCallback(p,a,b)    (p)->AddDestroyCallback(a,b)
#define IDirect3DRMDevice_DeleteDestroyCallback(p,a,b) (p)->DeleteDestroyCallback(a,b)
#define IDirect3DRMDevice_SetAppData(p,a)              (p)->SetAppData(a)
#define IDirect3DRMDevice_GetAppData(p)                (p)->GetAppData()
#define IDirect3DRMDevice_SetName(p,a)                 (p)->SetName(a)
#define IDirect3DRMDevice_GetName(p,a,b)               (p)->GetName(a,b)
#define IDirect3DRMDevice_GetClassName(p,a,b)          (p)->GetClassName(a,b)
/*** IDirect3DRMDevice methods ***/
#define IDirect3DRMDevice_Init(p,a,b)                  (p)->Init(a,b)
#define IDirect3DRMDevice_InitFromD3D(p,a,b)           (p)->InitFromD3D(a,b)
#define IDirect3DRMDevice_InitFromClipper(p,a,b,c,d)   (p)->InitFromClipper(a,b,c,d)
#define IDirect3DRMDevice_Update(p)                    (p)->Update()
#define IDirect3DRMDevice_AddUpdateCallback(p,a,b)     (p)->AddUpdateCallback(a,b)
#define IDirect3DRMDevice_DeleteUpdateCallback(p,a,b)  (p)->DeleteUpdateCallback(a,b)
#define IDirect3DRMDevice_SetBufferCount(p,a)          (p)->SetBufferCount(a)
#define IDirect3DRMDevice_GetBufferCount(p)            (p)->GetBufferCount()
#define IDirect3DRMDevice_SetDither(p,a)               (p)->SetDither(a)
#define IDirect3DRMDevice_SetShades(p,a)               (p)->SetShades(a)
#define IDirect3DRMDevice_SetQuality(p,a)              (p)->SetQuality(a)
#define IDirect3DRMDevice_SetTextureQuality(p,a)       (p)->SetTextureQuality(a)
#define IDirect3DRMDevice_GetViewports(p,a)            (p)->GetViewports(a)
#define IDirect3DRMDevice_GetDither(p)                 (p)->GetDither()
#define IDirect3DRMDevice_GetShades(p)                 (p)->GetShades()
#define IDirect3DRMDevice_GetHeight(p)                 (p)->GetHeight()
#define IDirect3DRMDevice_GetWidth(p)                  (p)->GetWidth()
#define IDirect3DRMDevice_GetTrianglesDrawn(p)         (p)->GetTrianglesDrawn()
#define IDirect3DRMDevice_GetWireframeOptions(p)       (p)->GetWireframeOptions()
#define IDirect3DRMDevice_GetQuality(p)                (p)->GetQuality()
#define IDirect3DRMDevice_GetColorModel(p)             (p)->GetColorModel()
#define IDirect3DRMDevice_GetTextureQuality(p)         (p)->GetTextureQuality()
#define IDirect3DRMDevice_GetDirect3DDevice(p,a)       (p)->GetDirect3DDevice(a)
#endif

/*****************************************************************************
 * IDirect3DRMDevice2 interface
 */
#ifdef WINE_NO_UNICODE_MACROS
#undef GetClassName
#endif
#define INTERFACE IDirect3DRMDevice2
DECLARE_INTERFACE_(IDirect3DRMDevice2,IDirect3DRMDevice)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject methods ***/
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetAppData)(THIS_ DWORD data) PURE;
    STDMETHOD_(DWORD, GetAppData)(THIS) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(GetClassName)(THIS_ DWORD *size, char *name) PURE;
    /*** IDirect3DRMDevice methods ***/
    STDMETHOD(Init)(THIS_ ULONG width, ULONG height) PURE;
    STDMETHOD(InitFromD3D)(THIS_ IDirect3D *d3d, IDirect3DDevice *d3d_device) PURE;
    STDMETHOD(InitFromClipper)(THIS_ IDirectDrawClipper *clipper, GUID *guid, int width, int height) PURE;
    STDMETHOD(Update)(THIS) PURE;
    STDMETHOD(AddUpdateCallback)(THIS_ D3DRMUPDATECALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteUpdateCallback)(THIS_ D3DRMUPDATECALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetBufferCount)(THIS_ DWORD) PURE;
    STDMETHOD_(DWORD, GetBufferCount)(THIS) PURE;
    STDMETHOD(SetDither)(THIS_ WINBOOL) PURE;
    STDMETHOD(SetShades)(THIS_ DWORD) PURE;
    STDMETHOD(SetQuality)(THIS_ D3DRMRENDERQUALITY) PURE;
    STDMETHOD(SetTextureQuality)(THIS_ D3DRMTEXTUREQUALITY) PURE;
    STDMETHOD(GetViewports)(THIS_ struct IDirect3DRMViewportArray **array) PURE;
    STDMETHOD_(WINBOOL, GetDither)(THIS) PURE;
    STDMETHOD_(DWORD, GetShades)(THIS) PURE;
    STDMETHOD_(DWORD, GetHeight)(THIS) PURE;
    STDMETHOD_(DWORD, GetWidth)(THIS) PURE;
    STDMETHOD_(DWORD, GetTrianglesDrawn)(THIS) PURE;
    STDMETHOD_(DWORD, GetWireframeOptions)(THIS) PURE;
    STDMETHOD_(D3DRMRENDERQUALITY, GetQuality)(THIS) PURE;
    STDMETHOD_(D3DCOLORMODEL, GetColorModel)(THIS) PURE;
    STDMETHOD_(D3DRMTEXTUREQUALITY, GetTextureQuality)(THIS) PURE;
    STDMETHOD(GetDirect3DDevice)(THIS_ IDirect3DDevice **d3d_device) PURE;
    /*** IDirect3DRMDevice2 methods ***/
    STDMETHOD(InitFromD3D2)(THIS_ IDirect3D2 *d3d, IDirect3DDevice2 *device) PURE;
    STDMETHOD(InitFromSurface)(THIS_ GUID *guid, IDirectDraw *ddraw, IDirectDrawSurface *surface) PURE;
    STDMETHOD(SetRenderMode)(THIS_ DWORD flags) PURE;
    STDMETHOD_(DWORD, GetRenderMode)(THIS) PURE;
    STDMETHOD(GetDirect3DDevice2)(THIS_ IDirect3DDevice2 **device) PURE;
};
#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)
/*** IUnknown methods ***/
#define IDirect3DRMDevice2_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DRMDevice2_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirect3DRMDevice2_Release(p)                   (p)->lpVtbl->Release(p)
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMDevice2_Clone(p,a,b,c)               (p)->lpVtbl->Clone(p,a,b,c)
#define IDirect3DRMDevice2_AddDestroyCallback(p,a,b)    (p)->lpVtbl->AddDestroyCallback(p,a,b)
#define IDirect3DRMDevice2_DeleteDestroyCallback(p,a,b) (p)->lpVtbl->DeleteDestroyCallback(p,a,b)
#define IDirect3DRMDevice2_SetAppData(p,a)              (p)->lpVtbl->SetAppData(p,a)
#define IDirect3DRMDevice2_GetAppData(p)                (p)->lpVtbl->GetAppData(p)
#define IDirect3DRMDevice2_SetName(p,a)                 (p)->lpVtbl->SetName(p,a)
#define IDirect3DRMDevice2_GetName(p,a,b)               (p)->lpVtbl->GetName(p,a,b)
#define IDirect3DRMDevice2_GetClassName(p,a,b)          (p)->lpVtbl->GetClassName(p,a,b)
/*** IDirect3DRMDevice methods ***/
#define IDirect3DRMDevice2_Init(p,a,b)                  (p)->lpVtbl->Init(p,a,b)
#define IDirect3DRMDevice2_InitFromD3D(p,a,b)           (p)->lpVtbl->InitFromD3D(p,a,b)
#define IDirect3DRMDevice2_InitFromClipper(p,a,b,c,d)   (p)->lpVtbl->InitFromClipper(p,a,b,c,d)
#define IDirect3DRMDevice2_Update(p)                    (p)->lpVtbl->Update(p)
#define IDirect3DRMDevice2_AddUpdateCallback(p,a,b)     (p)->lpVtbl->AddUpdateCallback(p,a,b)
#define IDirect3DRMDevice2_DeleteUpdateCallback(p,a,b)  (p)->lpVtbl->DeleteUpdateCallback(p,a,b)
#define IDirect3DRMDevice2_SetBufferCount(p,a)          (p)->lpVtbl->SetBufferCount(p,a)
#define IDirect3DRMDevice2_GetBufferCount(p)            (p)->lpVtbl->GetBufferCount(p)
#define IDirect3DRMDevice2_SetDither(p,a)               (p)->lpVtbl->SetDither(p,a)
#define IDirect3DRMDevice2_SetShades(p,a)               (p)->lpVtbl->SetShades(p,a)
#define IDirect3DRMDevice2_SetQuality(p,a)              (p)->lpVtbl->SetQuality(p,a)
#define IDirect3DRMDevice2_SetTextureQuality(p,a)       (p)->lpVtbl->SetTextureQuality(p,a)
#define IDirect3DRMDevice2_GetViewports(p,a)            (p)->lpVtbl->GetViewports(p,a)
#define IDirect3DRMDevice2_GetDither(p)                 (p)->lpVtbl->GetDither(p)
#define IDirect3DRMDevice2_GetShades(p)                 (p)->lpVtbl->GetShades(p)
#define IDirect3DRMDevice2_GetHeight(p)                 (p)->lpVtbl->GetHeight(p)
#define IDirect3DRMDevice2_GetWidth(p)                  (p)->lpVtbl->GetWidth(p)
#define IDirect3DRMDevice2_GetTrianglesDrawn(p)         (p)->lpVtbl->GetTrianglesDrawn(p)
#define IDirect3DRMDevice2_GetWireframeOptions(p)       (p)->lpVtbl->GetWireframeOptions(p)
#define IDirect3DRMDevice2_GetQuality(p)                (p)->lpVtbl->GetQuality(p)
#define IDirect3DRMDevice2_GetColorModel(p)             (p)->lpVtbl->GetColorModel(p)
#define IDirect3DRMDevice2_GetTextureQuality(p)         (p)->lpVtbl->GetTextureQuality(p)
#define IDirect3DRMDevice2_GetDirect3DDevice(p,a)       (p)->lpVtbl->GetDirect3DDevice(p,a)
/*** IDirect3DRMDevice2 methods ***/
#define IDirect3DRMDevice2_InitFromD3D2(p,a,b)          (p)->lpVtbl->InitFromD3D2(p,a,b)
#define IDirect3DRMDevice2_InitFromSurface(p,a,b,c)     (p)->lpVtbl->InitFromSurface(p,a,b,c)
#define IDirect3DRMDevice2_SetRenderMode(p,a)           (p)->lpVtbl->SetRenderMode(p,a)
#define IDirect3DRMDevice2_GetRenderMode(p)             (p)->lpVtbl->GetRenderMode(p)
#define IDirect3DRMDevice2_GetDirect3DDevice2(p,a)      (p)->lpVtbl->GetDirect3DDevice2(p,a)
#else
/*** IUnknown methods ***/
#define IDirect3DRMDevice2_QueryInterface(p,a,b)        (p)->QueryInterface(a,b)
#define IDirect3DRMDevice2_AddRef(p)                    (p)->AddRef()
#define IDirect3DRMDevice2_Release(p)                   (p)->Release()
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMDevice2_Clone(p,a,b,c)               (p)->Clone(a,b,c)
#define IDirect3DRMDevice2_AddDestroyCallback(p,a,b)    (p)->AddDestroyCallback(a,b)
#define IDirect3DRMDevice2_DeleteDestroyCallback(p,a,b) (p)->DeleteDestroyCallback(a,b)
#define IDirect3DRMDevice2_SetAppData(p,a)              (p)->SetAppData(a)
#define IDirect3DRMDevice2_GetAppData(p)                (p)->GetAppData()
#define IDirect3DRMDevice2_SetName(p,a)                 (p)->SetName(a)
#define IDirect3DRMDevice2_GetName(p,a,b)               (p)->GetName(a,b)
#define IDirect3DRMDevice2_GetClassName(p,a,b)          (p)->GetClassName(a,b)
/*** IDirect3DRMDevice methods ***/
#define IDirect3DRMDevice2_Init(p,a,b)                  (p)->Init(a,b)
#define IDirect3DRMDevice2_InitFromD3D(p,a,b)           (p)->InitFromD3D(a,b)
#define IDirect3DRMDevice2_InitFromClipper(p,a,b,c,d)   (p)->InitFromClipper(a,b,c,d)
#define IDirect3DRMDevice2_Update(p)                    (p)->Update()
#define IDirect3DRMDevice2_AddUpdateCallback(p,a,b)     (p)->AddUpdateCallback(a,b)
#define IDirect3DRMDevice2_DeleteUpdateCallback(p,a,b)  (p)->DeleteUpdateCallback(a,b)
#define IDirect3DRMDevice2_SetBufferCount(p,a)          (p)->SetBufferCount(a)
#define IDirect3DRMDevice2_GetBufferCount(p)            (p)->GetBufferCount()
#define IDirect3DRMDevice2_SetDither(p,a)               (p)->SetDither(a)
#define IDirect3DRMDevice2_SetShades(p,a)               (p)->SetShades(a)
#define IDirect3DRMDevice2_SetQuality(p,a)              (p)->SetQuality(a)
#define IDirect3DRMDevice2_SetTextureQuality(p,a)       (p)->SetTextureQuality(a)
#define IDirect3DRMDevice2_GetViewports(p,a)            (p)->GetViewports(a)
#define IDirect3DRMDevice2_GetDither(p)                 (p)->GetDither()
#define IDirect3DRMDevice2_GetShades(p)                 (p)->GetShades()
#define IDirect3DRMDevice2_GetHeight(p)                 (p)->GetHeight()
#define IDirect3DRMDevice2_GetWidth(p)                  (p)->GetWidth()
#define IDirect3DRMDevice2_GetTrianglesDrawn(p)         (p)->GetTrianglesDrawn()
#define IDirect3DRMDevice2_GetWireframeOptions(p)       (p)->GetWireframeOptions()
#define IDirect3DRMDevice2_GetQuality(p)                (p)->GetQuality()
#define IDirect3DRMDevice2_GetColorModel(p)             (p)->GetColorModel()
#define IDirect3DRMDevice2_GetTextureQuality(p)         (p)->GetTextureQuality()
#define IDirect3DRMDevice2_GetDirect3DDevice(p,a)       (p)->GetDirect3DDevice(a)
/*** IDirect3DRMDevice2 methods ***/
#define IDirect3DRMDevice2_InitFromD3D2(p,a,b)          (p)->InitFromD3D2(a,b)
#define IDirect3DRMDevice2_InitFromSurface(p,a,b,c)     (p)->InitFromSurface(a,b,c)
#define IDirect3DRMDevice2_SetRenderMode(p,a)           (p)->SetRenderMode(a)
#define IDirect3DRMDevice2_GetRenderMode(p)             (p)->GetRenderMode()
#define IDirect3DRMDevice2_GetDirect3DDevice2(p,a)      (p)->GetDirect3DDevice2(a)
#endif

/*****************************************************************************
 * IDirect3DRMDevice3 interface
 */
#ifdef WINE_NO_UNICODE_MACROS
#undef GetClassName
#endif
#define INTERFACE IDirect3DRMDevice3
DECLARE_INTERFACE_(IDirect3DRMDevice3,IDirect3DRMObject)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject methods ***/
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetAppData)(THIS_ DWORD data) PURE;
    STDMETHOD_(DWORD, GetAppData)(THIS) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(GetClassName)(THIS_ DWORD *size, char *name) PURE;
    /*** IDirect3DRMDevice methods ***/
    STDMETHOD(Init)(THIS_ ULONG width, ULONG height) PURE;
    STDMETHOD(InitFromD3D)(THIS_ IDirect3D *d3d, IDirect3DDevice *d3d_device) PURE;
    STDMETHOD(InitFromClipper)(THIS_ IDirectDrawClipper *clipper, GUID *guid, int width, int height) PURE;
    STDMETHOD(Update)(THIS) PURE;
    STDMETHOD(AddUpdateCallback)(THIS_ D3DRMUPDATECALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteUpdateCallback)(THIS_ D3DRMUPDATECALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetBufferCount)(THIS_ DWORD) PURE;
    STDMETHOD_(DWORD, GetBufferCount)(THIS) PURE;
    STDMETHOD(SetDither)(THIS_ WINBOOL) PURE;
    STDMETHOD(SetShades)(THIS_ DWORD) PURE;
    STDMETHOD(SetQuality)(THIS_ D3DRMRENDERQUALITY) PURE;
    STDMETHOD(SetTextureQuality)(THIS_ D3DRMTEXTUREQUALITY) PURE;
    STDMETHOD(GetViewports)(THIS_ struct IDirect3DRMViewportArray **array) PURE;
    STDMETHOD_(WINBOOL, GetDither)(THIS) PURE;
    STDMETHOD_(DWORD, GetShades)(THIS) PURE;
    STDMETHOD_(DWORD, GetHeight)(THIS) PURE;
    STDMETHOD_(DWORD, GetWidth)(THIS) PURE;
    STDMETHOD_(DWORD, GetTrianglesDrawn)(THIS) PURE;
    STDMETHOD_(DWORD, GetWireframeOptions)(THIS) PURE;
    STDMETHOD_(D3DRMRENDERQUALITY, GetQuality)(THIS) PURE;
    STDMETHOD_(D3DCOLORMODEL, GetColorModel)(THIS) PURE;
    STDMETHOD_(D3DRMTEXTUREQUALITY, GetTextureQuality)(THIS) PURE;
    STDMETHOD(GetDirect3DDevice)(THIS_ IDirect3DDevice **d3d_device) PURE;
    /*** IDirect3DRMDevice2 methods ***/
    STDMETHOD(InitFromD3D2)(THIS_ IDirect3D2 *d3d, IDirect3DDevice2 *device) PURE;
    STDMETHOD(InitFromSurface)(THIS_ GUID *guid, IDirectDraw *ddraw, IDirectDrawSurface *surface) PURE;
    STDMETHOD(SetRenderMode)(THIS_ DWORD flags) PURE;
    STDMETHOD_(DWORD, GetRenderMode)(THIS) PURE;
    STDMETHOD(GetDirect3DDevice2)(THIS_ IDirect3DDevice2 **device) PURE;
    /*** IDirect3DRMDevice3 methods ***/
    STDMETHOD(FindPreferredTextureFormat)(THIS_ DWORD BitDepths, DWORD flags, DDPIXELFORMAT *format) PURE;
    STDMETHOD(RenderStateChange)(THIS_ D3DRENDERSTATETYPE drsType, DWORD val, DWORD flags) PURE;
    STDMETHOD(LightStateChange)(THIS_ D3DLIGHTSTATETYPE drsType, DWORD val, DWORD flags) PURE;
    STDMETHOD(GetStateChangeOptions)(THIS_ DWORD state_class, DWORD state_idx, DWORD *flags) PURE;
    STDMETHOD(SetStateChangeOptions)(THIS_ DWORD StateClass, DWORD StateNum, DWORD flags) PURE;
};
#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)
/*** IUnknown methods ***/
#define IDirect3DRMDevice3_QueryInterface(p,a,b)               (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DRMDevice3_AddRef(p)                           (p)->lpVtbl->AddRef(p)
#define IDirect3DRMDevice3_Release(p)                          (p)->lpVtbl->Release(p)
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMDevice3_Clone(p,a,b,c)                      (p)->lpVtbl->Clone(p,a,b,c)
#define IDirect3DRMDevice3_AddDestroyCallback(p,a,b)           (p)->lpVtbl->AddDestroyCallback(p,a,b)
#define IDirect3DRMDevice3_DeleteDestroyCallback(p,a,b)        (p)->lpVtbl->DeleteDestroyCallback(p,a,b)
#define IDirect3DRMDevice3_SetAppData(p,a)                     (p)->lpVtbl->SetAppData(p,a)
#define IDirect3DRMDevice3_GetAppData(p)                       (p)->lpVtbl->GetAppData(p)
#define IDirect3DRMDevice3_SetName(p,a)                        (p)->lpVtbl->SetName(p,a)
#define IDirect3DRMDevice3_GetName(p,a,b)                      (p)->lpVtbl->GetName(p,a,b)
#define IDirect3DRMDevice3_GetClassName(p,a,b)                 (p)->lpVtbl->GetClassName(p,a,b)
/*** IDirect3DRMDevice methods ***/
#define IDirect3DRMDevice3_Init(p,a,b)                         (p)->lpVtbl->Init(p,a,b)
#define IDirect3DRMDevice3_InitFromD3D(p,a,b)                  (p)->lpVtbl->InitFromD3D(p,a,b)
#define IDirect3DRMDevice3_InitFromClipper(p,a,b,c,d)          (p)->lpVtbl->InitFromClipper(p,a,b,c,d)
#define IDirect3DRMDevice3_Update(p)                           (p)->lpVtbl->Update(p)
#define IDirect3DRMDevice3_AddUpdateCallback(p,a,b)            (p)->lpVtbl->AddUpdateCallback(p,a,b)
#define IDirect3DRMDevice3_DeleteUpdateCallback(p,a,b)         (p)->lpVtbl->DeleteUpdateCallback(p,a,b)
#define IDirect3DRMDevice3_SetBufferCount(p,a)                 (p)->lpVtbl->SetBufferCount(p,a)
#define IDirect3DRMDevice3_GetBufferCount(p)                   (p)->lpVtbl->GetBufferCount(p)
#define IDirect3DRMDevice3_SetDither(p,a)                      (p)->lpVtbl->SetDither(p,a)
#define IDirect3DRMDevice3_SetShades(p,a)                      (p)->lpVtbl->SetShades(p,a)
#define IDirect3DRMDevice3_SetQuality(p,a)                     (p)->lpVtbl->SetQuality(p,a)
#define IDirect3DRMDevice3_SetTextureQuality(p,a)              (p)->lpVtbl->SetTextureQuality(p,a)
#define IDirect3DRMDevice3_GetViewports(p,a)                   (p)->lpVtbl->GetViewports(p,a)
#define IDirect3DRMDevice3_GetDither(p)                        (p)->lpVtbl->GetDither(p)
#define IDirect3DRMDevice3_GetShades(p)                        (p)->lpVtbl->GetShades(p)
#define IDirect3DRMDevice3_GetHeight(p)                        (p)->lpVtbl->GetHeight(p)
#define IDirect3DRMDevice3_GetWidth(p)                         (p)->lpVtbl->GetWidth(p)
#define IDirect3DRMDevice3_GetTrianglesDrawn(p)                (p)->lpVtbl->GetTrianglesDrawn(p)
#define IDirect3DRMDevice3_GetWireframeOptions(p)              (p)->lpVtbl->GetWireframeOptions(p)
#define IDirect3DRMDevice3_GetQuality(p)                       (p)->lpVtbl->GetQuality(p)
#define IDirect3DRMDevice3_GetColorModel(p)                    (p)->lpVtbl->GetColorModel(p)
#define IDirect3DRMDevice3_GetTextureQuality(p)                (p)->lpVtbl->GetTextureQuality(p)
#define IDirect3DRMDevice3_GetDirect3DDevice(p,a)              (p)->lpVtbl->GetDirect3DDevice(p,a)
/*** IDirect3DRMDevice2 methods ***/
#define IDirect3DRMDevice3_InitFromD3D2(p,a,b)                 (p)->lpVtbl->InitFromD3D2(p,a,b)
#define IDirect3DRMDevice3_InitFromSurface(p,a,b,c)            (p)->lpVtbl->InitFromSurface(p,a,b,c)
#define IDirect3DRMDevice3_SetRenderMode(p,a)                  (p)->lpVtbl->SetRenderMode(p,a)
#define IDirect3DRMDevice3_GetRenderMode(p)                    (p)->lpVtbl->GetRenderMode(p)
#define IDirect3DRMDevice3_GetDirect3DDevice2(p,a)             (p)->lpVtbl->GetDirect3DDevice2(p,a)
/*** IDirect3DRMDevice3 methods ***/
#define IDirect3DRMDevice3_FindPreferredTextureFormat(p,a,b,c) (p)->lpVtbl->FindPreferredTextureFormat(p,a,b,c)
#define IDirect3DRMDevice3_RenderStateChange(p,a,b,c)          (p)->lpVtbl->RenderStateChange(p,a,b,c)
#define IDirect3DRMDevice3_LightStateChange(p,a,b,c)           (p)->lpVtbl->LightStateChange(p,a,b,c)
#define IDirect3DRMDevice3_GetStateChangeOptions(p,a,b,c)      (p)->lpVtbl->GetStateChangeOptions(p,a,b,c)
#define IDirect3DRMDevice3_SetStateChangeOptions(p,a,b,c)      (p)->lpVtbl->SetStateChangeOptions(p,a,b,c)
#else
/*** IUnknown methods ***/
#define IDirect3DRMDevice3_QueryInterface(p,a,b)               (p)->QueryInterface(a,b)
#define IDirect3DRMDevice3_AddRef(p)                           (p)->AddRef()
#define IDirect3DRMDevice3_Release(p)                          (p)->Release()
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMDevice3_Clone(p,a,b,c)                      (p)->Clone(a,b,c)
#define IDirect3DRMDevice3_AddDestroyCallback(p,a,b)           (p)->AddDestroyCallback(a,b)
#define IDirect3DRMDevice3_DeleteDestroyCallback(p,a,b)        (p)->DeleteDestroyCallback(a,b)
#define IDirect3DRMDevice3_SetAppData(p,a)                     (p)->SetAppData(a)
#define IDirect3DRMDevice3_GetAppData(p)                       (p)->GetAppData()
#define IDirect3DRMDevice3_SetName(p,a)                        (p)->SetName(a)
#define IDirect3DRMDevice3_GetName(p,a,b)                      (p)->GetName(a,b)
#define IDirect3DRMDevice3_GetClassName(p,a,b)                 (p)->GetClassName(a,b)
/*** IDirect3DRMDevice methods ***/
#define IDirect3DRMDevice3_Init(p,a,b)                         (p)->Init(a,b)
#define IDirect3DRMDevice3_InitFromD3D(p,a,b)                  (p)->InitFromD3D(a,b)
#define IDirect3DRMDevice3_InitFromClipper(p,a,b,c,d)          (p)->InitFromClipper(a,b,c,d)
#define IDirect3DRMDevice3_Update(p)                           (p)->Update()
#define IDirect3DRMDevice3_AddUpdateCallback(p,a,b)            (p)->AddUpdateCallback(a,b)
#define IDirect3DRMDevice3_DeleteUpdateCallback(p,a,b)         (p)->DeleteUpdateCallback(a,b)
#define IDirect3DRMDevice3_SetBufferCount(p,a)                 (p)->SetBufferCount(a)
#define IDirect3DRMDevice3_GetBufferCount(p)                   (p)->GetBufferCount()
#define IDirect3DRMDevice3_SetDither(p,a)                      (p)->SetDither(a)
#define IDirect3DRMDevice3_SetShades(p,a)                      (p)->SetShades(a)
#define IDirect3DRMDevice3_SetQuality(p,a)                     (p)->SetQuality(a)
#define IDirect3DRMDevice3_SetTextureQuality(p,a)              (p)->SetTextureQuality(a)
#define IDirect3DRMDevice3_GetViewports(p,a)                   (p)->GetViewports(a)
#define IDirect3DRMDevice3_GetDither(p)                        (p)->GetDither()
#define IDirect3DRMDevice3_GetShades(p)                        (p)->GetShades()
#define IDirect3DRMDevice3_GetHeight(p)                        (p)->GetHeight()
#define IDirect3DRMDevice3_GetWidth(p)                         (p)->GetWidth()
#define IDirect3DRMDevice3_GetTrianglesDrawn(p)                (p)->GetTrianglesDrawn()
#define IDirect3DRMDevice3_GetWireframeOptions(p)              (p)->GetWireframeOptions()
#define IDirect3DRMDevice3_GetQuality(p)                       (p)->GetQuality()
#define IDirect3DRMDevice3_GetColorModel(p)                    (p)->GetColorModel()
#define IDirect3DRMDevice3_GetTextureQuality(p)                (p)->GetTextureQuality()
#define IDirect3DRMDevice3_GetDirect3DDevice(p,a)              (p)->GetDirect3DDevice(a)
/*** IDirect3DRMDevice2 methods ***/
#define IDirect3DRMDevice3_InitFromD3D2(p,a,b)                 (p)->InitFromD3D2(a,b)
#define IDirect3DRMDevice3_InitFromSurface(p,a,b,c)            (p)->InitFromSurface(a,b,c)
#define IDirect3DRMDevice3_SetRenderMode(p,a)                  (p)->SetRenderMode(a)
#define IDirect3DRMDevice3_GetRenderMode(p)                    (p)->GetRenderMode()
#define IDirect3DRMDevice3_GetDirect3DDevice2(p,a)             (p)->GetDirect3DDevice2(a)
/*** IDirect3DRMDevice3 methods ***/
#define IDirect3DRMDevice3_FindPreferredTextureFormat(p,a,b,c) (p)->FindPreferredTextureFormat(a,b,c)
#define IDirect3DRMDevice3_RenderStateChange(p,a,b,c)          (p)->RenderStateChange(a,b,c)
#define IDirect3DRMDevice3_LightStateChange(p,a,b,c)           (p)->LightStateChange(a,b,c)
#define IDirect3DRMDevice3_GetStateChangeOptions(p,a,b,c)      (p)->GetStateChangeOptions(a,b,c)
#define IDirect3DRMDevice3_SetStateChangeOptions(p,a,b,c)      (p)->SetStateChangeOptions(a,b,c)
#endif

/*****************************************************************************
 * IDirect3DRMViewport interface
 */
#define INTERFACE IDirect3DRMViewport
DECLARE_INTERFACE_(IDirect3DRMViewport,IDirect3DRMObject)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject methods ***/
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetAppData)(THIS_ DWORD data) PURE;
    STDMETHOD_(DWORD, GetAppData)(THIS) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(GetClassName)(THIS_ DWORD *size, char *name) PURE;
    /*** IDirect3DRMViewport methods ***/
    STDMETHOD(Init) (THIS_ IDirect3DRMDevice *device, struct IDirect3DRMFrame *camera,
            DWORD x, DWORD y, DWORD width, DWORD height) PURE;
    STDMETHOD(Clear)(THIS) PURE;
    STDMETHOD(Render)(THIS_ struct IDirect3DRMFrame *frame) PURE;
    STDMETHOD(SetFront)(THIS_ D3DVALUE) PURE;
    STDMETHOD(SetBack)(THIS_ D3DVALUE) PURE;
    STDMETHOD(SetField)(THIS_ D3DVALUE) PURE;
    STDMETHOD(SetUniformScaling)(THIS_ WINBOOL) PURE;
    STDMETHOD(SetCamera)(THIS_ struct IDirect3DRMFrame *camera) PURE;
    STDMETHOD(SetProjection)(THIS_ D3DRMPROJECTIONTYPE) PURE;
    STDMETHOD(Transform)(THIS_ D3DRMVECTOR4D *d, D3DVECTOR *s) PURE;
    STDMETHOD(InverseTransform)(THIS_ D3DVECTOR *d, D3DRMVECTOR4D *s) PURE;
    STDMETHOD(Configure)(THIS_ LONG x, LONG y, DWORD width, DWORD height) PURE;
    STDMETHOD(ForceUpdate)(THIS_ DWORD x1, DWORD y1, DWORD x2, DWORD y2) PURE;
    STDMETHOD(SetPlane)(THIS_ D3DVALUE left, D3DVALUE right, D3DVALUE bottom, D3DVALUE top) PURE;
    STDMETHOD(GetCamera)(THIS_ struct IDirect3DRMFrame **camera) PURE;
    STDMETHOD(GetDevice)(THIS_ IDirect3DRMDevice **device) PURE;
    STDMETHOD(GetPlane)(THIS_ D3DVALUE *left, D3DVALUE *right, D3DVALUE *bottom, D3DVALUE *top) PURE;
    STDMETHOD(Pick)(THIS_ LONG x, LONG y, struct IDirect3DRMPickedArray **visuals) PURE;
    STDMETHOD_(WINBOOL, GetUniformScaling)(THIS) PURE;
    STDMETHOD_(LONG, GetX)(THIS) PURE;
    STDMETHOD_(LONG, GetY)(THIS) PURE;
    STDMETHOD_(DWORD, GetWidth)(THIS) PURE;
    STDMETHOD_(DWORD, GetHeight)(THIS) PURE;
    STDMETHOD_(D3DVALUE, GetField)(THIS) PURE;
    STDMETHOD_(D3DVALUE, GetBack)(THIS) PURE;
    STDMETHOD_(D3DVALUE, GetFront)(THIS) PURE;
    STDMETHOD_(D3DRMPROJECTIONTYPE, GetProjection)(THIS) PURE;
    STDMETHOD(GetDirect3DViewport)(THIS_ IDirect3DViewport **viewport) PURE;
};
#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)
/*** IUnknown methods ***/
#define IDirect3DRMViewport_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DRMViewport_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirect3DRMViewport_Release(p)                   (p)->lpVtbl->Release(p)
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMViewport_Clone(p,a,b,c)               (p)->lpVtbl->Clone(p,a,b,c)
#define IDirect3DRMViewport_AddDestroyCallback(p,a,b)    (p)->lpVtbl->AddDestroyCallback(p,a,b)
#define IDirect3DRMViewport_DeleteDestroyCallback(p,a,b) (p)->lpVtbl->DeleteDestroyCallback(p,a,b)
#define IDirect3DRMViewport_SetAppData(p,a)              (p)->lpVtbl->SetAppData(p,a)
#define IDirect3DRMViewport_GetAppData(p)                (p)->lpVtbl->GetAppData(p)
#define IDirect3DRMViewport_SetName(p,a)                 (p)->lpVtbl->SetName(p,a)
#define IDirect3DRMViewport_GetName(p,a,b)               (p)->lpVtbl->GetName(p,a,b)
#define IDirect3DRMViewport_GetClassName(p,a,b)          (p)->lpVtbl->GetClassName(p,a,b)
/*** IDirect3DRMViewport methods ***/
#define IDirect3DRMViewport_Init(p,a,b,c,d,e,f)          (p)->lpVtbl->Init(p,a,b,c,d,e,f)
#define IDirect3DRMViewport_Clear(p)                     (p)->lpVtbl->Clear(p)
#define IDirect3DRMViewport_Render(p,a)                  (p)->lpVtbl->Render(p,a)
#define IDirect3DRMViewport_SetFront(p,a)                (p)->lpVtbl->SetFront(p,a)
#define IDirect3DRMViewport_SetBack(p,a)                 (p)->lpVtbl->SetBack(p,a)
#define IDirect3DRMViewport_SetField(p,a)                (p)->lpVtbl->SetField(p,a)
#define IDirect3DRMViewport_SetUniformScaling(p,a)       (p)->lpVtbl->SetUniformScaling(p,a)
#define IDirect3DRMViewport_SetCamera(p,a)               (p)->lpVtbl->SetCamera(p,a)
#define IDirect3DRMViewport_SetProjection(p,a)           (p)->lpVtbl->SetProjection(p,a)
#define IDirect3DRMViewport_Transform(p,a,b)             (p)->lpVtbl->Transform(p,a,b)
#define IDirect3DRMViewport_InverseTransform(p,a,b)      (p)->lpVtbl->InverseTransform(p,a,b)
#define IDirect3DRMViewport_Configure(p,a,b,c,d)         (p)->lpVtbl->Configure(p,a,b,c,d)
#define IDirect3DRMViewport_ForceUpdate(p,a,b,c,d)       (p)->lpVtbl->ForceUpdate(p,a,b,c,d)
#define IDirect3DRMViewport_SetPlane(p,a,b,c,d)          (p)->lpVtbl->SetPlane(p,a,b,c,d)
#define IDirect3DRMViewport_GetCamera(p,a)               (p)->lpVtbl->GetCamera(p,a)
#define IDirect3DRMViewport_GetDevice(p,a)               (p)->lpVtbl->GetDevice(p,a)
#define IDirect3DRMViewport_GetPlane(p,a,b,c,d)          (p)->lpVtbl->GetPlane(p,a,b,c,d)
#define IDirect3DRMViewport_Pick(p,a,b,c)                (p)->lpVtbl->Pick(p,a,b,c)
#define IDirect3DRMViewport_GetUniformScaling(p)         (p)->lpVtbl->GetUniformScaling(p)
#define IDirect3DRMViewport_GetX(p)                      (p)->lpVtbl->GetX(p)
#define IDirect3DRMViewport_GetY(p)                      (p)->lpVtbl->GetY(p)
#define IDirect3DRMViewport_GetWidth(p)                  (p)->lpVtbl->GetWidth(p)
#define IDirect3DRMViewport_GetHeight(p)                 (p)->lpVtbl->GetHeight(p)
#define IDirect3DRMViewport_GetField(p)                  (p)->lpVtbl->GetField(p)
#define IDirect3DRMViewport_GetBack(p)                   (p)->lpVtbl->GetBack(p)
#define IDirect3DRMViewport_GetFront(p)                  (p)->lpVtbl->GetFront(p)
#define IDirect3DRMViewport_GetProjection(p)             (p)->lpVtbl->GetProjection(p)
#define IDirect3DRMViewport_GetDirect3DViewport(p,a)     (p)->lpVtbl->GetDirect3DViewport(p,a)
#else
/*** IUnknown methods ***/
#define IDirect3DRMViewport_QueryInterface(p,a,b)        (p)->QueryInterface(a,b)
#define IDirect3DRMViewport_AddRef(p)                    (p)->AddRef()
#define IDirect3DRMViewport_Release(p)                   (p)->Release()
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMViewport_Clone(p,a,b,c)               (p)->Clone(a,b,c)
#define IDirect3DRMViewport_AddDestroyCallback(p,a,b)    (p)->AddDestroyCallback(a,b)
#define IDirect3DRMViewport_DeleteDestroyCallback(p,a,b) (p)->DeleteDestroyCallback(a,b)
#define IDirect3DRMViewport_SetAppData(p,a)              (p)->SetAppData(a)
#define IDirect3DRMViewport_GetAppData(p)                (p)->GetAppData()
#define IDirect3DRMViewport_SetName(p,a)                 (p)->SetName(a)
#define IDirect3DRMViewport_GetName(p,a,b)               (p)->GetName(a,b)
#define IDirect3DRMViewport_GetClassName(p,a,b)          (p)->GetClassName(a,b)
/*** IDirect3DRMViewport methods ***/
#define IDirect3DRMViewport_Init(p,a,b,c,d,e,f)          (p)->Init(a,b,c,d,e,f)
#define IDirect3DRMViewport_Clear(p)                     (p)->Clear()
#define IDirect3DRMViewport_Render(p,a)                  (p)->Render(a)
#define IDirect3DRMViewport_SetFront(p,a)                (p)->SetFront(a)
#define IDirect3DRMViewport_SetBack(p,a)                 (p)->SetBack(a)
#define IDirect3DRMViewport_SetField(p,a)                (p)->SetField(a)
#define IDirect3DRMViewport_SetUniformScaling(p,a)       (p)->SetUniformScaling(a)
#define IDirect3DRMViewport_SetCamera(p,a)               (p)->SetCamera(a)
#define IDirect3DRMViewport_SetProjection(p,a)           (p)->SetProjection(a)
#define IDirect3DRMViewport_Transform(p,a,b)             (p)->Transform(a,b)
#define IDirect3DRMViewport_InverseTransform(p,a,b)      (p)->InverseTransform(a,b)
#define IDirect3DRMViewport_Configure(p,a,b,c,d)         (p)->Configure(a,b,c,d)
#define IDirect3DRMViewport_ForceUpdate(p,a,b,c,d)       (p)->ForceUpdate(a,b,c,d)
#define IDirect3DRMViewport_SetPlane(p,a,b,c,d)          (p)->SetPlane(a,b,c,d)
#define IDirect3DRMViewport_GetCamera(p,a)               (p)->GetCamera(a)
#define IDirect3DRMViewport_GetDevice(p,a)               (p)->GetDevice(a)
#define IDirect3DRMViewport_GetPlane(p,a,b,c,d)          (p)->GetPlane(a,b,c,d)
#define IDirect3DRMViewport_Pick(p,a,b,c)                (p)->Pick(a,b,c)
#define IDirect3DRMViewport_GetUniformScaling(p)         (p)->GetUniformScaling()
#define IDirect3DRMViewport_GetX(p)                      (p)->GetX()
#define IDirect3DRMViewport_GetY(p)                      (p)->GetY()
#define IDirect3DRMViewport_GetWidth(p)                  (p)->GetWidth()
#define IDirect3DRMViewport_GetHeight(p)                 (p)->GetHeight()
#define IDirect3DRMViewport_GetField(p)                  (p)->GetField()
#define IDirect3DRMViewport_GetBack(p)                   (p)->GetBack()
#define IDirect3DRMViewport_GetFront(p)                  (p)->GetFront()
#define IDirect3DRMViewport_GetProjection(p)             (p)->GetProjection()
#define IDirect3DRMViewport_GetDirect3DViewport(p,a)     (p)->GetDirect3DViewport(a)
#endif

/*****************************************************************************
 * IDirect3DRMViewport2 interface
 */
#define INTERFACE IDirect3DRMViewport2
DECLARE_INTERFACE_(IDirect3DRMViewport2,IDirect3DRMObject)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject methods ***/
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetAppData)(THIS_ DWORD data) PURE;
    STDMETHOD_(DWORD, GetAppData)(THIS) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(GetClassName)(THIS_ DWORD *size, char *name) PURE;
    /*** IDirect3DRMViewport2 methods ***/
    STDMETHOD(Init) (THIS_ IDirect3DRMDevice3 *device, struct IDirect3DRMFrame3 *camera,
            DWORD x, DWORD y, DWORD width, DWORD height) PURE;
    STDMETHOD(Clear)(THIS_ DWORD flags) PURE;
    STDMETHOD(Render)(THIS_ struct IDirect3DRMFrame3 *frame) PURE;
    STDMETHOD(SetFront)(THIS_ D3DVALUE) PURE;
    STDMETHOD(SetBack)(THIS_ D3DVALUE) PURE;
    STDMETHOD(SetField)(THIS_ D3DVALUE) PURE;
    STDMETHOD(SetUniformScaling)(THIS_ WINBOOL) PURE;
    STDMETHOD(SetCamera)(THIS_ struct IDirect3DRMFrame3 *camera) PURE;
    STDMETHOD(SetProjection)(THIS_ D3DRMPROJECTIONTYPE) PURE;
    STDMETHOD(Transform)(THIS_ D3DRMVECTOR4D *d, D3DVECTOR *s) PURE;
    STDMETHOD(InverseTransform)(THIS_ D3DVECTOR *d, D3DRMVECTOR4D *s) PURE;
    STDMETHOD(Configure)(THIS_ LONG x, LONG y, DWORD width, DWORD height) PURE;
    STDMETHOD(ForceUpdate)(THIS_ DWORD x1, DWORD y1, DWORD x2, DWORD y2) PURE;
    STDMETHOD(SetPlane)(THIS_ D3DVALUE left, D3DVALUE right, D3DVALUE bottom, D3DVALUE top) PURE;
    STDMETHOD(GetCamera)(THIS_ struct IDirect3DRMFrame3 **camera) PURE;
    STDMETHOD(GetDevice)(THIS_ IDirect3DRMDevice3 **device) PURE;
    STDMETHOD(GetPlane)(THIS_ D3DVALUE *left, D3DVALUE *right, D3DVALUE *bottom, D3DVALUE *top) PURE;
    STDMETHOD(Pick)(THIS_ LONG x, LONG y, struct IDirect3DRMPickedArray **visuals) PURE;
    STDMETHOD_(WINBOOL, GetUniformScaling)(THIS) PURE;
    STDMETHOD_(LONG, GetX)(THIS) PURE;
    STDMETHOD_(LONG, GetY)(THIS) PURE;
    STDMETHOD_(DWORD, GetWidth)(THIS) PURE;
    STDMETHOD_(DWORD, GetHeight)(THIS) PURE;
    STDMETHOD_(D3DVALUE, GetField)(THIS) PURE;
    STDMETHOD_(D3DVALUE, GetBack)(THIS) PURE;
    STDMETHOD_(D3DVALUE, GetFront)(THIS) PURE;
    STDMETHOD_(D3DRMPROJECTIONTYPE, GetProjection)(THIS) PURE;
    STDMETHOD(GetDirect3DViewport)(THIS_ IDirect3DViewport **viewport) PURE;
    STDMETHOD(TransformVectors)(THIS_ DWORD vector_count, D3DRMVECTOR4D *dst_vectors,
            D3DVECTOR *src_vectors) PURE;
    STDMETHOD(InverseTransformVectors)(THIS_ DWORD vector_count, D3DVECTOR *dst_vectors,
            D3DRMVECTOR4D *src_vectors) PURE;
};
#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)
/*** IUnknown methods ***/
#define IDirect3DRMViewport2_QueryInterface(p,a,b)            (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DRMViewport2_AddRef(p)                        (p)->lpVtbl->AddRef(p)
#define IDirect3DRMViewport2_Release(p)                       (p)->lpVtbl->Release(p)
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMViewport_2Clone(p,a,b,c)                   (p)->lpVtbl->Clone(p,a,b,c)
#define IDirect3DRMViewport2_AddDestroyCallback(p,a,b)        (p)->lpVtbl->AddDestroyCallback(p,a,b)
#define IDirect3DRMViewport2_DeleteDestroyCallback(p,a,b)     (p)->lpVtbl->DeleteDestroyCallback(p,a,b)
#define IDirect3DRMViewport2_SetAppData(p,a)                  (p)->lpVtbl->SetAppData(p,a)
#define IDirect3DRMViewport2_GetAppData(p)                    (p)->lpVtbl->GetAppData(p)
#define IDirect3DRMViewport2_SetName(p,a)                     (p)->lpVtbl->SetName(p,a)
#define IDirect3DRMViewport2_GetName(p,a,b)                   (p)->lpVtbl->GetName(p,a,b)
#define IDirect3DRMViewport2_GetClassName(p,a,b)              (p)->lpVtbl->GetClassName(p,a,b)
/*** IDirect3DRMViewport2 methods ***/
#define IDirect3DRMViewport2_Init(p,a,b,c,d,e,f)              (p)->lpVtbl->Init(p,a,b,c,d,e,f)
#define IDirect3DRMViewport2_Clear(p,a)                       (p)->lpVtbl->Clear(p,a)
#define IDirect3DRMViewport2_Render(p,a)                      (p)->lpVtbl->Render(p,a)
#define IDirect3DRMViewport2_SetFront(p,a)                    (p)->lpVtbl->SetFront(p,a)
#define IDirect3DRMViewport2_SetBack(p,a)                     (p)->lpVtbl->SetBack(p,a)
#define IDirect3DRMViewport2_SetField(p,a)                    (p)->lpVtbl->SetField(p,a)
#define IDirect3DRMViewport2_SetUniformScaling(p,a)           (p)->lpVtbl->SetUniformScaling(p,a)
#define IDirect3DRMViewport2_SetCamera(p,a)                   (p)->lpVtbl->SetCamera(p,a)
#define IDirect3DRMViewport2_SetProjection(p,a)               (p)->lpVtbl->SetProjection(p,a)
#define IDirect3DRMViewport2_Transform(p,a,b)                 (p)->lpVtbl->Transform(p,a,b)
#define IDirect3DRMViewport2_InverseTransform(p,a,b)          (p)->lpVtbl->InverseTransform(p,a,b)
#define IDirect3DRMViewport2_Configure(p,a,b,c,d)             (p)->lpVtbl->Configure(p,a,b,c,d)
#define IDirect3DRMViewport2_ForceUpdate(p,a,b,c,d)           (p)->lpVtbl->ForceUpdate(p,a,b,c,d)
#define IDirect3DRMViewport2_SetPlane(p,a,b,c,d)              (p)->lpVtbl->SetPlane(p,a,b,c,d)
#define IDirect3DRMViewport2_GetCamera(p,a)                   (p)->lpVtbl->GetCamera(p,a)
#define IDirect3DRMViewport2_GetDevice(p,a)                   (p)->lpVtbl->GetDevice(p,a)
#define IDirect3DRMViewport2_GetPlane(p,a,b,c,d)              (p)->lpVtbl->GetPlane(p,a,b,c,d)
#define IDirect3DRMViewport2_Pick(p,a,b,c)                    (p)->lpVtbl->Pick(p,a,b,c)
#define IDirect3DRMViewport2_GetUniformScaling(p)             (p)->lpVtbl->GetUniformScaling(p)
#define IDirect3DRMViewport2_GetX(p)                          (p)->lpVtbl->GetX(p)
#define IDirect3DRMViewport2_GetY(p)                          (p)->lpVtbl->GetY(p)
#define IDirect3DRMViewport2_GetWidth(p)                      (p)->lpVtbl->GetWidth(p)
#define IDirect3DRMViewport2_GetHeight(p)                     (p)->lpVtbl->GetHeight(p)
#define IDirect3DRMViewport2_GetField(p)                      (p)->lpVtbl->GetField(p)
#define IDirect3DRMViewport2_GetBack(p)                       (p)->lpVtbl->GetBack(p)
#define IDirect3DRMViewport2_GetFront(p)                      (p)->lpVtbl->GetFront(p)
#define IDirect3DRMViewport2_GetProjection(p)                 (p)->lpVtbl->GetProjection(p)
#define IDirect3DRMViewport2_GetDirect3DViewport(p,a)         (p)->lpVtbl->GetDirect3DViewport(p,a)
#define IDirect3DRMViewport2_TransformVectors(p,a,b,c)        (p)->lpVtbl->TransformVectors(p,a,b,c)
#define IDirect3DRMViewport2_InverseTransformVectors(p,a,b,c) (p)->lpVtbl->InverseTransformVectors(p,a,b,c)
#else
/*** IUnknown methods ***/
#define IDirect3DRMViewport2_QueryInterface(p,a,b)            (p)->QueryInterface(a,b)
#define IDirect3DRMViewport2_AddRef(p)                        (p)->AddRef()
#define IDirect3DRMViewport2_Release(p)                       (p)->Release()
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMViewport2_Clone(p,a,b,c)                   (p)->Clone(a,b,c)
#define IDirect3DRMViewport2_AddDestroyCallback(p,a,b)        (p)->AddDestroyCallback(a,b)
#define IDirect3DRMViewport2_DeleteDestroyCallback(p,a,b)     (p)->DeleteDestroyCallback(a,b)
#define IDirect3DRMViewport2_SetAppData(p,a)                  (p)->SetAppData(a)
#define IDirect3DRMViewport2_GetAppData(p)                    (p)->GetAppData()
#define IDirect3DRMViewport2_SetName(p,a)                     (p)->SetName(a)
#define IDirect3DRMViewport2_GetName(p,a,b)                   (p)->GetName(a,b)
#define IDirect3DRMViewport2_GetClassName(p,a,b)              (p)->GetClassName(a,b)
/*** IDirect3DRMViewport2 methods ***/
#define IDirect3DRMViewport2_Init(p,a,b,c,d,e,f)              (p)->Init(a,b,c,d,e,f)
#define IDirect3DRMViewport2_Clear(p)                         (p)->Clear()
#define IDirect3DRMViewport2_Render(p,a)                      (p)->Render(a)
#define IDirect3DRMViewport2_SetFront(p,a)                    (p)->SetFront(a)
#define IDirect3DRMViewport2_SetBack(p,a)                     (p)->SetBack(a)
#define IDirect3DRMViewport2_SetField(p,a)                    (p)->SetField(a)
#define IDirect3DRMViewport2_SetUniformScaling(p,a)           (p)->SetUniformScaling(a)
#define IDirect3DRMViewport2_SetCamera(p,a)                   (p)->SetCamera(a)
#define IDirect3DRMViewport2_SetProjection(p,a)               (p)->SetProjection(a)
#define IDirect3DRMViewport2_Transform(p,a,b)                 (p)->Transform(a,b)
#define IDirect3DRMViewport2_InverseTransform(p,a,b)          (p)->InverseTransform(a,b)
#define IDirect3DRMViewport2_Configure(p,a,b,c,d)             (p)->Configure(a,b,c,d)
#define IDirect3DRMViewport2_ForceUpdate(p,a,b,c,d)           (p)->ForceUpdate(a,b,c,d)
#define IDirect3DRMViewport2_SetPlane(p,a,b,c,d)              (p)->SetPlane(a,b,c,d)
#define IDirect3DRMViewport2_GetCamera(p,a)                   (p)->GetCamera(a)
#define IDirect3DRMViewport2_GetDevice(p,a)                   (p)->GetDevice(a)
#define IDirect3DRMViewport2_GetPlane(p,a,b,c,d)              (p)->GetPlane(a,b,c,d)
#define IDirect3DRMViewport2_Pick(p,a,b,c)                    (p)->Pick(a,b,c)
#define IDirect3DRMViewport2_GetUniformScaling(p)             (p)->GetUniformScaling()
#define IDirect3DRMViewport2_GetX(p)                          (p)->GetX()
#define IDirect3DRMViewport2_GetY(p)                          (p)->GetY()
#define IDirect3DRMViewport2_GetWidth(p)                      (p)->GetWidth()
#define IDirect3DRMViewport2_GetHeight(p)                     (p)->GetHeight()
#define IDirect3DRMViewport2_GetField(p)                      (p)->GetField()
#define IDirect3DRMViewport2_GetBack(p)                       (p)->GetBack()
#define IDirect3DRMViewport2_GetFront(p)                      (p)->GetFront()
#define IDirect3DRMViewport2_GetProjection(p)                 (p)->GetProjection()
#define IDirect3DRMViewport2_GetDirect3DViewport(p,a)         (p)->GetDirect3DViewport(a)
#define IDirect3DRMViewport2_TransformVectors(p,a,b,c)        (p)->TransformVectors(a,b,c)
#define IDirect3DRMViewport2_InverseTransformVectors(p,a,b,c) (p)->InverseTransformVectors(a,b,c)
#endif

/*****************************************************************************
 * IDirect3DRMFrame interface
 */
#define INTERFACE IDirect3DRMFrame
DECLARE_INTERFACE_(IDirect3DRMFrame,IDirect3DRMVisual)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject methods ***/
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetAppData)(THIS_ DWORD data) PURE;
    STDMETHOD_(DWORD, GetAppData)(THIS) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(GetClassName)(THIS_ DWORD *size, char *name) PURE;
    /*** IDirect3DRMFrame methods ***/
    STDMETHOD(AddChild)(THIS_ IDirect3DRMFrame *child) PURE;
    STDMETHOD(AddLight)(THIS_ struct IDirect3DRMLight *light) PURE;
    STDMETHOD(AddMoveCallback)(THIS_ D3DRMFRAMEMOVECALLBACK cb, void *ctx) PURE;
    STDMETHOD(AddTransform)(THIS_ D3DRMCOMBINETYPE, D3DRMMATRIX4D) PURE;
    STDMETHOD(AddTranslation)(THIS_ D3DRMCOMBINETYPE, D3DVALUE x, D3DVALUE y, D3DVALUE z) PURE;
    STDMETHOD(AddScale)(THIS_ D3DRMCOMBINETYPE, D3DVALUE sx, D3DVALUE sy, D3DVALUE sz) PURE;
    STDMETHOD(AddRotation)(THIS_ D3DRMCOMBINETYPE, D3DVALUE x, D3DVALUE y, D3DVALUE z, D3DVALUE theta) PURE;
    STDMETHOD(AddVisual)(THIS_ IDirect3DRMVisual *visual) PURE;
    STDMETHOD(GetChildren)(THIS_ struct IDirect3DRMFrameArray **children) PURE;
    STDMETHOD_(D3DCOLOR, GetColor)(THIS) PURE;
    STDMETHOD(GetLights)(THIS_ struct IDirect3DRMLightArray **lights) PURE;
    STDMETHOD_(D3DRMMATERIALMODE, GetMaterialMode)(THIS) PURE;
    STDMETHOD(GetParent)(THIS_ IDirect3DRMFrame **parent) PURE;
    STDMETHOD(GetPosition)(THIS_ IDirect3DRMFrame *reference, D3DVECTOR *return_position) PURE;
    STDMETHOD(GetRotation)(THIS_ IDirect3DRMFrame *reference, D3DVECTOR *axis, D3DVALUE *return_theta) PURE;
    STDMETHOD(GetScene)(THIS_ IDirect3DRMFrame **scene) PURE;
    STDMETHOD_(D3DRMSORTMODE, GetSortMode)(THIS) PURE;
    STDMETHOD(GetTexture)(THIS_ struct IDirect3DRMTexture **texture) PURE;
    STDMETHOD(GetTransform)(THIS_ D3DRMMATRIX4D return_matrix) PURE;
    STDMETHOD(GetVelocity)(THIS_ IDirect3DRMFrame *reference, D3DVECTOR *return_velocity, WINBOOL with_rotation) PURE;
    STDMETHOD(GetOrientation)(THIS_ IDirect3DRMFrame *reference, D3DVECTOR *dir, D3DVECTOR *up) PURE;
    STDMETHOD(GetVisuals)(THIS_ struct IDirect3DRMVisualArray **visuals) PURE;
    STDMETHOD(GetTextureTopology)(THIS_ WINBOOL *wrap_u, WINBOOL *wrap_v) PURE;
    STDMETHOD(InverseTransform)(THIS_ D3DVECTOR *d, D3DVECTOR *s) PURE;
    STDMETHOD(Load)(THIS_ void *filename, void *name, D3DRMLOADOPTIONS flags,
            D3DRMLOADTEXTURECALLBACK cb, void *ctx)PURE;
    STDMETHOD(LookAt)(THIS_ IDirect3DRMFrame *target, IDirect3DRMFrame *reference,
            D3DRMFRAMECONSTRAINT constraint) PURE;
    STDMETHOD(Move)(THIS_ D3DVALUE delta) PURE;
    STDMETHOD(DeleteChild)(THIS_ IDirect3DRMFrame *child) PURE;
    STDMETHOD(DeleteLight)(THIS_ struct IDirect3DRMLight *light) PURE;
    STDMETHOD(DeleteMoveCallback)(THIS_ D3DRMFRAMEMOVECALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteVisual)(THIS_ IDirect3DRMVisual *visual) PURE;
    STDMETHOD_(D3DCOLOR, GetSceneBackground)(THIS) PURE;
    STDMETHOD(GetSceneBackgroundDepth)(THIS_ IDirectDrawSurface **surface) PURE;
    STDMETHOD_(D3DCOLOR, GetSceneFogColor)(THIS) PURE;
    STDMETHOD_(WINBOOL, GetSceneFogEnable)(THIS) PURE;
    STDMETHOD_(D3DRMFOGMODE, GetSceneFogMode)(THIS) PURE;
    STDMETHOD(GetSceneFogParams)(THIS_ D3DVALUE *return_start, D3DVALUE *return_end, D3DVALUE *return_density) PURE;
    STDMETHOD(SetSceneBackground)(THIS_ D3DCOLOR) PURE;
    STDMETHOD(SetSceneBackgroundRGB)(THIS_ D3DVALUE red, D3DVALUE green, D3DVALUE blue) PURE;
    STDMETHOD(SetSceneBackgroundDepth)(THIS_ IDirectDrawSurface *surface) PURE;
    STDMETHOD(SetSceneBackgroundImage)(THIS_ struct IDirect3DRMTexture *texture) PURE;
    STDMETHOD(SetSceneFogEnable)(THIS_ WINBOOL) PURE;
    STDMETHOD(SetSceneFogColor)(THIS_ D3DCOLOR) PURE;
    STDMETHOD(SetSceneFogMode)(THIS_ D3DRMFOGMODE) PURE;
    STDMETHOD(SetSceneFogParams)(THIS_ D3DVALUE start, D3DVALUE end, D3DVALUE density) PURE;
    STDMETHOD(SetColor)(THIS_ D3DCOLOR) PURE;
    STDMETHOD(SetColorRGB)(THIS_ D3DVALUE red, D3DVALUE green, D3DVALUE blue) PURE;
    STDMETHOD_(D3DRMZBUFFERMODE, GetZbufferMode)(THIS) PURE;
    STDMETHOD(SetMaterialMode)(THIS_ D3DRMMATERIALMODE) PURE;
    STDMETHOD(SetOrientation)(THIS_ IDirect3DRMFrame *reference, D3DVALUE dx, D3DVALUE dy, D3DVALUE dz,
            D3DVALUE ux, D3DVALUE uy, D3DVALUE uz) PURE;
    STDMETHOD(SetPosition)(THIS_ IDirect3DRMFrame *reference, D3DVALUE x, D3DVALUE y, D3DVALUE z) PURE;
    STDMETHOD(SetRotation)(THIS_ IDirect3DRMFrame *reference, D3DVALUE x, D3DVALUE y, D3DVALUE z, D3DVALUE theta) PURE;
    STDMETHOD(SetSortMode)(THIS_ D3DRMSORTMODE) PURE;
    STDMETHOD(SetTexture)(THIS_ struct IDirect3DRMTexture *texture) PURE;
    STDMETHOD(SetTextureTopology)(THIS_ WINBOOL wrap_u, WINBOOL wrap_v) PURE;
    STDMETHOD(SetVelocity)(THIS_ IDirect3DRMFrame *reference,
            D3DVALUE x, D3DVALUE y, D3DVALUE z, WINBOOL with_rotation) PURE;
    STDMETHOD(SetZbufferMode)(THIS_ D3DRMZBUFFERMODE) PURE;
    STDMETHOD(Transform)(THIS_ D3DVECTOR *d, D3DVECTOR *s) PURE;
};
#undef INTERFACE

#if !defined(__cplusplus) || defined(CINTERFACE)
/*** IUnknown methods ***/
#define IDirect3DRMFrame_QueryInterface(p,a,b)            (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DRMFrame_AddRef(p)                        (p)->lpVtbl->AddRef(p)
#define IDirect3DRMFrame_Release(p)                       (p)->lpVtbl->Release(p)
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMFrame_Clone(p,a,b,c)                   (p)->lpVtbl->Clone(p,a,b,c)
#define IDirect3DRMFrame_AddDestroyCallback(p,a,b)        (p)->lpVtbl->AddDestroyCallback(p,a,b)
#define IDirect3DRMFrame_DeleteDestroyCallback(p,a,b)     (p)->lpVtbl->DeleteDestroyCallback(p,a,b)
#define IDirect3DRMFrame_SetAppData(p,a)                  (p)->lpVtbl->SetAppData(p,a)
#define IDirect3DRMFrame_GetAppData(p)                    (p)->lpVtbl->GetAppData(p)
#define IDirect3DRMFrame_SetName(p,a)                     (p)->lpVtbl->SetName(p,a)
#define IDirect3DRMFrame_GetName(p,a,b)                   (p)->lpVtbl->GetName(p,a,b)
#define IDirect3DRMFrame_GetClassName(p,a,b)              (p)->lpVtbl->GetClassName(p,a,b)
/*** IDirect3DRMFrame methods ***/
#define IDirect3DRMFrame_AddChild(p,a)                    (p)->lpVtbl->AddChild(p,a)
#define IDirect3DRMFrame_AddLight(p,a)                    (p)->lpVtbl->AddLight(p,a)
#define IDirect3DRMFrame_AddMoveCallback(p,a,b)           (p)->lpVtbl->AddMoveCallback(p,a,b)
#define IDirect3DRMFrame_AddTransform(p,a,b)              (p)->lpVtbl->AddTransform(p,a,b)
#define IDirect3DRMFrame_AddTranslation(p,a,b,c,d)        (p)->lpVtbl->AddTranslation(p,a,b,c,d)
#define IDirect3DRMFrame_AddScale(p,a,b,c,d)              (p)->lpVtbl->AddScale(p,a,b,c,d)
#define IDirect3DRMFrame_AddRotation(p,a,b,c,d,e)         (p)->lpVtbl->AddRotation(p,a,b,c,d,e)
#define IDirect3DRMFrame_AddVisual(p,a)                   (p)->lpVtbl->AddVisual(p,a)
#define IDirect3DRMFrame_GetChildren(p,a)                 (p)->lpVtbl->GetChildren(p,a)
#define IDirect3DRMFrame_GetColor(p)                      (p)->lpVtbl->GetColor(p)
#define IDirect3DRMFrame_GetLights(p,a)                   (p)->lpVtbl->GetLights(p,a)
#define IDirect3DRMFrame_GetMaterialMode(p)               (p)->lpVtbl->GetMaterialMode(p)
#define IDirect3DRMFrame_GetParent(p,a)                   (p)->lpVtbl->GetParent(p,a)
#define IDirect3DRMFrame_GetPosition(p,a,b)               (p)->lpVtbl->GetPosition(p,a,b)
#define IDirect3DRMFrame_GetRotation(p,a,b,c)             (p)->lpVtbl->GetRotation(p,a,b,c)
#define IDirect3DRMFrame_GetScene(p,a)                    (p)->lpVtbl->GetScene(p,a)
#define IDirect3DRMFrame_GetSortMode(p)                   (p)->lpVtbl->GetSortMode(p)
#define IDirect3DRMFrame_GetTexture(p,a)                  (p)->lpVtbl->GetTexture(p,a)
#define IDirect3DRMFrame_GetTransform(p,a)                (p)->lpVtbl->GetTransform(p,a)
#define IDirect3DRMFrame_GetVelocity(p,a,b,c)             (p)->lpVtbl->GetVelocity(p,a,b,c)
#define IDirect3DRMFrame_GetOrientation(p,a,b,c)          (p)->lpVtbl->GetOrientation(p,a,b,c)
#define IDirect3DRMFrame_GetVisuals(p,a)                  (p)->lpVtbl->GetVisuals(p,a)
#define IDirect3DRMFrame_GetTextureTopology(p,a,b)        (p)->lpVtbl->GetTextureTopology(p,a,b)
#define IDirect3DRMFrame_InverseTransform(p,a,b)          (p)->lpVtbl->InverseTransform(p,a,b)
#define IDirect3DRMFrame_Load(p,a,b,c,d,e)                (p)->lpVtbl->Load(p,a,b,c,d,e)
#define IDirect3DRMFrame_LookAt(p,a,b,c)                  (p)->lpVtbl->LookAt(p,a,b,c)
#define IDirect3DRMFrame_Move(p,a)                        (p)->lpVtbl->Move(p,a)
#define IDirect3DRMFrame_DeleteChild(p,a)                 (p)->lpVtbl->DeleteChild(p,a)
#define IDirect3DRMFrame_DeleteLight(p,a)                 (p)->lpVtbl->DeleteLight(p,a)
#define IDirect3DRMFrame_DeleteMoveCallback(p,a,b)        (p)->lpVtbl->DeleteMoveCallback(p,a,b)
#define IDirect3DRMFrame_DeleteVisual(p,a)                (p)->lpVtbl->DeleteVisual(p,a)
#define IDirect3DRMFrame_GetSceneBackground(p)            (p)->lpVtbl->GetSceneBackground(p)
#define IDirect3DRMFrame_GetSceneBackgroundDepth(p,a)     (p)->lpVtbl->GetSceneBackgroundDepth(p,a)
#define IDirect3DRMFrame_GetSceneFogColor(p)              (p)->lpVtbl->GetSceneFogColor(p)
#define IDirect3DRMFrame_GetSceneFogEnable(p)             (p)->lpVtbl->GetSceneFogEnable(p)
#define IDirect3DRMFrame_GetSceneFogMode(p)               (p)->lpVtbl->GetSceneFogMode(p)
#define IDirect3DRMFrame_GetSceneFogParams(p,a,b,c)       (p)->lpVtbl->GetSceneFogParams(p,a,b,c)
#define IDirect3DRMFrame_SetSceneBackground(p,a)          (p)->lpVtbl->SetSceneBackground(p,a)
#define IDirect3DRMFrame_SetSceneBackgroundRGB(p,a,b,c)   (p)->lpVtbl->SetSceneBackgroundRGB(p,a,b,c)
#define IDirect3DRMFrame_SetSceneBackgroundDepth(p,a)     (p)->lpVtbl->SetSceneBackgroundDepth(p,a)
#define IDirect3DRMFrame_SetSceneBackgroundImage(p,a)     (p)->lpVtbl->SetSceneBackgroundImage(p,a)
#define IDirect3DRMFrame_SetSceneFogEnable(p,a)           (p)->lpVtbl->SetSceneFogEnable(p,a)
#define IDirect3DRMFrame_SetSceneFogColor(p,a)            (p)->lpVtbl->SetSceneFogColor(p,a)
#define IDirect3DRMFrame_SetSceneFogMode(p,a)             (p)->lpVtbl->SetSceneFogMode(p,a)
#define IDirect3DRMFrame_SetSceneFogParams(p,a,b,c)       (p)->lpVtbl->SetSceneFogParams(p,a,b,c)
#define IDirect3DRMFrame_SetColor(p,a)                    (p)->lpVtbl->SetColor(p,a)
#define IDirect3DRMFrame_SetColorRGB(p,a,b,c)             (p)->lpVtbl->SetColorRGB(p,a,b,c)
#define IDirect3DRMFrame_GetZbufferMode(p)                (p)->lpVtbl->GetZbufferMode(p)
#define IDirect3DRMFrame_SetMaterialMode(p,a)             (p)->lpVtbl->SetMaterialMode(p,a)
#define IDirect3DRMFrame_SetOrientation(p,a,b,c,d,e,f,g)  (p)->lpVtbl->SetOrientation(p,a,b,c,d,e,f,g)
#define IDirect3DRMFrame_SetPosition(p,a,b,c,d)           (p)->lpVtbl->SetPosition(p,a,b,c,d)
#define IDirect3DRMFrame_SetRotation(p,a,b,c,d,e)         (p)->lpVtbl->SetRotation(p,a,b,c,d,e)
#define IDirect3DRMFrame_SetSortMode(p,a)                 (p)->lpVtbl->SetSortMode(p,a)
#define IDirect3DRMFrame_SetTexture(p,a)                  (p)->lpVtbl->SetTexture(p,a)
#define IDirect3DRMFrame_SetTextureTopology(p,a,b)        (p)->lpVtbl->SetTextureTopology(p,a,b)
#define IDirect3DRMFrame_SetVelocity(p,a,b,c,d,e)         (p)->lpVtbl->SetVelocity(p,a,b,c,d,e)
#define IDirect3DRMFrame_SetZbufferMode(p,a)              (p)->lpVtbl->SetZbufferMode(p,a)
#define IDirect3DRMFrame_Transform(p,a,b)                 (p)->lpVtbl->Transform(p,a,b)
#else
/*** IUnknown methods ***/
#define IDirect3DRMFrame_QueryInterface(p,a,b)            (p)->QueryInterface(a,b)
#define IDirect3DRMFrame_AddRef(p)                        (p)->AddRef()
#define IDirect3DRMFrame_Release(p)                       (p)->Release()
/*** IDirect3DRMObject methods ***/
#define IDirect3DRMFrame_Clone(p,a,b,c)                   (p)->Clone(a,b,c)
#define IDirect3DRMFrame_AddDestroyCallback(p,a,b)        (p)->AddDestroyCallback(a,b)
#define IDirect3DRMFrame_DeleteDestroyCallback(p,a,b)     (p)->DeleteDestroyCallback(a,b)
#define IDirect3DRMFrame_SetAppData(p,a)                  (p)->SetAppData(a)
#define IDirect3DRMFrame_GetAppData(p)                    (p)->GetAppData()
#define IDirect3DRMFrame_SetName(p,a)                     (p)->SetName(a)
#define IDirect3DRMFrame_GetName(p,a,b)                   (p)->GetName(a,b)
#define IDirect3DRMFrame_GetClassName(p,a,b)              (p)->GetClassName(a,b)
/*** IDirect3DRMFrame methods ***/
#define IDirect3DRMFrame_AddChild(p,a)                    (p)->AddChild(a)
#define IDirect3DRMFrame_AddLight(p,a)                    (p)->AddLight(a)
#define IDirect3DRMFrame_AddMoveCallback(p,a,b)           (p)->AddMoveCallback(a,b)
#define IDirect3DRMFrame_AddTransform(p,a,b)              (p)->AddTransform(a,b)
#define IDirect3DRMFrame_AddTranslation(p,a,b,c,d)        (p)->AddTranslation(a,b,c,d)
#define IDirect3DRMFrame_AddScale(p,a,b,c,d)              (p)->AddScale(a,b,c,d)
#define IDirect3DRMFrame_AddRotation(p,a,b,c,d,e)         (p)->AddRotation(a,b,c,d,e)
#define IDirect3DRMFrame_AddVisual(p,a)                   (p)->AddVisual(a)
#define IDirect3DRMFrame_GetChildren(p,a)                 (p)->GetChildren(a)
#define IDirect3DRMFrame_GetColor(p)                      (p)->GetColor()
#define IDirect3DRMFrame_GetLights(p,a)                   (p)->GetLights(a)
#define IDirect3DRMFrame_GetMaterialMode(p)               (p)->GetMaterialMode()
#define IDirect3DRMFrame_GetParent(p,a)                   (p)->GetParent(a)
#define IDirect3DRMFrame_GetPosition(p,a,b)               (p)->GetPosition(a,b)
#define IDirect3DRMFrame_GetRotation(p,a,b,c)             (p)->GetRotation(a,b,c)
#define IDirect3DRMFrame_GetScene(p,a)                    (p)->GetScene(a)
#define IDirect3DRMFrame_GetSortMode(p)                   (p)->GetSortMode()
#define IDirect3DRMFrame_GetTexture(p,a)                  (p)->GetTexture(a)
#define IDirect3DRMFrame_GetTransform(p,a)                (p)->GetTransform(a)
#define IDirect3DRMFrame_GetVelocity(p,a,b,c)             (p)->GetVelocity(a,b,c)
#define IDirect3DRMFrame_GetOrientation(p,a,b,c)          (p)->GetOrientation(a,b,c)
#define IDirect3DRMFrame_GetVisuals(p,a)                  (p)->GetVisuals(a)
#define IDirect3DRMFrame_GetTextureTopology(p,a,b)        (p)->GetTextureTopology(a,b)
#define IDirect3DRMFrame_InverseTransform(p,a,b)          (p)->InverseTransform(a,b)
#define IDirect3DRMFrame_Load(p,a,b,c,d,e)                (p)->Load(a,b,c,d,e)
#define IDirect3DRMFrame_LookAt(p,a,b,c)                  (p)->LookAt(a,b,c)
#define IDirect3DRMFrame_Move(p,a)                        (p)->Move(a)
#define IDirect3DRMFrame_DeleteChild(p,a)                 (p)->DeleteChild(a)
#define IDirect3DRMFrame_DeleteLight(p,a)                 (p)->DeleteLight(a)
#define IDirect3DRMFrame_DeleteMoveCallback(p,a,b)        (p)->DeleteMoveCallback(a,b)
#define IDirect3DRMFrame_DeleteVisual(p,a)                (p)->DeleteVisual(a)
#define IDirect3DRMFrame_GetSceneBackground(p)            (p)->GetSceneBackground()
#define IDirect3DRMFrame_GetSceneBackgroundDepth(p,a)     (p)->GetSceneBackgroundDepth(a)
#define IDirect3DRMFrame_GetSceneFogColor(p)              (p)->GetSceneFogColor()
#define IDirect3DRMFrame_GetSceneFogEnable(p)             (p)->GetSceneFogEnable()
#define IDirect3DRMFrame_GetSceneFogMode(p)               (p)->GetSceneFogMode()
#define IDirect3DRMFrame_GetSceneFogParams(p,a,b,c)       (p)->GetSceneFogParams(a,b,c)
#define IDirect3DRMFrame_SetSceneBackground(p,a)          (p)->SetSceneBackground(a)
#define IDirect3DRMFrame_SetSceneBackgroundRGB(p,a,b,c)   (p)->SetSceneBackgroundRGB(a,b,c)
#define IDirect3DRMFrame_SetSceneBackgroundDepth(p,a)     (p)->SetSceneBackgroundDepth(a)
#define IDirect3DRMFrame_SetSceneBackgroundImage(p,a)     (p)->SetSceneBackgroundImage(a)
#define IDirect3DRMFrame_SetSceneFogEnable(p,a)           (p)->SetSceneFogEnable(a)
#define IDirect3DRMFrame_SetSceneFogColor(p,a)            (p)->SetSceneFogColor(a)
#define IDirect3DRMFrame_SetSceneFogMode(p,a)             (p)->SetSceneFogMode(a)
#define IDirect3DRMFrame_SetSceneFogParams(p,a,b,c)       (p)->SetSceneFogParams(a,b,c)
#define IDirect3DRMFrame_SetColor(p,a)                    (p)->SetColor(a)
#define IDirect3DRMFrame_SetColorRGB(p,a,b,c)             (p)->SetColorRGB(a,b,c)
#define IDirect3DRMFrame_GetZbufferMode(p)                (p)->GetZbufferMode()
#define IDirect3DRMFrame_SetMaterialMode(p,a)             (p)->SetMaterialMode(a)
#define IDirect3DRMFrame_SetOrientation(p,a,b,c,d,e,f,g)  (p)->SetOrientation(a,b,c,d,e,f,g)
#define IDirect3DRMFrame_SetPosition(p,a,b,c,d)           (p)->SetPosition(a,b,c,d)
#define IDirect3DRMFrame_SetRotation(p,a,b,c,d,e)         (p)->SetRotation(a,b,c,d,e)
#define IDirect3DRMFrame_SetSortMode(p,a)                 (p)->SetSortMode(a)
#define IDirect3DRMFrame_SetTexture(p,a)                  (p)->SetTexture(a)
#define IDirect3DRMFrame_SetTextureTopology(p,a,b)        (p)->SetTextureTopology(a,b)
#define IDirect3DRMFrame_SetVelocity(p,a,b,c,d,e)         (p)->SetVelocity(a,b,c,d,e)
#define IDirect3DRMFrame_SetZbufferMode(p,a)              (p)->SetZbufferMode(a)
#define IDirect3DRMFrame_Transform(p,a,b)                 (p)->Transform(a,b)
#endif

/*****************************************************************************
 * IDirect3DRMFrame2 interface
 */
#define INTERFACE IDirect3DRMFrame2
DECLARE_INTERFACE_(IDirect3DRMFrame2,IDirect3DRMFrame)
{
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    /*** IDirect3DRMObject methods ***/
    STDMETHOD(Clone)(THIS_ IUnknown *outer, REFIID iid, void **out) PURE;
    STDMETHOD(AddDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteDestroyCallback)(THIS_ D3DRMOBJECTCALLBACK cb, void *ctx) PURE;
    STDMETHOD(SetAppData)(THIS_ DWORD data) PURE;
    STDMETHOD_(DWORD, GetAppData)(THIS) PURE;
    STDMETHOD(SetName)(THIS_ const char *name) PURE;
    STDMETHOD(GetName)(THIS_ DWORD *size, char *name) PURE;
    STDMETHOD(GetClassName)(THIS_ DWORD *size, char *name) PURE;
    /*** IDirect3DRMFrame methods ***/
    STDMETHOD(AddChild)(THIS_ IDirect3DRMFrame *child) PURE;
    STDMETHOD(AddLight)(THIS_ struct IDirect3DRMLight *light) PURE;
    STDMETHOD(AddMoveCallback)(THIS_ D3DRMFRAMEMOVECALLBACK cb, void *ctx) PURE;
    STDMETHOD(AddTransform)(THIS_ D3DRMCOMBINETYPE, D3DRMMATRIX4D) PURE;
    STDMETHOD(AddTranslation)(THIS_ D3DRMCOMBINETYPE, D3DVALUE x, D3DVALUE y, D3DVALUE z) PURE;
    STDMETHOD(AddScale)(THIS_ D3DRMCOMBINETYPE, D3DVALUE sx, D3DVALUE sy, D3DVALUE sz) PURE;
    STDMETHOD(AddRotation)(THIS_ D3DRMCOMBINETYPE, D3DVALUE x, D3DVALUE y, D3DVALUE z, D3DVALUE theta) PURE;
    STDMETHOD(AddVisual)(THIS_ IDirect3DRMVisual *visual) PURE;
    STDMETHOD(GetChildren)(THIS_ struct IDirect3DRMFrameArray **children) PURE;
    STDMETHOD_(D3DCOLOR, GetColor)(THIS) PURE;
    STDMETHOD(GetLights)(THIS_ struct IDirect3DRMLightArray **lights) PURE;
    STDMETHOD_(D3DRMMATERIALMODE, GetMaterialMode)(THIS) PURE;
    STDMETHOD(GetParent)(THIS_ IDirect3DRMFrame **parent) PURE;
    STDMETHOD(GetPosition)(THIS_ IDirect3DRMFrame *reference, D3DVECTOR *return_position) PURE;
    STDMETHOD(GetRotation)(THIS_ IDirect3DRMFrame *reference, D3DVECTOR *axis, D3DVALUE *return_theta) PURE;
    STDMETHOD(GetScene)(THIS_ IDirect3DRMFrame **scene) PURE;
    STDMETHOD_(D3DRMSORTMODE, GetSortMode)(THIS) PURE;
    STDMETHOD(GetTexture)(THIS_ struct IDirect3DRMTexture **texture) PURE;
    STDMETHOD(GetTransform)(THIS_ D3DRMMATRIX4D return_matrix) PURE;
    STDMETHOD(GetVelocity)(THIS_ IDirect3DRMFrame *reference, D3DVECTOR *return_velocity, WINBOOL with_rotation) PURE;
    STDMETHOD(GetOrientation)(THIS_ IDirect3DRMFrame *reference, D3DVECTOR *dir, D3DVECTOR *up) PURE;
    STDMETHOD(GetVisuals)(THIS_ struct IDirect3DRMVisualArray **visuals) PURE;
    STDMETHOD(GetTextureTopology)(THIS_ WINBOOL *wrap_u, WINBOOL *wrap_v) PURE;
    STDMETHOD(InverseTransform)(THIS_ D3DVECTOR *d, D3DVECTOR *s) PURE;
    STDMETHOD(Load)(THIS_ void *filename, void *name, D3DRMLOADOPTIONS flags,
            D3DRMLOADTEXTURECALLBACK cb, void *ctx)PURE;
    STDMETHOD(LookAt)(THIS_ IDirect3DRMFrame *target, IDirect3DRMFrame *reference,
            D3DRMFRAMECONSTRAINT constraint) PURE;
    STDMETHOD(Move)(THIS_ D3DVALUE delta) PURE;
    STDMETHOD(DeleteChild)(THIS_ IDirect3DRMFrame *child) PURE;
    STDMETHOD(DeleteLight)(THIS_ struct IDirect3DRMLight *light) PURE;
    STDMETHOD(DeleteMoveCallback)(THIS_ D3DRMFRAMEMOVECALLBACK cb, void *ctx) PURE;
    STDMETHOD(DeleteVisual)(TH