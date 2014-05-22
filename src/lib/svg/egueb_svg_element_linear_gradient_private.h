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

void egueb_svg_element_linear_gradient_deep_x1_get(Egueb_Dom_Node *n,
		Egueb_Svg_Coord *x1);
void egueb_svg_element_linear_gradient_deep_y1_get(Egueb_Dom_Node *n,
		Egueb_Svg_Coord *y1);
void egueb_svg_element_linear_gradient_deep_x2_get(Egueb_Dom_Node *n,
		Egueb_Svg_Coord *x2);
void egueb_svg_element_linear_gradient_deep_y2_get(Egueb_Dom_Node *n,
		Egueb_Svg_Coord *y2);

#endif
