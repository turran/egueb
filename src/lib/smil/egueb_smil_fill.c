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
#include "egueb_smil_fill.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_smil_fill_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_i32_data_from(v, data);
}

static void _egueb_smil_fill_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_i32_data_to(v, data);
}

static char * _egueb_smil_fill_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_smil_fill_string_to(v->data.i32);
}

static Eina_Bool _egueb_smil_fill_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	return egueb_smil_fill_string_from((Egueb_Smil_Fill *)&v->data.i32, str);
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_smil_fill_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_INT32,
	/* .data_to 		= */ _egueb_smil_fill_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ NULL,
	/* .copy 		= */ NULL,
	/* .string_to 		= */ _egueb_smil_fill_string_to,
	/* .string_from 	= */ _egueb_smil_fill_string_from,
	/* .interpolate 	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_smil_fill_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_smil_fill_string_from(Egueb_Smil_Fill *fill, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "freeze"))
		*fill = EGUEB_SMIL_FILL_FREEZE;
	else if (!strcmp(attr, "remove"))
		*fill = EGUEB_SMIL_FILL_REMOVE;
	else
		ret = EINA_FALSE;
	return ret;
}

EAPI char * egueb_smil_fill_string_to(Egueb_Smil_Fill fill)
{
	Eina_Bool ret = EINA_TRUE;

	switch (fill)
	{
		case EGUEB_SMIL_FILL_FREEZE:
		return strdup("freeze");

		case EGUEB_SMIL_FILL_REMOVE:
		return strdup("remove");

		default:
		return NULL;
	}
}
