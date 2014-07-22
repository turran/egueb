/* ETCH - Timeline Based Animation Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata, Hisham Mardam-Bey
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
#include "egueb_smil_keyframe.h"
#include "egueb_smil_timeline.h"
#include "egueb_smil_signal.h"

#include "egueb_smil_signal_private.h"
#include "egueb_smil_timeline_private.h"
#include "egueb_smil_keyframe_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SMIL_SIGNAL_CONTINUOUS_DESCRIPTOR egueb_dom_signal_continuous_descriptor_get()
#define EGUEB_SMIL_SIGNAL_CONTINUOUS_CLASS_GET(o) EGUEB_SMIL_SIGNAL_CONTINUOUS_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SMIL_SIGNAL_CONTINUOUS_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Smil_Signal_Continuous_Class, EGUEB_SMIL_SIGNAL_CONTINUOUS_DESCRIPTOR)
#define EGUEB_SMIL_SIGNAL_CONTINUOUS(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Smil_Signal_Continuous, EGUEB_SMIL_SIGNAL_CONTINUOUS_DESCRIPTOR)

typedef struct _Egueb_Smil_Signal_Continuous
{
	Egueb_Smil_Signal parent;
	Egueb_Smil_Clock start;
	Egueb_Smil_Clock end;
	Eina_List *keyframes;
	int repeat; /** number of times the animation will repeat, -1 for infinite */

	Egueb_Smil_Signal_Continuous_Interpolator interpolator; /** the interpolator to use for the requested data type */
	Egueb_Smil_Signal_Process_Callback cb; /** function to call when a value has been set */
	Egueb_Smil_Signal_State_Callback repeat_cb;
	void *curr;
} Egueb_Smil_Signal_Continuous;

typedef struct _Egueb_Smil_Signal_Continuous_Class
{
	Egueb_Smil_Signal_Class parent;
} Egueb_Smil_Signal_Continuous_Class;

typedef double (*Egueb_Smil_Signal_Continuous_Interpolator_Calc)(double m, Egueb_Smil_Keyframe_Interpolator_Data *data);


static int _egueb_smil_signal_continuous_keyframes_compare(const void *data1, const void *data2)
{
	const Egueb_Smil_Keyframe *k1 = data1;
	const Egueb_Smil_Keyframe *k2 = data2;

	if (k1->time < k2->time)
		return -1;
	else if (k1->time > k2->time)
		return 1;
	else
		return 0;
}

static void _egueb_smil_signal_continuous_keyframes_add(Egueb_Smil_Signal_Continuous *thiz,
		Egueb_Smil_Keyframe *k)
{
	Eina_List *last;

	thiz->keyframes = eina_list_sorted_insert(thiz->keyframes,
			_egueb_smil_signal_continuous_keyframes_compare, k);

	/* update the start */
	k = thiz->keyframes->data;
	thiz->start = k->time;

	/* update the end */
	last = eina_list_last(thiz->keyframes);
	k = last->data;
	thiz->end = k->time;
}

/*----------------------------------------------------------------------------*
 *                               The calc types                               *
 *----------------------------------------------------------------------------*/
static double _calc_linear(double m, Egueb_Smil_Keyframe_Interpolator_Data *data)
{
	return m;
}

static double _calc_discrete(double m, Egueb_Smil_Keyframe_Interpolator_Data *data)
{
	return m < 1 ? 0 : 1;
}

static double _calc_cosin(double m, Egueb_Smil_Keyframe_Interpolator_Data *data)
{
	return (1 - cos(m * M_PI))/2;
}

static double _calc_quadratic(double m, Egueb_Smil_Keyframe_Interpolator_Data *data)
{
	double t;

        /* FIXME: check if data->q.x0 and data->q.y0 are in [0,1] ? */
	/* TODO: bench that algo and the one with de casteljau */
	t = (-data->q.x0 + sqrt(data->q.x0 * data->q.x0 + m * (1 - 2 * data->q.x0))) / (1 - 2 * data->q.x0);
	return ((1 - 2 * data->q.y0) * t + 2 * data->q.y0) * t;
}

static double _calc_cubic(double m, Egueb_Smil_Keyframe_Interpolator_Data *data)
{
	/* TODO */
	return m;
}

