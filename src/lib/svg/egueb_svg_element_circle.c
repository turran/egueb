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
#include "egueb_svg_element_circle.h"
#include "egueb_svg_document.h"
#include "egueb_svg_shape_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_CIRCLE_DESCRIPTOR egueb_svg_element_circle_descriptor_get()
#define EGUEB_SVG_ELEMENT_CIRCLE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Circle_Class, EGUEB_SVG_ELEMENT_CIRCLE_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_CIRCLE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Circle, EGUEB_SVG_ELEMENT_CIRCLE_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Circle
{
	Egueb_Svg_Shape base;
	/* properties */
	Egueb_Dom_Node *cx;
	Egueb_Dom_Node *cy;
	Egueb_Dom_Node *radius;
	/* private */
	Enesim_Renderer *r;
	/* needed for the bounds */
	double gcx, gcy, gradius;
} Egueb_Svg_Element_Circle;

typedef struct _Egueb_Svg_Element_Circle_Class
{
	Egueb_Svg_Shape_Class base;
} Egueb_Svg_Element_Circle_Class;

/*----------------------------------------------------------------------------*
 *                               Shape interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_circle_generate_geometry(Egueb_Svg_Shape *s)
{
	Egueb_Svg_Element_Circle *thiz;
	Egueb_Svg_Element *e, *e_parent;
	Egueb_Svg_Length cx, cy, radius;
	Egueb_Dom_Node *relative, *doc;
	double font_size;

	thiz = EGUEB_SVG_ELEMENT_CIRCLE(s);
	egueb_dom_attr_final_get(thiz->cx, &cx);
	egueb_dom_attr_final_get(thiz->cy, &cy);
	egueb_dom_attr_final_get(thiz->radius, &radius);

	/* calculate the real size */
	egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(s), &relative);
	if (!relative)
	{
		WARN("No relative available");
		return EINA_FALSE;
	}
	egueb_dom_node_document_get(EGUEB_DOM_NODE(s), &doc);
	if (!doc)
	{
		WARN("No document set");
		egueb_dom_node_unref(relative);
		return EINA_FALSE;
	}

	e_parent = EGUEB_SVG_ELEMENT(relative);
	egueb_svg_document_font_size_get(doc, &font_size);

	thiz->gcx = egueb_svg_coord_final_get(&cx, e_parent->viewbox.w, font_size);
	thiz->gcy = egueb_svg_coord_final_get(&cy, e_parent->viewbox.h, font_size);
	thiz->gradius = egueb_svg_length_final_get(&radius, e_parent->viewbox.w, e_parent->viewbox.h, font_size);

	egueb_dom_node_unref(relative);
	egueb_dom_node_unref(doc);

	/* set the position */
	enesim_renderer_circle_center_set(thiz->r, thiz->gcx, thiz->gcy);
	/* set the radius */
	enesim_renderer_circle_radius_set(thiz->r, thiz->gradius);
	/* set the transformation */
	e = EGUEB_SVG_ELEMENT(s);
	enesim_renderer_transformation_set(thiz->r, &e->transform);
	/* update the viewbox */
	/* set the bounds */
	enesim_rectangle_coords_from(&e->viewbox,
			thiz->gcx - thiz->gradius,
			thiz->gcy - thiz->gradius,
			thiz->gradius * 2, thiz->gradius * 2);

	DBG("cx: %g, cy: %g, radius: %g", thiz->gcx, thiz->gcy, thiz->gradius);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_element_circle_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Circle *thiz;

	thiz = EGUEB_SVG_ELEMENT_CIRCLE(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_circle_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Element_Circle *thiz;

	thiz = EGUEB_SVG_ELEMENT_CIRCLE(r);
	enesim_rectangle_coords_from(bounds,
			thiz->gcx - thiz->gradius,
			thiz->gcy - thiz->gradius,
			thiz->gradius * 2, thiz->gradius * 2);
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_circle_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_CIRCLE);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_circle, Egueb_Svg_Element_Circle, cx);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_circle, Egueb_Svg_Element_Circle, cy);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_circle, Egueb_Svg_Element_Circle, radius);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_SHAPE_DESCRIPTOR,
		Egueb_Svg_Element_Circle, Egueb_Svg_Element_Circle_Class,
		egueb_svg_element_circle);

static void _egueb_svg_element_circle_class_init(void *k)
{
	Egueb_Svg_Shape_Class *klass;
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_SHAPE_CLASS(k);
	klass->generate_geometry = _egueb_svg_element_circle_generate_geometry;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->renderer_get = _egueb_svg_element_circle_renderer_get;
	r_klass->bounds_get = _egueb_svg_element_circle_bounds_get;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_circle_tag_name_get;
}

static void _egueb_svg_element_circle_class_deinit(void *k)
{
}

static void _egueb_svg_element_circle_instance_init(void *o)
{
	Egueb_Svg_Element_Circle *thiz;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_CIRCLE(o);
	r = enesim_renderer_circle_new();
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
	thiz->radius = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_R),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_circle, cx);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_circle, cy);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_circle, radius);
}

static void _egueb_svg_element_circle_instance_deinit(void *o)
{
	Egueb_Svg_Element_Circle *thiz;

	thiz = EGUEB_SVG_ELEMENT_CIRCLE(o);
	enesim_renderer_unref(thiz->r);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->cx);
	egueb_dom_node_unref(thiz->cy);
	egueb_dom_node_unref(thiz->radius);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_circle_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_circle);
	return n;
}

#if 0
EAPI Eina_Bool egueb_svg_is_circle(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Type type;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	type = egueb_svg_element_internal_type_get(t);
	return (type == ESVG_TYPE_CIRCLE) ? EINA_TRUE : EINA_FALSE;
}

EAPI void egueb_svg_element_circle_cx_set(Ender_Element *e, const Egueb_Svg_Coord *cx)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_CIRCLE_CX, cx);
}

EAPI void egueb_svg_element_circle_cx_get(Ender_Element *e, Egueb_Svg_Coord *cx)
{
}

EAPI void egueb_svg_element_circle_cy_set(Ender_Element *e, const Egueb_Svg_Coord *cy)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_CIRCLE_CY, cy);
}

EAPI void egueb_svg_element_circle_cy_get(Ender_Element *e, Egueb_Svg_Coord *cy)
{
}

EAPI void egueb_svg_element_circle_r_set(Ender_Element *e, const Egueb_Svg_Length *r)
{
	egueb_svg_element_property_length_set(e, ESVG_ELEMENT_CIRCLE_R, r);
}

EAPI void egueb_svg_element_circle_r_get(Ender_Element *e, Egueb_Svg_Length *r)
{
}
#endif
