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

#include "egueb_css_private.h"
#include "egueb_css_main.h"

#include "egueb_css_engine_selector_private.h"
#include "egueb_css_engine_rule_private.h"
#include "egueb_css_engine_style_private.h"

#include "egueb_dom_element_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_CSS_ELEMENT_STYLE_DESCRIPTOR egueb_css_element_style_descriptor_get()
#define EGUEB_CSS_ELEMENT_STYLE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Css_Element_Style_Class, EGUEB_CSS_ELEMENT_STYLE_DESCRIPTOR)
#define EGUEB_CSS_ELEMENT_STYLE_CLASS_GET(o) EGUEB_CSS_ELEMENT_STYLE_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_CSS_ELEMENT_STYLE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Css_Element_Style, EGUEB_CSS_ELEMENT_STYLE_DESCRIPTOR)

typedef struct _Egueb_Css_Element_Style
{
	Egueb_Dom_Element base;
	/* properties */
	Egueb_Dom_Node *type;
	/* private */
	Egueb_Dom_String *last_data;
	int last_length;
	Eina_Bool data_changed;
} Egueb_Css_Element_Style;

typedef struct _Egueb_Css_Element_Style_Class
{
	Egueb_Dom_Element_Class base;
} Egueb_Css_Element_Style_Class;

Enesim_Object_Descriptor * egueb_css_element_style_descriptor_get(void);

static Eina_Bool _egueb_css_element_is_style(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_CSS_ELEMENT_STYLE_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

static Eina_Bool _egueb_css_element_style_apply(Egueb_Dom_Node *n,
		Egueb_Dom_Node *topmost)
{
	Egueb_Css_Element_Style *thiz;
	Egueb_Dom_Node *data;

	thiz = EGUEB_CSS_ELEMENT_STYLE(n);

	/* Un apply previous style */
	if (thiz->last_data)
	{
		Egueb_Css_Engine_Style *s;

		DBG_ELEMENT (n, "Unapplying style element");
		s = egueb_css_engine_style_load_from_content(
				egueb_dom_string_string_get(thiz->last_data),
				thiz->last_length);
		egueb_css_engine_style_unapply(s, topmost);
		egueb_css_engine_style_free(s);
	}
	/* Apply the style */
	data = egueb_dom_node_child_first_get(n);
	if (data)
	{
		Egueb_Css_Engine_Style *s;
		Egueb_Dom_String *str;
		int length;

		DBG_ELEMENT (n, "Applying style element");
		str = egueb_dom_character_data_data_get(data);
		length = egueb_dom_character_data_length_get(data);
		s = egueb_css_engine_style_load_from_content(
				egueb_dom_string_string_get(str), length);
		egueb_css_engine_style_apply(s, topmost);
		egueb_css_engine_style_free(s);

		/* keep track of the last data */
		thiz->last_length = length;
		if (thiz->last_data)
			egueb_dom_string_unref(thiz->last_data);
		thiz->last_data = str;

		egueb_dom_node_unref(data);
	}
	/* do not process it again */
	thiz->data_changed = EINA_FALSE;

	return EINA_TRUE;
}

/* Iterate over every css style element and unapply/apply on each of
 * them. The thing is that if some style changes its content, we need
 * to revert its previous style apply back and so on
 */
static void _egueb_css_element_style_apply_recursive(Egueb_Dom_Node *n,
		Egueb_Dom_Node *topmost)
{
	Egueb_Dom_Node *child;

	child = egueb_dom_element_child_first_get(n);
	while (child)
	{
		Egueb_Dom_Node *tmp;

		if (_egueb_css_element_is_style(child))
			_egueb_css_element_style_apply (child, topmost);
		else
			_egueb_css_element_style_apply_recursive(child, topmost);
		tmp = egueb_dom_element_sibling_next_get(child);
		egueb_dom_node_unref(child);
		child = tmp;
	}
}

static void _egueb_css_element_style_character_data_modified_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Css_Element_Style *thiz;

	thiz = EGUEB_CSS_ELEMENT_STYLE(data);
	thiz->data_changed = EINA_TRUE;
}

