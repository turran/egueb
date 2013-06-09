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

#ifndef _EGUEB_SVG_REFERENCE_PAINT_SERVER_PRIVATE_H_
#define _EGUEB_SVG_REFERENCE_PAINT_SERVER_PRIVATE_H_

#include "egueb_svg_reference.h"
#include "egueb_svg_renderable.h"
#include "egueb_svg_reference_private.h"

typedef struct _Egueb_Svg_Reference_Paint_Server
{
	Egueb_Svg_Reference base;
} Egueb_Svg_Reference_Paint_Server;

typedef Enesim_Renderer * (*Egueb_Svg_Reference_Paint_Server_Renderer_Get)
		(Egueb_Svg_Reference_Paint_Server *thiz);

typedef struct _Egueb_Svg_Reference_Paint_Server_Class
{
	Egueb_Svg_Reference_Class base;
	Egueb_Svg_Reference_Paint_Server_Renderer_Get renderer_get;
} Egueb_Svg_Reference_Paint_Server_Class;

#define EGUEB_SVG_REFERENCE_PAINT_SERVER_DESCRIPTOR egueb_svg_reference_paint_server_descriptor_get()
Enesim_Object_Descriptor * egueb_svg_reference_paint_server_descriptor_get(void);

#define EGUEB_SVG_REFERENCE_PAINT_SERVER_CLASS_GET(o) 				\
		EGUEB_SVG_REFERENCE_PAINT_SERVER_CLASS(				\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_REFERENCE_PAINT_SERVER_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,	\
		Egueb_Svg_Reference_Paint_Server_Class,				\
		EGUEB_SVG_REFERENCE_PAINT_SERVER_DESCRIPTOR)
#define EGUEB_SVG_REFERENCE_PAINT_SERVER(o) ENESIM_OBJECT_INSTANCE_CHECK(o,	\
		Egueb_Svg_Reference_Paint_Server,					\
		EGUEB_SVG_REFERENCE_PAINT_SERVER_DESCRIPTOR)

/* paint_server */
void egueb_svg_reference_paint_server_renderer_get(Egueb_Svg_Reference *r,
		Enesim_Renderer **ren);

#endif

