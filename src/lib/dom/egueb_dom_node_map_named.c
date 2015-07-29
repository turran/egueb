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
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_node_map_named_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(ENESIM_OBJECT_DESCRIPTOR, Egueb_Dom_Node_Map_Named,
		Egueb_Dom_Node_Map_Named_Class, egueb_dom_node_map_named);

static void _egueb_dom_node_map_named_class_init(void *k)
{
}

static void _egueb_dom_node_map_named_instance_init(void *o)
{
	Egueb_Dom_Node_Map_Named *thiz;

	thiz = EGUEB_DOM_NODE_MAP_NAMED(o);
	thiz->ref = 1;
}

static void _egueb_dom_node_map_named_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/*
 * Node getNamedItem(in DOMString name);
 * Node getNamedItemNS(in DOMString namespaceURI, in DOMString localName)
 * raises(DOMException);
 */
EAPI Egueb_Dom_Node * egueb_dom_node_map_named_get(
		Egueb_Dom_Node_Map_Named *thiz, Egueb_Dom_String *name,
		Eina_Error *err)
{
	Egueb_Dom_Node_Map_Named_Class *klass;

	klass = EGUEB_DOM_NODE_MAP_NAMED_CLASS_GET(thiz);
	if (!klass->get)
	{
		if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}
	return klass->get(thiz, name, err);
}

/*
 * Node removeNamedItem(in DOMString name) raises(DOMException);
 * Node removeNamedItemNS(in DOMString namespaceURI, in DOMString localName)
 * raises(DOMException);
 */
EAPI Eina_Bool egueb_dom_node_map_named_remove(Egueb_Dom_Node_Map_Named *thiz,
		Egueb_Dom_String *name, Egueb_Dom_Node *n, Eina_Error *err)
{
	Egueb_Dom_Node_Map_Named_Class *klass;

	klass = EGUEB_DOM_NODE_MAP_NAMED_CLASS_GET(thiz);
	if (!klass->remove)
	{
		if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}
	return klass->remove(thiz, name, n, err);
}

/*
 * Node setNamedItem(in Node arg) raises(DOMException);
 * Node setNamedItemNS(in Node arg) raises(DOMException);
 */
EAPI Eina_Bool egueb_dom_node_map_named_set(Egueb_Dom_Node_Map_Named *thiz,
		Egueb_Dom_Node *n, Eina_Error *err)
{
	Egueb_Dom_Node_Map_Named_Class *klass;

	klass = EGUEB_DOM_NODE_MAP_NAMED_CLASS_GET(thiz);
	if (!klass->set)
	{
		if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}
	return klass->set(thiz, n, err);
}

/* Node item(in unsigned long index); */
EAPI Egueb_Dom_Node * egueb_dom_node_map_named_at(Egueb_Dom_Node_Map_Named *thiz,
		int idx)
{
	Egueb_Dom_Node_Map_Named_Class *klass;

	klass = EGUEB_DOM_NODE_MAP_NAMED_CLASS_GET(thiz);
	if (!klass->at) return NULL;
	return klass->at(thiz, idx);
}

/* readonly attribute unsigned long length; */
EAPI int egueb_dom_node_map_named_length(Egueb_Dom_Node_Map_Named *thiz)
{
	Egueb_Dom_Node_Map_Named_Class *klass;

	klass = EGUEB_DOM_NODE_MAP_NAMED_CLASS_GET(thiz);
	if (!klass->length) return 0;
	return klass->length(thiz);
}

EAPI Egueb_Dom_Node_Map_Named * egueb_dom_node_map_named_ref(Egueb_Dom_Node_Map_Named *thiz)
{
	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_node_map_named_unref(Egueb_Dom_Node_Map_Named *thiz)
{
	thiz->ref--;
	if (!thiz->ref)
	{
		enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
	}
}
