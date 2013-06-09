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
#ifndef _EGUEB_SVG_ELEMENT_GRADIENT_H_
#define _EGUEB_SVG_ELEMENT_GRADIENT_H_

EAPI Eina_Bool egueb_svg_is_linear_gradient(Egueb_Dom_Node *n);
EAPI void egueb_svg_gradient_deep_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units *units);
EAPI void egueb_svg_gradient_deep_transform_get(Egueb_Dom_Node *n,
		Enesim_Matrix *transform);

#if 0
EAPI Eina_Bool egueb_svg_is_gradient(Egueb_Dom_Node *n);
EAPI void egueb_svg_element_gradient_units_set(Ender_Element *e, Egueb_Svg_Element_Gradient_Units units);
EAPI void egueb_svg_element_gradient_units_get(Ender_Element *e, Egueb_Svg_Element_Gradient_Units *units);
EAPI Eina_Bool egueb_svg_element_gradient_units_is_set(Ender_Element *e);
EAPI void egueb_svg_element_gradient_transform_set(Ender_Element *e, const Enesim_Matrix *transform);
EAPI void egueb_svg_element_gradient_transform_get(Ender_Element *e, Enesim_Matrix *transform);
EAPI Eina_Bool egueb_svg_element_gradient_transform_is_set(Ender_Element *e);
EAPI void egueb_svg_element_gradient_spread_method_set(Ender_Element *e, Egueb_Svg_Spread_Method spread_method);
EAPI void egueb_svg_element_gradient_spread_method_get(Ender_Element *e, Egueb_Svg_Spread_Method *spread_method);
EAPI void egueb_svg_element_gradient_href_set(Ender_Element *e, const char *href);
EAPI void egueb_svg_element_gradient_href_get(Ender_Element *e, const char **href);
#endif

#endif
