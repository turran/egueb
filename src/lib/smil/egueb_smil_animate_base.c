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
#include "egueb_smil_event.h"
#include "egueb_smil_animation_private.h"
#include "egueb_smil_animate_base_private.h"

/*
 * This file handles the common attribute handling for the
 * 'animate_base value attributes'. The elements that inherit
 * from this are the 'animate' and 'animateTransform'
 * TODO
 * several properties must not be a string, but a list of values
 * like for example the values, times, keys, etc
 *
 * When doing a sum animation, in case there are other animations
 * use the base value for the first animation and the animation value for the
 * others
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_smil_animate_base_values_cb(void *data, void *user_data)
{
	Egueb_Smil_Animate_Base *thiz = user_data;
	Egueb_Smil_Animation *a;
	Egueb_Dom_Value *gv, v = EGUEB_DOM_VALUE_INIT;
	Egueb_Dom_String *s = data;

	a = EGUEB_SMIL_ANIMATION(thiz);

	egueb_dom_value_init(&v, a->d);
	if (!egueb_dom_value_string_from(&v, s))
	{
		ERR("Impossible to parse value '%s'", egueb_dom_string_string_get(s));
		return;
	}
	/* store our parsed value */
	gv = calloc(1, sizeof(Egueb_Dom_Value));
	*gv = v;
	thiz->generated_values = eina_list_append(thiz->generated_values, gv);
}

static void _egueb_smil_animate_base_property_set(Egueb_Smil_Animate_Base *thiz, Egueb_Dom_Value *v)
{
	Egueb_Smil_Animation *a;

	a = EGUEB_SMIL_ANIMATION(thiz);
	egueb_dom_attr_value_set(a->p, EGUEB_DOM_ATTR_TYPE_ANIMATED, v);
}

#if 0
typedef struct _Egueb_Smil_Animate_Base_Values_Data
{
	Eina_List *values;
	Egueb_Smil_Attribute_Animated_Descriptor *d;
} Egueb_Smil_Animate_Base_Values_Data;

typedef struct _Egueb_Smil_Animate_Base_Times_Data
{
	Eina_List *times;
	int64_t duration;
} Egueb_Smil_Animate_Base_Times_Data;

typedef struct _Egueb_Smil_Animate_Base_Descriptor_Internal
{
	Egueb_Dom_Tag_Free free;
	Egueb_Smil_Element_Initialize initialize;
	Egueb_Smil_Element_Attribute_Set attribute_set;
	Egueb_Dom_Tag_Attribute_Get attribute_get;
	Egueb_Smil_Animate_Base_Attribute_Descriptor_Get type_descriptor_get;
} Egueb_Smil_Animate_Base_Descriptor_Internal;

static Etch_Interpolator_Type _egueb_smil_animate_base_calc_mode_etch_to(Egueb_Smil_Calc_Mode c)
{
	switch (c)
	{
		case EGUEB_SMIL_CALC_MODE_DISCRETE:
		return ETCH_INTERPOLATOR_DISCRETE;

		case EGUEB_SMIL_CALC_MODE_LINEAR:
		return ETCH_INTERPOLATOR_LINEAR;

		case EGUEB_SMIL_CALC_MODE_SPLINE:
		return ETCH_INTERPOLATOR_CUBIC;

		/* FIXME TODO */
		case EGUEB_SMIL_CALC_MODE_PACED:
		default:
		return ETCH_INTERPOLATOR_LINEAR;
	}
}

static void * _egueb_smil_animate_base_property_destination_new(Egueb_Smil_Animate_Base *thiz)
{
	void *ret;
	void *value;

	ret = thiz->d->destination_new();
	value = eina_list_data_get(thiz->values);
	if (thiz->d->destination_value_from)
	{
		thiz->d->destination_value_from(ret, value);
	}
	return ret;
}

static void * _egueb_smil_animate_base_property_destination_get(Egueb_Smil_Animate_Base *thiz)
{
	void *ret;

	ret = thiz->d->destination_new();
	ender_element_property_value_get(thiz->parent_e, thiz->p, ret, NULL);
	if (thiz->d->destination_keep)
	{
		thiz->d->destination_keep(ret);
	}
	return ret;
}

static void * _egueb_smil_animate_base_property_value_get(Egueb_Smil_Animate_Base *thiz, void *value)
{
	void *destination;

	destination = thiz->d->destination_new();
	ender_element_property_value_get(thiz->parent_e, thiz->p, destination, NULL);
	/* convert it to a value */
	thiz->d->destination_value_to(destination, &value);
	thiz->d->destination_free(destination, EINA_FALSE);
	return value;
}
#endif

