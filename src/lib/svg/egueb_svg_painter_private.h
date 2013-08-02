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

#ifndef _EGUEB_SVG_PAINTER_PRIVATE_H_
#define _EGUEB_SVG_PAINTER_PRIVATE_H_

#include "egueb_svg_reference.h"
#include "egueb_svg_element_private.h"

/* given that the renderables can be childs of several type of parents
 * it is desired to define the behaviour of such renderables, for example
 * shapes being child of a clippath must not process the fill/stroke
 * attributes. A shape being child of a def should not create a renderer
 * or process anything, etc
 */

struct _Egueb_Svg_Painter
{
	Enesim_Object_Instance base;
	/* common */
	Eina_Bool visibility;
	Enesim_Color color;
	/* stroke */
	Enesim_Renderer_Shape_Draw_Mode draw_mode;
	Enesim_Renderer_Shape_Stroke_Cap stroke_cap;
	Enesim_Renderer_Shape_Stroke_Join stroke_join;
	Enesim_Color stroke_color;
	Enesim_Renderer *stroke_renderer;
	double stroke_weight;
	/* fill */
	Enesim_Renderer *fill_renderer;
	Enesim_Color fill_color;
	Enesim_Renderer_Shape_Fill_Rule fill_rule;
};

typedef Eina_Bool (*Egueb_Svg_Painter_Resolve)(Egueb_Svg_Painter *thiz,
		Egueb_Svg_Element *e);

typedef struct _Egueb_Svg_Painter_Class
{
	Enesim_Object_Class base;
	Egueb_Svg_Painter_Resolve resolve;
} Egueb_Svg_Painter_Class;

#define EGUEB_SVG_PAINTER_DESCRIPTOR egueb_svg_painter_descriptor_get()
Enesim_Object_Descriptor * egueb_svg_painter_descriptor_get(void);

#define EGUEB_SVG_PAINTER_CLASS_GET(o) EGUEB_SVG_PAINTER_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_PAINTER_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,			\
		Egueb_Svg_Painter_Class, EGUEB_SVG_PAINTER_DESCRIPTOR)
#define EGUEB_SVG_PAINTER(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Svg_Painter, EGUEB_SVG_PAINTER_DESCRIPTOR)

Eina_Bool egueb_svg_painter_resolve(Egueb_Svg_Painter *thiz, Egueb_Svg_Element *e);
void egueb_svg_painter_free(Egueb_Svg_Painter *thiz);

/* for now we dont export anything */
Egueb_Svg_Painter * egueb_svg_painter_generic_new(void);
Egueb_Svg_Painter * egueb_svg_painter_clip_path_new(Egueb_Svg_Reference *ref);

#endif
