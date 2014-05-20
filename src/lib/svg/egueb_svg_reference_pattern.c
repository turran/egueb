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
#include "egueb_svg_document.h"

#include "egueb_svg_element_private.h"
#include "egueb_svg_renderable_private.h"
#include "egueb_svg_element_pattern_private.h"
#include "egueb_svg_reference_paint_server_private.h"
#include "egueb_svg_event_request_painter_private.h"

/*
 * The pattern's attributes x, y, w, h defines the size of the pattern tile.
 * And the behaviour is that the pattern tile always repeats (no restrict,
 * pad, reflect, whatever). For that we need to create a rectangle renderer of
 * the size x, y, w, h (the pattern attributes)
 */
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
	Enesim_Renderer *tile;
	Enesim_Renderer *r;
} Egueb_Svg_Reference_Pattern;

typedef struct _Egueb_Svg_Reference_Pattern_Class
{
	Egueb_Svg_Reference_Paint_Server_Class base;
} Egueb_Svg_Reference_Pattern_Class;

/*----------------------------------------------------------------------------*
 *                               Event listeners                              *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_reference_pattern_event_request_painter_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Painter *painter;
	Egueb_Dom_Node *n;

	n = egueb_dom_event_target_get(e);
	DBG_ELEMENT(n, "Setting the generic painter on the renderable");
	painter = egueb_svg_renderable_class_painter_get(n);
	egueb_svg_event_request_painter_painter_set(e, painter);
	egueb_dom_node_unref(n);
}

/*----------------------------------------------------------------------------*
 *                               Event monitors                               *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_reference_pattern_event_monitor_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Reference *r = data;
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *svg;

	if (!egueb_dom_event_is_mutation(e) && !egueb_dom_event_is_process(e))
		return;
	/* We need to propagate the event not through the referenceable but
	 * through the topmost element. That is because it is very possible
	 * that the referenceable tree ends up in a defs element and thus
	 * it will not be propagated
	 */
	doc = egueb_dom_node_document_get(r->referenceable);
	svg = egueb_svg_document_element_root_get(doc);
	egueb_dom_node_unref(doc);

	egueb_dom_node_event_propagate(svg, e);
	egueb_dom_node_unref(svg);
}

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
static void _egueb_svg_reference_pattern_setup(
		Egueb_Svg_Reference *r)
{
	Egueb_Svg_Reference_Pattern *thiz;
	Egueb_Dom_Node *doc;

	thiz = EGUEB_SVG_REFERENCE_PATTERN(r);

	DBG_ELEMENT(EGUEB_DOM_NODE(r->referenceable), "Cloning the children into our own <g>");
	/* Make the document adopt the g, given that it has no document yet */
	doc = egueb_dom_node_document_get(r->referenceable);
	thiz->g = egueb_dom_document_node_adopt(doc, thiz->g, NULL);
	egueb_dom_node_unref(doc);
	/* clone every children, for smil elements we wont send the etch
	 * request up in the tree and thus it will simply not work
	 */
	egueb_svg_element_children_clone(thiz->g, r->referenceable);
	/* make the group get the presentation attributes from ourelves and
	 * the geometry relative from the referrer
	 */
	egueb_svg_element_presentation_relative_set(thiz->g, r->referenceable, NULL);
	egueb_svg_element_geometry_relative_set(thiz->g, r->referencer, NULL);
}

