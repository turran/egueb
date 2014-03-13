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
#include "egueb_smil_clock.h"
#include "egueb_smil_repeat_count.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_smil_repeat_count_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_from(v, data);
}

static void _egueb_smil_repeat_count_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_primitive_data_to(v, data);
}

static void _egueb_smil_repeat_count_free(Egueb_Dom_Value *v)
{
	if (v->owned)
	{
		free(v->data.ptr);
		v->data.ptr = NULL;
	}
}

static void _egueb_smil_repeat_count_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy,
		Eina_Bool content)
{
	const Egueb_Smil_Repeat_Count *vd = v->data.ptr;
	Egueb_Smil_Repeat_Count *cd;

	if (!vd) return;
	if (!copy->data.ptr)
	{

		copy->data.ptr = calloc(1, sizeof(Egueb_Smil_Repeat_Count));
		copy->owned = EINA_TRUE;
	}
	cd = copy->data.ptr;
	*cd = *vd;
}

static char * _egueb_smil_repeat_count_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return NULL;
}

static Eina_Bool _egueb_smil_repeat_count_string_from(Egueb_Dom_Value *v, const char *str)
{
	Eina_Bool ret = EINA_TRUE;
	Egueb_Smil_Repeat_Count *thiz;


	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = calloc(1, sizeof(Egueb_Smil_Repeat_Count));
		v->owned = EINA_TRUE;
	}

	thiz = v->data.ptr;
	if (!strcmp(str, "indefinite"))
		thiz->type = EGUEB_SMIL_REPEAT_COUNT_TYPE_INDEFINITE;
	else
	{
		if (sscanf(str, "%lg", &thiz->value) < 0)
		{
			ERR("Impossible to parse the repeat_count '%s'", str);
			ret = EINA_FALSE;
		}
		thiz->type = EGUEB_SMIL_REPEAT_COUNT_TYPE_FINITE;
	}
	return ret;
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_smil_repeat_count_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_smil_repeat_count_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_smil_repeat_count_free,
	/* .copy 		= */ _egueb_smil_repeat_count_copy,
	/* .string_to 		= */ _egueb_smil_repeat_count_string_to,
	/* .string_from 	= */ _egueb_smil_repeat_count_string_from,
	/* .interpolate 	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Smil_Repeat_Count EGUEB_SMIL_REPEAT_COUNT_INDEFINITE = { EGUEB_SMIL_REPEAT_COUNT_TYPE_INDEFINITE, 0 };

EAPI const Egueb_Dom_Value_Descriptor * egueb_smil_repeat_count_descriptor_get(void)
{
	return &_descriptor;
}
