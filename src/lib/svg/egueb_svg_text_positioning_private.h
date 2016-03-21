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

#ifndef _EGUEB_SVG_TEXT_POSITIONING_PRIVATE_H_
#define _EGUEB_SVG_TEXT_POSITIONING_PRIVATE_H_

#include "egueb_svg_renderable.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_painter_private.h"
#include "egueb_svg_renderable_private.h"
#include "egueb_svg_text_content_private.h"

typedef struct _Egueb_Svg_Text_Positioning
{
	Egueb_Svg_Text_Content base;
	/* attributes */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	Egueb_Dom_Node *dx;
	Egueb_Dom_Node *dy;
	/* private */
	double gx;
	double gy;
} Egueb_Svg_Text_Positioning;

typedef struct _Egueb_Svg_Text_Positioning_Class
{
	Egueb_Svg_Text_Content_Class base;
} Egueb_Svg_Text_Positioning_Class;

#define EGUEB_SVG_TEXT_POSITIONING_DESCRIPTOR egueb_svg_text_content_descriptor_get()
#define EGUEB_SVG_TEXT_POSITIONING_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Text_Positioning_Class, EGUEB_SVG_TEXT_POSITIONING_DESCRIPTOR)
#define EGUEB_SVG_TEXT_POSITIONING_CLASS_GET(o) \
		EGUEB_SVG_TEXT_POSITIONING_CLASS((ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_TEXT_POSITIONING(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Text_Positioning, EGUEB_SVG_TEXT_POSITIONING_DESCRIPTOR)

Enesim_Object_Descriptor *  egueb_svg_text_positioning_descriptor_get(void);

#endif

