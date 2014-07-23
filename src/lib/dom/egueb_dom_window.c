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
#include "egueb_dom_main.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_node.h"

#include "egueb_dom_window.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_Window
{
	const Egueb_Dom_Window_Descriptor *desc;
	Egueb_Dom_Node *doc;
	int ref;
	void *data;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Window * egueb_dom_window_new(
		const Egueb_Dom_Window_Descriptor *desc,
		Egueb_Dom_Node *doc)
{
	Egueb_Dom_Window *thiz;

	if (!desc) return NULL;

	thiz = calloc(1, sizeof(Egueb_Dom_Window));
	thiz->desc = desc;
	egueb_dom_node_weak_ref_add(doc, &thiz->doc);
	thiz->data = thiz->desc->create();
	thiz->ref = 1;

	return thiz;
}

EAPI Egueb_Dom_Window * egueb_dom_window_ref(Egueb_Dom_Window *thiz)
{
	if (!thiz) return thiz;
	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_window_unref(Egueb_Dom_Window *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
		egueb_dom_node_weak_ref_remove(thiz->doc, &thiz->doc);
		free(thiz);
	} 
}

EAPI void * egueb_dom_window_data_get(Egueb_Dom_Window *thiz)
{
	if (!thiz) return NULL;
	return thiz->data;
}

EAPI Egueb_Dom_Node * egueb_dom_window_document_get(Egueb_Dom_Window *thiz)
{
	if (!thiz) return NULL;
	return egueb_dom_node_ref(thiz->doc);
}

EAPI void * egueb_dom_window_timeout_set(Egueb_Dom_Window *thiz,
		Egueb_Dom_Window_Descriptor_Timeout_Cb cb,
		int64_t delay, void *user_data)
{
	if (!thiz) return NULL;
	if (thiz->desc->timeout_set)
		return thiz->desc->timeout_set(thiz->data, cb, delay, user_data);
	return NULL;
}

EAPI void egueb_dom_window_timeout_clear(Egueb_Dom_Window *thiz,
		void *timeout)
{
	if (!thiz) return;
	if (thiz->desc->timeout_clear)
		thiz->desc->timeout_clear(thiz->data, timeout);
}
