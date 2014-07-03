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
#include "egueb_dom_scripter.h"
#include "egueb_dom_event.h"
#include "egueb_dom_event_script.h"

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_SCRIPT_DESCRIPTOR egueb_dom_event_script_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_script_descriptor_get(void);
#define EGUEB_DOM_EVENT_SCRIPT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_Script, EGUEB_DOM_EVENT_SCRIPT_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_Script
{
	Egueb_Dom_Event base;
	Egueb_Dom_String *type;
	Egueb_Dom_Scripter *scripter;
} Egueb_Dom_Event_Script;

typedef struct _Egueb_Dom_Event_Script_Class
{
	Egueb_Dom_Event_Class base;
} Egueb_Dom_Event_Script_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_SCRIPT_SCRIPTER = EGUEB_DOM_STRING_STATIC("ScriptScripter");

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Dom_Event_Script, Egueb_Dom_Event_Script_Class,
		egueb_dom_event_script);

static void _egueb_dom_event_script_class_init(void *k)
{
}

static void _egueb_dom_event_script_instance_init(void *o)
{
}

static void _egueb_dom_event_script_instance_deinit(void *o)
{
	Egueb_Dom_Event_Script *thiz;

	thiz = EGUEB_DOM_EVENT_SCRIPT(o);
	egueb_dom_string_unref(thiz->type);
}

static Egueb_Dom_Event * _egueb_dom_event_script_new(Egueb_Dom_String *type)
{
	Egueb_Dom_Event_Script *thiz;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_script);
	thiz->type = type;

	return EGUEB_DOM_EVENT(thiz);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_SCRIPT_SCRIPTER = &_EGUEB_DOM_EVENT_SCRIPT_SCRIPTER;

EAPI Egueb_Dom_Event * egueb_dom_event_script_new(Egueb_Dom_String *type)
{
	Egueb_Dom_Event *e;

	if (!type) return NULL;

	e = _egueb_dom_event_script_new(type);
	egueb_dom_event_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_SCRIPT_SCRIPTER),
			EINA_TRUE, EINA_TRUE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE); 
	return e;
}

EAPI Egueb_Dom_String * egueb_dom_event_script_type_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Script *thiz;

	thiz = EGUEB_DOM_EVENT_SCRIPT(e);
	return egueb_dom_string_ref(thiz->type);
}

EAPI Egueb_Dom_Scripter * egueb_dom_event_script_scripter_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Script *thiz;

	thiz = EGUEB_DOM_EVENT_SCRIPT(e);
	return thiz->scripter;
}

EAPI void egueb_dom_event_script_scripter_set(Egueb_Dom_Event *e, Egueb_Dom_Scripter *sc)
{
	Egueb_Dom_Event_Script *thiz;

	thiz = EGUEB_DOM_EVENT_SCRIPT(e);
	thiz->scripter = sc;
}
