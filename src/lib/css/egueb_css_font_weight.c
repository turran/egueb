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
#include "egueb_css_main.h"
#include "egueb_css_font_weight.h"
#include "egueb_css_font_weight_private.h"
#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_basic_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_VALUE_ENUM_BOILERPLATE(egueb_css_font_weight, Egueb_Css_Font_Weight);
/*----------------------------------------------------------------------------*
 *                           Attribute interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_BASIC_BOILERPLATE(Egueb_Css_Font_Weight,
		Egueb_Css_Font_Weight_Attr, egueb_css_font_weight_value,
		egueb_css_font_weight_attr)
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_css_font_weight_string_from(Egueb_Css_Font_Weight *thiz,
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

char * egueb_css_font_weight_string_to(Egueb_Css_Font_Weight thiz)
{
	switch (thiz)
	{
		case EGUEB_CSS_FONT_WEIGHT_NORMAL:
		return strdup("normal");

		case EGUEB_CSS_FONT_WEIGHT_BOLD:
		return strdup("bold");

		case EGUEB_CSS_FONT_WEIGHT_BOLDER:
		return strdup("bolder");

		case EGUEB_CSS_FONT_WEIGHT_LIGHTER: 
		return strdup("lighter");

		case EGUEB_CSS_FONT_WEIGHT_100:
		return strdup("100");

		case EGUEB_CSS_FONT_WEIGHT_200:
		return strdup("200");

		case EGUEB_CSS_FONT_WEIGHT_300:
		return strdup("300");

		case EGUEB_CSS_FONT_WEIGHT_400:
		return strdup("400");

		case EGUEB_CSS_FONT_WEIGHT_500:
		return strdup("500");

		case EGUEB_CSS_FONT_WEIGHT_600:
		return strdup("600");

		case EGUEB_CSS_FONT_WEIGHT_700:
		return strdup("700");

		case EGUEB_CSS_FONT_WEIGHT_800:
		return strdup("800");

		case EGUEB_CSS_FONT_WEIGHT_900:
		return strdup("900");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_css_font_weight_attr_new(
		Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_css_font_weight_attr);
	egueb_dom_attr_init(n, egueb_dom_string_ref(EGUEB_CSS_NAME_FONT_WEIGHT),
			animatable, stylable, inheritable);
	return n;
}

EAPI const Egueb_Dom_Value_Descriptor *
		egueb_css_font_weight_value_descriptor_get(void)
{
	return &_egueb_css_font_weight_descriptor;
}
