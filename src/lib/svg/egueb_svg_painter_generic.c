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
#include "egueb_svg_paint.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_document.h"
#include "egueb_svg_paint_server.h"

#include "egueb_svg_painter_private.h"
#include "egueb_svg_reference_paint_server_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_PAINTER_GENERIC_DESCRIPTOR egueb_svg_painter_generic_descriptor_get()
#define EGUEB_SVG_PAINTER_GENERIC_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Painter_Generic_Class, EGUEB_SVG_PAINTER_GENERIC_DESCRIPTOR)
#define EGUEB_SVG_PAINTER_GENERIC(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Painter_Generic, EGUEB_SVG_PAINTER_GENERIC_DESCRIPTOR)

typedef struct _Egueb_Svg_Painter_Generic {
	Egueb_Svg_Painter base;
	/* possible references */
	Egueb_Svg_Reference *fill;
	Egueb_Svg_Reference *stroke;
	/* previous generated data */
	Egueb_Svg_Paint fill_paint_last;
	Egueb_Svg_Paint stroke_paint_last;
} Egueb_Svg_Painter_Generic;

typedef struct _Egueb_Svg_Painter_Generic_Class {
	Egueb_Svg_Painter_Class base;
} Egueb_Svg_Painter_Generic_Class;

typedef struct _Egueb_Svg_Painter_Generic_Stroke_Dasharray_Data {
	Egueb_Svg_Painter *p;
	Egueb_Svg_Element *e;
	double font_size;

	Enesim_Renderer_Shape_Stroke_Dash *dash;
} Egueb_Svg_Painter_Generic_Stroke_Dasharray_Data;

static void _egueb_svg_painter_generic_dash_foreach(void *data, void *user_data)
{
	Egueb_Svg_Painter_Generic_Stroke_Dasharray_Data *stroke_dasharray_data = user_data;
	Egueb_Svg_Painter *p = stroke_dasharray_data->p;
	Egueb_Svg_Element *e = stroke_dasharray_data->e;
	double font_size = stroke_dasharray_data->font_size;
	Egueb_Svg_Length *length = data;
	double viewport = 0;
	double length_final;

	if (length->unit == EGUEB_SVG_UNIT_LENGTH_PERCENT)
	{
		viewport = hypot(e->viewbox.w, e->viewbox.h) / M_SQRT2;
	}
	length_final = egueb_svg_coord_final_get(length, viewport, font_size);
	if (!stroke_dasharray_data->dash)
	{
		Enesim_Renderer_Shape_Stroke_Dash *dash;
		dash = calloc(1, sizeof(Enesim_Renderer_Shape_Stroke_Dash));
		dash->length = length_final;
		stroke_dasharray_data->dash = dash;
	}
	else
	{
		stroke_dasharray_data->dash->gap = length_final;
		p->stroke_dasharray = eina_list_append(p->stroke_dasharray, stroke_dasharray_data->dash);
		stroke_dasharray_data->dash = NULL;
	}
}

static void _egueb_svg_renderable_paint_set(Egueb_Dom_Node *n,
		Egueb_Dom_Node *doc,
		Enesim_Renderer_Shape_Draw_Mode *rdraw_mode,
		Enesim_Color *rcolor,
		Enesim_Renderer **renderer,
		Enesim_Renderer_Shape_Draw_Mode mode,
		double opacity,
		const Egueb_Svg_Color *current_color,
		Egueb_Svg_Reference **reference,
		const Egueb_Svg_Paint *current,
		Egueb_Svg_Paint *old)
{
	uint8_t op = opacity * 255;

	DBG("Opacity of %g", opacity);
	/* FIXME the fill color multiplies the fill renderer */
	if (current->type == EGUEB_SVG_PAINT_TYPE_COLOR)
	{
		const Egueb_Svg_Color *c = &current->color;

		enesim_color_components_from(rcolor,
				op, c->r, c->g, c->b);
		DBG("Paint type 'color': %08x", *rcolor);
		*rdraw_mode |= mode;
	}
	else if (current->type == EGUEB_SVG_PAINT_TYPE_SERVER)
	{
		DBG("Paint type 'server': '%s'", current->uri);
		if (!egueb_svg_paint_is_equal(current, old))
		{
			Egueb_Dom_Node *ref = NULL;
			Egueb_Dom_String iri = EGUEB_DOM_STRING_STATIC(current->uri);

			DBG("Trying to use '%s' as paint server", current->uri);
			if (*reference)
			{
				/* remove the reference in case it has it */
				egueb_svg_reference_free(*reference);
				*reference = NULL;
			}

			if (*renderer)
			{
				enesim_renderer_unref(*renderer);
				*renderer = NULL;
			}

			egueb_svg_document_element_get_by_iri(doc, &iri, &ref);
			if (!ref || !egueb_svg_is_paint_server(ref))
			{
				if (ref) egueb_dom_node_unref(ref);
			}
			else
			{
				*reference = egueb_svg_referenceable_reference_add(ref,
						n, NULL);
			}
		}
		if (*reference)
			egueb_svg_reference_paint_server_renderer_get(*reference, renderer);
		enesim_color_components_from(rcolor,
				op, 0xff, 0xff, 0xff);
		*rdraw_mode |= mode;
	}
	else if (current->type == EGUEB_SVG_PAINT_TYPE_NONE)
	{
		DBG("Paint type 'none'");
		*rdraw_mode &= ~mode;
	}
	else if (current->type == EGUEB_SVG_PAINT_TYPE_CURRENT_COLOR)
	{
		DBG("Paint type 'currentColor' %02x %02x %02x", current_color->r,
				current_color->g, current_color->b);
		enesim_color_components_from(rcolor,
				op, current_color->r, current_color->g, current_color->b);
		*rdraw_mode |= mode;
	}
	/* in case we have a reference process it */
	if (*reference)
	{
		egueb_svg_reference_process(*reference);
	}
	/* update the old paint */
	egueb_svg_paint_reset(old);
	egueb_svg_paint_copy(current, old);
}

