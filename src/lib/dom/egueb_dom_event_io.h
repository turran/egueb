/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Lios Zapata
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
#ifndef _EGUEB_DOM_EVENT_IO_H_
#define _EGUEB_DOM_EVENT_IO_H_

/**
 * @defgroup Egueb_Dom_Event_IO_Group IO Event
 * @brief IO Event
 * @ingroup Egueb_Dom_Event_Group
 * @{
 */

EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_IO_DATA;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_IO_IMAGE;

typedef void (*Egueb_Dom_Event_IO_Data_Cb)(Egueb_Dom_Node *n,
		Enesim_Stream *s);
typedef void (*Egueb_Dom_Event_IO_Image_Cb)(Egueb_Dom_Node *n,
		Enesim_Surface *s);

EAPI Egueb_Dom_Event * egueb_dom_event_io_data_new(Egueb_Dom_String *uri,
		Egueb_Dom_Event_IO_Data_Cb cb);
EAPI Egueb_Dom_Event_IO_Data_Cb egueb_dom_event_io_data_cb_get(
		Egueb_Dom_Event *e);
EAPI Egueb_Dom_Event * egueb_dom_event_io_image_new(
		Egueb_Dom_String *uri, Egueb_Dom_Event_IO_Image_Cb cb);
EAPI Egueb_Dom_Event_IO_Image_Cb egueb_dom_event_io_image_cb_get(
		Egueb_Dom_Event *e);

/**
 * @}
 */

#endif

