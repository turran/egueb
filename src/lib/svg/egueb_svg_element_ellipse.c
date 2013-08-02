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
#include "egueb_svg_element_ellipse.h"
#include "egueb_svg_document.h"
#include "egueb_svg_shape_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_ELLIPSE_DESCRIPTOR egueb_svg_element_ellipse_descriptor_get()
#define EGUEB_SVG_ELEMENT_ELLIPSE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Ellipse_Class, EGUEB_SVG_ELEMENT_ELLIPSE_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_ELLIPSE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Ellipse, EGUEB_SVG_ELEMENT_ELLIPSE_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Ellipse
{
	Egueb_Svg_Shape base;
	/* properties */
	Egueb_Dom_Node *cx;
	Egueb_Dom_Node *cy;
	Egueb_Dom_Node *rx;
	Egueb_Dom_Node *ry;
	/* private */
	Enesim_Renderer *r;
	/* needed for the bounds */
	double gcx, gcy, grx, gry;
} Egueb_Svg_Element_Ellipse;

typedef struct _Egueb_Svg_Element_Ellipse_Class
{
	Egueb_Svg_Shape_Class base;
} Egueb_Svg_Element_Ellipse_Class;
/*----------------------------------------------------------------------------*
 *                               Shape interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_ellipse_generate_geometry(Egueb_Svg_Shape *s,
		Egueb_Svg_Element *relative, Egueb_Dom_Node *doc)
{
	Egueb_Svg_Element_Ellipse *thiz;
	Egueb_Svg_Element *e;
	Egueb_Svg_Length cx, cy, rx, ry;
	double font_size;

	thiz = EGUEB_SVG_ELEMENT_ELLIPSE(s);
	egueb_dom_attr_final_get(thiz->cx, &cx);
	egueb_dom_attr_final_get(thiz->cy, &cy);
	egueb_dom_attr_final_get(thiz->rx, &rx);
	egueb_dom_attr_final_get(thiz->ry, &ry);

	/* calculate the real size */
	egueb_svg_document_font_size_get(doc, &font_size);
	thiz->gcx = egueb_svg_coord_final_get(&cx, relative->viewbox.w, font_size);
	thiz->gcy = egueb_svg_coord_final_get(&cy, relative->viewbox.h, font_size);
	thiz->grx = egueb_svg_coord_final_get(&rx, relative->viewbox.w, font_size);
	thiz->gry = egueb_svg_coord_final_get(&ry, relative->viewbox.h, font_size);

	/* set the position */
	enesim_renderer_ellipse_center_set(thiz->r, thiz->gcx, thiz->gcy);
	/* set the size */
	enesim_renderer_ellipse_radii_set(thiz->r, thiz->grx, thiz->gry);
	/* set the transformation */
	e = EGUEB_SVG_ELEMENT(s);
	enesim_renderer_transformation_set(thiz->r, &e->transform);

	DBG("cx: %g, cy: %g, rx: %g, ry: %g", thiz->gcx, thiz->gcy, thiz->grx, thiz->gry);

	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_element_ellipse_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Ellipse *thiz;

	thiz = EGUEB_SVG_ELEMENT_ELLIPSE(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_ellipse_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Element_Ellipse *thiz;

	thiz = EGUEB_SVG_ELEMENT_ELLIPSE(r);
	enesim_rectangle_coords_from(bounds,
			thiz->gcx - thiz->grx,
			thiz->gcy - thiz->gry,
			thiz->grx * 2, thiz->gry * 2);
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_ellipse_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_ELLIPSE);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_ellipse, Egueb_Svg_Element_Ellipse, cx);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_ellipse, Egueb_Svg_Element_Ellipse, cy);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_ellipse, Egueb_Svg_Element_Ellipse, rx);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_ellipse, Egueb_Svg_Element_Ellipse, ry);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_SHAPE_DESCRIPTOR,
		Egueb_Svg_Element_Ellipse, Egueb_Svg_Element_Ellipse_Class,
		egueb_svg_element_ellipse);

static void _egueb_svg_element_ellipse_class_init(void *k)
{
	Egueb_Svg_Shape_Class *klass;
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_SHAPE_CLASS(k);
	klass->generate_geometry = _egueb_svg_element_ellipse_generate_geometry;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->renderer_get = _egueb_svg_element_ellipse_renderer_get;
	r_klass->bounds_get = _egueb_svg_element_ellipse_bounds_get;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_ellipse_tag_name_get;
}

static void _egueb_svg_element_ellipse_class_deinit(void *k)
{
}

