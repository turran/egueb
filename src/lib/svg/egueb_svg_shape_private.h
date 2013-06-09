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

#ifndef _EGUEB_SVG_SHAPE_PRIVATE_H_
#define _EGUEB_SVG_SHAPE_PRIVATE_H_

#include "egueb_svg_renderable.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_painter_private.h"
#include "egueb_svg_renderable_private.h"

typedef struct _Egueb_Svg_Shape
{
	Egueb_Svg_Renderable base;
	/* private */
	Egueb_Svg_Painter *painter;
} Egueb_Svg_Shape;

typedef Eina_Bool (*Egueb_Svg_Shape_Generate_Geometry)(Egueb_Svg_Shape *thiz);
typedef void (*Egueb_Svg_Shape_Renderer_Propagate)(Egueb_Svg_Shape *thiz,
		Egueb_Svg_Painter *painter);

typedef struct _Egueb_Svg_Shape_Class
{
	Egueb_Svg_Renderable_Class base;
	Egueb_Svg_Shape_Generate_Geometry generate_geometry;
	Egueb_Svg_Shape_Renderer_Propagate renderer_propagate;
} Egueb_Svg_Shape_Class;

#define EGUEB_SVG_SHAPE_DESCRIPTOR egueb_svg_shape_descriptor_get()
#define EGUEB_SVG_SHAPE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Shape_Class, EGUEB_SVG_SHAPE_DESCRIPTOR)
#define EGUEB_SVG_SHAPE_CLASS_GET(o) \
		EGUEB_SVG_SHAPE_CLASS((ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_SHAPE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Shape, EGUEB_SVG_SHAPE_DESCRIPTOR)

Enesim_Object_Descriptor *  egueb_svg_shape_descriptor_get(void);

void egueb_svg_shape_painter_set(Egueb_Dom_Node *n, Egueb_Svg_Painter *painter);

#endif
