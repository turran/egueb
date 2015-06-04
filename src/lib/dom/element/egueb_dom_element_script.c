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
#include "egueb_dom_uri.h"
#include "egueb_dom_element.h"
#include "egueb_dom_scripter.h"
#include "egueb_dom_scripter.h"
#include "egueb_dom_attr_string.h"
#include "egueb_dom_event_mutation.h"
#include "egueb_dom_event_script.h"
#include "egueb_dom_event_io.h"
#include "egueb_dom_element_script.h"
#include "egueb_dom_character_data.h"

#include "egueb_dom_element_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_ELEMENT_SCRIPT_DESCRIPTOR egueb_dom_element_script_descriptor_get()
#define EGUEB_DOM_ELEMENT_SCRIPT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Dom_Element_Script_Class, EGUEB_DOM_ELEMENT_SCRIPT_DESCRIPTOR)
#define EGUEB_DOM_ELEMENT_SCRIPT_CLASS_GET(o) EGUEB_DOM_ELEMENT_SCRIPT_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_ELEMENT_SCRIPT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Dom_Element_Script, EGUEB_DOM_ELEMENT_SCRIPT_DESCRIPTOR)

typedef struct _Egueb_Dom_Element_Script
{
	Egueb_Dom_Element base;
	Egueb_Dom_Node *type;
	Egueb_Dom_Node *src;

	Egueb_Dom_String *last_type;
	Egueb_Dom_String *last_src;
	Egueb_Dom_Scripter *scripter;
	void *script;
	Eina_Bool data_changed;
} Egueb_Dom_Element_Script;

typedef struct _Egueb_Dom_Element_Script_Class
{
	Egueb_Dom_Element_Class base;
} Egueb_Dom_Element_Script_Class;

static void _egueb_dom_element_script_node_inserted_or_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Element_Script *thiz;
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node *related = NULL;
	Egueb_Dom_Node_Type type;

	related = egueb_dom_event_mutation_related_get(e);
	if (related != n)
	{
		goto not_us;
	}

	target = EGUEB_DOM_NODE(egueb_dom_event_target_get(e));
	type = egueb_dom_node_type_get(target);
	/* for svg it must be a cdata section, but some svg's has the script
	 * on a text node
	 */
	if (type != EGUEB_DOM_NODE_TYPE_CDATA_SECTION && type != EGUEB_DOM_NODE_TYPE_TEXT)
	{
		goto not_data;
	}

	/* finally inform ourselves that the cdata has changed and so, we need
	 * to create a new vm
	 */
	thiz = EGUEB_DOM_ELEMENT_SCRIPT(n);
	thiz->data_changed = EINA_TRUE;
not_data:
	egueb_dom_node_unref(target);
not_us:
	egueb_dom_node_unref(related);
}

static void _egueb_dom_element_script_run(Egueb_Dom_Element_Script *thiz,
		Egueb_Dom_String *data)
{
	Egueb_Dom_Node *doc;

	/* compile the script */
	if (!egueb_dom_scripter_load(thiz->scripter, data, &thiz->script))
	{
		WARN("Failed to load the script");
		goto done;
	}
	/* add the global variables */
	doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(thiz));
	egueb_dom_scripter_global_add(thiz->scripter, "document", doc, egueb_dom_node_item_get(doc));
	/* run it */
	egueb_dom_scripter_script_run(thiz->scripter, thiz->script);
done:
	egueb_dom_string_unref(data);
}

static void _egueb_dom_element_script_data_cb(Egueb_Dom_Node *n,
		Enesim_Stream *s)
{
	Egueb_Dom_Element_Script *thiz;
	Egueb_Dom_String *data;
	size_t size;
	void *sdata;

	thiz = EGUEB_DOM_ELEMENT_SCRIPT(n);

	sdata = enesim_stream_mmap(s, &size);
	data = egueb_dom_string_new_with_static_string(sdata);
	_egueb_dom_element_script_run(thiz, data);
	egueb_dom_string_unref(data);
	enesim_stream_munmap(s, sdata);
	enesim_stream_unref(s);
}

