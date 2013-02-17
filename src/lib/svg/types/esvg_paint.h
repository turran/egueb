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
#ifndef _ESVG_PAINT_H
#define _ESVG_PAINT_H

typedef enum _Esvg_Paint_Type
{
	ESVG_PAINT_TYPE_NONE,
	ESVG_PAINT_TYPE_CURRENT_COLOR,
	ESVG_PAINT_TYPE_COLOR,
	ESVG_PAINT_TYPE_SERVER,
	ESVG_PAINT_TYPES,
} Esvg_Paint_Type;

typedef struct _Esvg_Paint
{
	Esvg_Paint_Type type;
	Esvg_Color color;
	char *uri;
} Esvg_Paint;

typedef struct _Esvg_Paint_Animated
{
	Esvg_Paint base;
	Esvg_Paint anim;
} Esvg_Paint_Animated;

EAPI Eina_Bool esvg_paint_string_from(Esvg_Paint *paint, const char *attr);
EAPI Eina_Bool esvg_paint_is_equal(const Esvg_Paint *p1,
		const Esvg_Paint *p2);
#endif
