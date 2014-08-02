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

#include "egueb_smil_private.h"
#include "egueb_smil_main.h"
#include "egueb_smil_animation.h"
#include "egueb_smil_animation_private.h"
#include "egueb_smil_clock.h"
#include "egueb_smil_keyframe.h"
#include "egueb_smil_timeline.h"
#include "egueb_smil_event.h"

#include "egueb_smil_timeline_private.h"
/*
 * This file handles the common attribute handling of every
 * element on the animation system. That is:
 * 'set', 'animate', 'animateTransform'
 * TODO:
 * 1. For every animation added into an element define a list of animation
 * element that any node can have. This way we can know the order for later
 * usage of the sandwich model
 * 2. Add a weak reference to the target node
 * TODO whenever the this element is reparented, remove/add the attribute
 * of the list of animated attributes
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Smil_Animation_Event
{
	Egueb_Dom_Node *ref;
	Egueb_Smil_Animation *thiz;
	Egueb_Smil_Timing *t;
	Egueb_Dom_Node_Event_Listener *l;
} Egueb_Smil_Animation_Event;

typedef struct _Egueb_Smil_Animation_Event_Foreach_Data
{
	Egueb_Smil_Animation *thiz;
	Egueb_Dom_Event_Listener listener;
	Eina_List *events;
	int64_t offset;
} Egueb_Smil_Animation_Event_Foreach_Data;

/* convert a timing into a duration */
static Eina_Bool _egueb_smil_animation_timing_duration(Egueb_Smil_Animation *thiz,
		Egueb_Smil_Timing *t, Egueb_Smil_Duration *d)
{
	return EINA_TRUE;
}

/* convert a timing list into a duration */
static Eina_Bool _egueb_smil_animation_timing_list_duration(Egueb_Smil_Animation *thiz,
		Egueb_Dom_List *tl, Egueb_Smil_Duration *d)
{
	Egueb_Smil_Timing t;
	Eina_Iterator *it;

	it = egueb_dom_list_iterator_new(tl);
	while (eina_iterator_next(it, (void **)&t))
	{
		printf("timing\n");
	}
	eina_iterator_free(it);
	return EINA_TRUE;
}

static Egueb_Dom_Node * _egueb_smil_animation_target_get(Egueb_Smil_Animation *thiz)
{
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_String *xlink_href = NULL;

	egueb_dom_attr_final_get(thiz->xlink_href, &xlink_href);
	if (egueb_dom_string_is_valid(xlink_href))
	{
		Egueb_Dom_Node *doc;

		doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(thiz));
		if (!doc)
		{
			ERR("No document associated with the node");
			return NULL;
		}
		target = egueb_dom_document_element_get_by_id(doc, xlink_href, NULL);
		if (!target)
		{
			ERR("Invalid target");
			egueb_dom_node_unref(doc);
			egueb_dom_string_unref(xlink_href);
			return NULL;
		}
		egueb_dom_string_unref(xlink_href);
		egueb_dom_node_unref(doc);
		ERR("we have an xlink!");
	}
	else
	{
		target = egueb_dom_node_parent_get(EGUEB_DOM_NODE(thiz));
		egueb_dom_string_unref(xlink_href);
	}
	return target;
}

/* TODO an offset only timing should be treated as an event too
 * so we can have a fine grained system to start/stop animations
 */
