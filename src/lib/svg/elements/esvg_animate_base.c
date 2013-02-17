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
#include "esvg_private_animate_base.h"
#include "esvg_private_svg.h"

#include "esvg_animate_base.h"
#include "esvg_element_animation.h"
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
#define ESVG_LOG_DEFAULT _esvg_animate_base_log

static int _esvg_animate_base_log = -1;

#define ESVG_ANIMATE_BASE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_ANIMATE_BASE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_ANIMATE_BASE_MAGIC);\
	} while(0)

static Ender_Property *ESVG_ANIMATE_BASE_TO;
static Ender_Property *ESVG_ANIMATE_BASE_FROM;
static Ender_Property *ESVG_ANIMATE_BASE_VALUES;
static Ender_Property *ESVG_ANIMATE_BASE_KEY_TIMES;
static Ender_Property *ESVG_ANIMATE_BASE_KEY_SPLINES;
static Ender_Property *ESVG_ANIMATE_BASE_CALC_MODE;

typedef struct _Esvg_Animate_Base_Values_Data
{
	Eina_List *values;
	Esvg_Attribute_Animated_Descriptor *d;
} Esvg_Animate_Base_Values_Data;

typedef struct _Esvg_Animate_Base_Times_Data
{
	Eina_List *times;
	int64_t duration;
} Esvg_Animate_Base_Times_Data;

typedef struct _Esvg_Animate_Base_Descriptor_Internal
{
	Edom_Tag_Free free;
	Esvg_Element_Initialize initialize;
	Esvg_Element_Attribute_Set attribute_set;
	Edom_Tag_Attribute_Get attribute_get;
	Esvg_Animate_Base_Attribute_Descriptor_Get type_descriptor_get;
} Esvg_Animate_Base_Descriptor_Internal;

typedef struct _Esvg_Animate_Base
{
	EINA_MAGIC
	/* properties */
	Esvg_Animate_Base_Context current;
	/* interface */
	Esvg_Animate_Base_Descriptor_Internal descriptor;
	/* private */
	Ender_Element *thiz_e;
	Esvg_Attribute_Type attribute_type;
	Esvg_Attribute_Animated_Descriptor *d;
	Eina_List *values;
	Eina_List *times;
	void *destination_data;
	void *destination_add;
	void *destination_acc;
	int repeat_count;
	/* parent relation */
	Ender_Element *parent_e;
	Edom_Tag *parent_t;
	Ender_Property *p;
	/* etch related data */
	Etch *etch;
	Etch_Animation *etch_a;
	/* for the inheritance */
	void *data;
} Esvg_Animate_Base;

static Esvg_Animate_Base * _esvg_animate_base_get(Edom_Tag *t)
{
	Esvg_Animate_Base *thiz;

	thiz = esvg_element_animation_data_get(t);
	ESVG_ANIMATE_BASE_MAGIC_CHECK(thiz);

	return thiz;
}

static Etch_Interpolator_Type _esvg_animate_base_calc_mode_etch_to(Esvg_Calc_Mode c)
{
	switch (c)
	{
		case ESVG_CALC_MODE_DISCRETE:
		return ETCH_INTERPOLATOR_DISCRETE;

		case ESVG_CALC_MODE_LINEAR:
		return ETCH_INTERPOLATOR_LINEAR;

		case ESVG_CALC_MODE_SPLINE:
		return ETCH_INTERPOLATOR_CUBIC;

		/* FIXME TODO */
		case ESVG_CALC_MODE_PACED:
		default:
		return ETCH_INTERPOLATOR_LINEAR;
	}
}

static void * _esvg_animate_base_property_destination_new(Esvg_Animate_Base *thiz)
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

static void * _esvg_animate_base_property_destination_get(Esvg_Animate_Base *thiz)
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

static void * _esvg_animate_base_property_value_get(Esvg_Animate_Base *thiz, void *value)
{
	void *destination;

	destination = thiz->d->destination_new();
	ender_element_property_value_get(thiz->parent_e, thiz->p, destination, NULL);
	/* convert it to a value */
	thiz->d->destination_value_to(destination, &value);
	thiz->d->destination_free(destination, EINA_FALSE);
	return value;
}

