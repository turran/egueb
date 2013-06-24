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
#include "egueb_svg_element_a.h"
#include "egueb_svg_attr_string.h"
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
} Egueb_Svg_Element_A;

typedef struct _Egueb_Svg_Element_A_Class
{
	Egueb_Svg_Renderable_Container_Class base;
} Egueb_Svg_Element_A_Class;


#if 0
static void _egueb_svg_element_a_renderable_click(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_Element_A *thiz = data;
	Egueb_Svg_Event_Mouse *ev = event_data;
	Ender_Element *svg;

	DBG("Clicking on '%s'", thiz->real_href);
	svg = egueb_svg_element_topmost_get(e);
	egueb_svg_element_svg_go_to(svg, thiz->real_href);
}
#endif
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
#if 0
static Eina_Bool _egueb_svg_element_a_child_add(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element_A *thiz;
	Egueb_Svg_Type type;
	Ender_Element *e;

	thiz = _egueb_svg_element_a_get(t);
	type = egueb_svg_element_internal_type_get(child);
	if (egueb_svg_type_is_animation(type))
		return EINA_TRUE;

	if (!egueb_svg_type_is_renderable(type))
		return EINA_FALSE;

	e = egueb_svg_element_ender_get(child);
	/* add the mouse click event */
	ender_event_listener_add(e, "click", _egueb_svg_element_a_renderable_click, thiz);
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_a_child_remove(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element_A *thiz;
	Egueb_Svg_Type type;
	Ender_Element *e;

	thiz = _egueb_svg_element_a_get(t);
	type = egueb_svg_element_internal_type_get(child);
	if (!egueb_svg_type_is_renderable(type))
		return EINA_TRUE;

	e = egueb_svg_element_ender_get(child);
	/* remove the mouse click event */
	ender_event_listener_remove_full(e, "click", _egueb_svg_element_a_renderable_click, thiz);
	return EINA_TRUE;
}

static void _egueb_svg_element_a_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_A *thiz;

	thiz = _egueb_svg_element_a_get(t);
	free(thiz);
}

static Eina_Bool _egueb_svg_element_a_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "xlink:href") == 0)
	{
		egueb_svg_element_a_xlink_href_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}
	/*
	 * a color and a opacity are part of the presentation attributes
	 * and already parsed on the element
	 */
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_a_attribute_get(Egueb_Dom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static int * _egueb_svg_element_a_attribute_animated_fetch(Egueb_Dom_Tag *t, const char *attr)
{
	Egueb_Svg_Element_A *thiz;
	int *animated = NULL;

	thiz = _egueb_svg_element_a_get(t);
	if (!strcmp(attr, "xlink:href"))
		animated = &thiz->href.animated;
	return animated;
}

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_a_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *parent_context,
		Egueb_Svg_Element_Context *context,
		Egueb_Svg_Element_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element_A *thiz;
	Ender_Element *topmost;
	char *href;
	char *real;

	thiz = _egueb_svg_element_a_get(t);

	egueb_svg_attribute_animated_string_final_get(&thiz->href, &href);
	if (!href) goto done;

	egueb_svg_element_internal_topmost_get(t, &topmost);
	real = egueb_svg_element_svg_uri_resolve(topmost, href);
	if (!real) goto done;

	/* set the resolved uri */
	if (thiz->real_href)
	{
		free(thiz->real_href);
		thiz->real_href = NULL;
	}
	thiz->real_href = real;
done:
	/* call the setup on the children */
	return EINA_TRUE;
}

static Egueb_Svg_Element_Descriptor _descriptor = {
	/* .child_add		= */ _egueb_svg_element_a_child_add,
	/* .child_remove	= */ _egueb_svg_element_a_child_remove,
	/* .attribute_get 	= */ _egueb_svg_element_a_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _egueb_svg_element_a_free,
	/* .attribute_set 	= */ _egueb_svg_element_a_attribute_set,
	/* .attribute_animated_fetch = */ _egueb_svg_element_a_attribute_animated_fetch,
	/* .initialize 		= */ NULL,
	/* .setup		= */ _egueb_svg_element_a_setup,
};
#endif
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
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_a, Egueb_Svg_Element_A, xlink_href);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_RENDERABLE_CONTAINER_DESCRIPTOR,
		Egueb_Svg_Element_A, Egueb_Svg_Element_A_Class,
		egueb_svg_element_a);

static void _egueb_svg_element_a_class_init(void *k)
{
	Egueb_Dom_Element_Class *e_klass;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_a_tag_name_get;
}

static void _egueb_svg_element_a_class_deinit(void *k)
{
}

static void _egueb_svg_element_a_instance_init(void *o)
{
	Egueb_Svg_Element_A *thiz;

	thiz = EGUEB_SVG_ELEMENT_A(o);
	/* create the properties */
	thiz->xlink_href = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_XLINK_HREF),
			NULL, EINA_TRUE, EINA_FALSE, EINA_FALSE);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_a, xlink_href);
}

static void _egueb_svg_element_a_instance_deinit(void *o)
{
	Egueb_Svg_Element_A *thiz;

	thiz = EGUEB_SVG_ELEMENT_A(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->xlink_href);
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

#if 0
EAPI Eina_Bool egueb_svg_is_a(Ender_Element *e)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	return egueb_svg_is_a_internal(t);
}

EAPI void egueb_svg_element_a_xlink_href_set(Ender_Element *e, const char *href)
{
	egueb_svg_element_property_string_set(e, EGUEB_SVG_ELEMENT_A_XLINK_HREF, href);
}
#endif
