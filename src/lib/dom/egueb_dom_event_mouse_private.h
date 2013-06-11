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

#ifndef _EGUEB_DOM_EVENT_MOUSE_PRIVATE_H_
#define _EGUEB_DOM_EVENT_MOUSE_PRIVATE_H_

void egueb_dom_event_mouse_click_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail);
void egueb_dom_event_mouse_down_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail);
void egueb_dom_event_mouse_up_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail);
void egueb_dom_event_mouse_move_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail)
void egueb_dom_event_mouse_over_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail, Egueb_Dom_Node *related);
void egueb_dom_event_mouse_out_init(Egueb_Dom_Event *e,
		int screen_x, int screen_y, int client_x, int client_y,
		Eina_Bool alt_key, Eina_Bool ctrl_key, Eina_Bool shift_key,
		Eina_Bool meta_key, int button, int detail, Egueb_Dom_Node *related);

#endif
