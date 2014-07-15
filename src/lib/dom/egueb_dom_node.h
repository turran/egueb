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

#ifndef _EGUEB_DOM_NODE_H_
#define _EGUEB_DOM_NODE_H_

#include "egueb_dom_string.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_event.h"

/**
 * @file
 * @ender_group_proto{Egueb_Dom_Event}
 * @ender_group_proto{Egueb_Dom_Element}
 * @ender_group_proto{Egueb_Dom_Document}
 * @ender_group_proto{Egueb_Dom_Attr}
 * @ender_group{Egueb_Dom_Node_Definitions}
 * @ender_group{Egueb_Dom_Node_Type}
 * @ender_group{Egueb_Dom_Node}
 */

/**
 * @defgroup Egueb_Dom_Node_Definitions Definitions
 * @ingroup Egueb_Dom_Node
 * @{
 */

typedef struct _Egueb_Dom_Node_Event_Listener Egueb_Dom_Node_Event_Listener;
typedef Eina_Bool (*Egueb_Dom_Node_Cb)(Egueb_Dom_Node *n, void *data);

/**
 * @}
 * @defgroup Egueb_Dom_Node_Type Type
 * @ingroup Egueb_Dom_Node
 * @{
 */

typedef enum _Egueb_Dom_Node_Type
{
	EGUEB_DOM_NODE_TYPE_ELEMENT = 1,
	EGUEB_DOM_NODE_TYPE_ATTRIBUTE = 2,
	EGUEB_DOM_NODE_TYPE_TEXT = 3,
	EGUEB_DOM_NODE_TYPE_CDATA_SECTION = 4,
	EGUEB_DOM_NODE_TYPE_ENTITY_REFERENCE = 5,
	EGUEB_DOM_NODE_TYPE_ENTITY = 6,
	EGUEB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION = 7,
	EGUEB_DOM_NODE_TYPE_COMMENT = 8,
	EGUEB_DOM_NODE_TYPE_DOCUMENT = 9,
	EGUEB_DOM_NODE_TYPE_DOCUMENT_TYPE = 10,
	EGUEB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT = 11,
	EGUEB_DOM_NODE_TYPE_NOTATION = 12,
} Egueb_Dom_Node_Type;

/**
 * @}
 * @defgroup Egueb_Dom_Node Node
 * @ingroup Egueb_Dom_Group
 * @{
 */

/**
 * The node handle
 */
typedef struct _Egueb_Dom_Node Egueb_Dom_Node;

EAPI Eina_Bool egueb_dom_node_is_destroying(Egueb_Dom_Node *thiz);
EAPI void egueb_dom_node_unref(Egueb_Dom_Node *thiz);
EAPI Egueb_Dom_Node * egueb_dom_node_ref(Egueb_Dom_Node *thiz);
EAPI int egueb_dom_node_ref_get(Egueb_Dom_Node *thiz);

EAPI void egueb_dom_node_weak_ref(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event_Listener l, void *data);
EAPI void egueb_dom_node_weak_unref(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event_Listener l, void *data);
EAPI void egueb_dom_node_weak_ref_add(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node **weak_location);
EAPI void egueb_dom_node_weak_ref_remove(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node **weak_location);

EAPI Egueb_Dom_String * egueb_dom_node_name_get(Egueb_Dom_Node *thiz);
EAPI Egueb_Dom_String * egueb_dom_node_value_get(Egueb_Dom_Node *thiz);
EAPI Egueb_Dom_Node_Type egueb_dom_node_type_get(Egueb_Dom_Node *thiz);
EAPI Egueb_Dom_Node * egueb_dom_node_owner_document_get(Egueb_Dom_Node *thiz);
EAPI Egueb_Dom_Node * egueb_dom_node_parent_get(Egueb_Dom_Node *thiz);

EAPI Egueb_Dom_Node_List * egueb_dom_node_child_nodes_get(Egueb_Dom_Node *thiz);
EAPI Eina_Bool egueb_dom_node_children_foreach(Egueb_Dom_Node *thiz, Egueb_Dom_Node_Cb cb, void *data);
EAPI Egueb_Dom_Node * egueb_dom_node_child_first_get(Egueb_Dom_Node *thiz);
EAPI Egueb_Dom_Node * egueb_dom_node_child_last_get(Egueb_Dom_Node *thiz);
EAPI Eina_Bool egueb_dom_node_child_remove(Egueb_Dom_Node *thiz, Egueb_Dom_Node *child, Eina_Error *err);
EAPI Eina_Bool egueb_dom_node_child_append(Egueb_Dom_Node *thiz, Egueb_Dom_Node *child, Eina_Error *err);
EAPI Eina_Bool egueb_dom_node_insert_before(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node *child, Egueb_Dom_Node *ref, Eina_Error *err);

EAPI Egueb_Dom_Node * egueb_dom_node_sibling_previous_get(Egueb_Dom_Node *thiz);
EAPI Egueb_Dom_Node * egueb_dom_node_sibling_next_get(Egueb_Dom_Node *thiz);
EAPI Egueb_Dom_Node_Map_Named * egueb_dom_node_attributes_get(Egueb_Dom_Node *thiz);

EAPI Egueb_Dom_Node * egueb_dom_node_clone(Egueb_Dom_Node *thiz, Eina_Bool live,
		Eina_Bool deep, Eina_Error *err);

EAPI Egueb_Dom_Node_Event_Listener * egueb_dom_node_event_listener_add(Egueb_Dom_Node *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data);
EAPI void egueb_dom_node_event_listener_remove(Egueb_Dom_Node *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data);
EAPI void egueb_dom_node_event_listener_free(Egueb_Dom_Node_Event_Listener *node_listener);
EAPI Eina_Bool egueb_dom_node_event_dispatch(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *event, Eina_Bool *notprevented, Eina_Error *err);
EAPI Eina_Bool egueb_dom_node_event_propagate(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *event);
EAPI void egueb_dom_node_event_monitor_add(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event_Listener listener,
		void *data);
EAPI void egueb_dom_node_event_monitor_remove(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event_Listener listener,
		void *data);

EAPI void egueb_dom_node_user_data_set(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *key, void *data);
EAPI void * egueb_dom_node_user_data_get(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *key);

EAPI Eina_Bool egueb_dom_node_is_supported(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *name, Egueb_Dom_String *version);
EAPI void * egueb_dom_node_feature_get(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *name, Egueb_Dom_String *version);

EAPI Ender_Item * egueb_dom_node_item_get(Egueb_Dom_Node *thiz);
/**
 * @}
 */

#endif
