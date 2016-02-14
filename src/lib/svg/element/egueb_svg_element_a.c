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

#include "egueb_svg_main_private.h"
#include "egueb_svg_main.h"
#include "egueb_svg_string.h"
#include "egueb_svg_element_a.h"

#include "egueb_svg_attr_string_private.h"
#include "egueb_svg_renderable_container_private.h"

/* TODO
 * whenever a child is added, if it is of type renderable, then
 * register the click event, whenever the click event occurs, trigger
 * a function on the svg that will inform that the document should
 * go to another location
 * the 'a' can have any child that its parent may container except an 'a'
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_A_DESCRIPTOR egueb_svg_element_a_descriptor_get()
#define EGUEB_SVG_ELEMENT_A_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_A_Class, EGUEB_SVG_ELEMENT_A_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_A(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_A, EGUEB_SVG_ELEMENT_A_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_A
{
	Egueb_Svg_Renderable_Container base;
	/* properties */
	Egueb_Dom_Node *xlink_href;
	/* interface */
	/* private */
	Egueb_Dom_String *gxlink_href;
} Egueb_Svg_Element_A;

typedef struct _Egueb_Svg_Element_A_Class
{
	Egueb_Svg_Renderable_Container_Class base;
} Egueb_Svg_Element_A_Class;


static void _egueb_svg_element_a_renderable_click_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Element_A *thiz = data;
	Egueb_Dom_Event *ev;
	Egueb_Dom_Uri uri;

	DBG("Requesting a new uri '%s'", egueb_dom_string_chars_get(thiz->gxlink_href));
	if (!egueb_dom_uri_string_from(&uri, thiz->gxlink_href))
	{
		ERR("Invalid URI '%s'", egueb_dom_string_chars_get(thiz->gxlink_href));
		return;
	}
	ev = egueb_dom_event_navigation_go_to_new(&uri);
	egueb_dom_event_target_event_dispatch(
			EGUEB_DOM_EVENT_TARGET_CAST(thiz), ev, NULL, NULL);
}

static Eina_Bool _egueb_svg_element_a_get_target(Egueb_Dom_Event *e,
		Egueb_Dom_Node *n, Egueb_Dom_Node **t)
{
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node *related = NULL;

	related = egueb_dom_event_mutation_related_get(e);
	if (related != n)
	{
		egueb_dom_node_unref(related);
		return EINA_FALSE;
	}
	egueb_dom_node_unref(related);
	target = EGUEB_DOM_NODE(egueb_dom_event_target_get(e));
	if (!egueb_svg_is_renderable(target))
	{
		egueb_dom_node_unref(target);
		return EINA_FALSE;
	}
	*t = target;
	return EINA_TRUE;
}

static void _egueb_svg_element_a_node_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *target = NULL;

	if (!_egueb_svg_element_a_get_target(e, n, &target))
		return;
	egueb_dom_event_target_event_listener_add(
			EGUEB_DOM_EVENT_TARGET_CAST(target),
			EGUEB_DOM_EVENT_MOUSE_CLICK,
			_egueb_svg_element_a_renderable_click_cb, EINA_TRUE, n);
	egueb_dom_node_unref(target);
}

static void _egueb_svg_element_a_node_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *target = NULL;

	if (!_egueb_svg_element_a_get_target(e, n, &target))
		return;
	egueb_dom_event_target_event_listener_remove(
			EGUEB_DOM_EVENT_TARGET_CAST(target),
			EGUEB_DOM_EVENT_MOUSE_CLICK,
			_egueb_svg_element_a_renderable_click_cb, EINA_TRUE, n);
	egueb_dom_node_unref(target);
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_a_process(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_A *thiz;
	Egueb_Dom_String *gxlink_href = NULL;

	thiz = EGUEB_SVG_ELEMENT_A(r);
	egueb_dom_attr_final_get(thiz->xlink_href, &gxlink_href);
	if (thiz->gxlink_href)
	{
		egueb_dom_string_unref(thiz->gxlink_href);
		thiz->gxlink_href = NULL;
	}
	thiz->gxlink_href = gxlink_href;

	if (!egueb_svg_renderable_container_process(r))
		return EINA_FALSE;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_a_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_A);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_RENDERABLE_CONTAINER_DESCRIPTOR,
		Egueb_Svg_Element_A, Egueb_Svg_Element_A_Class,
		egueb_svg_element_a);

static void _egueb_svg_element_a_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	klass->process = _egueb_svg_element_a_process;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_a_tag_name_get;
}

static void _egueb_svg_element_a_instance_init(void *o)
{
	Egueb_Svg_Element_A *thiz;
	Egueb_Dom_Node *n;
	Egueb_Dom_Event_Target *evt;

	thiz = EGUEB_SVG_ELEMENT_A(o);
	/* create the properties */
	thiz->xlink_href = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_DOM_NAME_XLINK_HREF),
			NULL);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->xlink_href), NULL);

	evt = EGUEB_DOM_EVENT_TARGET_CAST(o);
	/* whenever a renderable is added/removed, add/remove the click event */
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_element_a_node_inserted_cb,
			EINA_TRUE, o);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_element_a_node_removed_cb,
			EINA_TRUE, o);
}

static void _egueb_svg_element_a_instance_deinit(void *o)
{
	Egueb_Svg_Element_A *thiz;

	thiz = EGUEB_SVG_ELEMENT_A(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->xlink_href);
	if (thiz->gxlink_href)
	{
		egueb_dom_string_unref(thiz->gxlink_href);
		thiz->gxlink_href = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_a_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_a);
	return n;
}

EAPI Eina_Bool egueb_svg_element_is_a(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_A_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI void egueb_svg_element_a_xlink_href_set(Egueb_Dom_Node *n, Egueb_Dom_String *v)
{
	Egueb_Svg_Element_A *thiz;

	thiz = EGUEB_SVG_ELEMENT_A(n);
	egueb_dom_attr_set(thiz->xlink_href, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_a_xlink_href_get(Egueb_Dom_Node *n, Egueb_Svg_String_Animated *v)
{
	Egueb_Svg_Element_A *thiz;

	thiz = EGUEB_SVG_ELEMENT_A(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->xlink_href, v);
}
