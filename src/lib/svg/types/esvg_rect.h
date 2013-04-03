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
#ifndef _ESVG_RECT_H
#define _ESVG_RECT_H

typedef Enesim_Rectangle Esvg_Rect;

typedef struct _Esvg_Rect_Animated
{
	Esvg_Rect base;
	Esvg_Rect anim;
} Esvg_Rect_Animated;

Eina_Bool esvg_rect_string_from(Esvg_Rect *thiz, const char *attr_val);

#endif