/* Egueb_Xlink - XLINK
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
#include "egueb_xlink_private.h"
#include "egueb_xlink_attr_href.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_object_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_XLINK_ATTR_HREF_DESCRIPTOR 					\
		egueb_xlink_attr_href_descriptor_get()
#define EGUEB_XLINK_ATTR_HREF(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Xlink_Attr_Href, EGUEB_XLINK_ATTR_HREF_DESCRIPTOR)

typedef struct _Egueb_Xlink_Attr_Href
{
	Egueb_Dom_Attr_Object base;
	Egueb_Dom_String *anim;
	Egueb_Dom_String *value;
	Egueb_Dom_String *def;

	Egueb_Dom_String *last;
	Egueb_Dom_Node *node;
	Egueb_Xlink_Attr_Href_Cb on_target_removed;
	Eina_Bool automatic_enqueue;
	/* TODO in the future we might decide to create
	 * events for attributes, so the attribute can now
	 * when it has been added/removed from a node. The mutation
	 * event for attributes is triggered on the element, never
	 * on the attribute
	 */
	Eina_Bool initialized;
} Egueb_Xlink_Attr_Href;

typedef struct _Egueb_Xlink_Attr_Href_Class
{
	Egueb_Dom_Attr_Object_Class base;
} Egueb_Xlink_Attr_Href_Class;

/*----------------------------------------------------------------------------*
 *                           Target related functions                         *
 *----------------------------------------------------------------------------*/
/* whenever the target has changed and we do automatic enqueue make sure
 * to enqueue the parent node
 */

static void _egueb_xlink_attr_href_target_request_cb(Egueb_Dom_Event *e,
		void *user_data)
{
	Egueb_Xlink_Attr_Href *thiz = user_data;
	Egueb_Dom_Attr *attr;

	attr = EGUEB_DOM_ATTR(thiz);
	/* request a process on the owner of the attribute */
	if (thiz->automatic_enqueue)
	{
		INFO("The xlink:href requested a process, let's request ourselves too");
		egueb_dom_element_request_process(attr->owner);
	}
}

static void _egueb_xlink_attr_href_target_destroyed_cb(Egueb_Dom_Event *e,
		void *user_data)
{
	Egueb_Xlink_Attr_Href *thiz = user_data;
	if (thiz->on_target_removed)
	{
		thiz->on_target_removed(EGUEB_DOM_NODE(thiz));
	}
}

static void _egueb_xlink_attr_href_target_cleanup(Egueb_Xlink_Attr_Href *thiz)
{
	if (thiz->node)
	{
		/* remove the weak ref */
		egueb_dom_node_weak_unref(thiz->node,
				_egueb_xlink_attr_href_target_destroyed_cb,
					thiz);
		/* remove the events associated with the target */
		egueb_dom_node_event_listener_remove(thiz->node,
				EGUEB_DOM_EVENT_PROCESS,
				_egueb_xlink_attr_href_target_request_cb,
				EINA_FALSE, thiz);
		thiz->node = NULL;
	}
}

static void _egueb_xlink_attr_href_target_setup(Egueb_Xlink_Attr_Href *thiz,
		Egueb_Dom_Node *target)
{
	/* Add a weak ref so in case the node is destroyed
	 * cleanup the parent's setup process (the animiation
	 * case)
	 */
	egueb_dom_node_weak_ref(target,
			_egueb_xlink_attr_href_target_destroyed_cb,
			thiz);
	thiz->node = target;
	/* in case we also want to listen any change to enqueue
	 * the parent too, check the passed in flag
	 * (the pattern, gradient case, any change on the
	 * target must recalc ourselves)
	 */
	if (thiz->automatic_enqueue)
	{
		egueb_dom_node_event_listener_add(target,
				EGUEB_DOM_EVENT_PROCESS,
				_egueb_xlink_attr_href_target_request_cb,
				EINA_FALSE, thiz);
	}
}
/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
/* whenever the owner document of the parent emits the id added/removed
 * check that the id is the target. If the target's id is removed from
 * the document, call the target_removed callback as the target is not longer
 * a valid target. If the target's id is added make sure to request a process
 * of the parent
 */ 
