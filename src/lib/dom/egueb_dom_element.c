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
#include "egueb_dom_element.h"
#include "egueb_dom_document.h"
#include "egueb_dom_event_mutation.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_attr_string.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"

#include "egueb_dom_document_private.h"
#include "egueb_dom_element_private.h"
#include "egueb_dom_attr_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_attr_private.h"
#include "egueb_dom_value_private.h"
#include "egueb_dom_event_mutation_private.h"
#include "egueb_dom_event_process_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_dom_element_clone_destroyed_cb(Egueb_Dom_Event *e,
		void *data);
static void _egueb_dom_element_original_destroyed_cb(Egueb_Dom_Event *e,
		void *data);

/* whenever an element is being removed from a document, be sure to remove
 * the enqueue flag
 */
static void _egueb_dom_element_document_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Element *thiz = data;
	Egueb_Dom_Event_Phase phase;

	phase = egueb_dom_event_phase_get(e);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;
	thiz->enqueued = EINA_FALSE;
}

static Eina_Bool _egueb_dom_element_properties_clone_cb(const Eina_Hash *hash,
		const void *key, void *data, void *fdata)
{
	Egueb_Dom_Node *clone = fdata;
	Egueb_Dom_Node *attr = data;
	Egueb_Dom_Node *n;
	Egueb_Dom_String *name;

	/* check if the cloned element has such attr */
	name = egueb_dom_attr_name_get(attr);
	n = egueb_dom_element_attribute_fetch(clone, name);
	egueb_dom_string_unref(name);
	if (n)
	{
		egueb_dom_attr_copy(attr, n);
	}
	else
	{
		n = egueb_dom_node_clone(attr, EINA_FALSE, EINA_FALSE, NULL);
		egueb_dom_element_attribute_add(clone, n, NULL); 
	}

	return EINA_TRUE;
}

/* whenever a node is inserted into the original node, clone the child
 * and insert it on the cloned tree
 */
static void _egueb_dom_element_original_node_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *origin = NULL;
	Egueb_Dom_Node *origin_child = NULL;
	Egueb_Dom_Node *origin_tmp = NULL;
	Egueb_Dom_Node *cloned = data;
	Egueb_Dom_Node *cloned_child;
	Egueb_Dom_Node *parent = NULL;

	/* check if the parent is the same as the node we have registered
	 * the event
	 */
	origin = egueb_dom_event_target_current_get(e);
	parent = egueb_dom_event_mutation_related_get(e);
	if (origin != parent)
	{
		egueb_dom_node_unref(origin);
		egueb_dom_node_unref(parent);
		return;
	}
	egueb_dom_node_unref(parent);

	DBG("Inserting a node on a cloned element");
	origin_child = egueb_dom_event_target_get(e);
	cloned_child = egueb_dom_node_clone(origin_child, EINA_TRUE, EINA_TRUE, NULL);
	if (!cloned_child) goto err_clone;

	/* check if it does not have any next sibling (simple append) */
	origin_tmp = egueb_dom_node_sibling_next_get(origin_child);
	if (!origin_tmp)
	{
		egueb_dom_node_child_append(cloned, cloned_child, NULL);
	}
	else
	{
		Egueb_Dom_Node *cloned_rel = NULL;


		/* iterate in the original tree over the prev siblings until
		 * we find the root, and we iterate in the opposite direction
		 * so we find the node to insert before
		 * TODO we might need to find a faster version
		 */
		egueb_dom_node_unref(origin_tmp);
		origin_tmp = egueb_dom_node_sibling_previous_get(origin_child);
		cloned_rel = egueb_dom_node_child_first_get(cloned);
		while (origin_tmp)
		{
			Egueb_Dom_Node *tmp1 = NULL;
			Egueb_Dom_Node *tmp2 = NULL;

			tmp2 = egueb_dom_node_sibling_next_get(cloned_rel);
			egueb_dom_node_unref(cloned_rel);
			cloned_rel = tmp2;

			tmp1 = egueb_dom_node_sibling_previous_get(origin_tmp);
			egueb_dom_node_unref(origin_tmp);
			origin_tmp = tmp1;
		}
		/* we need to insert the new node before the found rel */
		egueb_dom_node_insert_before(cloned, cloned_child, cloned_rel, NULL);
		egueb_dom_node_unref(cloned_rel);
	}

