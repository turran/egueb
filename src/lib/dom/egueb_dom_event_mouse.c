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

#include "egueb_dom_main.h"
#include "egueb_dom_node.h"
#include "egueb_dom_event.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_event_ui_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_MOUSE_DESCRIPTOR egueb_dom_event_mouse_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_mouse_descriptor_get(void);

#define EGUEB_DOM_EVENT_MOUSE_CLASS_GET(o) EGUEB_DOM_EVENT_MOUSE_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_MOUSE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Event_Mouse_Class,					\
		EGUEB_DOM_EVENT_MOUSE_DESCRIPTOR)
#define EGUEB_DOM_EVENT_MOUSE(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_Mouse, EGUEB_DOM_EVENT_MOUSE_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_Mouse
{
	Egueb_Dom_Event_UI base;
	Egueb_Dom_Node *related;
	int client_x;
	int client_y;
	int screen_x;
	int screen_y;
} Egueb_Dom_Event_Mouse;

/* altKey of type boolean, readonly

button of type unsigned short, readonly
ctrlKey of type boolean, readonly
metaKey of type boolean, readonly
relatedTarget of type EventTarget, readonly
shiftKey of type boolean, readonly
*/

typedef struct _Egueb_Dom_Event_Mouse_Class
{
	Egueb_Dom_Event_UI_Class base;
} Egueb_Dom_Event_Mouse_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_CLICK = EGUEB_DOM_STRING_STATIC("click");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_DOWN = EGUEB_DOM_STRING_STATIC("down");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_UP = EGUEB_DOM_STRING_STATIC("up");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_OVER = EGUEB_DOM_STRING_STATIC("over");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_MOVE = EGUEB_DOM_STRING_STATIC("move");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_OUT = EGUEB_DOM_STRING_STATIC("out");
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_UI_DESCRIPTOR,
		Egueb_Dom_Event_Mouse, Egueb_Dom_Event_Mouse_Class,
		egueb_dom_event_mouse);

static void _egueb_dom_event_mouse_class_init(void *k)
{
}

static void _egueb_dom_event_mouse_instance_init(void *o)
{
}

static void _egueb_dom_event_mouse_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Event * egueb_dom_event_mouse_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_mouse);
	return event;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_CLICK = &_EGUEB_DOM_EVENT_MOUSE_CLICK;
Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_DOWN = &_EGUEB_DOM_EVENT_MOUSE_DOWN;
Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_UP = &_EGUEB_DOM_EVENT_MOUSE_UP;
Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_OVER = &_EGUEB_DOM_EVENT_MOUSE_OVER;
Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_MOVE = &_EGUEB_DOM_EVENT_MOUSE_MOVE;
Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_OUT = &_EGUEB_DOM_EVENT_MOUSE_OUT;

EAPI Eina_Error egueb_dom_event_mouse_related_get(Egueb_Dom_Event *e, Egueb_Dom_Node **related)
{
	Egueb_Dom_Event_Mouse *thiz;

	if (!related) return EINA_ERROR_NONE;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	if (thiz->related) *related = egueb_dom_node_ref(thiz->related);
	else *related = NULL;

	return EINA_ERROR_NONE;
}

