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

#endif
