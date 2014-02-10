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
#include "egueb_dom_element.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_mutation.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_element_private.h"
#include "egueb_dom_document_private.h"

#include "egueb_dom_event_process_private.h"

#if 0
thiz->entities = eina_hash_string_superfast_new(NULL);
/* add common entities */
eina_hash_add(thiz->entities, "quot", strdup("\""));
eina_hash_add(thiz->entities, "amp", strdup("&"));
eina_hash_add(thiz->entities, "apos", strdup("'"));
eina_hash_add(thiz->entities, "lt", strdup("<"));
eina_hash_add(thiz->entities, "gt", strdup(">"));
thiz->string = eina_strbuf_new();
eina_hash_free(thiz->entities);
eina_strbuf_free(thiz->string);

static const char * _egueb_dom_parser_get_entity(Egueb_Dom_Parser *thiz, const char *s, const char **e)
{
	const char *tmp;
	char entity[128];
	int len;

	tmp = s;
	while (*tmp != ';')
		tmp++;
	len = tmp - s - 1;
	strncpy(entity, s + 1, len);
	entity[len] = '\0';
	*e = tmp;

	return eina_hash_find(thiz->entities, entity);
}

static Eina_Bool _egueb_dom_parser_transform_text(Egueb_Dom_Parser *thiz, const char *text, unsigned int len)
{
	Eina_Bool transformed = EINA_FALSE;
	const char *last = NULL;
	const char *s = text;

	/* until eos */
	if (!len) len = strlen(text);
	while (s < text + len && *s)
	{
		if (*s == '&')
		{
			const char *entity;
			const char *e;
			entity = _egueb_dom_parser_get_entity(thiz, s, &e);
			if (!entity)
			{
				s++;
				continue;
			}
			DBG("Entity '%s' found", entity);
			if (!transformed)
			{
				eina_strbuf_reset(thiz->string);
				eina_strbuf_append_length(thiz->string, text, s - text);
				transformed = EINA_TRUE;
			}
			if (last)
				eina_strbuf_append_length(thiz->string, last, s - last);
			eina_strbuf_append(thiz->string, entity);
			last = e + 1;
			s = e;
		}
		s++;
	}
	if (last)
	{
		eina_strbuf_append_length(thiz->string, last, s - last);
	}

	return transformed;
}
#endif
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* Forward declarations */
static void _egueb_dom_document_topmost_remove_events(Egueb_Dom_Document *thiz);

