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

#include "egueb_dom_event_ui_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR, Egueb_Dom_Event_UI,
		Egueb_Dom_Event_UI_Class, egueb_dom_event_ui);

static void _egueb_dom_event_ui_class_init(void *k)
{
}

static void _egueb_dom_event_ui_instance_init(void *o)
{
}

static void _egueb_dom_event_ui_instance_deinit(void *o)
{
	Egueb_Dom_Event_UI *thiz;

	thiz = EGUEB_DOM_EVENT_UI(o);
	if (thiz->view)
	{
		egueb_dom_window_unref(thiz->view);
		thiz->view = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI int egueb_dom_event_ui_detail_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_UI *thiz;

	thiz = EGUEB_DOM_EVENT_UI(e);
	return thiz->detail;
}

EAPI void egueb_dom_event_ui_init(Egueb_Dom_Event *e, Egueb_Dom_String *type,
		Eina_Bool bubbleable, Eina_Bool cancelable,
		Egueb_Dom_Window *view, int detail)
{
	Egueb_Dom_Event_UI *thiz;

	thiz = EGUEB_DOM_EVENT_UI(e);
	thiz->detail = detail;
	thiz->view = view;
	egueb_dom_event_init(e, type, bubbleable, EINA_TRUE, cancelable,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
}

EAPI Egueb_Dom_Window * egueb_dom_event_ui_view_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_UI *thiz;

	thiz = EGUEB_DOM_EVENT_UI(e);
	return egueb_dom_window_ref(thiz->view);
}
