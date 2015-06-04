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
#include "egueb_dom_event.h"
#include "egueb_dom_event_io.h"

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_IO_DESCRIPTOR egueb_dom_event_io_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_io_descriptor_get(void);
#define EGUEB_DOM_EVENT_IO(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_IO, EGUEB_DOM_EVENT_IO_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_IO
{
	Egueb_Dom_Event base;
	Egueb_Dom_Event_IO_Data_Cb data_cb;
	Egueb_Dom_Event_IO_Image_Cb image_cb;
	Enesim_Stream *stream;
	Egueb_Dom_Uri uri;
} Egueb_Dom_Event_IO;

typedef struct _Egueb_Dom_Event_IO_Class
{
	Egueb_Dom_Event_Class base;
} Egueb_Dom_Event_IO_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_IO_DONE = EGUEB_DOM_STRING_STATIC("IODone");
static Egueb_Dom_String _EGUEB_DOM_EVENT_IO_DATA = EGUEB_DOM_STRING_STATIC("IOData");
static Egueb_Dom_String _EGUEB_DOM_EVENT_IO_IMAGE = EGUEB_DOM_STRING_STATIC("IOImage");


/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Dom_Event_IO, Egueb_Dom_Event_IO_Class,
		egueb_dom_event_io);

static void _egueb_dom_event_io_class_init(void *k)
{
}

static void _egueb_dom_event_io_instance_init(void *o)
{
}

static void _egueb_dom_event_io_instance_deinit(void *o)
{
	Egueb_Dom_Event_IO *thiz = o;

	if (thiz->stream)
		enesim_stream_unref(thiz->stream);
	egueb_dom_uri_cleanup(&thiz->uri);
}

static Egueb_Dom_Event * _egueb_dom_event_io_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_io);
	return event;
}

static Egueb_Dom_Event * egueb_dom_event_io_done_new(Egueb_Dom_Event_IO *thiz)
{
	Egueb_Dom_Event *e;
	Egueb_Dom_Event_IO *ret;

	e = _egueb_dom_event_io_new();
	egueb_dom_event_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_IO_DONE),
			EINA_TRUE, EINA_TRUE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
	ret = EGUEB_DOM_EVENT_IO(e);
	/* copy the uri */
	ret->uri = thiz->uri;
	if (thiz->uri.location)
		ret->uri.location = egueb_dom_string_ref(thiz->uri.location);
	if (thiz->uri.fragment)
		ret->uri.fragment = egueb_dom_string_ref(thiz->uri.fragment);
	/* copy the stream */
	if (thiz->stream)
		ret->stream = enesim_stream_ref(thiz->stream);
	return e;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_IO_DATA = &_EGUEB_DOM_EVENT_IO_DATA;
Egueb_Dom_String *EGUEB_DOM_EVENT_IO_IMAGE = &_EGUEB_DOM_EVENT_IO_IMAGE;
Egueb_Dom_String *EGUEB_DOM_EVENT_IO_DONE = &_EGUEB_DOM_EVENT_IO_DONE;

EAPI Egueb_Dom_Event * egueb_dom_event_io_data_new(Egueb_Dom_Uri *uri,
		Egueb_Dom_Event_IO_Data_Cb cb)
{
	Egueb_Dom_Event_IO *thiz;
	Egueb_Dom_Event *e;

	if (!uri) return NULL;

	e = _egueb_dom_event_io_new();
	egueb_dom_event_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_IO_DATA),
			EINA_TRUE, EINA_TRUE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
	thiz = EGUEB_DOM_EVENT_IO(e);
	thiz->uri = *uri;
	if (thiz->uri.location)
		thiz->uri.location = egueb_dom_string_ref(thiz->uri.location);
	if (thiz->uri.fragment)
		thiz->uri.fragment = egueb_dom_string_ref(thiz->uri.fragment);
	thiz->data_cb = cb;
	return e;
}

EAPI Egueb_Dom_Event * egueb_dom_event_io_image_new(
		Enesim_Stream *stream, Egueb_Dom_Event_IO_Image_Cb cb)
{
	Egueb_Dom_Event_IO *thiz;
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_io_new();
	egueb_dom_event_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_IO_IMAGE),
			EINA_TRUE, EINA_TRUE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
	thiz = EGUEB_DOM_EVENT_IO(e);
	thiz->image_cb = cb;
	thiz->stream = enesim_stream_ref(stream);
	return e;
}

EAPI Enesim_Stream * egueb_dom_event_io_stream_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_IO *thiz;

	thiz = EGUEB_DOM_EVENT_IO(e);
	return enesim_stream_ref(thiz->stream);
}

EAPI void egueb_dom_event_io_uri_get(Egueb_Dom_Event *e, Egueb_Dom_Uri *uri)
{
	Egueb_Dom_Event_IO *thiz;

	thiz = EGUEB_DOM_EVENT_IO(e);
	egueb_dom_uri_copy(&thiz->uri, uri, EINA_FALSE);
}

/* TODO remove the previous getters and just let the app call this */
EAPI void egueb_dom_event_io_image_finish(Egueb_Dom_Event *e, Enesim_Surface *s)
{
	Egueb_Dom_Event_IO *thiz;
	Egueb_Dom_Event *done;
	Egueb_Dom_Event_Target *evt;

	if (!e) return;

	evt = egueb_dom_event_target_get(e);

	thiz = EGUEB_DOM_EVENT_IO(e);
	/* call the callback */
	if (thiz->image_cb)
	{
		thiz->image_cb(EGUEB_DOM_NODE(evt), s);
	}
	/* send the event */
	done = egueb_dom_event_io_done_new(thiz);
	egueb_dom_event_target_event_dispatch(evt, done, NULL, NULL);
	egueb_dom_event_target_unref(evt);
}

EAPI void egueb_dom_event_io_data_finish(Egueb_Dom_Event *e, Enesim_Stream *s)
{
	Egueb_Dom_Event_IO *thiz;
	Egueb_Dom_Event *done;
	Egueb_Dom_Event_Target *evt;

	if (!e) return;

	evt = egueb_dom_event_target_get(e);

	thiz = EGUEB_DOM_EVENT_IO(e);
	/* call the callback */
	if (thiz->data_cb)
	{
		thiz->data_cb(EGUEB_DOM_NODE(evt), s);
	}
	/* send the event */
	done = egueb_dom_event_io_done_new(thiz);
	egueb_dom_event_target_event_dispatch(evt, done, NULL, NULL);
	egueb_dom_event_target_unref(evt);
}
