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
#include "egueb_dom_node.h"
#include "egueb_dom_uri.h"
#include "egueb_dom_feature.h"
#include "egueb_dom_feature_io.h"
#include "egueb_dom_event_io.h"
#include "egueb_dom_document.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_feature_private.h"

#include <libgen.h>
#include <stdio.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_FEATURE_IO_DESCRIPTOR egueb_dom_feature_io_descriptor_get()
#define EGUEB_DOM_FEATURE_IO(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Feature_IO, EGUEB_DOM_FEATURE_IO_DESCRIPTOR)

typedef struct _Egueb_Dom_Feature_IO
{
	Egueb_Dom_Feature base;
	Egueb_Dom_Node *n;
	Eina_Bool enabled;
} Egueb_Dom_Feature_IO;

typedef struct _Egueb_Dom_Feature_IO_Class
{
	Egueb_Dom_Feature_Class base;
} Egueb_Dom_Feature_IO_Class;

static Egueb_Dom_String _EGUEB_DOM_FEATURE_IO_NAME = EGUEB_DOM_STRING_STATIC("EguebDomIO");

static void _egueb_dom_feature_io_data_load(Egueb_Dom_String *location,
		Egueb_Dom_Event *ev)
{
	const char *filename;

	filename = egueb_dom_string_chars_get(location);
	/* check the scheme */
	if (!strncmp(filename, "file://", 7))
	{
		Enesim_Stream *s;

		s = enesim_stream_file_new(filename + 7, "r");
		if (s)
		{
			DBG("Data '%s' loaded correctly", filename);
			egueb_dom_event_io_data_finish(ev, s);
		}
		else
		{
			WARN("Failed opening file '%s'", filename);
		}
	}
	else
	{
		WARN("Unsupported schema '%s'", filename);
	}
}

static void _egueb_dom_feature_io_relative_data_cb(Egueb_Dom_Uri *uri,
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *node;
	Egueb_Dom_String *location;
	char *s_location;
	char *filename;
	char *dir;
	int ret;

	node = EGUEB_DOM_NODE(egueb_dom_event_target_get(ev));
	doc = egueb_dom_node_owner_document_get(node);
	if (!doc)
	{
		WARN("No document available");
		goto beach;
	}

	location = egueb_dom_document_uri_get(doc);
	egueb_dom_node_unref(doc);

	if (!location)
	{
		WARN("No URI set on the document");
		goto beach;
	}

	s_location =  strdup(egueb_dom_string_chars_get(location));
	egueb_dom_string_unref(location);

	dir = dirname(s_location);
	ret = asprintf(&filename, "%s/%s", dir,
			egueb_dom_string_chars_get(uri->location));

	free(s_location);
	if (ret < 0)
		goto beach;

	DBG("Loading relative data at '%s'", filename);
	location = egueb_dom_string_steal(filename);
	_egueb_dom_feature_io_data_load(location, ev);
	egueb_dom_string_unref(location);
beach:
	egueb_dom_node_unref(node);
}

static void _egueb_dom_feature_io_data_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Uri uri;

	egueb_dom_event_io_uri_get(ev, &uri);
	if (uri.fragment != NULL)
		goto has_fragment;

	DBG("Data requested '%s' (%d)", egueb_dom_string_chars_get(
			uri.location), uri.type);
	if (uri.type == EGUEB_DOM_URI_TYPE_ABSOLUTE)
	{
		_egueb_dom_feature_io_data_load(uri.location, ev);
	}
	else
	{
		_egueb_dom_feature_io_relative_data_cb(&uri, ev, data);
	}

has_fragment:
	egueb_dom_uri_cleanup(&uri);
}