static void _egueb_smil_animation_event_cb(void *item, void *user_data)
{
	Egueb_Smil_Animation_Event_Foreach_Data *data = user_data;
	Egueb_Smil_Animation *thiz = data->thiz;
	Egueb_Smil_Timing *t = item;

	/* check the begin conditions and register the needed events */
	if (t->event)
	{
		Egueb_Smil_Animation_Event *h;
		Egueb_Dom_Node *ref;
		Egueb_Dom_String *event;

		INFO("Registering event '%s' on element '%s' with offset %"
				EGUEB_SMIL_CLOCK_FORMAT, t->event,
				t->id ? t->id : "NULL",
				EGUEB_SMIL_CLOCK_ARGS(t->offset));

		if (t->id)
		{
			Egueb_Dom_Node *doc;
			Egueb_Dom_String *id = NULL;

			doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(thiz));
			if (!doc)
			{
				ERR("No document set");
				return;
			}
			id = egueb_dom_string_new_with_string(t->id);
			ref = egueb_dom_document_element_get_by_id(doc, id, NULL);
			egueb_dom_node_unref(doc);
			egueb_dom_string_unref(id);
		}
		else
		{
			ref = _egueb_smil_animation_target_get(thiz);
		}

		if (!ref)
		{
			ERR("Impossible to find a target");
			return;
		}

		h = calloc(1, sizeof(Egueb_Smil_Animation_Event));
		h->t = t;
		h->thiz = thiz;
		egueb_dom_node_weak_ref_add(ref, &h->ref);
		data->events = eina_list_append(data->events, h);

		event = egueb_dom_string_new_with_string(t->event);
		h->l = egueb_dom_node_event_listener_add(ref, event, data->listener,
			EINA_FALSE, h);
		egueb_dom_string_unref(event);
		egueb_dom_node_unref(ref);
	}
	else
	{
		DBG("Offset only at %" EGUEB_SMIL_CLOCK_FORMAT, EGUEB_SMIL_CLOCK_ARGS(t->offset));
		if (t->offset < data->offset)
			data->offset = t->offset;
	}
	/* in case there's nothing on the list of events, set the offset to zero */
	if (!data->events)
	{
		data->offset = 0;
	}
}

static void _egueb_smil_animation_event_release(Eina_List *events)
{
	Egueb_Smil_Animation_Event *ev;
	EINA_LIST_FREE(events, ev)
	{
		egueb_dom_node_event_listener_free(ev->l);
		free(ev);
	}
}

static Eina_Bool _egueb_smil_animation_event_setup(Egueb_Smil_Animation *thiz,
		Egueb_Dom_Node *p, Egueb_Dom_Event_Listener listener,
		int64_t *offset, Eina_List **events)
{
	Egueb_Dom_List *l = NULL;
	Egueb_Smil_Animation_Event_Foreach_Data data;

	egueb_dom_attr_get(p, EGUEB_DOM_ATTR_TYPE_BASE, &l);

	data.thiz = thiz;
	data.listener = listener;
	data.offset = 0;
	data.events = NULL;
	egueb_dom_list_foreach(l, _egueb_smil_animation_event_cb, &data);

	*offset = data.offset;
	*events = data.events;

	return EINA_TRUE;
}

static void _egueb_smil_animation_begin(Egueb_Smil_Animation *thiz, int64_t offset)
{
	Egueb_Smil_Animation_Class *klass;

	if (thiz->started)
		return;
	thiz->started = EINA_TRUE;
	klass = EGUEB_SMIL_ANIMATION_CLASS_GET(thiz);

	INFO("Enabling animation with offset %" EGUEB_SMIL_CLOCK_FORMAT, EGUEB_SMIL_CLOCK_ARGS(offset));
	if (klass->begin)
	{
		int64_t time;

		time = egueb_smil_timeline_current_clock_get(thiz->timeline);
		time += offset;
		klass->begin(thiz, time);
	}
}

static void _egueb_smil_animation_begin_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Smil_Animation_Event *ev = data;
	Egueb_Smil_Animation *thiz = ev->thiz;
	Egueb_Dom_String *name;

	/* call the begin interface */
	name = egueb_dom_event_type_get(e);
	INFO("Begin event '%s' received", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);

	_egueb_smil_animation_begin(thiz, ev->t->offset);
}

static Eina_Bool _egueb_smil_animation_begin_setup(Egueb_Smil_Animation *thiz)
{
	int64_t offset = INT64_MAX;

	if (!_egueb_smil_animation_event_setup(thiz, thiz->begin,
			_egueb_smil_animation_begin_cb, &offset,
			&thiz->begin_events))
		return EINA_FALSE;
	return EINA_TRUE;
}

static void _egueb_smil_animation_begin_release(Egueb_Smil_Animation *thiz)
{
	_egueb_smil_animation_event_release(thiz->begin_events);
}

static void _egueb_smil_animation_end(Egueb_Smil_Animation *thiz)
{
	Egueb_Smil_Animation_Class *klass;

	if (!thiz->started)
		return;
	thiz->started = EINA_FALSE;
	klass = EGUEB_SMIL_ANIMATION_CLASS_GET(thiz);

	INFO("Ending animation");
	if (klass->end)
		klass->end(thiz);
}

static void _egueb_smil_animation_end_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Smil_Animation_Event *ev = data;
	Egueb_Smil_Animation *thiz = ev->thiz;
	Egueb_Dom_String *name;

	name = egueb_dom_event_type_get(e);
	INFO("End event '%s' received", egueb_dom_string_string_get(name));
	egueb_dom_string_unref(name);
	_egueb_smil_animation_end(thiz);
}

