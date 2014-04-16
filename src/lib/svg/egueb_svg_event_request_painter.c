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
#include "egueb_svg_main_private.h"
#include "egueb_svg_main.h"
#include "egueb_svg_painter.h"

#include "egueb_dom_event_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_svg_event_request_painter_private.h"
#include "egueb_svg_painter_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_EVENT_REQUEST_PAINTER_DESCRIPTOR egueb_svg_event_request_painter_descriptor_get()
Enesim_Object_Descriptor * egueb_svg_event_request_painter_descriptor_get(void);

#define EGUEB_SVG_EVENT_REQUEST_PAINTER(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Svg_Event_Request_Painter, EGUEB_SVG_EVENT_REQUEST_PAINTER_DESCRIPTOR)

typedef struct _Egueb_Svg_Event_Request_Painter
{
	Egueb_Dom_Event parent;
	Egueb_Svg_Painter *painter;
} Egueb_Svg_Event_Request_Painter;

typedef struct _Egueb_Svg_Event_Request_Painter_Class
{
	Egueb_Dom_Event_Class parent;
} Egueb_Svg_Event_Request_Painter_Class;

static Egueb_Dom_String _EGUEB_SVG_EVENT_REQUEST_PAINTER = EGUEB_DOM_STRING_STATIC("painterEvent");
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Svg_Event_Request_Painter,
		Egueb_Svg_Event_Request_Painter_Class,
		egueb_svg_event_request_painter);

static void _egueb_svg_event_request_painter_class_init(void *k)
{
}

static void _egueb_svg_event_request_painter_instance_init(void *o)
{
}

static void _egueb_svg_event_request_painter_instance_deinit(void *o)
{
	Egueb_Svg_Event_Request_Painter *thiz;

	thiz = EGUEB_SVG_EVENT_REQUEST_PAINTER(o);
	if (thiz->painter)
	{
		egueb_svg_painter_unref(thiz->painter);
		thiz->painter = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_String *EGUEB_SVG_EVENT_REQUEST_PAINTER = &_EGUEB_SVG_EVENT_REQUEST_PAINTER;

Egueb_Dom_Event * egueb_svg_event_request_painter_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_event_request_painter);
	egueb_dom_event_init(event, egueb_dom_string_ref(EGUEB_SVG_EVENT_REQUEST_PAINTER),
			EINA_TRUE, EINA_FALSE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
	return event;
}

void egueb_svg_event_request_painter_painter_set(Egueb_Dom_Event *e,
		Egueb_Svg_Painter *painter)
{
	Egueb_Svg_Event_Request_Painter *thiz;

	thiz = EGUEB_SVG_EVENT_REQUEST_PAINTER(e);
	if (thiz->painter)
		egueb_svg_painter_unref(thiz->painter);
	thiz->painter = painter;
}

Egueb_Svg_Painter * egueb_svg_event_request_painter_painter_get(Egueb_Dom_Event *e)
{
	Egueb_Svg_Event_Request_Painter *thiz;

	thiz = EGUEB_SVG_EVENT_REQUEST_PAINTER(e);
	if (thiz->painter)
		return egueb_svg_painter_ref(thiz->painter);
	return NULL;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