static Egueb_Smil_Signal_Continuous_Interpolator_Calc _calcs[ETCH_INTERPOLATOR_TYPES] = {
	/* ETCH_INTERPOLATOR_DISCRETE 	*/ _calc_discrete,
	/* ETCH_INTERPOLATOR_LINEAR 	*/ _calc_linear,
	/* ETCH_INTERPOLATOR_COSIN 	*/ _calc_cosin,
	/* ETCH_INTERPOLATOR_QUADRATIC 	*/ _calc_quadratic,
	/* ETCH_INTERPOLATOR_CUBIC 	*/ _calc_cubic,
};


/* This functions gets called on etch_process with curr time in an absolute
 * form, isnt better to pass a relative time (i.e relative to the start and end
 * of the animation) ?
 */
static void _egueb_smil_signal_continuous_animate(Egueb_Smil_Signal *s, Egueb_Smil_Clock curr)
{
	Egueb_Smil_Signal_Continuous *thiz;
	Egueb_Smil_Keyframe *start;
	Eina_List *l;

	thiz = EGUEB_SMIL_SIGNAL_CONTINUOUS(s);
	/* TODO instead of checking everytime every keyframe we can translate the
	 * keyframes based on the frame, when a keyframe has passed move it before
	 * like a circular list */
	EINA_LIST_FOREACH(thiz->keyframes, l, start)
	{
		Egueb_Smil_Keyframe *end;
		Eina_List *l_next = l->next;

		if (!l_next)
			break;

		end = l->data;
		if ((start->time <= curr) && (curr <= end->time))
		{
			Etch_Data old;
			double m;
			void *data = NULL;

			/* get the keyframe affected */
			//DBG("-> [%g] %g %g", curr, start->time, end->time);
			/* get the interval between 0 and 1 based on current frame and two keyframes */
			if (curr == start->time)
				m = 0;
			else if (curr == end->time)
				m = 1;
			else
				m = (double)(curr - start->time)/(end->time - start->time);
			/* calc the new m */
			m = _calcs[start->type](m, &start->data);
			/* interpolate the value with the new m */
			thiz->interpolator(start->value, end->value, m, thiz->curr, s->data);
			/* once the value has been set, call the callback */
			thiz->cb(thiz->curr, s->data);
			return;
		}

	}
}
/*----------------------------------------------------------------------------*
 *                              Signal interface                              *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_signal_continuous_process(Egueb_Smil_Signal *s,
		Egueb_Smil_Clock curr, unsigned int tpf)
{
	Egueb_Smil_Signal_Continuous *thiz;
	Egueb_Smil_Clock rcurr;
	Egueb_Smil_Clock atime; /* signal time */
	Egueb_Smil_Clock end;
	Egueb_Smil_Clock length;

	thiz = EGUEB_SMIL_SIGNAL_CONTINUOUS(s);
	/* TODO use thiz->start and thiz->end */
	DBG("[%" EGUEB_SMIL_CLOCK_FORMAT " %" EGUEB_SMIL_CLOCK_FORMAT "]"
			" %" EGUEB_SMIL_CLOCK_FORMAT \
			" %" EGUEB_SMIL_CLOCK_FORMAT \
			" %d",
			EGUEB_SMIL_CLOCK_ARGS (curr),
			EGUEB_SMIL_CLOCK_ARGS (s->offset),
			EGUEB_SMIL_CLOCK_ARGS (thiz->start),
			EGUEB_SMIL_CLOCK_ARGS (thiz->end),
			thiz->repeat);
	/*  are we after the start ? */
	if (curr < thiz->start + s->offset)
		return;
	/* some sanity checks */
	if (!(thiz->end - thiz->start))
		return;

	/* check if we have finished */
	if (thiz->repeat < 0)
		goto infinite;
	/* are we before the end ? */
	end = (thiz->end * thiz->repeat);
	if (curr > end + s->offset)
	{
		if (s->started && !s->stopped)
		{
			DBG("Stopping signal %p at %" EGUEB_SMIL_CLOCK_FORMAT
					" with end %" EGUEB_SMIL_CLOCK_FORMAT,
					thiz,
					EGUEB_SMIL_CLOCK_ARGS (curr),
					EGUEB_SMIL_CLOCK_ARGS (thiz->end));
			/* send the last tick that will trigger the signal
			 * for the end value
			 */
			_egueb_smil_signal_continuous_animate(s, thiz->end);
			egueb_smil_signal_stop(s);
		}
		else
		{
			WRN("Signal did not had time to start");
		}
		return;
	}