static Eina_Bool _egueb_smil_animation_end_setup(Egueb_Smil_Animation *thiz)
{
	int64_t offset = INT64_MAX;

	if (!_egueb_smil_animation_event_setup(thiz, thiz->end,
			_egueb_smil_animation_end_cb, &offset, &thiz->end_events))
		return EINA_FALSE;
	return EINA_TRUE;
}

static void _egueb_smil_animation_end_release(Egueb_Smil_Animation *thiz)
{
	_egueb_smil_animation_event_release(thiz->end_events);
}

#if 0
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_animation_attribute_type_set(Egueb_Dom_Tag *t, Egueb_Smil_Attribute_Type attribute_type)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(t);
	thiz->ctx.target.attribute_type = attribute_type;
}

static void _egueb_smil_animation_attribute_type_get(Egueb_Dom_Tag *t, Egueb_Smil_Attribute_Type *attribute_type)
{
	Egueb_Smil_Animation *thiz;

	if (!attribute_type) return;
	thiz = EGUEB_SMIL_ANIMATION(t);
	*attribute_type = thiz->ctx.target.attribute_type;
}

static void _egueb_smil_animation_additive_set(Egueb_Dom_Tag *t, Egueb_Smil_Additive additive)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(t);
	thiz->ctx.addition.additive = additive;
}

static void _egueb_smil_animation_additive_get(Egueb_Dom_Tag *t, Egueb_Smil_Additive *additive)
{
	Egueb_Smil_Animation *thiz;

	if (!additive) return;
	thiz = EGUEB_SMIL_ANIMATION(t);
	*additive = thiz->ctx.addition.additive;
}

static void _egueb_smil_animation_accumulate_set(Egueb_Dom_Tag *t, Egueb_Smil_Accumulate accumulate)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(t);
	thiz->ctx.addition.accumulate = accumulate;
}

static void _egueb_smil_animation_accumulate_get(Egueb_Dom_Tag *t, Egueb_Smil_Accumulate *accumulate)
{
	Egueb_Smil_Animation *thiz;

	if (!accumulate) return;
	thiz = EGUEB_SMIL_ANIMATION(t);
	*accumulate = thiz->ctx.addition.accumulate;
}

static void _egueb_smil_animation_repeat_count_set(Egueb_Dom_Tag *t, int repeat_count)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(t);
	thiz->ctx.timing.repeat_count = repeat_count;
}

static void _egueb_smil_animation_repeat_count_get(Egueb_Dom_Tag *t, int *repeat_count)
{
	Egueb_Smil_Animation *thiz;

	if (!repeat_count) return;
	thiz = EGUEB_SMIL_ANIMATION(t);
	*repeat_count = thiz->ctx.timing.repeat_count;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_smil_animation_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	/* target attributes */
	if (strcmp(key, "attributeType") == 0)
	{
		Egueb_Smil_Attribute_Type type;

		if (egueb_smil_attribute_type_string_from(&type, value))
			egueb_smil_animation_attribute_type_set(e, type);
	}
	/* addition attributes */
	else if (!strcmp(key, "repeatCount"))
	{
		int rc;

		egueb_smil_repeat_count_string_from(&rc, value);
		egueb_smil_animation_repeat_count_set(e, rc);
	}
	else
	{
		Egueb_Smil_Animation *thiz;
		Egueb_Dom_Tag *t;

		t = ender_element_object_get(e);
		thiz = EGUEB_SMIL_ANIMATION(t);
		if (thiz->descriptor.attribute_set)
			return thiz->descriptor.attribute_set(e, key, value);
		return EINA_FALSE;
	}

	return EINA_TRUE;
}
#endif

