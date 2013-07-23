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
#include "egueb_dom_list.h"
#include "egueb_dom_list_private.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_List
{
	Egueb_Dom_Node *owner;
	Eina_List *list;
	const Egueb_Dom_Value_Descriptor *content_descriptor;
	int ref;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_list_owner_set(Egueb_Dom_List *thiz, Egueb_Dom_Node *n)
{
	thiz->owner = n;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_List * egueb_dom_list_new(const Egueb_Dom_Value_Descriptor *d)
{
	Egueb_Dom_List *thiz;

	thiz = calloc(1, sizeof(Egueb_Dom_List));
	thiz->content_descriptor = d;
	thiz->ref = 1;
	return thiz;
}

EAPI Egueb_Dom_List * egueb_dom_list_ref(Egueb_Dom_List *thiz)
{
	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_list_unref(Egueb_Dom_List *thiz)
{
	thiz->ref--;
	if (!thiz->ref)
		free(thiz);
}

EAPI int egueb_dom_list_length(Egueb_Dom_List *thiz)
{
	return eina_list_count(thiz->list);
}

EAPI void * egueb_dom_list_item_get(Egueb_Dom_List *thiz, int idx)
{
	return eina_list_nth(thiz->list, idx);
}

EAPI void egueb_dom_list_clear(Egueb_Dom_List *thiz)
{
	int i;
	int length;

	length = egueb_dom_list_length(thiz);
	for (i = 0; i < length; i++)
	{
		egueb_dom_list_item_remove(thiz, i);
	}
}

EAPI void egueb_dom_list_item_remove(Egueb_Dom_List *thiz, int idx)
{
	Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
	Eina_List *l;

	/* TODO trigger the mutation event */
	l = eina_list_nth_list(thiz->list, idx);
	if (!l) return;

#if 0
	egueb_dom_value_init(&v, thiz->content_descriptor);
	vd.ptr = l->data;
	egueb_dom_value_data_from(&v, &vd);
	egueb_dom_value_reset(&v);
#endif
	free(l->data);
	thiz->list = eina_list_remove_list(thiz->list, l);
}

EAPI void egueb_dom_list_item_append(Egueb_Dom_List *thiz, void *data)
{
	/* TODO trigger the mutation event */
	thiz->list = eina_list_append(thiz->list, data);
}

EAPI void egueb_dom_list_foreach(Egueb_Dom_List *thiz, Egueb_Dom_List_Foreach cb, void *user_data)
{
	Eina_List *l, *l_next;
	void *data;

	if (!thiz) return;
	EINA_LIST_FOREACH_SAFE(thiz->list, l, l_next, data)
		cb(data, user_data);
}

EAPI Egueb_Dom_List * egueb_dom_list_copy(Egueb_Dom_List *thiz)
{
	Egueb_Dom_List *ret;
	Eina_List *l;
	void *data;

	if (!thiz) return NULL;
	ret = egueb_dom_list_new(thiz->content_descriptor);
	EINA_LIST_FOREACH(thiz->list, l, data)
	{
		Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
		Egueb_Dom_Value nv = EGUEB_DOM_VALUE_INIT;
		Egueb_Dom_Value_Data vdata;

		egueb_dom_value_init(&v, thiz->content_descriptor);
		egueb_dom_value_init(&nv, thiz->content_descriptor);
		vdata.ptr = data;
		egueb_dom_value_data_from(&v, &vdata);
		egueb_dom_value_copy(&v, &nv, EINA_TRUE);
		ret->list = eina_list_append(ret->list, nv.data.ptr);
	}
	return ret;
}
