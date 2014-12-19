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
#include "egueb_svg_stroke_line_join.h"

#include "egueb_svg_stroke_line_join_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_stroke_line_join_string_from(
		Egueb_Svg_Stroke_Line_Join *thiz, const char *value)
{
	if (!strcmp(value, "miter"))
		*thiz = EGUEB_SVG_STROKE_LINE_JOIN_MITER;
	else if (!strcmp(value, "round"))
		*thiz = EGUEB_SVG_STROKE_LINE_JOIN_ROUND;
	else if (!strcmp(value, "bevel"))
		*thiz = EGUEB_SVG_STROKE_LINE_JOIN_BEVEL;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

char * egueb_svg_stroke_line_join_string_to(Egueb_Svg_Stroke_Line_Join v)
{
	switch (v)
	{
		case EGUEB_SVG_STROKE_LINE_JOIN_MITER:
		return strdup("round");

		case EGUEB_SVG_STROKE_LINE_JOIN_ROUND:
		return strdup("round");

		case EGUEB_SVG_STROKE_LINE_JOIN_BEVEL:
		return strdup("bevel");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
