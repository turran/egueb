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
#include "egueb_dom_element.h"
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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Dom_Element_Property_Clone_Data
{
	Egueb_Dom_Node *thiz;
	Egueb_Dom_Node *other;
} Egueb_Dom_Element_Property_Clone_Data;

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

	egueb_dom_event_phase_get(e, &phase);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;
	thiz->enqueued = EINA_FALSE;
}

static Eina_Bool _egueb_dom_element_properties_clone_cb(Egueb_Dom_Attr_Fetch fetch,
		Egueb_Dom_Element_Property_Clone_Data *clone_data)
{
	Egueb_Dom_Node *pt, *po;

	/* fetch both properties */
	fetch(clone_data->thiz, &pt);
	fetch(clone_data->other, &po);

	egueb_dom_attr_copy(pt, po);

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
	egueb_dom_event_target_current_get(e, &origin);
	egueb_dom_event_mutation_related_get(e, &parent);
	if (origin != parent)
	{
		egueb_dom_node_unref(origin);
		egueb_dom_node_unref(parent);
		return;
	}
	egueb_dom_node_unref(parent);

	DBG("Inserting a node on a cloned element");
	egueb_dom_event_target_get(e, &origin_child);
	egueb_dom_node_clone(origin_child, EINA_TRUE, EINA_TRUE, &cloned_child);
	if (!cloned_child) goto err_clone;

	/* check if it does not have any next sibling (simple append) */
	egueb_dom_node_sibling_next_get(origin_child, &origin_tmp);
	if (!origin_tmp)
	{
		egueb_dom_node_child_append(cloned, cloned_child);
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
		egueb_dom_node_sibling_previous_get(origin_child, &origin_tmp);

		egueb_dom_node_child_first_get(cloned, &cloned_rel);
		while (origin_tmp)
		{
			Egueb_Dom_Node *tmp1 = NULL;
			Egueb_Dom_Node *tmp2 = NULL;

			egueb_dom_node_sibling_next_get(cloned_rel, &tmp2);
			egueb_dom_node_unref(cloned_rel);
			cloned_rel = tmp2;

			egueb_dom_node_sibling_previous_get(origin_tmp, &tmp1);
			egueb_dom_node_unref(origin_tmp);
			origin_tmp = tmp1;
		}
		/* we need to insert the new node before the found rel */
		egueb_dom_node_insert_before(cloned, cloned_child, cloned_rel);
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
	egueb_dom_event_target_current_get(e, &origin);
	egueb_dom_event_mutation_related_get(e, &parent);
	if (origin != parent)
	{
		egueb_dom_node_unref(origin);
		egueb_dom_node_unref(parent);
		return;
	}
	egueb_dom_node_unref(parent);
	DBG("Removing a node from a cloned element");
	egueb_dom_event_target_get(e, &origin_child);

	/* found the child in the cloned children */
	egueb_dom_node_sibling_previous_get(origin_child, &origin_tmp);
	egueb_dom_node_child_first_get(cloned, &cloned_child);
	while (origin_tmp)
	{
		Egueb_Dom_Node *tmp1 = NULL;
		Egueb_Dom_Node *tmp2 = NULL;

		egueb_dom_node_sibling_next_get(cloned_child, &tmp2);
		egueb_dom_node_unref(cloned_child);
		cloned_child = tmp2;

		egueb_dom_node_sibling_previous_get(origin_tmp, &tmp1);
		egueb_dom_node_unref(origin_tmp);
		origin_tmp = tmp1;
	}
	egueb_dom_node_child_remove(cloned, cloned_child);
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

	egueb_dom_event_phase_get(e, &phase);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	egueb_dom_event_mutation_attr_type_get(e, &type);
	egueb_dom_event_mutation_value_new_get(e, &v);
	egueb_dom_event_mutation_related_get(e, &attr);
	egueb_dom_event_mutation_attr_modification_type_get(e, &attr_type);

	egueb_dom_attr_name_get(attr, &s_attr);

	switch (type)
	{
		case EGUEB_DOM_EVENT_MUTATION_ATTR_TYPE_MODIFICATION:
		/* FIXME decide what to do when setting an animated value */
		DBG("Setting attribute '%s' on the cloned element",
			egueb_dom_string_string_get(s_attr));

		egueb_dom_value_init(&copy, v->descriptor);
		egueb_dom_value_copy(v, &copy);
		err = egueb_dom_element_property_value_set(clone, s_attr,
				attr_type, &copy);
		if (err != EINA_ERROR_NONE)
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
	egueb_dom_event_phase_get(e, &phase);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;
	
	DBG("Original destroyed, removing the events on the cloned node");
	egueb_dom_event_target_get(e, &n);
	egueb_dom_node_event_listener_remove(cloned,
			EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
			_egueb_dom_element_clone_destroyed_cb,
			EINA_FALSE, n);
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
	egueb_dom_event_phase_get(e, &phase);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	DBG("Clone destroyed, removing the events on the original node");
	egueb_dom_event_target_get(e, &cloned);
	/* the attr modified */
	egueb_dom_node_event_listener_remove(n,
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_dom_element_original_attr_modified_cb,
			EINA_FALSE, cloned);
	/* the children */
	egueb_dom_node_event_listener_remove(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_dom_element_original_node_inserted_cb,
			EINA_FALSE, cloned);
	egueb_dom_node_event_listener_remove(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_dom_element_original_node_removed_cb,
			EINA_FALSE, cloned);
	/* the destroy */
	egueb_dom_node_event_listener_remove(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
			_egueb_dom_element_original_destroyed_cb,
			EINA_FALSE, cloned);

	egueb_dom_node_unref(cloned);
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
static void _egueb_dom_element_clone(Egueb_Dom_Node *n, Eina_Bool live,
		Eina_Bool deep, Egueb_Dom_Node *clone)
{
	Egueb_Dom_Element_Property_Clone_Data data;
	Egueb_Dom_Element_Class *klass;
	Egueb_Dom_Node *child;
	Egueb_Dom_Attr_Fetch fetch;
	Eina_List *l;

	/* TODO copy every element attribute */
	/* class properties will be created directly on the constructor */
	/* set every property */
	data.thiz = n;
	data.other = clone;

	klass = EGUEB_DOM_ELEMENT_CLASS_GET(n);
	/* FIXME fix this */
	EINA_LIST_FOREACH(klass->properties->order, l, fetch)
	{
		_egueb_dom_element_properties_clone_cb(fetch, &data);
	}
	/* in case we are live, every change on the properties should
	 * trigger a change here
	 */
	if (live)
	{
		egueb_dom_node_event_listener_add(clone,
				EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
				_egueb_dom_element_clone_destroyed_cb,
				EINA_FALSE, n);
		egueb_dom_node_event_listener_add(n,
				EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
				_egueb_dom_element_original_destroyed_cb,
				EINA_FALSE, clone);
		egueb_dom_node_event_listener_add(n,
				EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
				_egueb_dom_element_original_attr_modified_cb,
				EINA_FALSE, clone);
		/* in case of a deep case, also listen for node added
		 * and node removed events
		 */
		if (deep)
		{
			egueb_dom_node_event_listener_add(n,
					EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
					_egueb_dom_element_original_node_inserted_cb,
					EINA_FALSE, clone);

			egueb_dom_node_event_listener_add(n,
					EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
					_egueb_dom_element_original_node_removed_cb,
					EINA_FALSE, clone);
		}
	}
	/* now add all the children it has */
	if (!deep) return;

	EINA_INLIST_FOREACH(n->children, child)
	{
		Egueb_Dom_Node *clone_child;

		egueb_dom_node_clone(child, live, deep, &clone_child);
		egueb_dom_node_child_append(clone, clone_child);
	}
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR, Egueb_Dom_Element,
		Egueb_Dom_Element_Class, egueb_dom_element);

static void _egueb_dom_element_class_init(void *k)
{
	Egueb_Dom_Node_Class *n_klass = EGUEB_DOM_NODE_CLASS(k);
	Egueb_Dom_Element_Class *klass;

	n_klass->type = EGUEB_DOM_NODE_TYPE_ELEMENT_NODE;
	n_klass->clone = _egueb_dom_element_clone;

	klass = EGUEB_DOM_ELEMENT_CLASS(k);
	klass->properties = eina_extra_ordered_hash_new(NULL);
}

static void _egueb_dom_element_class_deinit(void *k)
{
	Egueb_Dom_Element_Class *klass;

	klass = EGUEB_DOM_ELEMENT_CLASS(k);
	eina_extra_ordered_hash_free(klass->properties);
}

static void _egueb_dom_element_instance_init(void *o)
{
	Egueb_Dom_Element *thiz;

	thiz = EGUEB_DOM_ELEMENT(o);
	thiz->attributes = eina_extra_ordered_hash_new(EINA_FREE_CB(
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
	eina_extra_ordered_hash_free(thiz->attributes);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Error egueb_dom_element_process_children(Egueb_Dom_Element *thiz)
{
	Egueb_Dom_Node *n;
	Egueb_Dom_Node *child;
	Eina_Error ret = EINA_ERROR_NONE;

	n = EGUEB_DOM_NODE(thiz);
	EINA_INLIST_FOREACH(n->children, child)
	{
		Egueb_Dom_Node_Class *n_klass;

		n_klass = EGUEB_DOM_NODE_CLASS_GET(child);
		if (n_klass->type != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
			continue;

		ret = egueb_dom_element_process(child);
		if (ret != EINA_ERROR_NONE)
			break;
	}
	return ret;
}

void egueb_dom_element_dequeue(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element *thiz;

	thiz = EGUEB_DOM_ELEMENT(n);
	if (!thiz->enqueued)
	{
		WARN("Element not enqueued");
		egueb_dom_node_unref(n);
		return;
	}
	thiz->enqueued = EINA_FALSE;
}

void egueb_dom_element_enqueue(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element *thiz;

	thiz = EGUEB_DOM_ELEMENT(n);
	if (thiz->enqueued)
	{
		WARN("Node already enqueued, nothing to do");
		egueb_dom_node_unref(n);
		return;
	}
	thiz->enqueued = EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/*
 * readonly attribute DOMString tagName;
 */
EAPI Eina_Error egueb_dom_element_tag_name_get(Egueb_Dom_Node *node,
		Egueb_Dom_String **name)
{
	Egueb_Dom_Element_Class *klass;

	klass = EGUEB_DOM_ELEMENT_CLASS_GET(node);
	if (klass->tag_name_get)
	{
		*name = klass->tag_name_get(EGUEB_DOM_ELEMENT(node));
	}
	else
	{
		*name = NULL;
	}
	return EINA_ERROR_NONE;
}

/*
 * DOMString getAttribute(in DOMString name);
 */
EAPI Eina_Error egueb_dom_element_attribute_get(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_String **value)
{
	Egueb_Dom_Node *p = NULL;

	egueb_dom_element_property_fetch(node, name, &p);
	if (!p) return EINA_ERROR_NONE;

	egueb_dom_attr_string_get(p, EGUEB_DOM_ATTR_TYPE_BASE, value);
	egueb_dom_node_unref(p);

	return EINA_ERROR_NONE;
}

/*
 * void setAttribute(in DOMString name, in DOMString value)
 * raises(DOMException);
 */
EAPI Eina_Error egueb_dom_element_attribute_set(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_String *value)
{
	Egueb_Dom_Node *p = NULL;

	egueb_dom_element_property_fetch(node, name, &p);
	if (!p)
	{
		Egueb_Dom_Element *thiz;
		Egueb_Dom_String *attr_name;

		thiz = EGUEB_DOM_ELEMENT(node);
		attr_name = egueb_dom_string_new_with_string(
				egueb_dom_string_string_get(name));
		/* create a new string attribute */
		p = egueb_dom_attr_string_new(attr_name, NULL);
		eina_extra_ordered_hash_add(thiz->attributes,
				egueb_dom_string_string_get(name), p);
		p = egueb_dom_node_ref(p);
	}

	/* set the value */
	if (p)
	{
		egueb_dom_attr_string_set(p, EGUEB_DOM_ATTR_TYPE_BASE, value);
		egueb_dom_node_unref(p);
	}

	return EINA_ERROR_NONE;
}


EAPI Eina_Error egueb_dom_element_attribute_type_set(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_String *value)
{
	Egueb_Dom_Node *p = NULL;
	Eina_Error ret = EGUEB_DOM_ERROR_NOT_FOUND;

	egueb_dom_element_property_fetch(node, name, &p);
	/* set the value */
	if (p)
	{
		if (egueb_dom_attr_string_set(p, type, value))
			ret = EINA_ERROR_NONE;
		egueb_dom_node_unref(p);
	}
	return ret;
}

EAPI Eina_Error egueb_dom_element_property_fetch(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_Node **p)
{
	Egueb_Dom_Element_Class *klass;
	Egueb_Dom_Attr_Fetch fetch;
	Egueb_Dom_Node *attr;

	if (!p) return EGUEB_DOM_ERROR_INVALID_ACCESS;
	klass = EGUEB_DOM_ELEMENT_CLASS_GET(node);
	fetch = eina_extra_ordered_hash_find(klass->properties, egueb_dom_string_string_get(name));
	/* ok it is a class attribute */
	if (fetch)
	{
		/* fetch the atribute */
		fetch(node, &attr);
 	}
	/* we need to find the element attribute or create it otherwise */
	else
	{
		Egueb_Dom_Element *thiz;

		thiz = EGUEB_DOM_ELEMENT(node);
		attr = eina_extra_ordered_hash_find(thiz->attributes, egueb_dom_string_string_get(name));
	}

	if (!attr)
	{
		*p = NULL;
		return EGUEB_DOM_ERROR_NOT_FOUND;
	}
	else
	{
		*p = egueb_dom_node_ref(attr);
		return EINA_ERROR_NONE;
	}

}

EAPI Eina_Error egueb_dom_element_property_set_va(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, va_list args)
{
	Egueb_Dom_Node *p;
	Eina_Error ret;

	egueb_dom_element_property_fetch(node, name, &p);
	if (!p) return EGUEB_DOM_ERROR_NOT_FOUND;
	ret = egueb_dom_attr_set_va(p, prop_mask, args);
	egueb_dom_node_unref(p);
	return ret;
}

EAPI Eina_Error egueb_dom_element_property_get_va(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, va_list args)
{
	Egueb_Dom_Node *p;
	Eina_Error ret;

	egueb_dom_element_property_fetch(node, name, &p);
	if (!p) return EGUEB_DOM_ERROR_NOT_FOUND;
	ret = egueb_dom_attr_get_va(p, prop_mask, args);
	egueb_dom_node_unref(p);
	return ret;
}


/* Sets the value of a property. The value argument depends
 * on the property itself
 */
EAPI Eina_Error egueb_dom_element_property_set(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, ...)
{
	Eina_Error ret;
	va_list args;

	va_start(args, prop_mask);
	ret = egueb_dom_element_property_set_va(node, name, prop_mask, args);
	va_end(args);

	return ret;
}

/* Gets the value of a property. The value argument depends
 * on the property itself.
 */
EAPI Eina_Error egueb_dom_element_property_get(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, ...)
{
	Eina_Error ret;
	va_list args;

	va_start(args, prop_mask);
	ret = egueb_dom_element_property_get_va(node, name, prop_mask, args);
	va_end(args);

	return ret;
}

EAPI Eina_Error egueb_dom_element_property_value_set(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, Egueb_Dom_Attr_Type type, Egueb_Dom_Value *v)
{
	Egueb_Dom_Node *p;
	Eina_Error ret = EINA_ERROR_NONE;

	egueb_dom_element_property_fetch(node, name, &p);
	if (!p) return EGUEB_DOM_ERROR_NOT_FOUND;
	/* copy it to the property value */
	if (!egueb_dom_attr_value_set(p, type, v))
		ret = EGUEB_DOM_ERROR_NOT_SUPPORTED;
	egueb_dom_node_unref(p);
	return ret;
}

EAPI Eina_Error egueb_dom_element_property_value_get(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, Egueb_Dom_Attr_Type type, Egueb_Dom_Value *v)
{
	Egueb_Dom_Node *p;
	Eina_Error ret = EINA_ERROR_NONE;

	egueb_dom_element_property_fetch(node, name, &p);
	if (!p) return EGUEB_DOM_ERROR_NOT_FOUND;
	/* copy it to the property value */
	if (!egueb_dom_attr_value_get(p, type, v))
		ret = EGUEB_DOM_ERROR_NOT_SUPPORTED;
	egueb_dom_node_unref(p);
	return ret;
}


EAPI Eina_Error egueb_dom_element_class_property_add(Egueb_Dom_Node *n,
		Egueb_Dom_Node *p, Egueb_Dom_Attr_Fetch fetch)
{
	Egueb_Dom_Element_Class *klass;
	Egueb_Dom_Attr *attr;
	Egueb_Dom_Attr_Fetch old_fetch;

	if (!p) return EGUEB_DOM_ERROR_INVALID_ACCESS;

	attr = EGUEB_DOM_ATTR(p);
	if (!attr->name) return EGUEB_DOM_ERROR_NOT_SUPPORTED;
	/* set the owner on the property */
	if (attr->owner) return EGUEB_DOM_ERROR_NOT_SUPPORTED;
	attr->owner = n;
	/* add the property fetch in the class */
	klass = EGUEB_DOM_ELEMENT_CLASS_GET(n);
	old_fetch = eina_extra_ordered_hash_find(klass->properties,
			egueb_dom_string_string_get(attr->name));
	if (old_fetch)
	{
		WARN("Property '%s' already found",
				egueb_dom_string_string_get(attr->name));
		return EGUEB_DOM_ERROR_INUSE_ATTRIBUTE;
	}

	DBG("Adding property '%s'", egueb_dom_string_string_get(attr->name));
	eina_extra_ordered_hash_add(klass->properties,
			egueb_dom_string_string_get(attr->name), fetch);
	return EINA_ERROR_NONE;
}

/* Maybe rename this to enqueued? */
EAPI Eina_Bool egueb_dom_element_changed(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element *thiz;

	thiz = EGUEB_DOM_ELEMENT(n);
	return thiz->changed;
}

EAPI Eina_Bool egueb_dom_element_is_enqueued(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element *thiz;

	thiz = EGUEB_DOM_ELEMENT(n);
	return thiz->enqueued;
}

EAPI void egueb_dom_element_request_process(Egueb_Dom_Node *n)
{
	Egueb_Dom_Event *e;

	/* send the request process event */
	e = egueb_dom_event_mutation_new();
	egueb_dom_event_mutation_init_request_process(e);
	egueb_dom_node_event_dispatch(n, e, NULL);
}

EAPI Eina_Bool egueb_dom_element_process(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element *thiz;
	Egueb_Dom_Element_Class *klass;

	thiz = EGUEB_DOM_ELEMENT(n);
	klass = EGUEB_DOM_ELEMENT_CLASS_GET(thiz);
	if (klass->process) return klass->process(thiz);
	/* unset the flag that informs the inheritable change */
	thiz->inheritable_changed = EINA_FALSE;
	thiz->attr_changed = EINA_FALSE;
	return EINA_TRUE;
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
