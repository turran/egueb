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
#include "egueb_dom_event_ui.h"

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_focus.h"
#include "egueb_dom_event_ui_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_FOCUS_DESCRIPTOR egueb_dom_event_focus_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_focus_descriptor_get(void);

#define EGUEB_DOM_EVENT_FOCUS_CLASS_GET(o) EGUEB_DOM_EVENT_FOCUS_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_FOCUS_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Event_Focus_Class,					\
		EGUEB_DOM_EVENT_FOCUS_DESCRIPTOR)
#define EGUEB_DOM_EVENT_FOCUS(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_Focus, EGUEB_DOM_EVENT_FOCUS_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_Focus
{
	Egueb_Dom_Event_UI base;
} Egueb_Dom_Event_Focus;

typedef struct _Egueb_Dom_Event_Focus_Class
{
	Egueb_Dom_Event_UI_Class base;
} Egueb_Dom_Event_Focus_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_FOCUS_IN = EGUEB_DOM_STRING_STATIC("focusin");
static Egueb_Dom_String _EGUEB_DOM_EVENT_FOCUS_OUT = EGUEB_DOM_STRING_STATIC("focusout");
/*----------------------------------------------------------------------------*
 *                               Event interface                              *
 *----------------------------------------------------------------------------*/
static Ender_Item * _egueb_dom_event_focus_item_get(Egueb_Dom_Event *thiz)
{
	const Ender_Lib *lib;

	lib = ender_lib_find("egueb_dom");
	return ender_lib_item_find(lib, "egueb.dom.event.focus");
}

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_UI_DESCRIPTOR,
		Egueb_Dom_Event_Focus, Egueb_Dom_Event_Focus_Class,
		egueb_dom_event_focus);

static void _egueb_dom_event_focus_class_init(void *k)
{
	Egueb_Dom_Event_Class *klass;

	klass = EGUEB_DOM_EVENT_CLASS(k);
	klass->item_get = _egueb_dom_event_focus_item_get;
}

static void _egueb_dom_event_focus_instance_init(void *o)
{
}

static void _egueb_dom_event_focus_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_FOCUS_IN = &_EGUEB_DOM_EVENT_FOCUS_IN;
Egueb_Dom_String *EGUEB_DOM_EVENT_FOCUS_OUT = &_EGUEB_DOM_EVENT_FOCUS_OUT;

EAPI Egueb_Dom_Event * egueb_dom_event_focus_in_new(void)
{
	Egueb_Dom_Event_Focus *thiz;
	Egueb_Dom_Event *e;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_focus);
	e = EGUEB_DOM_EVENT(thiz);
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_FOCUS_IN),
			EINA_TRUE, EINA_FALSE, 0);
	return e;
}

EAPI Egueb_Dom_Event * egueb_dom_event_focus_out_new(void)
{
	Egueb_Dom_Event_Focus *thiz;
	Egueb_Dom_Event *e;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_focus);
	e = EGUEB_DOM_EVENT(thiz);
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_FOCUS_OUT),
			EINA_TRUE, EINA_FALSE, 0);
	return e;
}