static void _egueb_smil_animate_base_animation_add_keyframe(Etch_Animation *a,
	Etch_Interpolator_Type atype,
	Etch_Data *etch_data,
	int64_t time)
{
	Etch_Animation_Keyframe *k;
	DBG("Adding keyframe at time %" ETCH_TIME_FORMAT, ETCH_TIME_ARGS(time));
	k = etch_animation_keyframe_add(a);
	etch_animation_keyframe_type_set(k, atype);
	etch_animation_keyframe_value_set(k, etch_data);
	/* set the time */
	etch_animation_keyframe_time_set(k, time);
}

static void _egueb_smil_animate_base_interpolator_cb(Etch_Data *a, Etch_Data *b, double m, Etch_Data *res, void *data)
{
	egueb_dom_value_interpolate(res->data.external, a->data.external,
			b->data.external, m, NULL, NULL, 0);
#if 0
	Egueb_Smil_Animate_Base *thiz = data;

	thiz->d->interpolator(a->data.external, b->data.external, m, thiz->destination_add, thiz->destination_acc, thiz->repeat_count, res->data.external);
#endif
}

#if 0
static void _egueb_smil_animate_base_interpolator_add(Etch_Data *a, Etch_Data *b, double m, Etch_Data *res, void *data)
{
	Egueb_Smil_Animate_Base *thiz = data;

	ender_element_property_value_get(thiz->parent_e, thiz->p, thiz->destination_add, NULL);
	_egueb_smil_animate_base_interpolator_cb(a, b, m, res, data);
}
#endif
/*----------------------------------------------------------------------------*
 *                        The Etch external animation                         *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_animate_base_animation_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *user_data)
{
	Egueb_Smil_Animate_Base *thiz = user_data;
	_egueb_smil_animate_base_property_set(thiz, &thiz->dst_value);
}

static void _egueb_smil_animate_base_animation_start_cb(Etch_Animation *ea, void *data)
{
	Egueb_Smil_Animate_Base *thiz = data;
	Egueb_Smil_Animation *a;
	Egueb_Smil_Fill fill;
	Egueb_Dom_Event *ev;

	a = EGUEB_SMIL_ANIMATION(thiz);
	egueb_dom_attr_get(a->fill, EGUEB_DOM_ATTR_TYPE_BASE, &fill);
	/* in case of "remove" pick the last value it had */
	if (fill == EGUEB_SMIL_FILL_REMOVE)
	{
		egueb_dom_attr_final_value_get(a->p, &thiz->prv_value);
	}
	ev = egueb_smil_event_new();
	egueb_smil_event_init(ev, EGUEB_SMIL_EVENT_BEGIN, 0);
	egueb_dom_node_event_dispatch(a->target, ev, NULL);
}

#if 0
static void _egueb_smil_animate_base_animation_start_and_fetch_cb(Etch_Animation *a, void *data)
{
	Egueb_Smil_Animate_Base *thiz = data;
	Etch_Animation_Keyframe *k;
	Etch_Data kd;
	void *first;

	/* get the first value and store the current value there */
	first = eina_list_data_get(thiz->values);
	first = _egueb_smil_animate_base_property_value_get(thiz, first);
	/* set it as the first keyframe data */
	eina_list_data_set(thiz->values, first);
	/* replace the values pointer */
	k = etch_animation_keyframe_get(thiz->etch_a, 0);
	kd.type = ETCH_EXTERNAL;
	kd.data.external = first;
	etch_animation_keyframe_value_set(k, &kd);

	/* finally cal the animation */
	_egueb_smil_animate_base_animation_start_cb(a, data);
}
#endif

static void _egueb_smil_animate_base_animation_stop_cb(Etch_Animation *ea, void *data)
{
	Egueb_Smil_Animate_Base *thiz = data;
	Egueb_Smil_Animation *a;
	Egueb_Smil_Fill fill;
	Egueb_Dom_Event *ev;

	a = EGUEB_SMIL_ANIMATION(thiz);
	egueb_dom_attr_get(a->fill, EGUEB_DOM_ATTR_TYPE_BASE, &fill);
	/* in case of "remove" set the value it had when the animation started */
	if (fill == EGUEB_SMIL_FILL_REMOVE)
	{
		DBG("Going back to previous value");
		_egueb_smil_animate_base_property_set(thiz, &thiz->prv_value);
	}
	ev = egueb_smil_event_new();
	egueb_smil_event_init(ev, EGUEB_SMIL_EVENT_END, 0);
	egueb_dom_node_event_dispatch(a->target, ev, NULL);
}

