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
#include "egueb_dom_value.h"
#include "egueb_dom_list.h"
#include "egueb_dom_string.h"
#include "egueb_dom_utils.h"
#include "egueb_dom_string_list.h"
#include "egueb_dom_value_list.h"
#include "egueb_dom_value_dom_string.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_dom_string_list_cb(const char *attr, void *data)
{
	Egueb_Dom_String *s;
	Egueb_Dom_List *l = data;

	s = egueb_dom_string_new_with_string(attr);
	egueb_dom_list_item_append(l, s);
	return;
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Value_Descriptor _descriptor;

static void _egueb_dom_string_list_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_data_from(v, data);
}

static void _egueb_dom_string_list_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_data_to(v, data);
}

static void _egueb_dom_string_list_free(Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_free(v);
}

static void _egueb_dom_string_list_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy,
		Eina_Bool content)
{
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_descriptor);
	egueb_dom_value_list_copy(v, copy, content);
}

static char * _egueb_dom_string_list_string_to(const Egueb_Dom_Value *v)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, NULL);
	return egueb_dom_string_list_string_to(v->data.ptr);
}

static Eina_Bool _egueb_dom_string_list_string_from(Egueb_Dom_Value *v, const char *str)
{
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_descriptor, EINA_FALSE);
	if (!v->data.ptr)
	{
		v->data.ptr = egueb_dom_list_new(egueb_dom_value_dom_string_descriptor_get());
		v->owned = EINA_TRUE;
	}
	return egueb_dom_string_list_string_from(v->data.ptr, str);
}

static Egueb_Dom_Value_Descriptor _descriptor = {
	/* .data_from 		= */ _egueb_dom_string_list_data_from,
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .data_to 		= */ _egueb_dom_string_list_data_to,
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,
	/* .init 		= */ NULL,
	/* .free 		= */ _egueb_dom_string_list_free,
	/* .copy 		= */ _egueb_dom_string_list_copy,
	/* .string_to 		= */ _egueb_dom_string_list_string_to,
	/* .string_from 	= */ _egueb_dom_string_list_string_from,
	/* .interpolate 	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_dom_string_list_descriptor_get(void)
{
	return &_descriptor;
}

EAPI Eina_Bool egueb_dom_string_list_string_from(Egueb_Dom_List *l, const char *str)
{
	egueb_dom_list_clear(l);
	return egueb_dom_list_get(str, ';', _egueb_dom_string_list_cb, l);
}

EAPI char * egueb_dom_string_list_string_to(Egueb_Dom_List *l)
{
	ERR("Not supported");
	return NULL;
}

