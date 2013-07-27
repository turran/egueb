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
#include "egueb_dom_node_list.h"
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_node.h"
#include "egueb_dom_element.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_mutation.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_node_map_named_private.h"
#include "egueb_dom_document_private.h"
#include "egueb_dom_event_mutation_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Dom_Node_Event_Container
{
	Eina_List *listeners;
} Egueb_Dom_Node_Event_Container;

struct _Egueb_Dom_Node_Event_Listener
{
	Egueb_Dom_Event_Listener listener;
	Egueb_Dom_Node_Event_Container *container;
	Eina_Bool capture;
	void *data;
};

static void _egueb_dom_node_weak_ref_cb(Egueb_Dom_Event *e, void *data)
{
	Egueb_Dom_Node **weak_location = data;
	Egueb_Dom_Event_Phase phase;

	egueb_dom_event_phase_get(e, &phase);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	DBG("Weak reference destroyed, unsetting the pointer");
	*weak_location = NULL;
}

static void _egueb_dom_node_document_destroyed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *thiz = data;
	Egueb_Dom_Event_Phase phase;

	egueb_dom_event_phase_get(e, &phase);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;
	/* TODO shall we trigger the event document removed from? */
	thiz->owner_document = NULL;
}

static void _egueb_dom_node_event_container_free(void *d)
{
	Egueb_Dom_Node_Event_Container *thiz = d;
	Egueb_Dom_Node_Event_Listener *nl;

	EINA_LIST_FREE(thiz->listeners, nl)
	{
		free(nl);
	}
	free(thiz);
}

static void _egueb_dom_node_event_dispatch(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt)
{
	Egueb_Dom_Node_Event_Container *container;
	Egueb_Dom_Node_Event_Listener *nl;
	Eina_List *l;

	container = eina_hash_find(thiz->events,
		egueb_dom_string_string_get(evt->type));
	if (!container || !container->listeners)
		goto monitors;

	/* set the current target of the event */
	evt->current_target = thiz;

	EINA_LIST_FOREACH(container->listeners, l, nl)
	{
		if (nl->capture && (evt->phase ==
				EGUEB_DOM_EVENT_PHASE_CAPTURING))
		{
			if (evt->target == evt->current_target)
			{
				evt->phase = EGUEB_DOM_EVENT_PHASE_AT_TARGET;
				nl->listener(evt, nl->data);
				evt->phase = EGUEB_DOM_EVENT_PHASE_CAPTURING;
			}
			else
			{
				nl->listener(evt, nl->data);
			}
		}
		else if (!nl->capture && (evt->phase ==
				EGUEB_DOM_EVENT_PHASE_BUBBLING))
		{
			if (evt->target == evt->current_target)
			{
				evt->phase = EGUEB_DOM_EVENT_PHASE_AT_TARGET;
				nl->listener(evt, nl->data);
				evt->phase = EGUEB_DOM_EVENT_PHASE_BUBBLING;
			}
			else
			{
				nl->listener(evt, nl->data);
			}
		}
	}
monitors:
	/* now the monitors */
 	if (!thiz->monitors) return;

	EINA_LIST_FOREACH(thiz->monitors, l, nl)
	{
		if (evt->target == evt->current_target)
		{
			Egueb_Dom_Event_Phase old_phase = evt->phase;
			nl->listener(evt, nl->data);
			evt->phase = old_phase;
		}
		else
		{
			nl->listener(evt, nl->data);
		}
	}
}

static void _egueb_dom_node_event_capture(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt)
{
	Egueb_Dom_Node *parent = NULL;

	if (egueb_dom_node_parent_get(thiz, &parent) != EINA_ERROR_NONE)
		return;
	if (parent)
	{
		_egueb_dom_node_event_capture(parent, evt);
		egueb_dom_node_unref(parent);
	}
	if (!evt->stopped)
		_egueb_dom_node_event_dispatch(thiz, evt);
}

