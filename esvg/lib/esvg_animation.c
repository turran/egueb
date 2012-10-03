/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_attribute_animation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_animation.h"
#include "esvg_private_svg.h"

#include "esvg_animation.h"
#include "esvg_svg.h"

/*
 * This file handles the common attribute handling of every
 * element on the animation system. That is:
 * 'set', 'animate', 'animateTransform'
 * TODO whenever the this element is reparented, remove/add the attribute
 * of the list of animated attributes
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_ANIMATION_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_ANIMATION_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_ANIMATION_MAGIC);\
	} while(0)

static Ender_Property *ESVG_ANIMATION_ATTRIBUTE_NAME;
static Ender_Property *ESVG_ANIMATION_ATTRIBUTE_TYPE;
static Ender_Property *ESVG_ANIMATION_ADDITIVE;
static Ender_Property *ESVG_ANIMATION_ACCUMULATE;
static Ender_Property *ESVG_ANIMATION_REPEAT_COUNT;
static Ender_Property *ESVG_ANIMATION_DUR;
static Ender_Property *ESVG_ANIMATION_BEGIN;
static Ender_Property *ESVG_ANIMATION_END;

typedef struct _Esvg_Animation_Descriptor_Internal
{
	Edom_Tag_Free free;
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	Edom_Tag_Attribute_Get attribute_get;
	Esvg_Animation_Setup setup;
	Esvg_Animation_Enable enable;
	Esvg_Animation_Disable disable;
} Esvg_Animation_Descriptor_Internal;

typedef struct _Esvg_Animation
{
	EINA_MAGIC
	/* properties */
	Esvg_Animation_Context ctx;
	/* interface */
	Esvg_Animation_Descriptor_Internal descriptor;
	/* private */
	Edom_Tag *thiz_t;
	Ender_Element *thiz_e;
	Eina_List *begin_events;
	Eina_List *end_events;
	Eina_Bool started;
	Eina_Bool attribute_name_changed : 1;
	void *data;
} Esvg_Animation;

typedef struct _Esvg_Animation_Handler
{
	Esvg_Animation *thiz;
	Esvg_Animation_Event *ev;
	Ender_Listener *l;
} Esvg_Animation_Handler;

static Esvg_Animation * _esvg_animation_get(Edom_Tag *t)
{
	Esvg_Animation *thiz;

	thiz = esvg_element_data_get(t);
	ESVG_ANIMATION_MAGIC_CHECK(thiz);

	return thiz;
}

static void _esvg_animation_begin(Esvg_Animation *thiz, int64_t offset)
{
	Ender_Element *svg;
	double seconds;

	if (thiz->started)
		return;
	thiz->started = EINA_TRUE;
	esvg_element_internal_topmost_get(thiz->thiz_t, &svg);
	seconds = esvg_svg_time_get(svg);
	/* TODO maybe add 1 frame? */
	offset = (ESVG_CLOCK_SECONDS * seconds) + offset;
	if (thiz->descriptor.enable)
		thiz->descriptor.enable(thiz->thiz_t, offset);
}

static void _esvg_animation_begin_cb(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Animation_Handler *h = data;
	Esvg_Animation *thiz = h->thiz;

	/* call the begin interface */
	_esvg_animation_begin(thiz, h->ev->offset);
}

static void _esvg_animation_end_cb(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_Animation_Handler *h = data;
	Esvg_Animation *thiz = h->thiz;

	/* call the end interface */
	if (!thiz->started)
		return;
	printf(">>>> end cb %p\n", thiz->descriptor.disable);
	if (thiz->descriptor.disable)
		thiz->descriptor.disable(thiz->thiz_t);
}

