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

#ifndef _EGUEB_DOM_NODE_MAP_NAMED_
#define _EGUEB_DOM_NODE_MAP_NAMED_

/**
 * @file
 * @ender_group_proto{Egueb_Dom_Node}
 * @ender_group{Egueb_Dom_Node_Map_Named}
 */

/**
 * @defgroup Egueb_Dom_Node_Map_Named Node named map
 * @brief Node named map
 *
 * @ingroup Egueb_Dom_Node
 * @{
 */

/**
 * The node named map handle
 * @ender_name{egueb.dom.node.map_named}
 */
typedef struct _Egueb_Dom_Node_Map_Named Egueb_Dom_Node_Map_Named;

EAPI Egueb_Dom_Node * egueb_dom_node_map_named_get(
		Egueb_Dom_Node_Map_Named *thiz, Egueb_Dom_String *name,
		Eina_Error *err);
EAPI Eina_Bool egueb_dom_node_map_named_remove(Egueb_Dom_Node_Map_Named *thiz,
		Egueb_Dom_String *name, Egueb_Dom_Node *n, Eina_Error *err);
EAPI Eina_Bool egueb_dom_node_map_named_set(Egueb_Dom_Node_Map_Named *thiz,
		Egueb_Dom_Node *n, Eina_Error *err);
EAPI Egueb_Dom_Node * egueb_dom_node_map_named_at(Egueb_Dom_Node_Map_Named *thiz,
		int idx);
EAPI int egueb_dom_node_map_named_length(Egueb_Dom_Node_Map_Named *thiz);

EAPI Egueb_Dom_Node_Map_Named * egueb_dom_node_map_named_ref(Egueb_Dom_Node_Map_Named *thiz);
EAPI void egueb_dom_node_map_named_unref(Egueb_Dom_Node_Map_Named *thiz);

/**
 * @}
 */

#endif
