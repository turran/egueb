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
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_font_size_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_from(v, data);
}

static void _egueb_svg_font_size_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_to(v, data);
}

static void _egueb_svg_font_size_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	if (v->owned)
	{
		free(v->data.ptr);
		v->data.ptr = NULL;
	}
}

static void _egueb_svg_font_size_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy)
{
	Egueb_Svg_Font_Size *vl;
	Egueb_Svg_Font_Size *cl;

	if (!v->data.ptr)
		return;

	if (!copy->data.ptr)
	{
		copy->data.ptr = calloc(1, sizeof(Egueb_Svg_Font_Size));
		copy->owned = EINA_TRUE;
	}
	cl = copy->data.ptr;
	vl = v->data.ptr;
	*cl = *vl;
}

static char * _egueb_svg_font_size_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_font_size_string_to(v->data.ptr);
}

static Eina_Bool _egueb_svg_font_size_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = calloc(1, sizeof(Egueb_Svg_Font_Size));
		v->owned = EINA_TRUE;
	}
	return egueb_svg_font_size_string_from(v->data.ptr, str);
}

static void _egueb_svg_font_size_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_font_size_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_font_size_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_font_size_free,
	/* .copy 		= */ _egueb_svg_font_size_copy,
	/* .string_to 		= */ _egueb_svg_font_size_string_to,
	/* .string_from 	= */ _egueb_svg_font_size_string_from,
	/* .interpolate 	= */ _egueb_svg_font_size_interpolate,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_font_size_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_svg_font_size_string_from(Egueb_Svg_Font_Size *thiz, const char *attr_val)
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

EAPI char * egueb_svg_font_size_string_to(Egueb_Svg_Font_Size *thiz)
{
	ERR("TODO");
	return NULL;
}

EAPI double egueb_svg_font_size_final_get(const Egueb_Svg_Font_Size *thiz,
		double width, double height, double font_size,
		double parent_font_size)
{
	double ret = 0;
	switch (thiz->type)
	{
		case EGUEB_SVG_FONT_SIZE_TYPE_ABSOLUTE:

		break;
		case EGUEB_SVG_FONT_SIZE_TYPE_RELATIVE:

		break;
		case EGUEB_SVG_FONT_SIZE_TYPE_LENGTH:
		ret = egueb_svg_length_final_get(&thiz->value.length, width, height, font_size);
		break;
	}
	return ret;
}

