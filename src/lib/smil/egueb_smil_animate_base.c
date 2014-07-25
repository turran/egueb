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
#include "egueb_smil_additive.h"
#include "egueb_smil_calc_mode.h"
#include "egueb_smil_clock.h"
#include "egueb_smil_keyframe.h"
#include "egueb_smil_timeline.h"
#include "egueb_smil_event.h"

#include "egueb_smil_animation_private.h"
#include "egueb_smil_animate_base_private.h"
#include "egueb_smil_attr_additive_private.h"
#include "egueb_smil_attr_calc_mode_private.h"

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

static void _egueb_smil_animate_base_property_get(Egueb_Smil_Animate_Base *thiz, Egueb_Dom_Value *v)
{
	Egueb_Smil_Animation *a;

	a = EGUEB_SMIL_ANIMATION(thiz);
	egueb_dom_attr_final_value_get(a->p, v);
}

static Egueb_Smil_Keyframe_Interpolator_Type
_egueb_smil_animate_base_calc_mode_to_interpolator_type(Egueb_Smil_Calc_Mode c)
{
	switch (c)
	{
		case EGUEB_SMIL_CALC_MODE_DISCRETE:
		return EGUEB_SMIL_KEYFRAME_INTERPOLATOR_DISCRETE;

		case EGUEB_SMIL_CALC_MODE_LINEAR:
		return EGUEB_SMIL_KEYFRAME_INTERPOLATOR_LINEAR;

		case EGUEB_SMIL_CALC_MODE_SPLINE:
		return EGUEB_SMIL_KEYFRAME_INTERPOLATOR_CUBIC;

		/* FIXME TODO */
		case EGUEB_SMIL_CALC_MODE_PACED:
		default:
		return EGUEB_SMIL_KEYFRAME_INTERPOLATOR_LINEAR;
	}
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

#endif

static void _egueb_smil_animate_base_animation_add_keyframe(Egueb_Smil_Signal *s,
	Egueb_Smil_Keyframe_Interpolator_Type type,
	Egueb_Dom_Value *value,
	int64_t time)
{
	DBG("Adding keyframe at time %" EGUEB_SMIL_CLOCK_FORMAT, EGUEB_SMIL_CLOCK_ARGS(time));
	egueb_smil_signal_continuous_keyframe_simple_add(s, type, time, value);
}

static void _egueb_smil_animate_base_interpolator_cb(Egueb_Dom_Value *a, Egueb_Dom_Value *b,
		double m, void *data)
{
	Egueb_Smil_Animate_Base *thiz = data;
	egueb_dom_value_interpolate(&thiz->dst_value, a, b, m, thiz->add_value, NULL, 0);
	_egueb_smil_animate_base_property_set(thiz, &thiz->dst_value);
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
static void _egueb_smil_animate_base_animation_start_cb(Egueb_Smil_Signal *s, void *data)
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
	egueb_smil_event_init(ev, egueb_dom_string_ref(EGUEB_SMIL_EVENT_BEGIN), 0);
	egueb_dom_node_event_dispatch(a->target, ev, NULL, NULL);
}

static void _egueb_smil_animate_base_animation_start_and_fetch_cb(Egueb_Smil_Signal *s, void *data)
{
	Egueb_Smil_Animate_Base *thiz = data;
	Egueb_Smil_Animation *a;
	Egueb_Dom_Value *first;

	a = EGUEB_SMIL_ANIMATION(thiz);
	/* get the first value and store the current value there */
	first = eina_list_data_get(thiz->generated_values);
	egueb_dom_attr_final_value_get(a->p, first);
	/* finally call the animation */
	_egueb_smil_animate_base_animation_start_cb(s, data);
}

static void _egueb_smil_animate_base_animation_stop_cb(Egueb_Smil_Signal *s, void *data)
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
	egueb_smil_event_init(ev, egueb_dom_string_ref(EGUEB_SMIL_EVENT_END), 0);
	egueb_dom_node_event_dispatch(a->target, ev, NULL, NULL);
}

