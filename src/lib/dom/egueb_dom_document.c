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
#include "egueb_dom_document.h"
#include "egueb_dom_document_fragment.h"
#include "egueb_dom_element.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_mutation.h"
#include "egueb_dom_implementation.h"
#include "egueb_dom_uri.h"
#include "egueb_dom_utils.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_element_private.h"
#include "egueb_dom_document_private.h"

#include "egueb_dom_event_process_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_dom_document_insert_id(Egueb_Dom_Document *thiz, Egueb_Dom_Node *n,
		Egueb_Dom_String *id)
{
	/* FIXME to avoid finding the node, we need to know the old
	 * id, and for that we need to copy the previous data
	 * which is maybe slower than this? we need to test
	 */
	if (eina_hash_del_by_data(thiz->ids, n))
	{
		DBG("Previous id found, removing the node");
		egueb_dom_node_unref(n);
	}

	if (egueb_dom_string_is_valid(id) && thiz->ids)
	{
		Egueb_Dom_Node *old;
		const char *str;

		str = egueb_dom_string_string_get(id);
		old = eina_hash_find(thiz->ids, str);
		if (old)
		{
			WARN("Same id found for '%s', nothing to do", str);
			egueb_dom_string_unref(id);
			return;
		}
		INFO("Adding id '%s' to the list of ids", str);
		eina_hash_add(thiz->ids, str, egueb_dom_node_ref(n));
	}
	egueb_dom_string_unref(id);
}

static void _egueb_dom_document_attr_modified_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT(data);
	Egueb_Dom_Node *target;
	Egueb_Dom_String *attr_name = NULL;

	DBG("Node attr modified");
	target = egueb_dom_event_target_get(ev);

	/* check if the attribute is the id */
	egueb_dom_event_mutation_attr_name_get(ev, &attr_name);
	if (egueb_dom_string_is_valid(attr_name) &&
			(!strcmp(egueb_dom_string_string_get(attr_name), "id")))
	{
		Egueb_Dom_String *attr_val = NULL;

		egueb_dom_event_mutation_value_new_string_get(ev, &attr_val);
		_egueb_dom_document_insert_id(thiz, target, attr_val);
	}

	if (!egueb_dom_event_mutation_is_process_prevented(ev))
	{
		DBG("Process not prevented, enqueuing");
		egueb_dom_element_enqueue(target);
	}
	else
	{
		DBG("Process prevented, nothing to do");
		egueb_dom_node_unref(target);
	}
}

/* a node has been inserted into its parent */
static void _egueb_dom_document_node_inserted_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Node *target;
	Egueb_Dom_Node_Type type;
	Egueb_Dom_String *name;

	target = egueb_dom_event_target_get(ev);
	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT)
	{
		egueb_dom_node_unref(target);
		return;
	}

	name = egueb_dom_node_name_get(target);
	DBG("Node '%s' inserted", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	if (!egueb_dom_event_mutation_is_process_prevented(ev))
	{
		DBG("Process not prevented, enqueuing");
		egueb_dom_element_enqueue(target);
	}
	else
	{
		DBG("Process prevented, nothing to do");
		egueb_dom_node_unref(target);
	}
}

static void _egueb_dom_document_node_removed_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Node *target;
	Egueb_Dom_Node_Type type;
	Egueb_Dom_String *name;

	target = egueb_dom_event_target_get(ev);
	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT)
	{
		egueb_dom_node_unref(target);
		return;
	}

	name = egueb_dom_node_name_get(target);
	DBG("Node '%s' removed", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	/* remove the element from the queue */
	egueb_dom_element_dequeue(target);
}

static void _egueb_dom_document_node_insterted_into_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT(data);
	Egueb_Dom_Node *target;
	Egueb_Dom_Node_Type type;
	Egueb_Dom_String *id = NULL;
	Egueb_Dom_String id_attr = EGUEB_DOM_STRING_STATIC("id");
	Egueb_Dom_String *name;

	target = egueb_dom_event_target_get(ev);
	name = egueb_dom_node_name_get(target);
	INFO("Node '%s' inserted into the document", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT)
	{
		egueb_dom_node_unref(target);
		return;
	}

	/* add the element to the ids */
	id = egueb_dom_element_attribute_get(target, &id_attr);
	if (egueb_dom_string_is_valid(id))
	{
		_egueb_dom_document_insert_id(thiz, target, id);
	}
	egueb_dom_string_unref(id);
	egueb_dom_node_unref(target);
}