static void _egueb_xlink_attr_href_document_id_inserted_cb(Egueb_Dom_Event *e,
		void *user_data)
{
	Egueb_Xlink_Attr_Href *thiz = user_data;
	Egueb_Dom_Node *node;
	Egueb_Dom_Node *id_attr;
	Egueb_Dom_String *id_name;
	Egueb_Dom_String *id;

	node = egueb_dom_event_document_related_get(e);
	/* TODO use a public string EGUEB_[XLINK|DOM]_ID */
	id_name = egueb_dom_string_new_with_static_string("id");
	id_attr = egueb_dom_element_attribute_node_get(node, id_name);
	egueb_dom_attr_final_get(id_attr, &id);
	if (egueb_dom_string_is_equal(id, thiz->last)) {
		Egueb_Dom_Attr *attr;

		attr = EGUEB_DOM_ATTR(user_data);
		DBG("Id inserted %s", egueb_dom_string_string_get(id));
		egueb_dom_element_request_process(attr->owner);
	}
	egueb_dom_node_unref(id_attr);
	egueb_dom_node_unref(node);
	egueb_dom_string_unref(id_name);
	egueb_dom_string_unref(id);
}

static void _egueb_xlink_attr_href_document_id_removed_cb(Egueb_Dom_Event *e,
		void *user_data)
{
	Egueb_Xlink_Attr_Href *thiz = user_data;
	Egueb_Dom_Node *node;
	Egueb_Dom_Node *id_attr;
	Egueb_Dom_String *id_name;
	Egueb_Dom_String *id;

	node = egueb_dom_event_document_related_get(e);
	/* TODO use a public string EGUEB_[XLINK|DOM]_ID */
	id_name = egueb_dom_string_new_with_static_string("id");
	id_attr = egueb_dom_element_attribute_node_get(node, id_name);
	egueb_dom_attr_final_get(id_attr, &id);
	if (egueb_dom_string_is_equal(id, thiz->last)) {
		Egueb_Xlink_Attr_Href *thiz = user_data;

		DBG("Id removed %s", egueb_dom_string_string_get(id));
		_egueb_xlink_attr_href_target_cleanup(thiz);
		if (thiz->on_target_removed)
		{
			thiz->on_target_removed(EGUEB_DOM_NODE(thiz));
		}
	}
	egueb_dom_node_unref(id_attr);
	egueb_dom_node_unref(node);
	egueb_dom_string_unref(id_name);
	egueb_dom_string_unref(id);
	
}

/* Whenever a document is unset on the owner of the attribute, its target
 * is no longer valid, so we need to call the target_removed callback
 */
