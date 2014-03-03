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
#include "egueb_dom_string.h"
#include "egueb_dom_value.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_value_data_from(Egueb_Dom_Value *thiz, Egueb_Dom_Value_Data *data)
{
	if (thiz->descriptor->data_from)
		thiz->descriptor->data_from(thiz, data);
}

void egueb_dom_value_data_to(Egueb_Dom_Value *thiz, Egueb_Dom_Value_Data *data)
{
	if (thiz->descriptor->data_to)
		thiz->descriptor->data_to(thiz, data);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_dom_value_init(Egueb_Dom_Value *thiz,
		const Egueb_Dom_Value_Descriptor *descriptor)
{
	if (thiz->descriptor)
		egueb_dom_value_reset(thiz);
	thiz->descriptor = descriptor;
	if (thiz->descriptor->init)
		thiz->descriptor->init(thiz);
}

EAPI void egueb_dom_value_reset(Egueb_Dom_Value *thiz)
{
	if (thiz->descriptor && thiz->descriptor->free)
		thiz->descriptor->free(thiz);
	thiz->descriptor = NULL;
	thiz->data.ptr = NULL;
	thiz->owned = EINA_FALSE;
}

EAPI void egueb_dom_value_copy(const Egueb_Dom_Value *thiz,
		Egueb_Dom_Value *copy, Eina_Bool content)
{
	EINA_SAFETY_ON_FALSE_RETURN(thiz->descriptor == copy->descriptor);
	if (thiz->descriptor && thiz->descriptor->copy)
		thiz->descriptor->copy(thiz, copy, content);
	else
		*copy = *thiz;
}

EAPI Eina_Bool egueb_dom_value_string_to(const Egueb_Dom_Value *thiz, Egueb_Dom_String **str)
{
	char *value;

	if (!str) return EINA_FALSE;

	if (thiz->descriptor && thiz->descriptor->string_to)
	{
		value = thiz->descriptor->string_to(thiz);
		*str = egueb_dom_string_steal(value);
		return EINA_TRUE;
	}
	else return EINA_FALSE;
}

EAPI Eina_Bool egueb_dom_value_string_from(Egueb_Dom_Value *thiz, Egueb_Dom_String *str)
{
	const char *value;

	if (!str) return EINA_FALSE;
	value = egueb_dom_string_string_get(str);
	if (!value) return EINA_FALSE;

	if (thiz->descriptor && thiz->descriptor->string_from)
		return thiz->descriptor->string_from(thiz, value);
	else
		return EINA_FALSE;
}

EAPI void egueb_dom_value_interpolate(Egueb_Dom_Value *thiz,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b,
		double m, Egueb_Dom_Value *add, Egueb_Dom_Value *acc,
		int mul)
{
	/* TODO check that a, b, add, acc and res are from the same type */
	if (thiz->descriptor && thiz->descriptor->interpolate)
		thiz->descriptor->interpolate(thiz, a, b, m, add, acc, mul);
	else
	{
		if (m < 1)
			egueb_dom_value_copy(a, thiz, EINA_FALSE);
		else 
			egueb_dom_value_copy(b, thiz, EINA_FALSE);
	}
}

EAPI void egueb_dom_value_primitive_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
#if 0
	Egueb_Dom_Value vdata = EGUEB_DOM_VALUE_INIT;
	egueb_dom_value_init(&vdata, v->descriptor);
	vdata.data.ptr = data->ptr;
	egueb_dom_value_copy(&vdata, v, EINA_FALSE);
#else
	v->owned = EINA_FALSE;
	v->data.ptr = data->ptr;
#endif
}

EAPI void egueb_dom_value_primitive_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	Egueb_Dom_Value vdata = EGUEB_DOM_VALUE_INIT;
	egueb_dom_value_init(&vdata, v->descriptor);
	vdata.data.ptr = data->ptr;
	egueb_dom_value_copy(v, &vdata, EINA_FALSE);
}

EAPI void egueb_dom_value_enum_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{

}
