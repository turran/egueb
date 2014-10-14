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
#include "egueb_smil_event_timeline.h"

#include "egueb_dom_event_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_smil_event_timeline_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SMIL_EVENT_TIMELINE_DESCRIPTOR egueb_smil_event_timeline_descriptor_get()
Enesim_Object_Descriptor * egueb_smil_event_timeline_descriptor_get(void);

#define EGUEB_SMIL_EVENT_TIMELINE(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Smil_Event_Timeline, EGUEB_SMIL_EVENT_TIMELINE_DESCRIPTOR)

typedef struct _Egueb_Smil_Event_Timeline
{
	Egueb_Dom_Event parent;
	Egueb_Smil_Timeline *timeline;
} Egueb_Smil_Event_Timeline;

typedef struct _Egueb_Smil_Event_Timeline_Class
{
	Egueb_Dom_Event_Class parent;
} Egueb_Smil_Event_Timeline_Class;

static Egueb_Dom_String _EGUEB_SMIL_EVENT_TIMELINE = EGUEB_DOM_STRING_STATIC("timelineEvent");
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Smil_Event_Timeline, Egueb_Smil_Event_Timeline_Class,
		egueb_smil_event_timeline);

static void _egueb_smil_event_timeline_class_init(void *k)
{
}

static void _egueb_smil_event_timeline_instance_init(void *o)
{
}

static void _egueb_smil_event_timeline_instance_deinit(void *o)
{
	Egueb_Smil_Event_Timeline *thiz;

	thiz = EGUEB_SMIL_EVENT_TIMELINE(o);
	egueb_smil_timeline_unref(thiz->timeline);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_String *EGUEB_SMIL_EVENT_TIMELINE = &_EGUEB_SMIL_EVENT_TIMELINE;

Egueb_Dom_Event * egueb_smil_event_timeline_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_event_timeline);
	egueb_dom_event_init(event, egueb_dom_string_ref(EGUEB_SMIL_EVENT_TIMELINE),
			EINA_FALSE, EINA_TRUE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
	return event;
}

void egueb_smil_event_timeline_set(Egueb_Dom_Event *e, Egueb_Smil_Timeline *timeline)
{
	Egueb_Smil_Event_Timeline *thiz;

	thiz = EGUEB_SMIL_EVENT_TIMELINE(e);
	thiz->timeline = timeline;
}

Egueb_Smil_Timeline * egueb_smil_event_timeline_get(Egueb_Dom_Event *e)
{
	Egueb_Smil_Event_Timeline *thiz;

	thiz = EGUEB_SMIL_EVENT_TIMELINE(e);
	return egueb_smil_timeline_ref(thiz->timeline);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_smil_event_is_timeline(Egueb_Dom_Event *e)
{
	if (!e) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(e),
			EGUEB_SMIL_EVENT_TIMELINE_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}