static void _egueb_dom_document_node_removed_from_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT(data);
	Egueb_Dom_Node *target;
	Egueb_Dom_Node_Type type;
	Egueb_Dom_String *id = NULL;
	Egueb_Dom_String id_attr = EGUEB_DOM_STRING_STATIC("id");
	Egueb_Dom_String *name;

	target = egueb_dom_event_target_get(ev);
	name = egueb_dom_node_name_get(target);
	INFO("Node '%s' removed from the document", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT)
	{
		egueb_dom_node_unref(target);
		return;
	}

	/* remove the element from the ids */
	id = egueb_dom_element_attribute_get(target, &id_attr);
	if (egueb_dom_string_is_valid(id) && thiz->ids)
	{
		INFO("Removing id '%s' from the list of ids", id->str);
		eina_hash_del(thiz->ids,
				egueb_dom_string_string_get(id),
				target);
	}
	egueb_dom_string_unref(id);
	egueb_dom_node_unref(target);
}

static void _egueb_dom_document_request_process_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Node *target;

	DBG("Requesting process");
	target = egueb_dom_event_target_get(ev);
	/* Add it to the list in case it is not already there */
	egueb_dom_element_enqueue(target);
}

/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_document_child_appendable(Egueb_Dom_Node *n,
		Egueb_Dom_Node *child)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_Document_Class *klass;
	Egueb_Dom_Node *current;

	thiz = EGUEB_DOM_DOCUMENT(n);
	if (egueb_dom_node_type_get(child) != EGUEB_DOM_NODE_TYPE_ELEMENT)
	{
		WARN("The child is not an element");
		return EINA_FALSE;
	}

	current = egueb_dom_document_document_element_get(n);
	if (current)
	{
		egueb_dom_node_unref(current);
		WARN("Only one child per document");
		return EINA_FALSE;
	}

	klass = EGUEB_DOM_DOCUMENT_CLASS_GET(n);
	if (klass->child_appendable)
		return klass->child_appendable(thiz, child);
	return EINA_FALSE;
}

static Ender_Item * _egueb_dom_document_item_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document_Class *klass;
	Egueb_Dom_Document *thiz;
	Ender_Item *ret = NULL;

	thiz = EGUEB_DOM_DOCUMENT(n);
	klass = EGUEB_DOM_DOCUMENT_CLASS_GET(thiz);
	if (klass->item_get)
		ret = klass->item_get(thiz);
	if (!ret)
	{
		const Ender_Lib *lib;

		lib = ender_lib_find("egueb_dom");
		if (!lib) return NULL;

		ret = ender_lib_item_find(lib, "egueb.dom.document");
	}

	return ret;
}

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR,
		Egueb_Dom_Document, Egueb_Dom_Document_Class,
		egueb_dom_document)

static void _egueb_dom_document_class_init(void *k)
{
	Egueb_Dom_Node_Class *n_klass = EGUEB_DOM_NODE_CLASS(k);

	n_klass->type = EGUEB_DOM_NODE_TYPE_DOCUMENT;
	n_klass->child_appendable = _egueb_dom_document_child_appendable;
	n_klass->item_get = _egueb_dom_document_item_get;
}

static void _egueb_dom_document_instance_init(void *o)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_Node *n;

	n = EGUEB_DOM_NODE(o);
	/* a document is always in tree */
	n->in_tree = EINA_TRUE;

	/* register the needed events */
	/* in case the element is being inserted into another document or removed
	 * be sure to keep track of such changes
	 */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
			_egueb_dom_document_node_removed_from_document_cb,
			EINA_TRUE, n);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT,
			_egueb_dom_document_node_insterted_into_document_cb,
			EINA_TRUE, n);
	/* this events are needed to know whenever an element must be queued */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_dom_document_node_inserted_cb,
			EINA_FALSE, n);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_dom_document_node_removed_cb,
			EINA_FALSE, n);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_dom_document_attr_modified_cb,
			EINA_FALSE, n);
	/* in case an element needs to be processed, we will enqueue it */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_PROCESS,
			_egueb_dom_document_request_process_cb,
			EINA_FALSE, n);

 	thiz = EGUEB_DOM_DOCUMENT (o);
	/* add our private hash of ids */
	thiz->ids = eina_hash_string_superfast_new(EINA_FREE_CB(egueb_dom_node_unref));
}

