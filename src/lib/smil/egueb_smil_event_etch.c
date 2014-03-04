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
#include "egueb_smil_event.h"
#include "egueb_dom_event_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SMIL_EVENT_ETCH_DESCRIPTOR egueb_smil_event_etch_descriptor_get()
Enesim_Object_Descriptor * egueb_smil_event_etch_descriptor_get(void);

#define EGUEB_SMIL_EVENT_ETCH(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Smil_Event_Etch, EGUEB_SMIL_EVENT_ETCH_DESCRIPTOR)

typedef struct _Egueb_Smil_Event_Etch
{
	Egueb_Dom_Event parent;
	Etch *etch;
} Egueb_Smil_Event_Etch;

typedef struct _Egueb_Smil_Event_Etch_Class
{
	Egueb_Dom_Event_Class parent;
} Egueb_Smil_Event_Etch_Class;

static Egueb_Dom_String _EGUEB_SMIL_EVENT_ETCH = EGUEB_DOM_STRING_STATIC("etchEvent");
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Smil_Event_Etch, Egueb_Smil_Event_Etch_Class,
		egueb_smil_event_etch);

static void _egueb_smil_event_etch_class_init(void *k)
{
}

static void _egueb_smil_event_etch_instance_init(void *o)
{
}

static void _egueb_smil_event_etch_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_SMIL_EVENT_ETCH = &_EGUEB_SMIL_EVENT_ETCH;

EAPI Egueb_Dom_Event * egueb_smil_event_etch_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_smil_event_etch);
	egueb_dom_event_init(event, EGUEB_SMIL_EVENT_ETCH, EINA_FALSE, EINA_TRUE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);

}

EAPI void egueb_smil_event_etch_set(Egueb_Dom_Event *e, Etch *etch)
{
	Egueb_Smil_Event_Etch *thiz;

	thiz = EGUEB_SMIL_EVENT_ETCH(e);
	thiz->etch = etch;
}

EAPI Etch * egueb_smil_event_etch_get(Egueb_Dom_Event *e)
{
	Egueb_Smil_Event_Etch *thiz;

	thiz = EGUEB_SMIL_EVENT_ETCH(e);
	return thiz->etch;
}
