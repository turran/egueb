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

#ifndef _EGUEB_SVG_ELEMENT_USE_H_
#define _EGUEB_SVG_ELEMENT_USE_H_

/**
 * @file
 * @ender_group{Egueb_Svg_Element_Use}
 */

/**
 * @defgroup Egueb_Svg_Element_Use Use
 * @brief Use element @ender_inherits{Egueb_Svg_Element}
 * @ingroup Egueb_Svg_Shape_Group
 * @{
 */

EAPI Egueb_Dom_Node * egueb_svg_element_use_new(void);
EAPI Eina_Bool egueb_svg_element_is_use(Egueb_Dom_Node *n);

EAPI void egueb_svg_element_use_x_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *x);
EAPI void egueb_svg_element_use_x_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *x);
EAPI void egueb_svg_element_use_y_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *y);
EAPI void egueb_svg_element_use_y_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *y);
EAPI void egueb_svg_element_use_width_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *width);
EAPI void egueb_svg_element_use_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *width);
EAPI void egueb_svg_element_use_height_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *height);
EAPI void egueb_svg_element_use_height_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *height);
EAPI void egueb_svg_element_use_href_set_simple(Egueb_Dom_Node *n, Egueb_Dom_String *v);
EAPI void egueb_svg_element_use_href_get(Egueb_Dom_Node *n, Egueb_Svg_String_Animated *v);

/**
 * @}
 */

#endif
