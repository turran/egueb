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
#include "egueb_svg_element_tspan.h"
#include "egueb_svg_document.h"
#include "egueb_svg_attr_length_list_private.h"
#include "egueb_svg_text_positioning_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_TEXT_CONTENT_DESCRIPTOR,
		Egueb_Svg_Text_Positioning, Egueb_Svg_Text_Positioning_Class,
		egueb_svg_text_positioning);

static void _egueb_svg_text_positioning_class_init(void *k)
{
}

static void _egueb_svg_text_positioning_instance_init(void *o)
{
	Egueb_Svg_Text_Positioning *thiz;
	Egueb_Svg_Length *zero;
	Egueb_Dom_List *def;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_TEXT_POSITIONING(o);
	/* create the properties */
	/* create a default list with 0 as the value of the coordinate */
	def = egueb_dom_list_new(egueb_svg_length_descriptor_get());
	zero = calloc(1, sizeof(Egueb_Svg_Length));
	*zero = EGUEB_SVG_LENGTH_0;
	egueb_dom_list_item_append(def, zero);

	thiz->x = egueb_svg_attr_length_list_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_X),
			egueb_dom_list_ref(def), EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_list_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_Y),
			egueb_dom_list_ref(def), EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->dx = egueb_svg_attr_length_list_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_DX),
			egueb_dom_list_ref(def), EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->dy = egueb_svg_attr_length_list_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_DY),
			egueb_dom_list_ref(def), EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	egueb_dom_list_unref(def);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->x), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->y), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->dx), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->dy), NULL);
}

static void _egueb_svg_text_positioning_instance_deinit(void *o)
{
	Egueb_Svg_Text_Positioning *thiz;

	thiz = EGUEB_SVG_TEXT_POSITIONING(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
	egueb_dom_node_unref(thiz->dx);
	egueb_dom_node_unref(thiz->dy);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
