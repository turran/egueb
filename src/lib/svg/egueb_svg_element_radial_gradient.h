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
#ifndef _EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_H_
#define _EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_H_

EAPI Egueb_Dom_Node * egueb_svg_element_radial_gradient_new(void);
EAPI Eina_Bool egueb_svg_is_element_radial_gradient(Egueb_Dom_Node *n);

EAPI void egueb_svg_element_radial_gradient_deep_cx_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *cx);
EAPI void egueb_svg_element_radial_gradient_deep_cy_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *cy);
EAPI Eina_Bool egueb_svg_element_radial_gradient_deep_fx_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *fx);
EAPI Eina_Bool egueb_svg_element_radial_gradient_deep_fy_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *fy);
EAPI void egueb_svg_element_radial_gradient_deep_r_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *r);
#if 0
EAPI void egueb_svg_element_radial_gradient_cx_set(Ender_Element *e, const Egueb_Svg_Coord *cx);
EAPI void egueb_svg_element_radial_gradient_cx_get(Ender_Element *e, Egueb_Svg_Coord *cx);
EAPI Eina_Bool egueb_svg_element_radial_gradient_cx_is_set(Ender_Element *e);
EAPI void egueb_svg_element_radial_gradient_cy_set(Ender_Element *e, const Egueb_Svg_Coord *cy);
EAPI void egueb_svg_element_radial_gradient_cy_get(Ender_Element *e, Egueb_Svg_Coord *cy);
EAPI Eina_Bool egueb_svg_element_radial_gradient_cy_is_set(Ender_Element *e);
EAPI void egueb_svg_element_radial_gradient_fx_set(Ender_Element *e, const Egueb_Svg_Coord *fx);
EAPI void egueb_svg_element_radial_gradient_fx_get(Ender_Element *e, Egueb_Svg_Coord *fx);
EAPI Eina_Bool egueb_svg_element_radial_gradient_fx_is_set(Ender_Element *e);
EAPI void egueb_svg_element_radial_gradient_fy_set(Ender_Element *e, const Egueb_Svg_Coord *fy);
EAPI void egueb_svg_element_radial_gradient_fy_get(Ender_Element *e, Egueb_Svg_Coord *fy);
EAPI Eina_Bool egueb_svg_element_radial_gradient_fy_is_set(Ender_Element *e);
EAPI void egueb_svg_element_radial_gradient_r_set(Ender_Element *e, const Egueb_Svg_Length *rad);
EAPI void egueb_svg_element_radial_gradient_r_get(Ender_Element *e, Egueb_Svg_Length *rad);
EAPI Eina_Bool egueb_svg_element_radial_gradient_r_is_set(Ender_Element *e);
#endif
#endif

