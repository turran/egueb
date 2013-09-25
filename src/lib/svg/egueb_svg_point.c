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
#include "egueb_svg_point.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _egueb_svg_point_descriptor;

static void _egueb_svg_point_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_egueb_svg_point_descriptor);
	egueb_dom_value_primitive_data_from(v, data);
}

static void _egueb_svg_point_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_egueb_svg_point_descriptor);
	egueb_dom_value_primitive_data_to(v, data);
}

static void _egueb_svg_point_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_egueb_svg_point_descriptor);
	if (v->owned)
	{
		free(v->data.ptr);
		v->data.ptr = NULL;
	}
}

static void _egueb_svg_point_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy,
		Eina_Bool content)
{
	Egueb_Svg_Point *vl;
	Egueb_Svg_Point *cl;

	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_egueb_svg_point_descriptor);
	if (!v->data.ptr)
		return;

	if (!copy->data.ptr)
	{
		copy->data.ptr = calloc(1, sizeof(Egueb_Svg_Point));
		copy->owned = EINA_TRUE;
	}
	cl = copy->data.ptr;
	vl = v->data.ptr;
	*cl = *vl;
}

#if 0
static char * _egueb_svg_point_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_egueb_svg_point_descriptor, NULL);
	return egueb_svg_point_string_to(v->data.ptr);
}
#endif

static Eina_Bool _egueb_svg_point_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_egueb_svg_point_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = calloc(1, sizeof(Egueb_Svg_Point));
		v->owned = EINA_TRUE;
	}
	return egueb_svg_point_string_from(v->data.ptr, str);
}

static void _egueb_svg_point_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
	Egueb_Svg_Point *cv = v->data.ptr;
	Egueb_Svg_Point *cb = b->data.ptr;
	Egueb_Svg_Point *ca = a->data.ptr;

	etch_interpolate_double(ca->x, cb->x, m, &cv->x);
	etch_interpolate_double(ca->y, cb->y, m, &cv->y);
}

static Egueb_Dom_Value_Descriptor _egueb_svg_point_descriptor = {
	/* .data_from 		= */ _egueb_svg_point_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_point_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_point_free,
	/* .copy 		= */ _egueb_svg_point_copy,
	/* .string_to 		= */ NULL,
	/* .string_from 	= */ _egueb_svg_point_string_from,
	/* .interpolate 	= */ _egueb_svg_point_interpolate,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_point_descriptor_get(void)
{
	return &_egueb_svg_point_descriptor;
}

EAPI Eina_Bool egueb_svg_point_string_from(Egueb_Svg_Point *thiz, const char *attr)
{
	const char *tmp;
	char *endptr;

	if (!attr) return EINA_FALSE;
	EGUEB_DOM_SPACE_SKIP(attr);
	if (!attr) return EINA_FALSE;
	tmp = attr;

	if (!egueb_dom_double_get(tmp, &endptr, &thiz->x))
		return EINA_FALSE;

	tmp = endptr;
	EGUEB_DOM_SPACE_COMMA_SKIP(tmp);
	if (!tmp) return EINA_FALSE;

	if (!egueb_dom_double_get(tmp, &endptr, &thiz->y))
		return EINA_FALSE;

	return EINA_TRUE;
}