static void _esvg_animate_base_animation_add_keyframe(Etch_Animation *a,
	Esvg_Animate_Base_Context *c,
	Etch_Data *etch_data,
	int64_t time)
{
	Etch_Animation_Keyframe *k;
	Etch_Interpolator_Type atype;

	DBG("Adding keyframe at time %" ETCH_TIME_FORMAT, ETCH_TIME_ARGS(time));
	atype =_esvg_animate_base_calc_mode_etch_to(c->value.calc_mode);
	k = etch_animation_keyframe_add(a);
	etch_animation_keyframe_type_set(k, atype);
	etch_animation_keyframe_value_set(k, etch_data);
	/* set the time */
	etch_animation_keyframe_time_set(k, time);
}

static void _esvg_animate_base_interpolator(Etch_Data *a, Etch_Data *b, double m, Etch_Data *res, void *data)
{
	Esvg_Animate_Base *thiz = data;

	thiz->d->interpolator(a->data.external, b->data.external, m, thiz->destination_add, thiz->destination_acc, thiz->repeat_count, res->data.external);
}

static void _esvg_animate_base_interpolator_add(Etch_Data *a, Etch_Data *b, double m, Etch_Data *res, void *data)
{
	Esvg_Animate_Base *thiz = data;

	ender_element_property_value_get(thiz->parent_e, thiz->p, thiz->destination_add, NULL);
	_esvg_animate_base_interpolator(a, b, m, res, data);
}

/* FIXME we miss load and repeat events.
 * those have to be added on etch
 */
static void _esvg_animate_base_animation_cb(Etch_Animation_Keyframe *k,
		const Etch_Data *curr,
		const Etch_Data *prev,
		void *user_data)
{
	Esvg_Animate_Base *thiz = user_data;
	Esvg_Attribute_Type old_type;

	old_type = esvg_element_attribute_type_get(thiz->parent_t);
	esvg_element_attribute_type_set(thiz->parent_t, thiz->attribute_type);
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_TRUE);
	ender_element_property_value_set(thiz->parent_e, thiz->p, thiz->destination_data, NULL);
	/* restore the states */
	esvg_element_attribute_animate_set(thiz->parent_t, EINA_FALSE);
	esvg_element_attribute_type_set(thiz->parent_t, old_type);
}

static void _esvg_animate_base_animation_start_cb(Etch_Animation *a, void *data)
{
	Esvg_Animate_Base *thiz = data;
	ender_event_dispatch(thiz->thiz_e, "begin", NULL);
}

static void _esvg_animate_base_animation_start_and_fetch_cb(Etch_Animation *a, void *data)
{
	Esvg_Animate_Base *thiz = data;
	Etch_Animation_Keyframe *k;
	Etch_Data kd;
	void *first;

	/* get the first value and store the current value there */
	first = eina_list_data_get(thiz->values);
	first = _esvg_animate_base_property_value_get(thiz, first);
	/* set it as the first keyframe data */
	eina_list_data_set(thiz->values, first);
	/* replace the values pointer */
	k = etch_animation_keyframe_get(thiz->etch_a, 0);
	kd.type = ETCH_EXTERNAL;
	kd.data.external = first;
	etch_animation_keyframe_value_set(k, &kd);

	/* finally cal the animation */
	_esvg_animate_base_animation_start_cb(a, data);
}

static void _esvg_animate_base_animation_stop_cb(Etch_Animation *a, void *data)
{
	Esvg_Animate_Base *thiz = data;

	thiz->repeat_count = 0;
	ender_event_dispatch(thiz->thiz_e, "end", NULL);
}

static void _esvg_animate_base_animation_repeat_cb(Etch_Animation *a, void *data)
{
	Esvg_Animate_Base *thiz = data;

	thiz->repeat_count++;
	ender_event_dispatch(thiz->thiz_e, "repeat", NULL);
}

#if 0
static void _esvg_animate_key_splines_cb(const char *v, void *user_data)
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
#endif

static void _esvg_animate_base_values_cb(const char *v, void *user_data)
{
	Esvg_Animate_Base_Values_Data *data = user_data;
	void *get_data;

	get_data = data->d->value_new();
	if (data->d->value_get(v, &get_data))
		data->values = eina_list_append(data->values, get_data);
	else
		data->d->value_free(get_data);
}

