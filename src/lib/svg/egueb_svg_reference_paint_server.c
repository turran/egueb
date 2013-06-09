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
#include "egueb_svg_main_private.h"
#include "egueb_svg_main.h"
#include "egueb_svg_reference_paint_server_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Reference interface                            *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_REFERENCE_DESCRIPTOR,
		Egueb_Svg_Reference_Paint_Server,
		Egueb_Svg_Reference_Paint_Server_Class,
		egueb_svg_reference_paint_server);

static void _egueb_svg_reference_paint_server_class_init(void *k)
{
}

static void _egueb_svg_reference_paint_server_class_deinit(void *k)
{
}

static void _egueb_svg_reference_paint_server_instance_init(void *o)
{
}

static void _egueb_svg_reference_paint_server_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_reference_paint_server_renderer_get(Egueb_Svg_Reference *r,
		Enesim_Renderer **ren)
{
	Egueb_Svg_Reference_Paint_Server_Class *klass;

	if (!ren) return;

	klass = EGUEB_SVG_REFERENCE_PAINT_SERVER_CLASS_GET(r);
	if (!klass->renderer_get)
	{
		*ren = NULL;
		return;
	}
	else
	{
		*ren = klass->renderer_get(EGUEB_SVG_REFERENCE_PAINT_SERVER(r));
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/


