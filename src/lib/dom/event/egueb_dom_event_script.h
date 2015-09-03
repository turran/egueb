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
#ifndef _EGUEB_DOM_EVENT_SCRIPT_H_
#define _EGUEB_DOM_EVENT_SCRIPT_H_

/**
 * @file
 * @ender_group{Egueb_Dom_Event_Script}
 * @ender_group{Egueb_Dom_Event_Script_Definitions}
 */

/**
 * @defgroup Egueb_Dom_Event_Script_Definitions Definitions
 * @ingroup Egueb_Dom_Event_Script 
 * @{
 */

EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_SCRIPT_SCRIPTER;


/**
 * @}
 * @defgroup Egueb_Dom_Event_Script Script Event
 * @brief Script Event @ender_inherits{Egueb_Dom_Event}
 * @ingroup Egueb_Dom_Event
 * @{
 */

EAPI Egueb_Dom_Event * egueb_dom_event_script_new(Egueb_Dom_String *type);

EAPI Egueb_Dom_String * egueb_dom_event_script_type_get(Egueb_Dom_Event *e);
EAPI Egueb_Dom_Scripter * egueb_dom_event_script_scripter_get(Egueb_Dom_Event *e);
EAPI void egueb_dom_event_script_scripter_set(Egueb_Dom_Event *e, Egueb_Dom_Scripter *sc);

/**
 * @}
 */

#endif

