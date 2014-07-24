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

#include "egueb_smil_clock.h"
#include "egueb_smil_keyframe.h"
#include "egueb_smil_timeline.h"
#include "egueb_smil_signal.h"
#include "egueb_smil_signal_private.h"

#include "egueb_smil_timeline_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define DEFAULT_FPS 30

struct _Egueb_Smil_Timeline
{
	Eina_List *signals; /** List of signals */
	unsigned long frame; /** Current frame */
	unsigned int fps; /** Number of frames per second */
	Etch_Time tpf; /** Time per frame */
	Etch_Time curr; /** Current time in seconds */
	int ref;
};

static void _process(Egueb_Smil_Timeline *thiz)
{
	Egueb_Smil_Signal *a;
	Eina_List *l;

	/* iterate over the list of signals */
	EINA_LIST_FOREACH(thiz->signals, l, a)
	{
		egueb_smil_signal_process(a, thiz->curr, thiz->tpf);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/**
 * Advance the global time by one unit of seconds per frame
 * @param thiz The Egueb_Smil_Timeline instance
 */
void egueb_smil_timeline_tick(Egueb_Smil_Timeline *thiz)
{
	if (!thiz) return;
	/* TODO check for overflow */
	thiz->frame++;
	thiz->curr += thiz->tpf;
	_process(thiz);
}

/**
 * Sets the frames per second
 * @param thiz The Egueb_Smil_Timeline instance
 * @param fps Frames per second
 */
void egueb_smil_timeline_fps_set(Egueb_Smil_Timeline *thiz, unsigned int fps)
{
	double spf;

	thiz->fps = fps;
	spf = (double)1.0/fps;
	thiz->tpf = spf * EGUEB_SMIL_CLOCK_SECONDS;
}

/**
 * Sets the frames per second
 * @param thiz The Egueb_Smil_Timeline instance
 * @return Number of frames per second
 */
unsigned int egueb_smil_timeline_fps_get(Egueb_Smil_Timeline *thiz)
{
	if (!thiz) return 0;
	return thiz->fps;
}

unsigned int egueb_smil_timeline_tpf_get(Egueb_Smil_Timeline *thiz)
{
	return thiz->tpf;
}

Egueb_Smil_Clock egueb_smil_timeline_current_clock_get(Egueb_Smil_Timeline *thiz)
{
	return thiz->curr;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Create a new timeline
 * @return The newly created timeline
 */
EAPI Egueb_Smil_Timeline * egueb_smil_timeline_new(void)
{
	Egueb_Smil_Timeline *thiz;

	thiz = calloc(1, sizeof(Egueb_Smil_Timeline));
	thiz->ref = 1;
	egueb_smil_timeline_fps_set(thiz, DEFAULT_FPS);
	return thiz;
}

EAPI Egueb_Smil_Timeline * egueb_smil_timeline_ref(Egueb_Smil_Timeline *thiz)
{
	if (!thiz) return thiz;
	thiz->ref++;
	return thiz;
}

EAPI void egueb_smil_timeline_unref(Egueb_Smil_Timeline *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
		Egueb_Smil_Signal *s;

		EINA_LIST_FREE(thiz->signals, s)
		{
			s->timeline = NULL;
			egueb_smil_signal_unref(s);
		}
		free(thiz);
	}
}

EAPI void egueb_smil_timeline_signal_add(Egueb_Smil_Timeline *thiz, Egueb_Smil_Signal *s)
{
	if (!s) return;
	if (s->timeline)
	{
		WARN("The signal already has a timeline");
		egueb_smil_signal_unref(s);
		return;
	}
	thiz->signals = eina_list_append(thiz->signals, s);
	s->timeline = thiz;
}

/**
 * Remove the signal from the Egueb_Smil_Timeline instance
 * @param thiz The Egueb_Smil_Timeline instance to remove the signal from
 * @param a The signal to remove
 */
EAPI void egueb_smil_timeline_signal_remove(Egueb_Smil_Timeline *thiz, Egueb_Smil_Signal *s)
{
	if (!thiz) return;
	if (!s) return;
	if (s->timeline != thiz)
	{
		egueb_smil_signal_unref(s);
		return;
	}
	
	thiz->signals = eina_list_remove(thiz->signals, s);

	s->timeline = NULL;
	egueb_smil_signal_unref(s);
}
