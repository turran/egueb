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
#include "egueb_css_length.h"
#include "egueb_css_percentage.h"
#include "egueb_css_font_style.h"
#include "egueb_css_font_variant.h"
#include "egueb_css_font_weight.h"
#include "egueb_css_font_size.h"
#include "egueb_css_font_family.h"
#include "egueb_css_font.h"
#include "egueb_css_value_font.h"

#include "egueb_dom_attr_primitive_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                           Attribute interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_PRIMITIVE_BOILERPLATE(Egueb_Css_Font,
		Egueb_Css_Attr_Font, egueb_css_value_font,
		egueb_css_attr_font)
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_css_attr_font_new(Egueb_Dom_String *ns,
		Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_css_attr_font);
	egueb_dom_attr_init(n, egueb_dom_string_ref(EGUEB_CSS_NAME_FONT),
			ns, animatable, stylable, inheritable);
	return n;
}

/* TODO pass the an Enesim_Font */
EAPI Enesim_Text_Font * egueb_css_attr_font_resolve(Egueb_Dom_Node *attr,
		double rel_font_size, double rel_x_height)
{
	Egueb_Css_Font final_font = { 0 };
	Enesim_Text_Font *font = NULL;
	Enesim_Text_Engine *e;
	double font_size = 0;
	int count;
	int i;

	e = enesim_text_engine_default_get();
	if (!e)
		return NULL;

	if (!egueb_dom_attr_final_get(attr, &final_font))
	{
		enesim_text_engine_unref(e);
		return NULL;
	}

	switch (final_font.size.type)
	{
		case EGUEB_CSS_FONT_SIZE_TYPE_ABSOLUTE:
		ERR("TODO %d", final_font.size.value.absolute);
		break;

		case EGUEB_CSS_FONT_SIZE_TYPE_RELATIVE:
		{
			if (final_font.size.value.relative == EGUEB_CSS_FONT_SIZE_RELATIVE_LARGER)
				font_size = rel_font_size * 1.25;
			else
				font_size = rel_font_size * 0.75;
		}
		break;

		case EGUEB_CSS_FONT_SIZE_TYPE_LENGTH:
		{
			font_size = egueb_css_length_final_get(
					&final_font.size.value.length, rel_font_size,
					rel_x_height);
		}
		break;

		case EGUEB_CSS_FONT_SIZE_TYPE_PERCENTAGE:
		{
			font_size = rel_font_size * final_font.size.value.percentage / 100;
		}
		break;
	}

	count = egueb_dom_list_length(final_font.family);
	for (i = 0; i < count; i++)
	{
		Egueb_Css_Font_Family_Value *value;
		const char *family = NULL;
		char *fc_description = NULL;
		int weight = 80;

		value = egueb_dom_list_item_get(final_font.family, i);
		switch (value->type)
		{
			case EGUEB_CSS_FONT_FAMILY_TYPE_FAMILY:
			family = egueb_dom_string_chars_get(value->family);
			break;

			case EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_SERIF:
			family = ":family=sans";
			break;

			case EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_SANS_SERIF:
			family = ":family=sans-serif";
			break;

			case EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_CURSIVE:
			family = ":family=cursive";
			break;

			case EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_FANTASY:
			family = ":family=fantasy";
			break;

			case EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_MONOSPACE:
			family = ":family=monospace";
			break;
		}
		switch (final_font.weight)
		{
			case EGUEB_CSS_FONT_WEIGHT_NORMAL:
			weight = 80;
			break;

			case EGUEB_CSS_FONT_WEIGHT_BOLD:
			weight = 200;
			break;

			case EGUEB_CSS_FONT_WEIGHT_BOLDER:
			case EGUEB_CSS_FONT_WEIGHT_LIGHTER:
			default:
			break;	
		}

		if (asprintf(&fc_description, "%s:weight=%d", family, weight) < 0)
			break;
		/* TODO add the style and variant */
		if (fc_description)
		{
			font = enesim_text_font_new_description_from(e, fc_description,
					font_size);
			if (font)
				break;
		}
	}
	enesim_text_engine_unref(e);
	egueb_css_font_reset(&final_font);

	return font;
}
