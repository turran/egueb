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
#include "egueb_svg_element_line.h"
#include "egueb_svg_document.h"
#include "egueb_svg_shape_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_LINE_DESCRIPTOR egueb_svg_element_line_descriptor_get()
#define EGUEB_SVG_ELEMENT_LINE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Line_Class, EGUEB_SVG_ELEMENT_LINE_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_LINE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Line, EGUEB_SVG_ELEMENT_LINE_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Line
{
	Egueb_Svg_Shape base;
	/* properties */
	Egueb_Dom_Node *x1;
	Egueb_Dom_Node *y1;
	Egueb_Dom_Node *x2;
	Egueb_Dom_Node *y2;
	/* private */
	Enesim_Renderer *r;
	/* needed for the bounds */
	double gx1, gy1, gx2, gy2;
} Egueb_Svg_Element_Line;

typedef struct _Egueb_Svg_Element_Line_Class
{
	Egueb_Svg_Shape_Class base;
} Egueb_Svg_Element_Line_Class;

/*----------------------------------------------------------------------------*
 *                               Shape interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_line_generate_geometry(Egueb_Svg_Shape *s,
		Egueb_Svg_Element *relative, Egueb_Dom_Node *doc)
{
	Egueb_Svg_Element_Line *thiz;
	Egueb_Svg_Element *e;
	Egueb_Svg_Length x1, y1, x2, y2;
	double font_size;

	thiz = EGUEB_SVG_ELEMENT_LINE(s);
	egueb_dom_attr_final_get(thiz->x1, &x1);
	egueb_dom_attr_final_get(thiz->y1, &y1);
	egueb_dom_attr_final_get(thiz->x2, &x2);
	egueb_dom_attr_final_get(thiz->y2, &y2);

	/* calculate the real size */
	egueb_svg_document_font_size_get(doc, &font_size);
	thiz->gx1 = egueb_svg_coord_final_get(&x1, relative->viewbox.w, font_size);
	thiz->gy1 = egueb_svg_coord_final_get(&y1, relative->viewbox.h, font_size);
	thiz->gx2 = egueb_svg_coord_final_get(&x2, relative->viewbox.w, font_size);
	thiz->gy2 = egueb_svg_coord_final_get(&y2, relative->viewbox.h, font_size);

	/* set the position */
	enesim_renderer_line_x0_set(thiz->r, thiz->gx1);
	enesim_renderer_line_y0_set(thiz->r, thiz->gy1);
	enesim_renderer_line_x1_set(thiz->r, thiz->gx2);
	enesim_renderer_line_y1_set(thiz->r, thiz->gy2);

	/* set the transformation */
	e = EGUEB_SVG_ELEMENT(s);
	enesim_renderer_transformation_set(thiz->r, &e->transform);
	/* update the viewbox */
	enesim_rectangle_coords_from(&e->viewbox,
			thiz->gx1 < thiz->gx2 ? thiz->gx1 : thiz->gx2,
			thiz->gy1 < thiz->gy2 ? thiz->gy1 : thiz->gy2,
			fabs(thiz->gx1 - thiz->gx2),
			fabs(thiz->gy1 - thiz->gy2));
 
	DBG("x1: %g, y1: %g, x2: %g, y2: %g", thiz->gx1, thiz->gy1, thiz->gx2, thiz->gy2);

	return EINA_TRUE;
}