static void _egueb_dom_node_event_bubble(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt)
{
	Egueb_Dom_Node *parent = NULL;

	_egueb_dom_node_event_dispatch(thiz, evt);
	if (evt->stopped)
		return;
	if (egueb_dom_node_parent_get(thiz, &parent) != EINA_ERROR_NONE)
		return;
	if (parent)
	{
		_egueb_dom_node_event_bubble(parent, evt);
		egueb_dom_node_unref(parent);
	}
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(ENESIM_OBJECT_DESCRIPTOR, Egueb_Dom_Node,
		Egueb_Dom_Node_Class, egueb_dom_node);

static void _egueb_dom_node_class_init(void *k)
{
}

static void _egueb_dom_node_instance_init(void *o)
{
	Egueb_Dom_Node *thiz;

	thiz = EGUEB_DOM_NODE(o);
	thiz->ref = 1;
	thiz->events = eina_hash_string_superfast_new(
			_egueb_dom_node_event_container_free);
	thiz->user_data = eina_hash_string_superfast_new(NULL);
}

static void _egueb_dom_node_instance_deinit(void *o)
{
	Egueb_Dom_Node *thiz = EGUEB_DOM_NODE(o);
	Egueb_Dom_Event *event;
	Egueb_Dom_String *name = NULL;
	Egueb_Dom_Node_Event_Listener *el;

	thiz->destroying = EINA_TRUE;

	egueb_dom_node_name_get(thiz, &name);
	DBG("Destroying node '%s'", egueb_dom_string_string_get(name));
	/* we can not unref a node which has a parent and is not being
	 * destroyed
	 */
	if (thiz->parent && !thiz->parent->destroying)
	{
		ERR("Destroying the node '%s' with a parent. "
				"Use egueb_dom_node_child_remove()",
				egueb_dom_string_string_get(name));
	}
	egueb_dom_string_unref(name);
	/* remove the document weak ref */
	if (thiz->owner_document)
	{
		egueb_dom_node_weak_unref(thiz->owner_document,
			_egueb_dom_node_document_destroyed_cb, thiz);
		thiz->owner_document = NULL;
	}

	/* remove every child */
	while (thiz->children)
	{
		Egueb_Dom_Node *child;

		child = EINA_INLIST_CONTAINER_GET(thiz->children, Egueb_Dom_Node);
		egueb_dom_node_child_remove(thiz, child);
	}

	/* before freeing the element, call the destroy event */
	event = egueb_dom_event_mutation_new();
	egueb_dom_event_mutation_init_node_destroyed(event);
	egueb_dom_node_event_dispatch(thiz, event, NULL);
	/* remove the whole set of events */
	eina_hash_free(thiz->events);
	/* remove the monitors */
	EINA_LIST_FREE(thiz->monitors, el)
	{
		free(el);
	}
	/* and the user data */
	eina_hash_free(thiz->user_data);
}

/* we use this instead of an event listener to avoid allocation of a new
 * event per target
 */
static void _egueb_dom_node_document_set(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt, Egueb_Dom_Node *document)
{
	Egueb_Dom_Node *child;

	/* remove previous weak ref */
	if (thiz->owner_document)
	{
		egueb_dom_node_weak_unref(thiz->owner_document,
			_egueb_dom_node_document_destroyed_cb, thiz);
		thiz->owner_document = NULL;
	}
	/* add a wek ref */
	if (document)
	{
		egueb_dom_node_weak_ref(document,
			_egueb_dom_node_document_destroyed_cb, thiz);
		thiz->owner_document = document;
	}

	/* dispatch on the node first */
	egueb_dom_node_event_dispatch(thiz, egueb_dom_event_ref(evt), NULL);
	/* now on every children */
	EINA_INLIST_FOREACH(thiz->children, child)
	{
		_egueb_dom_node_document_set(child, egueb_dom_event_ref(evt),
				document);
	}
	egueb_dom_event_unref(evt);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_node_document_set(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node *document)
{
	Egueb_Dom_Event *event;

	if (document)
	{
		Egueb_Dom_Node_Type type;
		egueb_dom_node_type_get(document, &type);
		if (type != EGUEB_DOM_NODE_TYPE_DOCUMENT_NODE) return;
	}

	/* first remove */
	if ((document != thiz->owner_document) && (thiz->owner_document))
	{
		/* trigger the node removed from document mutation event */
		event = egueb_dom_event_mutation_new();
		egueb_dom_event_mutation_init_node_removed_from_document(event);
		_egueb_dom_node_document_set(thiz, event, NULL);
	}
	/* now add */
	if (document != thiz->owner_document)
	{
		/* trigger the node inserted into document mutation event */
		event = egueb_dom_event_mutation_new();
		egueb_dom_event_mutation_init_node_inserted_into_document(event);
		_egueb_dom_node_document_set(thiz, event, document);
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_dom_node_unref(Egueb_Dom_Node *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref && !thiz->destroying)
	{
		enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
	}
}

EAPI Egueb_Dom_Node * egueb_dom_node_ref(Egueb_Dom_Node *thiz)
{
	if (!thiz) return NULL;
	thiz->ref++;
	return thiz;
}

EAPI int egueb_dom_node_ref_get(Egueb_Dom_Node *thiz)
{
	if (!thiz) return -1;
	return thiz->ref;
}

EAPI void egueb_dom_node_weak_ref(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event_Listener l, void *data)
{
	if (!thiz) return;
	if (!l) return;

	egueb_dom_node_event_listener_add(thiz,
			EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
			l, EINA_FALSE, data);
}

EAPI void egueb_dom_node_weak_unref(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event_Listener l, void *data)
{
	if (!thiz) return;
	if (!l) return;

	egueb_dom_node_event_listener_remove(thiz,
			EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED,
			l, EINA_FALSE, data);
}

EAPI void egueb_dom_node_weak_ref_add(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node **weak_location)
{
	if (!thiz) return;
	if (!weak_location) return;

	egueb_dom_node_weak_ref(thiz, _egueb_dom_node_weak_ref_cb,
			weak_location);
	*weak_location = thiz;
}

EAPI void egueb_dom_node_weak_ref_remove(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node **weak_location)
{
	if (!thiz) return;
	if (!weak_location) return;

	egueb_dom_node_weak_unref(thiz, _egueb_dom_node_weak_ref_cb,
			weak_location);
	*weak_location = NULL;
}

/*
 * readonly attribute DOMString nodeName;
 */
EAPI Eina_Error egueb_dom_node_name_get(Egueb_Dom_Node *thiz, Egueb_Dom_String **name)
{
	Egueb_Dom_Node_Class *klass;

	if (!name) return EGUEB_DOM_ERROR_INVALID_ACCESS;
	klass = EGUEB_DOM_NODE_CLASS_GET(thiz);
	switch (klass->type)
	{
		case EGUEB_DOM_NODE_TYPE_ELEMENT_NODE:
		return egueb_dom_element_tag_name_get(thiz, name);
		break;

		case EGUEB_DOM_NODE_TYPE_TEXT_NODE:
		*name = egueb_dom_string_new_with_string("#text");
		break;

		case EGUEB_DOM_NODE_TYPE_CDATA_SECTION_NODE:
		*name = egueb_dom_string_new_with_string("#cdata-section");
		break;

		case EGUEB_DOM_NODE_TYPE_COMMENT_NODE:
		*name = egueb_dom_string_new_with_string("#comment");
		break;

		case EGUEB_DOM_NODE_TYPE_DOCUMENT_NODE:
		*name = egueb_dom_string_new_with_string("#document");
		break;

		case EGUEB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT_NODE:
		*name = egueb_dom_string_new_with_string("#document-fragment");
		break;

		case EGUEB_DOM_NODE_TYPE_ATTRIBUTE_NODE:
		return egueb_dom_attr_name_get(thiz, name);
		break;

		case EGUEB_DOM_NODE_TYPE_NOTATION_NODE:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_TYPE_NODE:
		case EGUEB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION_NODE:
		case EGUEB_DOM_NODE_TYPE_ENTITY_REFERENCE_NODE:
		case EGUEB_DOM_NODE_TYPE_ENTITY_NODE:
		default:
		return EGUEB_DOM_ERROR_NOT_SUPPORTED;
		break;
	}
	return EINA_ERROR_NONE;
}

/* attribute DOMString nodeValue;
 * raises(DOMException) on setting
 * raises(DOMException) on retrieval
 */
EAPI Eina_Error egueb_dom_node_value_get(Egueb_Dom_Node *thiz, Egueb_Dom_String **value)
{
	return EINA_ERROR_NONE;
}

/* Modified in DOM Level 2:
 * readonly attribute Document        ownerDocument;
 */
EAPI Eina_Error egueb_dom_node_document_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **owner)
{
	if (!thiz->owner_document) *owner = NULL;
	else *owner = egueb_dom_node_ref(EGUEB_DOM_NODE(thiz->owner_document));
	return EINA_ERROR_NONE;
}

/*
 * readonly attribute unsigned short nodeType;
 */
EAPI Eina_Error egueb_dom_node_type_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node_Type *type)
{
	Egueb_Dom_Node_Class *klass;

	klass = EGUEB_DOM_NODE_CLASS_GET(thiz);
	*type = klass->type;
	return EINA_ERROR_NONE;
}

/*
 * readonly attribute Node parentNode;
 */
EAPI Eina_Error egueb_dom_node_parent_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **parent)
{
	if (!thiz) return EGUEB_DOM_ERROR_NOT_FOUND;
	if (thiz->parent) *parent = egueb_dom_node_ref(thiz->parent);
	else *parent = NULL;
	return EINA_ERROR_NONE;
}