static void _egueb_xlink_attr_href_parent_document_unset_cb(Egueb_Dom_Event *e,
		void *user_data)
{
	Egueb_Xlink_Attr_Href *thiz = user_data;
	Egueb_Dom_Attr *attr;
	Egueb_Dom_Node *document;

	attr = EGUEB_DOM_ATTR(thiz);
	/* remove the events associated with the parent */
	egueb_dom_node_event_listener_remove(attr->owner,
			EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_UNSET,
			_egueb_xlink_attr_href_parent_document_unset_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_event_listener_remove(attr->owner,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
			_egueb_xlink_attr_href_parent_document_unset_cb,
			EINA_FALSE, thiz);
	/* remove the events associated with the document in case they are set */
	document = egueb_dom_node_owner_document_get(attr->owner);
	egueb_dom_node_event_listener_remove(document,
			EGUEB_DOM_EVENT_DOCUMENT_ID_REMOVED,
			_egueb_xlink_attr_href_document_id_removed_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_event_listener_remove(document,
			EGUEB_DOM_EVENT_DOCUMENT_ID_INSERTED,
			_egueb_xlink_attr_href_document_id_inserted_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_unref(document);
	/* call the target removed callback if set */
	if (thiz->on_target_removed)
		thiz->on_target_removed(EGUEB_DOM_NODE(thiz));
	/* make sure to intialize again */
	thiz->initialized = EINA_FALSE;
}
/*----------------------------------------------------------------------------*
 *                            Attr object interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_xlink_attr_href_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void ***o)
{
	Egueb_Xlink_Attr_Href *thiz;

	thiz = EGUEB_XLINK_ATTR_HREF(p);
	switch (type)
	{
		case EGUEB_DOM_ATTR_TYPE_ANIMATED:
		*o = (void **)&thiz->anim;
		break;

		case EGUEB_DOM_ATTR_TYPE_BASE:
		*o = (void **)&thiz->value;
		break;

		case EGUEB_DOM_ATTR_TYPE_DEFAULT:
		*o = (void **)&thiz->def;
		break;

		default:
		return EINA_FALSE;
		break;
	}
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                               Attr interface                               *
 *----------------------------------------------------------------------------*/
static void _egueb_xlink_attr_href_clone(Egueb_Dom_Attr *a, Eina_Bool live,
		Eina_Bool deep, Egueb_Dom_Attr *cloned)
{

}

static const Egueb_Dom_Value_Descriptor *
_egueb_xlink_attr_href_value_descriptor_get(Egueb_Dom_Attr *p)
{
	return egueb_dom_value_dom_string_descriptor_get();
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_OBJECT_DESCRIPTOR,
		Egueb_Xlink_Attr_Href, Egueb_Xlink_Attr_Href_Class,
		egueb_xlink_attr_href)

static void _egueb_xlink_attr_href_class_init(void *k)
{
	Egueb_Dom_Attr_Class *p_klass;
	Egueb_Dom_Attr_Object_Class *o_klass;

	p_klass = EGUEB_DOM_ATTR_CLASS(k);
	p_klass->value_descriptor_get = _egueb_xlink_attr_href_value_descriptor_get;
	p_klass->clone = _egueb_xlink_attr_href_clone;

	o_klass = EGUEB_DOM_ATTR_OBJECT_CLASS(k);
	o_klass->value_get = _egueb_xlink_attr_href_value_get;
}

static void _egueb_xlink_attr_href_instance_init(void *o)
{
}

static void _egueb_xlink_attr_href_instance_deinit(void *o)
{
	Egueb_Xlink_Attr_Href *thiz;

	thiz = EGUEB_XLINK_ATTR_HREF(o);
	if (thiz->def)
		egueb_dom_string_unref(thiz->def);
	if (thiz->value)
		egueb_dom_string_unref(thiz->value);
	if (thiz->anim)
		egueb_dom_string_unref(thiz->anim);

	if (thiz->last)
		egueb_dom_string_unref(thiz->last);
	_egueb_xlink_attr_href_target_cleanup(thiz);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_xlink_attr_href_new(Egueb_Dom_String *name,
		Egueb_Dom_String *def)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_xlink_attr_href);
	egueb_dom_attr_init(n, name, egueb_dom_string_ref(EGUEB_DOM_NAME_NS_XMLNS),
			EINA_TRUE, EINA_TRUE, EINA_TRUE);
	if (def) egueb_dom_attr_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, def);
	return n;
}

EAPI Eina_Bool egueb_xlink_attr_href_process(Egueb_Dom_Node *n)
{
	Egueb_Xlink_Attr_Href *thiz;
	Egueb_Dom_String *str = NULL;
	Eina_Bool ret = EINA_TRUE;

	thiz = EGUEB_XLINK_ATTR_HREF(n);
	/* register the needed events */
	if (!thiz->initialized)
	{
		Egueb_Dom_Attr *a;

		a = EGUEB_DOM_ATTR(n);
		/* Check that we are still part of a document */
		egueb_dom_node_event_listener_add(a->owner,
				EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_UNSET,
				_egueb_xlink_attr_href_parent_document_unset_cb,
				EINA_FALSE, thiz);
		egueb_dom_node_event_listener_add(a->owner,
				EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
				_egueb_xlink_attr_href_parent_document_unset_cb,
				EINA_FALSE, thiz);
		thiz->initialized = EINA_TRUE;
	}

	egueb_dom_attr_final_get(n, &str);
	/* early exit, nothing to do */
	if (egueb_dom_string_is_equal(str, thiz->last) && thiz->node)
	{
		INFO("Same id, nothing to do");
		if (!egueb_dom_string_is_valid(str))
			ret = EINA_FALSE;
		egueb_dom_string_unref(str);
		return ret;
	}

	/* 1. The attribute is not set -> return FALSE with no node */
	if (!egueb_dom_string_is_valid(str))
	{
		ret = EINA_FALSE;
		goto done;
	}
	else
	{
		Egueb_Dom_Attr *a;
		Egueb_Dom_Node *doc = NULL;
		Egueb_Dom_Node *target;

		a = EGUEB_DOM_ATTR(n);
		/* TODO is the document also set on the attr? */
		doc = egueb_dom_node_owner_document_get(a->owner);
		if (!doc)
		{
			WARN("No document set");
			ret = EINA_FALSE;
			goto done;
		}

		INFO("Valid id, processing");
		/* release any reference */
		_egueb_xlink_attr_href_target_cleanup(thiz);
		target = egueb_dom_document_element_get_by_id(doc, str, NULL);

		/* 2. The attribute is set but no element is found -> return TRUE with no node */
		if (!target)
		{
			DBG("Target '%s' not found", egueb_dom_string_string_get(str));
			/* We need to know whenever an element is inserted into
			 * the document with a specific id, register an event
			 * handler on the document itself. If it is found enqueue
			 * again the parent object and remove the callback
			 */
			egueb_dom_node_event_listener_add(doc,
				EGUEB_DOM_EVENT_DOCUMENT_ID_INSERTED,
				_egueb_xlink_attr_href_document_id_inserted_cb,
				EINA_FALSE, thiz);
		}
		/* 3. The attribute is set and the element is found -> return TRUE with node */
		else
		{
			DBG("Target '%s' found", egueb_dom_string_string_get(str));
			_egueb_xlink_attr_href_target_setup(thiz, target);
			egueb_dom_node_event_listener_add(doc,
					EGUEB_DOM_EVENT_DOCUMENT_ID_REMOVED,
					_egueb_xlink_attr_href_document_id_removed_cb,
					EINA_FALSE, thiz);
			egueb_dom_node_unref(target);
		}
		egueb_dom_node_unref(doc);
	}
done:
	/* swap the xlink:href */
	if (thiz->last)
	{
		egueb_dom_string_unref(thiz->last);
		thiz->last = NULL;
	}
	thiz->last = str;
	return ret;
}

EAPI void egueb_xlink_attr_href_automatic_enqueue_set(Egueb_Dom_Node *n,
		Eina_Bool enable)
{
	Egueb_Xlink_Attr_Href *thiz;

	thiz = EGUEB_XLINK_ATTR_HREF(n);
	thiz->automatic_enqueue = enable;
}

EAPI void egueb_xlink_attr_href_on_target_removed_set(Egueb_Dom_Node *n,
		Egueb_Xlink_Attr_Href_Cb cb)
{
	Egueb_Xlink_Attr_Href *thiz;

	thiz = EGUEB_XLINK_ATTR_HREF(n);
	thiz->on_target_removed = cb;
}

EAPI Egueb_Dom_Node * egueb_xlink_attr_href_node_get(Egueb_Dom_Node *n)
{
	Egueb_Xlink_Attr_Href *thiz;

	thiz = EGUEB_XLINK_ATTR_HREF(n);
	return egueb_dom_node_ref(thiz->node);
}
