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

typedef struct _Egueb_Dom_Node Egueb_Dom_Node;

typedef enum _Egueb_Dom_Node_Type
{
	EGUEB_DOM_NODE_TYPE_ELEMENT_NODE = 1,
	EGUEB_DOM_NODE_TYPE_ATTRIBUTE_NODE = 2,
	EGUEB_DOM_NODE_TYPE_TEXT_NODE = 3,
	EGUEB_DOM_NODE_TYPE_CDATA_SECTION_NODE = 4,
	EGUEB_DOM_NODE_TYPE_ENTITY_REFERENCE_NODE = 5,
	EGUEB_DOM_NODE_TYPE_ENTITY_NODE = 6,
	EGUEB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION_NODE = 7,
	EGUEB_DOM_NODE_TYPE_COMMENT_NODE = 8,
	EGUEB_DOM_NODE_TYPE_DOCUMENT_NODE = 9,
	EGUEB_DOM_NODE_TYPE_DOCUMENT_TYPE_NODE = 10,
	EGUEB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT_NODE = 11,
	EGUEB_DOM_NODE_TYPE_NOTATION_NODE = 12,
} Egueb_Dom_Node_Type;

EAPI void egueb_dom_node_unref(Egueb_Dom_Node *thiz);
EAPI Egueb_Dom_Node * egueb_dom_node_ref(Egueb_Dom_Node *thiz);
EAPI Eina_Error egueb_dom_node_name_get(Egueb_Dom_Node *thiz, Egueb_Dom_String **name);
EAPI Eina_Error egueb_dom_node_value_get(Egueb_Dom_Node *thiz, Egueb_Dom_String **value);
EAPI Eina_Error egueb_dom_node_type_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node_Type *type);
EAPI Eina_Error egueb_dom_node_parent_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **parent);
EAPI Eina_Error egueb_dom_node_children_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node_List *children);
EAPI Eina_Error egueb_dom_node_child_first_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **last);
EAPI Eina_Error egueb_dom_node_child_last_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **last);
EAPI Eina_Error egueb_dom_node_sibling_previous_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **sibling);
EAPI Eina_Error egueb_dom_node_sibling_next_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **sibling);
EAPI Eina_Error egueb_dom_node_attributes_get(Egueb_Dom_Node *thiz, Egueb_Dom_Named_Node_Map *map);

#endif
