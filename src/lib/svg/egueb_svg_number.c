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
#include "egueb_svg_number.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_svg_number_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	v->data.d = data->d;
}

static void _egueb_svg_number_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	double *ptr;
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	ptr = data->ptr;
	if (!ptr) return;
	*ptr = v->data.d;
}

static char * _egueb_svg_number_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_svg_number_string_to(v->data.d);
}

static Eina_Bool _egueb_svg_number_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	return egueb_svg_number_string_from(&v->data.d, str);
}

static void _egueb_svg_number_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
	etch_interpolate_double(a->data.d, b->data.d, m, &v->data.d);
	if (add)
		v->data.d += add->data.d;
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_svg_number_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_DOUBLE,
	/* .data_to 		= */ _egueb_svg_number_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ NULL,
	/* .copy 		= */ NULL,
	/* .string_to 		= */ _egueb_svg_number_string_to,
	/* .string_from 	= */ _egueb_svg_number_string_from,
	/* .interpolate 	= */ _egueb_svg_number_interpolate,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_number_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_svg_number_string_from(Egueb_Svg_Number *thiz,
		const char *attr_val)
{
	Egueb_Svg_Number val;
	char *endptr;

	if (!attr_val || !*attr_val)
		return EINA_FALSE;

	val = eina_extra_strtod(attr_val, &endptr);
	if (errno == ERANGE)
		return EINA_FALSE;
	if ((val == 0) && (attr_val == endptr))
		return EINA_FALSE;

	/* else, conversion has been done */
	if ((endptr == NULL) || (*endptr == '\0'))
	{
		*thiz = val;
		return EINA_TRUE;
	}

	ERR("Number %s is invalid", attr_val);
	return EINA_FALSE;
}

EAPI char * egueb_svg_number_string_to(Egueb_Svg_Number thiz)
{
	return NULL;
}

