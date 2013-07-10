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
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_renderable.h"
#include "egueb_svg_element.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_pattern.h"
#include "egueb_svg_reference_paint_server_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_REFERENCE_PATTERN_CLASS_GET(o) 				\
		EGUEB_SVG_REFERENCE_PATTERN_CLASS(				\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_REFERENCE_PATTERN_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,	\
		Egueb_Svg_Reference_Pattern_Class,				\
		EGUEB_SVG_REFERENCE_PATTERN_DESCRIPTOR)
#define EGUEB_SVG_REFERENCE_PATTERN(o) ENESIM_OBJECT_INSTANCE_CHECK(o,	\
		Egueb_Svg_Reference_Pattern,					\
		EGUEB_SVG_REFERENCE_PATTERN_DESCRIPTOR)

typedef struct _Egueb_Svg_Reference_Pattern
{
	Egueb_Svg_Reference_Paint_Server base;
	Egueb_Dom_Node *g;
	//Enesim_Renderer *content;
	Enesim_Renderer *r;
} Egueb_Svg_Reference_Pattern;

typedef struct _Egueb_Svg_Reference_Pattern_Class
{
	Egueb_Svg_Reference_Paint_Server_Class base;
} Egueb_Svg_Reference_Pattern_Class;

/*----------------------------------------------------------------------------*
 *                           Paint server interface                           *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_reference_pattern_renderer_get(
		Egueb_Svg_Reference_Paint_Server *p)
{
	Egueb_Svg_Reference_Pattern *thiz;

	thiz = EGUEB_SVG_REFERENCE_PATTERN(p);
	//return egueb_svg_renderable_renderer_get(thiz->g);
	return enesim_renderer_ref(thiz->r);
}
/*----------------------------------------------------------------------------*
 *                             Reference interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_reference_pattern_process(
		Egueb_Svg_Reference *r)
{
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_REFERENCE_PAINT_SERVER_DESCRIPTOR,
		Egueb_Svg_Reference_Pattern,
		Egueb_Svg_Reference_Pattern_Class,
		egueb_svg_reference_pattern);

static void _egueb_svg_reference_pattern_class_init(void *k)
{
	Egueb_Svg_Reference_Class *klass;
	Egueb_Svg_Reference_Paint_Server_Class *p_klass;

	klass = EGUEB_SVG_REFERENCE_CLASS(k);
	klass->process = _egueb_svg_reference_pattern_process;

	p_klass = EGUEB_SVG_REFERENCE_PAINT_SERVER_CLASS(k);
	p_klass->renderer_get = _egueb_svg_reference_pattern_renderer_get;
}

static void _egueb_svg_reference_pattern_class_deinit(void *k)
{
}

static void _egueb_svg_reference_pattern_instance_init(void *o)
{
	Egueb_Svg_Reference_Pattern *thiz;

	thiz = EGUEB_SVG_REFERENCE_PATTERN(o);
	thiz->g = egueb_svg_element_g_new();
	thiz->r = enesim_renderer_pattern_new();
}

static void _egueb_svg_reference_pattern_instance_deinit(void *o)
{
	Egueb_Svg_Reference_Pattern *thiz;

	thiz = EGUEB_SVG_REFERENCE_PATTERN(o);
	egueb_dom_node_unref(thiz->g);
	enesim_renderer_unref(thiz->r);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Reference * egueb_svg_reference_pattern_new(void)
{
	Egueb_Svg_Reference *r;
	r = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_reference_pattern);
	return r;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