/*
 * readonly attribute NodeList childNodes;
 */
EAPI Eina_Error egueb_dom_node_children_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node_List *children)
{
	//*children = EINA_INLIST_CONTAINER_GET(thiz->children, Egueb_Dom_Tag);
	return EINA_ERROR_NONE;
}

EAPI Eina_Bool egueb_dom_node_children_foreach(Egueb_Dom_Node *thiz, Egueb_Dom_Node_Cb cb, void *data)
{
	Egueb_Dom_Node *child;
	Eina_Bool ret = EINA_TRUE;

	EINA_INLIST_FOREACH(thiz->children, child)
	{
		ret = cb(child, data);
		if (!ret) break;
	}
	return ret;
}

/*
 * readonly attribute Node firstChild;
 */
EAPI Eina_Error egueb_dom_node_child_first_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **first)
{
	Egueb_Dom_Node *other = NULL;

	if (!thiz->children) *first = NULL;
	else other = EINA_INLIST_CONTAINER_GET(thiz->children, Egueb_Dom_Node);

	if (other) *first = egueb_dom_node_ref(other);
	return EINA_ERROR_NONE;
}

/*
 * readonly attribute Node lastChild;
 */
EAPI Eina_Error egueb_dom_node_child_last_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **last)
{
	Egueb_Dom_Node *other = NULL;

	if (!thiz->children) *last = NULL;
	else other = EINA_INLIST_CONTAINER_GET(thiz->children->last, Egueb_Dom_Node);

	if (other) *last = egueb_dom_node_ref(other);
	return EINA_ERROR_NONE;
}

