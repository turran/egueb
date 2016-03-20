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

#ifndef _EGUEB_SVG_TEXT_CONTAINER_PRIVATE_H_
#define _EGUEB_SVG_TEXT_CONTAINER_PRIVATE_H_

#include "egueb_svg_renderable.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_painter_private.h"
#include "egueb_svg_renderable_private.h"

typedef struct _Egueb_Svg_Text_Span
{
	Enesim_Renderer *r;
} Egueb_Svg_Text_Span;

typedef struct _Egueb_Svg_Text_Container
{
	Egueb_Svg_Text_Content base;
	Enesim_Renderer *r;
	/* list of texts strctures */
	Eina_List *spans;
	Eina_Bool renderable_tree_changed;
} Egueb_Svg_Text_Container;

typedef struct _Egueb_Svg_Text_Container_Class
{
	Egueb_Svg_Text_Content_Class base;
} Egueb_Svg_Text_Container_Class;

#define EGUEB_SVG_TEXT_CONTAINER_DESCRIPTOR egueb_svg_text_content_descriptor_get()
#define EGUEB_SVG_TEXT_CONTAINER_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Text_Container_Class, EGUEB_SVG_TEXT_CONTAINER_DESCRIPTOR)
#define EGUEB_SVG_TEXT_CONTAINER_CLASS_GET(o) \
		EGUEB_SVG_TEXT_CONTAINER_CLASS((ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_TEXT_CONTAINER(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Text_Container, EGUEB_SVG_TEXT_CONTAINER_DESCRIPTOR)

Enesim_Object_Descriptor *  egueb_svg_text_container_descriptor_get(void);

#endif