static inline void _egueb_svg_painter_generic_cleanup(
		Egueb_Svg_Painter *p)
{
	/* clear the context */
	if (p->fill_renderer)
	{
		enesim_renderer_unref(p->fill_renderer);
		p->fill_renderer = NULL;
	}
	if (p->stroke_renderer)
	{
		enesim_renderer_unref(p->stroke_renderer);
		p->stroke_renderer = NULL;
	}
}

static inline void _egueb_svg_painter_generic_resolve_visibility(
		Egueb_Svg_Painter *p, Egueb_Svg_Element *e)
{
	Egueb_Svg_Visibility visibility;

	/* check if it is visible or not */
	egueb_dom_attr_final_get(e->visibility, &visibility);
	if (visibility == EGUEB_SVG_VISIBILITY_VISIBLE)
		p->visibility = EINA_TRUE;
	else
		p->visibility = EINA_FALSE;
#if 0
	/* check if it should display or not */
	if (attr->display.v != EGUEB_SVG_DISPLAY_NONE)
		visible = visible && EINA_TRUE;
	else
		visible = EINA_FALSE;
#endif
}

static inline void _egueb_svg_painter_generic_resolve_fill(
		Egueb_Svg_Painter *p, Egueb_Svg_Element *e,
		Egueb_Dom_Node *doc, Egueb_Svg_Color *color)
{
	Egueb_Svg_Painter_Generic *thiz;
	Egueb_Svg_Paint fill = EGUEB_SVG_PAINT_INIT;
	Egueb_Svg_Number fill_opacity;

	egueb_dom_attr_final_get(e->fill, &fill);
	egueb_dom_attr_final_get(e->fill_opacity, &fill_opacity);

	thiz = EGUEB_SVG_PAINTER_GENERIC(p);
	_egueb_svg_renderable_paint_set(EGUEB_DOM_NODE(e), doc,
			&p->draw_mode,
			&p->fill_color,
			&p->fill_renderer,
			ENESIM_RENDERER_SHAPE_DRAW_MODE_FILL,
			fill_opacity,
			color,
			&thiz->fill,
			&fill,
			&thiz->fill_paint_last);
#if 0
	if (attr->fill_rule.v == EGUEB_SVG_EVEN_ODD)
	{
		rp->fill_rule = ENESIM_RENDERER_SHAPE_FILL_RULE_EVEN_ODD;
	}
	else
	{
		rp->fill_rule = ENESIM_RENDERER_SHAPE_FILL_RULE_NON_ZERO;
	}
#endif
	egueb_svg_paint_reset(&fill);
}

