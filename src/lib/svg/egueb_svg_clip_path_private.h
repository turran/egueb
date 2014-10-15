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

#ifndef _EGUEB_SVG_CLIP_PATH_PRIVATE_H
#define _EGUEB_SVG_CLIP_PATH_PRIVATE_H

Eina_Bool egueb_svg_clip_path_string_from(Egueb_Svg_Clip_Path *thiz,
		const char *str);
char * egueb_svg_clip_path_string_to(Egueb_Svg_Clip_Path *thiz);
void egueb_svg_clip_path_interpolate(Egueb_Svg_Clip_Path *v,
		Egueb_Svg_Clip_Path *a, Egueb_Svg_Clip_Path *b, double m,
		Egueb_Svg_Clip_Path *add, Egueb_Svg_Clip_Path *acc, int mul);
Eina_Bool egueb_svg_clip_path_is_equal(const Egueb_Svg_Clip_Path *p1,
		const Egueb_Svg_Clip_Path *p2);
void egueb_svg_clip_path_copy(const Egueb_Svg_Clip_Path *thiz,
		Egueb_Svg_Clip_Path *copy, Eina_Bool full);

#endif

