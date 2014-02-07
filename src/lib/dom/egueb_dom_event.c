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
#include "egueb_dom_element.h"
#include "egueb_dom_event.h"

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
	if (!thiz->cancelable)
	{
		DBG("Impossible to stop a not cancelable event '%s'", egueb_dom_string_string_get(thiz->type));
		return;
	}
	DBG("Stopping propagation of event '%s'", egueb_dom_string_string_get(thiz->type));
	thiz->stopped = EINA_TRUE;
}

EAPI Egueb_Dom_Event_Phase egueb_dom_event_phase_get(Egueb_Dom_Event *thiz)
{
	return thiz->phase;
}

EAPI Egueb_Dom_Node * egueb_dom_event_target_get(Egueb_Dom_Event *thiz)
{
	return egueb_dom_node_ref(thiz->target);
}

EAPI Egueb_Dom_Node * egueb_dom_event_target_current_get(Egueb_Dom_Event *thiz)
{
	return egueb_dom_node_ref(thiz->current_target);
}

/* void  initEvent(in DOMString eventTypeArg,
                                 in boolean canBubbleArg,
                                 in boolean cancelableArg);
 */
EAPI void egueb_dom_event_init(Egueb_Dom_Event *thiz, Egueb_Dom_String *type,
		Eina_Bool bubbleable, Eina_Bool capturable,
		Eina_Bool cancelable, Egueb_Dom_Event_Direction direction)
{
	_egueb_dom_event_instance_init(thiz);
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
#if 0
// Introduced in DOM Level 2:
  interface Event {

    readonly attribute DOMTimeStamp     timeStamp;
    void               preventDefault();
  };
#endif
