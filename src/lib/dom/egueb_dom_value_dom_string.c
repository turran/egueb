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
#include "egueb_dom_private.h"

#include "egueb_dom_main.h"
#include "egueb_dom_string.h"
#include "egueb_dom_value.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Egueb_Dom_Value_Descriptor _descriptor;
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static void _dom_string_data_from(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	v->data.ptr = data->ptr;
	v->owned = EINA_TRUE;
}

static void _dom_string_data_to(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	Egueb_Dom_String **ptr = data->ptr;
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	if (v->data.ptr)
		*ptr = egueb_dom_string_ref(v->data.ptr);
	else
		*ptr = NULL;
}

static void _dom_string_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	if (v->owned)
		egueb_dom_string_unref(v->data.ptr);
	v->data.ptr = NULL;
	v->owned = EINA_FALSE;
}

static void _dom_string_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	if (copy->data.ptr && copy->owned)
	{
		egueb_dom_string_unref(copy->data.ptr);
		copy->data.ptr = NULL;
	}
	if (v->data.ptr)
	{
		copy->data.ptr = egueb_dom_string_ref(v->data.ptr);
		copy->owned = EINA_TRUE;
	}
}

static char * _dom_string_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	if (v->data.ptr)
		return strdup(egueb_dom_string_string_get(v->data.ptr));
	else
		return NULL;
}

static Eina_Bool _dom_string_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (v->data.ptr)
	{
		egueb_dom_string_string_set(v->data.ptr, str);
	}
	else
	{
		v->data.ptr = egueb_dom_string_new_with_string(str);
		v->owned = EINA_TRUE;
	}
	return EINA_TRUE;
}

static void _dom_string_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
	if (m < 1) egueb_dom_value_copy(a, v);
	else egueb_dom_value_copy(b, v);
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _dom_string_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _dom_string_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _dom_string_free,
	/* .copy 		= */ _dom_string_copy,
	/* .string_to 		= */ _dom_string_string_to,
	/* .string_from 	= */ _dom_string_string_from,
	/* .interpolate 	= */ _dom_string_interpolate,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor *
		egueb_dom_value_dom_string_descriptor_get(void)
{
	return &_descriptor;
}

EAPI void egueb_dom_value_dom_string_set(Egueb_Dom_Value *v, Egueb_Dom_String *str)
{
	egueb_dom_value_reset(v);
	if (str)
	{
		v->descriptor = egueb_dom_value_dom_string_descriptor_get();
		v->data.ptr = egueb_dom_string_ref(str);
		v->owned = EINA_TRUE;
	}
}

EAPI Egueb_Dom_String * egueb_dom_value_dom_string_dup(Egueb_Dom_Value *v)
{
	if (v->data.ptr) return egueb_dom_string_ref(v->data.ptr);
	else return NULL;
}

EAPI const Egueb_Dom_String * egueb_dom_value_dom_string_get(Egueb_Dom_Value *v)
{
	return v->data.ptr;
}
