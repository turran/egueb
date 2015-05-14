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
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_mutation.h"

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_mutation_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED = EGUEB_DOM_STRING_STATIC("DOMNodeDestroyed");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_SUBTREE_MODIFIED = EGUEB_DOM_STRING_STATIC("DOMSubtreeModified");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED = EGUEB_DOM_STRING_STATIC("DOMNodeInserted");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED = EGUEB_DOM_STRING_STATIC("DOMNodeRemoved");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT = EGUEB_DOM_STRING_STATIC("DOMNodeInsertedIntoDocument");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT = EGUEB_DOM_STRING_STATIC("DOMNodeRemovedFromDocument");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED = EGUEB_DOM_STRING_STATIC("DOMAtttrModified");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_CHARACTER_DATA_MODIFIED = EGUEB_DOM_STRING_STATIC("DOMCharacterDataModified");

static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_SET = EGUEB_DOM_STRING_STATIC("DOMNodeDocumentSet");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_UNSET = EGUEB_DOM_STRING_STATIC("DOMNodeDocumentUnset");

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Dom_Event_Mutation, Egueb_Dom_Event_Mutation_Class,
		egueb_dom_event_mutation);

static void _egueb_dom_event_mutation_class_init(void *k)
{
}

static void _egueb_dom_event_mutation_instance_init(void *o)
{
}

static void _egueb_dom_event_mutation_instance_deinit(void *o)
{
	Egueb_Dom_Event_Mutation *thiz;

	thiz = EGUEB_DOM_EVENT_MUTATION(o);
	if (thiz->related)
	{
		egueb_dom_node_unref(thiz->related);
		thiz->related = NULL;
	}
	if (thiz->attr)
	{
		egueb_dom_string_unref(thiz->attr);
		thiz->attr = NULL;
	}
}

static Egueb_Dom_Event * _egueb_dom_event_mutation_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_mutation);
	return event;
}