static void _egueb_dom_document_instance_deinit(void *o)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_DOM_DOCUMENT(o);
	/* first remove the list */
	EINA_LIST_FREE(thiz->current_enqueued, n)
	{
		Egueb_Dom_String *name;

		name = egueb_dom_node_name_get(n);
		DBG("Removing the enqueued node '%s'", egueb_dom_string_string_get(name));
		egueb_dom_string_unref(name);
		egueb_dom_node_unref(n);
	}
	if (thiz->i)
		egueb_dom_implementation_unref(thiz->i);
	eina_hash_free(thiz->ids);
	thiz->ids = NULL;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int egueb_dom_document_current_run_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document *thiz;

	thiz = EGUEB_DOM_DOCUMENT(n);
	return thiz->current_run;
}

void egueb_dom_document_element_enqueue(Egueb_Dom_Node *n,
		Egueb_Dom_Node *node)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_String *name = NULL;

	if (egueb_dom_element_is_enqueued(node))
		goto done;

	thiz = EGUEB_DOM_DOCUMENT(n);
	name = egueb_dom_node_name_get(node);
	INFO("Node '%s' added to the list of damaged nodes",
			egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);
	thiz->current_enqueued = eina_list_append(thiz->current_enqueued,
			egueb_dom_node_ref(node));
done:
	egueb_dom_node_unref(node);
}

void egueb_dom_document_element_dequeue(Egueb_Dom_Node *n,
		Egueb_Dom_Node *node)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_String *name = NULL;
	Eina_List *l;

	if (!egueb_dom_element_is_enqueued(node))
		goto done;
	thiz = EGUEB_DOM_DOCUMENT(n);
	l = eina_list_data_find_list(thiz->current_enqueued, node);
	if (!l)
	{
		ERR("Enqueued?");
		goto done;
	}
	/* check if the data exists, so we can remove the reference */
	thiz->current_enqueued = eina_list_remove_list(
		thiz->current_enqueued, l);
	name = egueb_dom_node_name_get(node);
	INFO("Node '%s' removed from the list of damaged nodes",
			egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);
	egueb_dom_node_unref(node);
done:
	egueb_dom_node_unref(node);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/*
 * Introduced in DOM Level 2:
 * Element createElementNS(in DOMString namespaceURI,
 *   in DOMString qualifiedName)
 *   raises(DOMException);
 */
EAPI Egueb_Dom_Node * egueb_dom_document_element_ns_create(Egueb_Dom_Node *n,
		Egueb_Dom_String *ns_uri, Egueb_Dom_String *qname,
		Eina_Error *err)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_Document_Class *klass;
	Egueb_Dom_Node *new_element = NULL;
	Egueb_Dom_String *prefix = NULL;
	Egueb_Dom_String *local_name = NULL;

	/* split the prefix:local_name from the qualified name */
	if (!egueb_dom_qualified_name_resolve(qname, &prefix, &local_name))
	{
		ERR("Bad formed qualified name '%s'",
				egueb_dom_string_string_get(qname));
		*err = EGUEB_DOM_ERROR_NAMESPACE;
		return NULL;
	}

	/* if the element has a prefix but there is no namespace, error too */
	if (!egueb_dom_string_is_valid(ns_uri) &&
			egueb_dom_string_is_valid(prefix))
	{
		ERR("Prefix without a namespace on '%s'",
				egueb_dom_string_string_get(qname));
		*err = EGUEB_DOM_ERROR_NAMESPACE;
		return NULL;
		
	}

	klass = EGUEB_DOM_DOCUMENT_CLASS_GET(n);
	thiz = EGUEB_DOM_DOCUMENT(n);

	if (klass->element_create)
		new_element = klass->element_create(thiz,
				egueb_dom_string_string_get(ns_uri),
				egueb_dom_string_string_get(local_name));
	if (new_element)
	{
		egueb_dom_node_document_set(new_element, n);
	}
	else
	{
		if (err) *err = EGUEB_DOM_ERROR_INVALID_CHARACTER;
	}

	egueb_dom_string_unref(prefix);
	egueb_dom_string_unref(local_name);

	return new_element;
}

/*
 *  Element createElement(in DOMString tagName) raises(DOMException);
 */
EAPI Egueb_Dom_Node * egueb_dom_document_element_create(Egueb_Dom_Node *n,
		Egueb_Dom_String *name, Eina_Error *err)
{
	return egueb_dom_document_element_ns_create(n, NULL, name, err);
}

