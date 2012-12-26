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
#ifndef _ESVG_COLOR_H
#define _ESVG_COLOR_H

/* We need to use uint32_t here because we can not pass
 * data < 32bits through varargs
 */
typedef struct _Esvg_Color
{
	uint32_t r;
	uint32_t g;
	uint32_t b;
} Esvg_Color;

typedef struct _Esvg_Animated_Color
{
	Esvg_Color base;
	Esvg_Color anim;
} Esvg_Animated_Color;

EAPI Eina_Bool esvg_color_string_from(Esvg_Color *color, const char *attr_val);
EAPI Eina_Bool esvg_color_is_equal(const Esvg_Color *c1, const Esvg_Color *c2);

#endif
