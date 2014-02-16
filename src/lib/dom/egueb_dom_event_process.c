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
#include "egueb_dom_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_event_process_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Dom_Event_Process
{
	Egueb_Dom_Event base;
} Egueb_Dom_Event_Process;

typedef struct _Egueb_Dom_Event_Process_Class
{
	Egueb_Dom_Event_Class base;
} Egueb_Dom_Event_Process_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_PROCESS = EGUEB_DOM_STRING_STATIC("Process");
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Dom_Event_Process, Egueb_Dom_Event_Process_Class,
		egueb_dom_event_process);

static void _egueb_dom_event_process_class_init(void *k)
{
}

static void _egueb_dom_event_process_instance_init(void *o)
{
}

static void _egueb_dom_event_process_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_PROCESS = &_EGUEB_DOM_EVENT_PROCESS;

Egueb_Dom_Event * egueb_dom_event_process_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_process);
	egueb_dom_event_init(event, EGUEB_DOM_EVENT_PROCESS, EINA_TRUE,
			EINA_FALSE, EINA_TRUE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
	return event;
}
