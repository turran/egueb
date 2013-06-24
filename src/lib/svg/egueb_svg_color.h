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
#ifndef _EGUEB_SVG_COLOR_H
#define _EGUEB_SVG_COLOR_H

/* We need to use uint32_t here because we can not pass
 * data < 32bits through varargs
 */
typedef struct _Egueb_Svg_Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Egueb_Svg_Color;

typedef struct _Egueb_Svg_Color_Animated
{
	Egueb_Svg_Color base;
	Egueb_Svg_Color anim;
} Egueb_Svg_Color_Animated;

EAPI extern const Egueb_Svg_Color EGUEB_SVG_COLOR_BLACK;

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_color_descriptor_get(void);
EAPI Eina_Bool egueb_svg_color_string_from(Egueb_Svg_Color *color, const char *attr_val);
EAPI char * egueb_svg_color_string_to(Egueb_Svg_Color *thiz);
EAPI Eina_Bool egueb_svg_color_is_equal(const Egueb_Svg_Color *c1, const Egueb_Svg_Color *c2);
EAPI void egueb_svg_color_components_from(Egueb_Svg_Color *thiz, uint8_t r, uint8_t g, uint8_t b);

#endif
