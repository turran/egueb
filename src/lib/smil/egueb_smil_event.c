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
#include "egueb_smil_event.h"
#include "egueb_dom_event_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SMIL_EVENT_DESCRIPTOR egueb_smil_event_descriptor_get()
Enesim_Object_Descriptor * egueb_smil_event_descriptor_get(void);

#define EGUEB_SMIL_EVENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Smil_Event, EGUEB_SMIL_EVENT_DESCRIPTOR)

typedef struct _Egueb_Smil_Event
{
	Egueb_Dom_Event parent;
	int detail;
} Egueb_Smil_Event;

typedef struct _Egueb_Smil_Event_Class
{
	Egueb_Dom_Event_Class parent;
} Egueb_Smil_Event_Class;

static Egueb_Dom_String _EGUEB_SMIL_EVENT_BEGIN = EGUEB_DOM_STRING_STATIC("beginEvent");
static Egueb_Dom_String _EGUEB_SMIL_EVENT_END = EGUEB_DOM_STRING_STATIC("endEvent");
static Egueb_Dom_String _EGUEB_SMIL_EVENT_REPEAT = EGUEB_DOM_STRING_STATIC("repeatEvent");
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Smil_Event, Egueb_Smil_Event_Class,
		egueb_smil_event);

static void _egueb_smil_event_class_init(void *k)
{
}

static void _egueb_smil_event_instance_init(void *o)
{
}

static void _egueb_smil_event_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_SMIL_EVENT_BEGIN = &_EGUEB_SMIL_EVENT_BEGIN;
Egueb_Dom_String *EGUEB_SMIL_EVENT_END = &_EGUEB_SMIL_EVENT_END;
Egueb_Dom_String *EGUEB_SMIL_EVENT_REPEAT = &_EGUEB_SMIL_EVENT_REPEAT;

EAPI Egueb_Dom_Event * egueb_smil_event_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_event);
	return event;
}

EAPI void egueb_smil_event_init(Egueb_Dom_Event *e, Egueb_Dom_String *type, int detail)
{
	Egueb_Smil_Event *thiz;

	thiz = EGUEB_SMIL_EVENT(e);
	thiz->detail = detail;
	egueb_dom_event_init(e, type, EINA_FALSE, EINA_TRUE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
}

