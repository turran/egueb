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
#include "egueb_dom_event_mouse.h"
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
	Eina_Bool alt_key;
	Eina_Bool ctrl_key;
	Eina_Bool shift_key;
	Eina_Bool meta_key;
	int button;
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
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_DOWN = EGUEB_DOM_STRING_STATIC("mousedown");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_UP = EGUEB_DOM_STRING_STATIC("mouseup");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_OVER = EGUEB_DOM_STRING_STATIC("mouseover");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_MOVE = EGUEB_DOM_STRING_STATIC("mousemove");
static Egueb_Dom_String _EGUEB_DOM_EVENT_MOUSE_OUT = EGUEB_DOM_STRING_STATIC("mouseout");
/*----------------------------------------------------------------------------*
 *                               Event interface                              *
 *----------------------------------------------------------------------------*/
static Ender_Item * _egueb_dom_event_mouse_item_get(Egueb_Dom_Event *thiz)
{
	const Ender_Lib *lib;

	lib = ender_lib_find("egueb_dom");
	return ender_lib_item_find(lib, "egueb.dom.event.mouse");
}

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_UI_DESCRIPTOR,
		Egueb_Dom_Event_Mouse, Egueb_Dom_Event_Mouse_Class,
		egueb_dom_event_mouse);

static void _egueb_dom_event_mouse_class_init(void *k)
{
	Egueb_Dom_Event_Class *klass;

	klass = EGUEB_DOM_EVENT_CLASS(k);
	klass->item_get = _egueb_dom_event_mouse_item_get;
}

static void _egueb_dom_event_mouse_instance_init(void *o)
{
}

static void _egueb_dom_event_mouse_instance_deinit(void *o)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(o);
	if (thiz->related)
	{
		egueb_dom_node_unref(thiz->related);
		thiz->related = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_event_mouse_click_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	thiz->screen_x = screen_x;
	thiz->screen_y = screen_y;
	thiz->client_x = client_x;
	thiz->client_y = client_y;
	thiz->alt_key = alt_key;
	thiz->ctrl_key = ctrl_key;
	thiz->shift_key = shift_key;
	thiz->meta_key = meta_key;
	thiz->button = button;
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_MOUSE_CLICK),
			EINA_TRUE, EINA_TRUE, NULL, detail);
}

void egueb_dom_event_mouse_down_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	thiz->screen_x = screen_x;
	thiz->screen_y = screen_y;
	thiz->client_x = client_x;
	thiz->client_y = client_y;
	thiz->alt_key = alt_key;
	thiz->ctrl_key = ctrl_key;
	thiz->shift_key = shift_key;
	thiz->meta_key = meta_key;
	thiz->button = button;
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_MOUSE_DOWN),
			EINA_TRUE, EINA_TRUE, NULL, detail);
}

void egueb_dom_event_mouse_up_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	thiz->screen_x = screen_x;
	thiz->screen_y = screen_y;
	thiz->client_x = client_x;
	thiz->client_y = client_y;
	thiz->alt_key = alt_key;
	thiz->ctrl_key = ctrl_key;
	thiz->shift_key = shift_key;
	thiz->meta_key = meta_key;
	thiz->button = button;
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_MOUSE_UP),
			EINA_TRUE, EINA_TRUE, NULL, detail);
}

void egueb_dom_event_mouse_move_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	thiz->screen_x = screen_x;
	thiz->screen_y = screen_y;
	thiz->client_x = client_x;
	thiz->client_y = client_y;
	thiz->alt_key = alt_key;
	thiz->ctrl_key = ctrl_key;
	thiz->shift_key = shift_key;
	thiz->meta_key = meta_key;
	thiz->button = button;
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_MOUSE_MOVE),
			EINA_TRUE, EINA_FALSE, NULL, detail);
}

void egueb_dom_event_mouse_over_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail, Egueb_Dom_Node *related)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	thiz->screen_x = screen_x;
	thiz->screen_y = screen_y;
	thiz->client_x = client_x;
	thiz->client_y = client_y;
	thiz->alt_key = alt_key;
	thiz->ctrl_key = ctrl_key;
	thiz->shift_key = shift_key;
	thiz->meta_key = meta_key;
	thiz->button = button;
	thiz->related = related;
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_MOUSE_OVER),
			EINA_TRUE, EINA_FALSE, NULL, detail);
}

void egueb_dom_event_mouse_out_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail, Egueb_Dom_Node *related)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	thiz->screen_x = screen_x;
	thiz->screen_y = screen_y;
	thiz->client_x = client_x;
	thiz->client_y = client_y;
	thiz->alt_key = alt_key;
	thiz->ctrl_key = ctrl_key;
	thiz->shift_key = shift_key;
	thiz->meta_key = meta_key;
	thiz->button = button;
	thiz->related = related;
	egueb_dom_event_ui_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_MOUSE_OUT),
			EINA_TRUE, EINA_FALSE, NULL, detail);
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

EAPI Egueb_Dom_Node * egueb_dom_event_mouse_related_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	return egueb_dom_node_ref(thiz->related);
}

EAPI Egueb_Dom_Event * egueb_dom_event_mouse_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_mouse);
	return event;
}

/**
 * @ender_prop{client_x}
 */
EAPI int egueb_dom_event_mouse_client_x_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	return thiz->client_x;
}

/**
 * @ender_prop{client_y}
 */
EAPI int egueb_dom_event_mouse_client_y_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	return thiz->client_y;
}

/**
 * Get the client coordinates
 * @param[in] e The mouse event to get the client coords from
 * @param[out] x The x coordinate
 * @param[out] y The y coordinate
 */
EAPI void egueb_dom_event_mouse_client_coords_get(Egueb_Dom_Event *e, int *x, int *y)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	if (x) *x = thiz->client_x;
	if (y) *y = thiz->client_y;
}

/**
 * Get the screen coordinates
 * @param[in] e The mouse event to get the screen coords from
 * @param[out] x The x coordinate
 * @param[out] y The y coordinate
 */
EAPI void egueb_dom_event_mouse_screen_coords_get(Egueb_Dom_Event *e, int *x, int *y)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	if (x) *x = thiz->screen_x;
	if (y) *y = thiz->screen_y;
}

EAPI int egueb_dom_event_mouse_button_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Mouse *thiz;

	thiz = EGUEB_DOM_EVENT_MOUSE(e);
	return thiz->button;
}
