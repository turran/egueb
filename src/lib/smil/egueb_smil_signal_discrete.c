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
#define EGUEB_SMIL_SIGNAL_DISCRETE_DESCRIPTOR egueb_dom_signal_discrete_descriptor_get()
#define EGUEB_SMIL_SIGNAL_DISCRETE_CLASS_GET(o) EGUEB_SMIL_SIGNAL_DISCRETE_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SMIL_SIGNAL_DISCRETE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Smil_Signal_Discrete_Class, EGUEB_SMIL_SIGNAL_DISCRETE_DESCRIPTOR)
#define EGUEB_SMIL_SIGNAL_DISCRETE(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Smil_Signal_Discrete, EGUEB_SMIL_SIGNAL_DISCRETE_DESCRIPTOR)

typedef struct _Egueb_Smil_Signal_Discrete
{
	Egueb_Smil_Signal parent;
	Egueb_Smil_Clock start;
	Egueb_Smil_Clock end;
	Eina_List *keyframes;
	int repeat; /** number of times the animation will repeat, -1 for infinite */

	Egueb_Smil_Signal_Process_Callback cb; /** function to call when a value has been set */
	Egueb_Smil_Signal_State_Callback repeat_cb;
} Egueb_Smil_Signal_Discrete;

typedef struct _Egueb_Smil_Signal_Discrete_Class
{
	Egueb_Smil_Signal_Class parent;
} Egueb_Smil_Signal_Discrete_Class;

/*----------------------------------------------------------------------------*
 *                              Signal interface                              *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_signal_discrete_process(Egueb_Smil_Signal *s,
		Egueb_Smil_Clock curr, unsigned int tpf)
{
	Egueb_Smil_Signal_Discrete *thiz;
	Egueb_Smil_Clock rcurr;
	Egueb_Smil_Clock atime; /* signal time */
	Egueb_Smil_Clock end;
	Egueb_Smil_Clock length;

	thiz = EGUEB_SMIL_SIGNAL_DISCRETE(s);
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
			//_egueb_smil_signal_discrete_animate(s, thiz->end);
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
		//_egueb_smil_signal_discrete_animate(s, thiz->end);
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
	//_egueb_smil_signal_discrete_animate(s, rcurr);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SMIL_SIGNAL_DESCRIPTOR,
		Egueb_Smil_Signal_Discrete, Egueb_Smil_Signal_Discrete_Class,
		egueb_smil_signal_discrete);

static void _egueb_smil_signal_discrete_class_init(void *k)
{
	Egueb_Smil_Signal_Class *klass;

	klass = EGUEB_SMIL_SIGNAL_CLASS(k);
	klass->process = _egueb_smil_signal_discrete_process;
}

static void _egueb_smil_signal_discrete_instance_init(void *o)
{
	Egueb_Smil_Signal_Discrete *thiz;

	thiz = EGUEB_SMIL_SIGNAL_DISCRETE(o);
	thiz->start = UINT64_MAX;
	thiz->repeat = 1;
}

static void _egueb_smil_signal_discrete_instance_deinit(void *o)
{
	Egueb_Smil_Signal_Discrete *thiz;
	Egueb_Smil_Keyframe *k;

	thiz = EGUEB_SMIL_SIGNAL_DISCRETE(o);
	/* remove the single keyframe */
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Smil_Signal * egueb_smil_signal_discrete_new(Egueb_Dom_Value *curr, void *data)
{
	Egueb_Smil_Signal *s;

	s = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_signal_discrete);
	s->data = data;
	s->curr = curr;
	return s;
}