static void _egueb_smil_animate_base_animation_repeat_cb(Etch_Animation *ea, void *data)
{
}

#if 0
static void _egueb_smil_animate_key_splines_cb(const char *v, void *user_data)
{
	Etch_Data ndata;
	Etch_Data cdata;
	Etch_Data cp0;
	Etch_Data cp1;

	/* iterate over the attribute by either space or commas */
	/* get the current keyframe and the next */
	/* the range is from 0 to 1, so we need to multiply the value with that
	 * factor and set the value on the cubic argument */
}

static void _egueb_smil_animate_base_values_cb(const char *v, void *user_data)
{
	Egueb_Smil_Animate_Base_Values_Data *data = user_data;
	void *get_data;

	get_data = data->d->value_new();
	if (data->d->value_get(v, &get_data))
		data->values = eina_list_append(data->values, get_data);
	else
		data->d->value_free(get_data);
}

static void _egueb_smil_animate_base_time_cb(const char *v, void *user_data)
{
	Egueb_Smil_Animate_Base_Times_Data *data = user_data;
	Egueb_Smil_Number n;
	double percent;
	int64_t *t;

	egueb_smil_number_string_from(&n, v, 1.0);
	percent = n.value;
	if (percent < 0.0)
		percent = 0;
	else if (percent > 1.0)
		percent = 1.0;

	t = malloc(sizeof(int64_t));
	*t = data->duration * percent;
	data->times = eina_list_append(data->times, t);
}

static void _egueb_smil_animate_base_key_splines_cb(const char *v, void *user_data)
{
	Eina_List **ret = user_data;
	Egueb_Smil_Animate_Key_Spline *spline;

	/* fetch the four values */
	spline = calloc(1, sizeof(Egueb_Smil_Animate_Key_Spline));
	egueb_smil_animate_key_spline_string_from(spline, v);
	*ret = eina_list_append(*ret, spline);
}
#endif

static Eina_Bool _egueb_smil_animate_base_values_generate(Egueb_Smil_Animate_Base *thiz,
		Eina_Bool *has_from)
{
	Egueb_Dom_List *values;
	*has_from = EINA_TRUE;

	egueb_dom_attr_get(thiz->values, EGUEB_DOM_ATTR_TYPE_BASE, &values);
	if (values)
	{
		egueb_dom_list_foreach(values, _egueb_smil_animate_base_values_cb, thiz);
		egueb_dom_list_unref(values);
	}
	else
	{
		Egueb_Smil_Animation *a;
		Egueb_Dom_String *from = NULL;
		Egueb_Dom_String *to = NULL;

		a = EGUEB_SMIL_ANIMATION(thiz);

		/* get the 'from' attribute */
		egueb_dom_attr_get(thiz->from, EGUEB_DOM_ATTR_TYPE_BASE, &from);
		if (egueb_dom_string_is_valid(from))
		{
			Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
			Egueb_Dom_Value *nv;

			egueb_dom_value_init(&v, a->d);
			if (!egueb_dom_value_string_from(&v, from))
			{
				ERR("No valid 'from' value");
				egueb_dom_string_unref(from);
				return EINA_FALSE;
			}
			nv = calloc(1, sizeof(Egueb_Dom_Value));
			*nv = v;
			 
			thiz->generated_values = eina_list_append(thiz->generated_values, nv); 
		}
		else
		{
			/* mark the missing from */
			*has_from = EINA_FALSE;
		}
		egueb_dom_string_unref(from);

		/* get the 'to' attribute */
		egueb_dom_attr_get(thiz->to, EGUEB_DOM_ATTR_TYPE_BASE, &to);
		if (egueb_dom_string_is_valid(to))
		{
			Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
			Egueb_Dom_Value *nv;

			egueb_dom_value_init(&v, a->d);
			if (!egueb_dom_value_string_from(&v, to))
			{
				ERR("No valid 'to' value");
				egueb_dom_string_unref(to);
				return EINA_FALSE;
			}
			nv = calloc(1, sizeof(Egueb_Dom_Value));
			*nv = v;
			 
			thiz->generated_values = eina_list_append(thiz->generated_values, nv); 
		}
#if 0
		else if (c->value.by)
		{
			/* if no from, then everything is dynamic until the animation starts */
			/* TODO append the from to the values */
		}
#endif
	}

	return EINA_TRUE;
}
#if 0
static void _egueb_smil_animate_base_values_free(Eina_List *values, Egueb_Smil_Attribute_Animated_Value_Free free_cb)
{
	void *data;

	if (!values) return;

	EINA_LIST_FREE (values, data);
		free_cb(data);
}

