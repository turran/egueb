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
#include "egueb_smil_private.h"
#include "egueb_smil_calc_mode.h"

#include "egueb_dom_value_private.h"
#include "egueb_smil_calc_mode_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_smil_calc_mode_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_i32_data_from(v, data);
}

static void _egueb_smil_calc_mode_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_i32_data_to(v, data);
}

static char * _egueb_smil_calc_mode_string_to(const Egueb_Dom_Value *v)
{
	Eina_Bool ret = EINA_TRUE;

	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	switch (v->data.i32)
	{
		case EGUEB_SMIL_CALC_MODE_DISCRETE:
		return strdup("discrete");

		case EGUEB_SMIL_CALC_MODE_LINEAR:
		return strdup("linear");

		case EGUEB_SMIL_CALC_MODE_PACED:
		return strdup("paced");

		case EGUEB_SMIL_CALC_MODE_SPLINE:
		return strdup("spline");

		default:
		return NULL;
	}
}

static Eina_Bool _egueb_smil_calc_mode_string_from(Egueb_Dom_Value *v, const char *str)
{
	Eina_Bool ret = EINA_TRUE;

	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);

	if (!strcmp(str, "discrete"))
		v->data.i32 = EGUEB_SMIL_CALC_MODE_DISCRETE;
	else if (!strcmp(str, "linear"))
		v->data.i32 = EGUEB_SMIL_CALC_MODE_LINEAR;
	else if (!strcmp(str, "paced"))
		v->data.i32 = EGUEB_SMIL_CALC_MODE_PACED;
	else if (!strcmp(str, "spline"))
		v->data.i32 = EGUEB_SMIL_CALC_MODE_SPLINE;
	else
		ret = EINA_FALSE;
	return ret;
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_smil_calc_mode_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_INT32,
	/* .data_to 		= */ _egueb_smil_calc_mode_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ NULL,
	/* .copy 		= */ NULL,
	/* .string_to 		= */ _egueb_smil_calc_mode_string_to,
	/* .string_from 	= */ _egueb_smil_calc_mode_string_from,
	/* .interpolate 	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
const Egueb_Dom_Value_Descriptor * egueb_smil_calc_mode_descriptor_get(void)
{
	return &_descriptor;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
