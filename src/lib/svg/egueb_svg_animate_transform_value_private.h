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

#ifndef EGUEB_SVG_ANIMATE_TRANSFORM_VALUE_PRIVATE_H_
#define EGUEB_SVG_ANIMATE_TRANSFORM_VALUE_PRIVATE_H_

typedef struct _Egueb_Svg_Animate_Transform_Value
{
	double values[3];
	int count;
} Egueb_Svg_Animate_Transform_Value;

extern Egueb_Dom_Value_Descriptor egueb_svg_animate_transform_rotate_descriptor;
extern Egueb_Dom_Value_Descriptor egueb_svg_animate_transform_scale_descriptor;
extern Egueb_Dom_Value_Descriptor egueb_svg_animate_transform_translate_descriptor;
extern Egueb_Dom_Value_Descriptor egueb_svg_animate_transform_skew_descriptor;

#endif
