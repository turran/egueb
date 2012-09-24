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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_private_input.h"

#include "esvg_event.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Esvg_Input
{
	Esvg_Input_Descriptor *descriptor;
	int x;
	int y;
	void *data;
	Ender_Element *over;
	Ender_Element *grabbed;
};

#if 0
typedef struct _Esvg_Input_Find_Data
{
	Esvg_Input *thiz;
	Eina_Bool found;
} Esvg_Input_Find_Data;

static void _esvg_input_element_found(Esvg_Input *thiz, Ender_Element *e)
{
	/* FIXME check if the element exists, if so, send a mouse_move */
	/* if not, send a mousein */
	ender_event_dispatch(e, "mousein", NULL);
	/* in order to send a mouse out, also iterate from the last one added
	 * and check if it is still valid
	 */
}

static Eina_Bool _esvg_input_find(Edom_Tag *t, Edom_Tag *child,
		void *user_data)
{
	Esvg_Input_Find_Data *data = user_data;
	Esvg_Input *thiz = data->thiz;
	Esvg_Type type;

	type = esvg_element_internal_type_get(child);
	if (!esvg_type_is_renderable(type) && type != ESVG_A)
		return EINA_TRUE;

	if (esvg_type_is_renderable(type))
	{
		Ender_Element *e;
		Enesim_Renderer *r;
		Eina_Rectangle in;
		Eina_Rectangle bounds;

		esvg_renderable_internal_renderer_get(child, &r);
		eina_rectangle_coords_from(&in, (int)thiz->last_x, (int)thiz->last_y, 1, 1);
		enesim_renderer_destination_boundings(r, &bounds, 0, 0);
		if (!eina_rectangles_intersect(&bounds, &in))
			return EINA_TRUE;

		e = esvg_element_ender_get(child);
		_esvg_input_element_found(thiz, e);
		if (type == ESVG_G || type == ESVG_SVG)
		{
			edom_tag_child_reverse_foreach(child, _esvg_input_find, data);
		}
		return EINA_TRUE;
	}
	else
	{
		edom_tag_child_reverse_foreach(child, _esvg_input_find, data);
		return data->found;
	}
}
#endif
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
			printf("mouse move!\n");
		}
	}
	else
	{
		/* send out event on i->r */
		if (thiz->over)
		{
			ender_event_dispatch(thiz->over, "mouseout", &ev);
			printf("mouse out!\n");
		}
		/* send in event on r */
		if (e)
		{
			ender_event_dispatch(e, "mouseover", &ev);
			printf("mouse in!\n");
		}
	}
	/* update the current over */
	thiz->over = e;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