/*
 * readonly attribute Node previousSibling;
 */
EAPI Eina_Error egueb_dom_node_sibling_previous_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **sibling)
{
	Egueb_Dom_Node *other = NULL;
	Eina_Inlist *inlist;

	inlist = (EINA_INLIST_GET(thiz))->prev;
	if (!inlist) *sibling = NULL;
	else other = EINA_INLIST_CONTAINER_GET(inlist, Egueb_Dom_Node);

	if (other) *sibling = egueb_dom_node_ref(other);
	return EINA_ERROR_NONE;
}

/*
 * readonly attribute Node nextSibling;
 */
EAPI Eina_Error egueb_dom_node_sibling_next_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **sibling)
{
	Egueb_Dom_Node *other = NULL;
	Eina_Inlist *inlist;

	inlist = (EINA_INLIST_GET(thiz))->next;
	if (!inlist) *sibling = NULL;
	else other = EINA_INLIST_CONTAINER_GET(inlist, Egueb_Dom_Node);

	if (other) *sibling = egueb_dom_node_ref(other);
	return EINA_ERROR_NONE;
}

/*
 * readonly attribute NamedNodeMap attributes;
 */
EAPI Eina_Error egueb_dom_node_attributes_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node_Map_Named **map)
{
	if (!map) return EINA_ERROR_NONE;
	*map = egueb_dom_node_map_named_attr_new(thiz);
	return EINA_ERROR_NONE;
}