err_clone:
	egueb_dom_node_unref(origin);
	egueb_dom_node_unref(origin_child);
}

/* whenever a node is removed from the original node, remove the cloned
 * child from the cloned tree
 */
static void _egueb_dom_element_original_node_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *origin = NULL;
	Egueb_Dom_Node *origin_child = NULL;
	Egueb_Dom_Node *cloned = data;
	Egueb_Dom_Node *cloned_child = NULL;
	Egueb_Dom_Node *parent = NULL;
	Egueb_Dom_Node *origin_tmp = NULL;

	/* check if the parent is the same as the node we have registered
	 * the event
	 */
	origin = egueb_dom_event_target_current_get(e);
	parent = egueb_dom_event_mutation_related_get(e);
	if (origin != parent)
	{
		egueb_dom_node_unref(origin);
		egueb_dom_node_unref(parent);
		return;
	}
	egueb_dom_node_unref(parent);
	DBG("Removing a node from a cloned element");
	origin_child = egueb_dom_event_target_get(e);

	/* found the child in the cloned children */
	origin_tmp = egueb_dom_node_sibling_previous_get(origin_child);
	cloned_child = egueb_dom_node_child_first_get(cloned);
	while (origin_tmp)
	{
		Egueb_Dom_Node *tmp1 = NULL;
		Egueb_Dom_Node *tmp2 = NULL;

		tmp2 = egueb_dom_node_sibling_next_get(cloned_child);
		egueb_dom_node_unref(cloned_child);
		cloned_child = tmp2;

		tmp1 = egueb_dom_node_sibling_previous_get(origin_tmp);
		egueb_dom_node_unref(origin_tmp);
		origin_tmp = tmp1;
	}
	egueb_dom_node_child_remove(cloned, cloned_child, NULL);
	egueb_dom_node_unref(cloned_child);

	egueb_dom_node_unref(origin);
	egueb_dom_node_unref(origin_child);
}

/* whenever an attribute is set on the original node, modify the cloned node
 * with the same value
 */
static void _egueb_dom_element_original_attr_modified_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *clone = data;
	Egueb_Dom_Node *attr = NULL;
	Egueb_Dom_String *s_attr;
	const Egueb_Dom_Value *v;
	Egueb_Dom_Value copy = EGUEB_DOM_VALUE_INIT;
	Egueb_Dom_Attr_Type attr_type;
	Egueb_Dom_Event_Phase phase;
	Egueb_Dom_Event_Mutation_Attr_Type type;
	Eina_Error err;

	phase = egueb_dom_event_phase_get(e);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	egueb_dom_event_mutation_attr_type_get(e, &type);
	egueb_dom_event_mutation_value_new_get(e, &v);
	attr = egueb_dom_event_mutation_related_get(e);
	egueb_dom_event_mutation_attr_modification_type_get(e, &attr_type);

	s_attr = egueb_dom_attr_name_get(attr);

	switch (type)
	{
		case EGUEB_DOM_EVENT_MUTATION_ATTR_TYPE_MODIFICATION:
		/* FIXME decide what to do when setting an animated value */
		DBG("Setting attribute '%s' on the cloned element",
			egueb_dom_string_string_get(s_attr));

		egueb_dom_value_init(&copy, v->descriptor);
		egueb_dom_value_copy(v, &copy, EINA_FALSE);
		if (!egueb_dom_element_attribute_value_set(clone, s_attr,
				attr_type, &copy, &err))
		{
			ERR("Can not set the value '%s'", eina_error_msg_get(err));
		}
		egueb_dom_value_reset(&copy);
		break;

		default:
		WARN("Removing or adding an attribute on a cloned element is"
				" not supported yet");
		break;
	}
	egueb_dom_string_unref(s_attr);
	egueb_dom_node_unref(attr);
}

/* whenever the original clone is destroyed, remove the event listeners
 * from the cloned node
 */
static void _egueb_dom_element_original_destroyed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *cloned = data;
	Egueb_Dom_Node *n = NULL;
	Egueb_Dom_Event_Phase phase;

	/* unregister every event on the other end */
	phase = egueb_dom_event_phase_get(e);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;
	
	DBG("Original destroyed, removing the events on the cloned node");
	n = egueb_dom_event_target_get(e);
	egueb_dom_node_event_listener_remove(cloned,
			EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
			_egueb_dom_element_clone_destroyed_cb,
			EINA_TRUE, n);
	egueb_dom_node_unref(n);
} 

