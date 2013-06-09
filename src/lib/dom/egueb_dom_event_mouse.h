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

extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_CLICK = &_EGUEB_DOM_EVENT_MOUSE_CLICK;
extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_DOWN = &_EGUEB_DOM_EVENT_MOUSE_DOWN;
extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_UP = &_EGUEB_DOM_EVENT_MOUSE_UP;
extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_OVER = &_EGUEB_DOM_EVENT_MOUSE_OVER;
extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_MOVE = &_EGUEB_DOM_EVENT_MOUSE_MOVE;
extern Egueb_Dom_String *EGUEB_DOM_EVENT_MOUSE_OUT = &_EGUEB_DOM_EVENT_MOUSE_OUT;

EAPI Eina_Error egueb_dom_event_mouse_related_get(Egueb_Dom_Event *e, Egueb_Dom_Node **related);

#endif