static void _egueb_smil_animate_base_animation_repeat_cb(Egueb_Smil_Signal *s, void *data)
{
	/* TODO send the repeat event */
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
		Eina_Bool *has_from, Eina_Bool *has_by)
{
	Egueb_Dom_List *values;
	*has_from = EINA_TRUE;
	*has_by = EINA_FALSE;

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
				ERR("No valid 'from' value '%s'", egueb_dom_string_string_get(from));
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
		else
		{
			Egueb_Dom_String *by = NULL;
			egueb_dom_attr_get(thiz->by, EGUEB_DOM_ATTR_TYPE_BASE, &by);

			/* for 'by' animations we need to interpolate the by value
			 * from 0, 1 and add the original from
			 */
			if (egueb_dom_string_is_valid(by))
			{
				Egueb_Dom_Value v = EGUEB_DOM_VALUE_INIT;
				Egueb_Dom_Value *start, *end;

				egueb_dom_value_init(&v, a->d);
				if (!egueb_dom_value_string_from(&v, by))
				{
					ERR("No valid 'by' value");
					egueb_dom_string_unref(by);
					egueb_dom_string_unref(to);
					return EINA_FALSE;
				}
				start = calloc(1, sizeof(Egueb_Dom_Value));
				*start = v;

				end = calloc(1, sizeof(Egueb_Dom_Value));
				egueb_dom_value_init(end, a->d);
				egueb_dom_value_copy(start, end, EINA_TRUE);

				thiz->generated_values = eina_list_append(thiz->generated_values, start);
				thiz->generated_values = eina_list_append(thiz->generated_values, end);
				*has_by = EINA_TRUE;
			}
			egueb_dom_string_unref(by);
		}
		egueb_dom_string_unref(to);
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
			if (length == 1)
			{
				ERR("No enough values to generate an animation");
				return EINA_FALSE;
			}

			duration = dur.data.clock;
			inc = duration / (length - 1);
			for (i = 0; i < length; i++)
			{
				int64_t *d;

				d = malloc(sizeof(int64_t));
				*d = t;
				DBG("Adding time %" EGUEB_SMIL_CLOCK_FORMAT, EGUEB_SMIL_CLOCK_ARGS(t));
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
	Etch_Animation *s;
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
	s = egueb_smil_signal_external_add(thiz->etch,
			interpolator_cb,
			_egueb_smil_animate_base_animation_cb,
			start_cb,
			_egueb_smil_animate_base_animation_stop_cb,
			_egueb_smil_animate_base_animation_repeat_cb,
			NULL,
			thiz->destination_data,
			thiz);
	/* the repeat count */
	egueb_smil_signal_repeat_set(s, actx->timing.repeat_count);
	thiz->signal = s;
	/* add the keyframes */
	tt = thiz->times;
	EINA_LIST_FOREACH(thiz->values, l, v)
	{
		Etch_Data edata;

		time = eina_list_data_get(tt);
		edata.type = ETCH_EXTERNAL;
		edata.data.external = v;
		/* add a keyframe */
		_egueb_smil_animate_base_animation_add_keyframe(thiz->signal, &thiz->current, &edata, *time);
		tt = eina_list_next(tt);
	}
}

static void _egueb_smil_animate_base_cleanup(Egueb_Smil_Animate_Base *thiz)
{
	/* in case of animations free them */
	if (thiz->signal)
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
	if (!thiz->signal) return;

	egueb_smil_signal_offset_set(thiz->signal, offset);
	egueb_smil_signal_enable(thiz->signal);
}

static void _egueb_smil_animate_base_disable(Egueb_Dom_Tag *t)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = _egueb_smil_animate_base_get(t);
	if (!thiz->signal) return;

	egueb_smil_signal_disable(thiz->signal);
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
	Egueb_Smil_Additive additive;
	Egueb_Smil_Calc_Mode calc_mode = EGUEB_SMIL_CALC_MODE_LINEAR;
	Egueb_Dom_Value *v;
	Egueb_Smil_Signal_Continuous_Process interpolator_cb;
	Egueb_Smil_Signal_State_Callback start_cb;
	Egueb_Smil_Keyframe_Interpolator_Type atype;
	Eina_Bool has_from, has_by;
	Eina_List *l, *tt;

	thiz = EGUEB_SMIL_ANIMATE_BASE(a);
	egueb_dom_attr_final_get(thiz->additive, &additive);

	_egueb_smil_animate_base_values_generate(thiz, &has_from, &has_by);
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
	if (has_by)
	{
		additive = EGUEB_SMIL_ADDITIVE_SUM;
		/* TODO in case of from, use the from to add */
		if (has_from)
		{
			Egueb_Dom_Value *from;

			from = thiz->generated_values->data;
			thiz->generated_values = eina_list_remove_list(thiz->generated_values,
					thiz->generated_values);
			thiz->add_value = from;
		}
	}
	else
	{
		/* in case of no from, add another keyframe at time 0
		 * the value for such keyframe should be taken
		 * at the begin animation
		 */
		if (!has_from)
		{
			Egueb_Smil_Animation *a;
			Egueb_Dom_Value *from;

			a = EGUEB_SMIL_ANIMATION(thiz);

			from = calloc(1, sizeof(Egueb_Dom_Value));
			egueb_dom_value_init(from, a->d);
			/* just get a dummy value for now, so we can have an allocated value */
			egueb_dom_attr_final_value_get(a->p, from);
			thiz->generated_values = eina_list_prepend(thiz->generated_values, from);

			start_cb = _egueb_smil_animate_base_animation_start_and_fetch_cb;
		}
	}

	/* setup the holder of the destination value */
	egueb_dom_value_init(&thiz->dst_value, a->d);
	/* pick the first value and copy it to allocate in case we need to */
	egueb_dom_value_copy((Egueb_Dom_Value *)thiz->generated_values->data, &thiz->dst_value, EINA_TRUE);

	/* create the animation */
	thiz->signal = egueb_smil_signal_continuous_new(interpolator_cb, start_cb,
			_egueb_smil_animate_base_animation_stop_cb,
			_egueb_smil_animate_base_animation_repeat_cb, thiz);
	egueb_smil_timeline_signal_add(a->timeline, egueb_smil_signal_ref(thiz->signal));
	/* TODO the repeat count */
	/* TODO the repeat dur */
	/* get the interpolator type */
	egueb_dom_attr_final_get(thiz->calc_mode, &calc_mode);
	atype = _egueb_smil_animate_base_calc_mode_to_interpolator_type(calc_mode);

	/* add the keyframes */
	tt = thiz->generated_times;
	EINA_LIST_FOREACH(thiz->generated_values, l, v)
	{
		int64_t *time;

		time = eina_list_data_get(tt);
		/* add a keyframe */
		_egueb_smil_animate_base_animation_add_keyframe(thiz->signal, atype, v, *time);
		tt = eina_list_next(tt);
	}
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
	if (!thiz->signal) return;
	DBG("Beginning set at %" EGUEB_SMIL_CLOCK_FORMAT, EGUEB_SMIL_CLOCK_ARGS (offset));
	egueb_smil_signal_offset_set(thiz->signal, offset);
	egueb_smil_signal_enable(thiz->signal);
}

