/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include "egueb_svg_pointer_events.h"

#include "egueb_svg_pointer_events_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_pointer_events_string_from(
		Egueb_Svg_Pointer_Events *thiz, const char *value)
{
	if (!strcmp(value, "visiblePainted"))
		*thiz = EGUEB_SVG_POINTER_EVENTS_VISIBLE_PAINTED;
	else if (!strcmp(value, "visibleFill"))
		*thiz = EGUEB_SVG_POINTER_EVENTS_VISIBLE_FILL;
	else if (!strcmp(value, "visibleStroke"))
		*thiz = EGUEB_SVG_POINTER_EVENTS_VISIBLE_STROKE;
	else if (!strcmp(value, "visible"))
		*thiz = EGUEB_SVG_POINTER_EVENTS_VISIBLE;
	else if (!strcmp(value, "stroke"))
		*thiz = EGUEB_SVG_POINTER_EVENTS_STROKE;
	else if (!strcmp(value, "all"))
		*thiz = EGUEB_SVG_POINTER_EVENTS_ALL;
	else if (!strcmp(value, "none"))
		*thiz = EGUEB_SVG_POINTER_EVENTS_NONE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

char * egueb_svg_pointer_events_string_to(Egueb_Svg_Pointer_Events v)
{
	switch (v)
	{
		case EGUEB_SVG_POINTER_EVENTS_VISIBLE_PAINTED:
		return strdup("visiblePainted");

		case EGUEB_SVG_POINTER_EVENTS_VISIBLE_FILL:
		return strdup("visibleFill");

		case EGUEB_SVG_POINTER_EVENTS_VISIBLE_STROKE:
		return strdup("visibleStroke");

		case EGUEB_SVG_POINTER_EVENTS_VISIBLE:
		return strdup("visible");

		case EGUEB_SVG_POINTER_EVENTS_PAINTED:
		return strdup("painted");

		case EGUEB_SVG_POINTER_EVENTS_FILL:
		return strdup("fill");

		case EGUEB_SVG_POINTER_EVENTS_STROKE:
		return strdup("stroke");

		case EGUEB_SVG_POINTER_EVENTS_ALL:
		return strdup("all");

		case EGUEB_SVG_POINTER_EVENTS_NONE:
		return strdup("none");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

