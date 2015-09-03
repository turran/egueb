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

#ifndef _EGUEB_DOM_EVENT_MOUSE_H_
#define _EGUEB_DOM_EVENT_MOUSE_H_

#include "egueb_dom_event.h"

/**
 * @file
 * @ender_group{Egueb_Dom_Event_Mouse}
 * @ender_group{Egueb_Dom_Event_Mouse_Definitions}
 */

/**
 * @defgroup Egueb_Dom_Event_Mouse_Definitions Definitions
 * @ingroup Egueb_Dom_Event_Mouse 
 * @{
 */

EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_CLICK;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_DOWN;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_UP;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_OVER;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_MOVE;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_OUT;

/**
 * @}
 * @defgroup Egueb_Dom_Event_Mouse Mouse Event
 * TODO make it inherit from ui event
 * @brief Mouse Event @ender_inherits{Egueb_Dom_Event}
 * @ingroup Egueb_Dom_Event_UI_Group
 * @{
 */

EAPI Egueb_Dom_Event * egueb_dom_event_mouse_new(void);

EAPI Egueb_Dom_Node * egueb_dom_event_mouse_related_get(Egueb_Dom_Event *e);
EAPI void egueb_dom_event_mouse_screen_coords_get(Egueb_Dom_Event *e, int *x, int *y);
EAPI int egueb_dom_event_mouse_button_get(Egueb_Dom_Event *e);

EAPI int egueb_dom_event_mouse_client_x_get(Egueb_Dom_Event *e);
EAPI int egueb_dom_event_mouse_client_y_get(Egueb_Dom_Event *e);
EAPI void egueb_dom_event_mouse_client_coords_get(Egueb_Dom_Event *e, int *x, int *y);

/**
 * @}
 */

#endif

