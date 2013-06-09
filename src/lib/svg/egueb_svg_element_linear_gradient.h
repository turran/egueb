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

EAPI Egueb_Dom_Node * egueb_svg_element_linear_gradient_new(void);
EAPI Eina_Bool egueb_svg_is_element_linear_gradient(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_linear_gradient_deep_x1_get(Egueb_Dom_Node *n,
		Egueb_Svg_Coord *x1);
EAPI void egueb_svg_element_linear_gradient_deep_y1_get(Egueb_Dom_Node *n,
		Egueb_Svg_Coord *y1);
EAPI void egueb_svg_element_linear_gradient_deep_x2_get(Egueb_Dom_Node *n,
		Egueb_Svg_Coord *x2);
EAPI void egueb_svg_element_linear_gradient_deep_y2_get(Egueb_Dom_Node *n,
		Egueb_Svg_Coord *y2);
#if 0
EAPI void egueb_svg_element_linear_gradient_x1_set(Ender_Element *e, const Egueb_Svg_Coord *x1);
EAPI void egueb_svg_element_linear_gradient_x1_get(Ender_Element *e, Egueb_Svg_Coord *x1);
EAPI Eina_Bool egueb_svg_element_linear_gradient_x1_is_set(Ender_Element *e);
EAPI void egueb_svg_element_linear_gradient_y1_set(Ender_Element *e, const Egueb_Svg_Coord *y1);
EAPI void egueb_svg_element_linear_gradient_y1_get(Ender_Element *e, Egueb_Svg_Coord *y1);
EAPI Eina_Bool egueb_svg_element_linear_gradient_y1_is_set(Ender_Element *e);
EAPI void egueb_svg_element_linear_gradient_x2_set(Ender_Element *e, const Egueb_Svg_Coord *x2);
EAPI void egueb_svg_element_linear_gradient_x2_get(Ender_Element *e, Egueb_Svg_Coord *x2);
EAPI Eina_Bool egueb_svg_element_linear_gradient_x2_is_set(Ender_Element *e);
EAPI void egueb_svg_element_linear_gradient_y2_set(Ender_Element *e, const Egueb_Svg_Coord *y2);
EAPI void egueb_svg_element_linear_gradient_y2_get(Ender_Element *e, Egueb_Svg_Coord *y2);
EAPI Eina_Bool egueb_svg_element_linear_gradient_y2_is_set(Ender_Element *e);
#endif

#endif
