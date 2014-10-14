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

#include "egueb_smil_clock.h"
#include "egueb_smil_animation.h"
#include "egueb_smil_feature_animation.h"
#include "egueb_smil_event_timeline.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_feature_private.h"
#include "egueb_smil_keyframe_private.h"
#include "egueb_smil_timeline_private.h"
#include "egueb_smil_signal_private.h"
#include "egueb_smil_timeline_private.h"
#include "egueb_smil_animation_private.h"
#include "egueb_smil_event_timeline_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SMIL_FEATURE_ANIMATION_DESCRIPTOR egueb_smil_feature_animation_descriptor_get()
#define EGUEB_SMIL_FEATURE_ANIMATION(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Smil_Feature_Animation, EGUEB_SMIL_FEATURE_ANIMATION_DESCRIPTOR)

typedef struct _Egueb_Smil_Feature_Animation
{
	Egueb_Dom_Feature base;
	Egueb_Dom_Node *n;
	Egueb_Smil_Timeline *timeline;
	Egueb_Smil_Feature_Animation_Cb on_tick;
	Eina_Bool needs_dur_calc;
	Eina_List *animations;
} Egueb_Smil_Feature_Animation;

typedef struct _Egueb_Smil_Feature_Animation_Class
{
	Egueb_Dom_Feature_Class base;
} Egueb_Smil_Feature_Animation_Class;

static Egueb_Dom_String _EGUEB_SMIL_FEATURE_ANIMATION_NAME = EGUEB_DOM_STRING_STATIC("EguebSmilAnimation");

static void _egueb_smil_feature_animation_changed_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Smil_Feature_Animation *thiz = EGUEB_SMIL_FEATURE_ANIMATION(data);
	thiz->needs_dur_calc = EINA_TRUE;
}

static void _egueb_smil_feature_animation_insterted_into_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Smil_Feature_Animation *thiz = EGUEB_SMIL_FEATURE_ANIMATION(data);
	Egueb_Dom_Node *animation;

	animation = egueb_dom_event_target_get(ev);
	if (!egueb_smil_is_animation(animation))
	{
		egueb_dom_node_unref(animation);
		return;
	}
	egueb_dom_node_event_listener_add(animation,
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_smil_feature_animation_changed_cb,
			EINA_TRUE, thiz);
	thiz->animations = eina_list_append(thiz->animations, animation);
	thiz->needs_dur_calc = EINA_TRUE;
}

static void _egueb_smil_feature_animation_removed_from_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Smil_Feature_Animation *thiz = EGUEB_SMIL_FEATURE_ANIMATION(data);
	Egueb_Dom_Node *animation;

	animation = egueb_dom_event_target_get(ev);
	if (!egueb_smil_is_animation(animation))
	{
		egueb_dom_node_unref(animation);
		return;
	}
	egueb_dom_node_event_listener_remove(animation,
			EGUEB_DOM_EVENT_MUTATION_ATTR_MODIFIED,
			_egueb_smil_feature_animation_changed_cb,
			EINA_TRUE, thiz);
	thiz->animations = eina_list_remove(thiz->animations, animation);
	thiz->needs_dur_calc = EINA_TRUE;
}

static void _egueb_smil_feature_animation_timeline_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Smil_Feature_Animation *thiz = EGUEB_SMIL_FEATURE_ANIMATION(data);

	INFO("Requesting timeline");
	egueb_smil_event_timeline_set(e, egueb_smil_timeline_ref(thiz->timeline));
}

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_FEATURE_DESCRIPTOR,
		Egueb_Smil_Feature_Animation, Egueb_Smil_Feature_Animation_Class,
		egueb_smil_feature_animation);

static void _egueb_smil_feature_animation_class_init(void *k)
{
}

static void _egueb_smil_feature_animation_instance_init(void *o)
{
	Egueb_Smil_Feature_Animation *thiz = EGUEB_SMIL_FEATURE_ANIMATION(o);
	thiz->needs_dur_calc = EINA_TRUE;
	/* the animation system */
	thiz->timeline = egueb_smil_timeline_new();
}

