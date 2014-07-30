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
#include "egueb_smil_set.h"
#include "egueb_smil_clock.h"
#include "egueb_smil_keyframe.h"
#include "egueb_smil_timeline.h"
#include "egueb_smil_signal.h"
#include "egueb_smil_event.h"

#include "egueb_smil_animation_private.h"

/* We can optimize this, as there's no need to interpolate any value
 * during the set lifetime .... we need to know what to do with "sandwich"
 * animations/sets
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SMIL_SET_DESCRIPTOR egueb_smil_set_descriptor_get()
#define EGUEB_SMIL_SET_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, Egueb_Smil_Set_Class, EGUEB_SMIL_SET_DESCRIPTOR)
#define EGUEB_SMIL_SET(o) ENESIM_OBJECT_INSTANCE_CHECK(o, Egueb_Smil_Set, EGUEB_SMIL_SET_DESCRIPTOR)

typedef struct _Egueb_Smil_Set
{
	Egueb_Smil_Animation parent;
	/* properties */
	Egueb_Dom_Node *to;
	/* private */
	Egueb_Dom_Value to_value;
	Egueb_Dom_Value dst_value;
	Egueb_Dom_Value prv_value;
	/* timeline related data */
	Egueb_Smil_Signal *signal;
} Egueb_Smil_Set;

typedef struct _Egueb_Smil_Set_Class
{
	Egueb_Smil_Animation_Class parent;
} Egueb_Smil_Set_Class;

static void _egueb_smil_set_property_set(Egueb_Smil_Set *thiz, Egueb_Dom_Value *v)
{
	Egueb_Smil_Animation *a;

	a = EGUEB_SMIL_ANIMATION(thiz);
	egueb_dom_attr_value_set(a->attr, EGUEB_DOM_ATTR_TYPE_ANIMATED, v);
}
/*----------------------------------------------------------------------------*
 *                        The Etch external animation                         *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_set_animation_cb(Egueb_Dom_Value *v, void *data)
{
	Egueb_Smil_Set *thiz = data;
	_egueb_smil_set_property_set(thiz, v);
}

static void _egueb_smil_set_animation_start_cb(Egueb_Smil_Signal *s, void *data)
{
	Egueb_Smil_Set *thiz = data;
	Egueb_Smil_Animation *a;
	Egueb_Smil_Fill fill;
	Egueb_Dom_Event *ev;

	a = EGUEB_SMIL_ANIMATION(thiz);
	egueb_dom_attr_get(a->fill, EGUEB_DOM_ATTR_TYPE_BASE, &fill);
	/* in case of "remove" pick the last value it had */
	if (fill == EGUEB_SMIL_FILL_REMOVE)
	{
		egueb_dom_attr_final_value_get(a->attr, &thiz->prv_value);
	}
	ev = egueb_smil_event_new();
	egueb_smil_event_init(ev, egueb_dom_string_ref(EGUEB_SMIL_EVENT_BEGIN), 0);
	egueb_dom_node_event_dispatch(a->target, ev, NULL, NULL);
}

