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

#include "egueb_dom_string.h"
#include "egueb_dom_main.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_external.h"

#include "egueb_dom_event_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_EXTERNAL_DESCRIPTOR egueb_dom_event_external_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_external_descriptor_get(void);

#define EGUEB_DOM_EVENT_EXTERNAL_CLASS_GET(o) EGUEB_DOM_EVENT_EXTERNAL_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_EXTERNAL_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Event_External_Class,					\
		EGUEB_DOM_EVENT_EXTERNAL_DESCRIPTOR)
#define EGUEB_DOM_EVENT_EXTERNAL(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_External, EGUEB_DOM_EVENT_EXTERNAL_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_External
{
	Egueb_Dom_Event base;
	void *user_data;
	Egueb_Dom_Event_External_Free_Cb free_cb;
} Egueb_Dom_Event_External;

typedef struct _Egueb_Dom_Event_External_Class
{
	Egueb_Dom_Event_Class base;
} Egueb_Dom_Event_External_Class;
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Dom_Event_External, Egueb_Dom_Event_External_Class,
		egueb_dom_event_external);

static void _egueb_dom_event_external_class_init(void *k)
{
}

static void _egueb_dom_event_external_instance_init(void *o)
{
}

static void _egueb_dom_event_external_instance_deinit(void *o)
{
	Egueb_Dom_Event_External *thiz;

	thiz = EGUEB_DOM_EVENT_EXTERNAL(o);
	if (thiz->free_cb)
	{
		thiz->free_cb(thiz->user_data);
		thiz->user_data = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Event * egueb_dom_event_external_new(Egueb_Dom_String *type,
		Eina_Bool bubbleable, Eina_Bool capturable,
		Eina_Bool cancelable, Egueb_Dom_Event_Direction direction,
		void *user_data, Egueb_Dom_Event_External_Free_Cb free_cb)
{
	Egueb_Dom_Event_External *thiz;
	Egueb_Dom_Event *event;
	
	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_external);
	thiz->user_data = user_data;
	thiz->free_cb = free_cb;
	/* setup the event */
	event = EGUEB_DOM_EVENT(thiz);
	egueb_dom_event_init(event, type, bubbleable, capturable,
			cancelable, direction);
	
	return event;
}

EAPI void * egueb_dom_event_external_data_get(Egueb_Dom_Event *event)
{
	Egueb_Dom_Event_External *thiz;

	thiz = EGUEB_DOM_EVENT_EXTERNAL(event);
	return thiz->user_data;
}