static void _egueb_svg_element_line_renderer_propagate(Egueb_Svg_Shape *s,
		Egueb_Svg_Painter *painter)
{
	Egueb_Svg_Element_Line *thiz;
	Enesim_Color color;
	Enesim_Shape_Stroke_Cap stroke_cap;
	Enesim_Shape_Stroke_Join stroke_join;
	Enesim_Color stroke_color;
	Enesim_Renderer *stroke_renderer;
	Eina_Bool visibility;
	double stroke_weight;

	thiz = EGUEB_SVG_ELEMENT_LINE(s);

	egueb_svg_painter_visibility_get(painter, &visibility);
	egueb_svg_painter_color_get(painter, &color);
	egueb_svg_painter_stroke_cap_get(painter, &stroke_cap);
	egueb_svg_painter_stroke_join_get(painter, &stroke_join);
	egueb_svg_painter_stroke_color_get(painter, &stroke_color);
	egueb_svg_painter_stroke_weight_get(painter, &stroke_weight);
	egueb_svg_painter_stroke_renderer_get(painter, &stroke_renderer);

	/* overrides */
	enesim_renderer_shape_draw_mode_set(thiz->r, ENESIM_SHAPE_DRAW_MODE_STROKE);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);

	/* shape properties */
	enesim_renderer_shape_stroke_color_set(thiz->r, stroke_color);
	enesim_renderer_shape_stroke_weight_set(thiz->r, stroke_weight);
	enesim_renderer_shape_stroke_cap_set(thiz->r, stroke_cap);
	enesim_renderer_shape_stroke_join_set(thiz->r, stroke_join);

	/* base properties */
	enesim_renderer_color_set(thiz->r, color);
	enesim_renderer_visibility_set(thiz->r, visibility);
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_element_line_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Line *thiz;

	thiz = EGUEB_SVG_ELEMENT_LINE(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_line_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Element_Line *thiz;

	thiz = EGUEB_SVG_ELEMENT_LINE(r);
	enesim_rectangle_coords_from(bounds,
			thiz->gx1 < thiz->gx2 ? thiz->gx1 : thiz->gx2,
			thiz->gy1 < thiz->gy2 ? thiz->gy1 : thiz->gy2,
			fabs(thiz->gx1 - thiz->gx2),
			fabs(thiz->gy1 - thiz->gy2));
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_line_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_LINE);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_line, Egueb_Svg_Element_Line, x1);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_line, Egueb_Svg_Element_Line, y1);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_line, Egueb_Svg_Element_Line, x2);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_line, Egueb_Svg_Element_Line, y2);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_SHAPE_DESCRIPTOR,
		Egueb_Svg_Element_Line, Egueb_Svg_Element_Line_Class,
		egueb_svg_element_line);

static void _egueb_svg_element_line_class_init(void *k)
{
	Egueb_Svg_Shape_Class *klass;
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_SHAPE_CLASS(k);
	klass->generate_geometry = _egueb_svg_element_line_generate_geometry;
	klass->renderer_propagate = _egueb_svg_element_line_renderer_propagate;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->renderer_get = _egueb_svg_element_line_renderer_get;
	r_klass->bounds_get = _egueb_svg_element_line_bounds_get;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_line_tag_name_get;
}

static void _egueb_svg_element_line_class_deinit(void *k)
{
}

static void _egueb_svg_element_line_instance_init(void *o)
{
	Egueb_Svg_Element_Line *thiz;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_LINE(o);
	r = enesim_renderer_line_new();
	thiz->r = r;

	/* Default values */
	enesim_renderer_rop_set(thiz->r, ENESIM_BLEND);

	/* create the properties */
	thiz->x1 = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X1),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y1 = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y1),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->x2 = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X2),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y2 = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y2),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_line, x1);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_line, y1);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_line, x2);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_line, y2);
}

static void _egueb_svg_element_line_instance_deinit(void *o)
{
	Egueb_Svg_Element_Line *thiz;

	thiz = EGUEB_SVG_ELEMENT_LINE(o);
	enesim_renderer_unref(thiz->r);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->x1);
	egueb_dom_node_unref(thiz->y1);
	egueb_dom_node_unref(thiz->x2);
	egueb_dom_node_unref(thiz->y2);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_line_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_line);
	return n;
}

#if 0
EAPI void egueb_svg_element_line_x1_set(Ender_Element *e, const Egueb_Svg_Coord *x1)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_LINE_X1, x1);
}

EAPI void egueb_svg_element_line_x1_get(Ender_Element *e, Egueb_Svg_Coord *x1)
{
}

EAPI void egueb_svg_element_line_y1_set(Ender_Element *e, const Egueb_Svg_Coord *y1)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_LINE_Y1, y1);
}

EAPI void egueb_svg_element_line_y1_get(Ender_Element *e, Egueb_Svg_Coord *y1)
{
}

EAPI void egueb_svg_element_line_x2_set(Ender_Element *e, const Egueb_Svg_Coord *x2)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_LINE_X2, x2);
}

EAPI void egueb_svg_element_line_x2_get(Ender_Element *e, Egueb_Svg_Coord *x2)
{
}

EAPI void egueb_svg_element_line_y2_set(Ender_Element *e, const Egueb_Svg_Coord *y2)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_LINE_Y2, y2);
}

EAPI void egueb_svg_element_line_y2_get(Ender_Element *e, Egueb_Svg_Coord *y2)
{
}
#endif
