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
#include "egueb_svg_matrix.h"
#include "egueb_svg_length.h"
#include "egueb_svg_string.h"
#include "egueb_svg_spread_method.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_document.h"
#include "egueb_svg_element.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_gradient.h"
#include "egueb_svg_element_linear_gradient.h"

#include "egueb_svg_element_private.h"
#include "egueb_svg_gradient_private.h"
#include "egueb_svg_element_linear_gradient_private.h"
#include "egueb_svg_reference_gradient_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_REFERENCE_GRADIENT_LINEAR_CLASS_GET(o) 			\
		EGUEB_SVG_REFERENCE_GRADIENT_LINEAR_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_REFERENCE_GRADIENT_LINEAR_CLASS(k)				\
		ENESIM_OBJECT_CLASS_CHECK(k,					\
		Egueb_Svg_Reference_Gradient_Linear_Class,			\
		EGUEB_SVG_REFERENCE_GRADIENT_LINEAR_DESCRIPTOR)
#define EGUEB_SVG_REFERENCE_GRADIENT_LINEAR(o) ENESIM_OBJECT_INSTANCE_CHECK(o,	\
		Egueb_Svg_Reference_Gradient_Linear,				\
		EGUEB_SVG_REFERENCE_GRADIENT_LINEAR_DESCRIPTOR)

typedef struct _Egueb_Svg_Reference_Gradient_Linear
{
	Egueb_Svg_Reference_Gradient base;
	Enesim_Renderer *r;
} Egueb_Svg_Reference_Gradient_Linear;