/* DocumentFragment   createDocumentFragment(); */
EAPI Egueb_Dom_Node * egueb_dom_document_document_fragment_create(
		Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *ret;

	ret = egueb_dom_document_fragment_new();
	egueb_dom_node_document_set(ret, n);
	return ret;
}

/**
 * Gets the topmost element of a document
 * (readonly attribute Element documentElement)
 *
 * @ender_prop{document_element}
 * @param[in] n The document to get the topmost element from
 * @return The document @ender_type{egueb.dom.element}
 */
EAPI Egueb_Dom_Node * egueb_dom_document_document_element_get(Egueb_Dom_Node *n)
{
	return egueb_dom_node_child_first_get(n);
}

/* attribute DOMString       documentURI; */
EAPI Egueb_Dom_String * egueb_dom_document_uri_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document *thiz;

	thiz = EGUEB_DOM_DOCUMENT(n);
	if (thiz->uri)
		return egueb_dom_string_ref(thiz->uri);
	else
		return NULL;
}

EAPI void egueb_dom_document_uri_set(Egueb_Dom_Node *n, Egueb_Dom_String *uri)
{
	Egueb_Dom_Document *thiz;

	thiz = EGUEB_DOM_DOCUMENT(n);
	if (thiz->uri)
		egueb_dom_string_unref(thiz->uri);
	thiz->uri = uri;
}

EAPI void egueb_dom_document_element_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *element)
{
	Egueb_Dom_Node *current;

	current = egueb_dom_document_document_element_get(n);
	if (current)
	{
		egueb_dom_node_child_remove(n, current, NULL);
		egueb_dom_node_unref(current);
	}
	egueb_dom_node_child_append(n, element, NULL);
}

/**
 * Gets the element whose id is the one povided
 * @param[in] n The document to get the element from
 * @param[in] id The id of the element to get
 * @param[out] err The error in case some exception happens
 * @return The element if found. NULL otherwise @ender_type{egueb.dom.element} @ender_transfer{full}
 */  
EAPI Egueb_Dom_Node * egueb_dom_document_element_get_by_id(Egueb_Dom_Node *n,
		Egueb_Dom_String *id, Eina_Error *err)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_Node *found;

	if (!egueb_dom_string_is_valid(id))
	{
		if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return NULL;
	}

	thiz = EGUEB_DOM_DOCUMENT(n);
	found = eina_hash_find(thiz->ids, id->str);
	if (!found)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
		return NULL;
	}

	return egueb_dom_node_ref(found);
}

EAPI Egueb_Dom_Node * egueb_dom_document_element_get_by_iri(Egueb_Dom_Node *n,
		Egueb_Dom_String *iri, Eina_Error *err)
{
	Egueb_Dom_Uri uri = { 0 };

	if (!egueb_dom_string_is_valid(iri))
		return NULL;

	/* resolve the uri for relative/absolute */
	DBG("Looking for %s", egueb_dom_string_string_get(iri));
	if (!egueb_dom_uri_iri_from(&uri, iri))
		return NULL;
	/* get the element by iri, only local ones for now */
	if (uri.location || !uri.fragment)
	{
		ERR("Unsupported iri '%s'", egueb_dom_string_string_get(iri));
		egueb_dom_uri_cleanup(&uri);
		return NULL;
	}
	else
	{
		Egueb_Dom_Node *ret;
		ret = egueb_dom_document_element_get_by_id(n, uri.fragment, err);
		egueb_dom_uri_cleanup(&uri);
		return ret;
	}
}

/* Introduced in DOM Level 3:
 * Node adoptNode(in Node source) raises(DOMException);
 */
EAPI Egueb_Dom_Node * egueb_dom_document_node_adopt(Egueb_Dom_Node *n, Egueb_Dom_Node *adopted, Eina_Error *err)
{
	Egueb_Dom_Node_Type type;
	Egueb_Dom_Node *other;

	type = egueb_dom_node_type_get(adopted);
	switch (type)
	{
		case EGUEB_DOM_NODE_TYPE_ELEMENT:
		/* be sure to remove it from its parent */
		other = egueb_dom_node_parent_get(adopted);
		if (other)
		{
			egueb_dom_node_child_remove(other, adopted, NULL);
			egueb_dom_node_unref(other);
		}
		egueb_dom_node_document_set_recursive(adopted, n);
		return adopted;
		break;

		default:
		if (err) *err = EGUEB_DOM_ERROR_NOT_SUPPORTED;
		egueb_dom_node_unref(adopted);
		return NULL;
		break;
	}
}

