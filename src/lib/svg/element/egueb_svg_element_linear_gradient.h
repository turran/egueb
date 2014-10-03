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
#ifndef _EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_H_
#define _EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_H_

/**
 * @defgroup Egueb_Svg_Element_Linear_Gradient_Group Linear gradient
 * @brief Linear gradient
 * @ingroup Egueb_Svg_Gradient_Group
 * @{
 */

EAPI Egueb_Dom_Node * egueb_svg_element_linear_gradient_new(void);
EAPI Eina_Bool egueb_svg_is_element_linear_gradient(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_linear_gradient_x1_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v);
EAPI void egueb_svg_element_linear_gradient_x1_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v);
EAPI void egueb_svg_element_linear_gradient_y1_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v);
EAPI void egueb_svg_element_linear_gradient_y1_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v);
EAPI void egueb_svg_element_linear_gradient_x2_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v);
EAPI void egueb_svg_element_linear_gradient_x2_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v);
EAPI void egueb_svg_element_linear_gradient_y2_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v);
EAPI void egueb_svg_element_linear_gradient_y2_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v);

/**
 * @}
 */

#endif
