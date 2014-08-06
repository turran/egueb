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

#ifndef _EGUEB_DOM_EVENT_KEYBOARD_H_
#define _EGUEB_DOM_EVENT_KEYBOARD_H_

#include "egueb_dom_event.h"

/**
 * @file
 * @ender_group{Egueb_Dom_Event_Keyboard}
 */

/**
 * @defgroup Egueb_Dom_Event_Keyboard Keyboard Event
 * TODO make it inherit from ui event
 * @brief Keyboard Event @ender_inherits{Egueb_Dom_Event}
 * @ingroup Egueb_Dom_Event_UI_Group
 * @{
 */

typedef enum _Egueb_Dom_Key_Location {
	EGUEB_DOM_KEY_LOCATION_STANDARD,
	EGUEB_DOM_KEY_LOCATION_LEFT,
	EGUEB_DOM_KEY_LOCATION_RIGHT,
	EGUEB_DOM_KEY_LOCATION_NUMPAD,
} Egueb_Dom_Key_Location;

EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_KEY_DOWN;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_KEY_UP;

/**
 * @}
 */

#endif
