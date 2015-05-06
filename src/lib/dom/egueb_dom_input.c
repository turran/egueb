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
#include "egueb_dom_event_keyboard.h"
#include "egueb_dom_event_mouse.h"
#include "egueb_dom_event_focus.h"
#include "egueb_dom_input.h"

#include "egueb_dom_event_mouse_private.h"
#include "egueb_dom_event_keyboard_private.h"

/* TODO
 * Right now on every mouse move we go down through the tree to look for
 * node. We better pass the current over node to let the implementation
 * be more clever by just looking if the same node is still inside.
 * The algorithm then can check if the current node is really inside and if
 * not go iterating up if the parent is inside, and then down again if required
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_Input
{
	Egueb_Dom_Input_Descriptor *descriptor;
	int ref;
	/* compose key flags */
	Eina_Bool alt_key;
	Eina_Bool ctrl_key;
	Eina_Bool shift_key;
	Eina_Bool meta_key; 
	/* last position where the mouse went down */
	int downx;
	int downy;
	int x;
	int y;
	Egueb_Dom_Node *over;
	Egueb_Dom_Node *grabbed;
	Egueb_Dom_Node *focused;
	void *data;
};

static void _egueb_dom_input_free(Egueb_Dom_Input *thiz)
{
	if (thiz->over)
	{
		egueb_dom_node_unref(thiz->over);
		thiz->over = NULL;
	}
	if (thiz->grabbed)
	{
		egueb_dom_node_unref(thiz->grabbed);
		thiz->grabbed = NULL;
	}
	if (thiz->focused)
	{
		egueb_dom_node_unref(thiz->focused);
		thiz->focused = NULL;
	}
	free(thiz);
}

static Eina_Bool _egueb_dom_input_key_has_modifier(Egueb_Dom_Input *thiz)
{
	if (thiz->alt_key && thiz->ctrl_key && thiz->shift_key && thiz->meta_key)
		return EINA_TRUE;
	else
		return EINA_FALSE;
}

static void _egueb_dom_input_key_modifier_set(Egueb_Dom_String *key,
		Eina_Bool *alt_key, Eina_Bool *ctrl_key,
		Eina_Bool *shift_key, Eina_Bool *meta_key,
		Egueb_Dom_Key_Location *location)
{
	const char *s;

	s = egueb_dom_string_string_get(key);
	if (!strcmp(s, "Control_L"))
	{
		*ctrl_key = EINA_TRUE;
		*location = EGUEB_DOM_KEY_LOCATION_LEFT;
	}
	else if (!strcmp(s, "Control_R"))
	{
		*ctrl_key = EINA_TRUE;
		*location = EGUEB_DOM_KEY_LOCATION_RIGHT;
	}
	else if (!strcmp(s, "Shift_L"))
	{
		*shift_key = EINA_TRUE;
		*location = EGUEB_DOM_KEY_LOCATION_LEFT;
	}
	else if (!strcmp(s, "Shift_R"))
	{
		*shift_key = EINA_TRUE;
		*location = EGUEB_DOM_KEY_LOCATION_RIGHT;
	}
	else if (!strncmp(s, "KP", 2))
	{
		*location = EGUEB_DOM_KEY_LOCATION_NUMPAD;
	}
}

