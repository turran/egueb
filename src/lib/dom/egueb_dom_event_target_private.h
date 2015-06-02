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

struct _Egueb_Dom_Event_Target
{
	Enesim_Object_Instance base;
};

typedef void (*Egueb_Dom_Event_Target_Ref)(Egueb_Dom_Event_Target *thiz);
typedef void (*Egueb_Dom_Event_Target_Unref)(Egueb_Dom_Event_Target *thiz);
typedef Eina_Bool (*Egueb_Dom_Event_Target_Type_Get)(Egueb_Dom_Event_Target *thiz,
		const char **lib, const char **name);
typedef void (*Egueb_Dom_Event_Target_Event_Listener_Add)(Egueb_Dom_Event_Target *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data);
typedef void (*Egueb_Dom_Event_Target_Event_Listener_Remove)(Egueb_Dom_Event_Target *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data);
typedef Eina_Bool (*Egueb_Dom_Event_Target_Event_Dispatch)(Egueb_Dom_Event_Target *thiz,
		Egueb_Dom_Event *event, Eina_Bool *notprevented, Eina_Error *err);

typedef struct _Egueb_Dom_Event_Target_Class
{
	Enesim_Object_Class base;
	Egueb_Dom_Event_Target_Ref ref;
	Egueb_Dom_Event_Target_Unref unref;
	Egueb_Dom_Event_Target_Type_Get type_get;
	Egueb_Dom_Event_Target_Event_Listener_Add listener_add;
	Egueb_Dom_Event_Target_Event_Listener_Remove listener_remove;
	Egueb_Dom_Event_Target_Event_Dispatch dispatch;
} Egueb_Dom_Event_Target_Class;

#define EGUEB_DOM_EVENT_TARGET_DESCRIPTOR egueb_dom_event_target_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_target_descriptor_get(void);

#define EGUEB_DOM_EVENT_TARGET_CLASS_GET(o) EGUEB_DOM_EVENT_TARGET_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_TARGET_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,			\
		Egueb_Dom_Event_Target_Class, EGUEB_DOM_EVENT_TARGET_DESCRIPTOR)
#define EGUEB_DOM_EVENT_TARGET(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Event_Target, EGUEB_DOM_EVENT_TARGET_DESCRIPTOR)

#endif