static Eina_Bool _esvg_animation_event_setup(Esvg_Animation *thiz, Eina_List *events,
		Eina_List **phandlers, Ender_Event_Callback cb)
{
	Esvg_Animation_Event *ae;
	Eina_List *handlers = NULL;
	Eina_List *l;

	/* check the begin conditions and register the needed events */
	EINA_LIST_FOREACH (events, l, ae)
	{
		if (ae->event)
		{
			Ender_Element *ref = NULL;

			printf("registering event %s\n", ae->event);
			if (ae->id)
			{
				printf("on id %s\n", ae->id);
			}
			else
			{
				if (ae->repeat)
					ref = thiz->thiz_e;
				else
					ref = thiz->ctx.parent_e;
			}

			if (ref)
			{
				Esvg_Animation_Handler *h;

				h = calloc(1, sizeof(Esvg_Animation_Handler));
				h->l = ender_event_listener_add(ref, ae->event, cb, h);
				h->thiz = thiz;
				h->ev = ae;
				handlers = eina_list_append(handlers, h);
			}

		}
		else
		{

		}
		printf("with offset %lld\n", ae->offset);
	}
	*phandlers = handlers;

	return EINA_TRUE;
}

static void _esvg_animation_event_release(Eina_List *events)
{
	Esvg_Animation_Handler *h;

	EINA_LIST_FREE(events, h)
	{
		ender_event_listener_remove(h->l);
		free(h);
	}
}

static Eina_Bool _esvg_animation_begin_setup(Esvg_Animation *thiz)
{
	if (!_esvg_animation_event_setup(thiz, thiz->ctx.timing.begin,
			&thiz->begin_events, _esvg_animation_begin_cb))
		return EINA_FALSE;
	/* in case there is no event to trigger, just start now */
	if (!thiz->begin_events)
	{
		/* call the begin interface */
		/* FIXME fix the offset */
		if (thiz->descriptor.enable)
			thiz->descriptor.enable(thiz->thiz_t, 0);
	}
	return EINA_TRUE;
}

static void _esvg_animation_begin_release(Esvg_Animation *thiz)
{
	_esvg_animation_event_release(thiz->begin_events);
}

static Eina_Bool _esvg_animation_end_setup(Esvg_Animation *thiz)
{
	if (!_esvg_animation_event_setup(thiz, thiz->ctx.timing.end,
			&thiz->end_events, _esvg_animation_end_cb))
		return EINA_FALSE;
	return EINA_TRUE;
}

static void _esvg_animation_end_release(Esvg_Animation *thiz)
{
	_esvg_animation_event_release(thiz->end_events);
}

static Eina_Bool _esvg_animation_attribute_name_setup(Esvg_Animation *thiz)
{
	Esvg_Attribute_Animation_Attribute_Name *attr_name;
	Esvg_Animation_Context *ctx;
	Eina_Bool ret = EINA_TRUE;

	ctx = &thiz->ctx;
	attr_name = &ctx->target.attribute_name;
	/* check the attribute name if it has changed then keep
	 * track of the attribute being animated
	 */
	if (!esvg_string_is_equal(attr_name->curr, attr_name->prev))
	{
		if (attr_name->prev)
		{
			esvg_element_attribute_animation_remove(ctx->parent_t, attr_name->prev);
			free(attr_name->prev);
			attr_name->prev = NULL;
			ctx->p = NULL;
			/* TODO remove every event set */
		}
		if (attr_name->curr)
		{
			Ender_Property *p;
			Esvg_Attribute_Type old_type;

			/* set the attribute to animate */
			old_type = esvg_element_attribute_type_get(ctx->parent_t);
			esvg_element_attribute_type_set(ctx->parent_t, thiz->ctx.target.attribute_type);
			esvg_element_attribute_animation_add(ctx->parent_t, attr_name->curr);
			/* restore the attribute  */
			esvg_element_attribute_type_set(ctx->parent_t, old_type);
			attr_name->prev = attr_name->curr;

			/* get the property name */
			p = ender_element_property_get(ctx->parent_e, ctx->target.attribute_name.curr);
			if (!p)
			{
				ret = EINA_FALSE;
				goto done;
			}
			ctx->p = p;
		}
	}
done:
	thiz->attribute_name_changed = EINA_FALSE;
	return ret;
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _esvg_animation_attribute_name_set(Edom_Tag *t, const char *attribute_name)
{
	Esvg_Animation *thiz;
	Esvg_Attribute_Animation_Attribute_Name *attr_name;

	thiz = _esvg_animation_get(t);
	attr_name = &thiz->ctx.target.attribute_name;
	if (attr_name->curr)
	{
		free(attr_name->curr);
		attr_name->curr = NULL;
	}
	if (attribute_name)
		attr_name->curr = strdup(attribute_name);
	thiz->attribute_name_changed = EINA_TRUE;
}

static void _esvg_animation_attribute_name_get(Edom_Tag *t, const char **attribute_name)
{
	Esvg_Animation *thiz;
	Esvg_Attribute_Animation_Attribute_Name *attr_name;

	if (!attribute_name) return;
	thiz = _esvg_animation_get(t);
	attr_name = &thiz->ctx.target.attribute_name;
	*attribute_name = attr_name->curr;
}

static void _esvg_animation_attribute_type_set(Edom_Tag *t, Esvg_Attribute_Type attribute_type)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	thiz->ctx.target.attribute_type = attribute_type;
}

