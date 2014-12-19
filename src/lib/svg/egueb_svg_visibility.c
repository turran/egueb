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
#include "egueb_svg_visibility.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_visibility_string_from(Egueb_Svg_Visibility *v,
		const char *attr)
{
	if (!strcmp(attr, "visible"))
		*v = EGUEB_SVG_VISIBILITY_VISIBLE;
	else if (!strcmp(attr, "hidden"))
		*v = EGUEB_SVG_VISIBILITY_HIDDEN;
	else if (!strcmp(attr, "collapse"))
		*v = EGUEB_SVG_VISIBILITY_COLLAPSE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

char * egueb_svg_visibility_string_to(Egueb_Svg_Visibility v)
{
	switch (v)
	{
		case EGUEB_SVG_VISIBILITY_VISIBLE:
		return strdup("visible");

		case EGUEB_SVG_VISIBILITY_HIDDEN:
		return strdup("hidden");

		case EGUEB_SVG_VISIBILITY_COLLAPSE:
		return strdup("collapse");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