static void _egueb_dom_feature_io_image_cb(Egueb_Dom_Event *ev, void *data)
{
	Enesim_Buffer *b = NULL;
	Enesim_Stream *s;
	Enesim_Surface *src = NULL;
	Eina_Error err;

	s = egueb_dom_event_io_stream_get(ev);
	if (!s) return;

	if (!enesim_image_load(s, NULL, &b, NULL, NULL, &err))
	{
		ERR("Can not load image, error: %s", eina_error_msg_get(err));
	}
	else
	{
		DBG("Image loaded correectly");
		src = enesim_surface_new_buffer_from(b);
	}
	egueb_dom_event_io_image_finish(ev, src);
	enesim_stream_unref(s);
}

static void _egueb_dom_feature_io_node_destroyed_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Dom_Feature_IO *thiz = data;

	if (thiz->enabled)
	{
		Egueb_Dom_Event_Target *evt;

		evt = EGUEB_DOM_EVENT_TARGET(thiz->n);
		egueb_dom_event_target_event_listener_remove(evt,
				EGUEB_DOM_EVENT_IO_DATA,
				_egueb_dom_feature_io_data_cb,
				EINA_TRUE, thiz);
		egueb_dom_event_target_event_listener_remove(evt,
				EGUEB_DOM_EVENT_IO_IMAGE,
				_egueb_dom_feature_io_image_cb,
				EINA_TRUE, thiz);
	}
	thiz->n = NULL;
}

/*----------------------------------------------------------------------------*
 *                              Feature interface                             *
 *----------------------------------------------------------------------------*/
static const char * _egueb_dom_feature_io_type_get(Egueb_Dom_Feature *f)
{
	return "egueb.dom.feature.io";
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_FEATURE_DESCRIPTOR,
		Egueb_Dom_Feature_IO, Egueb_Dom_Feature_IO_Class,
		egueb_dom_feature_io);

static void _egueb_dom_feature_io_class_init(void *k)
{
	Egueb_Dom_Feature_Class *klass;

	klass = EGUEB_DOM_FEATURE_CLASS(k);
	klass->type_get = _egueb_dom_feature_io_type_get;
}

static void _egueb_dom_feature_io_instance_init(void *o)
{
}

static void _egueb_dom_feature_io_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_FEATURE_IO_NAME = &_EGUEB_DOM_FEATURE_IO_NAME;

EAPI Eina_Bool egueb_dom_feature_io_add(Egueb_Dom_Node *n)
{
	Egueb_Dom_Feature_IO *thiz;

	if (!n) return EINA_FALSE;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_feature_io);
	thiz->n = n;
	egueb_dom_node_weak_ref(n, _egueb_dom_feature_io_node_destroyed_cb, thiz);

	return egueb_dom_node_feature_add(n, EGUEB_DOM_FEATURE_IO_NAME, NULL,
		EGUEB_DOM_FEATURE(thiz));
}

EAPI void egueb_dom_feature_io_default_enable(Egueb_Dom_Feature *f,
		Eina_Bool enable)
{
	Egueb_Dom_Feature_IO *thiz;
	Egueb_Dom_Event_Target *evt;

	thiz = EGUEB_DOM_FEATURE_IO(f);
	if (enable == thiz->enabled)
		return;
	evt = EGUEB_DOM_EVENT_TARGET(thiz->n);
	thiz->enabled = enable;
	if (enable)
	{
		egueb_dom_event_target_event_listener_add(evt,
				EGUEB_DOM_EVENT_IO_DATA,
				_egueb_dom_feature_io_data_cb, EINA_TRUE, thiz);
		egueb_dom_event_target_event_listener_add(evt,
				EGUEB_DOM_EVENT_IO_IMAGE,
				_egueb_dom_feature_io_image_cb, EINA_TRUE, thiz);
	}
	else
	{
		egueb_dom_event_target_event_listener_remove(evt,
				EGUEB_DOM_EVENT_IO_DATA,
				_egueb_dom_feature_io_data_cb,
				EINA_TRUE, thiz);
		egueb_dom_event_target_event_listener_remove(evt,
				EGUEB_DOM_EVENT_IO_IMAGE,
				_egueb_dom_feature_io_image_cb,
				EINA_TRUE, thiz);
	}
}
