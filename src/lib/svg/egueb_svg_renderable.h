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

#ifndef _EGUEB_SVG_RENDERABLE_H_
#define _EGUEB_SVG_RENDERABLE_H_

#include "egueb_svg_matrix.h"

/**
 * @defgroup Egueb_Svg_Renderable_Group Renderable
 * @brief Renderable element
 * @ingroup Egueb_Svg_Element_Group
 * @{
 */

EAPI Eina_Bool egueb_svg_is_renderable(Egueb_Dom_Node *n);
EAPI Enesim_Renderer * egueb_svg_renderable_renderer_get(Egueb_Dom_Node *n);
EAPI Enesim_Renderer * egueb_svg_renderable_class_renderer_get(Egueb_Dom_Node *n);
EAPI void egueb_svg_renderable_bounds_get(Egueb_Dom_Node *n,
		Enesim_Rectangle *bounds);
EAPI void egueb_svg_renderable_user_bounds_get(Egueb_Dom_Node *n,
		Eina_Rectangle *bounds);

EAPI void egueb_svg_renderable_transform_get(Egueb_Dom_Node *n,
		Egueb_Svg_Matrix_Animated *m);
EAPI void egueb_svg_renderable_transform_set(Egueb_Dom_Node *n,
		Enesim_Matrix *m);

/**
 * @}
 */

#endif
