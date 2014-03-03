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
#include "Egueb_Css.h"
#include "egueb_css_private.h"
#include "egueb_css_font_weight.h"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_css_font_weight_string_from(Egueb_Css_Font_Weight *thiz,
		const char *str)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(str, "normal"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_NORMAL;
	else if (!strcmp(str, "bold"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_BOLD;
	else if (!strcmp(str, "bolder"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_BOLDER;
	else if (!strcmp(str, "lighter"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_LIGHTER;
	else if (!strcmp(str, "100"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_100;
	else if (!strcmp(str, "200"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_200;
	else if (!strcmp(str, "300"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_300;
	else if (!strcmp(str, "400"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_400;
	else if (!strcmp(str, "500"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_500;
	else if (!strcmp(str, "600"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_600;
	else if (!strcmp(str, "700"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_700;
	else if (!strcmp(str, "800"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_800;
	else if (!strcmp(str, "900"))
		*thiz = EGUEB_CSS_FONT_WEIGHT_900;
	else
		return EINA_FALSE;
	return ret;
}

EAPI const char * egueb_css_font_weight_string_to(Egueb_Css_Font_Weight thiz)
{
	switch (thiz)
	{
		case EGUEB_CSS_FONT_WEIGHT_NORMAL:
		return "normal";

		case EGUEB_CSS_FONT_WEIGHT_BOLD:
		return "bold";

		case EGUEB_CSS_FONT_WEIGHT_BOLDER:
		return "bolder";

		case EGUEB_CSS_FONT_WEIGHT_LIGHTER: 
		return "lighter";

		case EGUEB_CSS_FONT_WEIGHT_100:
		return "100";

		case EGUEB_CSS_FONT_WEIGHT_200:
		return "200";

		case EGUEB_CSS_FONT_WEIGHT_300:
		return "300";

		case EGUEB_CSS_FONT_WEIGHT_400:
		return "400";

		case EGUEB_CSS_FONT_WEIGHT_500:
		return "500";

		case EGUEB_CSS_FONT_WEIGHT_600:
		return "600";

		case EGUEB_CSS_FONT_WEIGHT_700:
		return "700";

		case EGUEB_CSS_FONT_WEIGHT_800:
		return "800";

		case EGUEB_CSS_FONT_WEIGHT_900:
		return "900";

		default:
		return NULL;
	}
}