/* whenever a cloned node is destroyed the event listeners are no longer
 * valid so we need to remove them
 */
static void _egueb_dom_element_clone_destroyed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *cloned = NULL;
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Event_Phase phase;

	/* unregister every event on the other end */
	phase = egueb_dom_event_phase_get(e);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	DBG("Clone destroyed, removing the events on the original node");
	cloned = egueb_dom_event_target_get(e);
	/* the attr modified */
	egueb_dom_node_event_listener_remove(n,
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_dom_element_original_attr_modified_cb,
			EINA_TRUE, cloned);
	/* the children */
	egueb_dom_node_event_listener_remove(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_dom_element_original_node_inserted_cb,
			EINA_TRUE, cloned);
	egueb_dom_node_event_listener_remove(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_dom_element_original_node_removed_cb,
			EINA_TRUE, cloned);
	/* the destroy */
	egueb_dom_node_event_listener_remove(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
			_egueb_dom_element_original_destroyed_cb,
			EINA_TRUE, cloned);

	egueb_dom_node_unref(cloned);
}

static Eina_Bool _egueb_dom_element_attributes_process_cb(const Eina_Hash *hash,
		const void *key, void *data, void *user_data)
{
	Egueb_Dom_Node *attr = data;
	egueb_dom_attr_process(attr);
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
static void _egueb_dom_element_clone(Egueb_Dom_Node *n, Eina_Bool live,
		Eina_Bool deep, Egueb_Dom_Node *clone)
{
	Egueb_Dom_Element *thiz;
	Egueb_Dom_Element_Class *klass;
	Egueb_Dom_Node *child;

	/* check if the clone is imeplemented */
	klass = EGUEB_DOM_ELEMENT_CLASS_GET(n);
	if (klass->clone)
	{
		klass->clone(EGUEB_DOM_ELEMENT(n), EGUEB_DOM_ELEMENT(clone));
	}

	/* copy every element attribute */
	thiz = EGUEB_DOM_ELEMENT(n);
	eina_hash_foreach(thiz->attributes, _egueb_dom_element_properties_clone_cb, clone);
	/* in case we are live, every change on the properties should
	 * trigger a change here
	 */
	if (live)
	{
		egueb_dom_node_event_listener_add(clone,
				EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
				_egueb_dom_element_clone_destroyed_cb,
				EINA_TRUE, n);
		egueb_dom_node_event_listener_add(n,
				EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
				_egueb_dom_element_original_destroyed_cb,
				EINA_TRUE, clone);
		egueb_dom_node_event_listener_add(n,
				EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
				_egueb_dom_element_original_attr_modified_cb,
				EINA_TRUE, clone);
		/* in case of a deep case, also listen for node added
		 * and node removed events
		 */
		if (deep)
		{
			egueb_dom_node_event_listener_add(n,
					EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
					_egueb_dom_element_original_node_inserted_cb,
					EINA_TRUE, clone);

			egueb_dom_node_event_listener_add(n,
					EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
					_egueb_dom_element_original_node_removed_cb,
					EINA_TRUE, clone);
		}
	}
	/* now add all the children it has */
	if (!deep) return;

	EINA_INLIST_FOREACH(n->children, child)
	{
		Egueb_Dom_Node *clone_child;

		clone_child = egueb_dom_node_clone(child, live, deep, NULL);
		egueb_dom_node_child_append(clone, clone_child, NULL);
	}
}

static Ender_Item * _egueb_dom_element_item_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element_Class *klass;
	Egueb_Dom_Element *thiz;
	Ender_Item *ret = NULL;

	thiz = EGUEB_DOM_ELEMENT(n);
	klass = EGUEB_DOM_ELEMENT_CLASS_GET(thiz);
	if (klass->item_get)
		ret = klass->item_get(thiz);
	if (!ret)
	{
		const Ender_Lib *lib;

		lib = ender_lib_find("egueb_dom");
		if (!lib) return NULL;

		ret = ender_lib_item_find(lib, "egueb.dom.element");
	}

	return ret;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR, Egueb_Dom_Element,
		Egueb_Dom_Element_Class, egueb_dom_element);

