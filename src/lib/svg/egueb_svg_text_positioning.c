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
#include "egueb_svg_document.h"
#include "egueb_svg_attr_length_list_private.h"
#include "egueb_svg_text_positioning_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                            Text content interface                          *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_text_positioning_initialize_state(
		Egueb_Svg_Text_Content *t)
{
	Egueb_Svg_Text_Positioning *thiz;
	Egueb_Dom_List *l;

	thiz = EGUEB_SVG_TEXT_POSITIONING(t);
	egueb_dom_attr_final_get(thiz->x, &l);
	t->state.x = egueb_dom_list_copy(l);
	egueb_dom_list_unref(l);

	egueb_dom_attr_final_get(thiz->y, &l);
	t->state.y = egueb_dom_list_copy(l);
	egueb_dom_list_unref(l);

	egueb_dom_attr_final_get(thiz->dx, &l);
	t->state.dx = egueb_dom_list_copy(l);
	egueb_dom_list_unref(l);

	egueb_dom_attr_final_get(thiz->dy, &l);
	t->state.dy = egueb_dom_list_copy(l);
	egueb_dom_list_unref(l);

	/* set the initial pen */
	t->state.pen_x = t->state.pen_y = 0;
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
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
	Egueb_Svg_Text_Content_Class *klass;

	klass = EGUEB_SVG_TEXT_CONTENT_CLASS(k);
	klass->initialize_state = _egueb_svg_text_positioning_initialize_state;
}

static void _egueb_svg_text_positioning_instance_init(void *o)
{
	Egueb_Svg_Text_Positioning *thiz;
	Egueb_Dom_List *def;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_TEXT_POSITIONING(o);
	/* create the properties */
	/* use an empty list for default values */
	def = egueb_dom_list_new(egueb_svg_length_descriptor_get());

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
