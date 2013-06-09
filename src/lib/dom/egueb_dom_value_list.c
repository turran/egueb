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
#include "egueb_dom_list.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_dom_value_list_data_from(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	v->data.ptr = data->ptr;
	v->owned = EINA_TRUE;
}

EAPI void egueb_dom_value_list_data_to(Egueb_Dom_Value *v,
		Egueb_Dom_Value_Data *data)
{
	Egueb_Dom_List **ptr = data->ptr;
	if (v->data.ptr)
		*ptr = egueb_dom_list_ref(v->data.ptr);
	else
		*ptr = NULL;
}

EAPI void egueb_dom_value_list_free(Egueb_Dom_Value *v)
{
	if (v->owned)
		egueb_dom_list_unref(v->data.ptr);
	v->data.ptr = NULL;
	v->owned = EINA_FALSE;
}

EAPI void egueb_dom_value_list_copy(const Egueb_Dom_Value *v,
		Egueb_Dom_Value *copy)
{
	Egueb_Dom_List *vl = v->data.ptr;
	Egueb_Dom_List *cl = v->data.ptr;

	if (copy->data.ptr && copy->owned)
	{
		egueb_dom_list_unref(copy->data.ptr);
		copy->data.ptr = NULL;
	}
	if (v->data.ptr)
	{
		copy->data.ptr = egueb_dom_list_ref(v->data.ptr);
		copy->owned = EINA_TRUE;
	}
}

EAPI void egueb_dom_list_interpolate(Egueb_Dom_Value *v,
		Egueb_Dom_Value *a, Egueb_Dom_Value *b, double m,
		Egueb_Dom_Value *add, Egueb_Dom_Value *acc, int mul)
{
	/* iterate over list a, b and v, and interpolate each value */
}