static void _egueb_dom_element_class_init(void *k)
{
	Egueb_Dom_Node_Class *n_klass = EGUEB_DOM_NODE_CLASS(k);

	n_klass->type = EGUEB_DOM_NODE_TYPE_ELEMENT;
	n_klass->clone = _egueb_dom_element_clone;
	n_klass->item_get = _egueb_dom_element_item_get;
}

static void _egueb_dom_element_instance_init(void *o)
{
	Egueb_Dom_Element *thiz;

	thiz = EGUEB_DOM_ELEMENT(o);
	thiz->attributes = eina_hash_string_superfast_new(EINA_FREE_CB(
			egueb_dom_node_unref));
	/* register some event handlers */
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
			_egueb_dom_element_document_removed_cb,
			EINA_FALSE, thiz);
}

static void _egueb_dom_element_instance_deinit(void *o)
{
	Egueb_Dom_Element *thiz;

	thiz = EGUEB_DOM_ELEMENT(o);
	eina_hash_free(thiz->attributes);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_dom_element_process_children(Egueb_Dom_Element *thiz)
{
	Egueb_Dom_Node *n;
	Egueb_Dom_Node *child;
	Eina_Bool ret = EINA_TRUE;

	n = EGUEB_DOM_NODE(thiz);
	EINA_INLIST_FOREACH(n->children, child)
	{
		Egueb_Dom_Node_Class *n_klass;

		n_klass = EGUEB_DOM_NODE_CLASS_GET(child);
		if (n_klass->type != EGUEB_DOM_NODE_TYPE_ELEMENT)
			continue;

		ret = egueb_dom_element_process(child);
		if (!ret)
			break;
	}
	return ret;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_String * egueb_dom_element_name_get(Egueb_Dom_Node *node)
{
	Egueb_Dom_String *s;
	Egueb_Dom_String sid = EGUEB_DOM_STRING_STATIC("id");

	/* get the id */
	s = egueb_dom_element_attribute_get(node, &sid);
	if (egueb_dom_string_is_valid(s))
		return s;
	/* if the id is not available, use the factory name */
	egueb_dom_string_unref(s);
	s = egueb_dom_element_tag_name_get(node);
	return s;
}

/*
 * readonly attribute DOMString tagName;
 */
EAPI Egueb_Dom_String * egueb_dom_element_tag_name_get(Egueb_Dom_Node *node)
{
	Egueb_Dom_Element_Class *klass;

	klass = EGUEB_DOM_ELEMENT_CLASS_GET(node);
	if (klass->tag_name_get)
		return klass->tag_name_get(EGUEB_DOM_ELEMENT(node));
	return NULL;
}

/*
 * DOMString getAttribute(in DOMString name);
 */
EAPI Egueb_Dom_String * egueb_dom_element_attribute_get(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name)
{
	Egueb_Dom_Node *p = NULL;
	Egueb_Dom_String *ret = NULL;

	p = egueb_dom_element_attribute_fetch(node, name);
	if (!p) return NULL;

	egueb_dom_attr_string_get(p, EGUEB_DOM_ATTR_TYPE_BASE, &ret);
	egueb_dom_node_unref(p);
	return ret;
}

/*
 * void setAttribute(in DOMString name, in DOMString value)
 * raises(DOMException);
 */
EAPI Eina_Bool egueb_dom_element_attribute_set(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_String *value,
		Eina_Error *err)
{
	Egueb_Dom_Node *p = NULL;

	p = egueb_dom_element_attribute_fetch(node, name);
	if (!p)
	{
		Egueb_Dom_String *attr_name;

		attr_name = egueb_dom_string_new_with_string(
				egueb_dom_string_string_get(name));
		/* create a new string attribute */
		p = egueb_dom_attr_string_new(attr_name, NULL, EINA_FALSE,
				EINA_FALSE, EINA_FALSE);
		egueb_dom_element_attribute_add(node, egueb_dom_node_ref(p), err);
	}

	/* set the value */
	if (p)
	{
		egueb_dom_attr_string_set(p, EGUEB_DOM_ATTR_TYPE_BASE, value);
		egueb_dom_node_unref(p);
	}

	return EINA_TRUE;
}


EAPI Eina_Bool egueb_dom_element_attribute_type_set(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_String *value, Eina_Error *err)
{
	Egueb_Dom_Node *p = NULL;

	p = egueb_dom_element_attribute_fetch(node, name);
	/* set the value */
	if (p)
	{
		Eina_Bool ret;

		ret = egueb_dom_attr_string_set(p, type, value);
		egueb_dom_node_unref(p);
		
		if (!ret)
		{
			if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
			return EINA_FALSE;
		}
		return EINA_TRUE;
	}
	else
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
		return EINA_FALSE;
	}
}

EAPI Egueb_Dom_Node * egueb_dom_element_attribute_fetch(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name)
{
	Egueb_Dom_Element *thiz;
	Egueb_Dom_Node *ret;

	thiz = EGUEB_DOM_ELEMENT(node);
	if (!name) return NULL;

	ret = eina_hash_find(thiz->attributes, egueb_dom_string_string_get(name));
	return egueb_dom_node_ref(ret);
}

EAPI Eina_Bool egueb_dom_element_attribute_masked_set_va(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, va_list args)
{
	Egueb_Dom_Node *p;
	Eina_Bool ret;

	p = egueb_dom_element_attribute_fetch(node, name);
	if (!p) return EINA_FALSE;
	ret = egueb_dom_attr_set_va(p, prop_mask, args);
	egueb_dom_node_unref(p);
	return ret;
}

EAPI Eina_Bool egueb_dom_element_attribute_masked_get_va(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, va_list args)
{
	Egueb_Dom_Node *p;
	Eina_Bool ret;

	p = egueb_dom_element_attribute_fetch(node, name);
	if (!p) return EINA_FALSE;
	ret = egueb_dom_attr_get_va(p, prop_mask, args);
	egueb_dom_node_unref(p);
	return ret;
}


/* Sets the value of a property. The value argument depends
 * on the property itself
 */
EAPI Eina_Bool egueb_dom_element_attribute_masked_set(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, ...)
{
	Eina_Bool ret;
	va_list args;

	va_start(args, prop_mask);
	ret = egueb_dom_element_attribute_masked_set_va(node, name, prop_mask, args);
	va_end(args);

	return ret;
}

/* Gets the value of a property. The value argument depends
 * on the property itself.
 */
EAPI Eina_Bool egueb_dom_element_attribute_masked_get(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, ...)
{
	Eina_Bool ret;
	va_list args;

	va_start(args, prop_mask);
	ret = egueb_dom_element_attribute_masked_get_va(node, name, prop_mask, args);
	va_end(args);

	return ret;
}

EAPI Eina_Bool egueb_dom_element_attribute_value_set(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_Value *v, Eina_Error *err)
{
	Egueb_Dom_Node *p;
	Eina_Bool ret;

	p = egueb_dom_element_attribute_fetch(node, name);
	if (!p)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
		return EINA_FALSE;
	}

	/* copy it to the property value */
	ret = egueb_dom_attr_value_set(p, type, v);
	egueb_dom_node_unref(p);

	if (!ret)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_SUPPORTED;
		return EINA_FALSE;
	}
	else
	{
		return EINA_TRUE;
	}
}

