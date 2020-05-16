
/*
 * gdiplusenums.h
 *
 * GDI+ enumerations
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

#ifndef __GDIPLUS_ENUMS_H
#define __GDIPLUS_ENUMS_H
#if __GNUC__ >=3
#pragma GCC system_header
#endif

typedef enum BrushType {
	BrushTypeSolidColor = 0,
	BrushTypeHatchFill = 1,
	BrushTypeTextureFill = 2,
	BrushTypePathGradient = 3,
	BrushTypeLinearGradient = 4
} BrushType;

typedef enum CombineMode {
	CombineModeReplace = 0,
	CombineModeIntersect = 1,
	CombineModeUnion = 2,
	CombineModeXor = 3,
	CombineModeExclude = 4,
	CombineModeComplement = 5
} CombineMode;

typedef enum CompositingMode {
	CompositingModeSourceOver = 0,
	CompositingModeSourceCopy = 1
} CompositingMode;

typedef enum CompositingQuality {
	CompositingQualityDefault = 0,