static void _esvg_animation_attribute_type_get(Edom_Tag *t, Esvg_Attribute_Type *attribute_type)
{
	Esvg_Animation *thiz;

	if (!attribute_type) return;
	thiz = _esvg_animation_get(t);
	*attribute_type = thiz->ctx.target.attribute_type;
}

static void _esvg_animation_dur_set(Edom_Tag *t, Esvg_Duration *dur)
{
	Esvg_Animation *thiz;

	if (!dur) return;
	thiz = _esvg_animation_get(t);
	thiz->ctx.timing.dur = *dur;
}

static void _esvg_animation_dur_get(Edom_Tag *t, Esvg_Duration *dur)
{
	Esvg_Animation *thiz;

	if (!dur) return;
	thiz = _esvg_animation_get(t);
	*dur = thiz->ctx.timing.dur;
}

static void _esvg_animation_additive_set(Edom_Tag *t, Esvg_Additive additive)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	thiz->ctx.addition.additive = additive;
}

static void _esvg_animation_additive_get(Edom_Tag *t, Esvg_Additive *additive)
{
	Esvg_Animation *thiz;

	if (!additive) return;
	thiz = _esvg_animation_get(t);
	*additive = thiz->ctx.addition.additive;
}

static void _esvg_animation_accumulate_set(Edom_Tag *t, Esvg_Accumulate accumulate)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	thiz->ctx.addition.accumulate = accumulate;
}

static void _esvg_animation_accumulate_get(Edom_Tag *t, Esvg_Accumulate *accumulate)
{
	Esvg_Animation *thiz;

	if (!accumulate) return;
	thiz = _esvg_animation_get(t);
	*accumulate = thiz->ctx.addition.accumulate;
}

static void _esvg_animation_repeat_count_set(Edom_Tag *t, int repeat_count)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	thiz->ctx.timing.repeat_count = repeat_count;
}

static void _esvg_animation_repeat_count_get(Edom_Tag *t, int *repeat_count)
{
	Esvg_Animation *thiz;

	if (!repeat_count) return;
	thiz = _esvg_animation_get(t);
	*repeat_count = thiz->ctx.timing.repeat_count;
}

static void _esvg_animation_begin_set(Edom_Tag *t, Eina_List *begin)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	thiz->ctx.timing.begin = begin;
}

static void _esvg_animation_end_set(Edom_Tag *t, Eina_List *end)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	thiz->ctx.timing.end = end;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_animation_initialize(Ender_Element *e)
{
	Esvg_Animation *thiz;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_animation_get(t);
	thiz->thiz_e = e;

	/* call the interface */
	if (thiz->descriptor.initialize)
		thiz->descriptor.initialize(e);
}

