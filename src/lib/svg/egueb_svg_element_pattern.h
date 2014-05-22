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

#ifndef _EGUEB_SVG_ELEMENT_PATTERN_H_
#define _EGUEB_SVG_ELEMENT_PATTERN_H_

EAPI Egueb_Dom_Node * egueb_svg_element_pattern_new(void);
EAPI Eina_Bool egueb_svg_element_is_pattern(Egueb_Dom_Node *n);

EAPI void egueb_svg_element_pattern_x_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v);
EAPI void egueb_svg_element_pattern_x_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v);
EAPI void egueb_svg_element_pattern_y_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v);
EAPI void egueb_svg_element_pattern_y_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v);
EAPI void egueb_svg_element_pattern_width_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *v);
EAPI void egueb_svg_element_pattern_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *v);
EAPI void egueb_svg_element_pattern_height_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *v);
EAPI void egueb_svg_element_pattern_height_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *v);
EAPI void egueb_svg_element_pattern_xlink_href_set(Egueb_Dom_Node *n, Egueb_Dom_String *v);
EAPI void egueb_svg_element_pattern_xlink_href_get(Egueb_Dom_Node *n, Egueb_Svg_String_Animated *v);
EAPI void egueb_svg_element_pattern_pattern_units_set(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units v);
EAPI void egueb_svg_element_pattern_pattern_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units_Animated *v);
EAPI void egueb_svg_element_pattern_pattern_transform_set(Egueb_Dom_Node *n,
		Enesim_Matrix *m);
EAPI void egueb_svg_element_pattern_pattern_transform_get(Egueb_Dom_Node *n,
		Egueb_Svg_Matrix_Animated *m);

#endif
