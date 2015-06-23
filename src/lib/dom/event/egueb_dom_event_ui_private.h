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
#ifndef _EGUEB_DOM_EVENT_UI_PRIVATE_H_
#define _EGUEB_DOM_EVENT_UI_PRIVATE_H_

#include "egueb_dom_event_private.h"

typedef struct _Egueb_Dom_Event_UI
{
	Egueb_Dom_Event base;
	Egueb_Dom_Window *view;
	int detail;
} Egueb_Dom_Event_UI;

typedef struct _Egueb_Dom_Event_UI_Class
{
	Egueb_Dom_Event_Class base;
} Egueb_Dom_Event_UI_Class;

#define EGUEB_DOM_EVENT_UI_DESCRIPTOR egueb_dom_event_ui_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_ui_descriptor_get(void);

#define EGUEB_DOM_EVENT_UI_CLASS_GET(o) EGUEB_DOM_EVENT_UI_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_UI_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Event_UI_Class, EGUEB_DOM_EVENT_UI_DESCRIPTOR)
#define EGUEB_DOM_EVENT_UI(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Event_UI, EGUEB_DOM_EVENT_UI_DESCRIPTOR)

#endif


