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

#include "egueb_dom_media_notifier.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_Media_Notifier
{
	const Egueb_Dom_Media_Notifier_Descriptor *desc;
	Egueb_Dom_Node *n;
	void *data;
	int ref;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Media_Notifier * egueb_dom_media_notifier_new(
		const Egueb_Dom_Media_Notifier_Descriptor *desc,
		void *data)
{
	Egueb_Dom_Media_Notifier *thiz;

	if (!desc) return NULL;

	thiz = calloc(1, sizeof(Egueb_Dom_Media_Notifier));
	thiz->desc = desc;
	thiz->data = data;
	thiz->ref = 1;

	return thiz;
}

EAPI Egueb_Dom_Media_Notifier * egueb_dom_media_notifier_ref(Egueb_Dom_Media_Notifier *thiz)
{
	if (!thiz) return thiz;
	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_media_notifier_unref(Egueb_Dom_Media_Notifier *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
		if (thiz->desc->destroy)
			thiz->desc->destroy(thiz->data);
		free(thiz);
	} 
}

EAPI void * egueb_dom_media_notifier_data_get(Egueb_Dom_Media_Notifier *thiz)
{
	if (!thiz) return NULL;
	return thiz->data;
}

#if 0
EAPI void egueb_dom_media_notifier_error_set(Egueb_Dom_Media_Notifier *thiz,
		Egueb_Dom_Media_Error error)
{

}

EAPI void egueb_dom_media_notifier_state_set(Egueb_Dom_Media_Notifier *thiz,
		Egueb_Dom_Media_State state)
{

}
#endif