static Eina_Bool _egueb_smil_animate_base_key_splines_generate(Egueb_Smil_Animate_Base_Context *c,
		Eina_List **ksplines)
{
	Eina_List *l = NULL;

	if (!c->value.key_splines)
		return EINA_TRUE;

	egueb_smil_list_string_from(c->value.key_splines, ';',
		_egueb_smil_animate_base_key_splines_cb, &l);
	*ksplines = l;
}

#endif
static Eina_Bool _egueb_smil_animate_base_times_generate(Egueb_Smil_Animate_Base *thiz)
{
	Egueb_Smil_Animation *a;
	Egueb_Smil_Duration dur;

	a = EGUEB_SMIL_ANIMATION(thiz);
	egueb_dom_attr_get(a->dur, EGUEB_DOM_ATTR_TYPE_BASE, &dur);
	/* generate the times list */
	/* get the duration */
	if (dur.type == EGUEB_SMIL_DURATION_TYPE_CLOCK)
	{
#if 0
		if (c->value.key_times)
		{
			Egueb_Smil_Animate_Base_Times_Data data;

			data.times = *times;
			data.duration = dur->data.clock;

			egueb_smil_list_string_from(c->value.key_times, ';',
					_egueb_smil_animate_base_time_cb, &data);
			*times = data.times;
		}
		else
#endif
		{
			int64_t t = 0;
			int i;
			int length;
			int64_t duration;
			int64_t inc;

			length = eina_list_count(thiz->generated_values);
			if (!length)
			{
				ERR("No generated values");
				return EINA_FALSE;
			}

			duration = dur.data.clock;
			inc = duration / (length - 1);
			for (i = 0; i < length; i++)
			{
				int64_t *d;

				d = malloc(sizeof(int64_t));
				*d = t;
				DBG("Adding time %" ETCH_TIME_FORMAT, ETCH_TIME_ARGS(t));
				thiz->generated_times = eina_list_append(thiz->generated_times, d);
				t += inc;
			}
		}
	}
	return EINA_TRUE;
}
#if 0
static void _egueb_smil_animate_base_times_free(Eina_List *times)
{
	int64_t *v;

	if (!times) return;
	EINA_LIST_FREE (times, v)
		free(v);
}

static void _egueb_smil_animate_base_animation_create(Egueb_Smil_Animate_Base *thiz,
		Egueb_Smil_Animation_Context *actx)
{
	Etch_Animation_State_Callback start_cb;
	Etch_Interpolator interpolator_cb;
	Etch_Animation *etch_a;
	Eina_List *l, *tt;
	Eina_Bool has_from;
	void *v;
	int64_t *time;

	/* default variants */
	interpolator_cb = _egueb_smil_animate_base_interpolator;
	start_cb = _egueb_smil_animate_base_animation_start_cb;
	/* generate the values and times */
	_egueb_smil_animate_base_values_generate(&thiz->current, thiz->d,
			&thiz->values, &has_from);
	/* before adding a new empty value, create the destination holder */
	thiz->destination_data = _egueb_smil_animate_base_property_destination_new(thiz);
	/* in case it has not a 'from' attribute, prepend a new empty value */
	if (!has_from)
	{
		void *from;

		if (!thiz->d->destination_value_to)
		{
			ERR("Can not convert an attribute to an animation data");
		}
		/* in case of no from, add another keyframe at time 0
		 * the value for such keyframe should be taken
		 * at the begin animation
		 */
		from = thiz->d->value_new();
		thiz->values = eina_list_prepend(thiz->values, from);
		start_cb = _egueb_smil_animate_base_animation_start_and_fetch_cb;
	}
	_egueb_smil_animate_base_times_generate(&actx->timing.dur, &thiz->current, thiz->values, &thiz->times);
	if (!thiz->times)
	{
		ERR("No time defined");
		return;
	}

	/* check if we are the first animation */
	if (actx->addition.additive == EGUEB_SMIL_ADDITIVE_SUM)
	{
		thiz->destination_add = _egueb_smil_animate_base_property_destination_get(thiz);
		if (actx->index > 1)
		{
			interpolator_cb = _egueb_smil_animate_base_interpolator_add;
		}
	}
	if (actx->addition.accumulate == EGUEB_SMIL_ACCUMULATE_SUM)
	{
		Eina_List *l2;

		/* get the latest value and keep it */
		l2 = eina_list_last(thiz->values);
		thiz->destination_acc = eina_list_data_get(l2);
	}
	/* create the animation */
	etch_a = etch_animation_external_add(thiz->etch,
			interpolator_cb,
			_egueb_smil_animate_base_animation_cb,
			start_cb,
			_egueb_smil_animate_base_animation_stop_cb,
			_egueb_smil_animate_base_animation_repeat_cb,
			NULL,
			thiz->destination_data,
			thiz);
	/* the repeat count */
	etch_animation_repeat_set(etch_a, actx->timing.repeat_count);
	thiz->etch_a = etch_a;
	/* add the keyframes */
	tt = thiz->times;
	EINA_LIST_FOREACH(thiz->values, l, v)
	{
		Etch_Data edata;

		time = eina_list_data_get(tt);
		edata.type = ETCH_EXTERNAL;
		edata.data.external = v;
		/* add a keyframe */
		_egueb_smil_animate_base_animation_add_keyframe(thiz->etch_a, &thiz->current, &edata, *time);
		tt = eina_list_next(tt);
	}
}

