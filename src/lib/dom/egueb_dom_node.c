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
#include "egueb_dom_list.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_node.h"
#include "egueb_dom_element.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_mutation.h"
#include "egueb_dom_feature.h"
#include "egueb_dom_document.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_node_list_private.h"
#include "egueb_dom_node_map_named_private.h"
#include "egueb_dom_element_private.h"
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

typedef struct _Egueb_Dom_Node_Feature
{
	Egueb_Dom_String *version;
	Egueb_Dom_Feature *feature;
} Egueb_Dom_Node_Feature;

static void _egueb_dom_node_feature_free(void *d)
{
	Egueb_Dom_Node_Feature *thiz;
	Eina_List *features = d;

	EINA_LIST_FREE(features, thiz)
	{
		if (thiz->version)
			egueb_dom_string_unref(thiz->version);
		if (thiz->feature)
			egueb_dom_feature_unref(thiz->feature);
		free(thiz);
	}
}

static Egueb_Dom_String * _egueb_dom_node_ns_prefix_lookup(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *ns_uri, Egueb_Dom_Node *from)
{
	Egueb_Dom_Node *ancestor;
	Egueb_Dom_String *ret;

	if (thiz->namespace_uri && egueb_dom_string_is_equal(thiz->namespace_uri, ns_uri) &&
			thiz->prefix)
	{
		Egueb_Dom_String *from_ns_uri;
		Eina_Bool is_equal;

		from_ns_uri = egueb_dom_node_namespace_uri_lookup(from, thiz->prefix);
		is_equal = egueb_dom_string_is_equal(from_ns_uri, ns_uri);
		egueb_dom_string_unref(from_ns_uri);

		if (is_equal)
		{
			return egueb_dom_string_ref(thiz->prefix);
		}
	}

#if 0
        if ( Element has attributes)
        { 
            for ( all DOM Level 2 valid local namespace declaration attributes of Element )
            {
                if (Attrs prefix == "xmlns" and 
                   Attrs value == namespaceURI and 
                   originalElement.lookupNamespaceURI(Attrs localname) == namespaceURI) 
                   { 
                      return (Attrs localname);
                   } 
            }
        } 
#endif
	ancestor = egueb_dom_node_get_ancestor_element(thiz);
	ret = _egueb_dom_node_ns_prefix_lookup(ancestor, ns_uri, from);
	egueb_dom_node_unref(ancestor);

	return ret;
}

static void _egueb_dom_node_weak_ref_cb(Egueb_Dom_Event *e, void *data)
{
	Egueb_Dom_Node **weak_location = data;
	Egueb_Dom_Event_Phase phase;

	phase = egueb_dom_event_phase_get(e);
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

	phase = egueb_dom_event_phase_get(e);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;
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
				EGUEB_DOM_EVENT_PHASE_CAPTURING ||
				evt->phase == EGUEB_DOM_EVENT_PHASE_AT_TARGET))
		{
			nl->listener(evt, nl->data);
		}
		else if (!nl->capture && (evt->phase ==
				EGUEB_DOM_EVENT_PHASE_BUBBLING ||
				evt->phase == EGUEB_DOM_EVENT_PHASE_AT_TARGET))
		{
			nl->listener(evt, nl->data);
		}
	}
monitors:
	/* now the monitors */
	if (thiz->monitors)
	{
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
	/* unset the target */
	evt->current_target = NULL;
}

static void _egueb_dom_node_event_capture(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt, Egueb_Dom_Node *from)
{
	Egueb_Dom_Node *parent = NULL;

	parent = egueb_dom_node_parent_get(thiz);
	if (parent)
	{
		_egueb_dom_node_event_capture(parent, evt, thiz);
		egueb_dom_node_unref(parent);
	}

	if (!evt->stopped)
	{
		evt->relative = from;
		_egueb_dom_node_event_dispatch(thiz, evt);
		return;
	}
	DBG("Event '%s' stopped", egueb_dom_string_string_get(evt->type));
}

static void _egueb_dom_node_event_bubble(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt)
{
	Egueb_Dom_Node *parent = NULL;

	_egueb_dom_node_event_dispatch(thiz, evt);
	if (evt->stopped)
	{
		DBG("Event '%s' stopped", egueb_dom_string_string_get(evt->type));
		return;
	}
	parent = egueb_dom_node_parent_get(thiz);
	if (parent)
	{
		evt->relative = thiz;
		_egueb_dom_node_event_bubble(parent, evt);
		egueb_dom_node_unref(parent);
	}
}

