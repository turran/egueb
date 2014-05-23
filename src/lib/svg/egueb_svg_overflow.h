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
#ifndef _EGUEB_SVG_OVERFLOW_H
#define _EGUEB_SVG_OVERFLOW_H

typedef enum _Egueb_Svg_Overflow
{
	EGUEB_SVG_OVERFLOW_VISIBLE,
	EGUEB_SVG_OVERFLOW_HIDDEN,
	EGUEB_SVG_OVERFLOW_SCROLL,
	EGUEB_SVG_OVERFLOW_AUTO,
} Egueb_Svg_Overflow;

typedef struct _Egueb_Svg_Overflow_Animated
{
	Egueb_Svg_Overflow base;
	Egueb_Svg_Overflow anim;
} Egueb_Svg_Overflow_Animated;

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_overflow_descriptor_get(void);
EAPI Eina_Bool egueb_svg_overflow_string_from(Egueb_Svg_Overflow *thiz, const char *value);
EAPI char * egueb_svg_overflow_string_to(Egueb_Svg_Overflow thiz);

#endif

