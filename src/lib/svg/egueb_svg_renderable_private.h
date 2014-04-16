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

#ifndef _EGUEB_SVG_RENDERABLE_PRIVATE_H_
#define _EGUEB_SVG_RENDERABLE_PRIVATE_H_

#include "egueb_svg_reference.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_painter_private.h"
#include "egueb_svg_element_private.h"
#include "egueb_svg_reference_private.h"

typedef struct _Egueb_Svg_Renderable
{
	Egueb_Svg_Element base;
	/* properties */
	Egueb_Dom_Node *transform;
	/* private */
	Enesim_Renderer *proxy;
	Egueb_Svg_Painter *painter;
	Egueb_Svg_Reference *clip_path;
	Egueb_Svg_Clip_Path clip_path_last;
} Egueb_Svg_Renderable;

typedef Enesim_Renderer * (*Egueb_Svg_Renderable_Renderer_Get)(Egueb_Svg_Renderable *thiz);
typedef void (*Egueb_Svg_Renderable_Bounds_Get)(Egueb_Svg_Renderable *thiz, Enesim_Rectangle *bounds);
typedef Eina_Bool (*Egueb_Svg_Renderable_Process)(Egueb_Svg_Renderable *r);
typedef Egueb_Svg_Painter * (*Egueb_Svg_Renderable_Painter_Get)(Egueb_Svg_Renderable *r);
typedef void (*Egueb_Svg_Renderable_Painter_Apply)(Egueb_Svg_Renderable *thiz,
		Egueb_Svg_Painter *painter);

typedef struct _Egueb_Svg_Renderable_Class
{
	Egueb_Svg_Element_Class base;
	Egueb_Svg_Renderable_Renderer_Get renderer_get;
	Egueb_Svg_Renderable_Painter_Get painter_get;
	Egueb_Svg_Renderable_Painter_Apply painter_apply;
	Egueb_Svg_Renderable_Bounds_Get bounds_get;
	Egueb_Svg_Renderable_Process process;
} Egueb_Svg_Renderable_Class;

#define EGUEB_SVG_RENDERABLE_DESCRIPTOR egueb_svg_renderable_descriptor_get()
#define EGUEB_SVG_RENDERABLE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Renderable_Class, EGUEB_SVG_RENDERABLE_DESCRIPTOR)
#define EGUEB_SVG_RENDERABLE_CLASS_GET(o) EGUEB_SVG_RENDERABLE_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_RENDERABLE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Renderable, EGUEB_SVG_RENDERABLE_DESCRIPTOR)

Enesim_Object_Descriptor *  egueb_svg_renderable_descriptor_get(void);
Enesim_Renderer * egueb_svg_renderable_renderer_get(Egueb_Dom_Node *n);
Enesim_Renderer * egueb_svg_renderable_class_renderer_get(Egueb_Dom_Node *n);
Egueb_Svg_Painter * egueb_svg_renderable_painter_get(Egueb_Dom_Node *n);
Egueb_Svg_Painter * egueb_svg_renderable_class_painter_get(Egueb_Dom_Node *n);

#endif
