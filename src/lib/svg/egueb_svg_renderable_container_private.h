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

#ifndef _EGUEB_SVG_RENDERABLE_CONTAINER_PRIVATE_H_
#define _EGUEB_SVG_RENDERABLE_CONTAINER_PRIVATE_H_

#include "egueb_svg_renderable.h"
#include "egueb_svg_renderable_private.h"

typedef struct _Egueb_Svg_Renderable_Container
{
	Egueb_Svg_Renderable base;
	Enesim_Renderer *background;
	Enesim_Renderer *compound;
	Eina_Bool renderable_tree_changed;
#if 0
	Egueb_Svg_Input *input;
	Eina_List *renderables;
#endif
} Egueb_Svg_Renderable_Container;

typedef struct _Egueb_Svg_Renderable_Container_Class
{
	Egueb_Svg_Renderable_Class base;
} Egueb_Svg_Renderable_Container_Class;

#define EGUEB_SVG_RENDERABLE_CONTAINER_DESCRIPTOR egueb_svg_renderable_container_descriptor_get()
#define EGUEB_SVG_RENDERABLE_CONTAINER_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Renderable_Container_Class, EGUEB_SVG_RENDERABLE_CONTAINER_DESCRIPTOR)
#define EGUEB_SVG_RENDERABLE_CONTAINER_CLASS_GET(o) \
		EGUEB_SVG_RENDERABLE_CONTAINER_CLASS((ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_RENDERABLE_CONTAINER(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Renderable_Container, EGUEB_SVG_RENDERABLE_CONTAINER_DESCRIPTOR)

Enesim_Object_Descriptor *  egueb_svg_renderable_container_descriptor_get(void);
Eina_Bool egueb_svg_renderable_container_process(Egueb_Svg_Renderable *r);
Enesim_Renderer * egueb_svg_renderable_container_renderer_get(
		Egueb_Svg_Renderable *r);

#if 0
Egueb_Svg_Renderable_Container * egueb_svg_renderable_container_new(Ender_Element *e);
void egueb_svg_renderable_container_free(Egueb_Svg_Renderable_Container *thiz);
void egueb_svg_renderable_container_clear(Egueb_Svg_Renderable_Container *thiz);
void egueb_svg_renderable_container_renderable_add(Egueb_Svg_Renderable_Container *thiz,
		Egueb_Dom_Tag *t);
void egueb_svg_renderable_container_renderable_remove(Egueb_Svg_Renderable_Container *thiz,
		Egueb_Dom_Tag *t);
#endif

#endif
