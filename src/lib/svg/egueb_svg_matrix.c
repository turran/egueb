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
#include "egueb_svg_matrix.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                          Parsing related functions                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_matrix_matrix_get(Egueb_Svg_Matrix *thiz,
		const char *attr_val, const char **endptr)
{
	int numelements = 6;
	double mx[6];

	if (!egueb_dom_function_get(attr_val, endptr, "matrix", &numelements, mx))
		return EINA_FALSE;
	if (numelements < 6)
		return EINA_FALSE;
	enesim_matrix_values_set(thiz, mx[0], mx[2], mx[4], mx[1], mx[3], mx[5], 0, 0, 1);

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_matrix_translate_get(Egueb_Svg_Matrix *thiz,
		const char *attr_val, const char **endptr)
{
	int numelements = 2;
	double tx[2];

	if (!egueb_dom_function_get(attr_val, endptr, "translate", &numelements, tx))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;
	/* on translation the tx is mandatory but not ty */
	if (numelements == 1)
		tx[1] = 0;
	enesim_matrix_translate(thiz, tx[0], tx[1]);

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_matrix_skewx_get(Egueb_Svg_Matrix *thiz,
		const char *attr_val, const char **endptr)
{
	double angle;
	int numelements = 1;

	if (!egueb_dom_function_get(attr_val, endptr, "skewX", &numelements, &angle))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;

	enesim_matrix_values_set(thiz, 1, tan(angle * M_PI / 180.0), 0, 0, 1, 0, 0, 0, 1);
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_matrix_skewy_get(Egueb_Svg_Matrix *thiz,
		const char *attr_val, const char **endptr)
{
	double angle;
	int numelements = 1;

	if (!egueb_dom_function_get(attr_val, endptr, "skewY", &numelements, &angle))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;

	enesim_matrix_values_set(thiz, 1, 0, 0, tan(angle * M_PI / 180.0), 1, 0, 0, 0, 1);
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_matrix_scale_get(Egueb_Svg_Matrix *thiz,
		const char *attr_val, const char **endptr)
{
	double sx[2];
	int numelements = 2;

	if (!egueb_dom_function_get(attr_val, endptr, "scale", &numelements, sx))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;
	if (numelements == 1)
		sx[1] = sx[0];
	enesim_matrix_scale(thiz, sx[0], sx[1]);

	return EINA_TRUE;
}


static Eina_Bool _egueb_svg_matrix_rotate_get(Egueb_Svg_Matrix *thiz,
		const char *attr_val, const char **endptr)
{
	double rx[3];
	int numelements = 3;

	if (!egueb_dom_function_get(attr_val, endptr, "rotate", &numelements, rx))
		return EINA_FALSE;
	if (numelements < 1)
		return EINA_FALSE;
	enesim_matrix_rotate(thiz, rx[0] * M_PI / 180.0);
	/* handle the origin */
	if (numelements > 1)
	{
		Egueb_Svg_Matrix tx;

		if (numelements < 3)
			return EINA_FALSE;
		enesim_matrix_translate(&tx, rx[1], rx[2]);
		enesim_matrix_compose(&tx, thiz, thiz);
		enesim_matrix_translate(&tx, -rx[1], -rx[2]);
		enesim_matrix_compose(thiz, &tx, thiz);
	}

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Dom value interface                            *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_matrix_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_from(v, data);
}

static void _egueb_svg_matrix_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_to(v, data);
}

static void _egueb_svg_matrix_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	if (v->owned)
	{
		free(v->data.ptr);
		v->data.ptr = NULL;
	}
}

static void _egueb_svg_matrix_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy,
		Eina_Bool content)
{
	Egueb_Svg_Matrix *vl;
	Egueb_Svg_Matrix *cl;

	if (!v->data.ptr)
		return;

	if (!copy->data.ptr)
	{
		copy->data.ptr = calloc(1, sizeof(Egueb_Svg_Matrix));
		copy->owned = EINA_TRUE;
	}
	cl = copy->data.ptr;
	vl = v->data.ptr;
	*cl = *vl;
}

static char * _egueb_svg_matrix_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_matrix_string_to(v->data.ptr);
}

static Eina_Bool _egueb_svg_matrix_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = calloc(1, sizeof(Egueb_Svg_Matrix));
		v->owned = EINA_TRUE;
	}
	return egueb_svg_matrix_string_from(v->data.ptr, str);
}

static void _egueb_svg_matrix_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_matrix_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_svg_matrix_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_svg_matrix_free,
	/* .copy 		= */ _egueb_svg_matrix_copy,
	/* .string_to 		= */ _egueb_svg_matrix_string_to,
	/* .string_from 	= */ _egueb_svg_matrix_string_from,
	/* .interpolate 	= */ _egueb_svg_matrix_interpolate,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Svg_Matrix EGUEB_SVG_MATRIX_IDENTITY = { 1, 0, 0, 0, 1, 0, 0, 0, 1};

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_matrix_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_svg_matrix_string_from(Egueb_Svg_Matrix *matrix, const char *attr)
{
	Eina_Bool ret;
	const char *endptr = NULL;
	typedef Eina_Bool (*Matrix_Get)(Egueb_Svg_Matrix *matrix, const char *attr_val, const char **endptr);

	Matrix_Get m[6] = {
		_egueb_svg_matrix_matrix_get,
		_egueb_svg_matrix_translate_get,
		_egueb_svg_matrix_rotate_get,
		_egueb_svg_matrix_scale_get,
		_egueb_svg_matrix_skewx_get,
		_egueb_svg_matrix_skewy_get,
	};
	enesim_matrix_identity(matrix);
	do
	{
		Egueb_Svg_Matrix parsed;
		int i;

		enesim_matrix_identity(&parsed);
		for (i = 0; i < 6; i++)
		{
			ret = m[i](&parsed, attr, &endptr);
			if (ret) break;
		}
		if (ret)
		{
			DBG("found, composing");
			enesim_matrix_compose(matrix, &parsed, matrix);
		}
		attr = endptr;
	}
	while (endptr && *endptr && ret);

	return ret;
}

EAPI char * egueb_svg_matrix_string_to(Egueb_Svg_Matrix *thiz)
{
	return NULL;
}
