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
#include "egueb_svg_shape_private.h"
#include "egueb_svg_element_text_private.h"
/*
 * TODO
 * this element must have a list of text spans renderers too, given that
 * several attributes are of a list type, we need to createa text span
 * for each group of glyphs
 */
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
	Egueb_Svg_Shape base;
	/* properties */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	/* private */
	double gx;
	double gy;
	Enesim_Text_Font *font;
	Enesim_Renderer *r;
} Egueb_Svg_Element_Tspan;

typedef struct _Egueb_Svg_Element_Tspan_Class
{
	Egueb_Svg_Shape_Class base;
} Egueb_Svg_Element_Tspan_Class;

/* from a node inserted or node removed event, get the tspan node in case
 * the node inserted/removed is of that type and the parent is ourselves
 */
static Eina_Bool _egueb_svg_element_tspan_mutation_get_tspan(Egueb_Dom_Event *e,
		Egueb_Dom_Node *us, Egueb_Dom_Node **tspan)
{
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node *related = NULL;
	Egueb_Dom_Node_Type type;
	Eina_Bool ret = EINA_FALSE;

	related = egueb_dom_event_mutation_related_get(e);
	if (related != us)
	{
		goto not_us;
	}

	target = egueb_dom_event_target_get(e);
	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_TEXT_NODE)
	{
		goto not_tspan;
	}

	*tspan = egueb_dom_node_ref(target);
	ret = EINA_TRUE;
not_tspan:
	egueb_dom_node_unref(target);
not_us:
	egueb_dom_node_unref(related);
	return ret;
}

static void _egueb_svg_element_tspan_node_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *tspan = NULL;

	if (_egueb_svg_element_tspan_mutation_get_tspan(e, n, &tspan))
	{
		Egueb_Svg_Element_Tspan *thiz;
		Enesim_Text_Buffer *nb = NULL;

		thiz = EGUEB_SVG_ELEMENT_TSPAN(n);
		/* set the internal buffer of the tspan span to be the one
		 * on the tspan node */
		egueb_dom_character_data_buffer_get(tspan, &nb);
		enesim_renderer_text_span_real_buffer_set(thiz->r, nb);
		/* now make the tspan node to use our own buffer from now on */
		enesim_renderer_text_span_buffer_get(thiz->r, &nb);
		egueb_dom_character_data_buffer_set(tspan, nb);
		egueb_dom_node_unref(tspan);
	}
}

static void _egueb_svg_element_tspan_node_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *tspan = NULL;

	if (_egueb_svg_element_tspan_mutation_get_tspan(e, n, &tspan))
	{
		Egueb_Svg_Element_Tspan *thiz;
		Enesim_Text_Buffer *nb;

		thiz = EGUEB_SVG_ELEMENT_TSPAN(n);
		enesim_renderer_text_span_real_buffer_get(thiz->r, &nb);
		egueb_dom_character_data_buffer_set(tspan, nb);
		egueb_dom_node_unref(tspan);
	}
}

/*----------------------------------------------------------------------------*
 *                               Shape interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_tspan_generate_geometry(Egueb_Svg_Shape *s,
		Egueb_Svg_Element *relative, Egueb_Dom_Node *doc)
{
	Egueb_Svg_Element_Tspan *thiz;
	Egueb_Svg_Element_Text_Pen *pen;
	Egueb_Svg_Element *e;
	Egueb_Svg_Length x, y;
	Enesim_Rectangle bounds;
	double gx, gy;
	double doc_font_size, gfont;
	int max;
	int size;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(s);
	egueb_dom_attr_final_get(thiz->x, &x);
	egueb_dom_attr_final_get(thiz->y, &y);

	/* calculate the real size */
	doc_font_size = egueb_svg_document_font_size_get(doc);
	thiz->gx = egueb_svg_coord_final_get(&x, relative->viewbox.w, doc_font_size);
	thiz->gy = egueb_svg_coord_final_get(&y, relative->viewbox.h, doc_font_size);

	/* get the pen we should use for every span */
	egueb_svg_element_text_pen_get(EGUEB_DOM_NODE(relative), &pen);

	/* set the position */
	e = EGUEB_SVG_ELEMENT(s);
	/* FIXME the font size accumulates from its parent, so we need to do
	 * similar to the transformation
	 */
	gfont = e->final_font_size;
	gx = pen->x;
	gy = pen->y;

	size = ceil(gfont);
	if (size < 0)
	{
		ERR("Negative font size of %d", size);
		size = 0;
	}
	enesim_renderer_text_span_font_set(thiz->r, enesim_text_font_ref(thiz->font));
	max = enesim_text_font_max_ascent_get(thiz->font);
	enesim_renderer_origin_set(thiz->r, gx, gy - max);

	INFO("matrix %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS (&e->transform));
	enesim_renderer_transformation_set(thiz->r, &e->transform);

	INFO("x: %g, y: %g, font-size: %d", gx, gy, size);
	enesim_renderer_shape_geometry_get(thiz->r, &bounds);
	INFO("advancing by w: %g, h: %g", bounds.w, bounds.h);
	pen->x += bounds.w;

	return EINA_TRUE;
}