static Eina_Bool _esvg_animation_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	/* target attributes */
	if (strcmp(key, "attributeName") == 0)
	{
		esvg_animation_attribute_name_set(e, value);
	}
	else if (strcmp(key, "attributeType") == 0)
	{
		Esvg_Attribute_Type type;

		if (esvg_attribute_type_string_from(&type, value))
			esvg_animation_attribute_type_set(e, type);
	}
	/* timing attributes */
	else if (!strcmp(key, "dur"))
	{
		Esvg_Duration dur;

		esvg_duration_string_from(&dur, value);
		esvg_animation_dur_set(e, &dur);
	}
	else if (!strcmp(key, "begin"))
	{
		Eina_List *begin = NULL;

		esvg_animation_event_list_string_from(&begin, value);
		esvg_animation_begin_set(e, begin);
	}
	else if (!strcmp(key, "end"))
	{
		Eina_List *end = NULL;

		esvg_animation_event_list_string_from(&end, value);
		esvg_animation_end_set(e, end);
	}
	/* addition attributes */
	else if (!strcmp(key, "additive"))
	{
		Esvg_Additive add;

		esvg_additive_string_from(&add, value);
		esvg_animation_additive_set(e, &add);
	}
	else if (!strcmp(key, "accumulate"))
	{
		Esvg_Accumulate acc;

		esvg_accumulate_string_from(&acc, value);
		esvg_animation_accumulate_set(e, &acc);
	}
	else if (!strcmp(key, "repeatCount"))
	{
		int rc;

		esvg_repeat_count_string_from(&rc, value);
		esvg_animation_repeat_count_set(e, rc);
	}
	else
	{
		Esvg_Animation *thiz;
		Edom_Tag *t;

		t = ender_element_object_get(e);
		thiz = _esvg_animation_get(t);
		if (thiz->descriptor.attribute_set)
			return thiz->descriptor.attribute_set(e, key, value);
		return EINA_FALSE;
	}
	

	return EINA_TRUE;
}

static Eina_Bool _esvg_animation_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static void _esvg_animation_free(Edom_Tag *t)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}

/* TODO optimize so many 'ifs' */
static Esvg_Element_Setup_Return _esvg_animation_setup(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Animation *thiz;
	Esvg_Animation_Context *ctx;
	Edom_Tag *parent_t;
	Ender_Element *parent_e;

	/* every animation needs to do its own setup only once
	 * unless some animation specifc attributes have changed
	 */
	if (!esvg_element_changed(t))
		return ESVG_SETUP_OK;

	thiz = _esvg_animation_get(t);
	ctx = &thiz->ctx;

	/* get the parent */
	ctx->parent_t = edom_tag_parent_get(t);
	if (!ctx->parent_t)
		return ESVG_SETUP_OK;
	ctx->parent_e = esvg_element_ender_get(ctx->parent_t);
	/* check if the name has changed */
	if (thiz->attribute_name_changed)
	{
		if (!_esvg_animation_attribute_name_setup(thiz))
			return ESVG_SETUP_OK;
	}

	_esvg_animation_begin_release(thiz);
	_esvg_animation_end_release(thiz);

	/* generate every animation */
	if (thiz->descriptor.setup)
	{
		if (!thiz->descriptor.setup(t, c, &thiz->ctx, error))
			return ESVG_SETUP_OK;
	}

	/* do the begin and end conditions */
	_esvg_animation_begin_setup(thiz);
	_esvg_animation_end_setup(thiz);

	return ESVG_SETUP_OK;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_animation_delete NULL
#define _esvg_animation_attribute_name_is_set NULL
#define _esvg_animation_attribute_type_is_set NULL
#define _esvg_animation_additive_is_set NULL
#define _esvg_animation_accumulate_is_set NULL
#define _esvg_animation_repeat_count_is_set NULL
#define _esvg_animation_dur_is_set NULL
#define _esvg_animation_begin_get NULL
#define _esvg_animation_begin_is_set NULL
#define _esvg_animation_begin_clear NULL
#define _esvg_animation_begin_remove NULL
#define _esvg_animation_begin_add NULL
#define _esvg_animation_end_get NULL
#define _esvg_animation_end_is_set NULL
#define _esvg_animation_end_clear NULL
#define _esvg_animation_end_remove NULL
#define _esvg_animation_end_add NULL
#include "generated/esvg_generated_animation.c"

Eina_Bool esvg_is_animation_internal(Edom_Tag *t)
{
	Esvg_Animation *thiz;
	Eina_Bool ret;

	if (!esvg_is_element_internal(t))
		return EINA_FALSE;
	thiz = esvg_element_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_ANIMATION_MAGIC);

	return ret;
}