static void _egueb_dom_node_event_start_capturing(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt)
{
	Egueb_Dom_Node *parent = NULL;

	if (!evt->capturable)
		return;

	if (evt->stopped)
	{
		DBG("Event '%s' stopped", egueb_dom_string_string_get(evt->type));
		return;
	}

	parent = egueb_dom_node_parent_get(thiz);
	if (parent)
	{
		evt->phase = EGUEB_DOM_EVENT_PHASE_CAPTURING;
		_egueb_dom_node_event_capture(parent, evt, thiz);
		egueb_dom_node_unref(parent);
	}
}

static void _egueb_dom_node_event_start_bubbling(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt)
{
	Egueb_Dom_Node *parent = NULL;

	if (!evt->bubbleable)
		return;

	if (evt->stopped)
	{
		DBG("Event '%s' stopped", egueb_dom_string_string_get(evt->type));
		return;
	}
	parent = egueb_dom_node_parent_get(thiz);
	if (parent)
	{
		evt->phase = EGUEB_DOM_EVENT_PHASE_BUBBLING;
		evt->relative = thiz;
		_egueb_dom_node_event_bubble(parent, evt);
		egueb_dom_node_unref(parent);
	}
}

static void _egueb_dom_node_event_start_at_target(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt)
{
	if (evt->stopped)
	{
		DBG("Event '%s' stopped", egueb_dom_string_string_get(evt->type));
		return;
	}
	DBG("Dispatching event '%s' at target",
			egueb_dom_string_string_get(evt->type));
	evt->phase = EGUEB_DOM_EVENT_PHASE_AT_TARGET;
	_egueb_dom_node_event_dispatch(thiz, evt);
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
	thiz->features = eina_hash_string_superfast_new(_egueb_dom_node_feature_free);
}

static void _egueb_dom_node_instance_deinit(void *o)
{
	Egueb_Dom_Node *thiz = EGUEB_DOM_NODE(o);
	Egueb_Dom_Node_Event_Listener *el;

	/* remove the whole set of events */
	eina_hash_free(thiz->events);
	/* remove the monitors */
	EINA_LIST_FREE(thiz->monitors, el)
	{
		free(el);
	}
	/* and the user data */
	eina_hash_free(thiz->user_data);
	/* and the features */
	eina_hash_free(thiz->features);
}

static void _egueb_dom_node_insert_into_document(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt)
{
	Egueb_Dom_Node *parent;
	Egueb_Dom_Node *child;

	parent = thiz->parent;
	if (egueb_dom_node_type_get(parent) == EGUEB_DOM_NODE_TYPE_DOCUMENT)
	{
		/* in case the parent is a document, set it */
		egueb_dom_node_document_set(thiz, parent);
	}
	else if ((parent->owner_document != thiz->owner_document) && (parent->owner_document))
	{
		/* in case the parent has a document and we dont, set the owner document */
		egueb_dom_node_document_set(thiz, parent->owner_document);
	}

	thiz->in_tree = EINA_TRUE;
	/* dispatch on the node first */
	egueb_dom_node_event_dispatch(thiz, egueb_dom_event_ref(evt), NULL, NULL);
	/* now on every children */
	EINA_INLIST_FOREACH(thiz->children, child)
	{
		_egueb_dom_node_insert_into_document(child, egueb_dom_event_ref(evt));
	}
	egueb_dom_event_unref(evt);
}

static void _egueb_dom_node_remove_from_document(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *evt)
{
	Egueb_Dom_Node *child;

	/* remove the weak reference in case we have one */
	egueb_dom_node_document_set(thiz, NULL);

	thiz->in_tree = EINA_FALSE;
	/* dispatch on the node first */
	egueb_dom_node_event_dispatch(thiz, egueb_dom_event_ref(evt), NULL, NULL);
	/* now on every children */
	EINA_INLIST_FOREACH(thiz->children, child)
	{
		_egueb_dom_node_remove_from_document(child, egueb_dom_event_ref(evt));
	}
	egueb_dom_event_unref(evt);
}

