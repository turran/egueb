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
#include "egueb_svg_text_anchor.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_VALUE_ENUM_BOLIERPLATE(egueb_svg_text_anchor, Egueb_Svg_Text_Anchor);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_text_anchor_descriptor_get(void)
{
	return &_egueb_svg_text_anchor_descriptor;
}

EAPI Eina_Bool egueb_svg_text_anchor_string_from(Egueb_Svg_Text_Anchor *thiz, const char *value)
{
	if (!strcmp(value, "start"))
		*thiz = EGUEB_SVG_TEXT_ANCHOR_START;
	else if (!strcmp(value, "middle"))
		*thiz = EGUEB_SVG_TEXT_ANCHOR_MIDDLE;
	else if (!strcmp(value, "end"))
		*thiz = EGUEB_SVG_TEXT_ANCHOR_END;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI char * egueb_svg_text_anchor_string_to(Egueb_Svg_Text_Anchor thiz)
{
	switch (thiz)
	{
		case EGUEB_SVG_TEXT_ANCHOR_START:
		return strdup("start");

		case EGUEB_SVG_TEXT_ANCHOR_MIDDLE:
		return strdup("middle");

		case EGUEB_SVG_TEXT_ANCHOR_END:
		return strdup("end");

		default:
		return NULL;
	}
}

