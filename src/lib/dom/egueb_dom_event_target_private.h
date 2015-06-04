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

#ifndef _EGUEB_DOM_EVENT_TARGET_PRIVATE_H_
#define _EGUEB_DOM_EVENT_TARGET_PRIVATE_H_

typedef struct _Egueb_Dom_Event_Target_Listener_Container
{
	Eina_List *listeners;
} Egueb_Dom_Event_Target_Listener_Container;

struct _Egueb_Dom_Event_Target_Listener
{
	Egueb_Dom_Event_Listener listener;
	Egueb_Dom_Event_Target_Listener_Container *container;
	Eina_Bool capture;
	void *data;
};

struct _Egueb_Dom_Event_Target
{
	Enesim_Object_Instance base;
	/* event related data */
	Eina_Hash *events;
	Eina_List *monitors;
};

typedef void (*Egueb_Dom_Event_Target_Ref)(Egueb_Dom_Event_Target *thiz);
typedef void (*Egueb_Dom_Event_Target_Unref)(Egueb_Dom_Event_Target *thiz);
typedef Eina_Bool (*Egueb_Dom_Event_Target_Type_Get)(Egueb_Dom_Event_Target *thiz,
		const char **lib, const char **name);
typedef Eina_Bool (*Egueb_Dom_Event_Target_Dispatch)(Egueb_Dom_Event_Target *thiz,
		Egueb_Dom_Event *event, Eina_Bool *notprevented, Eina_Error *err);

typedef struct _Egueb_Dom_Event_Target_Class
{
	Enesim_Object_Class base;
	Egueb_Dom_Event_Target_Ref ref;
	Egueb_Dom_Event_Target_Unref unref;
	Egueb_Dom_Event_Target_Type_Get type_get;
	Egueb_Dom_Event_Target_Dispatch dispatch;
} Egueb_Dom_Event_Target_Class;

#define EGUEB_DOM_EVENT_TARGET_CLASS_GET(o) EGUEB_DOM_EVENT_TARGET_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_TARGET_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,			\
		Egueb_Dom_Event_Target_Class, EGUEB_DOM_EVENT_TARGET_DESCRIPTOR)

#endif

