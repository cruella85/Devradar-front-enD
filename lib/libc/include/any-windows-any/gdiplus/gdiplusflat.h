/*
 * gdiplusflat.h
 *
 * GDI+ Flat API
 *
 * This file is part of the w32api package.
 *
 * Contributors:
 *   Created by Markus Koenig <markus@stber-koenig.de>
 *
 * THIS SOFTWARE IS NOT COPYRIGHTED
 *
 * This source code is offered for use in the public domain. You may
 * use, modify or distribute it freely.
 *
 * This code is distributed in the hope that it will be useful but
 * WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 * DISCLAIMED. This includes but is not limited to warranties of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef __GDIPLUS_FLAT_H
#define __GDIPLUS_FLAT_H
#if __GNUC__ >=3
#pragma GCC system_header
#endif

#ifdef __cplusplus
namespace DllExports {
extern "C" {
#endif

/* AdjustableArrowCap functions */
GpStatus WINGDIPAPI GdipCreateAdjustableArrowCap(REAL,REAL,BOOL,GpAdjustableArrowCap**);
GpStatus WINGDIPAPI GdipSetAdjustableArrowCapHeight(GpAdjustableArrowCap*,REAL);
GpStatus WINGDIPAPI GdipGetAdjustableArrowCapHeight(GpAdjustableArrowCap*,REAL*);
GpStatus WINGDIPAPI GdipSetAdjustableArrowCapWidth(GpAdjustableArrowCap*,REAL);
GpStatus WINGDIPAPI GdipGetAdjustableArrowCapWidth(GpAdjustableArrowCap*,REAL*);
GpStatus WINGDIPAPI GdipSetAdjustableArrowCapMiddleInset(GpAdjustableArrowCap*,REAL);
GpStatus WINGDIPAPI GdipGetAdjustableArrowCapMiddleInset(GpAdjustableArrowCap*,REAL*);
GpStatus WINGDIPAPI GdipSetAdjustableArrowCapFillState(GpAdjustableArrowCap*,BOOL);
GpStatus WINGDIPAPI GdipGetAdjustableArrowCapFillState(GpAdjustableArrowCap*,BOOL*);

