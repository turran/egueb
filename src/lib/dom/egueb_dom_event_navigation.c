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
#include "egueb_dom_uri.h"
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_navigation.h"

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_NAVIGATION_DESCRIPTOR egueb_dom_event_navigation_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_navigation_descriptor_get(void);
#define EGUEB_DOM_EVENT_NAVIGATION(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_Navigation, EGUEB_DOM_EVENT_NAVIGATION_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_Navigation
{
	Egueb_Dom_Event base;
	Egueb_Dom_Uri uri;
} Egueb_Dom_Event_Navigation;

typedef struct _Egueb_Dom_Event_Navigation_Class
{
	Egueb_Dom_Event_Class base;
} Egueb_Dom_Event_Navigation_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_NAVIGATION_GO_TO = EGUEB_DOM_STRING_STATIC("NavigationGoTo");

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Dom_Event_Navigation, Egueb_Dom_Event_Navigation_Class,
		egueb_dom_event_navigation);

static void _egueb_dom_event_navigation_class_init(void *k)
{
}

static void _egueb_dom_event_navigation_instance_init(void *o)
{
}

static void _egueb_dom_event_navigation_instance_deinit(void *o)
{
	Egueb_Dom_Event_Navigation *thiz;

	thiz = EGUEB_DOM_EVENT_NAVIGATION(o);
	egueb_dom_uri_cleanup(&thiz->uri);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_NAVIGATION_GO_TO = &_EGUEB_DOM_EVENT_NAVIGATION_GO_TO;

EAPI Egueb_Dom_Event * egueb_dom_event_navigation_go_to_new(
		Egueb_Dom_Uri *uri)
{
	Egueb_Dom_Event_Navigation *thiz;
	Egueb_Dom_Event *e;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_navigation);
	egueb_dom_uri_cleanup(&thiz->uri);
	thiz->uri = *uri;

	e = EGUEB_DOM_EVENT(thiz);
	egueb_dom_event_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_NAVIGATION_GO_TO),
			EINA_TRUE, EINA_TRUE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE); 
	return e;
}

EAPI void egueb_dom_event_navigation_uri_get(Egueb_Dom_Event *e, Egueb_Dom_Uri *uri)
{
	Egueb_Dom_Event_Navigation *thiz;

	thiz = EGUEB_DOM_EVENT_NAVIGATION(e);
	*uri = thiz->uri;
}
