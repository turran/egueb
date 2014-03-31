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
#include "egueb_svg_overflow.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_VALUE_ENUM_BOILERPLATE(egueb_svg_overflow, Egueb_Svg_Overflow);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_overflow_descriptor_get(void)
{
	return &_egueb_svg_overflow_descriptor;
}

EAPI Eina_Bool egueb_svg_overflow_string_from(Egueb_Svg_Overflow *thiz, const char *value)
{
	if (!strcmp(value, "visible"))
		*thiz = EGUEB_SVG_OVERFLOW_VISIBLE;
	else if (!strcmp(value, "hidden"))
		*thiz = EGUEB_SVG_OVERFLOW_HIDDEN;
	else if (!strcmp(value, "scroll"))
		*thiz = EGUEB_SVG_OVERFLOW_SCROLL;
	else if (!strcmp(value, "auto"))
		*thiz = EGUEB_SVG_OVERFLOW_AUTO;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI char * egueb_svg_overflow_string_to(Egueb_Svg_Overflow thiz)
{
	switch (thiz)
	{
		case EGUEB_SVG_OVERFLOW_VISIBLE:
		return strdup("visible");

		case EGUEB_SVG_OVERFLOW_HIDDEN:
		return strdup("hidden");

		case EGUEB_SVG_OVERFLOW_SCROLL:
		return strdup("scroll");

		case EGUEB_SVG_OVERFLOW_AUTO:
		return strdup("auto");

		default:
		return NULL;
	}
}

