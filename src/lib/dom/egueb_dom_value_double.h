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
#ifndef _EGUEB_DOM_VALUE_DOUBLE_H_
#define _EGUEB_DOM_VALUE_DOUBLE_H_

static inline void egueb_dom_value_double_set(Egueb_Dom_Value *v, double d)
{
	v->data.d = d;
}

static inline int egueb_dom_value_double_get(Egueb_Dom_Value *v)
{
	return v->data.d;
}

static inline void egueb_dom_value_double_data_from(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	v->data.d = data->d;
}

static inline void egueb_dom_value_double_data_to(Egueb_Dom_Value *v, Egueb_Dom_Value_Data *data)
{
	double *ptr = data->ptr;
	if (!ptr) return;
	*ptr = v->data.d;
}

EAPI const Egueb_Dom_Value_Descriptor *
		egueb_dom_value_double_descriptor_get(void);
EAPI void egueb_dom_value_double_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul);

#endif

