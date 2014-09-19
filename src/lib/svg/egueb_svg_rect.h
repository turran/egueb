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

#ifndef _EGUEB_SVG_RECT_H
#define _EGUEB_SVG_RECT_H

/**
 * @file
 * @ender_group{Egueb_Svg_Rect}
 * @ender_group{Egueb_Svg_Rect_Animated}
 */

/**
 * @defgroup Egueb_Svg_Rect_Definitions Rect definitions
 * @ingroup Egueb_Svg_Rect
 * @{
 */

typedef Enesim_Rectangle Egueb_Svg_Rect;

/**
 * @}
 * @defgroup Egueb_Svg_Rect_Animated Rect animated
 * @ingroup Egueb_Svg_Type_Group
 * @{
 */

typedef struct _Egueb_Svg_Rect_Animated
{
	Egueb_Svg_Rect base;
	Egueb_Svg_Rect anim;
} Egueb_Svg_Rect_Animated;

/**
 * @}
 */

#endif
