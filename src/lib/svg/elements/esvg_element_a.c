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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_element_private.h"
#include "esvg_element_a_private.h"
#include "esvg_element_svg_private.h"

#include "esvg_element.h"
#include "esvg_element_a.h"
#include "esvg_event.h"

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
#define ESVG_LOG_DEFAULT _esvg_element_a_log

static int _esvg_element_a_log = -1;

static Ender_Property *ESVG_ELEMENT_A_XLINK_HREF;

typedef struct _Esvg_A
{
	/* properties */
	Esvg_Attribute_Animated_String href;
	/* interface */
	/* private */
	char *real_href;
} Esvg_A;

static Esvg_A * _esvg_element_a_get(Edom_Tag *t)
{
	Esvg_A *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TYPE_A)
		return NULL;
	thiz = esvg_element_data_get(t);

	return thiz;
}

static void _esvg_element_a_renderable_click(Ender_Element *e,
		const char *event_name, void *event_data, void *data)
{
	Esvg_A *thiz = data;
	Esvg_Event_Mouse *ev = event_data;
	Ender_Element *svg;

	DBG("Clicking on '%s'", thiz->real_href);
	svg = esvg_element_topmost_get(e);
	esvg_element_svg_go_to(svg, thiz->real_href);
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_element_a_child_add(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_A *thiz;
	Esvg_Type type;
	Ender_Element *e;

	thiz = _esvg_element_a_get(t);
	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_animation(type))
		return EINA_TRUE;

	if (!esvg_type_is_renderable(type))
		return EINA_FALSE;

	e = esvg_element_ender_get(child);
	/* add the mouse click event */
	ender_event_listener_add(e, "click", _esvg_element_a_renderable_click, thiz);
	return EINA_TRUE;
}

static Eina_Bool _esvg_element_a_child_remove(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_A *thiz;
	Esvg_Type type;
	Ender_Element *e;

	thiz = _esvg_element_a_get(t);
	type = esvg_element_internal_type_get(child);
	if (!esvg_type_is_renderable(type))
		return EINA_TRUE;

	e = esvg_element_ender_get(child);
	/* remove the mouse click event */
	ender_event_listener_remove_full(e, "click", _esvg_element_a_renderable_click, thiz);
	return EINA_TRUE;
}

static void _esvg_element_a_free(Edom_Tag *t)
{
	Esvg_A *thiz;

	thiz = _esvg_element_a_get(t);
	free(thiz);
}

static Eina_Bool _esvg_element_a_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "xlink:href") == 0)
	{
		esvg_element_a_xlink_href_set(e, value);
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

static Eina_Bool _esvg_element_a_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static int * _esvg_element_a_attribute_animated_fetch(Edom_Tag *t, const char *attr)
{
	Esvg_A *thiz;
	int *animated = NULL;

	thiz = _esvg_element_a_get(t);
	if (!strcmp(attr, "xlink:href"))
		animated = &thiz->href.animated;
	return animated;
}

static Esvg_Element_Setup_Return _esvg_element_a_setup(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Esvg_A *thiz;
	Ender_Element *topmost;
	char *href;
	char *real;

	thiz = _esvg_element_a_get(t);

	esvg_attribute_animated_string_final_get(&thiz->href, &href);
	if (!href) goto done;

	esvg_element_internal_topmost_get(t, &topmost);
	real = esvg_element_svg_uri_resolve(topmost, href);
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

static Esvg_Element_Descriptor _descriptor = {
	/* .child_add		= */ _esvg_element_a_child_add,
	/* .child_remove	= */ _esvg_element_a_child_remove,
	/* .attribute_get 	= */ _esvg_element_a_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _esvg_element_a_free,
	/* .attribute_set 	= */ _esvg_element_a_attribute_set,
	/* .attribute_animated_fetch = */ _esvg_element_a_attribute_animated_fetch,
	/* .initialize 		= */ NULL,
	/* .setup		= */ _esvg_element_a_setup,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_a_new(void)
{
	Esvg_A *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_A));
	if (!thiz) return NULL;

	/* default values */

	t = esvg_element_new(&_descriptor, ESVG_TYPE_A, thiz);
	return t;
}

static void _esvg_element_a_xlink_href_set(Edom_Tag *t, Esvg_String_Animated *href)
{
	Esvg_A *thiz;
	Eina_Bool animating;

	thiz = _esvg_element_a_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_string_set(&thiz->href,
		href, animating);
}

static void _esvg_element_a_xlink_href_get(Edom_Tag *t, Esvg_String_Animated *href)
{
	Esvg_A *thiz;

	thiz = _esvg_element_a_get(t);
	esvg_attribute_animated_string_get(&thiz->href,
		href);
}

/* The ender wrapper */
#define _esvg_element_a_xlink_href_is_set NULL
#define _esvg_element_a_delete NULL
#include "esvg_generated_element_a.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool esvg_is_a_internal(Edom_Tag *t)
{
	if (esvg_element_internal_type_get(t) != ESVG_TYPE_A)
		return EINA_FALSE;
	return EINA_TRUE;
}

void esvg_element_a_init(void)
{
	_esvg_element_a_log = eina_log_domain_register("esvg_a", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_a_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_a_init();
}

void esvg_element_a_shutdown(void)
{
	if (_esvg_element_a_log < 0)
		return;
	_esvg_element_a_shutdown();
	eina_log_domain_unregister(_esvg_element_a_log);
	_esvg_element_a_log = -1;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_element_a_new(void)
{
	return ESVG_ELEMENT_NEW("SVGAElement");
}

EAPI Eina_Bool esvg_is_a(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_a_internal(t);
}

EAPI void esvg_element_a_xlink_href_set(Ender_Element *e, const char *href)
{
	esvg_element_property_string_set(e, ESVG_ELEMENT_A_XLINK_HREF, href);
}
