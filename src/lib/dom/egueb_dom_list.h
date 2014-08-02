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
#ifndef _EGUEB_DOM_LIST_H_
#define _EGUEB_DOM_LIST_H_

#include "egueb_dom_value.h"

typedef struct _Egueb_Dom_List Egueb_Dom_List;
typedef void (*Egueb_Dom_List_Foreach)(void *data, void *user_data);

EAPI Egueb_Dom_List * egueb_dom_list_new(const Egueb_Dom_Value_Descriptor *d);
EAPI Egueb_Dom_List * egueb_dom_list_copy(Egueb_Dom_List *thiz);
EAPI Egueb_Dom_List * egueb_dom_list_ref(Egueb_Dom_List *thiz);
EAPI void egueb_dom_list_unref(Egueb_Dom_List *thiz);

EAPI int egueb_dom_list_length(Egueb_Dom_List *thiz);

EAPI void * egueb_dom_list_item_get(Egueb_Dom_List *thiz, int idx);
EAPI void egueb_dom_list_clear(Egueb_Dom_List *thiz);
EAPI void egueb_dom_list_item_remove(Egueb_Dom_List *thiz, int idx);
EAPI void egueb_dom_list_item_append(Egueb_Dom_List *thiz, void *data);

EAPI void egueb_dom_list_foreach(Egueb_Dom_List *thiz, Egueb_Dom_List_Foreach cb, void *user_data);
EAPI Eina_Iterator * egueb_dom_list_iterator_new(Egueb_Dom_List *thiz);
EAPI void egueb_dom_list_interpolate(Egueb_Dom_List *v,
		Egueb_Dom_List *a, Egueb_Dom_List *b, double m,
		Egueb_Dom_List *add, Egueb_Dom_List *acc, int mul);

#endif
