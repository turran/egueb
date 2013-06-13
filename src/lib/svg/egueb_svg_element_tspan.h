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

#ifndef _EGUEB_SVG_ELEMENT_TSPAN_H_
#define _EGUEB_SVG_ELEMENT_TSPAN_H_

EAPI Egueb_Dom_Node * egueb_svg_element_tspan_new(void);
EAPI Eina_Error egueb_svg_element_tspan_x_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Length *x);
EAPI Eina_Error egueb_svg_element_tspan_y_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Length *y);

#endif

