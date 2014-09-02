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
#include "egueb_dom_element_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_CSS_ELEMENT_STYLE_DESCRIPTOR egueb_css_element_style_descriptor_get()
#define EGUEB_CSS_ELEMENT_STYLE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Dom_Element_Style_Class, EGUEB_CSS_ELEMENT_STYLE_DESCRIPTOR)
#define EGUEB_CSS_ELEMENT_STYLE_CLASS_GET(o) EGUEB_CSS_ELEMENT_STYLE_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_CSS_ELEMENT_STYLE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Dom_Element_Style, EGUEB_CSS_ELEMENT_STYLE_DESCRIPTOR)

typedef struct _Egueb_Dom_Element_Style
{
	Egueb_Dom_Element base;
	Egueb_Dom_Node *type;
	Eina_Bool data_changed;
} Egueb_Dom_Element_Style;

typedef struct _Egueb_Dom_Element_Style_Class
{
	Egueb_Dom_Element_Class base;
} Egueb_Dom_Element_Style_Class;

static void _egueb_css_element_style_node_inserted_or_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Element_Style *thiz;
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node *related = NULL;
	Egueb_Dom_Node_Type type;

	related = egueb_dom_event_mutation_related_get(e);
	if (related != n)
	{
		goto not_us;
	}

	target = egueb_dom_event_target_get(e);
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
	Egueb_Dom_Element_Style *thiz;
#if 0
	Egueb_Css_Style *s;
#endif

	thiz = EGUEB_CSS_ELEMENT_STYLE(e);

	/* Check if we need to setup the style again */
	if (!thiz->data_changed)
		return EINA_TRUE;

	/* TODO apply the style on every element */
#if 0
	s = egueb_css_style_load_from_content(cdata, length);
	egueb_svg_element_egueb_css_style_apply(root, thiz->s);
	thiz->data_changed = EINA_FALSE;
#endif

	return EINA_TRUE;
}

static Egueb_Dom_String * _egueb_css_element_style_tag_name_get(Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_CSS_STYLE);
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ELEMENT_DESCRIPTOR,
		Egueb_Dom_Element_Style, Egueb_Dom_Element_Style_Class,
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
	Egueb_Dom_Element_Style *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_CSS_ELEMENT_STYLE(o);
	n = EGUEB_DOM_NODE(o);
	/* add the attributes */
	thiz->type = egueb_dom_attr_string_new(
			egueb_dom_string_ref(EGUEB_DOM_NAME_TYPE),
			NULL, EINA_FALSE, EINA_FALSE, EINA_FALSE);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->type), NULL);

	/* add the events */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_css_element_style_node_inserted_or_removed_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_css_element_style_node_inserted_or_removed_cb,
			EINA_TRUE, thiz);
}

static void _egueb_css_element_style_instance_deinit(void *o)
{
	Egueb_Dom_Element_Style *thiz;

	thiz = EGUEB_CSS_ELEMENT_STYLE(o);
	egueb_dom_node_unref(thiz->type);
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
	Egueb_Dom_Element_Style *thiz;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_css_element_style);
	return EGUEB_DOM_NODE(thiz);
}
