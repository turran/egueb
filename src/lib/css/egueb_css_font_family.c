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
#include "egueb_css_font_family.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_object_private.h"
#include "egueb_css_font_family_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Eina_Bool egueb_css_font_family_value_string_from(
		Egueb_Css_Font_Family_Value *thiz, const char *attr_val)
{
	EGUEB_DOM_SPACE_SKIP(attr_val);
	if (!strcmp(attr_val, "serif"))
	{
		thiz->type = EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_SERIF;
		thiz->family = NULL;
	}
	else if (!strcmp(attr_val, "sans-serif"))
	{
		thiz->type = EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_SANS_SERIF;
		thiz->family = NULL;
	}
	else if (!strcmp(attr_val, "cursive"))
	{
		thiz->type = EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_CURSIVE;
		thiz->family = NULL;
	}
	else if (!strcmp(attr_val, "fantasy"))
	{
		thiz->type = EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_FANTASY;
		thiz->family = NULL;
	}
	else if (!strcmp(attr_val, "monospace"))
	{
		thiz->type = EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_MONOSPACE;
		thiz->family = NULL;
	}
	else
	{
		const char *tmp = attr_val;

		/* handle the 'string' case */
		if (*tmp == '\'')
		{
			const char *end;
			/* search for the end quote */
			for (end = tmp++; *end != '\''; end++);
			thiz->family = egueb_dom_string_new_with_length(tmp, end - tmp);
		}
		else
		{
			thiz->family = egueb_dom_string_new_with_string(tmp);
		}
		thiz->type = EGUEB_CSS_FONT_FAMILY_TYPE_FAMILY;
	}
	return EINA_TRUE;
}

static char * egueb_css_font_family_value_string_to(Egueb_Css_Font_Family_Value *thiz)
{
	ERR("Not implemented");
	return NULL;
}

static void egueb_css_font_family_value_reset(Egueb_Css_Font_Family_Value *thiz)
{
	egueb_dom_string_unref(thiz->family);
	thiz->family = NULL;
}

static void egueb_css_font_family_value_copy(Egueb_Css_Font_Family_Value *src,
		Egueb_Css_Font_Family_Value *dst, Eina_Bool content)
{
	dst->type = src->type;
	if (content)
		dst->family = egueb_dom_string_dup(src->family);
	else
		dst->family = egueb_dom_string_ref(src->family);
}

static void egueb_css_font_family_value_interpolate(Egueb_Css_Font_Family_Value *v,
		Egueb_Css_Font_Family_Value *a, Egueb_Css_Font_Family_Value *b,
		double m, Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
	ERR("Not implemented");
}

EGUEB_DOM_VALUE_PRIMITIVE_BOILERPLATE(egueb_css_font_family_value,
		Egueb_Css_Font_Family_Value);

static const Egueb_Dom_Value_Descriptor *
		egueb_css_font_family_value_value_descriptor_get(void)
{
	return &_egueb_css_font_family_value_descriptor;
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/

static void _egueb_css_font_family_string_from_cb(const char *attr, void *data)
{
	Egueb_Dom_List *list = data;
	Egueb_Css_Font_Family_Value *value;

	value = calloc(1, sizeof(Egueb_Css_Font_Family_Value));
	if (egueb_css_font_family_value_string_from(value, attr))
		egueb_dom_list_item_append(list, value);
	else
		free(value);
}

static char * egueb_css_font_family_string_to(Egueb_Dom_List *thiz)
{
	ERR("Not implemented");
	return NULL;
}

EGUEB_DOM_VALUE_LIST_BOILERPLATE(egueb_css_font_family,
		&_egueb_css_font_family_descriptor);
/*----------------------------------------------------------------------------*
 *                           Attribute interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_OBJECT_BOILERPLATE(Egueb_Css_Font_Family_Attr,
		egueb_css_font_family_value, egueb_css_font_family_attr)
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_css_font_family_string_from(Egueb_Dom_List *thiz, const char *attr_val)
{
	return egueb_dom_list_get(attr_val, ',', _egueb_css_font_family_string_from_cb, thiz);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_css_font_family_attr_new(
		Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_css_font_family_attr);
	egueb_dom_attr_init(n, egueb_dom_string_ref(EGUEB_CSS_NAME_FONT_FAMILY),
			animatable, stylable, inheritable);
	return n;
}

EAPI Egueb_Dom_List * egueb_css_font_family_new(void)
{
	Egueb_Dom_List *ret;
	ret = egueb_dom_list_new(egueb_css_font_family_value_value_descriptor_get());
	return ret;
}

EAPI const Egueb_Dom_Value_Descriptor *
		egueb_css_font_family_value_descriptor_get(void)
{
	return &_egueb_css_font_family_descriptor;
}

