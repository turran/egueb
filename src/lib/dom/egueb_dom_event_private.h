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
#ifndef _EGUEB_DOM_EVENT_PRIVATE_H_
#define _EGUEB_DOM_EVENT_PRIVATE_H_

#include "egueb_dom_node.h"

struct _Egueb_Dom_Event
{
	Enesim_Object_Instance parent;
	Egueb_Dom_String *type;
	Egueb_Dom_Node *target;
	Egueb_Dom_Node *current_target;
	Egueb_Dom_Node *relative;
	Egueb_Dom_Event_Phase phase;
	Eina_Bool bubbles;
	/* init attributes */
	Eina_Bool cancelable;
	Eina_Bool bubbleable;
	Eina_Bool capturable;
	Egueb_Dom_Event_Direction direction;
	/* private */
	Eina_Bool dispatching;
	Eina_Bool stopped;
	int ref;
};

typedef Ender_Item * (*Egueb_Dom_Event_Item_Get)(Egueb_Dom_Event *thiz);

typedef struct _Egueb_Dom_Event_Class
{
	Enesim_Object_Class parent;
	Egueb_Dom_Event_Item_Get item_get;
} Egueb_Dom_Event_Class;

#define EGUEB_DOM_EVENT_DESCRIPTOR egueb_dom_event_descriptor_get()

#define EGUEB_DOM_EVENT_CLASS_GET(o) EGUEB_DOM_EVENT_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,			\
		Egueb_Dom_Event_Class, EGUEB_DOM_EVENT_DESCRIPTOR)
#define EGUEB_DOM_EVENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Event, EGUEB_DOM_EVENT_DESCRIPTOR)

#endif