EAPI Eina_Bool egueb_dom_element_attribute_value_get(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_Value *v, Eina_Error *err)
{
	Egueb_Dom_Node *p;
	Eina_Bool ret;

	p = egueb_dom_element_attribute_fetch(node, name);
	if (!p)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
		return EINA_FALSE;
	}

	/* copy it to the property value */
	ret = egueb_dom_attr_value_get(p, type, v);
	egueb_dom_node_unref(p);

	if (!ret)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_SUPPORTED;
		return EINA_FALSE;
	}
	else
	{
		return EINA_TRUE;
	}
}

EAPI Eina_Bool egueb_dom_element_attribute_add(Egueb_Dom_Node *n,
		Egueb_Dom_Node *attr, Eina_Error *err)
{
	Egueb_Dom_Element *thiz;
	Egueb_Dom_Attr *a;
	Egueb_Dom_Node *old_attr;

	if (!n || !attr)
	{
		if (attr) egueb_dom_node_unref(attr);
		if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}

	a = EGUEB_DOM_ATTR(attr);
	if (!a->name)
	{
		egueb_dom_node_unref(attr);
		ERR("Attribute does not have a name");
		if (err) *err = EGUEB_DOM_ERROR_NOT_SUPPORTED;
		return EINA_FALSE;
	}

	if (a->owner)
	{
		egueb_dom_node_unref(attr);
		ERR("Attribute already has an owner");
		if (err) *err = EGUEB_DOM_ERROR_NOT_SUPPORTED;
		return EINA_FALSE;
	}

	thiz = EGUEB_DOM_ELEMENT(n);
	old_attr = eina_hash_find(thiz->attributes,
			egueb_dom_string_string_get(a->name));
	if (old_attr)
	{
		WARN("Attribute '%s' already found",
				egueb_dom_string_string_get(a->name));
		if (err) *err = EGUEB_DOM_ERROR_INUSE_ATTRIBUTE;
		egueb_dom_node_unref(attr);
		return EINA_FALSE;
	}
	DBG("Adding attribute '%s'", egueb_dom_string_string_get(a->name));
	/* set the owner on the attribute */
	a->owner = n;
	eina_hash_add(thiz->attributes, egueb_dom_string_string_get(a->name),
			attr);
	return EINA_TRUE;
}

