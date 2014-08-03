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
#include "egueb_smil_accumulate.h"
#include "egueb_smil_calc_mode.h"
#include "egueb_smil_clock.h"
#include "egueb_smil_keyframe.h"
#include "egueb_smil_timeline.h"
#include "egueb_smil_event.h"

#include "egueb_smil_key_spline_private.h"
#include "egueb_smil_animation_private.h"
#include "egueb_smil_animate_base_private.h"
#include "egueb_smil_attr_additive_private.h"
#include "egueb_smil_attr_accumulate_private.h"
#include "egueb_smil_attr_calc_mode_private.h"
#include "egueb_smil_attr_key_splines_private.h"

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

/* Yes it might be silly to do this, bu we do not have any dom list iterator yet */
static void _egueb_smil_animate_base_key_splines_cb(void *data, void *user_data)
{
	Egueb_Smil_Animate_Base *thiz = user_data;
	Egueb_Smil_Key_Spline *ks = data;
	Egueb_Smil_Key_Spline *gks;

	gks = calloc(1, sizeof(Egueb_Smil_Key_Spline));
	*gks = *ks;
	thiz->generated_keysplines = eina_list_append(thiz->generated_keysplines, gks);
}

static Eina_Bool _egueb_smil_animate_base_key_splines_generate(Egueb_Smil_Animate_Base *thiz)
{
	Egueb_Dom_List *key_splines = NULL;
	Eina_Bool ret;

	/* check that we do have key splines and that are valid */
	egueb_dom_attr_get(thiz->key_splines, EGUEB_DOM_ATTR_TYPE_BASE, &key_splines);
	if (key_splines)
	{
		int length;

		length = eina_list_count(thiz->generated_values);
		if (length != egueb_dom_list_length(key_splines) + 1)
		{
			WARN("The number of key splines (%d) do not match the"
					" number of values (%d)",
					egueb_dom_list_length(key_splines), length);
			ret = EINA_FALSE;
		}
		else
		{
			DBG("Using the provided keySplines");
			egueb_dom_list_foreach(key_splines, _egueb_smil_animate_base_key_splines_cb, thiz);
			ret = EINA_TRUE;
		}
		egueb_dom_list_unref(key_splines);
		return ret;
	}
	else
	{
		WARN("No keySplines set");
		return EINA_FALSE;
	}
}

static void _egueb_smil_animate_base_key_splines_free(Egueb_Smil_Animate_Base *thiz)
{
	Egueb_Smil_Key_Spline *spline;

	if (!thiz->generated_keysplines)
		return;
	EINA_LIST_FREE(thiz->generated_keysplines, spline)
		free(spline);
	thiz->generated_keysplines = NULL;
}

static Eina_Bool _egueb_smil_animate_base_calc_mode_generate(
		Egueb_Smil_Animate_Base *thiz)
{
	Egueb_Smil_Calc_Mode calc_mode = EGUEB_SMIL_CALC_MODE_LINEAR;
	Egueb_Smil_Keyframe_Interpolator_Type itype;

	egueb_dom_attr_final_get(thiz->calc_mode, &calc_mode);
	switch (calc_mode)
	{
		case EGUEB_SMIL_CALC_MODE_DISCRETE:
		itype = EGUEB_SMIL_KEYFRAME_INTERPOLATOR_DISCRETE;
		break;

		case EGUEB_SMIL_CALC_MODE_LINEAR:
		itype = EGUEB_SMIL_KEYFRAME_INTERPOLATOR_LINEAR;
		break;

		case EGUEB_SMIL_CALC_MODE_SPLINE:
		{
			if (_egueb_smil_animate_base_key_splines_generate(thiz))
				itype = EGUEB_SMIL_KEYFRAME_INTERPOLATOR_CUBIC;
			else
				itype = EGUEB_SMIL_KEYFRAME_INTERPOLATOR_LINEAR;
		}
		break;

		/* TODO handle the paced case */
		case EGUEB_SMIL_CALC_MODE_PACED:
		default:
		itype = EGUEB_SMIL_KEYFRAME_INTERPOLATOR_LINEAR;
		break;
	}
	thiz->itype = itype;
	return EINA_TRUE;
}