static void _egueb_dom_event_mutation_init_internal(Egueb_Dom_Event *e,
		Egueb_Dom_String *type, Eina_Bool bubbleable,
		Eina_Bool cancelable,
		Egueb_Dom_Node *related,
		const Egueb_Dom_Value *prev_value,
		const Egueb_Dom_Value *new_value,
		Egueb_Dom_String *attr,
		Egueb_Dom_Event_Mutation_Attr_Type attr_type,
		Egueb_Dom_Attr_Type attr_modification_type)
{
	Egueb_Dom_Event_Mutation *thiz;

	thiz = EGUEB_DOM_EVENT_MUTATION(e);
	thiz->related = related;
	thiz->prev_value = prev_value;
	thiz->new_value = new_value;
	thiz->attr = attr;
	thiz->attr_type = attr_type;
	thiz->attr_modification_type = attr_modification_type;
	egueb_dom_event_init(e, type, bubbleable, EINA_TRUE, cancelable,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Event * egueb_dom_event_mutation_node_inserted_new(Egueb_Dom_Node *parent)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_mutation_new();
	_egueb_dom_event_mutation_init_internal(e,
			egueb_dom_string_ref(EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED),
			EINA_TRUE, EINA_FALSE,
			parent, NULL, NULL, NULL, 0, 0);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_mutation_node_inserted_into_document_new(
		void)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_mutation_new();
	_egueb_dom_event_mutation_init_internal(e,
			egueb_dom_string_ref(EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT),
			EINA_FALSE, EINA_FALSE,
			NULL, NULL, NULL, NULL, 0, 0);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_mutation_node_removed_new(Egueb_Dom_Node *parent)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_mutation_new();
	_egueb_dom_event_mutation_init_internal(e,
			egueb_dom_string_ref(EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED),
			EINA_TRUE, EINA_FALSE,
			parent, NULL, NULL, NULL, 0, 0);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_mutation_node_removed_from_document_new(void)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_mutation_new();
	_egueb_dom_event_mutation_init_internal(e,
			egueb_dom_string_ref(EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT),
			EINA_FALSE, EINA_FALSE,
			NULL, NULL, NULL, NULL, 0, 0);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_mutation_node_destroyed_new(void)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_mutation_new();
	_egueb_dom_event_mutation_init_internal(e,
			egueb_dom_string_ref(EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED),
			EINA_FALSE, EINA_FALSE,
			NULL, NULL, NULL, NULL, 0, 0);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_mutation_attr_modified_new(
		Egueb_Dom_Node *related,
		const Egueb_Dom_Value *prev_value,
		const Egueb_Dom_Value *new_value,
		Egueb_Dom_String *name,
		Egueb_Dom_Event_Mutation_Attr_Type attr_type,
		Egueb_Dom_Attr_Type attr_modification_type)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_mutation_new();
	_egueb_dom_event_mutation_init_internal(e,
			egueb_dom_string_ref(EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED),
			EINA_TRUE, EINA_FALSE, related, prev_value, new_value, 
			name, attr_type, attr_modification_type);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_mutation_character_data_modified_new(void)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_mutation_new();
	_egueb_dom_event_mutation_init_internal(e,
			egueb_dom_string_ref(EGUEB_DOM_EVENT_MUTATION_CHARACTER_DATA_MODIFIED),
			EINA_TRUE, EINA_FALSE, NULL, NULL, NULL,
			NULL, 0, 0);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_mutation_node_document_set_new(void)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_mutation_new();
	_egueb_dom_event_mutation_init_internal(e,
			egueb_dom_string_ref(EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_SET),
			EINA_FALSE, EINA_FALSE, NULL, NULL, NULL,
			NULL, 0, 0);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_mutation_node_document_unset_new(void)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_mutation_new();
	_egueb_dom_event_mutation_init_internal(e,
			egueb_dom_string_ref(EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_UNSET),
			EINA_FALSE, EINA_FALSE, NULL, NULL, NULL,
			NULL, 0, 0);
	return e;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED = &_EGUEB_DOM_EVENT_MUTATION_NODE_DESTROYED;
Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_SUBTREE_MODIFIED = &_EGUEB_DOM_EVENT_MUTATION_SUBTREE_MODIFIED;
Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED = &_EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED;
Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED = &_EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED;
Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT = &_EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT;
Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT = &_EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT;
Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED = &_EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED;
Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_CHARACTER_DATA_MODIFIED = &_EGUEB_DOM_EVENT_MUTATION_CHARACTER_DATA_MODIFIED;

Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_SET = &_EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_SET;
Egueb_Dom_String *EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_UNSET = &_EGUEB_DOM_EVENT_MUTATION_NODE_DOCUMENT_UNSET;

EAPI Eina_Bool egueb_dom_event_is_mutation(Egueb_Dom_Event *e)
{
	if (!e) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(e),
			EGUEB_DOM_EVENT_MUTATION_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Egueb_Dom_Node * egueb_dom_event_mutation_related_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Mutation *thiz;

	thiz = EGUEB_DOM_EVENT_MUTATION(e);
	return egueb_dom_node_ref(thiz->related);
}

/* readonly attribute DOMString        prevValue; */
EAPI void egueb_dom_event_mutation_value_prev_string_get(Egueb_Dom_Event *e,
		Egueb_Dom_String **s)
{
	Egueb_Dom_Event_Mutation *thiz = EGUEB_DOM_EVENT_MUTATION(e);
	/* TODO */
	*s = NULL;
}

/* readonly attribute DOMString        newValue; */
EAPI void egueb_dom_event_mutation_value_new_string_get(Egueb_Dom_Event *e,
		Egueb_Dom_String **s)
{
	Egueb_Dom_Event_Mutation *thiz = EGUEB_DOM_EVENT_MUTATION(e);
	/* convert the value to a string */
	if (!thiz->new_value) return;
	egueb_dom_value_string_to(thiz->new_value, s);
}

/* readonly attribute DOMString        newValue; */
EAPI void egueb_dom_event_mutation_value_new_get(Egueb_Dom_Event *e,
		const Egueb_Dom_Value **v)
{
	Egueb_Dom_Event_Mutation *thiz = EGUEB_DOM_EVENT_MUTATION(e);
	*v = thiz->new_value;
}

/* readonly attribute DOMString       attrName; */
EAPI void egueb_dom_event_mutation_attr_name_get(Egueb_Dom_Event *e,
		Egueb_Dom_String **s)
{
	Egueb_Dom_Event_Mutation *thiz;

	if (!s) return;
	thiz = EGUEB_DOM_EVENT_MUTATION(e);
	if (!thiz->attr) *s = NULL;
	else *s = egueb_dom_string_ref(thiz->attr);
}

EAPI void egueb_dom_event_mutation_attr_type_get(Egueb_Dom_Event *e,
		Egueb_Dom_Event_Mutation_Attr_Type *attr_type)
{
	Egueb_Dom_Event_Mutation *thiz;

	if (!attr_type) return;
	thiz = EGUEB_DOM_EVENT_MUTATION(e);
	*attr_type = thiz->attr_type;
}

EAPI void egueb_dom_event_mutation_attr_modification_type_get(Egueb_Dom_Event *e,
		Egueb_Dom_Attr_Type *attr_modification_type)
{
	Egueb_Dom_Event_Mutation *thiz;

	if (!attr_modification_type) return;
	thiz = EGUEB_DOM_EVENT_MUTATION(e);
	*attr_modification_type = thiz->attr_modification_type;
}

EAPI Eina_Bool egueb_dom_event_mutation_is_attr_modified(
		const Egueb_Dom_String *str)
{
	const char *s_str;

	s_str = egueb_dom_string_string_get(str);
	if (str == EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED ||
			!strcmp(s_str, egueb_dom_string_string_get(
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED)))
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

/**
 * @brief Prevent the process of the node that triggered the event
 * @param[in] event The event to prevent for process
 *
 * By default EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
 * EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED and
 * EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED will enqueue the node that triggered
 * the event for process once the event reaches the topmost element. If the
 * event prevents the process by calling this function, then the node will
 * not be enqueued.
 */
EAPI void egueb_dom_event_mutation_process_prevent(Egueb_Dom_Event *event)
{
	Egueb_Dom_Event_Mutation *thiz;

	thiz = EGUEB_DOM_EVENT_MUTATION(event);
	thiz->process_prevent = EINA_TRUE;
}

/**
 * @brief Check if the mutation is event will trigger a process
 * @param[in] event The mutation event to check
 * @return EINA_TRUE if the event prevents a process and thus the node will not
 * be processed or EINA_FALSE if the node will be processed.
 */
EAPI Eina_Bool egueb_dom_event_mutation_is_process_prevented(
		Egueb_Dom_Event *event)
{
	Egueb_Dom_Event_Mutation *thiz;

	thiz = EGUEB_DOM_EVENT_MUTATION(event);
	return thiz->process_prevent;
}