EAPI Eina_Bool egueb_dom_element_is_enqueued(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element *thiz;

	thiz = EGUEB_DOM_ELEMENT(n);
	return thiz->enqueued;
}

/**
 * @brief Make the element be dequeued from the processing list
 * @param[in] n The element to be dequeued [transfer full]
 */
EAPI void egueb_dom_element_dequeue(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element *thiz;
	Egueb_Dom_Node *doc;

	if (egueb_dom_node_is_destroying(n))
	{
		WARN("Element is being destroyed, nothing to do");
		egueb_dom_node_unref(n);
		return;
	}

	doc = egueb_dom_node_owner_document_get(n);
	if (!doc)
	{
		WARN("Element does not have a document");
		egueb_dom_node_unref(n);
		return;
	}

	thiz = EGUEB_DOM_ELEMENT(n);
	if (!thiz->enqueued)
	{
		INFO("Element not enqueued");
		egueb_dom_node_unref(doc);
		egueb_dom_node_unref(n);
		return;
	}
	egueb_dom_document_element_dequeue(doc, n);
	thiz->enqueued = EINA_FALSE;
	egueb_dom_node_unref(doc);
}

/**
 * @brief Make the element be enqueued on the processing list
 * @param[in] n The element to be enqueued [transfer full]
 */
EAPI void egueb_dom_element_enqueue(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element *thiz;
	Egueb_Dom_Node *doc;

	if (egueb_dom_node_is_destroying(n))
	{
		WARN("Element is being destroyed, nothing to do");
		egueb_dom_node_unref(n);
		return;
	}

	doc = egueb_dom_node_owner_document_get(n);
	if (!doc)
	{
		WARN_ELEMENT(n, "Element does not have a document");
		egueb_dom_node_unref(n);
		return;
	}

	thiz = EGUEB_DOM_ELEMENT(n);
	if (thiz->enqueued)
	{
		INFO_ELEMENT(n, "Node already enqueued, nothing to do");
		egueb_dom_node_unref(doc);
		egueb_dom_node_unref(n);
		return;
	}
	egueb_dom_document_element_enqueue(doc, n);
	thiz->enqueued = EINA_TRUE;
	egueb_dom_node_unref(doc);
}

/**
 * @brief Request an element for a process
 * @param[in] n The element to request the process
 *
 * This will send an process event on the element for later requesting. If this
 * event reaches the topmost element in the bubbling phase it will be
 * automatically enqueued on the list of elements to process
 */
EAPI void egueb_dom_element_request_process(Egueb_Dom_Node *n)
{
	Egueb_Dom_Event *e;

	/* send the request process event */
	e = egueb_dom_event_process_new();
	egueb_dom_node_event_dispatch(n, e, NULL, NULL);
}

EAPI Eina_Bool egueb_dom_element_process(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element *thiz;
	Egueb_Dom_Element_Class *klass;
	Egueb_Dom_Node *doc;
	Eina_Bool ret = EINA_TRUE;

	thiz = EGUEB_DOM_ELEMENT(n);
	klass = EGUEB_DOM_ELEMENT_CLASS_GET(thiz);

	INFO_ELEMENT(n, "Processing");
	/* check if the document is actually processing, if so
	 * it means that or eiher the document called us, or some other
	 * element wants to process this element, remove the enqueued flag
	 */
	doc = egueb_dom_node_owner_document_get(n);
	if (doc)
	{
		if (egueb_dom_document_is_processing(doc))
			thiz->enqueued = EINA_FALSE;
	}

	if (klass->process)
		ret = klass->process(thiz);
	/* unset the flag that informs the inheritable change */
	thiz->inheritable_changed = EINA_FALSE;
	thiz->attr_changed = EINA_FALSE;
	/* process every attribute */
	eina_hash_foreach(thiz->attributes,
			_egueb_dom_element_attributes_process_cb, NULL);

	/* set the run timestamp */
	if (doc)
	{
		thiz->last_run = egueb_dom_document_current_run_get(doc);
		egueb_dom_node_unref(doc);
	}

	return ret;
}