static void _egueb_smil_feature_animation_instance_deinit(void *o)
{
	Egueb_Smil_Feature_Animation *thiz = EGUEB_SMIL_FEATURE_ANIMATION(o);

	if (thiz->animations)
	{
		Egueb_Dom_Node *animation;
		WARN("Refcount issue");
		EINA_LIST_FREE(thiz->animations, animation)
			egueb_dom_node_unref(animation);
	}
	/* remove timeline and all the animation system  */
	egueb_smil_timeline_unref(thiz->timeline);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_SMIL_FEATURE_ANIMATION_NAME = &_EGUEB_SMIL_FEATURE_ANIMATION_NAME;

EAPI Eina_Bool egueb_smil_feature_animation_fps_set(Egueb_Dom_Feature *f, int fps)
{
	Egueb_Smil_Feature_Animation *thiz;

	thiz = EGUEB_SMIL_FEATURE_ANIMATION(f);
	egueb_smil_timeline_fps_set(thiz->timeline, fps);
	return EINA_TRUE;
}

EAPI Eina_Bool egueb_smil_feature_animation_fps_get(Egueb_Dom_Feature *f, int *fps)
{
	Egueb_Smil_Feature_Animation *thiz;

	thiz = EGUEB_SMIL_FEATURE_ANIMATION(f);
	*fps = egueb_smil_timeline_fps_get(thiz->timeline);
	return EINA_TRUE;
}

EAPI Eina_Bool egueb_smil_feature_animation_tick(Egueb_Dom_Feature *f)
{
	Egueb_Smil_Feature_Animation *thiz;

	thiz = EGUEB_SMIL_FEATURE_ANIMATION(f);
	egueb_smil_timeline_tick(thiz->timeline);
	if (thiz->on_tick)
		thiz->on_tick(thiz->n);
	return EINA_TRUE;
}

EAPI Eina_Bool egueb_smil_feature_animation_duration_get(Egueb_Dom_Feature *f,
		Egueb_Smil_Clock *clock)
{
	Egueb_Smil_Feature_Animation *thiz;
	Eina_Bool ret = EINA_FALSE;

	thiz = EGUEB_SMIL_FEATURE_ANIMATION(f);
	/* initialize to a sane value */
	*clock = -1;
	if (!thiz->animations)
	{
		return EINA_FALSE;
	}
	/* get the max duration of every animation */
	else
	{
		Egueb_Dom_Node *animation;
		Eina_List *l;

		EINA_LIST_FOREACH(thiz->animations, l, animation)
		{
			Egueb_Smil_Clock aclock;

			ret = egueb_smil_animation_active_duration_get(animation, &aclock);
			if (!ret) break;

			/* get the max of every clock */
			if (aclock > *clock)
				*clock = aclock;
		}
	}
	return ret;
}

EAPI Eina_Bool egueb_smil_feature_animation_has_animations(Egueb_Dom_Feature *f)
{
	Egueb_Smil_Feature_Animation *thiz;

	thiz = EGUEB_SMIL_FEATURE_ANIMATION(f);
	if (thiz->animations)
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

EAPI void egueb_smil_feature_animation_on_tick_set(Egueb_Dom_Feature *f,
		Egueb_Smil_Feature_Animation_Cb on_tick)
{
	Egueb_Smil_Feature_Animation *thiz;

	thiz = EGUEB_SMIL_FEATURE_ANIMATION(f);
	thiz->on_tick = on_tick;
}

EAPI Eina_Bool egueb_smil_feature_animation_add(Egueb_Dom_Node *n)
{
	Egueb_Smil_Feature_Animation *thiz;

	if (!n) return EINA_FALSE;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_feature_animation);
	egueb_dom_node_weak_ref_add(n, &thiz->n);

	/* add the events needed to calculate the duration */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT,
			_egueb_smil_feature_animation_insterted_into_document_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
			_egueb_smil_feature_animation_removed_from_document_cb,
			EINA_TRUE, thiz);
	/* add the event to set the timeline */
	egueb_dom_node_event_listener_add(n,
			EGUEB_SMIL_EVENT_TIMELINE,
			_egueb_smil_feature_animation_timeline_cb,
			EINA_TRUE, thiz);

	return egueb_dom_node_feature_add(n, EGUEB_SMIL_FEATURE_ANIMATION_NAME,
			NULL, EGUEB_DOM_FEATURE(thiz));
}

