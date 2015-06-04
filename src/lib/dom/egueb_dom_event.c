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
#include "egueb_dom_element.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_target.h"

#include "egueb_dom_event_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(ENESIM_OBJECT_DESCRIPTOR, Egueb_Dom_Event,
		Egueb_Dom_Event_Class, egueb_dom_event);

static void _egueb_dom_event_class_init(void *k)
{
}

static void _egueb_dom_event_instance_init(void *o)
{
	Egueb_Dom_Event *thiz;

	thiz = EGUEB_DOM_EVENT(o);
	thiz->ref = 1;
}

static void _egueb_dom_event_instance_deinit(void *o)
{
	Egueb_Dom_Event *thiz;

	thiz = EGUEB_DOM_EVENT(o);
	if (thiz->type)
	{
		egueb_dom_string_unref(thiz->type);
		thiz->type = NULL;
	}
	if (thiz->target)
	{
		egueb_dom_event_target_unref(thiz->target);
		thiz->target = NULL;
	}
	if (thiz->current_target)
	{
		ERR("Current target is still set");
	}
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/* void               stopPropagation(); */
EAPI void egueb_dom_event_stop_propagation(Egueb_Dom_Event *thiz)
{
	if (!thiz) return;
	DBG("Stopping propagation of event '%s'", egueb_dom_string_string_get(thiz->type));
	thiz->stopped = EINA_TRUE;
}

/**
 * Get the phase of the event
 * @ender_prop{phase}
 * @param[in] thiz The event to get the phase from
 * @return The phase of the event
 */
EAPI Egueb_Dom_Event_Phase egueb_dom_event_phase_get(Egueb_Dom_Event *thiz)
{
	return thiz->phase;
}

/**
 * Get the target to which the event was originally dispatched.
 * @ender_prop{target}
 * @param[in] thiz The event to get the target from
 * @return The target
 */
EAPI Egueb_Dom_Event_Target * egueb_dom_event_target_get(Egueb_Dom_Event *thiz)
{
	return egueb_dom_event_target_ref(thiz->target);
}

EAPI Egueb_Dom_Event_Target * egueb_dom_event_target_current_get(Egueb_Dom_Event *thiz)
{
	return egueb_dom_event_target_ref(thiz->current_target);
}

/* void  initEvent(in DOMString eventTypeArg,
                                 in boolean canBubbleArg,
                                 in boolean cancelableArg);
 */
EAPI void egueb_dom_event_init(Egueb_Dom_Event *thiz, Egueb_Dom_String *type,
		Eina_Bool bubbleable, Eina_Bool capturable,
		Eina_Bool cancelable, Egueb_Dom_Event_Direction direction)
{
	thiz->type = type;
	thiz->bubbleable = bubbleable;
	thiz->capturable = capturable;
	thiz->cancelable = cancelable;
	thiz->direction = direction;
}

EAPI void egueb_dom_event_unref(Egueb_Dom_Event *thiz)
{
	thiz->ref--;
	if (!thiz->ref)
	{
		enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
	}
}

EAPI Egueb_Dom_Event * egueb_dom_event_ref(Egueb_Dom_Event *thiz)
{
	thiz->ref++;
	return thiz;
}

/**
 * Get the name of the event
 * @ender_prop{type}
 * @param[in] thiz The event to get the name from
 * @return The name of the event
 */
EAPI Egueb_Dom_String * egueb_dom_event_type_get(Egueb_Dom_Event *thiz)
{
	if (!thiz) return NULL;
	return egueb_dom_string_ref(thiz->type);
}

EAPI Ender_Item * egueb_dom_event_item_get(Egueb_Dom_Event *thiz)
{
	Egueb_Dom_Event_Class *klass;
	Ender_Item *ret = NULL;

	if (!thiz) return NULL;

	klass = EGUEB_DOM_EVENT_CLASS_GET(thiz);
	if (klass->item_get)
		ret = klass->item_get(thiz);
	if (!ret)
	{
		const Ender_Lib *lib;

		lib = ender_lib_find("egueb_dom");
		if (!lib) return NULL;

		ret = ender_lib_item_find(lib, "egueb.dom.event");
	}

	return ret;
}

#if 0
// Introduced in DOM Level 2:
  interface Event {

    readonly attribute DOMTimeStamp     timeStamp;
    void               preventDefault();
  };
#endif
