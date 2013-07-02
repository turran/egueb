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
#ifndef _EGUEB_DOM_EVENT_EXTERNAL_H_
#define _EGUEB_DOM_EVENT_EXTERNAL_H_

typedef void (*Egueb_Dom_Event_External_Free_Cb)(void *user_data);

EAPI Egueb_Dom_Event * egueb_dom_event_external_new(Egueb_Dom_String *type,
		Eina_Bool bubbleable, Eina_Bool cancelable, void *user_data,
		Egueb_Dom_Event_External_Free_Cb free_cb);
EAPI void * egueb_dom_event_external_data_get(Egueb_Dom_Event *event);

#endif
