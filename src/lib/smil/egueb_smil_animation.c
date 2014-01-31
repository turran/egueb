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
	Egueb_Smil_Animation *thiz;
	Egueb_Smil_Timing *t;
	Egueb_Dom_Node_Event_Listener *l;
} Egueb_Smil_Animation_Event;

typedef struct _Egueb_Smil_Animation_Event_Foreach_Data
{
	Egueb_Dom_Event_Listener listener;
	Eina_List *events;
	int64_t offset;
} Egueb_Smil_Animation_Event_Foreach_Data;

/* TODO an offset only timing should be treated as an event too
 * so we can have a fine grained system to start/stop animations
 */
static void _egueb_smil_animation_event_cb(void *item, void *user_data)
{
	Egueb_Smil_Animation_Event_Foreach_Data *data = user_data;
	Egueb_Smil_Timing *t = item;

	/* check the begin conditions and register the needed events */
	if (t->event)
	{
		DBG("Registering event '%s' on element '%s' with offset %"
				ETCH_TIME_FORMAT, t->event,
				t->id ? t->id : "NULL",
				ETCH_TIME_ARGS(t->offset));
#if 0
		if (!t->id)
		{
			if (t->repeat)
				ref = thiz->thiz_e;
			else
				ref = thiz->ctx.parent_e;
		}
		else
		{
			Ender_Element *topmost;
			egueb_smil_element_internal_topmost_get(thiz->thiz_t, &topmost);
			egueb_smil_element_svg_element_get(topmost, t->id, &ref);
		}

		if (ref)
		{
			Egueb_Smil_Animation_Event *h;

			h = calloc(1, sizeof(Egueb_Smil_Animation_Event));
			h->l = ender_event_listener_add(ref, t->event, cb, h);
			h->thiz = thiz;
			h->ev = t;
			handlers = eina_list_append(handlers, h);
		}
#endif
	}
	else
	{
		DBG("Offset only at %" ETCH_TIME_FORMAT, ETCH_TIME_ARGS(t->offset));
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
	double seconds;

	if (thiz->started)
		return;
	thiz->started = EINA_TRUE;
	klass = EGUEB_SMIL_ANIMATION_CLASS_GET(thiz);

	DBG("Enabling animation with offset %" ETCH_TIME_FORMAT, ETCH_TIME_ARGS(offset));
	if (klass->begin)
	{
		int64_t time;

		etch_timer_get(thiz->etch, &time);
		time += offset;
		klass->begin(thiz, time);
	}
}

static void _egueb_smil_animation_begin_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Smil_Animation_Event *ev = data;
	Egueb_Smil_Animation *thiz = ev->thiz;

	/* call the begin interface */
	//DBG("Begin event '%s' received", event_name);
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

static void _egueb_smil_animation_end_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Smil_Animation_Event *ev = data;
	Egueb_Smil_Animation *thiz = ev->thiz;

	/* call the end interface */
	//DBG("End event '%s' received", event_name);
	if (!thiz->started)
		return;
	DBG("Disabling animation");
#if 0
	if (thiz->descriptor.disable)
		thiz->descriptor.disable(thiz->thiz_t);
#endif
	thiz->started = EINA_FALSE;
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

static void _egueb_smil_animation_begin_set(Egueb_Dom_Tag *t, Eina_List *begin)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(t);
	thiz->ctx.timing.begin = begin;
}

static void _egueb_smil_animation_end_set(Egueb_Dom_Tag *t, Eina_List *end)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(t);
	thiz->ctx.timing.end = end;
}

static void _egueb_smil_animation_fill_set(Egueb_Dom_Tag *t, Egueb_Smil_Fill fill)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(t);
	thiz->ctx.timing.fill = fill;
}

