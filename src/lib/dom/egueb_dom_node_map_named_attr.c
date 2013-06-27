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

#include "egueb_dom_main.h"
#include "egueb_dom_node.h"
#include "egueb_dom_element.h"

#include "egueb_dom_element_private.h"
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_node_map_named_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_NODE_MAP_NAMED_ATTR_CLASS_GET(o) EGUEB_DOM_NODE_MAP_NAMED_ATTR_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_NODE_MAP_NAMED_ATTR_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Node_Map_Named_Attr_Class, EGUEB_DOM_NODE_MAP_NAMED_ATTR_DESCRIPTOR)
#define EGUEB_DOM_NODE_MAP_NAMED_ATTR(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Node_Map_Named_Attr, EGUEB_DOM_NODE_MAP_NAMED_ATTR_DESCRIPTOR)

typedef struct _Egueb_Dom_Node_Map_Named_Attr
{
	Egueb_Dom_Node_Map_Named base;
	Egueb_Dom_Node *own;
} Egueb_Dom_Node_Map_Named_Attr;

typedef struct _Egueb_Dom_Node_Map_Named_Attr_Class
{
	Egueb_Dom_Node_Map_Named_Class base;
} Egueb_Dom_Node_Map_Named_Attr_Class;
/*----------------------------------------------------------------------------*
 *                           Map named node interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Error _egueb_dom_node_map_named_attr_get(
		Egueb_Dom_Node_Map_Named *n, Egueb_Dom_String *name,
		Egueb_Dom_Node **node)
{
	Egueb_Dom_Node_Map_Named_Attr *thiz;
	Eina_Error ret;

	thiz = EGUEB_DOM_NODE_MAP_NAMED_ATTR(n);
	ret = egueb_dom_element_property_fetch(thiz->own, name, node);
	return ret;
}

static Eina_Error _egueb_dom_node_map_named_attr_remove(
		Egueb_Dom_Node_Map_Named *n,
		Egueb_Dom_String *name, Egueb_Dom_Node *node)
{
	return EINA_ERROR_NONE;
}

static Eina_Error _egueb_dom_node_map_named_attr_set(Egueb_Dom_Node_Map_Named *n,
		Egueb_Dom_Node *node)
{
	return EINA_ERROR_NONE;
}

static Eina_Error _egueb_dom_node_map_named_attr_at(Egueb_Dom_Node_Map_Named *n,
		int idx, Egueb_Dom_Node **node)
{
	Egueb_Dom_Node_Map_Named_Attr *thiz;
	Egueb_Dom_Element *e;
	Egueb_Dom_Element_Class *klass;
	Egueb_Dom_Node *attr;
	int count;

	thiz = EGUEB_DOM_NODE_MAP_NAMED_ATTR(n);
	klass = EGUEB_DOM_ELEMENT_CLASS_GET(thiz->own);
	count = eina_extra_ordered_hash_count (klass->properties);
	if (idx > count)
	{
		Egueb_Dom_Element *e;

		e = EGUEB_DOM_ELEMENT(thiz->own);
		idx -= count;
		attr = eina_extra_ordered_hash_nth_get(e->attributes, idx);
	}
	else
	{
		Egueb_Dom_Attr_Fetch fetch;
		fetch = eina_extra_ordered_hash_nth_get(klass->properties, idx);
		if (!fetch)
		{
			*node = NULL;
			return EGUEB_DOM_ERROR_NOT_FOUND;
		}
		fetch(thiz->own, &attr);
	}
	if (attr)
	{
		*node = egueb_dom_node_ref(attr);
	}
	return EINA_ERROR_NONE;
}

static int _egueb_dom_node_map_named_attr_length(Egueb_Dom_Node_Map_Named *n)
{
	Egueb_Dom_Node_Map_Named_Attr *thiz;
	Egueb_Dom_Element *e;
	Egueb_Dom_Element_Class *klass;
	int ret = 0;

	thiz = EGUEB_DOM_NODE_MAP_NAMED_ATTR(n);
	/* get the number of properties from the class and from the element */
	klass = EGUEB_DOM_ELEMENT_CLASS_GET(thiz->own);
	ret += eina_extra_ordered_hash_count (klass->properties);
	e = EGUEB_DOM_ELEMENT(thiz->own);
	ret += eina_extra_ordered_hash_count (e->attributes);

	return ret;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_NODE_MAP_NAMED_DESCRIPTOR,
		Egueb_Dom_Node_Map_Named_Attr,
		Egueb_Dom_Node_Map_Named_Attr_Class,
		egueb_dom_node_map_named_attr);

static void _egueb_dom_node_map_named_attr_class_init(void *k)
{
	Egueb_Dom_Node_Map_Named_Class *klass;

	klass = EGUEB_DOM_NODE_MAP_NAMED_CLASS(k);
	klass->get = _egueb_dom_node_map_named_attr_get;
	klass->set = _egueb_dom_node_map_named_attr_set;
	klass->at = _egueb_dom_node_map_named_attr_at;
	klass->length = _egueb_dom_node_map_named_attr_length;
}

static void _egueb_dom_node_map_named_attr_class_deinit(void *k)
{
}

static void _egueb_dom_node_map_named_attr_instance_init(void *o)
{
}

static void _egueb_dom_node_map_named_attr_instance_deinit(void *o)
{
	Egueb_Dom_Node_Map_Named_Attr *thiz;

	thiz = EGUEB_DOM_NODE_MAP_NAMED_ATTR(o);
	egueb_dom_node_unref(thiz->own);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Node_Map_Named * egueb_dom_node_map_named_attr_new(
		Egueb_Dom_Node *own)
{
	Egueb_Dom_Node_Map_Named_Attr *thiz;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_node_map_named_attr);
	thiz->own = egueb_dom_node_ref(own);
	return EGUEB_DOM_NODE_MAP_NAMED(thiz);
}
