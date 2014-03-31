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

#ifndef _EGUEB_DOM_VALUE_H_
#define _EGUEB_DOM_VALUE_H_

#include "egueb_dom_string.h"

typedef struct _Egueb_Dom_Value_Descriptor Egueb_Dom_Value_Descriptor;
typedef struct _Egueb_Dom_Value Egueb_Dom_Value;

typedef enum _Egueb_Dom_Value_Data_Type
{
	EGUEB_DOM_VALUE_DATA_TYPE_INT32,
	EGUEB_DOM_VALUE_DATA_TYPE_INT64,
	EGUEB_DOM_VALUE_DATA_TYPE_DOUBLE,
	EGUEB_DOM_VALUE_DATA_TYPE_PTR,
} Egueb_Dom_Value_Data_Type;

typedef union _Egueb_Dom_Value_Data
{
	int32_t i32;
	int64_t i64;
	double d;
	void *ptr;
} Egueb_Dom_Value_Data;

/* The value descriptor. It is used to create new value definitions */
typedef void (*Egueb_Dom_Value_Data_From)(Egueb_Dom_Value *thiz, Egueb_Dom_Value_Data *data);
typedef void (*Egueb_Dom_Value_Data_To)(Egueb_Dom_Value *thiz, Egueb_Dom_Value_Data *data);

typedef void (*Egueb_Dom_Value_Init)(Egueb_Dom_Value *thiz);
typedef void (*Egueb_Dom_Value_Free)(Egueb_Dom_Value *thiz);
typedef void (*Egueb_Dom_Value_Copy)(const Egueb_Dom_Value *thiz,
		Egueb_Dom_Value *copy, Eina_Bool content);

typedef char * (*Egueb_Dom_Value_String_To)(const Egueb_Dom_Value *thiz);
typedef Eina_Bool (*Egueb_Dom_Value_String_From)(Egueb_Dom_Value *thiz,
		const char *str);

typedef void (*Egueb_Dom_Value_Interpolate)(Egueb_Dom_Value *thiz,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul);

struct _Egueb_Dom_Value_Descriptor {
	Egueb_Dom_Value_Data_From data_from;
	Egueb_Dom_Value_Data_Type data_from_type;

	Egueb_Dom_Value_Data_To data_to;
	Egueb_Dom_Value_Data_Type data_to_type;

	Egueb_Dom_Value_Init init;
	Egueb_Dom_Value_Free free;
	Egueb_Dom_Value_Copy copy;

	Egueb_Dom_Value_String_To string_to;
	Egueb_Dom_Value_String_From string_from;

	Egueb_Dom_Value_Interpolate interpolate;
};

struct _Egueb_Dom_Value
{
	const Egueb_Dom_Value_Descriptor *descriptor;
	Eina_Bool owned;
	Egueb_Dom_Value_Data data;
};

/*----------------------------------------------------------------------------*
 *                               i32 helpers                                  *
 *----------------------------------------------------------------------------*/
static inline void egueb_dom_value_i32_set(Egueb_Dom_Value *v, int i)
{
	v->data.i32 = i;
}

static inline int egueb_dom_value_i32_get(Egueb_Dom_Value *v)
{
	return v->data.i32;
}

static inline void egueb_dom_value_i32_data_from(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	v->data.i32 = data->i32;
}

static inline void egueb_dom_value_i32_data_to(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	int *ptr = data->ptr;
	if (!ptr) return;
	*ptr = v->data.i32;
}


#define EGUEB_DOM_VALUE_INIT { 0, EINA_FALSE,  { 0 } }

EAPI void egueb_dom_value_init(Egueb_Dom_Value *thiz,
		const Egueb_Dom_Value_Descriptor *descriptor);
EAPI void egueb_dom_value_reset(Egueb_Dom_Value *thiz);
EAPI void egueb_dom_value_copy(const Egueb_Dom_Value *thiz, Egueb_Dom_Value *copy, Eina_Bool content);
EAPI Eina_Bool egueb_dom_value_string_to(const Egueb_Dom_Value *thiz,
		Egueb_Dom_String **str);
EAPI Eina_Bool egueb_dom_value_string_from(Egueb_Dom_Value *thiz,
		Egueb_Dom_String *str);
EAPI void egueb_dom_value_interpolate(Egueb_Dom_Value *thiz,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b,
		double m, Egueb_Dom_Value *add, Egueb_Dom_Value *acc,
		int mul);

/* primitive (structs) helpers */
EAPI void egueb_dom_value_primitive_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data);
EAPI void egueb_dom_value_primitive_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data);
/* enum helpers */
EAPI void egueb_dom_value_enum_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul);


/* Boilerplate code for an enum
 * Where name is the name used to prefix every function and c_type is the c_type
 * of the enum.
 *
 * The implementation should provide:
 * name##_string_to
 * name##_string_from
 */
#define EGUEB_DOM_VALUE_ENUM_BOILERPLATE(name, c_type)				\
static Egueb_Dom_Value_Descriptor _##name##_descriptor;				\
										\
static void _##name##_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)	\
{										\
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_##name##_descriptor);	\
	v->data.i32 = data->i32;						\
}										\
										\
