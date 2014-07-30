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
 * You should have received thiz copy of the GNU Lesser General Public
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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(ENESIM_OBJECT_DESCRIPTOR, Egueb_Smil_Signal,
		Egueb_Smil_Signal_Class, egueb_smil_signal);

static void _egueb_smil_signal_class_init(void *k)
{
}

static void _egueb_smil_signal_instance_init(void *o)
{
	Egueb_Smil_Signal *thiz;

	thiz = EGUEB_SMIL_SIGNAL(o);
	thiz->ref = 1;
}

static void _egueb_smil_signal_instance_deinit(void *o)
{
	Egueb_Smil_Signal *thiz;

	thiz = EGUEB_SMIL_SIGNAL(o);
	if (thiz->timeline)
	{
		CRIT("Still has a timeline parent, use egueb_smil_timeline_signal_remove()");
		return;
	}
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_smil_signal_process(Egueb_Smil_Signal *thiz, Egueb_Smil_Clock curr, unsigned int tpf)
{
	Egueb_Smil_Signal_Class *klass;

	klass = EGUEB_SMIL_SIGNAL_CLASS_GET(thiz);
	if (!thiz->enabled)
		return;
	if (klass->process)
		klass->process(thiz, curr, tpf);
}

void egueb_smil_signal_start(Egueb_Smil_Signal *thiz)
{
	DBG("Starting signal");
	if (thiz->start_cb)
		thiz->start_cb(thiz, thiz->data);
	thiz->started = EINA_TRUE;
}

void egueb_smil_signal_stop(Egueb_Smil_Signal *thiz)
{
	DBG("Stopping signal");
	thiz->stopped = EINA_TRUE;
	if (thiz->stop_cb)
		thiz->stop_cb(thiz, thiz->data);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Smil_Signal * egueb_smil_signal_ref(Egueb_Smil_Signal *thiz)
{
	if (!thiz) return thiz;
	thiz->ref++;
	return thiz;
}

EAPI void egueb_smil_signal_unref(Egueb_Smil_Signal *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
		enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
	}
}

/**
 * Disable a signal
 * @param thiz The Egueb_Smil_Signal
 */
EAPI void egueb_smil_signal_disable(Egueb_Smil_Signal *thiz)
{
	thiz->enabled = EINA_FALSE;
}

/**
 * Enable a signal
 * @param thiz The Egueb_Smil_Signal
 */
EAPI void egueb_smil_signal_enable(Egueb_Smil_Signal *thiz)
{
	thiz->enabled = EINA_TRUE;
	if (thiz->timeline)
	{
		Egueb_Smil_Clock curr;
		unsigned int tpf;

		tpf = egueb_smil_timeline_tpf_get(thiz->timeline);
		curr = egueb_smil_timeline_current_clock_get(thiz->timeline);
		egueb_smil_signal_process(thiz, curr, tpf);
	}
}

/**
 * Query whenever a signal is atually enabled
 * @param thiz The Egueb_Smil_Signal
 * @return EINA_TRUE or EINA_FALSE
 */
EAPI Eina_Bool egueb_smil_signal_enabled(Egueb_Smil_Signal *thiz)
{
	return thiz->enabled;
}

/**
 * Set an offset to a signal. That will increment every animation's keyframe time
 * @param thiz The Egueb_Smil_Signal
 * @param secs The number of seconds to increment
 * @param usecs The number of microsends to increment
 */
EAPI void egueb_smil_signal_offset_set(Egueb_Smil_Signal *thiz, Egueb_Smil_Clock inc)
{
	thiz->offset = inc;
}

/**
 * Get the timeline this signal belongs to
 * @param thiz The Egueb_Smil_Signal
 * @return The timeline @ender_transfer{full}
 */
EAPI Egueb_Smil_Timeline * egueb_smil_signal_timeline_get(Egueb_Smil_Signal *thiz)
{
	return egueb_smil_timeline_ref(thiz->timeline);
}