static void _egueb_dom_document_element_enqueue(Egueb_Dom_Document *thiz,
		Egueb_Dom_Node *node)
{
	Egueb_Dom_String *name = NULL;

	if (egueb_dom_element_is_enqueued(node))
		goto done;
	name = egueb_dom_node_name_get(node);
	INFO("Node '%s' added to the list of damaged nodes",
			egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	egueb_dom_element_enqueue(node);
	thiz->current_enqueued = eina_list_append(thiz->current_enqueued,
			egueb_dom_node_ref(node));
done:
	egueb_dom_node_unref(node);
}

static void _egueb_dom_document_element_dequeue(Egueb_Dom_Document *thiz,
		Egueb_Dom_Node *node)
{
	Eina_List *l;
	Egueb_Dom_String *name = NULL;

	if (!egueb_dom_element_is_enqueued(node))
		goto done;
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
	egueb_dom_element_dequeue(node);
	egueb_dom_node_unref(node);
done:
	egueb_dom_node_unref(node);
}

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

	if (egueb_dom_string_is_valid(id))
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

static void _egueb_dom_document_topmost_attr_modified_cb(Egueb_Dom_Event *ev,
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
	/* whenever an attribute has changed, trigger the request process */
	egueb_dom_element_request_process(target);
	egueb_dom_node_unref(target);
}

/* a node has been inserted into its parent */
static void _egueb_dom_document_topmost_node_inserted_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Node *target;
	Egueb_Dom_Node_Type type;
	Egueb_Dom_String *name;

	target = egueb_dom_event_target_get(ev);
	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
	{
		egueb_dom_node_unref(target);
		return;
	}

	name = egueb_dom_node_name_get(target);
	DBG("Node '%s' inserted", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	/* request a process */
	egueb_dom_element_request_process(target);
	egueb_dom_node_unref(target);
}

static void _egueb_dom_document_topmost_node_removed_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT(data);
	Egueb_Dom_Node *target;
	Egueb_Dom_Node_Type type;
	Egueb_Dom_String *name;

	target = egueb_dom_event_target_get(ev);
	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
	{
		egueb_dom_node_unref(target);
		return;
	}

	name = egueb_dom_node_name_get(target);
	DBG("Node '%s' removed", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	/* remove the element from the queue */
	_egueb_dom_document_element_dequeue(thiz, target);
}

static void _egueb_dom_document_element_insterted_into_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT(data);
	Egueb_Dom_Event_Phase phase;
	Egueb_Dom_Node *target;
	Egueb_Dom_Node_Type type;
	Egueb_Dom_String *id = NULL;
	Egueb_Dom_String id_attr = EGUEB_DOM_STRING_STATIC("id");
	Egueb_Dom_String *name;

	phase = egueb_dom_event_phase_get(ev);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	target = egueb_dom_event_target_get(ev);
	name = egueb_dom_node_name_get(target);
	INFO("Node '%s' inserted into the document", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
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

static void _egueb_dom_document_element_removed_from_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT(data);
	Egueb_Dom_Event_Phase phase;
	Egueb_Dom_Node *target;
	Egueb_Dom_Node_Type type;
	Egueb_Dom_String *id = NULL;
	Egueb_Dom_String id_attr = EGUEB_DOM_STRING_STATIC("id");
	Egueb_Dom_String *name;

	phase = egueb_dom_event_phase_get(ev);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	target = egueb_dom_event_target_get(ev);
	name = egueb_dom_node_name_get(target);
	INFO("Node '%s' removed from the document", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
	{
		egueb_dom_node_unref(target);
		return;
	}

	/* remove the element from the ids */
	id = egueb_dom_element_attribute_get(target, &id_attr);
	if (egueb_dom_string_is_valid(id))
	{
		INFO("Removing id '%s' from the list of ids", id->str);
		eina_hash_del(thiz->ids,
				egueb_dom_string_string_get(id),
				target);
	}
	egueb_dom_string_unref(id);
	/* remove the events */
	egueb_dom_node_event_listener_remove(target,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT,
			_egueb_dom_document_element_insterted_into_document_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_remove(target,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
			_egueb_dom_document_element_removed_from_document_cb,
			EINA_TRUE, thiz);

	egueb_dom_node_unref(target);
}

static void _egueb_dom_document_topmost_request_process_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Document *thiz = data;
	Egueb_Dom_Node *target;

	DBG("Requesting process");
	target = egueb_dom_event_target_get(ev);
	/* Add it to the list in case it is not already there */
	_egueb_dom_document_element_enqueue(thiz, target);
}

static void _egueb_dom_document_topmost_removed_from_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Document *thiz = data;
	Egueb_Dom_Event_Phase phase;

	phase = egueb_dom_event_phase_get(ev);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	/* remove the events registered */
	_egueb_dom_document_topmost_remove_events(thiz);
	/* update our own state */
	egueb_dom_node_unref(thiz->element);
	thiz->element = NULL;
}

static void _egueb_dom_document_topmost_remove_events(Egueb_Dom_Document *thiz)
{
	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
			_egueb_dom_document_topmost_removed_from_document_cb,
			EINA_TRUE, thiz);

	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_dom_document_topmost_node_inserted_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_dom_document_topmost_node_removed_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_dom_document_topmost_attr_modified_cb,
			EINA_TRUE, thiz);

	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_PROCESS,
			_egueb_dom_document_topmost_request_process_cb,
			EINA_FALSE, thiz);
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR,
		Egueb_Dom_Document, Egueb_Dom_Document_Class,
		egueb_dom_document)

static void _egueb_dom_document_class_init(void *k)
{
	Egueb_Dom_Node_Class *n_klass = EGUEB_DOM_NODE_CLASS(k);

	n_klass->type = EGUEB_DOM_NODE_TYPE_DOCUMENT_NODE;
}

static void _egueb_dom_document_instance_init(void *o)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT (o);
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
		egueb_dom_node_unref(n);
	}
	if (thiz->element)
	{
		/* Be sure to remove the topmost event handlers
		 * before setting the NULL document. This is done because
		 * we keep one reference of the topmost. When the element
		 * is being removed from the document it is also unreffed
		 * and thus a problem on the other event handlers because
		 * the node will be no longer valid
		 */
		_egueb_dom_document_topmost_remove_events(thiz);
		egueb_dom_node_document_set(thiz->element, NULL);
		egueb_dom_node_unref(thiz->element);
		thiz->element = NULL;
	}
	eina_hash_free(thiz->ids);
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
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/*
 *  Element createElement(in DOMString tagName) raises(DOMException);
 */
EAPI Egueb_Dom_Node * egueb_dom_document_element_create(Egueb_Dom_Node *n,
		Egueb_Dom_String *name, Eina_Error *err)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_Document_Class *klass;
	Egueb_Dom_Node *new_element = NULL;
	const char *str;

	klass = EGUEB_DOM_DOCUMENT_CLASS_GET(n);
	thiz = EGUEB_DOM_DOCUMENT(n);

	str = egueb_dom_string_string_get(name);
	if (klass->element_create)
		new_element = klass->element_create(thiz, str);
	if (new_element)
	{
		/* keep track of the document inserted event */
		egueb_dom_node_event_listener_add(new_element,
				EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT,
				_egueb_dom_document_element_insterted_into_document_cb,
				EINA_TRUE, thiz);
		egueb_dom_node_event_listener_add(new_element,
				EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
				_egueb_dom_document_element_removed_from_document_cb,
				EINA_TRUE, thiz);
		egueb_dom_node_document_set(new_element, EGUEB_DOM_NODE(thiz));
	}
	else
	{
		if (err) *err = EGUEB_DOM_ERROR_INVALID_CHARACTER;
	}
	return new_element;
}

