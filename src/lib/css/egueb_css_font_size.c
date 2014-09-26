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
#include "egueb_css_font_size.h"
#include "egueb_css_font_size_private.h"
#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_primitive_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _egueb_css_font_size_absolute_string_from(
		Egueb_Css_Font_Size_Absolute *thiz, const char *val)
{
	if (!strcmp(val, "xx-small"))
		*thiz = EGUEB_CSS_FONT_SIZE_ABSOLUTE_XX_SMALL;
	else if (!strcmp(val, "x-small"))
		*thiz = EGUEB_CSS_FONT_SIZE_ABSOLUTE_X_SMALL;
	else if (!strcmp(val, "small"))
		*thiz = EGUEB_CSS_FONT_SIZE_ABSOLUTE_SMALL;
	else if (!strcmp(val, "medium"))
		*thiz = EGUEB_CSS_FONT_SIZE_ABSOLUTE_MEDIUM;
	else if (!strcmp(val, "large"))
		*thiz = EGUEB_CSS_FONT_SIZE_ABSOLUTE_LARGE;
	else if (!strcmp(val, "x-large"))
		*thiz = EGUEB_CSS_FONT_SIZE_ABSOLUTE_X_LARGE;
	else if (!strcmp(val, "xx-large"))
		*thiz = EGUEB_CSS_FONT_SIZE_ABSOLUTE_XX_LARGE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

static Eina_Bool _egueb_css_font_size_relative_string_from(
		Egueb_Css_Font_Size_Relative *thiz, const char *val)
{
	if (!strcmp(val, "larger"))
		*thiz = EGUEB_CSS_FONT_SIZE_RELATIVE_LARGER;
	else if (!strcmp(val, "smaller"))
		*thiz = EGUEB_CSS_FONT_SIZE_RELATIVE_SMALLER;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_VALUE_PRIMITIVE_SIMPLE_BOILERPLATE(egueb_css_font_size, Egueb_Css_Font_Size);
/*----------------------------------------------------------------------------*
 *                           Attribute interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_PRIMITIVE_BOILERPLATE(Egueb_Css_Font_Size,
		Egueb_Svg_Font_Size_Attr, egueb_css_font_size_value,
		egueb_css_font_size_attr)
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_css_font_size_string_from(Egueb_Css_Font_Size *thiz,
		const char *str)
{
	if (_egueb_css_font_size_absolute_string_from(&thiz->value.absolute, str))
		thiz->type = EGUEB_CSS_FONT_SIZE_TYPE_ABSOLUTE;
	else if (_egueb_css_font_size_relative_string_from(&thiz->value.relative, str))
		thiz->type = EGUEB_CSS_FONT_SIZE_TYPE_RELATIVE;
	else if (egueb_css_length_string_from(&thiz->value.length, str))
		thiz->type = EGUEB_CSS_FONT_SIZE_TYPE_LENGTH;
	else if (egueb_css_percentage_string_from(&thiz->value.percentage, str))
		thiz->type = EGUEB_CSS_FONT_SIZE_TYPE_PERCENTAGE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

char * egueb_css_font_size_string_to(Egueb_Css_Font_Size *thiz)
{
	ERR("Not implemented");
	return NULL;
}

void egueb_css_font_size_interpolate(Egueb_Css_Font_Size *v,
		Egueb_Css_Font_Size *a, Egueb_Css_Font_Size *b, double m,
		Egueb_Css_Font_Size *add, Egueb_Css_Font_Size *acc, int mul)
{
	ERR("Not implemented");
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_css_font_size_value_descriptor_get(void)
{
	return &_egueb_css_font_size_descriptor;
}

EAPI Egueb_Dom_Node * egueb_css_font_size_attr_new(
		const Egueb_Css_Font_Size *def, Eina_Bool animatable,
		Eina_Bool stylable, Eina_Bool inheritable)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_css_font_size_attr);
	egueb_dom_attr_init(n, egueb_dom_string_ref(EGUEB_CSS_NAME_FONT_SIZE),
			animatable, stylable, inheritable);
	if (def) egueb_dom_attr_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, def);
	return n;
}