static void _esvg_animate_base_time_cb(const char *v, void *user_data)
{
	Esvg_Animate_Base_Times_Data *data = user_data;
	Esvg_Number n;
	double percent;
	int64_t *t;

	esvg_number_string_from(&n, v, 1.0);
	percent = n.value;
	if (percent < 0.0)
		percent = 0;
	else if (percent > 1.0)
		percent = 1.0;

	t = malloc(sizeof(int64_t));
	*t = data->duration * percent;
	data->times = eina_list_append(data->times, t);
}

static void _esvg_animate_base_key_splines_cb(const char *v, void *user_data)
{
	Eina_List **ret = user_data;
	Esvg_Animate_Key_Spline *spline;

	/* fetch the four values */
	spline = calloc(1, sizeof(Esvg_Animate_Key_Spline));
	esvg_animate_key_spline_string_from(spline, v);
	*ret = eina_list_append(*ret, spline);
}

static Eina_Bool _esvg_animate_base_values_generate(Esvg_Animate_Base_Context *c,
		Esvg_Attribute_Animated_Descriptor *d,
		Eina_List **values,
		Eina_Bool *has_from)
{
	*has_from = EINA_TRUE;

	if (c->value.values)
	{
		Esvg_Animate_Base_Values_Data data;

		data.values = *values;
		data.d = d;
		esvg_list_string_from(c->value.values, ';',
			_esvg_animate_base_values_cb, &data);
		*values = data.values;
	}
	else
	{
		if (c->value.from)
		{
			void *data;

			data = d->value_new();
			if (d->value_get(c->value.from, &data))
				*values = eina_list_append(*values, data);
			else
				d->value_free(data);
		}
		else
		{
			/* mark the missing from */
			*has_from = EINA_FALSE;
		}

		if (c->value.to)
		{
			void *data;

			data = d->value_new();
			if (d->value_get(c->value.to, &data))
				*values = eina_list_append(*values, data);
			else
				d->value_free(data);
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

static void _esvg_animate_base_values_free(Eina_List *values, Esvg_Attribute_Animated_Value_Free free_cb)
{
	void *data;

	if (!values) return;

	EINA_LIST_FREE (values, data);
		free_cb(data);
}

static Eina_Bool _esvg_animate_base_key_splines_generate(Esvg_Animate_Base_Context *c,
		Eina_List **ksplines)
{
	Eina_List *l = NULL;

	if (!c->value.key_splines)
		return EINA_TRUE;

	esvg_list_string_from(c->value.key_splines, ';',
		_esvg_animate_base_key_splines_cb, &l);
	*ksplines = l;
}


static Eina_Bool _esvg_animate_base_times_generate(Esvg_Duration *dur,
		Esvg_Animate_Base_Context *c,
		Eina_List *values,
		Eina_List **times)
{
	/* generate the times list */
	/* get the duration */
	if (dur->type == ESVG_DURATION_TYPE_CLOCK)
	{
		if (c->value.key_times)
		{
			Esvg_Animate_Base_Times_Data data;

			data.times = *times;
			data.duration = dur->data.clock;

			esvg_list_string_from(c->value.key_times, ';',
					_esvg_animate_base_time_cb, &data);
			*times = data.times;
		}
		else
		{
			int64_t t = 0;
			int i;
			int length;
			int64_t duration;
			int64_t inc;

			length = eina_list_count(values);
			if (!length)
			{
				printf("no values?\n");
				return EINA_FALSE;
			}
			duration = dur->data.clock;
			inc = duration / (length - 1);
			for (i = 0; i < length; i++)
			{
				int64_t *d;

				d = malloc(sizeof(int64_t));
				*d = t;
				DBG("Adding time %" ETCH_TIME_FORMAT, ETCH_TIME_ARGS(t));
				*times = eina_list_append(*times, d);
				t += inc;
			}
		}
	}
	return EINA_TRUE;
}

static void _esvg_animate_base_times_free(Eina_List *times)
{
	int64_t *v;

	if (!times) return;
	EINA_LIST_FREE (times, v)
		free(v);
}

static void _esvg_animate_base_animation_create(Esvg_Animate_Base *thiz,
		Esvg_Animation_Context *actx)
{
	Etch_Animation_State_Callback start_cb;
	Etch_Interpolator interpolator_cb;
	Etch_Animation *etch_a;
	Eina_List *l, *tt;
	Eina_Bool has_from;
	void *v;
	int64_t *time;

	/* default variants */
	interpolator_cb = _esvg_animate_base_interpolator;
	start_cb = _esvg_animate_base_animation_start_cb;
	/* generate the values and times */
	_esvg_animate_base_values_generate(&thiz->current, thiz->d,
			&thiz->values, &has_from);
	/* before adding a new empty value, create the destination holder */
	thiz->destination_data = _esvg_animate_base_property_destination_new(thiz);
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
		start_cb = _esvg_animate_base_animation_start_and_fetch_cb;
	}
	_esvg_animate_base_times_generate(&actx->timing.dur, &thiz->current, thiz->values, &thiz->times);
	if (!thiz->times)
	{
		ERR("No time defined");
		return;
	}

	/* check if we are the first animation */
	if (actx->addition.additive == ESVG_ADDITIVE_SUM)
	{
		thiz->destination_add = _esvg_animate_base_property_destination_get(thiz);
		if (actx->index > 1)
		{
			interpolator_cb = _esvg_animate_base_interpolator_add;
		}
	}
	if (actx->addition.accumulate == ESVG_ACCUMULATE_SUM)
	{
		Eina_List *l2;

		/* get the latest value and keep it */
		l2 = eina_list_last(thiz->values);
		thiz->destination_acc = eina_list_data_get(l2);
	}
	/* create the animation */
	etch_a = etch_animation_external_add(thiz->etch,
			interpolator_cb,
			_esvg_animate_base_animation_cb,
			start_cb,
			_esvg_animate_base_animation_stop_cb,
			_esvg_animate_base_animation_repeat_cb,
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
		_esvg_animate_base_animation_add_keyframe(thiz->etch_a, &thiz->current, &edata, *time);
		tt = eina_list_next(tt);
	}
}

static void _esvg_animate_base_cleanup(Esvg_Animate_Base *thiz)
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
		_esvg_animate_base_values_free(thiz->values, thiz->d->value_free);
		thiz->values = NULL;
	}
	if (thiz->times)
	{
		_esvg_animate_base_times_free(thiz->times);
		thiz->times = NULL;
	}
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_base_to_set(Edom_Tag *t, const char *to)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (thiz->current.value.to)
	{
		free(thiz->current.value.to);
		thiz->current.value.to = NULL;
	}
	if (to)
		thiz->current.value.to = strdup(to);
	thiz->current.changed = EINA_TRUE;
}

