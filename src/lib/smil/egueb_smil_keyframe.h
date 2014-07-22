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

#ifndef _EGUEB_SMIL_KEYFRAME_H_
#define _EGUEB_SMIL_KEYFRAME_H_

/**
 * Possible interpolator types
 */
typedef enum _Egueb_Smil_Keyframe_Interpolator_Type
{
	EGUEB_SMIL_KEYFRAME_INTERPOLATOR_DISCRETE, /**< The values are not interpolated, just discrete values */
	EGUEB_SMIL_KEYFRAME_INTERPOLATOR_LINEAR, /**< Linear interpolation */
	EGUEB_SMIL_KEYFRAME_INTERPOLATOR_COSIN, /***< Cosin interpolation */
	EGUEB_SMIL_KEYFRAME_INTERPOLATOR_QUADRATIC, /**< Quadratic bezier interpolation */
	EGUEB_SMIL_KEYFRAME_INTERPOLATOR_CUBIC, /**< Cubic bezier interpolation */
	EGUEB_SMIL_KEYFRAME_INTERPOLATOR_TYPES
} Egueb_Smil_Keyframe_Interpolator_Type;

#endif