void * esvg_animation_data_get(Edom_Tag *t)
{
	Esvg_Animation *thiz;

	thiz = _esvg_animation_get(t);
	return thiz->data;
}

Edom_Tag * esvg_animation_new(Esvg_Animation_Descriptor *descriptor, Esvg_Type type,
		void *data)
{
	Esvg_Animation *thiz;
	Esvg_Element_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Animation));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_ANIMATION_MAGIC);
	thiz->data = data;
	/* our own descriptor */
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->descriptor.attribute_get = descriptor->attribute_get;
	thiz->descriptor.initialize = descriptor->initialize;
	thiz->descriptor.enable = descriptor->enable;
	thiz->descriptor.disable = descriptor->disable;
	/* default values */
	thiz->ctx.timing.repeat_count = 1;

	pdescriptor.attribute_set = _esvg_animation_attribute_set;
	pdescriptor.attribute_get = _esvg_animation_attribute_get;
	pdescriptor.free = _esvg_animation_free;
	pdescriptor.initialize = _esvg_animation_initialize;
	pdescriptor.setup = _esvg_animation_setup;
	pdescriptor.cdata_set = NULL;
	pdescriptor.text_set = NULL;
	pdescriptor.text_get = NULL;

	t = esvg_element_new(&pdescriptor, type, thiz);
	/* store the tag */
	thiz->thiz_t = t;

	return t;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_animation(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_animation_internal(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_attribute_name_set(Ender_Element *e, const char *name)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_ATTRIBUTE_NAME, name, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_attribute_name_get(Ender_Element *e, const char **name)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animation_attribute_name_get(t, name);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_attribute_type_set(Ender_Element *e, Esvg_Attribute_Type type)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_ATTRIBUTE_TYPE, type, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_attribute_type_get(Ender_Element *e, Esvg_Attribute_Type *type)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animation_attribute_type_get(t, type);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_dur_set(Ender_Element *e, Esvg_Duration *dur)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_DUR, dur, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_dur_get(Ender_Element *e, Esvg_Duration *dur)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animation_dur_get(t, dur);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_additive_set(Ender_Element *e, Esvg_Additive *additive)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_ADDITIVE, additive, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_additive_get(Ender_Element *e, Esvg_Additive *additive)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animation_additive_get(t, additive);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_accumulate_set(Ender_Element *e, Esvg_Accumulate *accumulate)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_ACCUMULATE, accumulate, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_accumulate_get(Ender_Element *e, Esvg_Accumulate *accumulate)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animation_accumulate_get(t, accumulate);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_repeat_count_set(Ender_Element *e, int repeat_count)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_REPEAT_COUNT, repeat_count, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_repeat_count_get(Ender_Element *e, int *repeat_count)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animation_repeat_count_get(t, repeat_count);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_begin_set(Ender_Element *e, Eina_List *begin)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_BEGIN, begin, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animation_end_set(Ender_Element *e, Eina_List *end)
{
	ender_element_property_value_set(e, ESVG_ANIMATION_END, end, NULL);
}

