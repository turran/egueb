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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Egueb_Svg.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Module API                                     *
 *----------------------------------------------------------------------------*/
static Eina_Bool svg_source_init(void)
{
	/* This function will register the implementation source */
	egueb_svg_init();
	return EINA_TRUE;
}

static void svg_source_shutdown(void)
{
	egueb_svg_shutdown();
}

EINA_MODULE_INIT(svg_source_init);
EINA_MODULE_SHUTDOWN(svg_source_shutdown);
