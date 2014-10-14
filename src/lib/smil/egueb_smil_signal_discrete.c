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

#include "egueb_smil_keyframe_private.h"
#include "egueb_smil_timeline_private.h"
#include "egueb_smil_signal_private.h"
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
	Egueb_Dom_Value *value;

	Egueb_Smil_Signal_Discrete_Process process_cb; /** function to call when a value has been set */
} Egueb_Smil_Signal_Discrete;

typedef struct _Egueb_Smil_Signal_Discrete_Class
{
	Egueb_Smil_Signal_Class parent;
} Egueb_Smil_Signal_Discrete_Class;

/*----------------------------------------------------------------------------*
 *                              Signal interface                              *
 *----------------------------------------------------------------------------*/
static void _egueb_smil_signal_discrete_process(Egueb_Smil_Signal *s,
		Egueb_Smil_Clock curr, Egueb_Smil_Clock tpf)
{
	Egueb_Smil_Signal_Discrete *thiz;

	thiz = EGUEB_SMIL_SIGNAL_DISCRETE(s);
	if (curr < thiz->start + s->offset)
		return;
	DBG("[(%" EGUEB_SMIL_CLOCK_FORMAT ")"
			" %" EGUEB_SMIL_CLOCK_FORMAT,
			EGUEB_SMIL_CLOCK_ARGS (s->offset),
			EGUEB_SMIL_CLOCK_ARGS (thiz->start));
	/* FIXME we add +1 because of the rounding error */
	if (curr > thiz->start + s->offset + tpf + 1)
	{
		if (!s->started)
		{
			WARN("Signal did not had time to start at %" \
					EGUEB_SMIL_CLOCK_FORMAT " %" \
					EGUEB_SMIL_CLOCK_FORMAT,
					EGUEB_SMIL_CLOCK_ARGS(curr),
					EGUEB_SMIL_CLOCK_ARGS(thiz->start + s->offset + tpf));
		}
		return;
	}
	egueb_smil_signal_start(s);
	thiz->process_cb(thiz->value, s->data);
	egueb_smil_signal_stop(s);
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
}

static void _egueb_smil_signal_discrete_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
EAPI Egueb_Smil_Signal * egueb_smil_signal_discrete_new(
		Egueb_Smil_Signal_Discrete_Process process_cb,
		Egueb_Smil_Signal_State_Callback start_cb,
		Egueb_Smil_Signal_State_Callback stop_cb,
		Egueb_Dom_Value *value,
		void *data)
{
	Egueb_Smil_Signal *s;
	Egueb_Smil_Signal_Discrete *thiz;

	if (!process_cb)
		return NULL;

	s = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_signal_discrete);
	s->data = data;
	s->start_cb = start_cb;
	s->stop_cb = stop_cb;

	thiz = EGUEB_SMIL_SIGNAL_DISCRETE(s);
	thiz->process_cb = process_cb;
	thiz->value = value;

	return s;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
