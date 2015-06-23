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
#include "egueb_dom_event.h"
#include "egueb_dom_window.h"
#include "egueb_dom_event_ui.h"
#include "egueb_dom_event_window.h"

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_window.h"
#include "egueb_dom_event_ui_private.h"
#include "egueb_dom_event_window_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_WINDOW_DESCRIPTOR egueb_dom_event_window_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_window_descriptor_get(void);

#define EGUEB_DOM_EVENT_WINDOW_CLASS_GET(o) EGUEB_DOM_EVENT_WINDOW_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_WINDOW_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Event_Window_Class,					\
		EGUEB_DOM_EVENT_WINDOW_DESCRIPTOR)
#define EGUEB_DOM_EVENT_WINDOW(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_Window, EGUEB_DOM_EVENT_WINDOW_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_Window
{
	Egueb_Dom_Event_UI base;
} Egueb_Dom_Event_Window;

typedef struct _Egueb_Dom_Event_Window_Class
{
	Egueb_Dom_Event_UI_Class base;
} Egueb_Dom_Event_Window_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_WINDOW_RESIZE = EGUEB_DOM_STRING_STATIC("resize");
static Egueb_Dom_String _EGUEB_DOM_EVENT_WINDOW_SCROLL = EGUEB_DOM_STRING_STATIC("scroll");
static Egueb_Dom_String _EGUEB_DOM_EVENT_WINDOW_CLOSE = EGUEB_DOM_STRING_STATIC("close");

/*----------------------------------------------------------------------------*
 *                               Event interface                              *
 *----------------------------------------------------------------------------*/
static Ender_Item * _egueb_dom_event_window_item_get(Egueb_Dom_Event *thiz)
{
	const Ender_Lib *lib;

	lib = ender_lib_find("egueb_dom");
	return ender_lib_item_find(lib, "egueb.dom.event.window");
}

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_UI_DESCRIPTOR,
		Egueb_Dom_Event_Window, Egueb_Dom_Event_Window_Class,
		egueb_dom_event_window);

static void _egueb_dom_event_window_class_init(void *k)
{
	Egueb_Dom_Event_Class *klass;

	klass = EGUEB_DOM_EVENT_CLASS(k);
	klass->item_get = _egueb_dom_event_window_item_get;
}

static void _egueb_dom_event_window_instance_init(void *o)
{
}

static void _egueb_dom_event_window_instance_deinit(void *o)
{
}

static Egueb_Dom_Event * _egueb_dom_event_window_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_window);
	return event;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Event * egueb_dom_event_window_close_new(Egueb_Dom_Window *win)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_window_new();
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_WINDOW_CLOSE),
			EINA_TRUE, EINA_TRUE, win, 0);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_window_resize_new(Egueb_Dom_Window *win)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_window_new();
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_WINDOW_RESIZE),
			EINA_TRUE, EINA_TRUE, win, 0);
	return e;
}

Egueb_Dom_Event * egueb_dom_event_window_scroll_new(Egueb_Dom_Window *win)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_window_new();
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_WINDOW_SCROLL),
			EINA_TRUE, EINA_TRUE, win, 0);
	return e;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_WINDOW_RESIZE = &_EGUEB_DOM_EVENT_WINDOW_RESIZE;
Egueb_Dom_String *EGUEB_DOM_EVENT_WINDOW_CLOSE = &_EGUEB_DOM_EVENT_WINDOW_CLOSE;
Egueb_Dom_String *EGUEB_DOM_EVENT_WINDOW_SCROLL = &_EGUEB_DOM_EVENT_WINDOW_SCROLL;