static Eina_Bool _egueb_dom_animation_setup(Egueb_Smil_Animation *thiz,
		Egueb_Dom_Node *target)
{
	Egueb_Smil_Animation_Class *klass;
	Egueb_Dom_String *attribute_name = NULL;
	Egueb_Dom_Node *attr;

	/* check that we have a timeline */
	if (!thiz->timeline)
	{
		ERR("No timeline set");
		return EINA_FALSE;
	}

	/* set our target */
	thiz->target = target;
	egueb_dom_attr_final_get(thiz->attribute_name, &attribute_name);
	if (!attribute_name)
	{
		ERR("No 'attribute_name' set");
		return EINA_FALSE;
	}

	/* get the attribute from the element */
	if (thiz->attr)
	{
		egueb_dom_node_unref(thiz->attr);
		thiz->attr = NULL;
	}

	attr = egueb_dom_element_attribute_fetch(target, attribute_name);
	if (!attr)
	{
		ERR("No attribute '%s' found",
				egueb_dom_string_string_get(attribute_name));
		egueb_dom_string_unref(attribute_name);
		return EINA_FALSE;
	}

	thiz->attr = attr;

	/* get the value descriptor */
	klass = EGUEB_SMIL_ANIMATION_CLASS_GET(thiz);
	if (!klass->value_descriptor_get)
	{
		ERR("No value descriptor defined");
		egueb_dom_string_unref(attribute_name);
		return EINA_FALSE;
	}
	thiz->d = klass->value_descriptor_get(thiz);
	if (!thiz->d)
	{
		ERR("No value descriptor");
		egueb_dom_string_unref(attribute_name);
		return EINA_FALSE;
	}

	/* do the begin and end conditions */
	_egueb_smil_animation_begin_setup(thiz);
	_egueb_smil_animation_end_setup(thiz);

	if (klass->setup)
	{
		if (!klass->setup(thiz, target))
			return EINA_FALSE;
	}

	/* in case there is no event to trigger, just start now */
	if (!thiz->begin_events)
	{
		_egueb_smil_animation_begin(thiz, 0);
	}

	return EINA_TRUE;
}

static void _egueb_dom_animation_cleanup(Egueb_Smil_Animation *thiz,
		Egueb_Dom_Node *target)
{
	Egueb_Smil_Animation_Class *klass;
	klass = EGUEB_SMIL_ANIMATION_CLASS_GET(thiz);

	/* TODO destroy the animation */
	_egueb_smil_animation_begin_release(thiz);
	_egueb_smil_animation_end_release(thiz);

	if (klass->cleanup) klass->cleanup(thiz, target);
	if (thiz->attr)
	{
		egueb_dom_node_unref(thiz->attr);
		thiz->attr = NULL;
	}
	thiz->target = NULL;
}

/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
/* Whenever a node has been inserted into the document, request the timeline */
static void _egueb_smil_animation_inserted_into_document_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Smil_Animation *thiz;
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Event *request;

	INFO("Smil animation inserted into document");
	request = egueb_smil_event_timeline_new();
	egueb_dom_node_event_dispatch(n, egueb_dom_event_ref(request), NULL, NULL);

	thiz = EGUEB_SMIL_ANIMATION(n);
	thiz->timeline = egueb_smil_event_timeline_get(request);
	thiz->document_changed = EINA_TRUE;
	egueb_dom_event_unref(request);
}

/* Whenever a node has been removed from the document, remove the timeline,
 * and the animation
 */
static void _egueb_smil_animation_removed_from_document_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Smil_Animation *thiz = data;

	INFO("Smil animation removed from document");
	_egueb_dom_animation_cleanup(thiz, thiz->target);
	egueb_smil_timeline_unref(thiz->timeline);
	thiz->timeline = NULL;
}
/*----------------------------------------------------------------------------*
 *                            Animation interface                             *
 *----------------------------------------------------------------------------*/
static const Egueb_Dom_Value_Descriptor *
_egueb_smil_animation_value_descriptor_get(Egueb_Smil_Animation *thiz)
{
	return egueb_dom_attr_value_descriptor_get(thiz->attr);
}
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_smil_animation_process(Egueb_Dom_Element *e)
{
	Egueb_Smil_Animation *thiz;
	Egueb_Dom_Node *target = NULL;
	Eina_Bool ret;

	thiz = EGUEB_SMIL_ANIMATION(e);
	target = _egueb_smil_animation_target_get(thiz);
	if (!target)
	{
		return EINA_FALSE;
	}

	if (!egueb_dom_element_is_enqueued(EGUEB_DOM_NODE(e)) &&
			thiz->target == target && !thiz->document_changed)
	{
		egueb_dom_node_unref(target);
		return EINA_TRUE;
	}

	/* now the setup */
	_egueb_dom_animation_cleanup(thiz, target);
	ret = _egueb_dom_animation_setup(thiz, target);
	/* dont keep a reference to its target, in case the target
	 * is destroyed this will be destroyed first
	 */
	thiz->document_changed = EINA_FALSE;
	egueb_dom_node_unref(target);
	return ret;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_ELEMENT_DESCRIPTOR,
		Egueb_Smil_Animation, Egueb_Smil_Animation_Class, egueb_smil_animation);

