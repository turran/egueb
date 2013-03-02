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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_attribute_animation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_animation.h"
#include "esvg_private_svg.h"

#include "esvg_element_set.h"
#include "esvg_element_animation.h"

/* We can optimize this, as there's no need to interpolate any value
 * during the set lifetime .... we need to know what to do with "sandwich"
 * animations/sets
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_element_set_log

static int _esvg_element_set_log = -1;

static Ender_Property *ESVG_ELEMENT_SET_TO;

typedef struct _Esvg_Set
{
	/* properties */
	char *to;
	/* interface */
	/* private */
	Ender_Element *thiz_e;
	Esvg_Attribute_Type attribute_type;
	Esvg_Attribute_Animated_Descriptor *d;
	Eina_List *values;
	void *destination_data;
	void *destination_prev;
	/* parent relation */
	Ender_Element *parent_e;
	Edom_Tag *parent_t;
	Ender_Property *p;
	/* etch related data */
	Etch *etch;
	Etch_Animation *etch_a;
} Esvg_Element_Set;

static Esvg_Element_Set * _esvg_element_set_get(Edom_Tag *t)
{
	Esvg_Element_Set *thiz;

	thiz = esvg_element_animation_data_get(t);

	return thiz;
}

static void _esvg_element_set_cleanup(Esvg_Element_Set *thiz)
{

}

static void _esvg_element_set_interpolator_cb(Etch_Data *a, Etch_Data *b, double m, Etch_Data *res, void *data)
{
	Esvg_Element_Set *thiz = data;

	thiz->d->interpolator(a->data.external, b->data.external, m, NULL, NULL, 0, res->data.external);
}

static void _esvg_element_set_property_set(Esvg_Element_Set *thiz, void *data)
{
	Esvg_Attribute_Type old_type;

	old_type = esvg_element_attribute_type_get(thiz->parent_t);
	esvg_element_attribute_type_set(thiz->parent_t, thiz->attribute_type);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->p, data, NULL);
	/* restore the states */
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
	esvg_element_attribute_type_set(thiz->parent_t, old_type);
}

static void _esvg_element_set_animation_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *user_data)
{
	Esvg_Element_Set *thiz = user_data;
	_esvg_element_set_property_set(thiz, thiz->destination_data);
}

static void _esvg_element_set_animation_start_cb(Etch_Animation *a, void *data)
{
	Esvg_Element_Set *thiz = data;
	Esvg_Fill fill;

	esvg_element_animation_fill_get(thiz->thiz_e, &fill);
	/* in case of "remove" pick the last value it had */
	if (fill == ESVG_FILL_REMOVE)
	{
		void *dp;

		DBG("Storing current value");
		dp = thiz->d->destination_new();
		ender_element_property_value_get(thiz->parent_e, thiz->p, dp, NULL);
		if (thiz->d->destination_keep)
		{
			thiz->d->destination_keep(dp);
		}
		thiz->destination_prev = dp;
	}
	ender_event_dispatch(thiz->thiz_e, "begin", NULL);
}

static void _esvg_element_set_animation_stop_cb(Etch_Animation *a, void *data)
{
	Esvg_Element_Set *thiz = data;
	Esvg_Fill fill;

	esvg_element_animation_fill_get(thiz->thiz_e, &fill);
	/* in case of "remove" set the value it had when the animation started */
	if (fill == ESVG_FILL_REMOVE)
	{
		DBG("Going back to previous value");
		_esvg_element_set_property_set(thiz, thiz->destination_prev);
	}
	ender_event_dispatch(thiz->thiz_e, "end", NULL);
}
/*----------------------------------------------------------------------------*
 *                           The Animation interface                          *
 *----------------------------------------------------------------------------*/
static void _esvg_element_set_enable(Edom_Tag *t, int64_t offset)
{
	Esvg_Element_Set *thiz;

	thiz = _esvg_element_set_get(t);
	if (!thiz->etch_a) return;
	etch_animation_offset_add(thiz->etch_a, offset);
	etch_animation_enable(thiz->etch_a);
}

static void _esvg_element_set_disable(Edom_Tag *t)
{
	Esvg_Element_Set *thiz;

	thiz = _esvg_element_set_get(t);
	if (!thiz->etch_a) return;
	etch_animation_disable(thiz->etch_a);
}

static void _esvg_element_set_initialize(Ender_Element *e)
{
	Esvg_Element_Set *thiz;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_element_set_get(t);
	thiz->thiz_e = e;
}

static void _esvg_element_set_free(Edom_Tag *t)
{
	Esvg_Element_Set *thiz;

	thiz = _esvg_element_set_get(t);
	_esvg_element_set_cleanup(thiz);
	free(thiz);
}

