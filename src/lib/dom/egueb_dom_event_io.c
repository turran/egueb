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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_IO_DATA = &_EGUEB_DOM_EVENT_IO_DATA;
Egueb_Dom_String *EGUEB_DOM_EVENT_IO_IMAGE = &_EGUEB_DOM_EVENT_IO_IMAGE;

EAPI Egueb_Dom_Event * egueb_dom_event_io_data_new(Egueb_Dom_Uri *uri,
		Egueb_Dom_Event_IO_Data_Cb cb)
{
	Egueb_Dom_Event_IO *thiz;
	Egueb_Dom_Event *e;

	if (!uri) return NULL;

	e = _egueb_dom_event_io_new();
	egueb_dom_event_init(e, EGUEB_DOM_EVENT_IO_DATA, EINA_TRUE, EINA_TRUE,
			EINA_FALSE, EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE); 
	thiz = EGUEB_DOM_EVENT_IO(e);
	thiz->uri = *uri;
	if (thiz->uri.location)
		thiz->uri.location = egueb_dom_string_ref(thiz->uri.location);
	if (thiz->uri.fragment)
		thiz->uri.fragment = egueb_dom_string_ref(thiz->uri.fragment);
	thiz->data_cb = cb;
	return e;
}

EAPI Egueb_Dom_Event_IO_Data_Cb egueb_dom_event_io_data_cb_get(
		Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_IO *thiz;

	thiz = EGUEB_DOM_EVENT_IO(e);
	return thiz->data_cb;
}

EAPI Egueb_Dom_Event * egueb_dom_event_io_image_new(
		Enesim_Stream *stream, Egueb_Dom_Event_IO_Image_Cb cb)
{
	Egueb_Dom_Event_IO *thiz;
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_io_new();
	egueb_dom_event_init(e, EGUEB_DOM_EVENT_IO_IMAGE, EINA_TRUE, EINA_TRUE,
			EINA_FALSE, EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
	thiz = EGUEB_DOM_EVENT_IO(e);
	thiz->image_cb = cb;
	thiz->stream = enesim_stream_ref(stream);
	return e;
}

EAPI Egueb_Dom_Event_IO_Image_Cb egueb_dom_event_io_image_cb_get(
		Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_IO *thiz;

	thiz = EGUEB_DOM_EVENT_IO(e);
	return thiz->image_cb;
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
	uri->location = thiz->uri.location;
	if (uri->location)
		uri->location = egueb_dom_string_ref(uri->location);

	uri->fragment = thiz->uri.fragment;
	if (uri->fragment)
		uri->fragment = egueb_dom_string_ref(uri->fragment);
}
