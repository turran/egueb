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

#ifndef _EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_PRIVATE_H_
#define _EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_PRIVATE_H_

#include "egueb_svg_length.h"
#include "egueb_svg_rect.h"

void egueb_svg_element_radial_gradient_deep_cx_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *cx);
void egueb_svg_element_radial_gradient_deep_cy_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *cy);
Eina_Bool egueb_svg_element_radial_gradient_deep_fx_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *fx);
Eina_Bool egueb_svg_element_radial_gradient_deep_fy_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *fy);
void egueb_svg_element_radial_gradient_deep_r_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *r);

#endif
