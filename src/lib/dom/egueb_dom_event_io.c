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
#include "egueb_dom_private.h"

#include "egueb_dom_string.h"
#include "egueb_dom_main.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_io.h"

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_io_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Dom_Event_IO
{
	Egueb_Dom_Event base;
} Egueb_Dom_Event_IO;

typedef struct _Egueb_Dom_Event_IO_Class
{
	Egueb_Dom_Event_Class base;
} Egueb_Dom_Event_IO_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_IO_FILE = EGUEB_DOM_STRING_STATIC("IOFile");
static Egueb_Dom_String _EGUEB_DOM_EVENT_IO_IMAGE = EGUEB_DOM_STRING_STATIC("IOImage");
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Dom_Event_IO, Egueb_Dom_Event_IO_Class,
		egueb_dom_event_io);

static void _egueb_dom_event_io_class_init(void *k)
{
}

static void _egueb_dom_event_io_instance_init(void *o)
{
}

static void _egueb_dom_event_io_instance_deinit(void *o)
{
}

static Egueb_Dom_Event * _egueb_dom_event_io_new(void)
{
	Egueb_Dom_Event *event;
	event = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_io);
	return event;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_IO_DATA = &_EGUEB_DOM_EVENT_IO_DATA;
Egueb_Dom_String *EGUEB_DOM_EVENT_IO_IMAGE = &_EGUEB_DOM_EVENT_IO_IMAGE;

EAPI Egueb_Dom_Event * egueb_dom_event_io_data_new(
		Egueb_Dom_Event_IO_Data_Cb cb)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_io_new();
	egueb_dom_event_init(e, EGUEB_DOM_EVENT_IO_DATA, EINA_TRUE, EINA_FALSE,
			EINA_FALSE, EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE); 
}

EAPI Egueb_Dom_Event * egueb_dom_event_io_image_new(
		Egueb_Dom_Event_IO_Image_Cb cb)
{
	Egueb_Dom_Event *e;

	e = _egueb_dom_event_io_new();
	egueb_dom_event_init(e, EGUEB_DOM_EVENT_IO_IMAGEA, EINA_TRUE, EINA_FALSE,
			EINA_FALSE, EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE); 
}
