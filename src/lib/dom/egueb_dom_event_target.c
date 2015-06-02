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
#include "egueb_dom_event_target.h"

#include "egueb_dom_event_target_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
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
}

static void _egueb_dom_event_target_instance_deinit(void *o)
{
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

#if 0
EAPI Egueb_Dom_Node_Event_Listener * egueb_dom_node_event_listener_add(Egueb_Dom_Node *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data)
{
	Egueb_Dom_Node_Event_Listener *nl;
	Egueb_Dom_Node_Event_Container *container;
	const char *str;

	str = egueb_dom_string_string_get(type);
	container = eina_hash_find(thiz->events, str);
	if (!container)
	{
		container = calloc(1, sizeof(Egueb_Dom_Node_Event_Container));
		eina_hash_add(thiz->events, str, container);
	}

	nl = calloc(1, sizeof(Egueb_Dom_Node_Event_Listener));
	nl->listener = listener;
	nl->capture = capture;
	nl->data = data;
	nl->container = container;

	container->listeners = eina_list_append(container->listeners, nl);
	return nl;
}

/* void  removeEventListener(in DOMString type,
         in EventListener listener,
         in boolean useCapture);
*/
EAPI void egueb_dom_node_event_listener_remove(Egueb_Dom_Node *thiz,
		const Egueb_Dom_String *type, Egueb_Dom_Event_Listener listener,
		Eina_Bool capture, void *data)
{
	Egueb_Dom_Node_Event_Listener *nl;
	Egueb_Dom_Node_Event_Container *container;
	Eina_List *l;
	const char *str;

	str = egueb_dom_string_string_get(type);
	container = eina_hash_find(thiz->events, str);
	if (!container || !container->listeners) return;

	EINA_LIST_FOREACH(container->listeners, l, nl)
	{
		if (nl->listener != listener || nl->capture != capture || nl->data != data)
			continue;
		free(nl);
		container->listeners = eina_list_remove_list(
				container->listeners, l);
		break;
	}
}

EAPI void egueb_dom_node_event_listener_free(Egueb_Dom_Node_Event_Listener *node_listener)
{
	Egueb_Dom_Node_Event_Container *container;

	container = node_listener->container;
	container->listeners = eina_list_remove(container->listeners,
			node_listener);
	free(node_listener);
}

EAPI Eina_Bool egueb_dom_node_event_dispatch(Egueb_Dom_Node *thiz,
		Egueb_Dom_Event *event, Eina_Bool *notprevented,
		Eina_Error *err)
{

}
#endif
