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

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_keyboard.h"
#include "egueb_dom_event_ui_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_KEYBOARD_DESCRIPTOR egueb_dom_event_keyboard_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_keyboard_descriptor_get(void);

#define EGUEB_DOM_EVENT_KEYBOARD_CLASS_GET(o) EGUEB_DOM_EVENT_KEYBOARD_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_KEYBOARD_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Event_Keyboard_Class,					\
		EGUEB_DOM_EVENT_KEYBOARD_DESCRIPTOR)
#define EGUEB_DOM_EVENT_KEYBOARD(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_Keyboard, EGUEB_DOM_EVENT_KEYBOARD_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_Keyboard
{
	Egueb_Dom_Event_UI base;
	Egueb_Dom_String *key;
	Egueb_Dom_Key_Location location;
	Eina_Bool alt_key;
	Eina_Bool ctrl_key;
	Eina_Bool shift_key;
	Eina_Bool meta_key;
} Egueb_Dom_Event_Keyboard;

typedef struct _Egueb_Dom_Event_Keyboard_Class
{
	Egueb_Dom_Event_UI_Class base;
} Egueb_Dom_Event_Keyboard_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_KEY_DOWN = EGUEB_DOM_STRING_STATIC("keydown");
static Egueb_Dom_String _EGUEB_DOM_EVENT_KEY_UP = EGUEB_DOM_STRING_STATIC("keyup");
/*----------------------------------------------------------------------------*
 *                               Event interface                              *
 *----------------------------------------------------------------------------*/
static Ender_Item * _egueb_dom_event_keyboard_item_get(Egueb_Dom_Event *thiz)
{
	const Ender_Lib *lib;

	lib = ender_lib_find("egueb_dom");
	return ender_lib_item_find(lib, "egueb.dom.event.keyboard");
}

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_UI_DESCRIPTOR,
		Egueb_Dom_Event_Keyboard, Egueb_Dom_Event_Keyboard_Class,
		egueb_dom_event_keyboard);

static void _egueb_dom_event_keyboard_class_init(void *k)
{
	Egueb_Dom_Event_Class *klass;

	klass = EGUEB_DOM_EVENT_CLASS(k);
	klass->item_get = _egueb_dom_event_keyboard_item_get;
}

static void _egueb_dom_event_keyboard_instance_init(void *o)
{
}

static void _egueb_dom_event_keyboard_instance_deinit(void *o)
{
	Egueb_Dom_Event_Keyboard *thiz;

	thiz = EGUEB_DOM_EVENT_KEYBOARD(o);
	if (thiz->key)
	{
		egueb_dom_string_unref(thiz->key);
		thiz->key = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Event * egueb_dom_event_key_down_new(Egueb_Dom_String *key,
		Egueb_Dom_Key_Location location,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key)
{
	Egueb_Dom_Event_Keyboard *thiz;
	Egueb_Dom_Event *e;

	e = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_keyboard);
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_KEY_DOWN),
			EINA_TRUE, EINA_TRUE, NULL, 0);

	thiz = EGUEB_DOM_EVENT_KEYBOARD(e);
	thiz->key = key;
	thiz->alt_key = alt_key;
	thiz->ctrl_key = ctrl_key;
	thiz->shift_key = shift_key;
	thiz->meta_key = meta_key;
	thiz->location = location;
	return e;
}

Egueb_Dom_Event * egueb_dom_event_key_up_new(Egueb_Dom_String *key,
		Egueb_Dom_Key_Location location,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key)
{
	Egueb_Dom_Event_Keyboard *thiz;
	Egueb_Dom_Event *e;

	e = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_keyboard);
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_KEY_DOWN),
			EINA_TRUE, EINA_TRUE, NULL, 0);

	thiz = EGUEB_DOM_EVENT_KEYBOARD(e);
	thiz->key = key;
	thiz->alt_key = alt_key;
	thiz->ctrl_key = ctrl_key;
	thiz->shift_key = shift_key;
	thiz->meta_key = meta_key;
	thiz->location = location;
	return e;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_KEY_DOWN = &_EGUEB_DOM_EVENT_KEY_DOWN;
Egueb_Dom_String *EGUEB_DOM_EVENT_KEY_UP = &_EGUEB_DOM_EVENT_KEY_UP;

EAPI Egueb_Dom_String * egueb_dom_event_keyboard_key_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Keyboard *thiz;

	thiz = EGUEB_DOM_EVENT_KEYBOARD(e);
	return egueb_dom_string_ref(thiz->key);
}