static void _egueb_smil_animate_base_cleanup(Egueb_Smil_Animate_Base *thiz)
{
	/* in case of animations free them */
	if (thiz->etch_a)
	{

	}
	if (thiz->destination_data)
	{

	}

	if (thiz->values)
	{
		_egueb_smil_animate_base_values_free(thiz->values, thiz->d->value_free);
		thiz->values = NULL;
	}
	if (thiz->times)
	{
		_egueb_smil_animate_base_times_free(thiz->times);
		thiz->times = NULL;
	}
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_animate_base_to_set(Egueb_Dom_Tag *t, const char *to)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	if (thiz->current.value.to)
	{
		free(thiz->current.value.to);
		thiz->current.value.to = NULL;
	}
	if (to)
		thiz->current.value.to = strdup(to);
	thiz->current.changed = EINA_TRUE;
}

static void _egueb_smil_animate_base_to_get(Egueb_Dom_Tag *t, const char **to)
{
	Egueb_Smil_Animate_Base *thiz;

	if (!to) return;
	thiz = _egueb_smil_animate_base_get(t);
	*to = thiz->current.value.to;
}

static void _egueb_smil_animate_base_from_set(Egueb_Dom_Tag *t, const char *from)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	if (thiz->current.value.from)
	{
		free(thiz->current.value.from);
		thiz->current.value.from = NULL;
	}
	if (from)
		thiz->current.value.from = strdup(from);
	thiz->current.changed = EINA_TRUE;
}

static void _egueb_smil_animate_base_from_get(Egueb_Dom_Tag *t, const char **from)
{
	Egueb_Smil_Animate_Base *thiz;

	if (!from) return;
	thiz = _egueb_smil_animate_base_get(t);
	*from = thiz->current.value.from;
}

static void _egueb_smil_animate_base_values_set(Egueb_Dom_Tag *t, const char *values)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	if (thiz->current.value.values)
	{
		free(thiz->current.value.values);
		thiz->current.value.values = NULL;
	}
	if (values)
		thiz->current.value.values = strdup(values);
	thiz->current.changed = EINA_TRUE;
}

static void _egueb_smil_animate_base_values_get(Egueb_Dom_Tag *t, const char **values)
{
	Egueb_Smil_Animate_Base *thiz;

	if (!values) return;
	thiz = _egueb_smil_animate_base_get(t);
	*values = thiz->current.value.values;
}

static void _egueb_smil_animate_base_calc_mode_set(Egueb_Dom_Tag *t, Egueb_Smil_Calc_Mode calc_mode)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	thiz->current.value.calc_mode = calc_mode;
	thiz->current.changed = EINA_TRUE;
}

static void _egueb_smil_animate_base_calc_mode_get(Egueb_Dom_Tag *t, Egueb_Smil_Calc_Mode *calc_mode)
{
	Egueb_Smil_Animate_Base *thiz;

	if (!calc_mode) return;
	thiz = _egueb_smil_animate_base_get(t);
	*calc_mode = thiz->current.value.calc_mode;
}

