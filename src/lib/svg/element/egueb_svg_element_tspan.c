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
#include "egueb_svg_element_tspan.h"
#include "egueb_svg_text_positioning_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_TSPAN_DESCRIPTOR egueb_svg_element_tspan_descriptor_get()
#define EGUEB_SVG_ELEMENT_TSPAN_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Tspan_Class, EGUEB_SVG_ELEMENT_TSPAN_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_TSPAN(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Tspan, EGUEB_SVG_ELEMENT_TSPAN_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Tspan
{
	Egueb_Svg_Text_Positioning base;
} Egueb_Svg_Element_Tspan;

typedef struct _Egueb_Svg_Element_Tspan_Class
{
	Egueb_Svg_Text_Positioning_Class base;
} Egueb_Svg_Element_Tspan_Class;

/*----------------------------------------------------------------------------*
 *                           Text positining interface                        *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                            Text content interface                          *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_tspan_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_TSPAN);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_TEXT_POSITIONING_DESCRIPTOR,
		Egueb_Svg_Element_Tspan, Egueb_Svg_Element_Tspan_Class,
		egueb_svg_element_tspan);

static void _egueb_svg_element_tspan_class_init(void *k)
{
	Egueb_Dom_Element_Class *e_klass;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_tspan_tag_name_get;
}

static void _egueb_svg_element_tspan_instance_init(void *o)
{
}

static void _egueb_svg_element_tspan_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_tspan_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_tspan);
	return n;
}

EAPI Eina_Bool egueb_svg_is_element_tspan(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_TSPAN_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}
