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

#ifndef _EGUEB_DOM_NODE_LIST_H_
#define _EGUEB_DOM_NODE_LIST_H_

/**
 * @file
 * @ender_group_proto{Egueb_Dom_Node}
 * @ender_group{Egueb_Dom_Node_List}
 */

/* forward declarations */
typedef struct _Egueb_Dom_Node Egueb_Dom_Node;

/**
 * @defgroup Egueb_Dom_Node_List Node list
 * @ingroup Egueb_Dom_Node
 * @{
 */

typedef struct _Egueb_Dom_Node_List Egueb_Dom_Node_List;

EAPI int egueb_dom_node_list_length_get(Egueb_Dom_Node_List *thiz);
EAPI Egueb_Dom_Node * egueb_dom_node_list_item(Egueb_Dom_Node_List *thiz, int index);
EAPI Egueb_Dom_Node_List * egueb_dom_node_list_ref(Egueb_Dom_Node_List *thiz);
EAPI void egueb_dom_node_list_unref(Egueb_Dom_Node_List *thiz);

/**
 * @}
 */

#endif