EAPI Egueb_Dom_Node * egueb_dom_element_child_first_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *ret;

	ret = egueb_dom_node_child_first_get(n);
	while (ret && (egueb_dom_node_type_get(ret) != EGUEB_DOM_NODE_TYPE_ELEMENT))
	{
		Egueb_Dom_Node *next;

		next = egueb_dom_node_sibling_next_get(ret);
		egueb_dom_node_unref(ret);
		ret = next;
	}
	
	return ret;
}

EAPI Egueb_Dom_Node * egueb_dom_element_child_last_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *ret;

	ret = egueb_dom_node_child_last_get(n);
	while (ret && (egueb_dom_node_type_get(ret) != EGUEB_DOM_NODE_TYPE_ELEMENT))
	{
		Egueb_Dom_Node *next;

		next = egueb_dom_node_sibling_previous_get(ret);
		egueb_dom_node_unref(ret);
		ret = next;
	}
	
	return ret;

}

EAPI Egueb_Dom_Node * egueb_dom_element_sibling_previous_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *ret;

	ret = egueb_dom_node_sibling_previous_get(n);
	while (ret && (egueb_dom_node_type_get(ret) != EGUEB_DOM_NODE_TYPE_ELEMENT))
	{
		Egueb_Dom_Node *next;

		next = egueb_dom_node_sibling_previous_get(ret);
		egueb_dom_node_unref(ret);
		ret = next;
	}
	
	return ret;
}

EAPI Egueb_Dom_Node * egueb_dom_element_sibling_next_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *ret;

	ret = egueb_dom_node_sibling_next_get(n);
	while (ret && (egueb_dom_node_type_get(ret) != EGUEB_DOM_NODE_TYPE_ELEMENT))
	{
		Egueb_Dom_Node *next;

		next = egueb_dom_node_sibling_next_get(ret);
		egueb_dom_node_unref(ret);
		ret = next;
	}
	
	return ret;
}

#if 0
  /* For the remove, we need a way on the element inherited to set it to a default
   * value, but given that it depends on the element itself, we need to use
   * a property function, like clear() for example
   */
  void               removeAttribute(in DOMString name)
                                        raises(DOMException);
  Attr               getAttributeNode(in DOMString name);
  Attr               setAttributeNode(in Attr newAttr)
                                        raises(DOMException);
  Attr               removeAttributeNode(in Attr oldAttr)
                                        raises(DOMException);
  NodeList           getElementsByTagName(in DOMString name);
  // Introduced in DOM Level 2:
  DOMString          getAttributeNS(in DOMString namespaceURI, 
                                    in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  void               setAttributeNS(in DOMString namespaceURI, 
                                    in DOMString qualifiedName, 
                                    in DOMString value)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  void               removeAttributeNS(in DOMString namespaceURI, 
                                       in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  Attr               getAttributeNodeNS(in DOMString namespaceURI, 
                                        in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  Attr               setAttributeNodeNS(in Attr newAttr)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  NodeList           getElementsByTagNameNS(in DOMString namespaceURI, 
                                            in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  boolean            hasAttribute(in DOMString name);
  // Introduced in DOM Level 2:
  boolean            hasAttributeNS(in DOMString namespaceURI, 
                                    in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 3:
  readonly attribute TypeInfo        schemaTypeInfo;
  // Introduced in DOM Level 3:
  void               setIdAttribute(in DOMString name, 
                                    in boolean isId)
                                        raises(DOMException);
  // Introduced in DOM Level 3:
  void               setIdAttributeNS(in DOMString namespaceURI, 
                                      in DOMString localName, 
                                      in boolean isId)
                                        raises(DOMException);
  // Introduced in DOM Level 3:
  void               setIdAttributeNode(in Attr idAttr, 
                                        in boolean isId)
                                        raises(DOMException);
};
#endif