static Eina_Bool _egueb_svg_reference_pattern_process(
		Egueb_Svg_Reference *r)
{
	Egueb_Svg_Reference_Pattern *thiz;
	Egueb_Svg_Length x, y, w, h;
	Egueb_Svg_Referenceable_Units units;
	Egueb_Svg_Rect vb;
	Egueb_Dom_Node *doc;
	Enesim_Matrix m, transform;
	Eina_Bool ret;
	Eina_Bool has_viewbox;
	double gx, gy, gw, gh;

	thiz = EGUEB_SVG_REFERENCE_PATTERN(r);
	DBG("Processing a pattern reference");
	/* get the final attributes */
	egueb_svg_element_pattern_deep_x_get(r->referenceable, &x);
	egueb_svg_element_pattern_deep_y_get(r->referenceable, &y);
	egueb_svg_element_pattern_deep_width_get(r->referenceable, &w);
	egueb_svg_element_pattern_deep_height_get(r->referenceable, &h);
	egueb_svg_element_pattern_deep_units_get(r->referenceable, &units);
	egueb_svg_element_pattern_deep_transform_get(r->referenceable, &transform);
	has_viewbox = egueb_svg_element_pattern_deep_viewbox_get(r->referenceable, &vb);

	if (units == EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX)
	{
		Egueb_Svg_Element *e_referencer;
		Enesim_Rectangle bounds;

		e_referencer = EGUEB_SVG_ELEMENT(r->referencer);
		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		gx = egueb_svg_coord_final_get(&x, 1, 1);
		gy = egueb_svg_coord_final_get(&y, 1, 1);
		gw = egueb_svg_length_final_get(&w, 1, 1, 1);
		gh = egueb_svg_length_final_get(&h, 1, 1, 1);

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
		doc = egueb_dom_node_document_get(r->referencer);
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
		gx = egueb_svg_coord_final_get(&x, ge_relative->viewbox.w, font_size);
		gy = egueb_svg_coord_final_get(&y, ge_relative->viewbox.h, font_size);
		gw = egueb_svg_length_final_get(&w, ge_relative->viewbox.w, ge_relative->viewbox.h, font_size);
		gh = egueb_svg_length_final_get(&h, ge_relative->viewbox.w, ge_relative->viewbox.h, font_size);

		m = ge_relative->transform;
		egueb_dom_node_unref(g_relative);
	}
	if (has_viewbox)
	{
		Egueb_Svg_Element *e;
		Enesim_Matrix em1, em2;

		e = EGUEB_SVG_ELEMENT(thiz->g);
		enesim_rectangle_coords_from(&e->viewbox, vb.x, vb.y, vb.w, vb.h);
		enesim_matrix_scale(&em1, gw / vb.w, gh / vb.h);
		enesim_matrix_translate(&em2, -vb.x, -vb.y);
		enesim_matrix_compose(&em2, &em1, &em2);
		INFO("Using a new viewbox %g %g %g %g", vb.x, vb.y, vb.w, vb.h);
		egueb_svg_renderable_transform_set(thiz->g, &em2);
	}

	/* set the content properties */
	thiz = EGUEB_SVG_REFERENCE_PATTERN(r);
	enesim_renderer_rectangle_x_set(thiz->tile, gx);
	enesim_renderer_rectangle_y_set(thiz->tile, gy);
	enesim_renderer_rectangle_width_set(thiz->tile, gw);
	enesim_renderer_rectangle_height_set(thiz->tile, gh);
	enesim_renderer_transformation_set(thiz->tile, &m);

	/* set the container properties */
	enesim_renderer_transformation_set(thiz->r, &transform);

	INFO("Coordinates x = %g, y = %g, w = %g, h = %g", gx, gy, gw, gh);
	INFO("Tile transformation %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS(&m));
	INFO("Pattern transformation %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS(&transform));

	ret = egueb_dom_element_process(thiz->g);

	return ret;
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
	klass->setup = _egueb_svg_reference_pattern_setup;

	p_klass = EGUEB_SVG_REFERENCE_PAINT_SERVER_CLASS(k);
	p_klass->renderer_get = _egueb_svg_reference_pattern_renderer_get;
}

static void _egueb_svg_reference_pattern_instance_init(void *o)
{
	Egueb_Svg_Reference_Pattern *thiz;
	Enesim_Renderer *src;

	thiz = EGUEB_SVG_REFERENCE_PATTERN(o);
	thiz->g = egueb_svg_element_g_new();
	/* Monitor the events that we need to propagate upstream */
	egueb_dom_node_event_monitor_add(thiz->g,
			_egueb_svg_reference_pattern_event_monitor_cb, thiz);
	/* Whenever a child element requests a painter, make sure to set the
	 * default one
	 */
	egueb_dom_node_event_listener_add(thiz->g,
			EGUEB_SVG_EVENT_REQUEST_PAINTER,
			_egueb_svg_reference_pattern_event_request_painter_cb,
			EINA_FALSE, NULL);
	src = egueb_svg_renderable_renderer_get(thiz->g);

	thiz->tile = enesim_renderer_rectangle_new();
	enesim_renderer_shape_fill_renderer_set(thiz->tile, src);
	enesim_renderer_shape_draw_mode_set(thiz->tile, ENESIM_RENDERER_SHAPE_DRAW_MODE_FILL);

	thiz->r = enesim_renderer_pattern_new();
	enesim_renderer_pattern_repeat_mode_set(thiz->r, ENESIM_REPEAT_MODE_REPEAT);
	enesim_renderer_pattern_source_renderer_set(thiz->r, enesim_renderer_ref(thiz->tile));
}

static void _egueb_svg_reference_pattern_instance_deinit(void *o)
{
	Egueb_Svg_Reference_Pattern *thiz;

	thiz = EGUEB_SVG_REFERENCE_PATTERN(o);
	egueb_dom_node_unref(thiz->g);
	enesim_renderer_unref(thiz->r);
	enesim_renderer_unref(thiz->tile);
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
