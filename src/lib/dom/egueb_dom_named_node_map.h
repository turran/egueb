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

#ifndef _EGUEB_NAMED_NODE_MAP
#define _EGUEB_NAMED_NODE_MAP

typedef struct _Egueb_Dom_Named_Node_Map Egueb_Dom_Named_Node_Map;

EAPI Egueb_Dom_Node * egueb_dom_named_node_map_get(
		Egueb_Dom_Named_Node_Map *thiz, Egueb_Dom_String *name);
EAPI Eina_Error egueb_dom_named_node_map_remove(Egueb_Dom_Named_Node_Map *thiz,
		Egueb_Dom_String *name);
EAPI Eina_Error egueb_dom_named_node_map_set(Egueb_Dom_Named_Node_Map *thiz,
		Egueb_Dom_Node *node);
EAPI Egueb_Dom_Node * egueb_dom_named_node_at(Egueb_Dom_Named_Node_Map *thiz,
		int idx);
EAPI int egueb_dom_named_node_length(Egueb_Dom_Named_Node_Map *thiz);

#endif
