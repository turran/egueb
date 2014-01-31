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
#include "egueb_svg_element_stop.h"
#include "egueb_svg_element_private.h"
#include "egueb_svg_element_stop_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_STOP_DESCRIPTOR egueb_svg_element_stop_descriptor_get()
#define EGUEB_SVG_ELEMENT_STOP_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Stop_Class, EGUEB_SVG_ELEMENT_STOP_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_STOP(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Stop, EGUEB_SVG_ELEMENT_STOP_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Stop
{
	Egueb_Svg_Element base;
	/* properties */
	Egueb_Dom_Node *offset;
	/* interface */
	/* private */
	Enesim_Renderer_Gradient_Stop s;
} Egueb_Svg_Element_Stop;

typedef struct _Egueb_Svg_Element_Stop_Class
{
	Egueb_Svg_Element_Class base;
} Egueb_Svg_Element_Stop_Class;
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_stop_process(Egueb_Svg_Element *e)
{
	Egueb_Svg_Element_Stop *thiz;
	Egueb_Svg_Color color;
	Egueb_Svg_Length offset = EGUEB_SVG_LENGTH_0;
	Egueb_Svg_Number opacity;

	DBG("Processing stop");
	thiz = EGUEB_SVG_ELEMENT_STOP(e);

	egueb_dom_attr_final_get(e->stop_opacity, &opacity);
	egueb_dom_attr_final_get(e->stop_color, &color);
	egueb_dom_attr_final_get(thiz->offset, &offset);

	enesim_argb_components_from(&thiz->s.argb, lrint(opacity * 255),
			color.r, color.g, color.b);

	if (offset.unit == EGUEB_SVG_UNIT_LENGTH_PERCENT)
		thiz->s.pos = offset.value / 100.0;
	else
		thiz->s.pos = offset.value;

	if (thiz->s.pos > 1)
		thiz->s.pos = 1;
	else if (thiz->s.pos < 0)
		thiz->s.pos = 0;
	DBG("Stop color = %08x pos = %g", thiz->s.argb, thiz->s.pos);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_stop_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_STOP);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_ELEMENT_DESCRIPTOR,
		Egueb_Svg_Element_Stop, Egueb_Svg_Element_Stop_Class,
		egueb_svg_element_stop);

static void _egueb_svg_element_stop_class_init(void *k)
{
	Egueb_Svg_Element_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_ELEMENT_CLASS(k);
	klass->process = _egueb_svg_element_stop_process;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_stop_tag_name_get;
}

static void _egueb_svg_element_stop_instance_init(void *o)
{
	Egueb_Svg_Element_Stop *thiz;
	Egueb_Dom_Node *n;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_STOP(o);
	/* create the properties */
	thiz->offset = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_OFFSET),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->offset), NULL);
}

static void _egueb_svg_element_stop_instance_deinit(void *o)
{
	Egueb_Svg_Element_Stop *thiz;

	thiz = EGUEB_SVG_ELEMENT_STOP(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->offset);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Enesim_Renderer_Gradient_Stop * egueb_svg_element_stop_gradient_stop_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Stop *thiz;

	thiz = EGUEB_SVG_ELEMENT_STOP(n);
	return &thiz->s;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_element_is_stop(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_STOP_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Egueb_Dom_Node * egueb_svg_element_stop_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_stop);
	return n;
}

#if 0
EAPI void egueb_svg_element_stop_offset_set(Ender_Element *e, const Egueb_Svg_Length *offset)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_STOP_OFFSET, offset, NULL);
}
#endif
