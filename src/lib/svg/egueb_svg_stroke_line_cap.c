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
#include "egueb_svg_stroke_line_cap.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_VALUE_ENUM_BOILERPLATE(egueb_svg_stroke_line_cap, Egueb_Svg_Stroke_Line_Cap);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_stroke_line_cap_descriptor_get(void)
{
	return &_egueb_svg_stroke_line_cap_descriptor;
}

EAPI Eina_Bool egueb_svg_stroke_line_cap_string_from(Egueb_Svg_Stroke_Line_Cap *thiz, const char *value)
{
	if (!strcmp(value, "butt"))
		*thiz = EGUEB_SVG_STROKE_LINE_CAP_BUTT;
	else if (!strcmp(value, "round"))
		*thiz = EGUEB_SVG_STROKE_LINE_CAP_ROUND;
	else if (!strcmp(value, "square"))
		*thiz = EGUEB_SVG_STROKE_LINE_CAP_SQUARE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI char * egueb_svg_stroke_line_cap_string_to(Egueb_Svg_Stroke_Line_Cap thiz)
{
	switch (thiz)
	{
		case EGUEB_SVG_STROKE_LINE_CAP_BUTT:
		return strdup("butt");

		case EGUEB_SVG_STROKE_LINE_CAP_ROUND:
		return strdup("round");

		case EGUEB_SVG_STROKE_LINE_CAP_SQUARE:
		return strdup("square");

		default:
		return NULL;
	}
}
