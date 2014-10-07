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
#include "egueb_css_private.h"

#include "egueb_css_font_style.h"

#include "egueb_css_font_style_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_css_font_style_string_from(Egueb_Css_Font_Style *thiz,
		const char *str)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(str, "normal"))
		*thiz = EGUEB_CSS_FONT_STYLE_NORMAL;
	else if (!strcmp(str, "italic"))
		*thiz = EGUEB_CSS_FONT_STYLE_ITALIC;
	else if (!strcmp(str, "oblique"))
		*thiz = EGUEB_CSS_FONT_STYLE_OBLIQUE;
	else
		return EINA_FALSE;
	return ret;
}

char * egueb_css_font_style_string_to(Egueb_Css_Font_Style thiz)
{
	switch (thiz)
	{
		case EGUEB_CSS_FONT_STYLE_NORMAL:
		return strdup("normal");

		case EGUEB_CSS_FONT_STYLE_ITALIC:
		return strdup("italic");

		case EGUEB_CSS_FONT_STYLE_OBLIQUE:
		return strdup("oblique");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

