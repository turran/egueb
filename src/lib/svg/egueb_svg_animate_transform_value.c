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
#include "egueb_svg_animate_transform_value_private.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_svg_animate_transform_value_create(Egueb_Dom_Value *v)
{
	if (!v->data.ptr)
	{
		v->data.ptr = calloc(1, sizeof(Egueb_Svg_Animate_Transform_Value));
		v->owned = EINA_TRUE;
	}
}
/*----------------------------------------------------------------------------*
 *                             Value descriptor                               *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_animate_transform_value_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	egueb_dom_value_primitive_data_from(v, data);
}

static void _egueb_svg_animate_transform_value_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	egueb_dom_value_primitive_data_to(v, data);
}

static void _egueb_svg_animate_transform_value_free(Egueb_Dom_Value *v)
{
	if (v->owned)
	{
		free(v->data.ptr);
		v->data.ptr = NULL;
	}
}

static void _egueb_svg_animate_transform_value_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy,
		Eina_Bool content)
{
	Egueb_Svg_Animate_Transform_Value *vl;
	Egueb_Svg_Animate_Transform_Value *cl;

	if (!v->data.ptr)
		return;

	_egueb_svg_animate_transform_value_create(copy);
	cl = copy->data.ptr;
	vl = v->data.ptr;
	*cl = *vl;
}

static Eina_Bool _egueb_svg_animate_transform_value_string_from(Egueb_Dom_Value *v, const char *str)
{
	Egueb_Svg_Animate_Transform_Value *thiz;
	const char *tmp = str;
	char *end;

	_egueb_svg_animate_transform_value_create(v);
	thiz = v->data.ptr;

	/* parse */
	EGUEB_DOM_SPACE_SKIP(tmp);
	while (tmp)
	{
		double val;

		EGUEB_DOM_SPACE_SKIP(tmp);
		if (!*tmp)
			break;

		if (!egueb_dom_double_get(tmp, &end, &val))
			val = 0;
		if (end == tmp)
			break;
		tmp = end;

		/* too many values */
		if (thiz->count > 3)
		{
			break;
		}

		thiz->values[thiz->count] = val;
		thiz->count++;

		/* skip the comma and the blanks */
		EGUEB_DOM_SPACE_COMMA_SKIP(tmp);
	}
	return EINA_TRUE;
}

static void _egueb_svg_animate_transform_value_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
	Egueb_Svg_Animate_Transform_Value *vv = v->data.ptr;
	Egueb_Svg_Animate_Transform_Value *va = a->data.ptr;
	Egueb_Svg_Animate_Transform_Value *vb = b->data.ptr;

	egueb_dom_value_interpolate_double(va->values[0], vb->values[0], m, &vv->values[0]);
	egueb_dom_value_interpolate_double(va->values[1], vb->values[1], m, &vv->values[1]);
	egueb_dom_value_interpolate_double(va->values[2], vb->values[2], m, &vv->values[2]);
	vv->count = va->count > vb->count ? va->count : vb->count;
}
/*----------------------------------------------------------------------------*
 *                            Rotate descriptor                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_animate_transform_rotate_string_from(Egueb_Dom_Value *v, const char *str)
{
	Egueb_Svg_Animate_Transform_Value *thiz;

	if (!_egueb_svg_animate_transform_value_string_from(v, str))
		return EINA_FALSE;
	thiz = v->data.ptr;

	if (thiz->count != 1 && thiz->count !=3)
	{
		_egueb_svg_animate_transform_value_free(v);
		return EINA_FALSE;
	}
	/* [<cx> <cy>] */
	if (thiz->count == 1)
	{
		thiz->values[1] = 0;
		thiz->values[2] = 0;
	}
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_animate_transform_translate_string_from(Egueb_Dom_Value *v, const char *str)
{
	Egueb_Svg_Animate_Transform_Value *thiz;

	if (!_egueb_svg_animate_transform_value_string_from(v, str))
		return EINA_FALSE;
	thiz = v->data.ptr;

	if (thiz->count != 1 && thiz->count !=2)
	{
		_egueb_svg_animate_transform_value_free(v);
		return EINA_FALSE;
	}

	/* [<ty>] */
	thiz->values[2] = 0;
	if (thiz->count == 1)
	{
		thiz->values[1] = 0;
	}

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_animate_transform_scale_string_from(Egueb_Dom_Value *v, const char *str)
{
	Egueb_Svg_Animate_Transform_Value *thiz;

	if (!_egueb_svg_animate_transform_value_string_from(v, str))
		return EINA_FALSE;
	thiz = v->data.ptr;

	if (thiz->count != 1 && thiz->count !=2)
	{
		_egueb_svg_animate_transform_value_free(v);
		return EINA_FALSE;
	}
	/* [<sy>] */
	thiz->values[2] = 0;
	if (thiz->count == 1)
	{
		thiz->values[1] = thiz->values[0];
	}

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_animate_transform_skew_string_from(Egueb_Dom_Value *v, const char *str)
{
	Egueb_Svg_Animate_Transform_Value *thiz;

	if (!_egueb_svg_animate_transform_value_string_from(v, str))
		return EINA_FALSE;
	thiz = v->data.ptr;

	if (thiz->count != 1)
	{
		_egueb_svg_animate_transform_value_free(v);
		return EINA_FALSE;
	}
	thiz->values[1] = 0;
	thiz->values[2] = 0;

	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Value_Descriptor egueb_svg_animate_transform_rotate_descriptor = {
	/* .data_from 		= */ _egueb_svg_animate_transform_value_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_animate_transform_value_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_animate_transform_value_free,
	/* .copy 		= */ _egueb_svg_animate_transform_value_copy,
	/* .string_to 		= */ NULL,
	/* .string_from 	= */ _egueb_svg_animate_transform_rotate_string_from,
	/* .interpolate 	= */ _egueb_svg_animate_transform_value_interpolate,
};

Egueb_Dom_Value_Descriptor egueb_svg_animate_transform_translate_descriptor = {
	/* .data_from 		= */ _egueb_svg_animate_transform_value_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_animate_transform_value_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_animate_transform_value_free,
	/* .copy 		= */ _egueb_svg_animate_transform_value_copy,
	/* .string_to 		= */ NULL,
	/* .string_from 	= */ _egueb_svg_animate_transform_translate_string_from,
	/* .interpolate 	= */ _egueb_svg_animate_transform_value_interpolate,
};

Egueb_Dom_Value_Descriptor egueb_svg_animate_transform_scale_descriptor = {
	/* .data_from 		= */ _egueb_svg_animate_transform_value_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_animate_transform_value_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_animate_transform_value_free,
	/* .copy 		= */ _egueb_svg_animate_transform_value_copy,
	/* .string_to 		= */ NULL,
	/* .string_from 	= */ _egueb_svg_animate_transform_scale_string_from,
	/* .interpolate 	= */ _egueb_svg_animate_transform_value_interpolate,
};

Egueb_Dom_Value_Descriptor egueb_svg_animate_transform_skew_descriptor = {
	/* .data_from 		= */ _egueb_svg_animate_transform_value_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_animate_transform_value_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_animate_transform_value_free,
	/* .copy 		= */ _egueb_svg_animate_transform_value_copy,
	/* .string_to 		= */ NULL,
	/* .string_from 	= */ _egueb_svg_animate_transform_skew_string_from,
	/* .interpolate 	= */ _egueb_svg_animate_transform_value_interpolate,
};