static void _esvg_animate_base_to_get(Edom_Tag *t, const char **to)
{
	Esvg_Animate_Base *thiz;

	if (!to) return;
	thiz = _esvg_animate_base_get(t);
	*to = thiz->current.value.to;
}

static void _esvg_animate_base_from_set(Edom_Tag *t, const char *from)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (thiz->current.value.from)
	{
		free(thiz->current.value.from);
		thiz->current.value.from = NULL;
	}
	if (from)
		thiz->current.value.from = strdup(from);
	thiz->current.changed = EINA_TRUE;
}

static void _esvg_animate_base_from_get(Edom_Tag *t, const char **from)
{
	Esvg_Animate_Base *thiz;

	if (!from) return;
	thiz = _esvg_animate_base_get(t);
	*from = thiz->current.value.from;
}

static void _esvg_animate_base_values_set(Edom_Tag *t, const char *values)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (thiz->current.value.values)
	{
		free(thiz->current.value.values);
		thiz->current.value.values = NULL;
	}
	if (values)
		thiz->current.value.values = strdup(values);
	thiz->current.changed = EINA_TRUE;
}

static void _esvg_animate_base_values_get(Edom_Tag *t, const char **values)
{
	Esvg_Animate_Base *thiz;

	if (!values) return;
	thiz = _esvg_animate_base_get(t);
	*values = thiz->current.value.values;
}

static void _esvg_animate_base_calc_mode_set(Edom_Tag *t, Esvg_Calc_Mode calc_mode)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	thiz->current.value.calc_mode = calc_mode;
	thiz->current.changed = EINA_TRUE;
}

