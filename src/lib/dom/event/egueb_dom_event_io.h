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
#ifndef _EGUEB_DOM_EVENT_IO_H_
#define _EGUEB_DOM_EVENT_IO_H_

/**
 * @file
 * @ender_group{Egueb_Dom_Event_IO_Definitions}
 * @ender_group{Egueb_Dom_Event_IO}
 */

/**
 * @defgroup Egueb_Dom_Event_IO_Definitions Definitions
 * @ingroup Egueb_Dom_Event_IO
 * @{
 */

/**
 * @ender_name{egueb.dom.event.io.data_cb}
 */
typedef void (*Egueb_Dom_Event_IO_Data_Cb)(Egueb_Dom_Node *n,
		Enesim_Stream *s);
/**
 * @ender_name{egueb.dom.event.io.image_cb}
 */
typedef void (*Egueb_Dom_Event_IO_Image_Cb)(Egueb_Dom_Node *n,
		Enesim_Surface *s);

EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_IO_DATA;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_IO_IMAGE;
EAPI extern Egueb_Dom_String *EGUEB_DOM_EVENT_IO_DONE;


/**
 * @}
 * @defgroup Egueb_Dom_Event_IO IO Event
 * @brief IO Event @ender_inherits{Egueb_Dom_Event}
 * @ingroup Egueb_Dom_Event
 * @{
 */

EAPI Egueb_Dom_Event * egueb_dom_event_io_data_new(Egueb_Dom_Uri *uri,
		Egueb_Dom_Event_IO_Data_Cb cb);
EAPI Egueb_Dom_Event * egueb_dom_event_io_image_new(
		Enesim_Stream *s, Egueb_Dom_Event_IO_Image_Cb cb);

EAPI Enesim_Stream * egueb_dom_event_io_stream_get(Egueb_Dom_Event *e);
EAPI void egueb_dom_event_io_uri_get(Egueb_Dom_Event *e, Egueb_Dom_Uri *uri);
EAPI void egueb_dom_event_io_image_finish(Egueb_Dom_Event *e, Enesim_Surface *s);
EAPI void egueb_dom_event_io_data_finish(Egueb_Dom_Event *e, Enesim_Stream *s);

/**
 * @}
 */

#endif