static void _egueb_smil_animate_base_key_times_set(Egueb_Dom_Tag *t, const char *key_times)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	if (thiz->current.value.key_times)
	{
		free(thiz->current.value.key_times);
		thiz->current.value.key_times = NULL;
	}
	if (key_times)
		thiz->current.value.key_times = strdup(key_times);
	thiz->current.changed = EINA_TRUE;
}

static void _egueb_smil_animate_base_key_times_get(Egueb_Dom_Tag *t, const char **key_times)
{
	Egueb_Smil_Animate_Base *thiz;

	if (!key_times) return;
	thiz = _egueb_smil_animate_base_get(t);
	*key_times = thiz->current.value.key_times;
}

static void _egueb_smil_animate_base_key_splines_set(Egueb_Dom_Tag *t, const char *key_splines)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	if (thiz->current.value.key_splines)
	{
		free(thiz->current.value.key_splines);
		thiz->current.value.key_splines = NULL;
	}
	if (key_splines)
		thiz->current.value.key_splines = strdup(key_splines);
	thiz->current.changed = EINA_TRUE;
}

static void _egueb_smil_animate_base_key_splines_get(Egueb_Dom_Tag *t, const char **key_splines)
{
	Egueb_Smil_Animate_Base *thiz;

	if (!key_splines) return;
	thiz = _egueb_smil_animate_base_get(t);
	*key_splines = thiz->current.value.key_splines;
}
/*----------------------------------------------------------------------------*
 *                        The Esvg Animation interface                        *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_animate_base_enable(Egueb_Dom_Tag *t, int64_t offset)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	if (!thiz->etch_a) return;

	etch_animation_offset_add(thiz->etch_a, offset);
	etch_animation_enable(thiz->etch_a);
}

static void _egueb_smil_animate_base_disable(Egueb_Dom_Tag *t)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	if (!thiz->etch_a) return;

	etch_animation_disable(thiz->etch_a);
}

static void _egueb_smil_animate_base_initialize(Ender_Element *e)
{
	Egueb_Smil_Animate_Base *thiz;
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _egueb_smil_animate_base_get(t);
	thiz->thiz_e = e;

	/* call the interface */
	if (thiz->descriptor.initialize)
		thiz->descriptor.initialize(e);
}

static Eina_Bool _egueb_smil_animate_base_setup(Egueb_Dom_Tag *t,
		Egueb_Smil_Context *c,
		Egueb_Smil_Animation_Context *actx,
		Enesim_Log **error)
{
}

/* The ender wrapper */
#define _egueb_smil_animate_base_calc_mode_is_set NULL
#endif
/*----------------------------------------------------------------------------*
 *                             Animation interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_smil_animate_base_setup(Egueb_Smil_Animation *a,
		Egueb_Dom_Node *target)
{
	Egueb_Smil_Animate_Base *thiz;
	Egueb_Dom_Value *v;
	Etch_Interpolator interpolator_cb;
	Etch_Animation_State_Callback start_cb;
	Etch_Interpolator_Type atype;
	Etch_Animation *etch_a;
	Eina_Bool has_from;
	Eina_List *l, *tt;
#if 0
	Egueb_Smil_Attribute_Animated_Descriptor *d;
	Ender_Element *svg_e;
	Ender_Container *ec;
	Ender_Descriptor *descriptor;
	const Ender_Constraint *cnst;
	Ender_Constraint_Type cnst_type;
	Etch *etch;
	const char *name;
#endif

	thiz = EGUEB_SMIL_ANIMATE_BASE(a);
	_egueb_smil_animate_base_values_generate(thiz, &has_from);
	if (!thiz->generated_values)
	{
		ERR("No values generated");
		return EINA_FALSE;
	}
	_egueb_smil_animate_base_times_generate(thiz);
	if (!thiz->generated_times)
	{
		ERR("No times generated");
		return EINA_FALSE;
	}
	/* check that we have the same number of times and values */
	if (eina_list_count(thiz->generated_values) != eina_list_count(thiz->generated_times))
	{
		ERR("Number of values and times does not match");
		return EINA_FALSE;
	}

	/* default variants */
	interpolator_cb = _egueb_smil_animate_base_interpolator_cb;
	start_cb = _egueb_smil_animate_base_animation_start_cb;

	/* setup the holder of the destination value */
	egueb_dom_value_init(&thiz->dst_value, a->d);
	/* pick the first value and copy it */
	egueb_dom_value_copy((Egueb_Dom_Value *)thiz->generated_values->data, &thiz->dst_value, EINA_TRUE);

	/* create the animation */
	etch_a = etch_animation_external_add(a->etch,
			interpolator_cb,
			_egueb_smil_animate_base_animation_cb,
			start_cb,
			_egueb_smil_animate_base_animation_stop_cb,
			_egueb_smil_animate_base_animation_repeat_cb,
			NULL,
			&thiz->dst_value,
			thiz);
	/* the repeat count */
	//etch_animation_repeat_set(etch_a, actx->timing.repeat_count);
	/* get the interpolator type */
	//atype =_egueb_smil_animate_base_calc_mode_etch_to(c->value.calc_mode);
	atype = ETCH_INTERPOLATOR_LINEAR;

	thiz->etch_a = etch_a;

	/* add the keyframes */
	tt = thiz->generated_times;
	EINA_LIST_FOREACH(thiz->generated_values, l, v)
	{
		Etch_Data edata;
		int64_t *time;

		time = eina_list_data_get(tt);
		edata.type = ETCH_EXTERNAL;
		edata.data.external = v;
		/* add a keyframe */
		_egueb_smil_animate_base_animation_add_keyframe(thiz->etch_a, atype, &edata, *time);
		tt = eina_list_next(tt);
	}