/* readonly attribute Element documentElement; */
EAPI Egueb_Dom_Node * egueb_dom_document_element_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document *thiz;

	thiz = EGUEB_DOM_DOCUMENT(n);
	return egueb_dom_node_ref(thiz->element);
}

EAPI void egueb_dom_document_element_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *element)
{
	Egueb_Dom_Document *thiz;

	thiz = EGUEB_DOM_DOCUMENT(n);
	if (thiz->element)
	{
		egueb_dom_node_document_set(thiz->element, NULL);
	}
	if (element)
	{
		egueb_dom_node_document_set(element, n);
		thiz->element = element;
		/* register the needed events */
		/* in case the element is being inserted into another document or removed
		 * be sure to keep track of such changes
		 */
		egueb_dom_node_event_listener_add(element,
				EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
				_egueb_dom_document_topmost_removed_from_document_cb,
				EINA_TRUE, thiz);
		/* this events are needed to know whenever an element must be queued */
		egueb_dom_node_event_listener_add(element,
				EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
				_egueb_dom_document_topmost_node_inserted_cb,
				EINA_TRUE, thiz);
		egueb_dom_node_event_listener_add(element,
				EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
				_egueb_dom_document_topmost_node_removed_cb,
				EINA_TRUE, thiz);
		egueb_dom_node_event_listener_add(element,
				EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
				_egueb_dom_document_topmost_attr_modified_cb,
				EINA_TRUE, thiz);
		/* in case an element needs to be processed, we will enqueue it */
		egueb_dom_node_event_listener_add(element,
				EGUEB_DOM_EVENT_PROCESS,
				_egueb_dom_document_topmost_request_process_cb,
				EINA_FALSE, thiz);
		/* add the element to the process list */
		_egueb_dom_document_element_enqueue(thiz, egueb_dom_node_ref(element));
	}
}

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

/* TODO move the svg version here */
EAPI Egueb_Dom_Node * egueb_dom_document_element_get_by_iri(Egueb_Dom_Node *n,
		Egueb_Dom_String *id, Eina_Error *err)
{
	/* import the node in case it belongs to other document */
	return NULL;
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
		case EGUEB_DOM_NODE_TYPE_ELEMENT_NODE:
		/* be sure to remove it from its parent */
		other = egueb_dom_node_parent_get(adopted);
		if (other)
		{
			egueb_dom_node_child_remove(other, adopted, NULL);
			egueb_dom_node_unref(other);
		}
		egueb_dom_node_document_set(adopted, n);
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
	Eina_List *l, *l_next;

	thiz = EGUEB_DOM_DOCUMENT(n);

	thiz->processing = EINA_TRUE;
	/* increment the current run */
	thiz->current_run++;
	/* process every enqueued node */
	EINA_LIST_FOREACH_SAFE(thiz->current_enqueued, l, l_next, n)
	{
		Egueb_Dom_String *name = NULL;
		Egueb_Dom_Element *e;

		e = EGUEB_DOM_ELEMENT(n);
		if (e->last_run == thiz->current_run)
			goto dequeue;

		if (!egueb_dom_element_is_enqueued(n))
			goto skip;

		name = egueb_dom_node_name_get(n);
		INFO("Processing '%s'", egueb_dom_string_string_get(name));
		egueb_dom_string_unref(name);

		egueb_dom_element_process(n);
dequeue:
		egueb_dom_element_dequeue(n);
skip:
		thiz->current_enqueued = eina_list_remove_list(thiz->current_enqueued, l);

		egueb_dom_node_unref(n);
	}
	/* TODO it might be possible that a process of a node implies
	 * a new change
	 */
	thiz->processing = EINA_FALSE;
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
EAPI void egueb_dom_document_timer_add_cb_set(Egueb_Dom_Node *n,
		Egueb_Dom_Document_Timer_Add_Cb cb, void *user_data)
{

}

EAPI void egueb_dom_document_timer_remove_cb_set(Egueb_Dom_Node *n,
		Egueb_Dom_Document_Timer_Remove_Cb cb, void *user_data)
{

}

EAPI Egueb_Dom_Timer * egueb_dom_document_timer_add(Egueb_Dom_Node *n,
		long timeout, Egueb_Dom_Document_Timer_Cb cb, void *user_data)
{

}

EAPI void egueb_dom_document_timer_remove(Egueb_Dom_Node *n, Egueb_Dom_Timer *t)
{

}
#endif

#if 0
interface Document : Node {
  readonly attribute DocumentType     doctype;
  readonly attribute DOMImplementation  implementation;
  DocumentFragment   createDocumentFragment();
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
  Element            createElementNS(in DOMString namespaceURI, 
                                     in DOMString qualifiedName)
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
           attribute DOMString       documentURI;
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
