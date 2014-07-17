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
#include "egueb_dom_uri.h"
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"
#include "egueb_dom_video_provider.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_multimedia.h"

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_MULTIMEDIA_DESCRIPTOR egueb_dom_event_multimedia_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_multimedia_descriptor_get(void);
#define EGUEB_DOM_EVENT_MULTIMEDIA(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_Multimedia, EGUEB_DOM_EVENT_MULTIMEDIA_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_Multimedia
{
	Egueb_Dom_Event base;
	const Egueb_Dom_Video_Provider_Notifier *notifier;
	Enesim_Renderer *image;
	Egueb_Dom_Video_Provider *video_provider;
} Egueb_Dom_Event_Multimedia;

typedef struct _Egueb_Dom_Event_Multimedia_Class
{
	Egueb_Dom_Event_Class base;
} Egueb_Dom_Event_Multimedia_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_MULTIMEDIA_VIDEO = EGUEB_DOM_STRING_STATIC("MultimediaVideo");

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Dom_Event_Multimedia, Egueb_Dom_Event_Multimedia_Class,
		egueb_dom_event_multimedia);

static void _egueb_dom_event_multimedia_class_init(void *k)
{
}

static void _egueb_dom_event_multimedia_instance_init(void *o)
{
}

static void _egueb_dom_event_multimedia_instance_deinit(void *o)
{
	Egueb_Dom_Event_Multimedia *thiz;

	thiz = EGUEB_DOM_EVENT_MULTIMEDIA(o);
	enesim_renderer_unref(thiz->image);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_MULTIMEDIA_VIDEO = &_EGUEB_DOM_EVENT_MULTIMEDIA_VIDEO;

EAPI Egueb_Dom_Event * egueb_dom_event_multimedia_video_new(
		const Egueb_Dom_Video_Provider_Notifier *notifier,
		Enesim_Renderer *image)
{
	Egueb_Dom_Event_Multimedia *thiz;
	Egueb_Dom_Event *e;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_multimedia);
	thiz->notifier = notifier;
	thiz->image = image;

	e = EGUEB_DOM_EVENT(thiz);
	egueb_dom_event_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_MULTIMEDIA_VIDEO),
			EINA_TRUE, EINA_TRUE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE); 
	return e;
}

EAPI Egueb_Dom_Video_Provider * egueb_dom_event_multimedia_video_provider_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Multimedia *thiz;

	thiz = EGUEB_DOM_EVENT_MULTIMEDIA(e);
	return thiz->video_provider;
}

EAPI void egueb_dom_event_multimedia_video_provider_set(Egueb_Dom_Event *e, Egueb_Dom_Video_Provider *sc)
{
	Egueb_Dom_Event_Multimedia *thiz;

	thiz = EGUEB_DOM_EVENT_MULTIMEDIA(e);
	thiz->video_provider = sc;
}
