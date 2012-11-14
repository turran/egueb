/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_private_input.h"

#include "esvg_element.h"
#include "esvg_event.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Esvg_Input
{
	Esvg_Input_Descriptor *descriptor;
	int downx;
	int downy;
	int x;
	int y;
	void *data;
	Ender_Element *over;
	Ender_Element *grabbed;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Esvg_Input * esvg_input_new(Esvg_Input_Descriptor *descriptor, void *data)
{
	Esvg_Input *thiz;

	thiz = calloc(1, sizeof(Esvg_Input));
	thiz->descriptor = descriptor;
	thiz->data = data;
	return thiz;
}

void esvg_input_free(Esvg_Input *thiz)
{
	free(thiz);
}

void esvg_input_feed_mouse_down(Esvg_Input *thiz, int button)
{
	Esvg_Event_Mouse ev;
	double rel_x, rel_y;

	if (!thiz->over)
		return;
	/* store the coordinates where the mouse buton down was done to
	 * trigger the click later
	 */
	thiz->grabbed = thiz->over;
	thiz->downx = thiz->x;
	thiz->downy = thiz->y;
	
	//printf("mouse down! on %s\n", esvg_element_name_get(thiz->over));
	ender_event_dispatch(thiz->over, "mousedown", &ev);
}

void esvg_input_feed_mouse_up(Esvg_Input *thiz, int button)
{
	Esvg_Event_Mouse ev;

	/* send the event to the grabbed object */
	if (!thiz->grabbed)
		return;

	//printf("mouse up! on %s\n", esvg_element_name_get(thiz->grabbed));
	ender_event_dispatch(thiz->grabbed, "mouseup", &ev);
	/* in case the down coordinates are the same as the current coordinates
	 * send a click event
	 */
	/* TODO define this threshold somewhere */
	if ((fabs(thiz->downx - thiz->x) < 2) &&
			(fabs(thiz->downy - thiz->y) < 2))
	{
		//printf("mouse click! on %s\n", esvg_element_name_get(thiz->grabbed));
		ender_event_dispatch(thiz->grabbed, "click", &ev);
	}
	thiz->grabbed = NULL;

}

void esvg_input_feed_mouse_move(Esvg_Input *thiz, int x, int y)
{
	Esvg_Event_Mouse ev;
	Ender_Element *e;

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
			ender_event_dispatch(thiz->grabbed,
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
		ender_event_dispatch(thiz->grabbed,
					eon_input_event_names[EON_INPUT_EVENT_MOUSE_MOVE], &ev);

		return;
	}
#endif
	/* get the element at x,y */
	e = thiz->descriptor->element_at(thiz->data, x, y);
	ev.screen_x = x;
	ev.screen_y = y;

	if (e == thiz->over)
	{
		/* send move */
		if (e)
		{
			ender_event_dispatch(e, "mousemove", &ev);
			//printf("mouse move! on %s\n", esvg_element_name_get(e));
		}
	}
	else
	{
		/* send out event on i->r */
		if (thiz->over)
		{
			ender_event_dispatch(thiz->over, "mousemove", &ev);
			ender_event_dispatch(thiz->over, "mouseout", &ev);
			//printf("mouse out! on %s\n", esvg_element_name_get(thiz->over));
		}
		/* send in event on r */
		if (e)
		{
			ender_event_dispatch(e, "mouseover", &ev);
			ender_event_dispatch(e, "mousemove", &ev);
			//printf("mouse in! %s\n", esvg_element_name_get(e));
		}
	}
	/* update the current over */
	thiz->over = e;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