static void _egueb_smil_animation_class_init(void *k)
{
	Egueb_Dom_Element_Class *klass;
	Egueb_Smil_Animation_Class *thiz_klass;

	klass = EGUEB_DOM_ELEMENT_CLASS(k);
	klass->process = _egueb_smil_animation_process;

	thiz_klass = EGUEB_SMIL_ANIMATION_CLASS(k);
	thiz_klass->value_descriptor_get = _egueb_smil_animation_value_descriptor_get;
}

static void _egueb_smil_animation_instance_init(void *o)
{
	Egueb_Smil_Animation *thiz;
	Egueb_Dom_Node *n;

	/* add a callback whenever the node has been removed from another */
	n = EGUEB_DOM_NODE(o);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT,
			_egueb_smil_animation_inserted_into_document_cb,
			EINA_FALSE, n);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
			_egueb_smil_animation_removed_from_document_cb,
			EINA_FALSE, n);

	thiz = EGUEB_SMIL_ANIMATION(o);
	thiz->attribute_name = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SMIL_ATTRIBUTE_NAME), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->fill = egueb_smil_attr_fill_new(
			egueb_dom_string_ref(EGUEB_SMIL_FILL),
			EGUEB_SMIL_FILL_REMOVE);
	thiz->dur = egueb_smil_attr_duration_new(
			egueb_dom_string_ref(EGUEB_SMIL_DUR),
			&EGUEB_SMIL_DURATION_INDEFINITE);
	thiz->begin = egueb_smil_attr_timing_list_new(
			egueb_dom_string_ref(EGUEB_SMIL_BEGIN), NULL);
	thiz->end = egueb_smil_attr_timing_list_new(
			egueb_dom_string_ref(EGUEB_SMIL_END), NULL);
	thiz->xlink_href = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_DOM_XLINK_HREF),
			NULL, EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->repeat_count = egueb_smil_attr_repeat_count_new();
	thiz->repeat_dur = egueb_smil_attr_duration_new(
			egueb_dom_string_ref(EGUEB_SMIL_NAME_REPEAT_DUR), NULL);

	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->attribute_name), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->fill), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->dur), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->begin), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->end), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->xlink_href), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->repeat_count), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->repeat_dur), NULL);
}

