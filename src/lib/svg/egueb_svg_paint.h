/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#ifndef _EGUEB_SVG_PAINT_H
#define _EGUEB_SVG_PAINT_H

#include "egueb_svg_color.h"

typedef enum _Egueb_Svg_Paint_Type
{
	EGUEB_SVG_PAINT_TYPE_NONE,
	EGUEB_SVG_PAINT_TYPE_CURRENT_COLOR,
	EGUEB_SVG_PAINT_TYPE_COLOR,
	EGUEB_SVG_PAINT_TYPE_SERVER,
	EGUEB_SVG_PAINT_TYPES,
} Egueb_Svg_Paint_Type;

typedef struct _Egueb_Svg_Paint
{
	Egueb_Svg_Paint_Type type;
	Egueb_Svg_Color color;
	char *uri;
} Egueb_Svg_Paint;

typedef struct _Egueb_Svg_Paint_Animated
{
	Egueb_Svg_Paint base;
	Egueb_Svg_Paint anim;
} Egueb_Svg_Paint_Animated;

#define EGUEB_SVG_PAINT_INIT { EGUEB_SVG_PAINT_TYPE_NONE, {0, 0, 0}, NULL }

EAPI extern const Egueb_Svg_Paint EGUEB_SVG_PAINT_BLACK;
EAPI extern const Egueb_Svg_Paint EGUEB_SVG_PAINT_NONE;
EAPI extern const Egueb_Svg_Paint EGUEB_SVG_PAINT_CURRENT_COLOR;

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_paint_descriptor_get(void);

EAPI Eina_Bool egueb_svg_paint_is_equal(const Egueb_Svg_Paint *p1,
		const Egueb_Svg_Paint *p2);
EAPI void egueb_svg_paint_copy(const Egueb_Svg_Paint *thiz, Egueb_Svg_Paint *copy, Eina_Bool full);
EAPI void egueb_svg_paint_reset(Egueb_Svg_Paint *thiz);

#endif