/* TODO same as set, share this */
static void _egueb_smil_animate_base_end(Egueb_Smil_Animation *a)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = EGUEB_SMIL_ANIMATE_BASE(a);
	if (!thiz->signal) return;
	DBG("Ending");
	egueb_smil_signal_disable(thiz->signal);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
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

static void _egueb_smil_animate_base_instance_init(void *o)
{
	Egueb_Smil_Animate_Base *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SMIL_ANIMATE_BASE(o);
	/* create the properties */
	thiz->additive = egueb_smil_attr_additive_new(
			egueb_dom_string_ref(EGUEB_SMIL_ADDITIVE), EGUEB_SMIL_ADDITIVE_REPLACE);
	thiz->by = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SMIL_BY), NULL, EINA_FALSE,
			EINA_FALSE, EINA_FALSE);
	thiz->to = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SMIL_TO), NULL, EINA_FALSE,
			EINA_FALSE, EINA_FALSE);
	thiz->from = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_SMIL_FROM), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->values = egueb_dom_attr_string_list_new(
			egueb_dom_string_ref(EGUEB_SMIL_VALUES), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->calc_mode = egueb_smil_attr_calc_mode_new();

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->by), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->to), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->from), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->values), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->calc_mode), NULL);
}

static void _egueb_smil_animate_base_instance_deinit(void *o)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = EGUEB_SMIL_ANIMATE_BASE(o);
	/* the cleanup will be called as part of the deinitialization */
	egueb_dom_node_unref(thiz->additive);
	egueb_dom_node_unref(thiz->by);
	egueb_dom_node_unref(thiz->to);
	egueb_dom_node_unref(thiz->from);
	egueb_dom_node_unref(thiz->values);
	egueb_dom_node_unref(thiz->calc_mode);
#if 0
	egueb_dom_node_unref(thiz->key_times);
	egueb_dom_node_unref(thiz->key_splines);
#endif
	egueb_smil_signal_unref(thiz->signal);
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
