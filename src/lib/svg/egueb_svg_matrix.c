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

#include "egueb_svg_matrix_private.h"
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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_matrix_string_from(Egueb_Svg_Matrix *matrix, const char *attr)
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

char * egueb_svg_matrix_string_to(Egueb_Svg_Matrix *thiz)
{
	ERR("Not implemented");
	return NULL;
}

void egueb_svg_matrix_interpolate(Egueb_Svg_Matrix *v,
		Egueb_Svg_Matrix *a, Egueb_Svg_Matrix *b, double m,
		Egueb_Svg_Matrix *add, Egueb_Svg_Matrix *acc, int mul)
{
	ERR("Not implemented");
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Svg_Matrix EGUEB_SVG_MATRIX_IDENTITY = { 1, 0, 0, 0, 1, 0, 0, 0, 1};