typedef struct _Egueb_Svg_Refernece_Gradient_Linear_Class
{
	Egueb_Svg_Reference_Gradient_Class base;
} Egueb_Svg_Reference_Gradient_Linear_Class;
/*----------------------------------------------------------------------------*
 *                              Gradient interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_reference_gradient_linear_process(
		Egueb_Svg_Reference_Gradient *g)
{
	Egueb_Svg_Reference_Gradient_Linear *thiz;
	Egueb_Svg_Reference *r;
	Egueb_Svg_Length x1, y1, x2, y2;
	Egueb_Svg_Referenceable_Units units;
	Egueb_Dom_Node *doc = NULL;
	Enesim_Matrix m, transform;
	double gx1, gy1, gx2, gy2;

	DBG("Processing a linear gradient reference");
	/* get the final attributes */
	r = EGUEB_SVG_REFERENCE(g);
	egueb_svg_element_linear_gradient_deep_x1_get(r->referenceable, &x1);
	egueb_svg_element_linear_gradient_deep_y1_get(r->referenceable, &y1);
	egueb_svg_element_linear_gradient_deep_x2_get(r->referenceable, &x2);
	egueb_svg_element_linear_gradient_deep_y2_get(r->referenceable, &y2);
	egueb_svg_gradient_deep_units_get(r->referenceable, &units);
	egueb_svg_gradient_deep_transform_get(r->referenceable, &transform);

	if (units == EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX)
	{
		Enesim_Rectangle bounds;
		Egueb_Svg_Element *e_referencer;

		e_referencer = EGUEB_SVG_ELEMENT(r->referencer);
		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		gx1 = egueb_svg_coord_final_get(&x1, 1, 1);
		gy1 = egueb_svg_coord_final_get(&y1, 1, 1);
		gx2 = egueb_svg_coord_final_get(&x2, 1, 1);
		gy2 = egueb_svg_coord_final_get(&y2, 1, 1);

		egueb_svg_renderable_bounds_get(r->referencer, &bounds);
		enesim_matrix_values_set(&m, bounds.w, 0, bounds.x, 0, bounds.h, bounds.y, 0, 0, 1);
		DBG("Using the object bounding box %" ENESIM_RECTANGLE_FORMAT, ENESIM_RECTANGLE_ARGS(&bounds));
		/* transform the bounds using the context matrix */
		enesim_matrix_compose(&e_referencer->transform, &m, &m);
	}
	else
	{
		Egueb_Dom_Node *g_relative;
		Egueb_Svg_Element *ge_relative;
		double font_size;

		DBG("Using the user space on use");
		doc = egueb_dom_node_owner_document_get(r->referencer);
		if (!doc)
		{
			WARN("No document set");
			return EINA_FALSE;
		}
		font_size = egueb_svg_document_font_size_get(doc);
		egueb_dom_node_unref(doc);

		g_relative = egueb_svg_element_geometry_relative_get(r->referencer);
		if (!g_relative)
		{
			WARN("No relative geometry");
			return EINA_FALSE;
		}
		ge_relative = EGUEB_SVG_ELEMENT(g_relative);

		/* use the user space coordiantes */
		gx1 = egueb_svg_coord_final_get(&x1, ge_relative->viewbox.w, font_size);
		gy1 = egueb_svg_coord_final_get(&y1, ge_relative->viewbox.h, font_size);
		gx2 = egueb_svg_coord_final_get(&x2, ge_relative->viewbox.w, font_size);
		gy2 = egueb_svg_coord_final_get(&y2, ge_relative->viewbox.h, font_size);

		m = ge_relative->transform;
		egueb_dom_node_unref(g_relative);
	}
	/* Apply the gradient transform */
	if (enesim_matrix_type_get(&transform) != ENESIM_MATRIX_TYPE_IDENTITY)
		enesim_matrix_compose(&m, &transform, &m);

	/* set the renderer properties */
	thiz = EGUEB_SVG_REFERENCE_GRADIENT_LINEAR(r);
	enesim_renderer_transformation_set(thiz->r, &m);
	enesim_renderer_gradient_linear_x0_set(thiz->r, gx1);
	enesim_renderer_gradient_linear_y0_set(thiz->r, gy1);
	enesim_renderer_gradient_linear_x1_set(thiz->r, gx2);
	enesim_renderer_gradient_linear_y1_set(thiz->r, gy2);

	DBG("Coordinates x1 = %g, y1 = %g, x2 = %g, y2 = %g", gx1, gy1, gx2, gy2);
	DBG("Transformation %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS(&m));

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                           Paint server interface                           *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_reference_gradient_linear_renderer_get(
		Egueb_Svg_Reference_Paint_Server *p)
{
	Egueb_Svg_Reference_Gradient_Linear *thiz;

	thiz = EGUEB_SVG_REFERENCE_GRADIENT_LINEAR(p);
	return enesim_renderer_ref(thiz->r);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_REFERENCE_GRADIENT_DESCRIPTOR,
		Egueb_Svg_Reference_Gradient_Linear,
		Egueb_Svg_Reference_Gradient_Linear_Class,
		egueb_svg_reference_gradient_linear);

static void _egueb_svg_reference_gradient_linear_class_init(void *k)
{
	Egueb_Svg_Reference_Gradient_Class *klass;
	Egueb_Svg_Reference_Paint_Server_Class *p_klass;

	klass = EGUEB_SVG_REFERENCE_GRADIENT_CLASS(k);
	klass->process = _egueb_svg_reference_gradient_linear_process;

	p_klass = EGUEB_SVG_REFERENCE_PAINT_SERVER_CLASS(k);
	p_klass->renderer_get = _egueb_svg_reference_gradient_linear_renderer_get;
}

static void _egueb_svg_reference_gradient_linear_instance_init(void *o)
{
	Egueb_Svg_Reference_Gradient_Linear *thiz;

	thiz = EGUEB_SVG_REFERENCE_GRADIENT_LINEAR(o);
	thiz->r = enesim_renderer_gradient_linear_new();
}

static void _egueb_svg_reference_gradient_linear_instance_deinit(void *o)
{
	Egueb_Svg_Reference_Gradient_Linear *thiz;

	thiz = EGUEB_SVG_REFERENCE_GRADIENT_LINEAR(o);
	enesim_renderer_unref(thiz->r);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Reference * egueb_svg_reference_gradient_linear_new(void)
{
	Egueb_Svg_Reference *r;
	r = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_reference_gradient_linear);
	return r;
}
