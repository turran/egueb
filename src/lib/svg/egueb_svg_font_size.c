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

#include "egueb_svg_main_private.h"
#include "egueb_svg_length.h"
#include "egueb_svg_font_size.h"

#include "egueb_dom_value_private.h"
#include "egueb_svg_length_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _egueb_svg_font_size_absolute_string_from(
		Egueb_Svg_Font_Size_Absolute *thiz, const char *val)
{
	if (!strcmp(val, "xx-small"))
		*thiz = EGUEB_SVG_FONT_SIZE_ABSOLUTE_XX_SMALL;
	else if (!strcmp(val, "x-small"))
		*thiz = EGUEB_SVG_FONT_SIZE_ABSOLUTE_X_SMALL;
	else if (!strcmp(val, "small"))
		*thiz = EGUEB_SVG_FONT_SIZE_ABSOLUTE_SMALL;
	else if (!strcmp(val, "medium"))
		*thiz = EGUEB_SVG_FONT_SIZE_ABSOLUTE_MEDIUM;
	else if (!strcmp(val, "large"))
		*thiz = EGUEB_SVG_FONT_SIZE_ABSOLUTE_LARGE;
	else if (!strcmp(val, "x-large"))
		*thiz = EGUEB_SVG_FONT_SIZE_ABSOLUTE_X_LARGE;
	else if (!strcmp(val, "xx-large"))
		*thiz = EGUEB_SVG_FONT_SIZE_ABSOLUTE_XX_LARGE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_font_size_relative_string_from(
		Egueb_Svg_Font_Size_Relative *thiz, const char *val)
{
	if (!strcmp(val, "larger"))
		*thiz = EGUEB_SVG_FONT_SIZE_RELATIVE_LARGER;
	else if (!strcmp(val, "smaller"))
		*thiz = EGUEB_SVG_FONT_SIZE_RELATIVE_SMALLER;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Eina_Bool egueb_svg_font_size_string_from(Egueb_Svg_Font_Size *thiz, const char *attr_val)
{
	if (_egueb_svg_font_size_absolute_string_from(&thiz->value.absolute, attr_val))
		thiz->type = EGUEB_SVG_FONT_SIZE_TYPE_ABSOLUTE;
	else if (_egueb_svg_font_size_relative_string_from(&thiz->value.relative, attr_val))
		thiz->type = EGUEB_SVG_FONT_SIZE_TYPE_RELATIVE;
	else if (egueb_svg_length_string_from(&thiz->value.length, attr_val))
		thiz->type = EGUEB_SVG_FONT_SIZE_TYPE_LENGTH;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

static char * egueb_svg_font_size_string_to(Egueb_Svg_Font_Size *thiz)
{
	ERR("TODO");
	return NULL;
}

static void egueb_svg_font_size_interpolate(Egueb_Svg_Font_Size *v,
		Egueb_Svg_Font_Size *a, Egueb_Svg_Font_Size *b, double m,
		Egueb_Svg_Font_Size *add, Egueb_Svg_Font_Size *acc, int mul)
{
	ERR("Not implemented");
}

EGUEB_DOM_VALUE_PRIMITIVE_SIMPLE_BOILERPLATE(egueb_svg_font_size, Egueb_Svg_Font_Size);
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_font_size_descriptor_get(void)
{
	return &_egueb_svg_font_size_descriptor;
}

EAPI double egueb_svg_font_size_final_get(const Egueb_Svg_Font_Size *thiz,
		double width, double height, double font_size,
		double parent_font_size)
{
	double ret = 0;
	switch (thiz->type)
	{
		case EGUEB_SVG_FONT_SIZE_TYPE_ABSOLUTE:
		ERR("TODO");
		break;
		case EGUEB_SVG_FONT_SIZE_TYPE_RELATIVE:
		if (thiz->value.relative == EGUEB_SVG_FONT_SIZE_RELATIVE_LARGER)
			ret = font_size * 1.25;
		else
			ret = font_size * 0.75;
		break;
		case EGUEB_SVG_FONT_SIZE_TYPE_LENGTH:
		ret = egueb_svg_length_final_get(&thiz->value.length, width, height, font_size);
		break;
	}
	return ret;
}