#if 0

	/* TODO pass the etch from the animation class */
	/* TODO simplify the animation context */

	/* get the etch associated with the topmost svg */
	egueb_smil_element_internal_topmost_get(t, &svg_e);
	if (!svg_e)
		return EINA_FALSE;

	etch = egueb_smil_element_svg_etch_get(svg_e);
	if (!etch)
		return EINA_FALSE;

	/* get the descriptor */
	if (!thiz->descriptor.type_descriptor_get)
		return EINA_FALSE;
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
	if (!thiz->descriptor.type_descriptor_get(t, name, &d))
		return EINA_FALSE;

	/* store our own needed context data */
	thiz->etch = etch;
	thiz->parent_e = actx->parent_e;
	thiz->parent_t = actx->parent_t;
	thiz->attribute_type = actx->target.attribute_type;
	thiz->p = actx->p;
	thiz->d = d;
	_egueb_smil_animate_base_animation_create(thiz, actx);
#endif

	return EINA_TRUE;
}

static void _egueb_smil_animate_base_cleanup(Egueb_Smil_Animation *a,
		Egueb_Dom_Node *target)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = EGUEB_SMIL_ANIMATE_BASE(a);
}

/* TODO same as set, share this */
static void _egueb_smil_animate_base_begin(Egueb_Smil_Animation *a, int64_t offset)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = EGUEB_SMIL_ANIMATE_BASE(a);
	if (!thiz->etch_a) return;
	DBG("Beginning set at %" ETCH_TIME_FORMAT, ETCH_TIME_ARGS (offset));
	etch_animation_offset_add(thiz->etch_a, offset);
	etch_animation_enable(thiz->etch_a);
}

/* TODO same as set, share this */
static void _egueb_smil_animate_base_end(Egueb_Smil_Animation *a)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = EGUEB_SMIL_ANIMATE_BASE(a);
	if (!thiz->etch_a) return;
	DBG("Ending");
	etch_animation_disable(thiz->etch_a);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_smil_animate_base, Egueb_Smil_Animate_Base, calc_mode);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_smil_animate_base, Egueb_Smil_Animate_Base, by);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_smil_animate_base, Egueb_Smil_Animate_Base, to);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_smil_animate_base, Egueb_Smil_Animate_Base, from);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_smil_animate_base, Egueb_Smil_Animate_Base, values);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_smil_animate_base, Egueb_Smil_Animate_Base, key_times);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_smil_animate_base, Egueb_Smil_Animate_Base, key_splines);

ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SMIL_ANIMATION_DESCRIPTOR,
		Egueb_Smil_Animate_Base, Egueb_Smil_Animate_Base_Class, egueb_smil_animate_base);

static void _egueb_smil_animate_base_class_init(void *k)
{
	Egueb_Smil_Animation_Class *klass;

	klass = EGUEB_SMIL_ANIMATION_CLASS(k);
	klass->cleanup = _egueb_smil_animate_base_cleanup;
	klass->setup = _egueb_smil_animate_base_setup;
	klass->begin = _egueb_smil_animate_base_begin;
	klass->end = _egueb_smil_animate_base_end;
}

static void _egueb_smil_animate_base_class_deinit(void *k)
{
}

