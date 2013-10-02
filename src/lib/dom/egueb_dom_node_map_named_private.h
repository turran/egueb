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

#ifndef _EGUEB_DOM_NODE_MAP_NAMED_PRIVATE_
#define _EGUEB_DOM_NODE_MAP_NAMED_PRIVATE_

struct _Egueb_Dom_Node_Map_Named
{
	Enesim_Object_Instance base;
};

typedef Egueb_Dom_Node * (*Egueb_Dom_Node_Map_Named_Get)(
		Egueb_Dom_Node_Map_Named *thiz, Egueb_Dom_String *name,
		Eina_Error *err);
typedef Eina_Bool (*Egueb_Dom_Node_Map_Named_Remove)(Egueb_Dom_Node_Map_Named *thiz,
		Egueb_Dom_String *name, Egueb_Dom_Node *node, Eina_Error *err);
typedef Eina_Bool (*Egueb_Dom_Node_Map_Named_Set)(Egueb_Dom_Node_Map_Named *thiz,
		Egueb_Dom_Node *node, Eina_Error *err);
typedef Egueb_Dom_Node * (*Egueb_Dom_Node_Map_Named_At)(Egueb_Dom_Node_Map_Named *thiz,
		int idx);
typedef int (*Egueb_Dom_Node_Map_Named_Length)(Egueb_Dom_Node_Map_Named *thiz);

typedef struct _Egueb_Dom_Node_Map_Named_Class
{
	Enesim_Object_Class base;
	Egueb_Dom_Node_Map_Named_Get get;
	Egueb_Dom_Node_Map_Named_Remove remove;
	Egueb_Dom_Node_Map_Named_Set set;
	Egueb_Dom_Node_Map_Named_At at;
	Egueb_Dom_Node_Map_Named_Length length;
} Egueb_Dom_Node_Map_Named_Class;

#define EGUEB_DOM_NODE_MAP_NAMED_DESCRIPTOR egueb_dom_node_map_named_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_node_map_named_descriptor_get(void);

#define EGUEB_DOM_NODE_MAP_NAMED_CLASS_GET(o) EGUEB_DOM_NODE_MAP_NAMED_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_NODE_MAP_NAMED_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Node_Map_Named_Class, EGUEB_DOM_NODE_MAP_NAMED_DESCRIPTOR)
#define EGUEB_DOM_NODE_MAP_NAMED(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Node_Map_Named, EGUEB_DOM_NODE_MAP_NAMED_DESCRIPTOR)

/* implementations */
Egueb_Dom_Node_Map_Named * egueb_dom_node_map_named_attr_new(Egueb_Dom_Node *own);

#endif
