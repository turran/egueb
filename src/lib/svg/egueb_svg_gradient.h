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
#ifndef _EGUEB_SVG_GRADIENT_H_
#define _EGUEB_SVG_GRADIENT_H_

/**
 * @defgroup Egueb_Svg_Gradient_Group Gradients
 * @brief Gradient
 * @ingroup Egueb_Svg_Paint_Server_Group
 * @{
 */

EAPI Eina_Bool egueb_svg_is_gradient(Egueb_Dom_Node *n);

EAPI void egueb_svg_gradient_xlink_href_set(Egueb_Dom_Node *n, Egueb_Dom_String *v);
EAPI void egueb_svg_gradient_xlink_href_get(Egueb_Dom_Node *n, Egueb_Svg_String_Animated *v);
EAPI void egueb_svg_gradient_gradient_units_set(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units v);
EAPI void egueb_svg_gradient_gradient_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units_Animated *v);
EAPI void egueb_svg_gradient_gradient_transform_set(Egueb_Dom_Node *n,
		Enesim_Matrix *m);
EAPI void egueb_svg_gradient_gradient_transform_get(Egueb_Dom_Node *n,
		Egueb_Svg_Matrix_Animated *m);

/**
 * @}
 */

#endif