static void _egueb_smil_animate_base_instance_init(void *o)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = EGUEB_SMIL_ANIMATE_BASE(o);
	/* create the properties */
	thiz->by = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SMIL_BY), NULL);
	thiz->to = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SMIL_TO), NULL);
	thiz->from = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SMIL_FROM), NULL);
	thiz->values = egueb_dom_attr_string_list_new(
			egueb_dom_string_ref(EGUEB_SMIL_VALUES), NULL);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_smil_animate_base, by);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_smil_animate_base, to);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_smil_animate_base, from);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_smil_animate_base, values);
}

static void _egueb_smil_animate_base_instance_deinit(void *o)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = EGUEB_SMIL_ANIMATE_BASE(o);
	/* the cleanup will be called as part of the deinitialization */
	egueb_dom_node_unref(thiz->by);
	egueb_dom_node_unref(thiz->to);
	egueb_dom_node_unref(thiz->from);
	egueb_dom_node_unref(thiz->values);
#if 0
	egueb_dom_node_unref(thiz->calc_mode);
	egueb_dom_node_unref(thiz->key_times);
	egueb_dom_node_unref(thiz->key_splines);
#endif
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
#if 0
Eina_Bool egueb_smil_is_animate_base_internal(Egueb_Dom_Tag *t)
{
	Egueb_Smil_Animate_Base *thiz;
	Eina_Bool ret;

	if (!egueb_smil_is_element_internal(t))
		return EINA_FALSE;
	thiz = egueb_smil_element_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, EGUEB_SMIL_ANIMATE_BASE_MAGIC);

	return ret;
}

void * egueb_smil_animate_base_data_get(Egueb_Dom_Tag *t)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	return thiz->data;
}

Egueb_Dom_Tag * egueb_smil_animate_base_new(Egueb_Smil_Animate_Base_Descriptor *descriptor, Egueb_Smil_Type type,
		void *data)
{
	Egueb_Smil_Animate_Base *thiz;
	Egueb_Smil_Animation_Descriptor pdescriptor;
	Egueb_Dom_Tag *t;

	thiz = calloc(1, sizeof(Egueb_Smil_Animate_Base));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, EGUEB_SMIL_ANIMATE_BASE_MAGIC);
	thiz->data = data;
	/* default values */
	thiz->current.value.calc_mode = EGUEB_SMIL_CALC_MODE_LINEAR;
	/* our own descriptor */
	thiz->descriptor.type_descriptor_get = descriptor->type_descriptor_get;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->descriptor.attribute_get = descriptor->attribute_get;
	thiz->descriptor.initialize = descriptor->initialize;
	/* parent descriptor */
	pdescriptor.attribute_set = _egueb_smil_animate_base_attribute_set;
	pdescriptor.attribute_get = _egueb_smil_animate_base_attribute_get;
	pdescriptor.free = _egueb_smil_animate_base_free;
	pdescriptor.initialize = _egueb_smil_animate_base_initialize;
	pdescriptor.setup = _egueb_smil_animate_base_setup;
	pdescriptor.enable = _egueb_smil_animate_base_enable;
	pdescriptor.disable = _egueb_smil_animate_base_disable;

	t = egueb_smil_element_animation_new(&pdescriptor, type, thiz);

	return t;
}
#endif
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
#if 0
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_smil_is_animate_base(Ender_Element *e)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	return egueb_smil_is_animate_base_internal(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_to_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ANIMATE_BASE_TO, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_to_get(Ender_Element *e, const char **v)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animate_base_to_get(t, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_from_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ANIMATE_BASE_FROM, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_from_get(Ender_Element *e, const char **v)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animate_base_from_get(t, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_values_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ANIMATE_BASE_VALUES, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_values_get(Ender_Element *e, const char **v)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animate_base_values_get(t, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_calc_mode_set(Ender_Element *e, Egueb_Smil_Calc_Mode calc_mode)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ANIMATE_BASE_CALC_MODE, calc_mode, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_calc_mode_get(Ender_Element *e, Egueb_Smil_Calc_Mode *calc_mode)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animate_base_calc_mode_get(t, calc_mode);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_key_times_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ANIMATE_BASE_KEY_TIMES, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_key_times_get(Ender_Element *e, const char **v)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animate_base_key_times_get(t, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_key_splines_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, EGUEB_SMIL_ANIMATE_BASE_KEY_SPLINES, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_smil_animate_base_key_splines_get(Ender_Element *e, const char **v)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	_egueb_smil_animate_base_key_splines_get(t, v);
}
#endif
