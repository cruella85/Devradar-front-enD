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
GpStatus WINGDIPAPI GdipCreateFromHWND(HWND,GpGraphics**);
GpStatus WINGDIPAPI GdipCreateFromHWNDICM(HWND,GpGraphics**);
GpStatus WINGDIPAPI GdipDeleteGraphics(GpGraphics*);
GpStatus WINGDIPAPI GdipGetDC(GpGraphics*,HDC*);
GpStatus WINGDIPAPI GdipReleaseDC(GpGraphics*,HDC);
GpStatus WINGDIPAPI GdipSetCompositingMode(GpGraphics*,CompositingMode);
GpStatus WINGDIPAPI GdipGetCompositingMode(GpGraphics*,CompositingMode*);
GpStatus WINGDIPAPI GdipSetRenderingOrigin(GpGraphics*,INT,INT);
GpStatus WINGDIPAPI GdipGetRenderingOrigin(GpGraphics*,INT*,INT*);
GpStatus WINGDIPAPI GdipSetCompositingQuality(GpGraphics*,CompositingQuality);
GpStatus WINGDIPAPI GdipGetCompositingQuality(GpGraphics*,CompositingQuality*);
GpStatus WINGDIPAPI GdipSetSmoothingMode(GpGraphics*,SmoothingMode);
GpStatus WINGDIPAPI GdipGetSmoothingMode(GpGraphics*,SmoothingMode*);
GpStatus WINGDIPAPI GdipSetPixelOffsetMode(GpGraphics*,PixelOffsetMode);
GpStatus WINGDIPAPI GdipGetPixelOffsetMode(GpGraphics*,PixelOffsetMode*);
GpStatus WINGDIPAPI GdipSetTextRenderingHint(GpGraphics*,TextRenderingHint);
GpStatus WINGDIPAPI GdipGetTextRenderingHint(GpGraphics*,TextRenderingHint*);
GpStatus WINGDIPAPI GdipSetTextContrast(GpGraphics*,UINT);
GpStatus WINGDIPAPI GdipGetTextContrast(GpGraphics*,UINT*);
GpStatus WINGDIPAPI GdipSetInterpolationMode(GpGraphics*,InterpolationMode);
GpStatus WINGDIPAPI GdipGraphicsSetAbort(GpGraphics*,GdiplusAbort*);
GpStatus WINGDIPAPI GdipGetInterpolationMode(GpGraphics*,InterpolationMode*);
GpStatus WINGDIPAPI GdipSetWorldTransform(GpGraphics*,GpMatrix*);
GpStatus WINGDIPAPI GdipResetWorldTransform(GpGraphics*);
GpStatus WINGDIPAPI GdipMultiplyWorldTransform(GpGraphics*,GDIPCONST GpMatrix*,GpMatrixOrder);
GpStatus WINGDIPAPI GdipTranslateWorldTransform(GpGraphics*,REAL,REAL,GpMatrixOrder);
GpStatus WINGDIPAPI GdipScaleWorldTransform(GpGraphics*,REAL,REAL,GpMatrixOrder);
GpStatus WINGDIPAPI GdipRotateWorldTransform(GpGraphics*,REAL,GpMatrixOrder);
GpStatus WINGDIPAPI GdipGetWorldTransform(GpGraphics*,GpMatrix*);
GpStatus WINGDIPAPI GdipResetPageTransform(GpGraphics*);
GpStatus WINGDIPAPI GdipGetPageUnit(GpGraphics*,GpUnit*);
GpStatus WINGDIPAPI GdipGetPageScale(GpGraphics*,REAL*);
GpStatus WINGDIPAPI GdipSetPageUnit(GpGraphics*,GpUnit);
GpStatus WINGDIPAPI GdipSetPageScale(GpGraphics*,REAL);
GpStatus WINGDIPAPI GdipGetDpiX(GpGraphics*,REAL*);
GpStatus WINGDIPAPI GdipGetDpiY(GpGraphics*,REAL*);
GpStatus WINGDIPAPI GdipTransformPoints(GpGraphics*,GpCoordinateSpace,GpCoordinateSpace,GpPointF*,INT);
GpStatus WINGDIPAPI GdipTransformPointsI(GpGraphics*,GpCoordinateSpace,GpCoordinateSpace,GpPoint*,INT);
GpStatus WINGDIPAPI GdipGetNearestColor(GpGraphics*,ARGB*);
HPALETTE WINGDIPAPI GdipCreateHalftonePalette(void);
GpStatus WINGDIPAPI GdipDrawLine(GpGraphics*,GpPen*,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawLineI(GpGraphics*,GpPen*,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipDrawLines(GpGraphics*,GpPen*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipDrawLinesI(GpGraphics*,GpPen*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipDrawArc(GpGraphics*,GpPen*,REAL,REAL,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawArcI(GpGraphics*,GpPen*,INT,INT,INT,INT,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawBezier(GpGraphics*,GpPen*,REAL,REAL,REAL,REAL,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawBezierI(GpGraphics*,GpPen*,INT,INT,INT,INT,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipDrawBeziers(GpGraphics*,GpPen*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipDrawBeziersI(GpGraphics*,GpPen*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipDrawRectangle(GpGraphics*,GpPen*,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawRectangleI(GpGraphics*,GpPen*,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipDrawRectangles(GpGraphics*,GpPen*,GDIPCONST GpRectF*,INT);
GpStatus WINGDIPAPI GdipDrawRectanglesI(GpGraphics*,GpPen*,GDIPCONST GpRect*,INT);
GpStatus WINGDIPAPI GdipDrawEllipse(GpGraphics*,GpPen*,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawEllipseI(GpGraphics*,GpPen*,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipDrawPie(GpGraphics*,GpPen*,REAL,REAL,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawPieI(GpGraphics*,GpPen*,INT,INT,INT,INT,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawPolygon(GpGraphics*,GpPen*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipDrawPolygonI(GpGraphics*,GpPen*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipDrawPath(GpGraphics*,GpPen*,GpPath*);
GpStatus WINGDIPAPI GdipDrawCurve(GpGraphics*,GpPen*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipDrawCurveI(GpGraphics*,GpPen*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipDrawCurve2(GpGraphics*,GpPen*,GDIPCONST GpPointF*,INT,REAL);
GpStatus WINGDIPAPI GdipDrawCurve2I(GpGraphics*,GpPen*,GDIPCONST GpPoint*,INT,REAL);
GpStatus WINGDIPAPI GdipDrawCurve3(GpGraphics*,GpPen*,GDIPCONST GpPointF*,INT,INT,INT,REAL);
GpStatus WINGDIPAPI GdipDrawCurve3I(GpGraphics*,GpPen*,GDIPCONST GpPoint*,INT,INT,INT,REAL);
GpStatus WINGDIPAPI GdipDrawClosedCurve(GpGraphics*,GpPen*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipDrawClosedCurveI(GpGraphics*,GpPen*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipDrawClosedCurve2(GpGraphics*,GpPen*,GDIPCONST GpPointF*,INT,REAL);
GpStatus WINGDIPAPI GdipDrawClosedCurve2I(GpGraphics*,GpPen*,GDIPCONST GpPoint*,INT,REAL);
GpStatus WINGDIPAPI GdipGraphicsClear(GpGraphics*,ARGB);
GpStatus WINGDIPAPI GdipFillRectangle(GpGraphics*,GpBrush*,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipFillRectangleI(GpGraphics*,GpBrush*,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipFillRectangles(GpGraphics*,GpBrush*,GDIPCONST GpRectF*,INT);
GpStatus WINGDIPAPI GdipFillRectanglesI(GpGraphics*,GpBrush*,GDIPCONST GpRect*,INT);
GpStatus WINGDIPAPI GdipFillPolygon(GpGraphics*,GpBrush*,GDIPCONST GpPointF*,INT,GpFillMode);
GpStatus WINGDIPAPI GdipFillPolygonI(GpGraphics*,GpBrush*,GDIPCONST GpPoint*,INT,GpFillMode);
GpStatus WINGDIPAPI GdipFillPolygon2(GpGraphics*,GpBrush*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipFillPolygon2I(GpGraphics*,GpBrush*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipFillEllipse(GpGraphics*,GpBrush*,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipFillEllipseI(GpGraphics*,GpBrush*,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipFillPie(GpGraphics*,GpBrush*,REAL,REAL,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipFillPieI(GpGraphics*,GpBrush*,INT,INT,INT,INT,REAL,REAL);
GpStatus WINGDIPAPI GdipFillPath(GpGraphics*,GpBrush*,GpPath*);
GpStatus WINGDIPAPI GdipFillClosedCurve(GpGraphics*,GpBrush*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipFillClosedCurveI(GpGraphics*,GpBrush*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipFillClosedCurve2(GpGraphics*,GpBrush*,GDIPCONST GpPointF*,INT,REAL,GpFillMode);
GpStatus WINGDIPAPI GdipFillClosedCurve2I(GpGraphics*,GpBrush*,GDIPCONST GpPoint*,INT,REAL,GpFillMode);
GpStatus WINGDIPAPI GdipFillRegion(GpGraphics*,GpBrush*,GpRegion*);
GpStatus WINGDIPAPI GdipDrawImage(GpGraphics*,GpImage*,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawImageI(GpGraphics*,GpImage*,INT,INT);
GpStatus WINGDIPAPI GdipDrawImageRect(GpGraphics*,GpImage*,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipDrawImageRectI(GpGraphics*,GpImage*,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipDrawImagePoints(GpGraphics*,GpImage*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipDrawImagePointsI(GpGraphics*,GpImage*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipDrawImagePointRect(GpGraphics*,GpImage*,REAL,REAL,REAL,REAL,REAL,REAL,GpUnit);
GpStatus WINGDIPAPI GdipDrawImagePointRectI(GpGraphics*,GpImage*,INT,INT,INT,INT,INT,INT,GpUnit);
GpStatus WINGDIPAPI GdipDrawImageRectRect(GpGraphics*,GpImage*,REAL,REAL,REAL,REAL,REAL,REAL,REAL,REAL,GpUnit,GDIPCONST GpImageAttributes*,DrawImageAbort,VOID*);
GpStatus WINGDIPAPI GdipDrawImageRectRectI(GpGraphics*,GpImage*,INT,INT,INT,INT,INT,INT,INT,INT,GpUnit,GDIPCONST GpImageAttributes*,DrawImageAbort,VOID*);
GpStatus WINGDIPAPI GdipDrawImagePointsRect(GpGraphics*,GpImage*,GDIPCONST GpPointF*,INT,REAL,REAL,REAL,REAL,GpUnit,GDIPCONST GpImageAttributes*,DrawImageAbort,VOID*);
GpStatus WINGDIPAPI GdipDrawImagePointsRectI(GpGraphics*,GpImage*,GDIPCONST GpPoint*,INT,INT,INT,INT,INT,GpUnit,GDIPCONST GpImageAttributes*,DrawImageAbort,VOID*);
GpStatus WINGDIPAPI GdipDrawImageFX(GpGraphics*,GpImage*,GpRectF*,GpMatrix*,CGpEffect*,GpImageAttributes*,GpUnit);
#ifdef __cplusplus
GpStatus WINGDIPAPI GdipEnumerateMetafileDestPoint(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST PointF&,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
GpStatus WINGDIPAPI GdipEnumerateMetafileDestPointI(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST Point&,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
GpStatus WINGDIPAPI GdipEnumerateMetafileDestRect(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST RectF&,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
GpStatus WINGDIPAPI GdipEnumerateMetafileDestRectI(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST Rect&,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
#endif
GpStatus WINGDIPAPI GdipEnumerateMetafileDestPoints(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST PointF*,INT,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
GpStatus WINGDIPAPI GdipEnumerateMetafileDestPointsI(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST Point*,INT,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
#ifdef __cplusplus
GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestPoint(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST PointF&,GDIPCONST RectF&,Unit,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestPointI(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST Point&,GDIPCONST Rect&,Unit,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestRect(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST RectF&,GDIPCONST RectF&,Unit,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestRectI(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST Rect&,GDIPCONST Rect&,Unit,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestPoints(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST PointF*,INT,GDIPCONST RectF&,Unit,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
GpStatus WINGDIPAPI GdipEnumerateMetafileSrcRectDestPointsI(GpGraphics*,GDIPCONST GpMetafile*,GDIPCONST Point*,INT,GDIPCONST Rect&,Unit,EnumerateMetafileProc,VOID*,GDIPCONST GpImageAttributes*);
#endif
GpStatus WINGDIPAPI GdipSetClipGraphics(GpGraphics*,GpGraphics*,CombineMode);
GpStatus WINGDIPAPI GdipSetClipRect(GpGraphics*,REAL,REAL,REAL,REAL,CombineMode);
GpStatus WINGDIPAPI GdipSetClipRectI(GpGraphics*,INT,INT,INT,INT,CombineMode);
GpStatus WINGDIPAPI GdipSetClipPath(GpGraphics*,GpPath*,CombineMode);
GpStatus WINGDIPAPI GdipSetClipRegion(GpGraphics*,GpRegion*,CombineMode);
GpStatus WINGDIPAPI GdipSetClipHrgn(GpGraphics*,HRGN,CombineMode);
GpStatus WINGDIPAPI GdipResetClip(GpGraphics*);
GpStatus WINGDIPAPI GdipTranslateClip(GpGraphics*,REAL,REAL);
GpStatus WINGDIPAPI GdipTranslateClipI(GpGraphics*,INT,INT);
GpStatus WINGDIPAPI GdipGetClip(GpGraphics*,GpRegion*);
GpStatus WINGDIPAPI GdipGetClipBounds(GpGraphics*,GpRectF*);
GpStatus WINGDIPAPI GdipGetClipBoundsI(GpGraphics*,GpRect*);
GpStatus WINGDIPAPI GdipIsClipEmpty(GpGraphics*,BOOL*);
GpStatus WINGDIPAPI GdipGetVisibleClipBounds(GpGraphics*,GpRectF*);
GpStatus WINGDIPAPI GdipGetVisibleClipBoundsI(GpGraphics*,GpRect*);
GpStatus WINGDIPAPI GdipIsVisibleClipEmpty(GpGraphics*,BOOL*);
GpStatus WINGDIPAPI GdipIsVisiblePoint(GpGraphics*,REAL,REAL,BOOL*);
GpStatus WINGDIPAPI GdipIsVisiblePointI(GpGraphics*,INT,INT,BOOL*);
GpStatus WINGDIPAPI GdipIsVisibleRect(GpGraphics*,REAL,REAL,REAL,REAL,BOOL*);
GpStatus WINGDIPAPI GdipIsVisibleRectI(GpGraphics*,INT,INT,INT,INT,BOOL*);
GpStatus WINGDIPAPI GdipSaveGraphics(GpGraphics*,GraphicsState*);
GpStatus WINGDIPAPI GdipRestoreGraphics(GpGraphics*,GraphicsState);
GpStatus WINGDIPAPI GdipBeginContainer(GpGraphics*,GDIPCONST GpRectF*,GDIPCONST GpRectF*,GpUnit,GraphicsContainer*);
GpStatus WINGDIPAPI GdipBeginContainerI(GpGraphics*,GDIPCONST GpRect*,GDIPCONST GpRect*,GpUnit,GraphicsContainer*);
GpStatus WINGDIPAPI GdipBeginContainer2(GpGraphics*,GraphicsContainer*);
GpStatus WINGDIPAPI GdipEndContainer(GpGraphics*,GraphicsContainer);
GpStatus WINGDIPAPI GdipComment(GpGraphics*,UINT,GDIPCONST BYTE*);

/* GraphicsPath functions */
GpStatus WINGDIPAPI GdipCreatePath(GpFillMode,GpPath**);
GpStatus WINGDIPAPI GdipCreatePath2(GDIPCONST GpPointF*,GDIPCONST BYTE*,INT,GpFillMode,GpPath**);
GpStatus WINGDIPAPI GdipCreatePath2I(GDIPCONST GpPoint*,GDIPCONST BYTE*,INT,GpFillMode,GpPath**);
GpStatus WINGDIPAPI GdipClonePath(GpPath*,GpPath**);
GpStatus WINGDIPAPI GdipDeletePath(GpPath*);
GpStatus WINGDIPAPI GdipResetPath(GpPath*);
GpStatus WINGDIPAPI GdipGetPointCount(GpPath*,INT*);
GpStatus WINGDIPAPI GdipGetPathTypes(GpPath*,BYTE*,INT);
GpStatus WINGDIPAPI GdipGetPathPoints(GpPath*,GpPointF*,INT);
GpStatus WINGDIPAPI GdipGetPathPointsI(GpPath*,GpPoint*,INT);
GpStatus WINGDIPAPI GdipGetPathFillMode(GpPath*,GpFillMode*);
GpStatus WINGDIPAPI GdipSetPathFillMode(GpPath*,GpFillMode);
GpStatus WINGDIPAPI GdipGetPathData(GpPath*,GpPathData*);
GpStatus WINGDIPAPI GdipStartPathFigure(GpPath*);
GpStatus WINGDIPAPI GdipClosePathFigure(GpPath*);
GpStatus WINGDIPAPI GdipClosePathFigures(GpPath*);
GpStatus WINGDIPAPI GdipSetPathMarker(GpPath*);
GpStatus WINGDIPAPI GdipClearPathMarkers(GpPath*);
GpStatus WINGDIPAPI GdipReversePath(GpPath*);
GpStatus WINGDIPAPI GdipGetPathLastPoint(GpPath*,GpPointF*);
GpStatus WINGDIPAPI GdipAddPathLine(GpPath*,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipAddPathLine2(GpPath*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipAddPathArc(GpPath*,REAL,REAL,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipAddPathBezier(GpPath*,REAL,REAL,REAL,REAL,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipAddPathBeziers(GpPath*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipAddPathCurve(GpPath*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipAddPathCurve2(GpPath*,GDIPCONST GpPointF*,INT,REAL);
GpStatus WINGDIPAPI GdipAddPathCurve3(GpPath*,GDIPCONST GpPointF*,INT,INT,INT,REAL);
GpStatus WINGDIPAPI GdipAddPathClosedCurve(GpPath*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipAddPathClosedCurve2(GpPath*,GDIPCONST GpPointF*,INT,REAL);
GpStatus WINGDIPAPI GdipAddPathRectangle(GpPath*,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipAddPathRectangles(GpPath*,GDIPCONST GpRectF*,INT);
GpStatus WINGDIPAPI GdipAddPathEllipse(GpPath*,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipAddPathPie(GpPath*,REAL,REAL,REAL,REAL,REAL,REAL);
GpStatus WINGDIPAPI GdipAddPathPolygon(GpPath*,GDIPCONST GpPointF*,INT);
GpStatus WINGDIPAPI GdipAddPathPath(GpPath*,GDIPCONST GpPath*,BOOL);
GpStatus WINGDIPAPI GdipAddPathString(GpPath*,GDIPCONST WCHAR*,INT,GDIPCONST GpFontFamily*,INT,REAL,GDIPCONST RectF*,GDIPCONST GpStringFormat*);
GpStatus WINGDIPAPI GdipAddPathStringI(GpPath*,GDIPCONST WCHAR*,INT,GDIPCONST GpFontFamily*,INT,REAL,GDIPCONST Rect*,GDIPCONST GpStringFormat*);
GpStatus WINGDIPAPI GdipAddPathLineI(GpPath*,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipAddPathLine2I(GpPath*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipAddPathArcI(GpPath*,INT,INT,INT,INT,REAL,REAL);
GpStatus WINGDIPAPI GdipAddPathBezierI(GpPath*,INT,INT,INT,INT,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipAddPathBeziersI(GpPath*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipAddPathCurveI(GpPath*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipAddPathCurve2I(GpPath*,GDIPCONST GpPoint*,INT,REAL);
GpStatus WINGDIPAPI GdipAddPathCurve3I(GpPath*,GDIPCONST GpPoint*,INT,INT,INT,REAL);
GpStatus WINGDIPAPI GdipAddPathClosedCurveI(GpPath*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipAddPathClosedCurve2I(GpPath*,GDIPCONST GpPoint*,INT,REAL);
GpStatus WINGDIPAPI GdipAddPathRectangleI(GpPath*,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipAddPathRectanglesI(GpPath*,GDIPCONST GpRect*,INT);
GpStatus WINGDIPAPI GdipAddPathEllipseI(GpPath*,INT,INT,INT,INT);
GpStatus WINGDIPAPI GdipAddPathPieI(GpPath*,INT,INT,INT,INT,REAL,REAL);
GpStatus WINGDIPAPI GdipAddPathPolygonI(GpPath*,GDIPCONST GpPoint*,INT);
GpStatus WINGDIPAPI GdipFlattenPath(GpPath*,GpMatrix*,REAL);
GpStatus WINGDIPAPI GdipWindingModeOutline(GpPath*,GpMatrix*,REAL);
GpStatus WINGDIPAPI GdipWidenPath(GpPath*,GpPen*,GpMatrix*,REAL);
GpStatus WINGDIPAPI GdipWarpPath(GpPath*,GpMatrix*,GDIPCONST GpPointF*,INT,REAL,REAL,REAL,REAL,WarpMode,REAL);
GpStatus WINGDIPAPI GdipTransformPath(GpPath*,GpMatrix*);
GpStatus WINGDIPAPI GdipGetPathWorldBounds(GpPath*,GpRectF*,GDIPCONST GpMatrix*,GDIPCONST GpPen*);
GpStatus WINGDIPAPI GdipGetPathWorldBoundsI(GpPath*,GpRect*,GDIPCONST GpMatrix*,GDIPCONST GpPen*);
GpStatus WINGDIPAPI GdipIsVisiblePathPoint(GpPath*,REAL,REAL,GpGraphics*,BOOL*);
GpStatus WINGDIPAPI GdipIsVisiblePathPointI(GpPath*,INT,INT,GpGraphics*,BOOL*);
GpStatus WINGDIPAPI GdipIsOutlineVisiblePathPoint(GpPath*,REAL,REAL,GpPen*,GpGraphics*,BOOL*);
GpStatus WINGDIPAPI GdipIsOutlineVisiblePathPointI(GpPath*,INT,INT,GpPen*,GpGraphics*,BOOL*);

/* HatchBrush functions */
GpStatus WINGDIPAPI GdipCreateHatchBrush(GpHatchStyle,ARGB,ARGB,GpHatch**);
GpStatus WINGDIPAPI GdipGetHatchStyle(GpHatch*,GpHatchStyle*);
GpStatus WINGDIPAPI GdipGetHatchForegroundColor(GpHatch*,ARGB*);
GpStatus WINGDIPAPI GdipGetHatchBackgroundColor(GpHatch*,ARGB*);

/* Image functions */
GpStatus WINGDIPAPI GdipLoadImageFromStream(IStream*,GpImage**);
GpStatus WINGDIPAPI GdipLoadImageFromFile(GDIPCONST WCHAR*,GpImage**);
GpStatus WINGDIPAPI GdipLoadImageFromStreamICM(IStream*,GpImage**);
GpStatus WINGDIPAPI GdipLoadImageFromFileICM(GDIPCONST WCHAR*,GpImage**);
GpStatus WINGDIPAPI GdipCloneImage(GpImage*,GpImage**);
GpStatus WINGDIPAPI GdipDisposeImage(GpImage*);
GpStatus WINGDIPAPI GdipSaveImageToFile(GpI