static void _##name##_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)	\
{										\
	c_type *ptr;								\
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_##name##_descriptor);	\
	ptr = data->ptr;							\
	if (!ptr) return;							\
	*ptr = v->data.i32;							\
}										\
										\
static char * _##name##_string_to(const Egueb_Dom_Value *v)			\
{										\
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_##name##_descriptor,	\
			NULL);							\
	return name##_string_to((c_type )v->data.i32);				\
}										\
										\
static Eina_Bool _##name##_string_from(Egueb_Dom_Value *v, const char *str)	\
{										\
	EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor == &_##name##_descriptor,	\
			EINA_FALSE);						\
	return name##_string_from((c_type *)&v->data.i32, str);			\
}										\
										\
static void _##name##_interpolate(Egueb_Dom_Value *v,				\
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,		\
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)		\
{										\
	egueb_dom_value_enum_interpolate(v, a, b, m, add, acc, mul);		\
}										\
										\
static Egueb_Dom_Value_Descriptor _##name##_descriptor = {			\
	/* .data_from 		= */ _##name##_data_from,			\
	/* .data_from_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_INT32,		\
	/* .data_to 		= */ _##name##_data_to,				\
	/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,		\
	/* .init 		= */ NULL,					\
	/* .free 		= */ NULL,					\
	/* .copy 		= */ NULL,					\
	/* .string_to 		= */ _##name##_string_to,			\
	/* .string_from 	= */ _##name##_string_from,			\
	/* .interpolate 	= */ _##name##_interpolate,			\
}

/* Boilerplate code for a primitive (struct)
 * Where name is the name used to prefix every function and c_type is the c_type
 * of the struct.
 *
 * The implementation should provide:
 * name##_reset
 * name##_copy
 * name##_string_to
 * name##_string_from
 */
#define EGUEB_DOM_VALUE_PRIMITIVE_BOILERPLATE(name, c_type)			\
static Egueb_Dom_Value_Descriptor _##name##_descriptor;				\
										\
static void _##name##_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)	\
{										\
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_##name##_descriptor);	\
	egueb_dom_value_primitive_data_from(v, data);				\
}										\
										\
static void _##name##_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)	\
{										\
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_##name##_descriptor);	\
	egueb_dom_value_primitive_data_to(v, data);				\
}										\
										\
static void _##name##_free(Egueb_Dom_Value *v)					\
{										\
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_##name##_descriptor);	\
	if (v->owned)								\
	{									\
		name##_reset(v->data.ptr);					\
		free(v->data.ptr);						\
		v->data.ptr = NULL;						\
	}									\
}										\
										\
static void _##name##_copy(const Egueb_Dom_Value *v, Egueb_Dom_Value *copy,	\
		Eina_Bool content)						\
{										\
	if (!v->data.ptr)							\
		return;								\
										\
	if (!copy->data.ptr)							\
	{									\
		copy->data.ptr = calloc(1, sizeof(c_type));			\
		copy->owned = EINA_TRUE;					\
	}									\
	name##_copy(v->data.ptr, copy->data.ptr, content);			\
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
		v->data.ptr = calloc(1, sizeof(c_type));			\
		v->owned = EINA_TRUE;						\
	}									\
	return name##_string_from(v->data.ptr, str);				\
}										\
										\
static void _##name##_interpolate(Egueb_Dom_Value *v, Egueb_Dom_Value *a,	\
		Egueb_Dom_Value *b, double m, Egueb_Dom_Value *add,		\
		Egueb_Dom_Value *acc, int mul)					\
{										\
	void *p_add = NULL;							\
	void *p_acc = NULL;							\
										\
	EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == &_##name##_descriptor);	\
	EINA_SAFETY_ON_FALSE_RETURN(a->descriptor == &_##name##_descriptor);	\
	EINA_SAFETY_ON_FALSE_RETURN(b->descriptor == &_##name##_descriptor);	\
										\
	if (add)								\
	{									\
		EINA_SAFETY_ON_FALSE_RETURN(add->descriptor ==			\
				&_##name##_descriptor);				\
		p_add = add->data.ptr;						\
	}									\
										\
	if (acc)								\
	{									\
		EINA_SAFETY_ON_FALSE_RETURN(acc->descriptor ==			\
				&_##name##_descriptor);				\
		p_acc = acc->data.ptr;						\
	}									\
	name##_interpolate(v->data.ptr, a->data.ptr, b->data.ptr, m, p_add,	\
			p_acc, mul);						\
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

/* Boilerplate code for a simple primitive (struct)
 * Where name is the name used to prefix every function and c_type is the c_type
 * of the struct.
 *
 * The implementation should provide:
 * name##_string_to
 * name##_string_from
 */
#define EGUEB_DOM_VALUE_PRIMITIVE_SIMPLE_BOILERPLATE(name, c_type)		\
static inline void name##_reset(c_type *t)					\
{										\
	/* nothing here */							\
}										\
										\
static inline void name##_copy(const c_type *s, c_type *d,			\
		Eina_Bool full EINA_UNUSED)					\
{										\
	/* simple copy */							\
	*d = *s;								\
}										\
EGUEB_DOM_VALUE_PRIMITIVE_BOILERPLATE(name, c_type)

/* TODO boilerplate code for an object */



#endif
