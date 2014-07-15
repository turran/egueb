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
#include "egueb_svg_length.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_element.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_filter.h"
#include "egueb_svg_document.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"

#include "egueb_svg_referenceable_private.h"
#include "egueb_svg_attr_string_private.h"
#include "egueb_svg_attr_length_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_FILTER_DESCRIPTOR egueb_svg_element_filter_descriptor_get()
#define EGUEB_SVG_ELEMENT_FILTER_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Filter_Class, EGUEB_SVG_ELEMENT_FILTER_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_FILTER(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Filter, EGUEB_SVG_ELEMENT_FILTER_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Filter
{
	Egueb_Svg_Referenceable base;
	/* properties */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	Egueb_Dom_Node *width;
	Egueb_Dom_Node *height;
	Egueb_Dom_Node *xlink_href;
	/* private */
} Egueb_Svg_Element_Filter;

typedef struct _Egueb_Svg_Element_Filter_Class
{
	Egueb_Svg_Referenceable_Class base;
} Egueb_Svg_Element_Filter_Class;

/*----------------------------------------------------------------------------*
 *                           Referenceable interface                          *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_filter_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_RECT);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_REFERENCEABLE_DESCRIPTOR,
		Egueb_Svg_Element_Filter, Egueb_Svg_Element_Filter_Class,
		egueb_svg_element_filter);

static void _egueb_svg_element_filter_class_init(void *k)
{
	Egueb_Dom_Element_Class *e_klass;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_filter_tag_name_get;
}

static void _egueb_svg_element_filter_instance_init(void *o)
{
	Egueb_Svg_Element_Filter *thiz;
	Egueb_Svg_Length minus10, plus120;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_ELEMENT_FILTER(o);
	egueb_svg_length_set(&minus10, -10.0, EGUEB_SVG_LENGTH_UNIT_PERCENT);
	egueb_svg_length_set(&plus120, 120, EGUEB_SVG_LENGTH_UNIT_PERCENT);

	/* create the properties */
	thiz->x = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X),
			&minus10, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y),
			&minus10, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->width = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_WIDTH),
			&plus120,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->height = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_HEIGHT),
			&plus120,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_DOM_XLINK_HREF),
			NULL);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->x), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->y), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->width), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->height), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->xlink_href), NULL);
}

static void _egueb_svg_element_filter_instance_deinit(void *o)
{
	Egueb_Svg_Element_Filter *thiz;

	thiz = EGUEB_SVG_ELEMENT_FILTER(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
	egueb_dom_node_unref(thiz->width);
	egueb_dom_node_unref(thiz->height);
	egueb_dom_node_unref(thiz->xlink_href);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_filter_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_filter);
	return n;
}

#if 0
EAPI Eina_Bool egueb_svg_is_filter(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Type type;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	type = egueb_svg_element_internal_type_get(t);
	return (type == ESVG_TYPE_FILTER) ? EINA_TRUE : EINA_FALSE;
}

EAPI void egueb_svg_element_filter_x_set(Ender_Element *e, const Egueb_Svg_Coord *x)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_FILTER_X, x);
}

EAPI void egueb_svg_element_filter_x_get(Ender_Element *e, Egueb_Svg_Coord *x)
{
}

EAPI void egueb_svg_element_filter_y_set(Ender_Element *e, const Egueb_Svg_Coord *y)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_FILTER_Y, y);
}

EAPI void egueb_svg_element_filter_y_get(Ender_Element *e, Egueb_Svg_Coord *y)
{
}

EAPI void egueb_svg_element_filter_width_set(Ender_Element *e, const Egueb_Svg_Length *width)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_FILTER_WIDTH, width);
}

EAPI void egueb_svg_element_filter_width_get(Ender_Element *e, Egueb_Svg_Length *width)
{
}

EAPI void egueb_svg_element_filter_height_set(Ender_Element *e, const Egueb_Svg_Length *height)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_FILTER_HEIGHT, height);
}

EAPI void egueb_svg_element_filter_height_get(Ender_Element *e, Egueb_Svg_Length *height)
{
}

EAPI void egueb_svg_element_filter_xlink_href_set(Ender_Element *e, const char *href)
{
	egueb_svg_element_property_string_set(e, ESVG_ELEMENT_FILTER_XLINK_HREF, href);
}

EAPI void egueb_svg_element_filter_xlink_href_get(Ender_Element *e, const char **href)
{
}
#endif