/*
 * Node removeChild(in Node oldChild) raises(DOMException);
 */
EAPI Eina_Error egueb_dom_node_child_remove(Egueb_Dom_Node *thiz, Egueb_Dom_Node *child)
{
	Egueb_Dom_Event *event;

	if (!thiz) return EGUEB_DOM_ERROR_NOT_FOUND;
	if (child->parent != thiz)
		return EGUEB_DOM_ERROR_NOT_FOUND;

	/* trigger the mutation event */
	event = egueb_dom_event_mutation_new();
	egueb_dom_event_mutation_init_node_removed(event, thiz);
	egueb_dom_node_event_dispatch(child, event, NULL);

	thiz->children = eina_inlist_remove(thiz->children, EINA_INLIST_GET(child));
	child->parent = NULL;
	egueb_dom_node_document_set(child, NULL);
	egueb_dom_node_unref(child);

	return EINA_ERROR_NONE;
}

/*
 * Node appendChild(in Node newChild)
 */
EAPI Eina_Error egueb_dom_node_child_append(Egueb_Dom_Node *thiz, Egueb_Dom_Node *child)
{
	return egueb_dom_node_insert_before(thiz, child, NULL);
}

/*
 * Modified in DOM Level 3:
 * Node               insertBefore(in Node newChild,
                                  in Node refChild)
                                        raises(DOMException);
 */
EAPI Eina_Error egueb_dom_node_insert_before(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node *child, Egueb_Dom_Node *ref)
{
	Egueb_Dom_Node_Class *klass;
	Egueb_Dom_Event *event;
	Eina_Bool appendable = EINA_TRUE;

	if (!thiz) return EGUEB_DOM_ERROR_INVALID_ACCESS;
	if (!child) return EGUEB_DOM_ERROR_INVALID_ACCESS;

	/* NOT_FOUND_ERR: Raised if refChild is not a child of this node. */
	if (ref && ref->parent != thiz) return EGUEB_DOM_ERROR_NOT_FOUND;

	if (child->owner_document && (thiz->owner_document != child->owner_document))
		return EGUEB_DOM_ERROR_WRONG_DOCUMENT;

	klass = EGUEB_DOM_NODE_CLASS_GET(thiz);
	if (klass->child_appendable)
		appendable = klass->child_appendable(thiz, child);

	if (!appendable) return EGUEB_DOM_ERROR_HIERARCHY_REQUEST;

	if (!ref)
		thiz->children = eina_inlist_append(thiz->children,
				EINA_INLIST_GET(child));
	else
		thiz->children = eina_inlist_prepend_relative(thiz->children,
				EINA_INLIST_GET(child), EINA_INLIST_GET(ref));
	child->parent = thiz;

	/* trigger the node inserted mutation event */
	event = egueb_dom_event_mutation_new();
	egueb_dom_event_mutation_init_node_inserted(event, thiz);
	egueb_dom_node_event_dispatch(child, event, NULL);

	/* set the owner document on the child */
	if (thiz->owner_document != child->owner_document)
		egueb_dom_node_document_set(child, thiz->owner_document);
	return EINA_ERROR_NONE;
}

/*  Node               cloneNode(in boolean deep); */
EAPI Eina_Error egueb_dom_node_clone(Egueb_Dom_Node *thiz, Eina_Bool live,
		Eina_Bool deep, Egueb_Dom_Node **clone)
{
	Egueb_Dom_Node_Class *klass;
	Enesim_Object_Descriptor *d;
	Enesim_Object_Class *k;

	if (!thiz) return EGUEB_DOM_ERROR_INVALID_ACCESS;
	/* create the object using the same descriptor and class */
	d = ENESIM_OBJECT_INSTANCE_DESCRIPTOR_GET(thiz);
	k = ENESIM_OBJECT_INSTANCE_CLASS(thiz);
	*clone = enesim_object_descriptor_instance_new(d, k);

	/* now call the implementation clone */
	klass = EGUEB_DOM_NODE_CLASS(k);
	if (klass->clone)
		klass->clone(thiz, live, deep, *clone);

	return EINA_ERROR_NONE;
}