static void _egueb_smil_animate_base_keyframes_generate(Egueb_Smil_Animate_Base *thiz)
{
	Egueb_Dom_Value *v;
	Eina_List *l, *tt;

	/* add the keyframes */
	tt = thiz->generated_times;
	if (thiz->itype != EGUEB_SMIL_KEYFRAME_INTERPOLATOR_CUBIC)
	{
		EINA_LIST_FOREACH(thiz->generated_values, l, v)
		{
			int64_t *time;

			time = eina_list_data_get(tt);

			/* add a keyframe */
			DBG("Adding keyframe at time %"
					EGUEB_SMIL_CLOCK_FORMAT,
					EGUEB_SMIL_CLOCK_ARGS(*time));
			egueb_smil_signal_continuous_keyframe_simple_add(thiz->signal,
					thiz->itype, *time, v);
			tt = eina_list_next(tt);
		}
	}
	else
	{
		Eina_List *ss;

		ss = thiz->generated_keysplines; 
		EINA_LIST_FOREACH(thiz->generated_values, l, v)
		{
			Egueb_Smil_Key_Spline *spline;
			double x0 = 0;
			double y0 = 0;
			double x1 = 1;
			double y1 = 1;
			int64_t *time;

			time = eina_list_data_get(tt);
			spline = eina_list_data_get(ss);

			/* add a keyframe */
			if (spline)
			{
				x0 = spline->x;
				y0 = spline->y;
				x1 = spline->cx;
				y1 = spline->cy;
			}
			DBG("Adding spline based keyframe at time %"
					EGUEB_SMIL_CLOCK_FORMAT,
					EGUEB_SMIL_CLOCK_ARGS(*time));
			egueb_smil_signal_continuous_keyframe_cubic_add(thiz->signal,
					*time, x0, y0, x1, y1, v);
			tt = eina_list_next(tt);
			ss = eina_list_next(ss);
		}
	}
}

/*----------------------------------------------------------------------------*
 *                        The Etch external animation                         *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_animate_base_interpolator_cb(Egueb_Dom_Value *va, Egueb_Dom_Value *vb,
		double m, void *data)
{
	Egueb_Smil_Animation *a;
	Egueb_Smil_Animate_Base *thiz = data;
	Egueb_Smil_Animate_Base_Class *klass;

	klass = EGUEB_SMIL_ANIMATE_BASE_CLASS_GET(thiz);
	if (!klass->interpolate(thiz, va, vb, m, thiz->add_value, NULL, 0))
		return;

	a = EGUEB_SMIL_ANIMATION(thiz);
	egueb_dom_attr_value_set(a->attr, EGUEB_DOM_ATTR_TYPE_ANIMATED, &thiz->dst_value);
}

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
		/* TODO do a copy, as it might be a reference to a value */
		egueb_dom_attr_final_value_get(a->attr, &thiz->prv_value);
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
	egueb_dom_attr_final_value_get(a->attr, first);
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
		egueb_dom_attr_value_set(a->attr, EGUEB_DOM_ATTR_TYPE_ANIMATED, &thiz->prv_value);
	}
	ev = egueb_smil_event_new();
	egueb_smil_event_init(ev, egueb_dom_string_ref(EGUEB_SMIL_EVENT_END), 0);
	egueb_dom_node_event_dispatch(a->target, ev, NULL, NULL);
}

static void _egueb_smil_animate_base_animation_repeat_cb(Egueb_Smil_Signal *s, void *data)
{
	/* TODO send the repeat event */
}

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

static void _egueb_smil_animate_base_values_free(Egueb_Smil_Animate_Base *thiz)
{
	Egueb_Dom_Value *value;

	if (!thiz->generated_values)
		return;

	EINA_LIST_FREE(thiz->generated_values, value)
	{
		if (value)
		{
			egueb_dom_value_reset(value);
			free(value);
		}
	}
	thiz->generated_values = NULL;
}

