/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#include "egueb_svg_private_attribute_presentation.h"
#include "egueb_svg_context_private.h"
#include "egueb_svg_element_private.h"
#include "egueb_svg_element_a_private.h"
#include "egueb_svg_element_svg_private.h"

#include "egueb_svg_element.h"
#include "egueb_svg_element_a.h"
#include "egueb_svg_event.h"

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
#define ESVG_LOG_DEFAULT _egueb_svg_element_a_log

static int _egueb_svg_element_a_log = -1;

static Ender_Property *ESVG_ELEMENT_A_XLINK_HREF;

typedef struct _Egueb_Svg_A
{
	/* properties */
	Egueb_Svg_Attribute_Animated_String href;
	/* interface */
	/* private */
	char *real_href;
} Egueb_Svg_A;

static Egueb_Svg_A * _egueb_svg_element_a_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_A *thiz;

	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_A)
		return NULL;
	thiz = egueb_svg_element_data_get(t);

	return thiz;
}

static void _egueb_svg_element_a_renderable_click(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Egueb_Svg_A *thiz = data;
	Egueb_Svg_Event_Mouse *ev = event_data;
	Ender_Element *svg;

	DBG("Clicking on '%s'", thiz->real_href);
	svg = egueb_svg_element_topmost_get(e);
	egueb_svg_element_svg_go_to(svg, thiz->real_href);
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_a_child_add(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_A *thiz;
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
	Egueb_Svg_A *thiz;
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
	Egueb_Svg_A *thiz;

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
	Egueb_Svg_A *thiz;
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
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_A *thiz;
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
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_svg_element_a_new(void)
{
	Egueb_Svg_A *thiz;
	Egueb_Dom_Tag *t;

	thiz = calloc(1, sizeof(Egueb_Svg_A));
	if (!thiz) return NULL;

	/* default values */

	t = egueb_svg_element_new(&_descriptor, ESVG_TYPE_A, thiz);
	return t;
}

static void _egueb_svg_element_a_xlink_href_set(Egueb_Dom_Tag *t, Egueb_Svg_String_Animated *href)
{
	Egueb_Svg_A *thiz;
	Eina_Bool animating;

	thiz = _egueb_svg_element_a_get(t);
	animating = egueb_svg_element_attribute_animate_get(t);
	egueb_svg_attribute_animated_string_set(&thiz->href,
		href, animating);
}

static void _egueb_svg_element_a_xlink_href_get(Egueb_Dom_Tag *t, Egueb_Svg_String_Animated *href)
{
	Egueb_Svg_A *thiz;

	thiz = _egueb_svg_element_a_get(t);
	egueb_svg_attribute_animated_string_get(&thiz->href,
		href);
}

/* The ender wrapper */
#define _egueb_svg_element_a_xlink_href_is_set NULL
#define _egueb_svg_element_a_delete NULL
#include "egueb_svg_generated_element_a.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_is_a_internal(Egueb_Dom_Tag *t)
{
	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_A)
		return EINA_FALSE;
	return EINA_TRUE;
}

void egueb_svg_element_a_init(void)
{
	_egueb_svg_element_a_log = eina_log_domain_register("egueb_svg_a", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_element_a_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_element_a_init();
}

void egueb_svg_element_a_shutdown(void)
{
	if (_egueb_svg_element_a_log < 0)
		return;
	_egueb_svg_element_a_shutdown();
	eina_log_domain_unregister(_egueb_svg_element_a_log);
	_egueb_svg_element_a_log = -1;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * egueb_svg_element_a_new(void)
{
	return ESVG_ELEMENT_NEW("SVGAElement");
}

EAPI Eina_Bool egueb_svg_is_a(Ender_Element *e)
{
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	return egueb_svg_is_a_internal(t);
}

EAPI void egueb_svg_element_a_xlink_href_set(Ender_Element *e, const char *href)
{
	egueb_svg_element_property_string_set(e, ESVG_ELEMENT_A_XLINK_HREF, href);
}