infinite:
	/* normalize to signal time */
	atime = curr - (thiz->start + s->offset);

	/* ok we are on the range */
	/* calculate the relative current time */
	length = thiz->end - thiz->start;
	rcurr = atime % length;
	rcurr += thiz->start;
	/* if the previous tick was outside the start-end
	 * means that we are going to repeat
	 */
	if (((rcurr - tpf) < thiz->start) && (s->started && !s->stopped))
	{
		DBG("Repeating signal %p", thiz);
		/* force it to pass through the last keyframe on the repeat */ 
		_egueb_smil_signal_continuous_animate(s, thiz->end);
		if (thiz->repeat_cb)
			thiz->repeat_cb(s, s->data);
		return;
	}

	if (!s->started && !s->stopped)
	{
		egueb_smil_signal_start(s);
	}

	DBG("Animating at %" EGUEB_SMIL_CLOCK_FORMAT " for [%" 
			EGUEB_SMIL_CLOCK_FORMAT " %" EGUEB_SMIL_CLOCK_FORMAT "]",
			EGUEB_SMIL_CLOCK_ARGS (rcurr),
			EGUEB_SMIL_CLOCK_ARGS (thiz->start),
			EGUEB_SMIL_CLOCK_ARGS (thiz->end));
	_egueb_smil_signal_continuous_animate(s, rcurr);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SMIL_SIGNAL_DESCRIPTOR,
		Egueb_Smil_Signal_Continuous, Egueb_Smil_Signal_Continuous_Class,
		egueb_smil_signal_continuous);

static void _egueb_smil_signal_continuous_class_init(void *k)
{
	Egueb_Smil_Signal_Class *klass;

	klass = EGUEB_SMIL_SIGNAL_CLASS(k);
	klass->process = _egueb_smil_signal_continuous_process;
}

static void _egueb_smil_signal_continuous_instance_init(void *o)
{
	Egueb_Smil_Signal_Continuous *thiz;

	thiz = EGUEB_SMIL_SIGNAL_CONTINUOUS(o);
	thiz->start = UINT64_MAX;
	thiz->repeat = 1;
}

static void _egueb_smil_signal_continuous_instance_deinit(void *o)
{
	Egueb_Smil_Signal_Continuous *thiz;
	Egueb_Smil_Keyframe *k;

	thiz = EGUEB_SMIL_SIGNAL_CONTINUOUS(o);
	/* remove every keyframe */
	EINA_LIST_FREE(thiz->keyframes, k)
	{
		if (k->value_free)
			k->value_free(k->value);
		free(k);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Smil_Signal * egueb_smil_signal_continuous_new(void *data)
{
	Egueb_Smil_Signal *s;

	s = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_signal_continuous);
	s->data = data;
	return s;
}

/**
 * Set the number of times the animation should repeat
 * @param a The Egueb_Smil_Signal
 * @param times Number of times, -1 for infinite
 */
EAPI void egueb_smil_signal_continuous_repeat_set(Egueb_Smil_Signal *s, int times)
{
	Egueb_Smil_Signal_Continuous *thiz;

	thiz = EGUEB_SMIL_SIGNAL_CONTINUOUS(s);
	thiz->repeat = times;
}


EAPI void egueb_smil_signal_continuous_keyframe_simple_add(Egueb_Smil_Signal *s,
		Egueb_Smil_Keyframe_Interpolator_Type type,
		Egueb_Smil_Clock clock,
		void *value, Etch_Free value_free)
{
	Egueb_Smil_Signal_Continuous *thiz;
	Egueb_Smil_Keyframe *k;

	k = calloc(1, sizeof(Egueb_Smil_Keyframe));
	k->type = type;
	k->time = clock;
	k->value = value;
	k->value_free = value_free;

	thiz = EGUEB_SMIL_SIGNAL_CONTINUOUS(s);
	_egueb_smil_signal_continuous_keyframes_add(thiz, k);
}

EAPI void egueb_smil_signal_continuous_keyframe_quadratic_add(Egueb_Smil_Signal *s,
		Egueb_Smil_Clock clock, double x0, double y0,
		void *value, Etch_Free value_free)
{
	Egueb_Smil_Signal_Continuous *thiz;
	Egueb_Smil_Keyframe *k;

	k = calloc(1, sizeof(Egueb_Smil_Keyframe));
	k->type = EGUEB_SMIL_KEYFRAME_INTERPOLATOR_QUADRATIC;
	k->time = clock;
	k->value = value;
	k->value_free = value_free;
	k->data.q.x0 = x0;
	k->data.q.y0 = y0;

	thiz = EGUEB_SMIL_SIGNAL_CONTINUOUS(s);
	_egueb_smil_signal_continuous_keyframes_add(thiz, k);
}
