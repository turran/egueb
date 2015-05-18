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

#include "egueb_smil_keyframe_private.h"
#include "egueb_smil_timeline_private.h"
#include "egueb_smil_signal_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define DEFAULT_FPS 30

struct _Egueb_Smil_Timeline
{
	Eina_List *signals; /** List of signals */
	unsigned long frame; /** Current frame */
	unsigned int fps; /** Number of frames per second */
	Egueb_Smil_Clock tpf; /** Time per frame */
	Egueb_Smil_Clock curr; /** Current time in seconds */
	Eina_Bool processing;
	int ref;
};

static void _process(Egueb_Smil_Timeline *thiz)
{
	Egueb_Smil_Signal *a;
	Eina_List *l;
	Eina_List *l_next;

	thiz->processing = EINA_TRUE;
	/* iterate over the list of signals */
	DBG("[%" EGUEB_SMIL_CLOCK_FORMAT "] %" EGUEB_SMIL_CLOCK_FORMAT,
			EGUEB_SMIL_CLOCK_ARGS (thiz->curr),
			EGUEB_SMIL_CLOCK_ARGS (thiz->tpf));
	EINA_LIST_FOREACH_SAFE(thiz->signals, l, l_next, a)
	{
		if (!a->remove_me)
			egueb_smil_signal_process(a, thiz->curr, thiz->tpf);
		else
		{
			thiz->signals = eina_list_remove_list(thiz->signals, l);
			a->timeline = NULL;
			egueb_smil_signal_unref(a);
		}
	}
	thiz->processing = EINA_FALSE;
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

Egueb_Smil_Timeline * egueb_smil_timeline_new(void)
{
	Egueb_Smil_Timeline *thiz;

	thiz = calloc(1, sizeof(Egueb_Smil_Timeline));
	thiz->ref = 1;
	egueb_smil_timeline_fps_set(thiz, DEFAULT_FPS);
	return thiz;
}

Egueb_Smil_Timeline * egueb_smil_timeline_ref(Egueb_Smil_Timeline *thiz)
{
	if (!thiz) return thiz;
	thiz->ref++;
	return thiz;
}

void egueb_smil_timeline_unref(Egueb_Smil_Timeline *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
		Egueb_Smil_Signal *s;

		ERR("Destroting the timeline");
		EINA_LIST_FREE(thiz->signals, s)
		{
			ERR("taking away the timeline");
			s->timeline = NULL;
			egueb_smil_signal_unref(s);
		}
		free(thiz);
	}
}

void egueb_smil_timeline_signal_add(Egueb_Smil_Timeline *thiz, Egueb_Smil_Signal *s)
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

void egueb_smil_timeline_signal_remove(Egueb_Smil_Timeline *thiz, Egueb_Smil_Signal *s)
{
	if (!s)
		return;
	if (!thiz)
		goto done;

	if (s->timeline != thiz)
	{
		WARN("The signal does not belong to this timeline (%p != %p)",
				s->timeline, thiz);
		goto done;
	}

	if (thiz->processing)
	{
		INFO("The timeline is processing, cant remove now");
		s->remove_me = EINA_TRUE;
		goto done;
	}

	thiz->signals = eina_list_remove(thiz->signals, s);
	s->timeline = NULL;
done:
	egueb_smil_signal_unref(s);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