static void _esvg_animate_base_calc_mode_get(Edom_Tag *t, Esvg_Calc_Mode *calc_mode)
{
	Esvg_Animate_Base *thiz;

	if (!calc_mode) return;
	thiz = _esvg_animate_base_get(t);
	*calc_mode = thiz->current.value.calc_mode;
}

static void _esvg_animate_base_key_times_set(Edom_Tag *t, const char *key_times)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (thiz->current.value.key_times)
	{
		free(thiz->current.value.key_times);
		thiz->current.value.key_times = NULL;
	}
	if (key_times)
		thiz->current.value.key_times = strdup(key_times);
	thiz->current.changed = EINA_TRUE;
}

static void _esvg_animate_base_key_times_get(Edom_Tag *t, const char **key_times)
{
	Esvg_Animate_Base *thiz;

	if (!key_times) return;
	thiz = _esvg_animate_base_get(t);
	*key_times = thiz->current.value.key_times;
}

static void _esvg_animate_base_key_splines_set(Edom_Tag *t, const char *key_splines)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (thiz->current.value.key_splines)
	{
		free(thiz->current.value.key_splines);
		thiz->current.value.key_splines = NULL;
	}
	if (key_splines)
		thiz->current.value.key_splines = strdup(key_splines);
	thiz->current.changed = EINA_TRUE;
}

static void _esvg_animate_base_key_splines_get(Edom_Tag *t, const char **key_splines)
{
	Esvg_Animate_Base *thiz;

	if (!key_splines) return;
	thiz = _esvg_animate_base_get(t);
	*key_splines = thiz->current.value.key_splines;
}
/*----------------------------------------------------------------------------*
 *                        The Esvg Animation interface                        *
 *----------------------------------------------------------------------------*/
static void _esvg_animate_base_enable(Edom_Tag *t, int64_t offset)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (!thiz->etch_a) return;

	etch_animation_offset_add(thiz->etch_a, offset);
	etch_animation_enable(thiz->etch_a);
}

static void _esvg_animate_base_disable(Edom_Tag *t)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	if (!thiz->etch_a) return;

	etch_animation_disable(thiz->etch_a);
}

static void _esvg_animate_base_initialize(Ender_Element *e)
{
	Esvg_Animate_Base *thiz;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_animate_base_get(t);
	thiz->thiz_e = e;

	/* call the interface */
	if (thiz->descriptor.initialize)
		thiz->descriptor.initialize(e);
}

static Eina_Bool _esvg_animate_base_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	/* value attributes */
	/* for from and to, we should not fetch the property an generate
	 * the final data type, we better pass it still as strings
	 */
	if (strcmp(key, "calcMode") == 0)
	{
		Esvg_Calc_Mode cm;

		esvg_calc_mode_string_from(&cm, value);
		esvg_animate_base_calc_mode_set(e, cm);
	}
	else if (strcmp(key, "values") == 0)
	{
		esvg_animate_base_values_set(e, value);
	}
	else if (strcmp(key, "keyTimes") == 0)
	{
		esvg_animate_base_key_times_set(e, value);
	}
	else if (strcmp(key, "keySplines") == 0)
	{
		esvg_animate_base_key_splines_set(e, value);
	}
	else if (strcmp(key, "from") == 0)
	{
		esvg_animate_base_from_set(e, value);
	}
	else if (strcmp(key, "to") == 0)
	{
		esvg_animate_base_to_set(e, value);
	}
	else if (strcmp(key, "by") == 0)
	{
	}
	else
	{
		Esvg_Animate_Base *thiz;
		Edom_Tag *t;

		t = ender_element_object_get(e);
		thiz = _esvg_animate_base_get(t);

		if (thiz->descriptor.attribute_set)
			return thiz->descriptor.attribute_set(e, key, value);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _esvg_animate_base_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static void _esvg_animate_base_free(Edom_Tag *t)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	_esvg_animate_base_cleanup(thiz);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}