static void _egueb_css_element_style_node_inserted_or_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Css_Element_Style *thiz;
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
	/* for svg it must be a cdata section, but some svg's has the style
	 * on a text node
	 */
	if (type != EGUEB_DOM_NODE_TYPE_CDATA_SECTION && type != EGUEB_DOM_NODE_TYPE_TEXT)
	{
		goto not_data;
	}

	/* finally inform ourselves that the cdata has changed  to apply the style */
	thiz = EGUEB_CSS_ELEMENT_STYLE(n);
	thiz->data_changed = EINA_TRUE;
not_data:
	egueb_dom_node_unref(target);
not_us:
	egueb_dom_node_unref(related);
}
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_css_element_style_process(Egueb_Dom_Element *e)
{
	Egueb_Css_Element_Style *thiz;
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *topmost;
	Egueb_Dom_Node *n;

	/* only process every css style element, for the first
	 * element that actually needs to be processed
	 */
	thiz = EGUEB_CSS_ELEMENT_STYLE(e);
	if (!thiz->data_changed)
		return EINA_TRUE;


	n = EGUEB_DOM_NODE(e);
	doc = egueb_dom_node_owner_document_get(n);
	if (!doc)
		return EINA_FALSE;

	topmost = egueb_dom_document_document_element_get(doc);
	if (!topmost)
	{
		egueb_dom_node_unref(doc);
		return EINA_FALSE;
	}

	_egueb_css_element_style_apply_recursive(topmost, topmost);
	egueb_dom_node_unref(topmost);
	egueb_dom_node_unref(doc);

	return EINA_TRUE;
}

static Egueb_Dom_String * _egueb_css_element_style_tag_name_get(Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_CSS_NAME_STYLE);
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ELEMENT_DESCRIPTOR,
		Egueb_Css_Element_Style, Egueb_Css_Element_Style_Class,
		egueb_css_element_style);

static void _egueb_css_element_style_class_init(void *k)
{
	Egueb_Dom_Element_Class *klass;

	klass = EGUEB_DOM_ELEMENT_CLASS(k);
	klass->tag_name_get = _egueb_css_element_style_tag_name_get;
	klass->process = _egueb_css_element_style_process;
}

static void _egueb_css_element_style_instance_init(void *o)
{
	Egueb_Css_Element_Style *thiz;
	Egueb_Dom_Event_Target *evt;
	Egueb_Dom_Node *n;

	thiz = EGUEB_CSS_ELEMENT_STYLE(o);
	n = EGUEB_DOM_NODE(o);
	/* add the attributes */
	thiz->type = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_DOM_NAME_TYPE),
			NULL, NULL, EINA_FALSE, EINA_FALSE, EINA_FALSE);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->type), NULL);

	/* add the events */
	evt = EGUEB_DOM_EVENT_TARGET(n);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_css_element_style_node_inserted_or_removed_cb,
			EINA_TRUE, thiz);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_css_element_style_node_inserted_or_removed_cb,
			EINA_TRUE, thiz);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_CHARACTER_DATA_MODIFIED,
			_egueb_css_element_style_character_data_modified_cb,
			EINA_TRUE, n);
}

static void _egueb_css_element_style_instance_deinit(void *o)
{
	Egueb_Css_Element_Style *thiz;

	thiz = EGUEB_CSS_ELEMENT_STYLE(o);
	egueb_dom_node_unref(thiz->type);
	if (thiz->last_data)
		egueb_dom_string_unref(thiz->last_data);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_css_element_is_style(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_CSS_ELEMENT_STYLE_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Egueb_Dom_Node * egueb_css_element_style_new(void)
{
	Egueb_Css_Element_Style *thiz;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_css_element_style);
	return EGUEB_DOM_NODE(thiz);
}
