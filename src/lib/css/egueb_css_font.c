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

#include "egueb_css_length.h"
#include "egueb_css_percentage.h"
#include "egueb_css_font_style.h"
#include "egueb_css_font_variant.h"
#include "egueb_css_font_weight.h"
#include "egueb_css_font_size.h"
#include "egueb_css_font_family.h"
#include "egueb_css_font.h"

#include "egueb_css_font_private.h"
#include "egueb_css_font_family_private.h"
#include "egueb_css_font_size_private.h"
#include "egueb_css_font_style_private.h"
#include "egueb_css_font_variant_private.h"
#include "egueb_css_font_weight_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_css_font_string_from(Egueb_Css_Font *thiz,
		const char *str)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(str, "caption"))
		thiz->type = EGUEB_CSS_FONT_TYPE_CAPTION;
	else if (!strcmp(str, "icon"))
		thiz->type = EGUEB_CSS_FONT_TYPE_ICON;
	else if (!strcmp(str, "menu"))
		thiz->type = EGUEB_CSS_FONT_TYPE_MENU;
	else if (!strcmp(str, "message-box"))
		thiz->type = EGUEB_CSS_FONT_TYPE_MESSAGE_BOX;
	else if (!strcmp(str, "small-caption"))
		thiz->type = EGUEB_CSS_FONT_TYPE_SMALL_CAPTION;
	else if (!strcmp(str, "status-bar"))
		thiz->type = EGUEB_CSS_FONT_TYPE_STATUS_BAR;
	else
	{
		Eina_Bool has_style = EINA_FALSE;
		Eina_Bool has_variant = EINA_FALSE;
		Eina_Bool has_weight = EINA_FALSE;
		Eina_Bool has_size = EINA_FALSE;
		Eina_Bool has_family = EINA_FALSE;
		char *tmp;
		char *found;
		char delim[] = { ' ', ',', '\0' };
		char *saveptr;

		/* set default values */
		thiz->size.type = EGUEB_CSS_FONT_SIZE_TYPE_ABSOLUTE;
		thiz->size.value.absolute = EGUEB_CSS_FONT_SIZE_ABSOLUTE_MEDIUM;
		thiz->style = EGUEB_CSS_FONT_STYLE_NORMAL;
		thiz->weight = EGUEB_CSS_FONT_WEIGHT_NORMAL;
		thiz->variant = EGUEB_CSS_FONT_VARIANT_NORMAL;
		thiz->family = NULL;

		/* start parsing */
		tmp = found = strdup(str);
		while ((found = strtok_r(found, delim, &saveptr)))
		{
			Eina_Bool used = EINA_FALSE;
			if (!has_style)
			{
				if (egueb_css_font_style_string_from(&thiz->style, found))
				{
					has_style = EINA_TRUE;
					used = EINA_TRUE;
				}
			}

			if (!has_variant && !used)
			{
				if (egueb_css_font_variant_string_from(&thiz->variant, found))
				{
					has_variant = EINA_TRUE;
					used = EINA_TRUE;
				}
			}

			if (!has_weight && !used)
			{
				if (egueb_css_font_weight_string_from(&thiz->weight, found))
				{
					has_weight = EINA_TRUE;
					used = EINA_TRUE;
				}
			}

			if (!has_size && !used)
			{
				if (egueb_css_font_size_string_from(&thiz->size, found))
				{
					has_size = EINA_TRUE;
					used = EINA_TRUE;
				}
			}

			/* TODO line height */
			if (!has_family && !used)
			{
				thiz->family = egueb_css_font_family_new();
				egueb_css_font_family_string_from(thiz->family, found);
				has_family = EINA_TRUE;
			}
			found = NULL;
		}
		free(tmp);

		/* [ [ <'font-style'> || <'font-variant'> || <'font-weight'> ]? <'font-size'> [ / <'line-height'> ]? <'font-family'> ] */
		//printf("%d %d %d %d %d\n", has_style, has_variant, has_weight, has_size, has_family);
		thiz->type = EGUEB_CSS_FONT_TYPE_USER;
		if (!has_family)
			ret = EINA_FALSE;
	}
	return ret;
}

char * egueb_css_font_string_to(Egueb_Css_Font *thiz)
{
	ERR("Not implemented");
	return NULL;
}

void egueb_css_font_copy(Egueb_Css_Font *src,
		Egueb_Css_Font *dst, Eina_Bool content)
{
	dst->type = src->type;
	dst->style = src->style;
	dst->variant = src->variant;
	dst->weight = src->weight;
	dst->size = src->size;
	if (content)
		dst->family = egueb_dom_list_copy(src->family);
	else
		dst->family = egueb_dom_list_ref(src->family);
}

void egueb_css_font_interpolate(Egueb_Css_Font *v,
		Egueb_Css_Font *a, Egueb_Css_Font *b, double m,
		Egueb_Css_Font *add, Egueb_Css_Font *acc, int mul)
{
	ERR("Not implemented");
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_css_font_reset(Egueb_Css_Font *thiz)
{
	egueb_dom_list_unref(thiz->family);
	thiz->family = NULL;
}
