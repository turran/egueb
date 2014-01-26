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

#ifndef _EGUEB_DOM_VALUE_PRIVATE_H_
#define _EGUEB_DOM_VALUE_PRIVATE_H_

typedef enum _Egueb_Dom_Value_Data_Type
{
	EGUEB_DOM_VALUE_DATA_TYPE_INT32,
	EGUEB_DOM_VALUE_DATA_TYPE_INT64,
	EGUEB_DOM_VALUE_DATA_TYPE_DOUBLE,
	EGUEB_DOM_VALUE_DATA_TYPE_PTR,
} Egueb_Dom_Value_Data_Type;

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

void egueb_dom_value_data_from(Egueb_Dom_Value *thiz, Egueb_Dom_Value_Data *data);
void egueb_dom_value_data_to(Egueb_Dom_Value *thiz, Egueb_Dom_Value_Data *data);

#define EGUEB_DOM_VALUE_DATA_COLLECT(value, type, data, args) 		\
	{								\
		switch (type)						\
		{							\
			case EGUEB_DOM_VALUE_DATA_TYPE_INT32:		\
			data.i32 = va_arg(args, int32_t);		\
			break;						\
			case EGUEB_DOM_VALUE_DATA_TYPE_INT64:		\
			data.i64 = va_arg(args, int64_t);		\
			break;						\
			case EGUEB_DOM_VALUE_DATA_TYPE_DOUBLE:		\
			data.d = va_arg(args, double);			\
			break;						\
			case EGUEB_DOM_VALUE_DATA_TYPE_PTR:		\
			data.ptr = va_arg(args, void *);		\
			break;						\
			default:					\
			break;						\
		}							\
	}

#define EGUEB_DOM_VALUE_DATA_TO(value, descriptor, args) 			\
	{									\
		Egueb_Dom_Value_Data _data;					\
		EGUEB_DOM_VALUE_DATA_COLLECT(value, descriptor->data_to_type,	\
				_data, args);					\
		egueb_dom_value_data_to(value, &_data);				\
	}

#define EGUEB_DOM_VALUE_DATA_FROM(value, descriptor, args) 			\
	{									\
		Egueb_Dom_Value_Data _data;					\
		EGUEB_DOM_VALUE_DATA_COLLECT(value, descriptor->data_from_type,	\
				_data, args);					\
		egueb_dom_value_init(value, descriptor);			\
		egueb_dom_value_data_from(value, &_data);			\
	}

#define EGUEB_DOM_VALUE_BASIC_BOLIERPLATE(name, type)				\
	static Egueb_Dom_Value_Descriptor name##_descriptor;			\
										\
	static void name##_data_from(Egueb_Dom_Value *v,			\
			Egueb_Dom_Value_Data *data)				\
	{									\
		EINA_SAFETY_ON_FALSE_RETURN(v->descriptor ==			\
				&##name##_descriptor);				\
		egueb_dom_value_basic_data_from(v, type, data);			\
	}									\
										\
	static void name##_data_to(Egueb_Dom_Value *v, 				\
			Egueb_Dom_Value_Data *data)				\
	{									\
		EINA_SAFETY_ON_FALSE_RETURN(v->descriptor == 			\
				&##name##_descriptor);				\
		egueb_dom_value_basic_data_to(v, type, data);			\
	}									\
										\
	static char * name##_string_to(const Egueb_Dom_Value *v)		\
	{									\
		EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor ==		\
				&##name##_descriptor, NULL);			\
		return egueb_dom_value_basic_string_to(v, type);		\
	}									\
										\
	static Eina_Bool name##_string_from(Egueb_Dom_Value *v, const char *str)\
	{									\
		EINA_SAFETY_ON_FALSE_RETURN_VAL(v->descriptor ==		\
				&##name##_descriptor, EINA_FALSE);		\
		return EINA_TRUE;						\
	}									\
										\
	static Egueb_Dom_Value_Descriptor name##_descriptor = {			\
		/* .data_from 		= */ name##_data_from,			\
		/* .data_from_type 	= */ type,				\
		/* .data_to 		= */ name##_data_to,			\
		/* .data_to_type 	= */ EGUEB_DOM_VALUE_DATA_TYPE_PTR,	\
		/* .init 		= */ NULL,				\
		/* .free 		= */ NULL,				\
		/* .copy 		= */ NULL,				\
		/* .string_to 		= */ name##_string_to,			\
		/* .string_from 	= */ name##_string_from,		\
		/* .interpolate 	= */ NULL,				\
	};

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
#define EGUEB_DOM_VALUE_PRIMITIVE_BOLIERPLATE(name, c_type)			\
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
#define EGUEB_DOM_VALUE_PRIMITIVE_SIMPLE_BOLIERPLATE(name, c_type)		\
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
EGUEB_DOM_VALUE_PRIMITIVE_BOLIERPLATE(name, c_type)

/* TODO boilerplate code for an object */


#endif
