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
#include "egueb_svg_paint_server_private.h"
#include "egueb_svg_paint_server.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_REFERENCEABLE_DESCRIPTOR,
		Egueb_Svg_Paint_Server, Egueb_Svg_Paint_Server_Class,
		egueb_svg_paint_server);

static void _egueb_svg_paint_server_class_init(void *k)
{
}

static void _egueb_svg_paint_server_class_deinit(void *k)
{
}

static void _egueb_svg_paint_server_instance_init(void *o)
{
}

static void _egueb_svg_paint_server_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_svg_is_paint_server(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_PAINT_SERVER_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}