static inline void _egueb_svg_painter_generic_resolve_stroke(
		Egueb_Svg_Painter *p, Egueb_Svg_Element *e,
		Egueb_Dom_Node *doc, Egueb_Svg_Color *color)
{
	Egueb_Svg_Painter_Generic *thiz;
	Egueb_Svg_Painter_Generic_Stroke_Dasharray_Data stroke_dasharray_data;
	Egueb_Svg_Paint stroke = EGUEB_SVG_PAINT_INIT;
	Egueb_Svg_Length stroke_width;
	Egueb_Svg_Number stroke_opacity;
	Egueb_Svg_Stroke_Line_Cap stroke_line_cap;
	Egueb_Svg_Stroke_Line_Join stroke_line_join;
	Egueb_Dom_List *stroke_dasharray = NULL;
	double stroke_viewport = 0;
	double font_size;

	egueb_dom_attr_final_get(e->stroke, &stroke);
	egueb_dom_attr_final_get(e->stroke_opacity, &stroke_opacity);
	egueb_dom_attr_final_get(e->stroke_width, &stroke_width);
	egueb_dom_attr_final_get(e->stroke_line_cap, &stroke_line_cap);
	egueb_dom_attr_final_get(e->stroke_line_join, &stroke_line_join);
	egueb_dom_attr_final_get(e->stroke_dasharray, &stroke_dasharray);

	thiz = EGUEB_SVG_PAINTER_GENERIC(p);
	_egueb_svg_renderable_paint_set(EGUEB_DOM_NODE(e), doc,
			&p->draw_mode,
			&p->stroke_color,
			&p->stroke_renderer,
			ENESIM_RENDERER_SHAPE_DRAW_MODE_STROKE,
			stroke_opacity,
			color,
			&thiz->stroke,
			&stroke,
			&thiz->stroke_paint_last);
	/* the stroke dash */
	if (p->stroke_dasharray)
	{
		Enesim_Renderer_Shape_Stroke_Dash *dash;
		EINA_LIST_FREE(p->stroke_dasharray, dash)
			free(dash);
	}
	egueb_svg_document_font_size_get(doc, &font_size);

	stroke_dasharray_data.e = e;
	stroke_dasharray_data.p = p;
	stroke_dasharray_data.font_size = font_size;
	stroke_dasharray_data.dash = NULL;
	egueb_dom_list_foreach(stroke_dasharray, _egueb_svg_painter_generic_dash_foreach, &stroke_dasharray_data);
	/* handle the odd/even thing */
	if (egueb_dom_list_length(stroke_dasharray) % 2)
		egueb_dom_list_foreach(stroke_dasharray, _egueb_svg_painter_generic_dash_foreach, &stroke_dasharray_data);
	/* the stroke types */
	p->stroke_cap = stroke_line_cap;
	p->stroke_join = stroke_line_join;
	/* handle the stroke width */
	if (stroke_width.unit == EGUEB_SVG_UNIT_LENGTH_PERCENT)
	{
		stroke_viewport = hypot(e->viewbox.w, e->viewbox.h) / M_SQRT2;
	}
	p->stroke_weight = egueb_svg_coord_final_get(&stroke_width,
			stroke_viewport, font_size);
	egueb_svg_paint_reset(&stroke);
}

/*----------------------------------------------------------------------------*
 *                             Painter interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_painter_generic_resolve(Egueb_Svg_Painter *p,
		Egueb_Svg_Element *e)
{
	Egueb_Svg_Color color;
	Egueb_Svg_Number opacity;
	Egueb_Dom_Node *doc;
	uint8_t lopacity;

	_egueb_svg_painter_generic_cleanup(p);

	egueb_dom_node_document_get(EGUEB_DOM_NODE(e), &doc);
	if (!doc)
	{
		WARN("No document set");
		return EINA_FALSE;
	}
	_egueb_svg_painter_generic_resolve_visibility(p, e);

	egueb_dom_attr_final_get(e->opacity, &opacity);
	egueb_dom_attr_final_get(e->color, &color);
	/* calculate the opacity */
	lopacity = opacity * 255;
	enesim_color_components_from(&p->color,
			lopacity, 0xff, 0xff, 0xff);
	/* set the fill */
	_egueb_svg_painter_generic_resolve_fill(p, e, doc, &color);
	/* set the stroke */
	_egueb_svg_painter_generic_resolve_stroke(p, e, doc, &color);
	egueb_dom_node_unref(doc);

	/* finally dump the painter */
	INFO("Common visibility: %d, opacity %08x", p->visibility, p->color);
	INFO("Shape draw_mode: %08x", p->draw_mode);
	INFO("Fill color: %08x, renderer: %p", p->fill_color, p->fill_renderer);
	INFO("Stroke color: %08x, renderer: %p", p->stroke_color, p->stroke_renderer);
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_PAINTER_DESCRIPTOR,
		Egueb_Svg_Painter_Generic, Egueb_Svg_Painter_Generic_Class,
		egueb_svg_painter_generic);

static void _egueb_svg_painter_generic_class_init(void *k)
{
	Egueb_Svg_Painter_Class *klass;

	klass = EGUEB_SVG_PAINTER_CLASS(k);
	klass->resolve = _egueb_svg_painter_generic_resolve;
}

static void _egueb_svg_painter_generic_instance_init(void *o)
{
}

static void _egueb_svg_painter_generic_instance_deinit(void *o)
{
	Egueb_Svg_Painter_Generic *thiz;

	/* the painter abstract will destroy the renderers */
	/* now remove the references */
	thiz = EGUEB_SVG_PAINTER_GENERIC(o);
	if (thiz->stroke)
	{
		egueb_svg_reference_free(thiz->stroke);
		thiz->stroke = NULL;
	}
	if (thiz->fill)
	{
		egueb_svg_reference_free(thiz->fill);
		thiz->fill = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Painter * egueb_svg_painter_generic_new(void)
{
	Egueb_Svg_Painter *p;

	p = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_painter_generic);
	return p;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