static void _egueb_smil_set_animation_stop_cb(Egueb_Smil_Signal *s, void *data)
{
	Egueb_Smil_Set *thiz = data;
	Egueb_Smil_Animation *a;
	Egueb_Smil_Fill fill;
	Egueb_Dom_Event *ev;

	a = EGUEB_SMIL_ANIMATION(thiz);
	egueb_dom_attr_get(a->fill, EGUEB_DOM_ATTR_TYPE_BASE, &fill);
	/* in case of "remove" set the value it had when the animation started */
	if (fill == EGUEB_SMIL_FILL_REMOVE)
	{
		DBG("Going back to previous value");
		_egueb_smil_set_property_set(thiz, &thiz->prv_value);
	}
	ev = egueb_smil_event_new();
	egueb_smil_event_init(ev, egueb_dom_string_ref(EGUEB_SMIL_EVENT_END), 0);
	egueb_dom_node_event_dispatch(a->target, ev, NULL, NULL);
}
/*----------------------------------------------------------------------------*
 *                             Animation interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_smil_set_setup(Egueb_Smil_Animation *a,
		Egueb_Dom_Node *target)
{
	Egueb_Smil_Set *thiz;
	Egueb_Dom_String *to = NULL;

	thiz = EGUEB_SMIL_SET(a);
	egueb_dom_attr_final_get(thiz->to, &to);
	if (!to)
	{
		ERR("No 'set' attribute set");
		return EINA_FALSE;
	}

	egueb_dom_value_init(&thiz->to_value, a->d);
	if (!egueb_dom_value_string_from(&thiz->to_value, to))
	{
		ERR("No valid 'set' value '%s'", egueb_dom_string_string_get(to));
		egueb_dom_string_unref(to);
		return EINA_FALSE;
	}
	egueb_dom_string_unref(to);
	/* setup the holder of the destination value */
	egueb_dom_value_init(&thiz->dst_value, egueb_dom_attr_value_descriptor_get(a->attr));
	egueb_dom_value_copy(&thiz->to_value, &thiz->dst_value, EINA_TRUE);

	thiz->signal = egueb_smil_signal_discrete_new(
			_egueb_smil_set_animation_cb,
			_egueb_smil_set_animation_start_cb,
			_egueb_smil_set_animation_stop_cb,
			&thiz->dst_value, thiz);
	egueb_smil_timeline_signal_add(a->timeline, egueb_smil_signal_ref(thiz->signal));
	/* TODO the repeat count */

	return EINA_TRUE;
}

static void _egueb_smil_set_cleanup(Egueb_Smil_Animation *a,
		Egueb_Dom_Node *target)
{
	Egueb_Smil_Set *thiz;

	thiz = EGUEB_SMIL_SET(a);
	egueb_dom_value_reset(&thiz->to_value);
	egueb_dom_value_reset(&thiz->dst_value);
}

static void _egueb_smil_set_begin(Egueb_Smil_Animation *a, int64_t offset)
{
	Egueb_Smil_Set *thiz;

	thiz = EGUEB_SMIL_SET(a);
	if (!thiz->signal) return;
	DBG("Beginning set at %" EGUEB_SMIL_CLOCK_FORMAT, EGUEB_SMIL_CLOCK_ARGS (offset));
	egueb_smil_signal_offset_set(thiz->signal, offset);
	egueb_smil_signal_enable(thiz->signal);
}

static void _egueb_smil_set_end(Egueb_Smil_Animation *a)
{
	Egueb_Smil_Set *thiz;

	thiz = EGUEB_SMIL_SET(a);
	if (!thiz->signal) return;
	DBG("Ending");
	egueb_smil_signal_disable(thiz->signal);
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_smil_set_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SMIL_NAME_SET);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SMIL_ANIMATION_DESCRIPTOR,
		Egueb_Smil_Set, Egueb_Smil_Set_Class, egueb_smil_set);

static void _egueb_smil_set_class_init(void *k)
{
	Egueb_Smil_Animation_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SMIL_ANIMATION_CLASS(k);
	klass->cleanup = _egueb_smil_set_cleanup;
	klass->setup = _egueb_smil_set_setup;
	klass->begin = _egueb_smil_set_begin;
	klass->end = _egueb_smil_set_end;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_smil_set_tag_name_get;
}

static void _egueb_smil_set_instance_init(void *o)
{
	Egueb_Smil_Set *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SMIL_SET(o);
	/* create the properties */
	thiz->to = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SMIL_TO), NULL, EINA_FALSE,
			EINA_FALSE, EINA_FALSE);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->to), NULL);
}

static void _egueb_smil_set_instance_deinit(void *o)
{
	Egueb_Smil_Set *thiz;

	thiz = EGUEB_SMIL_SET(o);

	/* destroy the properties */
	egueb_dom_node_unref(thiz->to);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_smil_set_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_set);
	return n;
}

#if 0
EAPI void egueb_smil_set_to_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ELEMENT_SET_TO, v, NULL);
}

EAPI void egueb_smil_set_to_get(Ender_Element *e, const char **v)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_set_to_get(t, v);
}
#endif