/* void  addEventListener(in DOMString type,
         in EventListener listener,
         in boolean useCapture);
*/
EAPI Egueb_Dom_Node_Event_Listener * egueb_dom_node_event_listener_add(Egueb_Dom_Node *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data)
{
	Egueb_Dom_Node_Event_Listener *nl;
	Egueb_Dom_Node_Event_Container *container;
	const char *str;

	str = egueb_dom_string_string_get(type);
	container = eina_hash_find(thiz->events, str);
	if (!container)
	{
		container = calloc(1, sizeof(Egueb_Dom_Node_Event_Container));
		eina_hash_add(thiz->events, str, container);
	}

	nl = calloc(1, sizeof(Egueb_Dom_Node_Event_Listener));
	nl->listener = listener;
	nl->capture = capture;
	nl->data = data;
	nl->container = container;

	container->listeners = eina_list_append(container->listeners, nl);
	return nl;
}

/* void  removeEventListener(in DOMString type,
         in EventListener listener,
         in boolean useCapture);
*/
EAPI void egueb_dom_node_event_listener_remove(Egueb_Dom_Node *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data)
{
	Egueb_Dom_Node_Event_Listener *nl;
	Egueb_Dom_Node_Event_Container *container;
	Eina_List *l;
	const char *str;

	str = egueb_dom_string_string_get(type);
	container = eina_hash_find(thiz->events, str);
	if (!container || !container->listeners) return;

	EINA_LIST_FOREACH(container->listeners, l, nl)
	{
		if (nl->listener != listener || nl->capture != capture || nl->data != data)
			continue;
		free(nl);
		container->listeners = eina_list_remove_list(
				container->listeners, l);
		break;
	}
}

EAPI void egueb_dom_node_event_listener_free(Egueb_Dom_Node_Event_Listener *node_listener)
{
	Egueb_Dom_Node_Event_Container *container;

	container = node_listener->container;
	container->listeners = eina_list_remove(container->listeners,
			node_listener);
	free(node_listener);
}

EAPI Eina_Error egueb_dom_node_event_monitor_add(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event_Listener listener,
		void *data)
{
	Egueb_Dom_Node_Event_Listener *nl;

	nl = calloc(1, sizeof(Egueb_Dom_Node_Event_Listener));
	nl->listener = listener;
	nl->data = data;

	thiz->monitors = eina_list_append(thiz->monitors, nl);
	return EINA_ERROR_NONE;
}

EAPI void egueb_dom_node_event_monitor_remove(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event_Listener listener,
		void *data)
{
	Egueb_Dom_Node_Event_Listener *nl;
	Eina_List *l;

	EINA_LIST_FOREACH(thiz->monitors, l, nl)
	{
		if (nl->listener != listener || nl->data != data)
			continue;
		free(nl);
		thiz->monitors = eina_list_remove_list(thiz->monitors, l);
		break;
	}
}

/* boolean dispatchEvent(in Event evt) raises(EventException); */
EAPI Eina_Error egueb_dom_node_event_dispatch(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *event, Eina_Bool *notprevented)
{
	/* FIXME also if the string is empty */
	if (!event || !event->type) return EINA_ERROR_NONE;
	/* return EGUEB_DOM_EVENT_UNSPECIFIED_EVENT_TYPE_ERR; */

	if (event->dispatching) return EINA_ERROR_NONE;
	/* DISPATCH_REQUEST_ERR: Raised if the Event object is already being dispatched in the tree.*/

	/* NOT_SUPPORTED_ERR Raised if the Event object has not been created using DocumentEvent.createEvent() or does not support the interface CustomEvent */

	/* setup the event with the basic attributes */
	event->target = thiz;
	event->dispatching = EINA_TRUE;

	/* first the capture phase from all its parents */
	event->phase = EGUEB_DOM_EVENT_PHASE_CAPTURING;
	_egueb_dom_node_event_capture(thiz, event);
	/* finally the bubbling phase */
	event->phase = EGUEB_DOM_EVENT_PHASE_BUBBLING;
	_egueb_dom_node_event_bubble(thiz, event);

	event->dispatching = EINA_FALSE;
	egueb_dom_event_unref(event);
	return EINA_ERROR_NONE;
}

