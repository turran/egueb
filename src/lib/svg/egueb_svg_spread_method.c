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
#include "egueb_svg_spread_method.h"

#include "egueb_svg_spread_method_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_spread_method_string_from(Egueb_Svg_Spread_Method *v,
		const char *attr)
{
	if (strncmp(attr, "pad", 3) == 0)
		*v = EGUEB_SVG_SPREAD_METHOD_PAD;
	else if (strncmp(attr, "reflect", 7) == 0)
		*v = EGUEB_SVG_SPREAD_METHOD_REFLECT;
	else if (strncmp(attr, "repeat", 6) == 0)
		*v = EGUEB_SVG_SPREAD_METHOD_REPEAT;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

char * egueb_svg_spread_method_string_to(Egueb_Svg_Spread_Method v)
{
	switch (v)
	{
		case EGUEB_SVG_SPREAD_METHOD_PAD:
		return strdup("pad");

		case EGUEB_SVG_SPREAD_METHOD_REFLECT:
		return strdup("reflect");

		case EGUEB_SVG_SPREAD_METHOD_REPEAT:
		return strdup("repeat");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