static void _egueb_smil_animate_base_key_times_generate(void *data, void *user_data)
{
	Egueb_Smil_Animate_Base *thiz = user_data;
	Egueb_Smil_Animation *a;
	Egueb_Smil_Duration dur;
	double key_time;
	int64_t *d;
	int64_t t;

	a = EGUEB_SMIL_ANIMATION(thiz);

	/* the range of the values goes from 0 - 1, being the range the
	 * simple duration
	 */
	egueb_dom_attr_get(a->dur, EGUEB_DOM_ATTR_TYPE_BASE, &dur);
 	key_time = *((double *)data);
	t = dur.data.clock * key_time;

	d = malloc(sizeof(int64_t));
	*d = t;
	DBG("Adding time %" EGUEB_SMIL_CLOCK_FORMAT, EGUEB_SMIL_CLOCK_ARGS(t));
	thiz->generated_times = eina_list_append(thiz->generated_times, d);
}

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
		Egueb_Dom_List *key_times = NULL;
		int length;

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

		egueb_dom_attr_get(thiz->key_times, EGUEB_DOM_ATTR_TYPE_BASE, &key_times);
		if (key_times)
		{
			if (egueb_dom_list_length(key_times) != length)
			{
				egueb_dom_list_unref(key_times);
				ERR("The number of key times (%d) do not match the"
						" number of values (%d)",
						egueb_dom_list_length(key_times), length);
				return EINA_FALSE;
			}
			egueb_dom_list_foreach(key_times, _egueb_smil_animate_base_key_times_generate, thiz);
			egueb_dom_list_unref(key_times);
		}
		else
		{
			int64_t t = 0;
			int i;
			int64_t duration;
			int64_t inc;

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

static void _egueb_smil_animate_base_times_free(Egueb_Smil_Animate_Base *thiz)
{
	int64_t *v;

	if (!thiz->generated_times)
		return;
	EINA_LIST_FREE(thiz->generated_times, v)
		free(v);
	thiz->generated_times = NULL;
}

/*----------------------------------------------------------------------------*
 *                            Animate base interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_smil_animate_base_interpolate(
		Egueb_Smil_Animate_Base *thiz, Egueb_Dom_Value *va,
		Egueb_Dom_Value *vb, double m, Egueb_Dom_Value *add,
		Egueb_Dom_Value *mul, int accum)
{
	egueb_dom_value_interpolate(&thiz->dst_value, va, vb, m, thiz->add_value, mul, accum);
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Animation interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_smil_animate_base_setup(Egueb_Smil_Animation *a,
		Egueb_Dom_Node *target)
{
	Egueb_Smil_Animate_Base *thiz;
	Egueb_Smil_Additive additive;
	Egueb_Smil_Signal_Continuous_Process interpolator_cb;
	Egueb_Smil_Signal_State_Callback start_cb;
	Egueb_Dom_Value value = EGUEB_DOM_VALUE_INIT;
	Eina_Bool has_from, has_by;

	thiz = EGUEB_SMIL_ANIMATE_BASE(a);
	egueb_dom_attr_final_get(thiz->additive, &additive);

	if (!_egueb_smil_animate_base_values_generate(thiz, &has_from, &has_by))
	{
		ERR("Failed generating the values");
		return EINA_FALSE;
	}

	if (!thiz->generated_values)
	{
		ERR("No values generated");
		return EINA_FALSE;
	}

	if (!_egueb_smil_animate_base_times_generate(thiz))
	{
		ERR("Failed generating the times");
		_egueb_smil_animate_base_values_free(thiz);
		return EINA_FALSE;
	}
	if (!thiz->generated_times)
	{
		ERR("No times generated");
		_egueb_smil_animate_base_values_free(thiz);
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
			egueb_dom_attr_final_value_get(a->attr, from);
			thiz->generated_values = eina_list_prepend(thiz->generated_values, from);

			start_cb = _egueb_smil_animate_base_animation_start_and_fetch_cb;
		}
	}
	if (!_egueb_smil_animate_base_calc_mode_generate(thiz))
	{
		ERR("Failed to generate the calc mode");
		_egueb_smil_animate_base_values_free(thiz);
		_egueb_smil_animate_base_times_free(thiz);
		return EINA_FALSE;
	}

	/* get a value and copy it so we can allocate in case we need to */
	egueb_dom_value_init(&value, egueb_dom_attr_value_descriptor_get(a->attr));
	egueb_dom_attr_final_value_get(a->attr, &value);

	/* setup the holder of the destination value */
	egueb_dom_value_init(&thiz->dst_value, egueb_dom_attr_value_descriptor_get(a->attr));
	egueb_dom_value_copy(&value, &thiz->dst_value, EINA_TRUE);
	egueb_dom_value_reset(&value);

	/* create the animation */
	thiz->signal = egueb_smil_signal_continuous_new(interpolator_cb, start_cb,
			_egueb_smil_animate_base_animation_stop_cb,
			_egueb_smil_animate_base_animation_repeat_cb, thiz);
	egueb_smil_timeline_signal_add(a->timeline, egueb_smil_signal_ref(thiz->signal));
	/* TODO the repeat count */
	/* TODO the repeat dur */
	/* get the interpolator type */
	_egueb_smil_animate_base_keyframes_generate(thiz);
	return EINA_TRUE;
}

