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
#include "egueb_dom_node.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node_list_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_Node_List
{
	Eina_List *nodes;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Node_List * egueb_dom_node_list_new(Eina_List *nodes)
{
	Egueb_Dom_Node_List *thiz;

	if (!nodes) return NULL;

	thiz = calloc(1, sizeof(Egueb_Dom_Node_List));
	thiz->nodes = nodes;
	return thiz;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Get the number of nodes in the list
 * (readonly attribute unsigned long length)
 * @prop{length}
 * @param[in] thiz The list to get the number of nodes from
 * @return The number of nodes in the list
 */
EAPI int egueb_dom_node_list_length_get(Egueb_Dom_Node_List *thiz)
{
	if (!thiz) return 0;
	return eina_list_count(thiz->nodes);
}

EAPI Egueb_Dom_Node * egueb_dom_node_list_item(Egueb_Dom_Node_List *thiz, int index)
{
	if (!thiz) return NULL;
	return egueb_dom_node_ref(eina_list_nth(thiz->nodes, index));
}

EAPI void egueb_dom_node_list_free(Egueb_Dom_Node_List *thiz)
{
	Egueb_Dom_Node *n;

	if (!thiz) return;

	EINA_LIST_FREE(thiz->nodes, n)
		egueb_dom_node_unref(n);
	free(thiz);
}
