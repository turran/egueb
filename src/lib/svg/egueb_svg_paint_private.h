/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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

#ifndef _EGUEB_SVG_PAINT_PRIVATE_H
#define _EGUEB_SVG_PAINT_PRIVATE_H

Eina_Bool egueb_svg_paint_string_from(Egueb_Svg_Paint *paint, const char *attr);
char * egueb_svg_paint_string_to(Egueb_Svg_Paint *thiz);
void egueb_svg_paint_interpolate(Egueb_Svg_Paint *v,
		Egueb_Svg_Paint *a, Egueb_Svg_Paint *b, double m,
		Egueb_Svg_Paint *add, Egueb_Svg_Paint *acc, int mul);
Eina_Bool egueb_svg_paint_is_equal(const Egueb_Svg_Paint *p1,
		const Egueb_Svg_Paint *p2);
void egueb_svg_paint_copy(const Egueb_Svg_Paint *thiz, Egueb_Svg_Paint *copy, Eina_Bool full);

#endif
