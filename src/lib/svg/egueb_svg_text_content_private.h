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

#ifndef _EGUEB_SVG_TEXT_CONTENT_PRIVATE_H_
#define _EGUEB_SVG_TEXT_CONTENT_PRIVATE_H_

#include "egueb_svg_renderable.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_painter_private.h"
#include "egueb_svg_renderable_private.h"

typedef struct _Egueb_Svg_Text_Content_State
{
	Egueb_Dom_List *x;
	Egueb_Dom_List *y;
	Egueb_Dom_List *dx;
	Egueb_Dom_List *dy;
	double pen_x, pen_y;
} Egueb_Svg_Text_Content_State;

typedef struct _Egueb_Svg_Text_Content
{
	Egueb_Svg_Renderable base;
	Egueb_Svg_Painter *painter;
	Enesim_Renderer *r;
	/* list of texts strctures */
	Eina_List *spans;
	Eina_Bool renderable_tree_changed;
	Enesim_Text_Font *gfont;
	double gfont_size;
	double gx;
	double gy;
	/* pen last position */
	double px;
	double py;
	Egueb_Svg_Text_Content_State state;
} Egueb_Svg_Text_Content;

typedef void (*Egueb_Svg_Text_Content_Initialize_State)(Egueb_Svg_Text_Content *thiz);

typedef struct _Egueb_Svg_Text_Content_Class
{
	Egueb_Svg_Renderable_Class base;
	Egueb_Svg_Text_Content_Initialize_State initialize_state;
} Egueb_Svg_Text_Content_Class;

#define EGUEB_SVG_TEXT_CONTENT_DESCRIPTOR egueb_svg_text_content_descriptor_get()
#define EGUEB_SVG_TEXT_CONTENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Text_Content_Class, EGUEB_SVG_TEXT_CONTENT_DESCRIPTOR)
#define EGUEB_SVG_TEXT_CONTENT_CLASS_GET(o) \
		EGUEB_SVG_TEXT_CONTENT_CLASS((ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_TEXT_CONTENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Text_Content, EGUEB_SVG_TEXT_CONTENT_DESCRIPTOR)

Enesim_Object_Descriptor *  egueb_svg_text_content_descriptor_get(void);

#endif

