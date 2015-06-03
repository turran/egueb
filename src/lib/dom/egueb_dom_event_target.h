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
#ifndef _EGUEB_DOM_EVENT_TARGET_H_
#define _EGUEB_DOM_EVENT_TARGET_H_

/**
 * @file
 * @ender_group{Egueb_Dom_Event_Target}
 * @ender_group{Egueb_Dom_Event_Target_Definitions}
 */

/**
 * @defgroup Egueb_Dom_Event_Target_Definitions Definitions
 * @ingroup Egueb_Dom_Event_Target
 * @{
 */

typedef struct _Egueb_Dom_Event_Target_Listener Egueb_Dom_Event_Target_Listener;

/**
 * @}
 */

/**
 * @defgroup Egueb_Dom_Event_Target Event target
 * @{
 */

typedef struct _Egueb_Dom_Event_Target Egueb_Dom_Event_Target;

EAPI Egueb_Dom_Event_Target * egueb_dom_event_target_ref(
		Egueb_Dom_Event_Target *thiz);
EAPI void egueb_dom_event_target_unref(Egueb_Dom_Event_Target *thiz);
EAPI Eina_Bool egueb_dom_event_target_type_get(Egueb_Dom_Event_Target *thiz,
		const char **lib, const char **name);

EAPI Egueb_Dom_Event_Target_Listener * egueb_dom_event_target_event_listener_add(
		Egueb_Dom_Event_Target *thiz,
		const Egueb_Dom_String *type,
		Egueb_Dom_Event_Listener listener, Eina_Bool capture,
		void *data);
EAPI void egueb_dom_event_target_event_listener_remove(Egueb_Dom_Event_Target *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data);
EAPI void egueb_dom_event_target_event_listener_free(
		Egueb_Dom_Event_Target_Listener *target_listener);

EAPI Eina_Bool egueb_dom_event_target_event_dispatch(Egueb_Dom_Event_Target *thiz,
		Egueb_Dom_Event *event, Eina_Bool *notprevented,
		Eina_Error *err);

EAPI Enesim_Object_Descriptor * egueb_dom_event_target_descriptor_get(void);
#define EGUEB_DOM_EVENT_TARGET_DESCRIPTOR egueb_dom_event_target_descriptor_get()
#define EGUEB_DOM_EVENT_TARGET(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Event_Target, EGUEB_DOM_EVENT_TARGET_DESCRIPTOR)


/**
 * @}
 */

#endif