/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_element_script_process(Egueb_Dom_Element *e)
{
	Egueb_Dom_Element_Script *thiz;
	Egueb_Dom_String *type = NULL;
	Egueb_Dom_String *src = NULL;
	Eina_Bool type_changed = EINA_FALSE;

	thiz = EGUEB_DOM_ELEMENT_SCRIPT(e);

	/* Check if we need to setup the script again */
	egueb_dom_attr_final_get(thiz->type, &type);
	if (!egueb_dom_string_is_equal(type, thiz->last_type))
		type_changed = EINA_TRUE;

	egueb_dom_attr_final_get(thiz->src, &src);
	if (!egueb_dom_string_is_equal(src, thiz->last_src))
		thiz->data_changed = EINA_TRUE;

	if (!thiz->data_changed && !type_changed)
		goto done;

	if (thiz->scripter)
	{
		ERR("Impossible to re-generate the whole scripting VM");
	}
	else
	{
		Egueb_Dom_Event *ev;
		Egueb_Dom_String *final_type;

		INFO("Processing the script");
		if (!type)
			final_type = egueb_dom_string_new_with_static_string("application/ecmascript");
		else
			final_type = egueb_dom_string_ref(type);

		ev = egueb_dom_event_script_new(final_type);
		egueb_dom_event_target_event_dispatch(EGUEB_DOM_EVENT_TARGET(e),
				egueb_dom_event_ref(ev), NULL, NULL);
		/* instantiate the vm */
		thiz->scripter = egueb_dom_event_script_scripter_get(ev);
		egueb_dom_event_unref(ev);

		if (thiz->scripter)
		{
			Egueb_Dom_Node *data;

			data = egueb_dom_node_child_first_get(EGUEB_DOM_NODE(e));
			if (data)
			{
				Egueb_Dom_String *data_txt;

				INFO("Using the text node");
				data_txt = egueb_dom_character_data_data_get(data);
				_egueb_dom_element_script_run(thiz, data_txt);
			}
			/* check if we have a a src attribute */
			else
			{
				Egueb_Dom_Event *e;
				Egueb_Dom_Uri u;

				if (!egueb_dom_uri_string_from(&u, src))
				{
					ERR_ELEMENT(EGUEB_DOM_NODE(thiz), "Wrong src");
					goto failed;
				}

				INFO("Request a load of '%s'", egueb_dom_string_string_get(src));
				e = egueb_dom_event_io_data_new(&u, _egueb_dom_element_script_data_cb);
				egueb_dom_event_target_event_dispatch(
						EGUEB_DOM_EVENT_TARGET(thiz),
						e, NULL, NULL);
				egueb_dom_uri_cleanup(&u);
			}
		}
	}


failed:
	thiz->data_changed = EINA_FALSE;
	/* swap the last content type */
	if (thiz->last_type)
		egueb_dom_string_unref(thiz->last_type);
	thiz->last_type = egueb_dom_string_ref(type);
	/* swap the last src */
	if (thiz->last_src)
		egueb_dom_string_unref(thiz->last_src);
	thiz->last_src = egueb_dom_string_ref(src);
done:
	egueb_dom_string_unref(src);
	egueb_dom_string_unref(type);
	return EINA_TRUE;
}

static Egueb_Dom_String * _egueb_dom_element_script_tag_name_get(Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_DOM_NAME_SCRIPT);
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ELEMENT_DESCRIPTOR,
		Egueb_Dom_Element_Script, Egueb_Dom_Element_Script_Class,
		egueb_dom_element_script);

static void _egueb_dom_element_script_class_init(void *k)
{
	Egueb_Dom_Element_Class *klass;

	klass = EGUEB_DOM_ELEMENT_CLASS(k);
	klass->tag_name_get = _egueb_dom_element_script_tag_name_get;
	klass->process = _egueb_dom_element_script_process;
}

static void _egueb_dom_element_script_instance_init(void *o)
{
	Egueb_Dom_Element_Script *thiz;
	Egueb_Dom_Node *n;
	Egueb_Dom_Event_Target *evt;

	thiz = EGUEB_DOM_ELEMENT_SCRIPT(o);
	n = EGUEB_DOM_NODE(o);
	/* add the attributes */
	thiz->type = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_DOM_NAME_TYPE),
			NULL, NULL, EINA_FALSE, EINA_FALSE, EINA_FALSE);
	thiz->src = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_DOM_NAME_SRC),
			NULL, NULL, EINA_FALSE, EINA_FALSE, EINA_FALSE);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->type), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->src), NULL);

	/* add the events */
	evt = EGUEB_DOM_EVENT_TARGET(o);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_dom_element_script_node_inserted_or_removed_cb,
			EINA_TRUE, thiz);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_dom_element_script_node_inserted_or_removed_cb,
			EINA_TRUE, thiz);
}

static void _egueb_dom_element_script_instance_deinit(void *o)
{
	Egueb_Dom_Element_Script *thiz;

	thiz = EGUEB_DOM_ELEMENT_SCRIPT(o);
	if (thiz->scripter)
	{
		egueb_dom_scripter_script_free(thiz->scripter, thiz->script);
	}
	egueb_dom_node_unref(thiz->type);
	egueb_dom_node_unref(thiz->src);

	if (thiz->last_type)
		egueb_dom_string_unref(thiz->last_type);
	if (thiz->last_src)
		egueb_dom_string_unref(thiz->last_src);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_dom_element_is_script(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_DOM_ELEMENT_SCRIPT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Egueb_Dom_Node * egueb_dom_element_script_new(void)
{
	Egueb_Dom_Element_Script *thiz;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_element_script);
	return EGUEB_DOM_NODE(thiz);
}