static void _egueb_svg_element_ellipse_instance_init(void *o)
{
	Egueb_Svg_Element_Ellipse *thiz;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_ELLIPSE(o);
	r = enesim_renderer_ellipse_new();
	thiz->r = r;

	/* Default values */
	enesim_renderer_rop_set(thiz->r, ENESIM_BLEND);

	/* create the properties */
	thiz->cx = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_CX),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->cy = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_CY),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->rx = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_RX),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->ry = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_RY),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_ellipse, cx);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_ellipse, cy);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_ellipse, rx);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_ellipse, ry);
}

static void _egueb_svg_element_ellipse_instance_deinit(void *o)
{
	Egueb_Svg_Element_Ellipse *thiz;

	thiz = EGUEB_SVG_ELEMENT_ELLIPSE(o);
	enesim_renderer_unref(thiz->r);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->cx);
	egueb_dom_node_unref(thiz->cy);
	egueb_dom_node_unref(thiz->rx);
	egueb_dom_node_unref(thiz->ry);
}
#if 0

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_ellipse_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Ellipse *thiz;
	Egueb_Svg_Length lcx, lcy;
	Egueb_Svg_Length lrx, lry;

	thiz = _egueb_svg_element_ellipse_get(t);
	/* position */
	egueb_svg_attribute_animated_length_final_get(&thiz->cx, &lcx);
	egueb_svg_attribute_animated_length_final_get(&thiz->cy, &lcy);
	thiz->gcx = egueb_svg_coord_final_get(&lcx, ctx->viewbox.w, ctx->font_size);
	thiz->gcy = egueb_svg_coord_final_get(&lcy, ctx->viewbox.h, ctx->font_size);
	/* radius */
	egueb_svg_attribute_animated_length_final_get(&thiz->rx, &lrx);
	egueb_svg_attribute_animated_length_final_get(&thiz->ry, &lry);
	thiz->grx = egueb_svg_coord_final_get(&lrx, ctx->viewbox.w, ctx->font_size);
	thiz->gry = egueb_svg_coord_final_get(&lry, ctx->viewbox.h, ctx->font_size);
	/* set the bounds */
	DBG("calling the setup on the ellipse (%g %g %g %g)", thiz->gcx, thiz->gcy, thiz->grx, thiz->gry);

	return ESVG_SETUP_OK;
}
static Eina_Bool _egueb_svg_element_ellipse_renderer_propagate(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		Egueb_Svg_Renderable_Context *rctx,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Ellipse *thiz;

	thiz = _egueb_svg_element_ellipse_get(t);


	/* shape properties */
	enesim_renderer_shape_fill_color_set(thiz->r, rctx->fill_color);
	enesim_renderer_shape_fill_renderer_set(thiz->r, rctx->fill_renderer);
	enesim_renderer_shape_stroke_color_set(thiz->r, rctx->stroke_color);
	enesim_renderer_shape_stroke_renderer_set(thiz->r, rctx->stroke_renderer);

	enesim_renderer_shape_stroke_weight_set(thiz->r, rctx->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_RENDERER_SHAPE_STROKE_LOCATION_CENTER);
	enesim_renderer_shape_draw_mode_set(thiz->r, rctx->draw_mode);
	/* base properties */
	enesim_renderer_transformation_set(thiz->r, &ctx->transform);
	enesim_renderer_color_set(thiz->r, rctx->color);

	return EINA_TRUE;
}
#endif
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_ellipse_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_ellipse);
	return n;
}

#if 0
EAPI Eina_Bool egueb_svg_is_ellipse(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Type type;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	type = egueb_svg_element_internal_type_get(t);
	return (type == ESVG_TYPE_ELLIPSE) ? EINA_TRUE : EINA_FALSE;
}

EAPI void egueb_svg_element_ellipse_cx_set(Ender_Element *e, const Egueb_Svg_Coord *cx)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_ELLIPSE_CX, cx);
}

EAPI void egueb_svg_element_ellipse_cx_get(Ender_Element *e, Egueb_Svg_Coord *cx)
{
}

EAPI void egueb_svg_element_ellipse_cy_set(Ender_Element *e, const Egueb_Svg_Coord *cy)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_ELLIPSE_CY, cy);
}

EAPI void egueb_svg_element_ellipse_cy_get(Ender_Element *e, Egueb_Svg_Coord *cy)
{
}

EAPI void egueb_svg_element_ellipse_rx_set(Ender_Element *e, const Egueb_Svg_Length *rx)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_ELLIPSE_RX, rx);
}

EAPI void egueb_svg_element_ellipse_rx_get(Ender_Element *e, Egueb_Svg_Length *rx)
{
}

EAPI void egueb_svg_element_ellipse_ry_set(Ender_Element *e, const Egueb_Svg_Length *ry)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_ELLIPSE_RY, ry);
}

EAPI void egueb_svg_element_ellipse_ry_get(Ender_Element *e, Egueb_Svg_Length *ry)
{
}
#endif
