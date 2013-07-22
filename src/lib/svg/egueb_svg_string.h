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
#ifndef _EGUEB_SVG_STRING_H
#define _EGUEB_SVG_STRING_H

typedef struct _Egueb_Svg_String_Animated
{
	Egueb_Dom_String *base;
	Egueb_Dom_String *anim;
} Egueb_Svg_String_Animated;

/* TODO make this function work with a dom string instead */
EAPI Eina_Bool egueb_svg_string_is_equal(const char *s1, const char *s2);
EAPI void egueb_svg_string_animated_cleanup(Egueb_Svg_String_Animated *anim);

#endif