/* Bitmap functions */
GpStatus WINGDIPAPI GdipCreateBitmapFromStream(IStream*,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateBitmapFromFile(GDIPCONST WCHAR*,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateBitmapFromStreamICM(IStream*,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateBitmapFromFileICM(GDIPCONST WCHAR*,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateBitmapFromScan0(INT,INT,INT,PixelFormat,BYTE*,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateBitmapFromGraphics(INT,INT,GpGraphics*,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateBitmapFromDirectDrawSurface(IDirectDrawSurface7*,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateBitmapFromGdiDib(GDIPCONST BITMAPINFO*,VOID*,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateBitmapFromHBITMAP(HBITMAP,HPALETTE,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateHBITMAPFromBitmap(GpBitmap*,HBITMAP*,ARGB);
GpStatus WINGDIPAPI GdipCreateBitmapFromHICON(HICON,GpBitmap**);
GpStatus WINGDIPAPI GdipCreateHICONFromBitmap(GpBitmap*,HICON*);
GpStatus WINGDIPAPI GdipCreateBitmapFromResource(HINSTANCE,GDIPCONST WCHAR*,GpBitmap**);
GpStatus WINGDIPAPI GdipCloneBitmapArea(REAL,REAL,REAL,REAL,PixelFormat,GpBitmap*,GpBitmap**);
GpStatus WINGDIPAPI GdipCloneBitmapAreaI(INT,INT,INT,INT,PixelFormat,GpBitmap*,GpBitmap**);
GpStatus WINGDIPAPI GdipBitmapLockBits(GpBitmap*,GDIPCONST GpRect*,UINT,PixelFormat,BitmapData*);
GpStatus WINGDIPAPI GdipBitmapUnlockBits(GpBitmap*,BitmapData*);
GpStatus WINGDIPAPI GdipBitmapGetPixel(GpBitmap*,INT,INT,ARGB*);
GpStatus WINGDIPAPI GdipBitmapSetPixel(GpBitmap*,INT,INT,ARGB);
GpStatus WINGDIPAPI GdipBitmapSetResolution(GpBitmap*,REAL,REAL);
GpStatus WINGDIPAPI GdipBitmapConvertFormat(GpBitmap*,PixelFormat,DitherType,PaletteType,ColorPalette*,REAL);
GpStatus WINGDIPAPI GdipInitializePalette(ColorPalette*,PaletteType,INT,BOOL,GpBitmap*);
GpStatus WINGDIPAPI GdipBitmapApplyEffect(GpBitmap*,CGpEffect*,RECT*,BOOL,VOID**,INT*);
GpStatus WINGDIPAPI GdipBitmapCreateApplyEffect(GpBitmap**,INT,CGpEffect*,RECT*,RECT*,GpBitmap**,BOOL,VOID**,INT*);
GpStatus WINGDIPAPI GdipBitmapGetHistogram(GpBitmap*,HistogramFormat,UINT,UINT*,UINT*,UINT*,UINT*);
GpStatus WINGDIPAPI GdipBitmapGetHistogramSize(HistogramFormat,UINT*);

/* Brush functions */
GpStatus WINGDIPAPI GdipCloneBrush(GpBrush*,GpBrush**);
GpStatus WINGDIPAPI GdipDeleteBrush(GpBrush*);
GpStatus WINGDIPAPI GdipGetBrushType(GpBrush*,GpBrushType*);

/* CachedBitmap functions */
GpStatus WINGDIPAPI GdipCreateCachedBitmap(GpBitmap*,GpGraphics*,GpCachedBitmap**);
GpStatus WINGDIPAPI GdipDeleteCachedBitmap(GpCachedBitmap*);
GpStatus WINGDIPAPI GdipDrawCachedBitmap(GpGraphics*,GpCachedBitmap*,INT,INT);

/* CustomLineCap functions */
GpStatus WINGDIPAPI GdipCreateCustomLineCap(GpPath*,GpPath*,GpLineCap,REAL,GpCustomLineCap**);
GpStatus WINGDIPAPI GdipDeleteCustomLineCap(GpCustomLineCap*);
GpStatus WINGDIPAPI GdipCloneCustomLineCap(GpCustomLineCap*,GpCustomLineCap**);
GpStatus WINGDIPAPI GdipGetCustomLineCapType(GpCustomLineCap*,CustomLineCapType*);
GpStatus WINGDIPAPI GdipSetCustomLineCapStrokeCaps(GpCustomLineCap*,GpLineCap,GpLineCap);
GpStatus WINGDIPAPI GdipGetCustomLineCapStrokeCaps(GpCustomLineCap*,GpLineCap*,GpLineCap*);
GpStatus WINGDIPAPI GdipSetCustomLineCapStrokeJoin(GpCustomLineCap*,GpLineJoin);
GpStatus WINGDIPAPI GdipGetCustomLineCapStrokeJoin(GpCustomLineCap*,GpLineJoin*);
GpStatus WINGDIPAPI GdipSetCustomLineCapBaseCap(GpCustomLineCap*,GpLineCap);
GpStatus WINGDIPAPI GdipGetCustomLineCapBaseCap(GpCustomLineCap*,GpLineCap*);
GpStatus WINGDIPAPI GdipSetCustomLineCapBaseInset(GpCustomLineCap*,REAL);
GpStatus WINGDIPAPI GdipGetCustomLineCapBaseInset(GpCustomLineCap*,REAL*);
GpStatus WINGDIPAPI GdipSetCustomLineCapWidthScale(GpCustomLineCap*,REAL);
GpStatus WINGDIPAPI GdipGetCustomLineCapWidthScale(GpCustomLineCap*,REAL*);

/* Effect functions */
GpStatus WINGDIPAPI GdipCreateEffect(GDIPCONST GUID,CGpEffect**);
GpStatus WINGDIPAPI GdipDeleteEffect(CGpEffect*);
GpStatus WINGDIPAPI GdipGetEffectParameterSize(CGpEffect*,UINT*);
GpStatus WINGDIPAPI GdipSetEffectParameters(CGpEffect*,GDIPCONST VOID*,UINT);
GpStatus WINGDIPAPI GdipGetEffectParameters(CGpEffect*,UINT*,VOID*);

/* Font functions */
GpStatus WINGDIPAPI GdipCreateFontFromDC(HDC,GpFont**);
GpStatus WINGDIPAPI GdipCreateFontFromLogfontA(HDC,GDIPCONST LOGFONTA*,GpFont**);
GpStatus WINGDIPAPI GdipCreateFontFromLogfontW(HDC,GDIPCONST LOGFONTW*,GpFont**);
GpStatus WINGDIPAPI GdipCreateFont(GDIPCONST GpFontFamily*,REAL,INT,Unit,GpFont**);
GpStatus WINGDIPAPI GdipCloneFont(GpFont*,GpFont**);
GpStatus WINGDIPAPI GdipDeleteFont(GpFont*);
GpStatus WINGDIPAPI GdipGetFamily(GpFont*,GpFontFamily**);
GpStatus WINGDIPAPI GdipGetFontStyle(GpFont*,INT*);
GpStatus WINGDIPAPI GdipGetFontSize(GpFont*,REAL*);
GpStatus WINGDIPAPI GdipGetFontUnit(GpFont*,Unit*);
GpStatus WINGDIPAPI GdipGetFontHeight(GDIPCONST GpFont*,GDIPCONST GpGraphics*,REAL*);
GpStatus WINGDIPAPI GdipGetFontHeightGivenDPI(GDIPCONST GpFont*,REAL,REAL*);
GpStatus WINGDIPAPI GdipGetLogFontA(GpFont*,GpGraphics*,LOGFONTA*);
GpStatus WINGDIPAPI GdipGetLogFontW(GpFont*,GpGraphics*,LOGFONTW*);
GpStatus WINGDIPAPI GdipNewInstalledFontCollection(GpFontCollection**);
GpStatus WINGDIPAPI GdipNewPrivateFontCollection(GpFontCollection**);
GpStatus WINGDIPAPI GdipDeletePrivateFontCollection(GpFontCollection**);
GpStatus WINGDIPAPI GdipGetFontCollectionFamilyCount(GpFontCollection*,INT*);
GpStatus WINGDIPAPI GdipGetFontCollectionFamilyList(GpFontCollection*,INT,GpFontFamily**,INT*);
GpStatus WINGDIPAPI GdipPrivateAddFontFile(GpFontCollection*,GDIPCONST WCHAR*);
GpStatus WINGDIPAPI GdipPrivateAddMemoryFont(GpFontCollection*,GDIPCONST void*,INT);

/* FontFamily functions */
GpStatus WINGDIPAPI GdipCreateFontFamilyFromName(GDIPCONST WCHAR*,GpFontCollection*,GpFontFamily**);
GpStatus WINGDIPAPI GdipDeleteFontFamily(GpFontFamily*);
GpStatus WINGDIPAPI GdipCloneFontFamily(GpFontFamily*,GpFontFamily**);
GpStatus WINGDIPAPI GdipGetGenericFontFamilySansSerif(GpFontFamily**);
GpStatus WINGDIPAPI GdipGetGenericFontFamilySerif(GpFontFamily**);
GpStatus WINGDIPAPI GdipGetGenericFontFamilyMonospace(GpFontFamily**);
GpStatus WINGDIPAPI GdipGetFamilyName(GDIPCONST GpFontFamily*,WCHAR[LF_FACESIZE],LANGID);
GpStatus WINGDIPAPI GdipIsStyleAvailable(GDIPCONST GpFontFamily*,INT,BOOL*);
GpStatus WINGDIPAPI GdipFontCollectionEnumerable(GpFontCollection*,GpGraphics*,INT*);
GpStatus WINGDIPAPI GdipFontCollectionEnumerate(GpFontCollection*,INT,GpFontFamily**,INT*,GpGraphics*);
GpStatus WINGDIPAPI GdipGetEmHeight(GDIPCONST GpFontFamily*,INT,UINT16*);
GpStatus WINGDIPAPI GdipGetCellAscent(GDIPCONST GpFontFamily*,INT,UINT16*);
GpStatus WINGDIPAPI GdipGetCellDescent(GDIPCONST GpFontFamily*,INT,UINT16*);
GpStatus WINGDIPAPI GdipGetLineSpacing(GDIPCONST GpFontFamily*,INT,UINT16*);

/* Graphics functions */
GpStatus WINGDIPAPI GdipFlush(GpGraphics*,GpFlushIntention);
GpStatus WINGDIPAPI GdipCreateFromHDC(HDC,GpGraphics**);
GpStatus WINGDIPAPI GdipCreateFromHDC2(HDC,HANDLE,GpGraphics**);
GpStatus WINGDIPAPI GdipCreateFromHWND(HWND