static void _egueb_smil_animate_base_cleanup(Egueb_Smil_Animation *a,
		Egueb_Dom_Node *target)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = EGUEB_SMIL_ANIMATE_BASE(a);
	_egueb_smil_animate_base_values_free(thiz);
	_egueb_smil_animate_base_times_free(thiz);
	_egueb_smil_animate_base_key_splines_free(thiz);
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
	Egueb_Smil_Animate_Base_Class *thiz_klass;

	klass = EGUEB_SMIL_ANIMATION_CLASS(k);
	klass->cleanup = _egueb_smil_animate_base_cleanup;
	klass->setup = _egueb_smil_animate_base_setup;
	klass->begin = _egueb_smil_animate_base_begin;
	klass->end = _egueb_smil_animate_base_end;

	thiz_klass = EGUEB_SMIL_ANIMATE_BASE_CLASS(k);
	thiz_klass->interpolate = _egueb_smil_animate_base_interpolate;
}

static void _egueb_smil_animate_base_instance_init(void *o)
{
	Egueb_Smil_Animate_Base *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SMIL_ANIMATE_BASE(o);
	/* create the properties */
	thiz->additive = egueb_smil_attr_additive_new();
	thiz->accumulate = egueb_smil_attr_accumulate_new();
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
	thiz->key_times = egueb_dom_attr_double_list_new(
			egueb_dom_string_ref(EGUEB_SMIL_NAME_KEY_TIMES), NULL,
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->key_splines = egueb_smil_attr_key_splines_new();

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->additive), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->accumulate), NULL);
	/* values over time */
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->by), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->to), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->from), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->values), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->calc_mode), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->key_times), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->key_splines), NULL);
}

static void _egueb_smil_animate_base_instance_deinit(void *o)
{
	Egueb_Smil_Animate_Base *thiz;

	thiz = EGUEB_SMIL_ANIMATE_BASE(o);
	/* the cleanup will be called as part of the deinitialization */
	egueb_dom_node_unref(thiz->additive);
	egueb_dom_node_unref(thiz->accumulate);
	egueb_dom_node_unref(thiz->by);
	egueb_dom_node_unref(thiz->to);
	egueb_dom_node_unref(thiz->from);
	egueb_dom_node_unref(thiz->values);
	egueb_dom_node_unref(thiz->calc_mode);
	egueb_dom_node_unref(thiz->key_times);
	egueb_dom_node_unref(thiz->key_splines);
	egueb_smil_signal_unref(thiz->signal);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
