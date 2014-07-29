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

#ifndef _EGUEB_DOM_VALUE_LIST_H_
#define _EGUEB_DOM_VALUE_LIST_H_

EAPI void egueb_dom_value_list_data_from(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data);
EAPI void egueb_dom_value_list_data_to(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data);
EAPI void egueb_dom_value_list_free(Egueb_Dom_Value *v);
EAPI void egueb_dom_value_list_copy(const Egueb_Dom_Value *v,
		Egueb_Dom_Value *copy, Eina_Bool content);
EAPI void egueb_dom_value_list_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul);

/* Boilerplate code for a list based type
 */ 
#define EGUEB_DOM_VALUE_LIST_BOILERPLATE(name, content_descriptor)		\
static Egueb_Dom_Value_Descriptor _##name##_descriptor;				\
										\
static void _##name##_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)	\
{										\
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_##name##_descriptor);	\
	egueb_dom_value_list_data_from(v, data);				\
}										\
										\
static void _##name##_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)	\
{										\
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_##name##_descriptor);	\
	egueb_dom_value_list_data_to(v, data);					\
}										\
										\
static void _##name##_free(Egueb_Dom_Value *v)					\
{										\
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_##name##_descriptor);	\
	egueb_dom_value_list_free(v);						\
}										\
										\
static void _##name##_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy,	\
		Eina_Bool content)						\
{										\
	egueb_dom_value_list_copy(v, copy, content);				\
}										\
										\
static char * _##name##_string_to(const Egueb_Dom_Value *v)			\
{										\
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_##name##_descriptor,	\
			NULL);							\
	return name##_string_to(v->data.ptr);					\
}										\
										\
static Eina_Bool _##name##_string_from(Egueb_Dom_Value *v, const char *str)	\
{										\
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_##name##_descriptor,	\
			EINA_FALSE);						\
	if (!v->data.ptr)							\
	{									\
		v->data.ptr = egueb_dom_list_new(content_descriptor);		\
		v->owned = EINA_TRUE;						\
	}									\
	else									\
	{									\
		egueb_dom_list_clear(v->data.ptr);				\
	}									\
	return name##_string_from(v->data.ptr, str);				\
}										\
										\
static void _##name##_interpolate(Egueb_Dom_Value *v, Egueb_Dom_Value *a,	\
		Egueb_Dom_Value *b, double m, Egueb_Dom_Value *add,		\
		Egueb_Dom_Value *acc, int mul)					\
{										\
	egueb_dom_value_list_interpolate(v, a, b, m, add, acc, mul);		\
}										\
										\
static Egueb_Dom_Value_Descriptor _##name##_descriptor = {			\
	/* .data_from 		= */ _##name##_data_from,			\
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,		\
	/* .data_to 		= */ _##name##_data_to,				\
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,		\
	/* .init 		= */ NULL,					\
	/* .free 		= */ _##name##_free,				\
	/* .copy 		= */ _##name##_copy,				\
	/* .string_to 		= */ _##name##_string_to,			\
	/* .string_from 	= */ _##name##_string_from,			\
	/* .interpolate 	= */ _##name##_interpolate,			\
}


#endif