static void _egueb_smil_animation_instance_deinit(void *o)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(o);
	/* the cleanup will be called as part of the deinitialization */
	egueb_dom_node_unref(thiz->attribute_name);
	egueb_dom_node_unref(thiz->fill);
	egueb_dom_node_unref(thiz->dur);
	egueb_dom_node_unref(thiz->begin);
	egueb_dom_node_unref(thiz->end);
	egueb_dom_node_unref(thiz->repeat_count);
	egueb_dom_node_unref(thiz->repeat_dur);
	egueb_dom_node_unref(thiz->xlink_href);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_smil_animation_active_duration_get(Egueb_Dom_Node *n,
		Egueb_Smil_Clock *duration)
{
	Egueb_Smil_Animation *thiz;
	Egueb_Smil_Duration dur;
	Egueb_Smil_Duration repeat_dur;
	Egueb_Smil_Duration final;
	Egueb_Smil_Repeat_Count repeat_count;
	Egueb_Dom_List *begin = NULL;
	Egueb_Dom_List *end = NULL;
	Egueb_Smil_Duration end_dur;
	Eina_Bool dur_is_set;
	Eina_Bool repeat_count_is_set;
	Eina_Bool repeat_dur_is_set;
	Eina_Bool end_is_set;
	Eina_Bool begin_is_set;

	/* http://www.w3.org/TR/2001/REC-smil-animation-20010904/#ComputingActiveDur */
	thiz = EGUEB_SMIL_ANIMATION(n);
	dur_is_set = egueb_dom_attr_final_get(thiz->dur, &dur);
	repeat_count_is_set = egueb_dom_attr_final_get(thiz->repeat_count, &repeat_count);
	repeat_dur_is_set = egueb_dom_attr_final_get(thiz->repeat_dur, &repeat_dur);
	end_is_set = egueb_dom_attr_final_get(thiz->end, &end);
	begin_is_set = egueb_dom_attr_final_get(thiz->begin, &begin);

	/* initialize the final value */
	final.type = EGUEB_SMIL_DURATION_TYPE_INDEFINITE;
	final.data.clock = -1;

	if (end_is_set && begin_is_set)
	{
		Egueb_Smil_Duration begin_dur;

		_egueb_smil_animation_timing_list_duration(thiz, begin, &begin_dur);
		_egueb_smil_animation_timing_list_duration(thiz, end, &end_dur);
	}
	egueb_dom_list_unref(begin);
	egueb_dom_list_unref(end);

	/* we should return the min of duration/repeatcount*d/repeatdur/end-begin */
	if (dur_is_set)
	{
 		if (dur.type == EGUEB_SMIL_DURATION_TYPE_CLOCK)
		{
			final.type = EGUEB_SMIL_DURATION_TYPE_CLOCK;
			final.data.clock = dur.data.clock;
			if (repeat_count_is_set)
			{
				if (repeat_count.type == EGUEB_SMIL_REPEAT_COUNT_TYPE_FINITE)
				{
					final.data.clock = final.data.clock * repeat_count.value;
				}
				else
				{
					final.type = EGUEB_SMIL_DURATION_TYPE_INDEFINITE;
				}
			}
		}
	}

	/* in case is indefinite, use it, otherwise calc the min in case the above
	 * is not indefinite
	 */
	if (repeat_dur_is_set)
	{
		if (repeat_dur.type == EGUEB_SMIL_DURATION_TYPE_CLOCK)
		{
			if (final.type == EGUEB_SMIL_DURATION_TYPE_CLOCK)
			{
				final.data.clock = final.data.clock < repeat_dur.data.clock ?
						final.data.clock : repeat_dur.data.clock;
			}
			else
			{
				final.type = EGUEB_SMIL_DURATION_TYPE_CLOCK;
				final.data.clock = repeat_dur.data.clock;
			}
		}
		else if (repeat_dur.type == EGUEB_SMIL_DURATION_TYPE_INDEFINITE)
		{
			final.type = EGUEB_SMIL_DURATION_TYPE_INDEFINITE;
		}
	}
	/* TODO handle the end */

	if (final.type == EGUEB_SMIL_DURATION_TYPE_CLOCK)
	{
		*duration = final.data.clock;
		return EINA_TRUE;
	}
	else
	{
		return EINA_FALSE;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_smil_is_animation(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SMIL_ANIMATION_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

#if 0
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_attribute_name_set(Ender_Element *e, const char *name)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_ANIMATION_ATTRIBUTE_NAME, name, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_attribute_name_get(Ender_Element *e, const char **name)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animation_attribute_name_get(t, name);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_attribute_type_set(Ender_Element *e, Egueb_Smil_Attribute_Type type)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_ANIMATION_ATTRIBUTE_TYPE, type, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_attribute_type_get(Ender_Element *e, Egueb_Smil_Attribute_Type *type)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animation_attribute_type_get(t, type);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_dur_set(Ender_Element *e, Egueb_Smil_Duration *dur)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_ANIMATION_DUR, dur, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_dur_get(Ender_Element *e, Egueb_Smil_Duration *dur)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animation_dur_get(t, dur);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_additive_set(Ender_Element *e, Egueb_Smil_Additive additive)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_ANIMATION_ADDITIVE, additive, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_additive_get(Ender_Element *e, Egueb_Smil_Additive *additive)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animation_additive_get(t, additive);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_accumulate_set(Ender_Element *e, Egueb_Smil_Accumulate accumulate)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_ANIMATION_ACCUMULATE, accumulate, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_accumulate_get(Ender_Element *e, Egueb_Smil_Accumulate *accumulate)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animation_accumulate_get(t, accumulate);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_repeat_count_set(Ender_Element *e, int repeat_count)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_ANIMATION_REPEAT_COUNT, repeat_count, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_repeat_count_get(Ender_Element *e, int *repeat_count)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animation_repeat_count_get(t, repeat_count);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_begin_set(Ender_Element *e, Eina_List *begin)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_ANIMATION_BEGIN, begin, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_end_set(Ender_Element *e, Eina_List *end)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_ANIMATION_END, end, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_fill_set(Ender_Element *e, Egueb_Smil_Fill fill)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_ANIMATION_FILL, fill, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animation_fill_get(Ender_Element *e, Egueb_Smil_Fill *fill)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animation_fill_get(t, fill);
}
#endif