static Eina_Bool _esvg_element_set_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "to") == 0)
	{
		esvg_element_set_to_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _esvg_element_set_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Eina_Bool _esvg_element_set_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Animation_Context *actx,
		Enesim_Log **error)
{
	Esvg_Element_Set *thiz;
	Esvg_Attribute_Animated_Descriptor *d;
	Esvg_Duration *dur = &actx->timing.dur;
	Ender_Element *svg_e;
	Ender_Container *ec;
	Ender_Descriptor *descriptor;
	const Ender_Constraint *cnst;
	Ender_Constraint_Type cnst_type;
	Etch *etch;
	Etch_Data etch_data;
	Etch_Animation *etch_a;
	Etch_Animation_Keyframe *k;
	const char *name;
	void *to_data;

	thiz = _esvg_element_set_get(t);
	_esvg_element_set_cleanup(thiz);

	/* get the etch associated with the topmost svg */
	esvg_element_internal_topmost_get(t, &svg_e);
	if (!svg_e)
		return EINA_FALSE;

	etch = esvg_element_svg_etch_get(svg_e);
	if (!etch)
		return EINA_FALSE;

	/* get the descriptor */
	ec = ender_property_container_get(actx->p);
	cnst = ender_container_constraint_get(ec);
	if (!cnst)
		return EINA_FALSE;
	cnst_type = ender_constraint_type_get(cnst);
	if (cnst_type != ENDER_CONSTRAINT_DESCRIPTOR)
		return EINA_FALSE;
	descriptor = ender_constraint_descriptor_descriptor_get(cnst);
	if (!descriptor)
		return EINA_FALSE;
	name = ender_descriptor_name_get(descriptor);
	if (!strcmp(name, "esvg_animated_transform"))
		return EINA_FALSE;
	else if (!strcmp(name, "esvg_animated_frequency"))
		return EINA_FALSE;
	d = esvg_attribute_animated_descriptor_get(name);
	if (!d) return EINA_FALSE;

	/* store our own needed context data */
	thiz->etch = etch;
	thiz->parent_e = actx->parent_e;
	thiz->parent_t = actx->parent_t;
	thiz->attribute_type = actx->target.attribute_type;
	thiz->p = actx->p;
	thiz->d = d;
	/* get the 'to' value */
	to_data = thiz->d->value_new();
	if (thiz->d->value_get(thiz->to, &to_data))
	thiz->values = eina_list_append(thiz->values, to_data); 
	/* create the destination holder */
	thiz->destination_data = thiz->d->destination_new();
	if (thiz->d->destination_value_from)
	{
		void *value = eina_list_data_get(thiz->values);
		thiz->d->destination_value_from(thiz->destination_data, value);
	}
	/* create the values from the 'to' attribute */
	etch_data.type = ETCH_EXTERNAL;
	etch_data.data.external = to_data;
	/* create the animation */
	etch_a = etch_animation_external_add(thiz->etch,
			_esvg_element_set_interpolator_cb,
			_esvg_element_set_animation_cb,
			_esvg_element_set_animation_start_cb,
			_esvg_element_set_animation_stop_cb,
			NULL,
			NULL,
			thiz->destination_data,
			thiz);
	/* the repeat count */
	etch_animation_repeat_set(etch_a, actx->timing.repeat_count);
	thiz->etch_a = etch_a;

	k = etch_animation_keyframe_add(thiz->etch_a);
	etch_animation_keyframe_type_set(k, ETCH_INTERPOLATOR_DISCRETE);
	etch_animation_keyframe_value_set(k, &etch_data);
	etch_animation_keyframe_time_set(k, 0);

	k = etch_animation_keyframe_add(thiz->etch_a);
	etch_animation_keyframe_type_set(k, ETCH_INTERPOLATOR_DISCRETE);
	etch_animation_keyframe_value_set(k, &etch_data);
	if (dur->type == ESVG_DURATION_TYPE_CLOCK)
	{
		etch_animation_keyframe_time_set(k, dur->data.clock);
	}
	else
	{
		etch_animation_keyframe_time_set(k, 1);
		etch_animation_repeat_set(etch_a, -1);
	}
	return EINA_TRUE;
}

static Esvg_Animation_Descriptor _descriptor = {
	/* .attribute_get 	= */ _esvg_element_set_attribute_get,
	/* .free 		= */ _esvg_element_set_free,
	/* .initialize 		= */ _esvg_element_set_initialize,
	/* .attribute_set 	= */ _esvg_element_set_attribute_set,
	/* .setup 		= */ _esvg_element_set_setup,
	/* .enable 		= */ _esvg_element_set_enable,
	/* .disable 		= */ _esvg_element_set_disable,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _esvg_element_set_to_set(Edom_Tag *t, const char *to)
{
	Esvg_Element_Set *thiz;

	thiz = _esvg_element_set_get(t);
	if (thiz->to)
	{
		free(thiz->to);
		thiz->to = NULL;
	}
	if (to)
		thiz->to = strdup(to);
}

static void _esvg_element_set_to_get(Edom_Tag *t, const char **to)
{
	Esvg_Element_Set *thiz;

	if (!to) return;
	thiz = _esvg_element_set_get(t);
	*to = thiz->to;
}

static Edom_Tag * _esvg_element_set_new(void)
{
	Esvg_Element_Set *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Element_Set));
	t = esvg_element_animation_new(&_descriptor, ESVG_TYPE_SET, thiz);
	return t;
}

/* The ender wrapper */
#define _esvg_element_set_delete NULL
#define _esvg_element_set_to_is_set NULL
#include "esvg_generated_element_set.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_set_init(void)
{
	_esvg_element_set_log = eina_log_domain_register("esvg_set", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_set_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_set_init();
}

void esvg_element_set_shutdown(void)
{
	if (_esvg_element_set_log < 0)
		return;
	_esvg_element_set_shutdown();
	eina_log_domain_unregister(_esvg_element_set_log);
	_esvg_element_set_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Ender_Element * esvg_element_set_new(void)
{
	return ESVG_ELEMENT_NEW("SVGSetElement");
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_set_to_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_SET_TO, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_element_set_to_get(Ender_Element *e, const char **v)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_element_set_to_get(t, v);
}

