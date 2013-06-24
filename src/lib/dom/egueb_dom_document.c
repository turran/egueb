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
#include "egueb_dom_document.h"
#include "egueb_dom_element.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_mutation.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_element_private.h"
#include "egueb_dom_document_private.h"

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
static void _egueb_dom_document_mutation_attr_modified_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT(data);
	Egueb_Dom_Node *target;
	Egueb_Dom_String *attr_name = NULL;

	DBG("Node attr modified");
	egueb_dom_event_target_get(ev, &target);

	/* check if the attribute is the id */
	egueb_dom_event_mutation_attr_name_get(ev, &attr_name);
	if (egueb_dom_string_is_valid(attr_name) &&
			(!strcmp(egueb_dom_string_string_get(attr_name), "id")))
	{
		Egueb_Dom_String *attr_val = NULL;

		/* FIXME to avoid finding the node, we need to know the old
		 * id, and for that we need to copy the previous data
		 * which is maybe slower than this? we need to test
		 */
		eina_hash_del_by_data(thiz->ids, target);

		egueb_dom_event_mutation_value_new_string_get(ev, &attr_val);
		if (egueb_dom_string_is_valid(attr_val))
		{
			DBG("Adding id '%s' to the list of ids", attr_val->str);
			eina_hash_add(thiz->ids,attr_val->str, egueb_dom_node_ref(target));
		}
	}
	if (!egueb_dom_event_mutation_process_prevented(ev))
		egueb_dom_element_enqueue_process(target);
	else
		egueb_dom_node_unref(target);
}

