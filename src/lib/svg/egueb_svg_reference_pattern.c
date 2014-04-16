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
	Enesim_Renderer *r;
} Egueb_Svg_Reference_Pattern;

typedef struct _Egueb_Svg_Reference_Pattern_Class
{
	Egueb_Svg_Reference_Paint_Server_Class base;
} Egueb_Svg_Reference_Pattern_Class;

#if 0
static Eina_Bool _pattern_setup(Enesim_Renderer *r,
		const Egueb_Svg_Element_Context *state,
		Enesim_Renderer *rel)
{
	Egueb_Svg_Pattern *thiz;
	Egueb_Svg_Pattern_Units pu;
	Enesim_Matrix m;
	double x;
	double y;
	double w;
	double h;

	thiz = _egueb_svg_pattern_get(r);
	pu = thiz->units;
	if (pu == ESVG_OBJECT_BOUNDING_BOX)
	{
		Eina_Rectangle bbox;

		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		x = egueb_svg_coord_final_get(&thiz->x, 1);
		y = egueb_svg_coord_final_get(&thiz->y, 1);
		w = egueb_svg_coord_final_get(&thiz->width, 1);
		h = egueb_svg_coord_final_get(&thiz->height, 1);

		enesim_renderer_destination_bounds_get(rel, &bbox, 0, 0);
		enesim_matrix_values_set(&m, bbox.w, 0, bbox.x, 0, bbox.h, bbox.y, 0, 0, 1);
	}
	else
	{
		double vw;
		double vh;

		/* use the user space coordiantes */
		vw = state->viewbox_w;
		vh = state->viewbox_h;
		x = egueb_svg_coord_final_get(&thiz->x, vw);
		y = egueb_svg_coord_final_get(&thiz->y, vh);
		w = egueb_svg_coord_final_get(&thiz->width, vw);
		h = egueb_svg_coord_final_get(&thiz->height, vh);

		m = state->transform;
	}

	/* set the properties */
	enesim_renderer_pattern_x_set(thiz->r, x);
	enesim_renderer_pattern_y_set(thiz->r, y);
	enesim_renderer_pattern_width_set(thiz->r, w);
	enesim_renderer_pattern_height_set(thiz->r, h);

	printf("pattern setup %g %g %g %g\n", x, y, w, h);
	if (enesim_matrix_type_get(&thiz->transform) != ENESIM_MATRIX_IDENTITY)
	{
		enesim_matrix_compose(&m, &thiz->transform, &m);
	}
	enesim_renderer_transformation_set(thiz->r, &m);
	enesim_renderer_pattern_source_set(thiz->r, thiz->content);
	printf("ok, the content set %p\n", thiz->content);

	/* TODO we need to set the new viewbox */
	/* 1. setup the content */
	/* 2. get the content renderer */
	/* 3. assign it */
#if 0
	{
		Egueb_Svg_Element_Context new_state;

		memset(&new_state, 0, sizeof(Egueb_Svg_Element_Context));
		new_state.viewbox_w = w;
		new_state.viewbox_h = h;
		new_state.transform = m;

		egueb_svg_element_setup(thiz->content, estate, attr, s, error);
		enesim_renderer_pattern_source_set(thiz->r, thiz->content);
	}
#endif

	return EINA_TRUE;
}
#endif

/*----------------------------------------------------------------------------*
 *                           Paint server interface                           *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_reference_pattern_renderer_get(
		Egueb_Svg_Reference_Paint_Server *p)
{
	Egueb_Svg_Reference_Pattern *thiz;

	thiz = EGUEB_SVG_REFERENCE_PATTERN(p);
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
