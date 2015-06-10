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
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_node.h"
#include "egueb_dom_window.h"

#include "egueb_dom_event_private.h"
#include "egueb_dom_event_target_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_WINDOW_CLASS_GET(o) EGUEB_DOM_WINDOW_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_WINDOW_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,			\
		Egueb_Dom_Window_Class, EGUEB_DOM_WINDOW_DESCRIPTOR)
#define EGUEB_DOM_WINDOW_DESCRIPTOR egueb_dom_window_descriptor_get()
#define EGUEB_DOM_WINDOW(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Window, EGUEB_DOM_WINDOW_DESCRIPTOR)

struct _Egueb_Dom_Window
{
	Egueb_Dom_Event_Target base;
	const Egueb_Dom_Window_Descriptor *desc;
	Egueb_Dom_Node *doc;
	int ref;
	void *data;
};

typedef struct _Egueb_Dom_Window_Class
{
	Egueb_Dom_Event_Target_Class base;
} Egueb_Dom_Window_Class;

/*----------------------------------------------------------------------------*
 *                            Event target interface                          *
 *----------------------------------------------------------------------------*/
static void _egueb_dom_window_ref(Egueb_Dom_Event_Target *target)
{
	Egueb_Dom_Window *thiz;

	thiz = EGUEB_DOM_WINDOW(target);
	egueb_dom_window_ref(thiz);
}

static void _egueb_dom_window_unref(Egueb_Dom_Event_Target *target)
{
	Egueb_Dom_Window *thiz;

	thiz = EGUEB_DOM_WINDOW(target);
	egueb_dom_window_unref(thiz);
}

static Eina_Bool _egueb_dom_window_type_get(Egueb_Dom_Event_Target *target,
		const char **lib, const char **name)
{
	if (lib) *lib = "egueb_dom";
	if (name) *name = "window";

	return EINA_TRUE;
}

static Eina_Bool _egueb_dom_window_dispatch(Egueb_Dom_Event_Target *target,
		Egueb_Dom_Event *event, Eina_Bool *notprevented, Eina_Error *err)
{
	Egueb_Dom_Window *thiz;
	Egueb_Dom_Event_Target_Listener_Container *container;
	Egueb_Dom_Event_Target_Listener *nl;
	Eina_List *l;

	thiz = EGUEB_DOM_WINDOW(target);
	/* DISPATCH_REQUEST_ERR: Raised if the Event object is already being dispatched in the tree.*/
	/* NOT_SUPPORTED_ERR Raised if the Event object has not been created using DocumentEvent.createEvent() or does not support the interface CustomEvent */

	if (event->target)
	{
		egueb_dom_event_target_unref(event->target);
		event->target = NULL;
	}
	/* setup the event with the basic attributes */
	egueb_dom_window_ref(thiz);
	event->target = target;
	event->dispatching = EINA_TRUE;

	/* finally dispatch */
	container = eina_hash_find(target->events,
		egueb_dom_string_string_get(event->type));
	if (!container || !container->listeners)
		goto done;

	EINA_LIST_FOREACH(container->listeners, l, nl)
	{
		nl->listener(event, nl->data);
	}

done:
	event->dispatching = EINA_FALSE;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_TARGET_DESCRIPTOR,
		Egueb_Dom_Window, Egueb_Dom_Window_Class, egueb_dom_window);

static void _egueb_dom_window_class_init(void *k)
{
	Egueb_Dom_Event_Target_Class *klass;

	klass = EGUEB_DOM_EVENT_TARGET_CLASS(k);
	klass->ref = _egueb_dom_window_ref;
	klass->unref = _egueb_dom_window_unref;
	klass->type_get = _egueb_dom_window_type_get;
	klass->dispatch = _egueb_dom_window_dispatch;
}

static void _egueb_dom_window_instance_init(void *o)
{

}

static void _egueb_dom_window_instance_deinit(void *o)
{
	Egueb_Dom_Window *thiz;

	thiz = EGUEB_DOM_WINDOW(o);
	egueb_dom_node_weak_ref_remove(thiz->doc, &thiz->doc);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Window * egueb_dom_window_new(
		const Egueb_Dom_Window_Descriptor *desc,
		Egueb_Dom_Node *doc)
{
	Egueb_Dom_Window *thiz;

	if (!desc) return NULL;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_window);
	thiz->desc = desc;
	egueb_dom_node_weak_ref_add(doc, &thiz->doc);
	thiz->data = thiz->desc->create();
	thiz->ref = 1;

	return thiz;
}

EAPI Egueb_Dom_Window * egueb_dom_window_ref(Egueb_Dom_Window *thiz)
{
	if (!thiz) return thiz;
	thiz->ref++;
	return thiz;
}

EAPI void egueb_dom_window_unref(Egueb_Dom_Window *thiz)
{
	if (!thiz) return;
	thiz->ref--;
	if (!thiz->ref)
	{
		enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
	} 
}

EAPI void * egueb_dom_window_data_get(Egueb_Dom_Window *thiz)
{
	if (!thiz) return NULL;
	return thiz->data;
}

EAPI Egueb_Dom_Node * egueb_dom_window_document_get(Egueb_Dom_Window *thiz)
{
	if (!thiz) return NULL;
	return egueb_dom_node_ref(thiz->doc);
}

EAPI void * egueb_dom_window_timeout_set(Egueb_Dom_Window *thiz,
		Egueb_Dom_Window_Descriptor_Timeout_Cb cb,
		int64_t delay, void *user_data)
{
	if (!thiz) return NULL;
	if (thiz->desc->timeout_set)
		return thiz->desc->timeout_set(thiz->data, cb, delay, user_data);
	return NULL;
}

EAPI void egueb_dom_window_timeout_clear(Egueb_Dom_Window *thiz,
		void *timeout)
{
	if (!thiz) return;
	if (thiz->desc->timeout_clear)
		thiz->desc->timeout_clear(thiz->data, timeout);
}
