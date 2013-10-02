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
#include "egueb_svg_spread_method.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_document.h"
#include "egueb_svg_element.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_gradient.h"
#include "egueb_svg_element_radial_gradient.h"

#include "egueb_svg_element_private.h"
#include "egueb_svg_reference_gradient_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_REFERENCE_GRADIENT_RADIAL_CLASS_GET(o) 			\
		EGUEB_SVG_REFERENCE_GRADIENT_RADIAL_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_REFERENCE_GRADIENT_RADIAL_CLASS(k)				\
		ENESIM_OBJECT_CLASS_CHECK(k,					\
		Egueb_Svg_Reference_Gradient_Radial_Class,			\
		EGUEB_SVG_REFERENCE_GRADIENT_RADIAL_DESCRIPTOR)
#define EGUEB_SVG_REFERENCE_GRADIENT_RADIAL(o) ENESIM_OBJECT_INSTANCE_CHECK(o,	\
		Egueb_Svg_Reference_Gradient_Radial,				\
		EGUEB_SVG_REFERENCE_GRADIENT_RADIAL_DESCRIPTOR)

typedef struct _Egueb_Svg_Reference_Gradient_Radial
{
	Egueb_Svg_Reference_Gradient base;
	Enesim_Renderer *r;
} Egueb_Svg_Reference_Gradient_Radial;

