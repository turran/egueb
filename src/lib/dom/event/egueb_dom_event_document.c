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
#include "egueb_dom_window.h"
#include "egueb_dom_event_ui.h"

#include "egueb_dom_string_private.h"
#include "egueb_dom_event_document.h"
#include "egueb_dom_event_ui_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_EVENT_DOCUMENT_DESCRIPTOR egueb_dom_event_document_descriptor_get()
Enesim_Object_Descriptor * egueb_dom_event_document_descriptor_get(void);

#define EGUEB_DOM_EVENT_DOCUMENT_CLASS_GET(o) EGUEB_DOM_EVENT_DOCUMENT_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_EVENT_DOCUMENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Event_Document_Class,					\
		EGUEB_DOM_EVENT_DOCUMENT_DESCRIPTOR)
#define EGUEB_DOM_EVENT_DOCUMENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Event_Document, EGUEB_DOM_EVENT_DOCUMENT_DESCRIPTOR)

typedef struct _Egueb_Dom_Event_Document
{
	Egueb_Dom_Event base;
	Egueb_Dom_Node *related;
} Egueb_Dom_Event_Document;

typedef struct _Egueb_Dom_Event_Document_Class
{
	Egueb_Dom_Event_Class base;
} Egueb_Dom_Event_Document_Class;

static Egueb_Dom_String _EGUEB_DOM_EVENT_DOCUMENT_ID_INSERTED = EGUEB_DOM_STRING_STATIC("DocumentIdInserted");
static Egueb_Dom_String _EGUEB_DOM_EVENT_DOCUMENT_ID_REMOVED = EGUEB_DOM_STRING_STATIC("DocumentIdRemoved");
/*----------------------------------------------------------------------------*
 *                               Event interface                              *
 *----------------------------------------------------------------------------*/
static Ender_Item * _egueb_dom_event_document_item_get(Egueb_Dom_Event *thiz)
{
	const Ender_Lib *lib;

	lib = ender_lib_find("egueb_dom");
	return ender_lib_item_find(lib, "egueb.dom.event.document");
}

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_EVENT_DESCRIPTOR,
		Egueb_Dom_Event_Document, Egueb_Dom_Event_Document_Class,
		egueb_dom_event_document);

static void _egueb_dom_event_document_class_init(void *k)
{
	Egueb_Dom_Event_Class *klass;

	klass = EGUEB_DOM_EVENT_CLASS(k);
	klass->item_get = _egueb_dom_event_document_item_get;
}

static void _egueb_dom_event_document_instance_init(void *o)
{
}

static void _egueb_dom_event_document_instance_deinit(void *o)
{
	Egueb_Dom_Event_Document *thiz;

	thiz = EGUEB_DOM_EVENT_DOCUMENT(o);
	egueb_dom_node_unref(thiz->related);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
EAPI Egueb_Dom_Event * egueb_dom_event_document_id_inserted_new(Egueb_Dom_Node *n)
{
	Egueb_Dom_Event_Document *thiz;
	Egueb_Dom_Event *e;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_document);
	thiz->related = n;

	e = EGUEB_DOM_EVENT(thiz);
	egueb_dom_event_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_DOCUMENT_ID_INSERTED),
			EINA_FALSE, EINA_FALSE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
	return e;
}

EAPI Egueb_Dom_Event * egueb_dom_event_document_id_removed_new(Egueb_Dom_Node *n)
{
	Egueb_Dom_Event_Document *thiz;
	Egueb_Dom_Event *e;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_event_document);
	thiz->related = n;

	e = EGUEB_DOM_EVENT(thiz);
	egueb_dom_event_init(e, egueb_dom_string_ref(EGUEB_DOM_EVENT_DOCUMENT_ID_REMOVED),
			EINA_FALSE, EINA_FALSE, EINA_FALSE,
			EGUEB_DOM_EVENT_DIRECTION_CAPTURE_BUBBLE);
	return e;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_EVENT_DOCUMENT_ID_INSERTED = &_EGUEB_DOM_EVENT_DOCUMENT_ID_INSERTED;
Egueb_Dom_String *EGUEB_DOM_EVENT_DOCUMENT_ID_REMOVED = &_EGUEB_DOM_EVENT_DOCUMENT_ID_REMOVED;

EAPI Egueb_Dom_Node * egueb_dom_event_document_related_get(Egueb_Dom_Event *e)
{
	Egueb_Dom_Event_Document *thiz;

	thiz = EGUEB_DOM_EVENT_DOCUMENT(e);
	return egueb_dom_node_ref(thiz->related);

}