EAPI void egueb_dom_document_process(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document *thiz;
	Eina_List *current_enqueued;
	Eina_List *l, *l_next;

	thiz = EGUEB_DOM_DOCUMENT(n);

	DBG("Processing document");
	thiz->processing = EINA_TRUE;
	/* increment the current run */
	thiz->current_run++;
	/* process every enqueued node */
	current_enqueued = thiz->current_enqueued;
	thiz->current_enqueued = NULL;

	EINA_LIST_FOREACH_SAFE(current_enqueued, l, l_next, n)
	{
		Egueb_Dom_Element *e;

		e = EGUEB_DOM_ELEMENT(n);
		if (e->last_run == thiz->current_run)
		{
			DBG_ELEMENT(n, "Node already processed");
			goto skip;
		}

		if (!egueb_dom_element_is_enqueued(n))
		{
			WARN_ELEMENT(n, "Node not queued, skipping");
			goto skip;
		}

		INFO_ELEMENT(n, "Processing enqueued element");
		egueb_dom_element_process(n);
skip:
		current_enqueued = eina_list_remove_list(current_enqueued, l);

		egueb_dom_node_unref(n);
	}
	thiz->processing = EINA_FALSE;
}

EAPI Eina_Bool egueb_dom_document_is_processing(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document *thiz;

	if (!n) return EINA_FALSE;
	thiz = EGUEB_DOM_DOCUMENT(n);
	return thiz->processing;
}

EAPI Eina_Bool egueb_dom_document_needs_process(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document *thiz;

	thiz = EGUEB_DOM_DOCUMENT(n);
	if (thiz->current_enqueued)
		return EINA_TRUE;
	return EINA_FALSE;
}

EAPI void egueb_dom_document_process_queue_clear(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document *thiz;
	Eina_List *l, *l_next;

	thiz = EGUEB_DOM_DOCUMENT(n);
	/* process every enqueued node */
	EINA_LIST_FOREACH_SAFE(thiz->current_enqueued, l, l_next, n)
	{
		Egueb_Dom_Element *e;

		thiz->current_enqueued = eina_list_remove_list(thiz->current_enqueued, l);
		e = EGUEB_DOM_ELEMENT(n);
		e->enqueued = EINA_FALSE;
		egueb_dom_node_unref(n);
	}
}

#if 0
interface Document : Node {
  readonly attribute DocumentType     doctype;
  readonly attribute DOMImplementation  implementation;
  Text               createTextNode(in DOMString data);
  Comment            createComment(in DOMString data);
  CDATASection       createCDATASection(in DOMString data)
                                        raises(DOMException);
  ProcessingInstruction createProcessingInstruction(in DOMString target, 
                                                    in DOMString data)
                                        raises(DOMException);
  Attr               createAttribute(in DOMString name)
                                        raises(DOMException);
  EntityReference    createEntityReference(in DOMString name)
                                        raises(DOMException);
  NodeList           getElementsByTagName(in DOMString tagname);
  // Introduced in DOM Level 2:
  Node               importNode(in Node importedNode, 
                                in boolean deep)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  Attr               createAttributeNS(in DOMString namespaceURI, 
                                       in DOMString qualifiedName)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  NodeList           getElementsByTagNameNS(in DOMString namespaceURI, 
                                            in DOMString localName);

// Introduced in DOM Level 3:
  readonly attribute DOMString       inputEncoding;
  // Introduced in DOM Level 3:
  readonly attribute DOMString       xmlEncoding;
  // Introduced in DOM Level 3:
           attribute boolean         xmlStandalone;
                                        // raises(DOMException) on setting

  // Introduced in DOM Level 3:
           attribute DOMString       xmlVersion;
                                        // raises(DOMException) on setting

  // Introduced in DOM Level 3:
           attribute boolean         strictErrorChecking;
  // Introduced in DOM Level 3:
  readonly attribute DOMConfiguration domConfig;
  // Introduced in DOM Level 3:
  void               normalizeDocument();
  // Introduced in DOM Level 3:
  Node               renameNode(in Node n, 
                                in DOMString namespaceURI, 
                                in DOMString qualifiedName)
                                        raises(DOMException);
};

};

#endif