typedef struct _Egueb_Svg_Refernece_Gradient_Radial_Class
{
	Egueb_Svg_Reference_Gradient_Class base;
} Egueb_Svg_Reference_Gradient_Radial_Class;
/*----------------------------------------------------------------------------*
 *                              Gradient interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_reference_gradient_radial_process(
		Egueb_Svg_Reference_Gradient *g)
{
	Egueb_Svg_Reference_Gradient_Radial *thiz;
	Egueb_Svg_Reference *r;
	Egueb_Svg_Length cx, cy, fx, fy, rad;
	Egueb_Svg_Referenceable_Units units;
	Egueb_Dom_Node *doc = NULL;
	Enesim_Matrix m, transform;
	double gcx, gcy, gfx, gfy, grad;

	DBG("Processing a radial gradient reference");
	/* get the final attributes */
	r = EGUEB_SVG_REFERENCE(g);
	egueb_svg_element_radial_gradient_deep_cx_get(r->referenceable, &cx);
	egueb_svg_element_radial_gradient_deep_cy_get(r->referenceable, &cy);
	if (!egueb_svg_element_radial_gradient_deep_fx_get(r->referenceable, &fx))
		fx = cx;
	if (!egueb_svg_element_radial_gradient_deep_fy_get(r->referenceable, &fy))
		fy = cy;
	egueb_svg_element_radial_gradient_deep_r_get(r->referenceable, &rad);
	egueb_svg_gradient_deep_units_get(r->referenceable, &units);
	egueb_svg_gradient_deep_transform_get(r->referenceable, &transform);

	if (units == EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX)
	{
		Egueb_Svg_Element *e_referencer;
		Enesim_Rectangle bounds;

		e_referencer = EGUEB_SVG_ELEMENT(r->referencer);
		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		gcx = egueb_svg_coord_final_get(&cx, 1, 1);
		gcy = egueb_svg_coord_final_get(&cy, 1, 1);
		gfx = egueb_svg_coord_final_get(&fx, 1, 1);
		gfy = egueb_svg_coord_final_get(&fy, 1, 1);
		grad = egueb_svg_length_final_get(&rad, 1, 1, 1);

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
		doc = egueb_dom_node_document_get(EGUEB_DOM_NODE(r->referencer));
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
		gcx = egueb_svg_coord_final_get(&cx, ge_relative->viewbox.w, font_size);
		gcy = egueb_svg_coord_final_get(&cy, ge_relative->viewbox.h, font_size);
		gfx = egueb_svg_coord_final_get(&fx, ge_relative->viewbox.w, font_size);
		gfy = egueb_svg_coord_final_get(&fy, ge_relative->viewbox.h, font_size);
		grad = egueb_svg_length_final_get(&rad, ge_relative->viewbox.w, ge_relative->viewbox.h, font_size);

		m = ge_relative->transform;
		egueb_dom_node_unref(g_relative);
	}
	/* Apply the gradient transform */
	if (enesim_matrix_type_get(&transform) != ENESIM_MATRIX_IDENTITY)
		enesim_matrix_compose(&m, &transform, &m);

	/* set the renderer properties */
	thiz = EGUEB_SVG_REFERENCE_GRADIENT_RADIAL(r);
	enesim_renderer_transformation_set(thiz->r, &m);
	enesim_renderer_gradient_radial_center_x_set(thiz->r, gcx);
	enesim_renderer_gradient_radial_center_y_set(thiz->r, gcy);
	enesim_renderer_gradient_radial_focus_x_set(thiz->r, gfx);
	enesim_renderer_gradient_radial_focus_y_set(thiz->r, gfy);
	enesim_renderer_gradient_radial_radius_set(thiz->r, grad);

	INFO("Coordinates cx = %g, cy = %g, fx = %g, fy = %g, rad = %g", gcx, gcy, gfx, gfy, grad);
	INFO("Transformation %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS(&m));

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                           Paint server interface                           *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_reference_gradient_radial_renderer_get(
		Egueb_Svg_Reference_Paint_Server *p)
{
	Egueb_Svg_Reference_Gradient_Radial *thiz;

	thiz = EGUEB_SVG_REFERENCE_GRADIENT_RADIAL(p);
	return enesim_renderer_ref(thiz->r);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_REFERENCE_GRADIENT_DESCRIPTOR,
		Egueb_Svg_Reference_Gradient_Radial,
		Egueb_Svg_Reference_Gradient_Radial_Class,
		egueb_svg_reference_gradient_radial);

static void _egueb_svg_reference_gradient_radial_class_init(void *k)
{
	Egueb_Svg_Reference_Gradient_Class *klass;
	Egueb_Svg_Reference_Paint_Server_Class *p_klass;

	klass = EGUEB_SVG_REFERENCE_GRADIENT_CLASS(k);
	klass->process = _egueb_svg_reference_gradient_radial_process;

	p_klass = EGUEB_SVG_REFERENCE_PAINT_SERVER_CLASS(k);
	p_klass->renderer_get = _egueb_svg_reference_gradient_radial_renderer_get;
}

static void _egueb_svg_reference_gradient_radial_instance_init(void *o)
{
	Egueb_Svg_Reference_Gradient_Radial *thiz;

	thiz = EGUEB_SVG_REFERENCE_GRADIENT_RADIAL(o);
	thiz->r = enesim_renderer_gradient_radial_new();
}

static void _egueb_svg_reference_gradient_radial_instance_deinit(void *o)
{
	Egueb_Svg_Reference_Gradient_Radial *thiz;

	thiz = EGUEB_SVG_REFERENCE_GRADIENT_RADIAL(o);
	enesim_renderer_unref(thiz->r);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Reference * egueb_svg_reference_gradient_radial_new(void)
{
	Egueb_Svg_Reference *r;
	r = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_reference_gradient_radial);
	return r;
}
#if 0
static Eina_Bool _egueb_svg_element_radial_gradient_propagate(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		Egueb_Svg_Element_Gradient_Context *gctx,
		Enesim_Renderer *r,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;
	Egueb_Svg_Element_Gradient_Units gu;
	Enesim_Repeat_Mode mode;
	Enesim_Matrix m;
	Egueb_Svg_Coord lcx;
	Egueb_Svg_Coord lcy;
	Egueb_Svg_Coord lfx;
	Egueb_Svg_Coord lfy;
	Egueb_Svg_Coord lrad;
	double cx;
	double cy;
	double fx;
	double fy;
	double rad;

	thiz = _egueb_svg_element_radial_gradient_get(t);

	gu = gctx->units;
	switch (gctx->spread_method)
	{
		case ESVG_SPREAD_METHOD_PAD:
		mode = ENESIM_PAD;
		break;

		case ESVG_SPREAD_METHOD_REPEAT:
		mode = ENESIM_REPEAT;
		break;

		case ESVG_SPREAD_METHOD_REFLECT:
		mode = ENESIM_REFLECT;
		break;

		default:
		return EINA_FALSE;
	}
	enesim_renderer_gradient_mode_set(r, mode);

	/* the coordinates can come from the href
	 * we need to found which one isnt set and use that
	 * for the calculus
	 */
	_egueb_svg_element_radial_gradient_merge(thiz, t, &lcx, &lcy, &lfx, &lfy, &lrad);

	if (gu == ESVG_OBJECT_BOUNDING_BOX)
	{
		DBG("Using object bounding box %g %g %g %g", ctx->bounds.x, ctx->bounds.y, ctx->bounds.w, ctx->bounds.h);
		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		cx = egueb_svg_coord_final_get(&lcx, 1, 1);
		cy = egueb_svg_coord_final_get(&lcy, 1, 1);
		fx = egueb_svg_coord_final_get(&lfx, 1, 1);
		fy = egueb_svg_coord_final_get(&lfy, 1, 1);
		/* the enesim renderer radius is not scaled by the transformation? */
		rad = egueb_svg_length_final_get(&lrad, ctx->bounds.w, ctx->bounds.h, 1);
		enesim_matrix_values_set(&m, ctx->bounds.w, 0, ctx->bounds.x, 0, ctx->bounds.h, ctx->bounds.y, 0, 0, 1);
		/* transform the bounds using the context matrix */
		enesim_matrix_compose(&ctx->transform, &m, &m);
	}
	else
	{
		double w;
		double h;

		/* use the user space coordiantes */
		w = ctx->viewbox.w;
		h = ctx->viewbox.h;

		cx = egueb_svg_coord_final_get(&lcx, w, ctx->font_size);
		cy = egueb_svg_coord_final_get(&lcy, h, ctx->font_size);
		fx = egueb_svg_coord_final_get(&lfx, w, ctx->font_size);
		fy = egueb_svg_coord_final_get(&lfy, h, ctx->font_size);
		rad = egueb_svg_length_final_get(&lrad, w, h, ctx->font_size);
		m = ctx->transform;
	}
	if (enesim_matrix_type_get(&gctx->transform) != ENESIM_MATRIX_IDENTITY)
	{
		enesim_matrix_compose(&m, &gctx->transform, &m);
	}
	enesim_renderer_transformation_set(r, &m);
	enesim_renderer_gradient_radial_center_x_set(r, cx);
	enesim_renderer_gradient_radial_center_y_set(r, cy);
	enesim_renderer_gradient_radial_focus_x_set(r, fx);
	enesim_renderer_gradient_radial_focus_y_set(r, fy);
	enesim_renderer_gradient_radial_radius_set(r, rad);

	DBG("Setup transform %" ENESIM_MATRIX_FORMAT " cx %g cy %g fx %g fy %g"
			" rad %g", ENESIM_MATRIX_ARGS (&m),
			cx, cy, fx, fy, rad);

	return ESVG_SETUP_OK;
}
#endif

