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
#ifndef _EGUEB_DOM_EVENT_UI_H_
#define _EGUEB_DOM_EVENT_UI_H_

/**
 * @file
 * @ender_group{Egueb_Dom_Event_UI}
 */

/**
 * @defgroup Egueb_Dom_Event_UI UI Event
 * @brief UI Event @ender_inherits{Egueb_Dom_Event}
 * @ingroup Egueb_Dom_Event
 * @{
 */

EAPI int egueb_dom_event_ui_detail_get(Egueb_Dom_Event *n);
EAPI void egueb_dom_event_ui_init(Egueb_Dom_Event *e, Egueb_Dom_String *type,
		Eina_Bool bubbleable, Eina_Bool cancelable, Egueb_Dom_Window *view, int detail);
EAPI Egueb_Dom_Window * egueb_dom_event_ui_view_get(Egueb_Dom_Event *e);

/**
 * @}
 */

#endif