static void _egueb_dom_document_mutation_node_inserted_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT(data);
	Egueb_Dom_Node *target;
	Egueb_Dom_Node_Type type;
	Egueb_Dom_String *name;

	egueb_dom_event_target_get(ev, &target);
	egueb_dom_node_type_get(target, &type);

	egueb_dom_node_name_get(target, &name);
	DBG("Node '%s' inserted", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	/* addr the element to the ids */
	if (type == EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
	{
		Egueb_Dom_String *id = NULL;
		Egueb_Dom_String id_attr = EGUEB_DOM_STRING_STATIC("id");

		egueb_dom_element_attribute_get(target, &id_attr, &id);
		if (egueb_dom_string_is_valid(id))
		{
			DBG("Adding id '%s' to the list of ids", id->str);
			eina_hash_add(thiz->ids,
					egueb_dom_string_string_get(id),
					egueb_dom_node_ref(target));
		}
		egueb_dom_string_unref(id);
	}

	if (!egueb_dom_event_mutation_process_prevented(ev))
		egueb_dom_node_unref(target);
	else
		egueb_dom_element_enqueue_process(target);
}

static void _egueb_dom_document_mutation_node_removed_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Document *thiz = EGUEB_DOM_DOCUMENT(data);
	Egueb_Dom_Node *target;
	Egueb_Dom_String *name;
	Egueb_Dom_Node_Type type;

	egueb_dom_event_target_get(ev, &target);
	egueb_dom_node_type_get(target, &type);

	egueb_dom_node_name_get(target, &name);
	DBG("Node '%s' removed", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	/* remove the element from the ids */
	if (type == EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
	{
		Egueb_Dom_String *id = NULL;
		Egueb_Dom_String id_attr = EGUEB_DOM_STRING_STATIC("id");

		egueb_dom_element_attribute_get(target, &id_attr, &id);
		if (egueb_dom_string_is_valid(id))
		{
			DBG("Removing id '%s' from the list of ids", id->str);
			eina_hash_del(thiz->ids,
					egueb_dom_string_string_get(id),
					target);
		}
		egueb_dom_string_unref(id);
	}

	if (!egueb_dom_event_mutation_process_prevented(ev))
		egueb_dom_document_dequeue_process(thiz, egueb_dom_node_ref(target));
	egueb_dom_node_unref(target);
}

static void _egueb_dom_document_element_insterted_into_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Document *thiz = data;
	Egueb_Dom_Node *target;
	Egueb_Dom_Event_Phase phase;

	egueb_dom_event_phase_get(ev, &phase);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	egueb_dom_event_target_get(ev, &target);
	/* register the needed events */
	egueb_dom_node_event_listener_add(target,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_dom_document_mutation_node_inserted_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(target,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_dom_document_mutation_node_removed_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(target,
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_dom_document_mutation_attr_modified_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_unref(target);
}

static void _egueb_dom_document_element_removed_from_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Document *thiz = data;
	Egueb_Dom_Event_Phase phase;

	egueb_dom_event_phase_get(ev, &phase);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	/* remove the events registered */
	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT,
			_egueb_dom_document_element_insterted_into_document_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
			_egueb_dom_document_element_removed_from_document_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_dom_document_mutation_node_inserted_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_dom_document_mutation_node_removed_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_remove(thiz->element,
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_dom_document_mutation_attr_modified_cb,
			EINA_TRUE, thiz);
	/* update our own state */
	egueb_dom_node_unref(thiz->element);
	thiz->element = NULL;
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
	Egueb_Dom_Document_Class *klass = EGUEB_DOM_DOCUMENT_CLASS(k);

	n_klass->type = EGUEB_DOM_NODE_TYPE_DOCUMENT_NODE;
	/* set default methods */
	klass->process = egueb_dom_document_process_default;
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
		egueb_dom_node_document_set(thiz->element, NULL);
	}
	eina_hash_free(thiz->ids);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_document_process_default(Egueb_Dom_Document *thiz)
{
	Egueb_Dom_Node *n;
	Eina_List *l, *l_next;

	/* process every enqueued node */
	EINA_LIST_FOREACH_SAFE(thiz->current_enqueued, l, l_next, n)
	{
		Egueb_Dom_String *name = NULL;
		Egueb_Dom_Element *e;

		egueb_dom_node_name_get(n, &name);
		INFO("Processing '%s'", egueb_dom_string_string_get(name));
		egueb_dom_string_unref(name);

		egueb_dom_element_process(n);
		thiz->current_enqueued = eina_list_remove_list(thiz->current_enqueued, l);
		e = EGUEB_DOM_ELEMENT(n);
		e->enqueued = EINA_FALSE;

		egueb_dom_node_unref(n);
	}
	/* TODO it might be possible that a process of a node implies
	 * a new change
	 */
}

void egueb_dom_document_enqueue_process(Egueb_Dom_Document *thiz,
		Egueb_Dom_Node *node)
{
	Egueb_Dom_Element *e;
	Egueb_Dom_String *name = NULL;

	e = EGUEB_DOM_ELEMENT(node);
	egueb_dom_node_name_get(node, &name);
	INFO("Node '%s' added to the list of damaged nodes",
			egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);
	e->enqueued = EINA_TRUE;
	thiz->current_enqueued = eina_list_append(thiz->current_enqueued, node);
}

void egueb_dom_document_dequeue_process(Egueb_Dom_Document *thiz,
		Egueb_Dom_Node *node)
{
	Eina_List *l;

	l = eina_list_data_find_list(thiz->current_enqueued, node);
	/* check if the data exists, so we can remove the reference */
	if (l)
	{
		Egueb_Dom_Element *e;
		Egueb_Dom_String *name = NULL;

		e = EGUEB_DOM_ELEMENT(node);
		thiz->current_enqueued = eina_list_remove_list(
				thiz->current_enqueued, l);
		egueb_dom_node_name_get(node, &name);
		INFO("Node '%s' removed from the list of damaged nodes",
				egueb_dom_string_string_get(name));
		egueb_dom_string_unref(name);
		egueb_dom_node_unref(node);
		e->enqueued = EINA_FALSE;
	}
	egueb_dom_node_unref(node);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/*
 *  Element createElement(in DOMString tagName) raises(DOMException);
 */
EAPI Eina_Error egueb_dom_document_element_create(Egueb_Dom_Node *n,
		Egueb_Dom_String *name, Egueb_Dom_Node **ret)
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
		/* TODO keep track of the document inserted event */
		egueb_dom_node_document_set(new_element, EGUEB_DOM_NODE(thiz));
		if (!thiz->element)
		{
			egueb_dom_document_element_set(n, new_element);
		}
	}
	*ret = new_element;

	return EINA_ERROR_NONE;
}

/* readonly attribute Element documentElement; */
EAPI Eina_Error egueb_dom_document_element_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **element)
{
	Egueb_Dom_Document *thiz;

	thiz = EGUEB_DOM_DOCUMENT(n);
	if (element && thiz->element)
		*element = egueb_dom_node_ref(thiz->element);
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_dom_document_element_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *element)
{
	Egueb_Dom_Document *thiz;

	thiz = EGUEB_DOM_DOCUMENT(n);
	if (thiz->element)
	{
		egueb_dom_node_document_set(thiz->element, NULL);
		egueb_dom_node_unref(thiz->element);
		thiz->element = NULL;
	}
	thiz->element = element;
	if (element)
	{
		/* in case the element is being inserted into another document or removed
		 * be sure to keep track of such changes
		 */
		egueb_dom_node_event_listener_add(element,
				EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT,
				_egueb_dom_document_element_insterted_into_document_cb,
				EINA_FALSE, thiz);
		egueb_dom_node_event_listener_add(element,
				EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
				_egueb_dom_document_element_removed_from_document_cb,
				EINA_FALSE, thiz);

		egueb_dom_node_document_set(element, n);
		/* add the element to the process list */
		egueb_dom_document_enqueue_process(thiz, egueb_dom_node_ref(element));
	}

	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_dom_document_element_get_by_id(Egueb_Dom_Node *n,
		Egueb_Dom_String *id, Egueb_Dom_Node **element)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_Node *found;

	if (!element) return EGUEB_DOM_ERROR_INVALID_ACCESS;
	if (!egueb_dom_string_is_valid(id))
		return EGUEB_DOM_ERROR_INVALID_ACCESS;

	thiz = EGUEB_DOM_DOCUMENT(n);
	found = eina_hash_find(thiz->ids, id->str);
	if (!found) return EGUEB_DOM_ERROR_NOT_FOUND;

	*element = egueb_dom_node_ref(found);
	return EINA_ERROR_NONE;
}

/* Introduced in DOM Level 3:
 * Node adoptNode(in Node source) raises(DOMException);
 */
EAPI Eina_Error egueb_dom_document_node_adopt(Egueb_Dom_Node *n, Egueb_Dom_Node *adopted, Egueb_Dom_Node **ret)
{
	Egueb_Dom_Node_Type type;
	Eina_Error err = EINA_ERROR_NONE;

	egueb_dom_node_type_get(adopted, &type);
	switch (type)
	{
		case EGUEB_DOM_NODE_TYPE_ELEMENT_NODE:
		/* TODO be sure to remove it from its parent */
		egueb_dom_node_document_set(adopted, n);
		break;

		default:
		err = EGUEB_DOM_ERROR_NOT_SUPPORTED;
		break;
	}
	if (err == EINA_ERROR_NONE)
	{
		if (ret) *ret = egueb_dom_node_ref(adopted);
	}
	else
	{
		if (ret) *ret = NULL;
	}
	egueb_dom_node_unref(adopted);

	return err;
}

EAPI void egueb_dom_document_process(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document *thiz;
	Egueb_Dom_Document_Class *klass;

	thiz = EGUEB_DOM_DOCUMENT(n);
	klass = EGUEB_DOM_DOCUMENT_CLASS_GET(n);

	thiz->processing = EINA_TRUE;
	if (klass->process)
		klass->process(thiz);
	thiz->processing = EINA_FALSE;
}

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