static void _egueb_smil_animation_fill_get(Egueb_Dom_Tag *t, Egueb_Smil_Fill *fill)
{
	Egueb_Smil_Animation *thiz;

	if (!fill) return;
	thiz = EGUEB_SMIL_ANIMATION(t);
	*fill = thiz->ctx.timing.fill;
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
	Egueb_Dom_Node *p;
	Egueb_Dom_String *attribute_name = NULL;

	/* check that we have an etch */
	if (!thiz->etch)
	{
		ERR("No etch set");
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
	/* get the property from the element */
	p = egueb_dom_element_property_fetch(target, attribute_name);
	if (!p)
	{
		ERR("No Property '%s' found",
				egueb_dom_string_string_get(attribute_name));
		egueb_dom_string_unref(attribute_name);
		return EINA_FALSE;
	}
	egueb_dom_string_unref(attribute_name);
	if (thiz->p)
	{
		egueb_dom_node_unref(thiz->p);
		thiz->p = NULL;
	}

	thiz->p = p;
	thiz->d = egueb_dom_attr_value_descriptor_get(p);
	/* do the begin and end conditions */
	_egueb_smil_animation_begin_setup(thiz);
	_egueb_smil_animation_end_setup(thiz);

	klass = EGUEB_SMIL_ANIMATION_CLASS_GET(thiz);
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
	if (thiz->p)
	{
		egueb_dom_node_unref(thiz->p);
		thiz->p = NULL;
	}
	thiz->target = NULL;
}

static void _egueb_dom_animation_node_removed_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Smil_Animation *thiz;
	Egueb_Dom_Node *n;
	Egueb_Dom_Node *related;
	Egueb_Dom_Event_Phase phase;

	phase = egueb_dom_event_phase_get(ev);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	n = egueb_dom_event_target_get(ev);
	related = egueb_dom_event_mutation_related_get(ev);

	thiz = EGUEB_SMIL_ANIMATION(n);
	_egueb_dom_animation_cleanup(thiz, related);
	egueb_dom_node_unref(n);
	egueb_dom_node_unref(related);
}
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_smil_animation_process(Egueb_Dom_Element *e)
{
	Egueb_Smil_Animation *thiz;
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_String *xlink_href = NULL;
	Eina_Bool ret;

	thiz = EGUEB_SMIL_ANIMATION(e);
	egueb_dom_attr_final_get(thiz->xlink_href, &xlink_href);
	if (egueb_dom_string_is_valid(xlink_href))
	{
		Egueb_Dom_Node *doc;

		doc = egueb_dom_node_document_get(EGUEB_DOM_NODE(e));
		if (!doc)
		{
			ERR("No document associated with the node");
			return EINA_FALSE;
		}
		target = egueb_dom_document_element_get_by_id(doc, xlink_href, NULL);
		if (!target)
		{
			ERR("Invalid target");
			egueb_dom_node_unref(doc);
			egueb_dom_string_unref(xlink_href);
			return EINA_FALSE;
		}
		egueb_dom_string_unref(xlink_href);
		egueb_dom_node_unref(doc);
		ERR("we have an xlink!");
	}
	else
	{
		target = egueb_dom_node_parent_get(EGUEB_DOM_NODE(e));
		egueb_dom_string_unref(xlink_href);
	}

	if (!egueb_dom_element_changed(EGUEB_DOM_NODE(e)) &&
			thiz->target == target && !thiz->document_changed)
	{
		egueb_dom_node_unref(target);
		return EINA_TRUE;
	}

	if (!target) return EINA_FALSE;
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

	klass = EGUEB_DOM_ELEMENT_CLASS(k);
	klass->process = _egueb_smil_animation_process;
}

static void _egueb_smil_animation_instance_init(void *o)
{
	Egueb_Smil_Animation *thiz;
	Egueb_Dom_Node *n;

	/* add a callback whenever the node has been removed from another */
	n = EGUEB_DOM_NODE(o);
	/* FIXME this is only valid for parent target, not for xlink:href */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_dom_animation_node_removed_cb,
			EINA_TRUE, NULL);

	thiz = EGUEB_SMIL_ANIMATION(o);
	thiz->attribute_name = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SMIL_ATTRIBUTE_NAME), NULL);
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
			NULL);

	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->attribute_name), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->fill), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->dur), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->begin), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->end), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->xlink_href), NULL);
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
	egueb_dom_node_unref(thiz->xlink_href);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
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

EAPI Etch * egueb_smil_animation_etch_get(Egueb_Dom_Node *n)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(n);
	return thiz->etch;
}

EAPI void egueb_smil_animation_etch_set(Egueb_Dom_Node *n, Etch *etch)
{
	Egueb_Smil_Animation *thiz;

	thiz = EGUEB_SMIL_ANIMATION(n);
	thiz->etch = etch;
	thiz->document_changed = EINA_TRUE;
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