/* propagate an event in another tree */
EAPI Eina_Error egueb_dom_node_event_propagate(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *event)
{
	if (!event->dispatching) return EINA_ERROR_NONE;
	if (event->phase == EGUEB_DOM_EVENT_PHASE_CAPTURING)
		_egueb_dom_node_event_capture(thiz, event);
	else if (event->phase == EGUEB_DOM_EVENT_PHASE_BUBBLING)
		_egueb_dom_node_event_bubble(thiz, event);

	return EINA_ERROR_NONE;
}

/* Introduced in DOM Level 3:
 * DOMUserData        setUserData(in DOMString key, 
                                 in DOMUserData data, 
                                 in UserDataHandler handler);
 */
EAPI Eina_Error egueb_dom_node_user_data_set(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *key, void *data)
{
	const char *str;

	if (!key) return EGUEB_DOM_ERROR_NOT_FOUND;
	str = egueb_dom_string_string_get(key);
	if (!str) return EGUEB_DOM_ERROR_NOT_FOUND;
	if (!data)
		eina_hash_del_by_key(thiz->user_data, key);
	else
		eina_hash_add(thiz->user_data, str, data);
	return EINA_ERROR_NONE;
}

/* Introduced in DOM Level 3:
 * DOMUserData        getUserData(in DOMString key);
 */
EAPI void * egueb_dom_node_user_data_get(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *key)
{
	const char *str;

	if (!key) return NULL;
	str = egueb_dom_string_string_get(key);
	return eina_hash_find(thiz->user_data, str);
}

#if 0
// Introduced in DOM Level 2:
  interface EventListener {
    void               handleEvent(in Event evt);
  };


  // Modified in DOM Level 3:
  Node               replaceChild(in Node newChild, 
                                  in Node oldChild)
                                        raises(DOMException);
  boolean            hasChildNodes();
  // Modified in DOM Level 3:
  void               normalize();
  // Introduced in DOM Level 2:
  boolean            isSupported(in DOMString feature, 
                                 in DOMString version);
  // Introduced in DOM Level 2:
  readonly attribute DOMString       namespaceURI;
  // Introduced in DOM Level 2:
           attribute DOMString       prefix;
                                        // raises(DOMException) on setting

  // Introduced in DOM Level 2:
  readonly attribute DOMString       localName;
  // Introduced in DOM Level 2:
  boolean            hasAttributes();
  // Introduced in DOM Level 3:
  readonly attribute DOMString       baseURI;

  // DocumentPosition
  const unsigned short      DOCUMENT_POSITION_DISCONNECTED = 0x01;
  const unsigned short      DOCUMENT_POSITION_PRECEDING    = 0x02;
  const unsigned short      DOCUMENT_POSITION_FOLLOWING    = 0x04;
  const unsigned short      DOCUMENT_POSITION_CONTAINS     = 0x08;
  const unsigned short      DOCUMENT_POSITION_CONTAINED_BY = 0x10;
  const unsigned short      DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC = 0x20;

  // Introduced in DOM Level 3:
  unsigned short     compareDocumentPosition(in Node other)
                                        raises(DOMException);
  // Introduced in DOM Level 3:
           attribute DOMString       textContent;
                                        // raises(DOMException) on setting
                                        // raises(DOMException) on retrieval

  // Introduced in DOM Level 3:
  boolean            isSameNode(in Node other);
  // Introduced in DOM Level 3:
  DOMString          lookupPrefix(in DOMString namespaceURI);
  // Introduced in DOM Level 3:
  boolean            isDefaultNamespace(in DOMString namespaceURI);
  // Introduced in DOM Level 3:
  DOMString          lookupNamespaceURI(in DOMString prefix);
  // Introduced in DOM Level 3:
  boolean            isEqualNode(in Node arg);
  // Introduced in DOM Level 3:
  DOMObject          getFeature(in DOMString feature, 
                                in DOMString version);

#endif
