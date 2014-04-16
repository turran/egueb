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

#ifndef _EGUEB_SVG_GRADIENT_PRIVATE_H_
#define _EGUEB_SVG_GRADIENT_PRIVATE_H_

#include "egueb_svg_paint_server_private.h" 
#include "egueb_svg_reference_gradient_private.h"

typedef struct _Egueb_Svg_Gradient
{
	Egueb_Svg_Paint_Server base;
	/* attributes */
	Egueb_Dom_Node *units;
	Egueb_Dom_Node *transform;
	Egueb_Dom_Node *spread_method;
	Egueb_Dom_Node *xlink_href;
	/* private */
} Egueb_Svg_Gradient;

typedef struct _Egueb_Svg_Gradient_Class
{
	Egueb_Svg_Paint_Server_Class base;
} Egueb_Svg_Gradient_Class;

#define EGUEB_SVG_GRADIENT_DESCRIPTOR egueb_svg_gradient_descriptor_get()
#define EGUEB_SVG_GRADIENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Gradient_Class, 					\
		EGUEB_SVG_GRADIENT_DESCRIPTOR)
#define EGUEB_SVG_GRADIENT_CLASS_GET(o) EGUEB_SVG_GRADIENT_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_GRADIENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Gradient, EGUEB_SVG_GRADIENT_DESCRIPTOR)
Enesim_Object_Descriptor *  egueb_svg_gradient_descriptor_get(void);

void egueb_svg_gradient_href_node_get(Egueb_Dom_Node *n, Egueb_Dom_Node **href);

void egueb_svg_gradient_deep_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units *units);
void egueb_svg_gradient_deep_transform_get(Egueb_Dom_Node *n,
		Enesim_Matrix *transform);
void egueb_svg_gradient_deep_stop_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **stop);
void egueb_svg_gradient_deep_spread_method_get(Egueb_Dom_Node *n,
		Egueb_Svg_Spread_Method *spread_method);

#endif
