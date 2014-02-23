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
#include "egueb_svg_length.h"
#include "egueb_svg_stroke_dasharray.h"
#include "egueb_dom_value_list.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_svg_stroke_dasharray_string_to_cb(void *data, void *user_data)
{
	Egueb_Svg_Length *seg = data;
	Eina_Strbuf *strbuf = user_data;
	char *str;

	str = egueb_svg_length_string_to(seg);
	if (eina_strbuf_length_get(strbuf))
	{
		eina_strbuf_append_char(strbuf, ' ');
	}
	if (str)
	{
		eina_strbuf_append(strbuf, str);
		free(str);
	}
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_stroke_dasharray_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_data_from(v, data);
}

static void _egueb_svg_stroke_dasharray_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_data_to(v, data);
}

static void _egueb_svg_stroke_dasharray_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_free(v);
}

static void _egueb_svg_stroke_dasharray_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy,
		Eina_Bool content)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_copy(v, copy, content);
}

static char * _egueb_svg_stroke_dasharray_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_stroke_dasharray_string_to(v->data.ptr);
}

static Eina_Bool _egueb_svg_stroke_dasharray_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = egueb_dom_list_new(egueb_svg_length_descriptor_get());
		v->owned = EINA_TRUE;
	}
	return egueb_svg_stroke_dasharray_string_from(v->data.ptr, str);
}

static void _egueb_svg_stroke_dasharray_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
	egueb_dom_value_list_interpolate(v, a, b, m, add, acc, mul);
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_stroke_dasharray_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_stroke_dasharray_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_stroke_dasharray_free,
	/* .copy 		= */ _egueb_svg_stroke_dasharray_copy,
	/* .string_to 		= */ _egueb_svg_stroke_dasharray_string_to,
	/* .string_from 	= */ _egueb_svg_stroke_dasharray_string_from,
	/* .interpolate 	= */ _egueb_svg_stroke_dasharray_interpolate,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_stroke_dasharray_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_svg_stroke_dasharray_string_from(Egueb_Dom_List *l,
		const char *value)
{
	egueb_dom_list_clear(l);
	if (!strcmp(value, "none"))
		return EINA_TRUE;
	else
	{
		char *found;
		char *attr = (char *)value;
		char sep;

		EGUEB_BASE_SPACE_SKIP(attr);

 		sep = ',';
		found = strchr(attr, sep);
		if (!found)
		{
			sep = ' ';
			found = strchr(attr, sep);
		}

		while (found)
		{
			Egueb_Svg_Length *length;

			*found = '\0';
			EGUEB_BASE_SPACE_SKIP(attr);

			length = calloc(1, sizeof(Egueb_Svg_Length));
			egueb_svg_length_string_from(length, attr);
			egueb_dom_list_item_append(l, length);

			*found = sep;
			attr = found + 1;
			EGUEB_BASE_SPACE_SKIP(attr);

			sep = ',';
			found = strchr(attr, sep);
			if (!found)
			{
				sep = ' ';
				found = strchr(attr, sep);
			}
		}
		if (attr)
		{
			Egueb_Svg_Length *length;

			length = calloc(1, sizeof(Egueb_Svg_Length));
			egueb_svg_length_string_from(length, attr);
			egueb_dom_list_item_append(l, length);
		}
		return EINA_TRUE;
	}
}

EAPI char * egueb_svg_stroke_dasharray_string_to(Egueb_Dom_List *l)
{
	char *ret;

	if (!egueb_dom_list_length(l))
	{
		ret = strdup("none");
	}
	else
	{
		Eina_Strbuf *strbuf;
		strbuf = eina_strbuf_new();
		egueb_dom_list_foreach(l, _egueb_svg_stroke_dasharray_string_to_cb, strbuf);
		ret = eina_strbuf_string_steal(strbuf);
		eina_strbuf_free(strbuf);
	}
	return ret;
}
