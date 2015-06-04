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
#include "egueb_dom_node.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_target.h"

#include "egueb_dom_event_target_private.h"
#include "egueb_dom_event_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_dom_event_target_listener_container_free(void *d)
{
	Egueb_Dom_Event_Target_Listener_Container *thiz = d;
	Egueb_Dom_Event_Target_Listener *nl;

	EINA_LIST_FREE(thiz->listeners, nl)
	{
		free(nl);
	}
	free(thiz);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(ENESIM_OBJECT_DESCRIPTOR, Egueb_Dom_Event_Target,
		Egueb_Dom_Event_Target_Class, egueb_dom_event_target);

static void _egueb_dom_event_target_class_init(void *k)
{
}

static void _egueb_dom_event_target_instance_init(void *o)
{
	Egueb_Dom_Event_Target *thiz;

	thiz = EGUEB_DOM_EVENT_TARGET(o);
	thiz->events = eina_hash_string_superfast_new(
			_egueb_dom_event_target_listener_container_free);
}

static void _egueb_dom_event_target_instance_deinit(void *o)
{
	Egueb_Dom_Event_Target *thiz;
	Egueb_Dom_Event_Target_Listener *el;

	thiz = EGUEB_DOM_EVENT_TARGET(o);
	/* remove the whole set of events */
	eina_hash_free(thiz->events);
	/* remove the monitors */
	EINA_LIST_FREE(thiz->monitors, el)
	{
		free(el);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Event_Target * egueb_dom_event_target_ref(
		Egueb_Dom_Event_Target *thiz)
{
	Egueb_Dom_Event_Target_Class *klass;

	klass = EGUEB_DOM_EVENT_TARGET_CLASS_GET(thiz);
	klass->ref(thiz);
	return thiz;
}

EAPI void egueb_dom_event_target_unref(Egueb_Dom_Event_Target *thiz)
{
	Egueb_Dom_Event_Target_Class *klass;

	klass = EGUEB_DOM_EVENT_TARGET_CLASS_GET(thiz);
	klass->unref(thiz);
}

EAPI Eina_Bool egueb_dom_event_target_type_get(Egueb_Dom_Event_Target *thiz,
		const char **lib, const char **name)
{
	Egueb_Dom_Event_Target_Class *klass;

	klass = EGUEB_DOM_EVENT_TARGET_CLASS_GET(thiz);
	return klass->type_get(thiz, lib, name);
}

/* void  addEventListener(in DOMString type,
         in EventListener listener,
         in boolean useCapture);
*/
EAPI Egueb_Dom_Event_Target_Listener *
egueb_dom_event_target_event_listener_add(Egueb_Dom_Event_Target *thiz,
		const Egueb_Dom_String *type,
		Egueb_Dom_Event_Listener listener, Eina_Bool capture,
		void *data)
{
	Egueb_Dom_Event_Target_Listener *tl;
	Egueb_Dom_Event_Target_Listener_Container *container;
	const char *str;

	str = egueb_dom_string_string_get(type);
	container = eina_hash_find(thiz->events, str);
	if (!container)
	{
		container = calloc(1, sizeof(Egueb_Dom_Event_Target_Listener_Container));
		eina_hash_add(thiz->events, str, container);
	}

	tl = calloc(1, sizeof(Egueb_Dom_Event_Target_Listener));
	tl->listener = listener;
	tl->capture = capture;
	tl->data = data;
	tl->container = container;

	container->listeners = eina_list_append(container->listeners, tl);
	return tl;
}

/* void  removeEventListener(in DOMString type,
         in EventListener listener,
         in boolean useCapture);
*/
EAPI void egueb_dom_event_target_event_listener_remove(Egueb_Dom_Event_Target *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data)
{
	Egueb_Dom_Event_Target_Listener *tl;
	Egueb_Dom_Event_Target_Listener_Container *container;
	Eina_List *l;
	const char *str;

	str = egueb_dom_string_string_get(type);
	container = eina_hash_find(thiz->events, str);
	if (!container || !container->listeners) return;

	EINA_LIST_FOREACH(container->listeners, l, tl)
	{
		if (tl->listener != listener || tl->capture != capture || tl->data != data)
			continue;
		free(tl);
		container->listeners = eina_list_remove_list(
				container->listeners, l);
		break;
	}
}

EAPI void egueb_dom_event_target_event_listener_free(
		Egueb_Dom_Event_Target_Listener *target_listener)
{
	Egueb_Dom_Event_Target_Listener_Container *container;

	container = target_listener->container;
	container->listeners = eina_list_remove(container->listeners,
			target_listener);
	free(target_listener);
}

/**
 * Dispatch an event in an event target
 * @param[in] thiz The event target to dispatch the event into
 * @param[in] event The event to dispatch
 * @param[out] notprevented In case the event has not been prevented
 * @param[out] err The exception in case the dispatch fails
 * @return EINA_TRUE if the disptach succeeds, EINA_FALSE otherwise.
 */
EAPI Eina_Bool egueb_dom_event_target_event_dispatch(Egueb_Dom_Event_Target *thiz,
		Egueb_Dom_Event *event, Eina_Bool *notprevented,
		Eina_Error *err)
{
	Egueb_Dom_Event_Target_Class *klass;
	Egueb_Dom_Event_Target_Listener_Container *container;
	Egueb_Dom_Event_Target_Listener *nl;
	Eina_List *l;
	Eina_Bool ret = EINA_TRUE;

	klass = EGUEB_DOM_EVENT_TARGET_CLASS_GET(thiz);
	/* FIXME also if the string is empty */
	if (!event || !event->type)
	{
		if (event)
			egueb_dom_event_unref(event);
		if (err)
			*err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}
	if (event->dispatching)
	{
		egueb_dom_event_unref(event);
		/* return EGUEB_DOM_EVENT_UNSPECIFIED_EVENT_TYPE_ERR; */
		if (err)
			*err = EGUEB_DOM_ERROR_INVALID_ACCESS;
		return EINA_FALSE;
	}

	container = eina_hash_find(thiz->events,
		egueb_dom_string_string_get(event->type));
	if (!container || !container->listeners)
		goto monitors;

	ret = klass->dispatch(thiz, event, notprevented, err);

monitors:
	/* now the monitors */
	if (thiz->monitors)
	{
		EINA_LIST_FOREACH(thiz->monitors, l, nl)
		{
			if (event->target == event->current_target)
			{
				Egueb_Dom_Event_Phase old_phase = event->phase;
				nl->listener(event, nl->data);
				event->phase = old_phase;
			}
			else
			{
				nl->listener(event, nl->data);
			}
		}
	}
	return ret;
}

EAPI void egueb_dom_event_target_monitor_add(
		Egueb_Dom_Event_Target *thiz,
		Egueb_Dom_Event_Listener listener,
		void *data)
{
	Egueb_Dom_Event_Target_Listener *nl;

	nl = calloc(1, sizeof(Egueb_Dom_Event_Target_Listener));
	nl->listener = listener;
	nl->data = data;

	thiz->monitors = eina_list_append(thiz->monitors, nl);
}

EAPI void egueb_dom_event_target_monitor_remove(
		Egueb_Dom_Event_Target *thiz,
		Egueb_Dom_Event_Listener listener,
		void *data)
{
	Egueb_Dom_Event_Target_Listener *nl;
	Eina_List *l;

	EINA_LIST_FOREACH(thiz->monitors, l, nl)
	{
		if (nl->listener != listener || nl->data != data)
			continue;
		free(nl);
		thiz->monitors = eina_list_remove_list(thiz->monitors, l);
		break;
	}
}