static Eina_Bool _esvg_animate_base_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Animation_Context *actx,
		Enesim_Error **error)
{
	Esvg_Animate_Base *thiz;
	Esvg_Attribute_Animated_Descriptor *d;
	Ender_Element *svg_e;
	Ender_Container *ec;
	Ender_Descriptor *descriptor;
	const Ender_Constraint *cnst;
	Ender_Constraint_Type cnst_type;
	Etch *etch;
	const char *name;

	thiz = _esvg_animate_base_get(t);
	_esvg_animate_base_cleanup(thiz);

	/* TODO pass the etch from the animation class */
	/* TODO simplify the animation context */

	/* get the etch associated with the topmost svg */
	esvg_element_internal_topmost_get(t, &svg_e);
	if (!svg_e)
		return EINA_FALSE;

	etch = esvg_element_svg_etch_get(svg_e);
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
	_esvg_animate_base_animation_create(thiz, actx);

	return EINA_TRUE;
}

/* The ender wrapper */
#define _esvg_animate_base_delete NULL
#define _esvg_animate_base_to_is_set NULL
#define _esvg_animate_base_from_is_set NULL
#define _esvg_animate_base_values_is_set NULL
#define _esvg_animate_base_calc_mode_is_set NULL
#define _esvg_animate_base_key_times_is_set NULL
#define _esvg_animate_base_key_splines_is_set NULL
#include "esvg_generated_animate_base.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_animate_base_init(void)
{
	_esvg_animate_base_log = eina_log_domain_register("esvg_animate_base", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_animate_base_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_animate_base_init();
}

void esvg_animate_base_shutdown(void)
{
	if (_esvg_animate_base_log < 0)
		return;
	_esvg_animate_base_shutdown();
	eina_log_domain_unregister(_esvg_animate_base_log);
	_esvg_animate_base_log = -1;
}

Eina_Bool esvg_is_animate_base_internal(Edom_Tag *t)
{
	Esvg_Animate_Base *thiz;
	Eina_Bool ret;

	if (!esvg_is_element_internal(t))
		return EINA_FALSE;
	thiz = esvg_element_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_ANIMATE_BASE_MAGIC);

	return ret;
}

void * esvg_animate_base_data_get(Edom_Tag *t)
{
	Esvg_Animate_Base *thiz;

	thiz = _esvg_animate_base_get(t);
	return thiz->data;
}

Edom_Tag * esvg_animate_base_new(Esvg_Animate_Base_Descriptor *descriptor, Esvg_Type type,
		void *data)
{
	Esvg_Animate_Base *thiz;
	Esvg_Animation_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Animate_Base));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_ANIMATE_BASE_MAGIC);
	thiz->data = data;
	/* default values */
	thiz->current.value.calc_mode = ESVG_CALC_MODE_LINEAR;
	/* our own descriptor */
	thiz->descriptor.type_descriptor_get = descriptor->type_descriptor_get;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->descriptor.attribute_get = descriptor->attribute_get;
	thiz->descriptor.initialize = descriptor->initialize;
	/* parent descriptor */
	pdescriptor.attribute_set = _esvg_animate_base_attribute_set;
	pdescriptor.attribute_get = _esvg_animate_base_attribute_get;
	pdescriptor.free = _esvg_animate_base_free;
	pdescriptor.initialize = _esvg_animate_base_initialize;
	pdescriptor.setup = _esvg_animate_base_setup;
	pdescriptor.enable = _esvg_animate_base_enable;
	pdescriptor.disable = _esvg_animate_base_disable;

	t = esvg_element_animation_new(&pdescriptor, type, thiz);

	return t;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_animate_base(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_animate_base_internal(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_to_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_BASE_TO, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_to_get(Ender_Element *e, const char **v)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animate_base_to_get(t, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_from_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_BASE_FROM, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_from_get(Ender_Element *e, const char **v)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animate_base_from_get(t, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_values_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_BASE_VALUES, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_values_get(Ender_Element *e, const char **v)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animate_base_values_get(t, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_calc_mode_set(Ender_Element *e, Esvg_Calc_Mode calc_mode)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_BASE_CALC_MODE, calc_mode, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_calc_mode_get(Ender_Element *e, Esvg_Calc_Mode *calc_mode)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animate_base_calc_mode_get(t, calc_mode);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_key_times_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_BASE_KEY_TIMES, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_key_times_get(Ender_Element *e, const char **v)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animate_base_key_times_get(t, v);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_key_splines_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ANIMATE_BASE_KEY_SPLINES, v, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_animate_base_key_splines_get(Ender_Element *e, const char **v)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	_esvg_animate_base_key_splines_get(t, v);
}

