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
#include "egueb_svg_zoom_and_pan.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_zoom_and_pan_string_from(Egueb_Svg_Zoom_And_Pan *v,
		const char *attr)
{
	if (!strcmp(attr, "disable"))
		*v = EGUEB_SVG_ZOOM_AND_PAN_DISABLE;
	else if (!strcmp(attr, "magnify"))
		*v = EGUEB_SVG_ZOOM_AND_PAN_MAGNIFY;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

char * egueb_svg_zoom_and_pan_string_to(Egueb_Svg_Zoom_And_Pan v)
{
	switch (v)
	{
		case EGUEB_SVG_ZOOM_AND_PAN_DISABLE:
		return strdup("disable");

		case EGUEB_SVG_ZOOM_AND_PAN_MAGNIFY:
		return strdup("magnify");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