static void _egueb_dom_input_key_modifier_unset(Egueb_Dom_String *key,
		Eina_Bool *alt_key, Eina_Bool *ctrl_key,
		Eina_Bool *shift_key, Eina_Bool *meta_key,
		Egueb_Dom_Key_Location *location)
{
	const char *s;

	s = egueb_dom_string_string_get(key);
	if (!strcmp(s, "Control_L"))
	{
		*ctrl_key = EINA_FALSE;
		*location = EGUEB_DOM_KEY_LOCATION_LEFT;
	}
	else if (!strcmp(s, "Control_R"))
	{
		*ctrl_key = EINA_FALSE;
		*location = EGUEB_DOM_KEY_LOCATION_RIGHT;
	}
	else if (!strcmp(s, "Shift_L"))
	{
		*shift_key = EINA_FALSE;
		*location = EGUEB_DOM_KEY_LOCATION_LEFT;
	}
	else if (!strcmp(s, "Shift_R"))
	{
		*shift_key = EINA_FALSE;
		*location = EGUEB_DOM_KEY_LOCATION_RIGHT;
	}
	else if (!strncmp(s, "KP", 2))
	{
		*location = EGUEB_DOM_KEY_LOCATION_NUMPAD;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Input * egueb_dom_input_new(Egueb_Dom_Input_Descriptor *descriptor, void *data)
{
	Egueb_Dom_Input *thiz;

	thiz = calloc(1, sizeof(Egueb_Dom_Input));
	thiz->descriptor = descriptor;
	thiz->data = data;
	thiz->ref = 1;

	return thiz;
}

EAPI void egueb_dom_input_unref(Egueb_Dom_Input *thiz)
{
	thiz->ref--;
	if (!thiz->ref)
	{
		_egueb_dom_input_free(thiz);
	}
}

EAPI Egueb_Dom_Input * egueb_dom_input_ref(Egueb_Dom_Input *thiz)
{
	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_input_mouse_position_get(Egueb_Dom_Input *thiz,
		int *x, int *y)
{
	if (!thiz) return;

	if (x) *x = thiz->x;
	if (y) *y = thiz->y;
}

EAPI void egueb_dom_input_feed_mouse_down(Egueb_Dom_Input *thiz, int button)
{
	Egueb_Dom_Event *ev;

	if (!thiz->over)
		return;
	/* store the coordinates where the mouse buton down was done to
	 * trigger the click later
	 */
	thiz->grabbed = egueb_dom_node_ref(thiz->over);
	thiz->downx = thiz->x;
	thiz->downy = thiz->y;

	ev = egueb_dom_event_mouse_new();
	egueb_dom_event_mouse_down_init(ev, thiz->x, thiz->y, thiz->x, thiz->y,
			EINA_FALSE, EINA_FALSE, EINA_FALSE,
			EINA_FALSE, button, 0);
	egueb_dom_node_event_dispatch(thiz->grabbed, ev, NULL, NULL);
}

EAPI void egueb_dom_input_feed_mouse_up(Egueb_Dom_Input *thiz, int button)
{
	Egueb_Dom_Event *ev;

	/* send the event to the grabbed object */
	if (!thiz->grabbed)
		return;

	ev = egueb_dom_event_mouse_new();
	egueb_dom_event_mouse_up_init(ev, thiz->x, thiz->y, thiz->x, thiz->y,
			EINA_FALSE, EINA_FALSE, EINA_FALSE,
			EINA_FALSE, button, 0);
	egueb_dom_node_event_dispatch(thiz->grabbed, ev, NULL, NULL);
	/* in case the down coordinates are the same as the current coordinates
	 * send a click event
	 */
	/* TODO define this threshold somewhere */
	if ((fabs(thiz->downx - thiz->x) < 2) &&
			(fabs(thiz->downy - thiz->y) < 2))
	{
		ev = egueb_dom_event_mouse_new();
		egueb_dom_event_mouse_click_init(ev, thiz->x, thiz->y, thiz->x, thiz->y,
				EINA_FALSE, EINA_FALSE, EINA_FALSE,
				EINA_FALSE, 0, 0);
		egueb_dom_node_event_dispatch(thiz->grabbed, ev, NULL, NULL);
	}
	egueb_dom_node_unref(thiz->grabbed);
	thiz->grabbed = NULL;
}

EAPI void egueb_dom_input_feed_mouse_move(Egueb_Dom_Input *thiz, int x, int y)
{
	Egueb_Dom_Node *n;

	thiz->x = x;
	thiz->y = y;

#if 0
	/* in case of dragging */
	if (thiz->grabbed)
	{
		Eon_Event_Mouse_Move ev;
		double rel_x, rel_y;


		rel_x = thiz->pointer.rel_x + (px - thiz->pointer.x);
		rel_y = thiz->pointer.rel_y + (py - thiz->pointer.y);
		/* we first send the drag start */
		if (!thiz->pointer.dragging)
		{
			Eon_Event_Mouse_Drag_Start ev_ds;

			ev_ds.input = thiz->input;
			ev_ds.x = x;
			ev_ds.y = y;
			ev_ds.rel_x = rel_x;
			ev_ds.rel_y = rel_y;
			egueb_dom_node_event_dispatch(thiz->grabbed,
					eon_input_event_names[EON_INPUT_EVENT_MOUSE_DRAG_START], &ev_ds);
			thiz->pointer.dragging = EINA_TRUE;
		}

		ev.input = thiz->input;
		ev.x = x;
		ev.y = y;
		ev.rel_x = rel_x;
		ev.rel_y = rel_y;
		ev.offset_x = offset_x;
		ev.offset_y = offset_y;
		egueb_dom_node_event_dispatch(thiz->grabbed,
					eon_input_event_names[EON_INPUT_EVENT_MOUSE_MOVE], &ev);

		return;
	}
#endif
	/* get the element at x,y */
	n = thiz->descriptor->element_at(thiz->over, x, y, thiz->data);
	//ev.screen_x = x;
	//ev.screen_y = y;

	if (n == thiz->over)
	{
		/* send move */
		if (n)
		{
			Egueb_Dom_Event *ev;

			ev = egueb_dom_event_mouse_new();
			egueb_dom_event_mouse_move_init(ev, x, y, x, y,
					EINA_FALSE, EINA_FALSE, EINA_FALSE,
					EINA_FALSE, 0, 0);
			egueb_dom_node_event_dispatch(n, ev, NULL, NULL);
		}
	}
	else
	{
		/* send out event on i->r */
		if (thiz->over)
		{

			Egueb_Dom_Event *ev;

			//egueb_dom_node_event_dispatch(thiz->over, "mousemove", &ev);
			ev = egueb_dom_event_mouse_new();
			egueb_dom_event_mouse_out_init(ev, x, y, x, y,
					EINA_FALSE, EINA_FALSE, EINA_FALSE,
					EINA_FALSE, 0, 0, n ? egueb_dom_node_ref(n) : NULL);
			egueb_dom_node_event_dispatch(thiz->over, ev, NULL, NULL);
			DBG("Out");
		}
		/* send in event on r */
		if (n)
		{
			Egueb_Dom_Event *ev;

			ev = egueb_dom_event_mouse_new();
			egueb_dom_event_mouse_over_init(ev, x, y, x, y,
					EINA_FALSE, EINA_FALSE, EINA_FALSE,
					EINA_FALSE, 0, 0, thiz->over ? egueb_dom_node_ref(thiz->over) : NULL);
			egueb_dom_node_event_dispatch(n, ev, NULL, NULL);
			DBG("Over");
		}
	}

	/* update the current over */
	if (thiz->over)
	{
		egueb_dom_node_unref(thiz->over);
	}
	thiz->over = n;
}

EAPI void egueb_dom_input_feed_mouse_wheel(Egueb_Dom_Input *thiz, int deltax, int deltay, int deltaz)
{
	if (!thiz->over)
		return;
	/* TODO create the wheel event */
	/* TODO dispatch it on the over node */
}

EAPI void egueb_dom_input_feed_key_down(Egueb_Dom_Input *thiz,
		Egueb_Dom_String *key)
{
	Egueb_Dom_Event *ev;
	Egueb_Dom_Key_Location location = EGUEB_DOM_KEY_LOCATION_STANDARD;
	const char *s;

	if (!key) return;

	s = egueb_dom_string_string_get(key);
	/* check if we need to navigate */
	if (!strcmp(s, "Tab"))
	{
		if (!_egueb_dom_input_key_has_modifier(thiz))
		{
			if (thiz->descriptor->focus_next)
			{
				Egueb_Dom_Node *n;
				n = thiz->descriptor->focus_next(thiz->focused, thiz->data);
				if (thiz->focused)
				{
					Egueb_Dom_Event *ev;

					DBG("Focus out");
					ev = egueb_dom_event_focus_out_new();
					egueb_dom_node_event_dispatch(thiz->focused, ev, NULL, NULL);
					egueb_dom_node_unref(thiz->focused);
				}
				thiz->focused = n;
				if (thiz->focused)
				{
					Egueb_Dom_Event *ev;

					DBG("Focus in");
					ev = egueb_dom_event_focus_in_new();
					egueb_dom_node_event_dispatch(thiz->focused, ev, NULL, NULL);
				}
			}
		}
		egueb_dom_string_unref(key);
		return;
	}
	/* check for the control keys */
	_egueb_dom_input_key_modifier_set(key, &thiz->alt_key, &thiz->ctrl_key,
			&thiz->shift_key, &thiz->meta_key, &location);

	/* nothing to do if we dont have any focused element */
	if (!thiz->focused)
	{
		egueb_dom_string_unref(key);
		return;
	}

	/* finally the key down */
	DBG("Key down");
	ev = egueb_dom_event_key_down_new(egueb_dom_string_ref(key), EGUEB_DOM_KEY_LOCATION_STANDARD,
		thiz->alt_key, thiz->ctrl_key, thiz->shift_key, thiz->meta_key);
	egueb_dom_node_event_dispatch(thiz->focused, ev, NULL, NULL);
	/* check for the activation */
	if (!strcmp(s, "Enter") || !strcmp(s, "Return"))
	{
		Egueb_Dom_Event *ev;

		DBG("Activating");
		ev = egueb_dom_event_mouse_new();
		egueb_dom_event_mouse_click_init(ev, thiz->x, thiz->y, thiz->x, thiz->y,
				EINA_FALSE, EINA_FALSE, EINA_FALSE,
				EINA_FALSE, 0, 0);
		egueb_dom_node_event_dispatch(thiz->focused, ev, NULL, NULL);
	}
	egueb_dom_string_unref(key);
}

EAPI void egueb_dom_input_feed_key_up(Egueb_Dom_Input *thiz,
		Egueb_Dom_String *key)
{
	Egueb_Dom_Event *ev;
	Egueb_Dom_Key_Location location = EGUEB_DOM_KEY_LOCATION_STANDARD;
	const char *s;

	if (!key) return;

	s = egueb_dom_string_string_get(key);
	if (!strcmp(s, "Tab"))
	{
		egueb_dom_string_unref(key);
		return;
	}

	/* check for the control keys */
	_egueb_dom_input_key_modifier_unset(key, &thiz->alt_key, &thiz->ctrl_key,
			&thiz->shift_key, &thiz->meta_key, &location);

	/* unset the flags */
	if (!thiz->focused)
	{
		egueb_dom_string_unref(key);
		return;
	}
	ev = egueb_dom_event_key_up_new(key, EGUEB_DOM_KEY_LOCATION_STANDARD,
		thiz->alt_key, thiz->ctrl_key, thiz->shift_key, thiz->meta_key);
	egueb_dom_node_event_dispatch(thiz->focused, ev, NULL, NULL);
}
