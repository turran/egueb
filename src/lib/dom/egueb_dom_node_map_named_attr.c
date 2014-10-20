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

typedef struct _Egueb_Dom_Node_Map_Named_Attr_Nth_Data
{
	int i;
	int nth;
	Egueb_Dom_Node *ret;
} Egueb_Dom_Node_Map_Named_Attr_Nth_Data;

typedef struct _Egueb_Dom_Node_Map_Named_Attr_Count_Data
{
	int total;
} Egueb_Dom_Node_Map_Named_Attr_Count_Data;

static Eina_Bool _eina_hash_nth_get_cb(const Eina_Hash *hash,
		const void *key, void *data, void *fdata)
{
	Egueb_Dom_Node_Map_Named_Attr_Nth_Data *nth_data = fdata;

	if (nth_data->i == nth_data->nth)
	{
		nth_data->ret = data;
		return EINA_FALSE;
	}
	else
	{
		nth_data->i++;
		return EINA_TRUE;
	}
}

static Eina_Bool _eina_hash_count_cb(const Eina_Hash *hash,
		const void *key, void *data, void *fdata)
{

	Egueb_Dom_Node_Map_Named_Attr_Count_Data *count_data = fdata;
	count_data->total++;
	return EINA_TRUE;
}

static Egueb_Dom_Node * _eina_hash_nth_get(Eina_Hash *hash, int nth)
{
	Egueb_Dom_Node_Map_Named_Attr_Nth_Data data;

	data.i = 0;
	data.nth = nth;
	data.ret = NULL;

	eina_hash_foreach(hash, _eina_hash_nth_get_cb, &data);
	return egueb_dom_node_ref(data.ret);
}

static int _eina_hash_count(Eina_Hash *hash)
{
	Egueb_Dom_Node_Map_Named_Attr_Count_Data data;

	data.total = 0;
	eina_hash_foreach(hash, _eina_hash_count_cb, &data);
	return data.total;
}

/*----------------------------------------------------------------------------*
 *                           Map named node interface                         *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Node * _egueb_dom_node_map_named_attr_get(
		Egueb_Dom_Node_Map_Named *n, Egueb_Dom_String *name,
		Eina_Error *err)
{
	Egueb_Dom_Node_Map_Named_Attr *thiz;
	Egueb_Dom_Node *ret;

	thiz = EGUEB_DOM_NODE_MAP_NAMED_ATTR(n);
	ret = egueb_dom_element_attribute_node_get(thiz->own, name);
	if (!ret)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
	}
	return ret;
}

static Eina_Bool _egueb_dom_node_map_named_attr_remove(
		Egueb_Dom_Node_Map_Named *n,
		Egueb_Dom_String *name, Egueb_Dom_Node *node,
		Eina_Error *err)
{
	if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
	return EINA_FALSE;
}

static Eina_Bool _egueb_dom_node_map_named_attr_set(Egueb_Dom_Node_Map_Named *n,
		Egueb_Dom_Node *node, Eina_Error *err)
{
	if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
	return EINA_FALSE;
}

static Egueb_Dom_Node * _egueb_dom_node_map_named_attr_at(Egueb_Dom_Node_Map_Named *n,
		int idx)
{
	Egueb_Dom_Node_Map_Named_Attr *thiz;
	Egueb_Dom_Element *e;

	thiz = EGUEB_DOM_NODE_MAP_NAMED_ATTR(n);
	e = EGUEB_DOM_ELEMENT(thiz->own);

	return _eina_hash_nth_get(e->attributes, idx);
}

static int _egueb_dom_node_map_named_attr_length(Egueb_Dom_Node_Map_Named *n)
{
	Egueb_Dom_Node_Map_Named_Attr *thiz;
	Egueb_Dom_Element *e;

	thiz = EGUEB_DOM_NODE_MAP_NAMED_ATTR(n);
	e = EGUEB_DOM_ELEMENT(thiz->own);

	return _eina_hash_count(e->attributes);
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