static void _egueb_dom_node_free(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_String *name = NULL;
	Egueb_Dom_Event *event;

	name = egueb_dom_node_name_get(thiz);
	DBG("Destroying node '%s'", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	thiz->destroying = EINA_TRUE;

	/* we can not unref a node which has a parent and is not being
	 * destroyed
	 */
	if (thiz->parent && !thiz->parent->destroying)
	{
		CRIT("Destroying the node with a parent. "
				"Use egueb_dom_node_child_remove()");
	}

	/* remove every child */
	while (thiz->children)
	{
		Eina_Error err;
		Egueb_Dom_Node *child;

		child = EINA_INLIST_CONTAINER_GET(thiz->children, Egueb_Dom_Node);
		if (!egueb_dom_node_child_remove(thiz, child, &err))
		{
			ERR("Failed removing a child (%s), bad things will happen",
					eina_error_msg_get(err));
		}
	}

	/* before freeing the element, call the destroy event */
	event = egueb_dom_event_mutation_node_destroyed_new();
	egueb_dom_node_event_dispatch(thiz, event, NULL, NULL);
	/* remove the document weak ref */
	if (thiz->owner_document)
	{
		egueb_dom_node_weak_unref(thiz->owner_document,
			_egueb_dom_node_document_destroyed_cb, thiz);
		thiz->owner_document = NULL;
	}

	/* finally free the enesim instance */
	enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_node_document_set(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node *document)
{
	if (document)
	{
		Egueb_Dom_Node_Type type;
		type = egueb_dom_node_type_get(document);
		if (type != EGUEB_DOM_NODE_TYPE_DOCUMENT) return;
	}

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
}

void egueb_dom_node_document_set_recursive(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node *document)
{
	Egueb_Dom_Node *child;

	egueb_dom_node_document_set(thiz, document);
	EINA_INLIST_FOREACH(thiz->children, child)
	{
		egueb_dom_node_document_set_recursive(child, document);
	}
}

Eina_Bool egueb_dom_node_feature_add(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *name, Egueb_Dom_String *version,
		Egueb_Dom_Feature *feature)
{
	Egueb_Dom_Node_Feature *f;
	Eina_List *features;
	const char *str;

	if (!name) return EINA_FALSE;
	str = egueb_dom_string_string_get(name);

	f = calloc(1, sizeof(Egueb_Dom_Node_Feature));
	if (version)
		f->version = egueb_dom_string_ref(version);
	f->feature = feature;

	features = eina_hash_find(thiz->features, str);
	if (!features)
	{
		features = eina_list_append(features, f);
		eina_hash_add(thiz->features, str, features);
	}
	else
	{
		features = eina_list_append(features, f);
	}
	return EINA_TRUE;
}

Egueb_Dom_Node * egueb_dom_node_get_ancestor_element(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_Node *ancestor;

	ancestor = egueb_dom_node_parent_get(thiz);
	while (ancestor)
	{
		Egueb_Dom_Node *tmp;

		if (egueb_dom_node_type_get(ancestor) == EGUEB_DOM_NODE_TYPE_ELEMENT)
		{
			return ancestor;
		}
		tmp = egueb_dom_node_parent_get(ancestor);
		egueb_dom_node_unref(tmp);
		ancestor = tmp;
	}
	return NULL;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_dom_node_is_destroying(Egueb_Dom_Node *thiz)
{
	if (!thiz) return EINA_FALSE;
	return thiz->destroying;
}

EAPI void egueb_dom_node_unref(Egueb_Dom_Node *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref && !thiz->destroying)
	{
		_egueb_dom_node_free(thiz);
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
EAPI Egueb_Dom_String * egueb_dom_node_name_get(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_Node_Class *klass;

	klass = EGUEB_DOM_NODE_CLASS_GET(thiz);
	switch (klass->type)
	{
		case EGUEB_DOM_NODE_TYPE_ELEMENT:
		return egueb_dom_element_tag_name_get(thiz);
		break;

		case EGUEB_DOM_NODE_TYPE_TEXT:
		return egueb_dom_string_new_with_string("#text");
		break;

		case EGUEB_DOM_NODE_TYPE_CDATA_SECTION:
		return egueb_dom_string_new_with_string("#cdata-section");
		break;

		case EGUEB_DOM_NODE_TYPE_COMMENT:
		return egueb_dom_string_new_with_string("#comment");
		break;

		case EGUEB_DOM_NODE_TYPE_DOCUMENT:
		return egueb_dom_string_new_with_string("#document");
		break;

		case EGUEB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
		return egueb_dom_string_new_with_string("#document-fragment");
		break;

		case EGUEB_DOM_NODE_TYPE_ATTRIBUTE:
		return egueb_dom_attr_name_get(thiz);
		break;

		case EGUEB_DOM_NODE_TYPE_NOTATION:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_TYPE:
		case EGUEB_DOM_NODE_TYPE_PROCESSING_INSTRUCTION:
		case EGUEB_DOM_NODE_TYPE_ENTITY_REFERENCE:
		case EGUEB_DOM_NODE_TYPE_ENTITY:
		default:
		return NULL;
		break;
	}
}

/* attribute DOMString nodeValue;
 * raises(DOMException) on setting
 * raises(DOMException) on retrieval
 */
EAPI Egueb_Dom_String * egueb_dom_node_value_get(Egueb_Dom_Node *thiz)
{
	return NULL;
}

/**
 * Gets the document associtaed with this node
 * (readonly attribute Document ownerDocument)
 *
 * @ender_prop{owner_document}
 * @param[in] thiz The node to get the document from
 * @return The document @ender_type{egueb.dom.document}
 */
EAPI Egueb_Dom_Node * egueb_dom_node_owner_document_get(Egueb_Dom_Node *thiz)
{
	return egueb_dom_node_ref(EGUEB_DOM_NODE(thiz->owner_document));
}

/*
 * readonly attribute unsigned short nodeType;
 */
EAPI Egueb_Dom_Node_Type egueb_dom_node_type_get(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_Node_Class *klass;

	klass = EGUEB_DOM_NODE_CLASS_GET(thiz);
	return klass->type;
}

/*
 * readonly attribute Node parentNode;
 */
EAPI Egueb_Dom_Node * egueb_dom_node_parent_get(Egueb_Dom_Node *thiz)
{
	if (!thiz) return NULL;
	return egueb_dom_node_ref(thiz->parent);
}

/**
 * Get the children nodes of a node
 * (readonly attribute NodeList childNodes)
 * @ender_prop{child_nodes}
 * @param[in] thiz The node to get the list of children nodes from
 * @return The children nodes
 */
EAPI Egueb_Dom_Node_List * egueb_dom_node_child_nodes_get(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_Node *child;
	Eina_List *tmp = NULL;

	EINA_INLIST_FOREACH(thiz->children, child)
	{
		tmp = eina_list_append(tmp, egueb_dom_node_ref(child));
	}
	return egueb_dom_node_list_new(tmp);
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
EAPI Egueb_Dom_Node * egueb_dom_node_child_first_get(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_Node *other = NULL;

	if (!thiz->children) return NULL;
	else other = EINA_INLIST_CONTAINER_GET(thiz->children, Egueb_Dom_Node);

	if (other) return egueb_dom_node_ref(other);
	return NULL;
}

/*
 * readonly attribute Node lastChild;
 */
EAPI Egueb_Dom_Node * egueb_dom_node_child_last_get(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_Node *other = NULL;

	if (!thiz->children) return NULL;
	else other = EINA_INLIST_CONTAINER_GET(thiz->children->last, Egueb_Dom_Node);

	return egueb_dom_node_ref(other);
}

/*
 * readonly attribute Node previousSibling;
 */
EAPI Egueb_Dom_Node * egueb_dom_node_sibling_previous_get(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_Node *other = NULL;
	Eina_Inlist *inlist;

	inlist = (EINA_INLIST_GET(thiz))->prev;
	if (!inlist) return NULL;
	else other = EINA_INLIST_CONTAINER_GET(inlist, Egueb_Dom_Node);

	return egueb_dom_node_ref(other);
}

/*
 * readonly attribute Node nextSibling;
 */
EAPI Egueb_Dom_Node * egueb_dom_node_sibling_next_get(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_Node *other = NULL;
	Eina_Inlist *inlist;

	inlist = (EINA_INLIST_GET(thiz))->next;
	if (!inlist) return NULL;
	else other = EINA_INLIST_CONTAINER_GET(inlist, Egueb_Dom_Node);

	return egueb_dom_node_ref(other);
}

/*
 * readonly attribute NamedNodeMap attributes;
 */
EAPI Egueb_Dom_Node_Map_Named * egueb_dom_node_attributes_get(Egueb_Dom_Node *thiz)
{
	return egueb_dom_node_map_named_attr_new(thiz);
}

/*
 * Node removeChild(in Node oldChild) raises(DOMException);
 */
EAPI Eina_Bool egueb_dom_node_child_remove(Egueb_Dom_Node *thiz, Egueb_Dom_Node *child,
		Eina_Error *err)
{
	Egueb_Dom_Event *event;

	if (!thiz)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
		egueb_dom_node_unref(child);
		return EINA_FALSE;
	}

	if (child->parent != thiz)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
		egueb_dom_node_unref(child);
		return EINA_FALSE;
	}

	/* remove the node in the tree in case the parent is on the tree */
	if (thiz->in_tree && child->in_tree)
	{
		event = egueb_dom_event_mutation_node_removed_from_document_new();
		_egueb_dom_node_remove_from_document(child, event); 
	}

	/* trigger the mutation event */
	event = egueb_dom_event_mutation_node_removed_new(egueb_dom_node_ref(thiz));
	egueb_dom_node_event_dispatch(child, event, NULL, NULL);

	thiz->children = eina_inlist_remove(thiz->children, EINA_INLIST_GET(child));
	child->parent = NULL;
	egueb_dom_node_unref(child);

	return EINA_TRUE;
}

/*
 * Node appendChild(in Node newChild) raises(DOMException)
 */
EAPI Eina_Bool egueb_dom_node_child_append(Egueb_Dom_Node *thiz, Egueb_Dom_Node *child,
		Eina_Error *err)
{
	return egueb_dom_node_insert_before(thiz, child, NULL, err);
}

/*
 * Modified in DOM Level 3:
 * Node               insertBefore(in Node newChild,
                                  in Node refChild)
                                        raises(DOMException);
 */
EAPI Eina_Bool egueb_dom_node_insert_before(Egueb_Dom_Node *thiz,
		Egueb_Dom_Node *child, Egueb_Dom_Node *ref, Eina_Error *err)
{
	Egueb_Dom_Node_Class *klass;
	Egueb_Dom_Event *event;
	Eina_Bool appendable = EINA_TRUE;

	if (!thiz)
	{
		if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}
	if (!child)
	{
		if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}

	/* NOT_FOUND_ERR: Raised if refChild is not a child of this node. */
	if (ref && ref->parent != thiz)
	{
		if (err) *err = EGUEB_DOM_ERROR_NOT_FOUND;
		return EINA_FALSE;
	}

	if (child->owner_document)
	{
		Egueb_Dom_Node_Type type;

		type = egueb_dom_node_type_get(thiz);
 		if (type == EGUEB_DOM_NODE_TYPE_DOCUMENT)
		{
			if (child->owner_document != thiz)
			{
				if (err) *err = EGUEB_DOM_ERROR_WRONG_DOCUMENT;
				return EINA_FALSE;
			}
		}
		else
		{
			if (thiz->owner_document != child->owner_document)
			{
				if (err) *err = EGUEB_DOM_ERROR_WRONG_DOCUMENT;
				return EINA_FALSE;
			}
		}
	}

	klass = EGUEB_DOM_NODE_CLASS_GET(thiz);
	if (klass->child_appendable)
		appendable = klass->child_appendable(thiz, child);

	if (!appendable)
	{
		if (err) *err = EGUEB_DOM_ERROR_HIERARCHY_REQUEST;
		return EINA_FALSE;
	}

	if (!ref)
		thiz->children = eina_inlist_append(thiz->children,
				EINA_INLIST_GET(child));
	else
		thiz->children = eina_inlist_prepend_relative(thiz->children,
				EINA_INLIST_GET(child), EINA_INLIST_GET(ref));
	child->parent = thiz;

	/* trigger the node inserted mutation event */
	event = egueb_dom_event_mutation_node_inserted_new(egueb_dom_node_ref(thiz));
	egueb_dom_node_event_dispatch(child, event, NULL, NULL);

	/* insert the node in the tree in case the parent is on the tree too,
	 * the document will be set too in case the child does not have one
	 */
	if (thiz->in_tree && !child->in_tree)
	{
		event = egueb_dom_event_mutation_node_inserted_into_document_new();
		_egueb_dom_node_insert_into_document(child, event); 
	}
	/* otherwise, check if the parent has a doc and the child doesnt, if that's
	 * the case, the child needs to be part of the document too
	 */
	else if (!child->owner_document)
	{
		Egueb_Dom_Node_Type type;

		type = egueb_dom_node_type_get(thiz);
		if (type == EGUEB_DOM_NODE_TYPE_DOCUMENT)
		{
			egueb_dom_node_document_set_recursive(child, thiz);
		}
		else if (thiz->owner_document)
		{
			egueb_dom_node_document_set_recursive(child, thiz->owner_document);
		}
	}
	return EINA_TRUE;
}

/*  Node               cloneNode(in boolean deep); */
EAPI Egueb_Dom_Node * egueb_dom_node_clone(Egueb_Dom_Node *thiz, Eina_Bool live,
		Eina_Bool deep, Eina_Error *err)
{
	Egueb_Dom_Node_Class *klass;
	Egueb_Dom_Node *ret;
	Enesim_Object_Descriptor *d;
	Enesim_Object_Class *k;

	if (!thiz)
	{
		if (err) *err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return NULL;
	}

	/* create the object using the same descriptor and class */
	d = ENESIM_OBJECT_INSTANCE_DESCRIPTOR_GET(thiz);
	k = ENESIM_OBJECT_INSTANCE_CLASS(thiz);
	ret = enesim_object_descriptor_instance_new(d, k);

	/* now call the implementation clone */
	klass = EGUEB_DOM_NODE_CLASS(k);
	if (klass->clone)
		klass->clone(thiz, live, deep, ret);

	return ret;
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

EAPI void egueb_dom_node_event_monitor_add(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event_Listener listener,
		void *data)
{
	Egueb_Dom_Node_Event_Listener *nl;

	nl = calloc(1, sizeof(Egueb_Dom_Node_Event_Listener));
	nl->listener = listener;
	nl->data = data;

	thiz->monitors = eina_list_append(thiz->monitors, nl);
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

/**
 * Dispatch an event in a node
 * @param[in] thiz The even to dispatch the event into
 * @param[in] event The event to dispatch
 * @param[out] notprevented In case the event has not been prevented
 * @param[out] err The exception in case the dispatch fails
 * @return EINA_TRUE if the disptach succeeds, EINA_FALSE otherwise.
 */
EAPI Eina_Bool egueb_dom_node_event_dispatch(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *event, Eina_Bool *notprevented,
		Eina_Error *err)
{
	/* FIXME also if the string is empty */
	if (!event || !event->type)
	{
		if (event)
			egueb_dom_event_unref(event);
		if (err)
			*err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}
	if (event->dispatching)
	{
		egueb_dom_event_unref(event);
		/* return EGUEB_DOM_EVENT_UNSPECIFIED_EVENT_TYPE_ERR; */
		if (err)
			*err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}
	if (thiz->freezed)
	{
		egueb_dom_event_unref(event);
		return EINA_FALSE;
	}

	/* DISPATCH_REQUEST_ERR: Raised if the Event object is already being dispatched in the tree.*/
	/* NOT_SUPPORTED_ERR Raised if the Event object has not been created using DocumentEvent.createEvent() or does not support the interface CustomEvent */

	/* setup the event with the basic attributes */
	event->target = egueb_dom_node_ref(thiz);
	event->dispatching = EINA_TRUE;

	if (event->direction == EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE)
	{
		/* first the capture phase from all its parents */
		_egueb_dom_node_event_start_capturing(thiz, event);
		_egueb_dom_node_event_start_at_target(thiz, event);
		/* finally the bubbling phase */
		_egueb_dom_node_event_start_bubbling(thiz, event);
	}
	else
	{
		/* first the bubbling phase */
		_egueb_dom_node_event_start_bubbling(thiz, event);
		/* finally the capture phase from all its parents */
		_egueb_dom_node_event_start_capturing(thiz, event);
		_egueb_dom_node_event_start_at_target(thiz, event);
	}

	event->dispatching = EINA_FALSE;
	egueb_dom_event_unref(event);
	return EINA_TRUE;
}

/* propagate an event in another tree */
EAPI Eina_Bool egueb_dom_node_event_propagate(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *event)
{
	if (!event->dispatching) return EINA_FALSE;
	if (event->phase == EGUEB_DOM_EVENT_PHASE_CAPTURING)
	{
		_egueb_dom_node_event_capture(thiz, event, NULL);
	}
	else if (event->phase == EGUEB_DOM_EVENT_PHASE_BUBBLING)
	{
		_egueb_dom_node_event_bubble(thiz, event);
	}

	return EINA_FALSE;
}

/* Introduced in DOM Level 3:
 * DOMUserData        setUserData(in DOMString key, 
                                 in DOMUserData data, 
                                 in UserDataHandler handler);
 */
EAPI void egueb_dom_node_user_data_set(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *key, void *data)
{
	const char *str;

	if (!key) return;
	str = egueb_dom_string_string_get(key);
	if (!str) return;
	if (!data)
		eina_hash_del_by_key(thiz->user_data, key);
	else
		eina_hash_add(thiz->user_data, str, data);
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

/* Introduced in DOM Level 2:
 * boolean            isSupported(in DOMString feature, 
 *                                in DOMString version);
 */
EAPI Eina_Bool egueb_dom_node_is_supported(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *name, Egueb_Dom_String *version)
{
	Eina_List *features;
	const char *str;

	if (!name) return EINA_FALSE;
	str = egueb_dom_string_string_get(name);

	features = eina_hash_find(thiz->features, str);
	if (!features) return EINA_FALSE;
	return EINA_TRUE;
}

/* Introduced in DOM Level 3:
 * DOMObject          getFeature(in DOMString feature, 
 *                               in DOMString version);
 */
EAPI void * egueb_dom_node_feature_get(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *name, Egueb_Dom_String *version)
{
	Egueb_Dom_Node_Feature *f = NULL;
	Eina_List *features;
	const char *str;

	if (!name) return NULL;
	str = egueb_dom_string_string_get(name);

	features = eina_hash_find(thiz->features, str);
	if (!features) return NULL;

	if (!version)
	{
		/* pick the first one */
		f = eina_list_data_get(features);
	}
	else
	{
		Egueb_Dom_Node_Feature *f_tmp;
		Eina_List *l;

		/* pick the matching one */
		EINA_LIST_FOREACH(features, l, f_tmp)
		{
			if (egueb_dom_string_is_equal(f->version, version))
			{
				f = f_tmp;
				break;
			}
		}
	}
	if (!f) return NULL;

	if (!f->feature)
		return egueb_dom_node_ref(thiz);
	else
		return egueb_dom_feature_ref(f->feature);
}

/* Introduced in DOM Level 3:
 * DOMString          lookupPrefix(in DOMString namespaceURI);
 */
EAPI Egueb_Dom_String * egueb_dom_node_prefix_lookup(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *ns_uri)
{
	Egueb_Dom_String *ret = NULL;
	Egueb_Dom_Node *tmp;

	if (!thiz)
		return NULL;

	if (!egueb_dom_string_is_valid(ns_uri))
		return NULL;

	switch (egueb_dom_node_type_get(thiz))
	{
		case EGUEB_DOM_NODE_TYPE_ELEMENT:
		ret = _egueb_dom_node_ns_prefix_lookup(thiz, ns_uri, thiz);
		break;

		case EGUEB_DOM_NODE_TYPE_DOCUMENT:
		tmp = egueb_dom_document_document_element_get(thiz);
             	ret = egueb_dom_node_prefix_lookup(tmp, ns_uri);
		egueb_dom_node_unref(tmp);
		break;

		break;

		case EGUEB_DOM_NODE_TYPE_ATTRIBUTE:
		tmp = egueb_dom_attr_owner_get(thiz);
             	ret = egueb_dom_node_prefix_lookup(tmp, ns_uri);
		egueb_dom_node_unref(tmp);
		break;
		
		case EGUEB_DOM_NODE_TYPE_ENTITY:
		case EGUEB_DOM_NODE_TYPE_NOTATION:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_TYPE:
		return NULL;

		default:
		tmp = egueb_dom_node_get_ancestor_element(thiz);
		ret = egueb_dom_node_prefix_lookup(tmp, ns_uri);
		egueb_dom_node_unref(tmp);
		break;
	}
	return ret;
} 

/* Introduced in DOM Level 3:
 * boolean            isDefaultNamespace(in DOMString namespaceURI);
 */

EAPI Eina_Bool egueb_dom_node_is_default_namespace(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *ns_uri)
{
	Egueb_Dom_Node *tmp;
	Eina_Bool ret = EINA_FALSE;

	if (!thiz)
		return EINA_FALSE;

	switch (egueb_dom_node_type_get(thiz))
	{
		case EGUEB_DOM_NODE_TYPE_ELEMENT:
		ret = egueb_dom_element_is_default_namespace(thiz, ns_uri);
		break;

		case EGUEB_DOM_NODE_TYPE_DOCUMENT:
		tmp = egueb_dom_document_document_element_get(thiz);
		ret = egueb_dom_element_is_default_namespace(tmp, ns_uri);
		egueb_dom_node_unref(tmp);
		break;

		case EGUEB_DOM_NODE_TYPE_ATTRIBUTE:
		tmp = egueb_dom_attr_owner_get(thiz);
		ret = egueb_dom_element_is_default_namespace(tmp, ns_uri);
		egueb_dom_node_unref(tmp);
		break;

		case EGUEB_DOM_NODE_TYPE_ENTITY:
		case EGUEB_DOM_NODE_TYPE_NOTATION:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_TYPE:
		break;

		default:
		tmp = egueb_dom_node_get_ancestor_element(thiz);
		ret = egueb_dom_element_is_default_namespace(tmp, ns_uri);
		egueb_dom_node_unref(tmp);
		break;
	}
	return ret;
}

/* Introduced in DOM Level 3:
 * DOMString          lookupNamespaceURI(in DOMString prefix);
 */
EAPI Egueb_Dom_String * egueb_dom_node_namespace_uri_lookup(Egueb_Dom_Node *thiz,
		Egueb_Dom_String *prefix)
{
	Egueb_Dom_String *ret = NULL;
	Egueb_Dom_Node *tmp;

	if (!thiz)
		return NULL;

	switch (egueb_dom_node_type_get(thiz))
	{
		case EGUEB_DOM_NODE_TYPE_ELEMENT:
		ret = egueb_dom_element_namespace_uri_lookup(thiz, prefix);
		break;

		case EGUEB_DOM_NODE_TYPE_DOCUMENT:
		tmp = egueb_dom_document_document_element_get(thiz);
		ret = egueb_dom_node_namespace_uri_lookup(tmp, prefix);
		egueb_dom_node_unref(tmp);
		break;

		case EGUEB_DOM_NODE_TYPE_ATTRIBUTE:
		tmp = egueb_dom_attr_owner_get(thiz);
		ret = egueb_dom_node_namespace_uri_lookup(tmp, prefix);
		egueb_dom_node_unref(tmp);
		break;

		case EGUEB_DOM_NODE_TYPE_ENTITY:
		case EGUEB_DOM_NODE_TYPE_NOTATION:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_FRAGMENT:
		case EGUEB_DOM_NODE_TYPE_DOCUMENT_TYPE:
		break;

		default:
		tmp = egueb_dom_node_get_ancestor_element(thiz);
		ret = egueb_dom_node_namespace_uri_lookup(tmp, prefix);
		egueb_dom_node_unref(tmp);
		break;
	}
	return ret;
}



/**
 * Introduced in DOM Level 2:
 * attribute DOMString       prefix;
 * raises(DOMException) on setting
 */
EAPI Egueb_Dom_String * egueb_dom_node_prefix_get(Egueb_Dom_Node *thiz)
{
	return egueb_dom_string_ref(thiz->prefix);
}

/**
 * Get the item a node represents for Ender
 * This will allow easily downcasting on the bindings
 * @ender_prop{item}
 * @ender_prop_downcast
 * @param[in] thiz The node to get the item from
 * @return the item related to a node
 */
EAPI Ender_Item * egueb_dom_node_item_get(Egueb_Dom_Node *thiz)
{
	Egueb_Dom_Node_Class *klass;
	Ender_Item *ret = NULL;

	if (!thiz) return NULL;

	klass = EGUEB_DOM_NODE_CLASS_GET(thiz);
	if (klass->item_get)
		ret = klass->item_get(thiz);
	if (!ret)
	{
		const Ender_Lib *lib;

		lib = ender_lib_find("egueb_dom");
		if (!lib) return NULL;

		ret = ender_lib_item_find(lib, "egueb.dom.node");
	}

	return ret;
}

EAPI void egueb_dom_node_freeze(Egueb_Dom_Node *thiz)
{
	thiz->freezed++;
}

EAPI Eina_Bool egueb_dom_node_is_freezed(Egueb_Dom_Node *thiz)
{
	return !!thiz->freezed;
}

EAPI void egueb_dom_node_thaw(Egueb_Dom_Node *thiz)
{
	if (!thiz->freezed)
	{
		ERR("Node already thawed");
		return;
	}
	thiz->freezed--;
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
  readonly attribute DOMString       namespaceURI;

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
  boolean            isEqualNode(in Node arg);

#endif