static void _egueb_svg_element_tspan_renderer_propagate(Egueb_Svg_Shape *s,
		Egueb_Svg_Painter *painter)
{
	Egueb_Svg_Element_Tspan *thiz;
	Enesim_Color color;
	Enesim_Color fill_color;
	Eina_Bool visibility;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(s);

	egueb_svg_painter_visibility_get(painter, &visibility);
	egueb_svg_painter_color_get(painter, &color);
	egueb_svg_painter_fill_color_get(painter, &fill_color);

	enesim_renderer_color_set(thiz->r, fill_color);
	enesim_renderer_visibility_set(thiz->r, visibility);
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_element_tspan_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_tspan_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(r);
#if 0
	enesim_rectangle_coords_from(bounds, thiz->gx, thiz->gy,
			thiz->gw, thiz->gh);
#endif
}
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
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_tspan, Egueb_Svg_Element_Tspan, x);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_tspan, Egueb_Svg_Element_Tspan, y);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_SHAPE_DESCRIPTOR,
		Egueb_Svg_Element_Tspan, Egueb_Svg_Element_Tspan_Class,
		egueb_svg_element_tspan);

static void _egueb_svg_element_tspan_class_init(void *k)
{
	Egueb_Svg_Shape_Class *klass;
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_SHAPE_CLASS(k);
	klass->generate_geometry = _egueb_svg_element_tspan_generate_geometry;
	klass->renderer_propagate = _egueb_svg_element_tspan_renderer_propagate;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->bounds_get = _egueb_svg_element_tspan_bounds_get;
	r_klass->renderer_get = _egueb_svg_element_tspan_renderer_get;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_tspan_tag_name_get;
}

static void _egueb_svg_element_tspan_instance_init(void *o)
{
	Egueb_Svg_Element_Tspan *thiz;
	Enesim_Text_Engine *e;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(o);
	e = enesim_text_engine_default_get();
	thiz->font = enesim_text_font_new_description_from(e, "Sans:style=Regular", 16);
	enesim_text_engine_unref(e);

	r = enesim_renderer_text_span_new();
	enesim_renderer_text_span_font_set(r, enesim_text_font_ref(thiz->font));
	enesim_renderer_color_set(r, 0xff000000);
	thiz->r = r;

	/* Default values */

	/* create the properties */
	thiz->x = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_tspan, x);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_tspan, y);

	/* add the events */
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_element_tspan_node_inserted_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_element_tspan_node_removed_cb,
			EINA_TRUE, thiz);
}

static void _egueb_svg_element_tspan_instance_deinit(void *o)
{
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(o);
	enesim_renderer_unref(thiz->r);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_is_element_tspan(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_TSPAN_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Egueb_Dom_Node * egueb_svg_element_tspan_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_tspan);
	return n;
}

EAPI void egueb_svg_element_tspan_x_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Length *x)
{
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(n);
	egueb_dom_attr_set(thiz->x, EGUEB_DOM_ATTR_TYPE_BASE, x);
}

EAPI void egueb_svg_element_tspan_y_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Length *y)
{
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(n);
	egueb_dom_attr_set(thiz->y, EGUEB_DOM_ATTR_TYPE_BASE, y);
}

