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
#ifndef _EGUEB_DOM_EVENT_H_
#define _EGUEB_DOM_EVENT_H_

#include "egueb_dom_string.h"

/**
 * @file
 * @ender_group{Egueb_Dom_Event_Definition}
 * @ender_group{Egueb_Dom_Event_Phase}
 * @ender_group{Egueb_Dom_Event_Direction}
 * @ender_group{Egueb_Dom_Event}
 */

/**
 * @defgroup Egueb_Dom_Event_Phase Event phase
 * @ingroup Egueb_Dom_Event
 * @{
 */

typedef enum _Egueb_Dom_Event_Phase
{
	/** The event is going from topmost element to target element */
	EGUEB_DOM_EVENT_PHASE_CAPTURING,
	/** The event is at target */
	EGUEB_DOM_EVENT_PHASE_AT_TARGET,
	/** The event is going from the target to the topmost element */
	EGUEB_DOM_EVENT_PHASE_BUBBLING,
	EGUEB_DOM_EVENT_PHASES,
} Egueb_Dom_Event_Phase;

/**
 * @}
 * @defgroup Egueb_Dom_Event_Direction Event direction
 * @ingroup Egueb_Dom_Event
 * @{
 */

typedef enum _Egueb_Dom_Event_Direction
{
	EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE,
	EGUEB_DOM_EVENT_DIRECTION_BUBBLE_CAPTURE,
} Egueb_Dom_Event_Direction;

/**
 * @}
 * @defgroup Egueb_Dom_Event Event
 * @brief Event
 * @ingroup Egueb_Dom_Group
 * @{
 */

typedef struct _Egueb_Dom_Event Egueb_Dom_Event;

EAPI void egueb_dom_event_init(Egueb_Dom_Event *thiz, Egueb_Dom_String *type,
		Eina_Bool bubbleable, Eina_Bool capturable, Eina_Bool cancelable,
		Egueb_Dom_Event_Direction direction);

EAPI void egueb_dom_event_unref(Egueb_Dom_Event *thiz);
EAPI Egueb_Dom_Event * egueb_dom_event_ref(Egueb_Dom_Event *thiz);

EAPI Egueb_Dom_String * egueb_dom_event_type_get(Egueb_Dom_Event *thiz);
EAPI Egueb_Dom_Event_Phase egueb_dom_event_phase_get(Egueb_Dom_Event *thiz);
EAPI Egueb_Dom_Node * egueb_dom_event_target_get(Egueb_Dom_Event *thiz);
EAPI Egueb_Dom_Node * egueb_dom_event_target_current_get(Egueb_Dom_Event *thiz);
EAPI Egueb_Dom_Node * egueb_dom_event_relative_get(Egueb_Dom_Event *thiz);
EAPI void egueb_dom_event_stop_propagation(Egueb_Dom_Event *thiz);
EAPI Ender_Item * egueb_dom_event_item_get(Egueb_Dom_Event *thiz);

/**
 * @}
 * @defgroup Egueb_Dom_Event_Definition Event definitions
 * @ingroup Egueb_Dom_Event
 * @{
 */

typedef void (*Egueb_Dom_Event_Listener)(Egueb_Dom_Event *ev, void *data);


/**
 * @}
 */

#endif
