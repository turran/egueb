/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
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
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _EGUEB_SMIL_KEYFRAME_PRIVATE_H_
#define _EGUEB_SMIL_KEYFRAME_PRIVATE_H_

/**
 * Specific data needed for quadratic bezier animations
 */
typedef struct _Egueb_Smil_Keyframe_Interpolator_Data_Quadratic
{
	/** Control point */
	double x0;
	double y0;
} Egueb_Smil_Keyframe_Interpolator_Data_Quadratic;

typedef struct _Egueb_Smil_Keyframe_Interpolator_Data_Cubic
{
	/** Control points */
	double x0;
	double y0;
	double x1;
	double y1;
} Egueb_Smil_Keyframe_Interpolator_Data_Cubic;

typedef union _Egueb_Smil_Keyframe_Interpolator_Data
{
	Egueb_Smil_Keyframe_Interpolator_Data_Quadratic q;
	Egueb_Smil_Keyframe_Interpolator_Data_Cubic c;
} Egueb_Smil_Keyframe_Interpolator_Data;

typedef struct _Egueb_Smil_Keyframe
{
	Egueb_Smil_Clock time;
	Egueb_Smil_Keyframe_Interpolator_Type type;
	Egueb_Smil_Keyframe_Interpolator_Data data;
	Egueb_Dom_Value *value;
} Egueb_Smil_Keyframe;

#endif
