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
#ifndef _EGUEB_SVG_NUMBER_H
#define _EGUEB_SVG_NUMBER_H

typedef double Egueb_Svg_Number;

typedef struct _Egueb_Svg_Number_Animated
{
	Egueb_Svg_Number base;
	Egueb_Svg_Number anim;
} Egueb_Svg_Number_Animated;

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_number_descriptor_get(void);
EAPI Eina_Bool egueb_svg_number_string_from(Egueb_Svg_Number *thiz, const char *attr_val);
EAPI char * egueb_svg_number_string_to(Egueb_Svg_Number thiz);

#endif
