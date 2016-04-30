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
#include "egueb_svg_point.h"
#include "egueb_svg_element_polyline.h"
#include "egueb_svg_document.h"

#include "egueb_svg_shape_private.h"
#include "egueb_svg_attr_point_list_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_POLYLINE_DESCRIPTOR egueb_svg_element_polyline_descriptor_get()
#define EGUEB_SVG_ELEMENT_POLYLINE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Polyline_Class, EGUEB_SVG_ELEMENT_POLYLINE_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_POLYLINE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Polyline, EGUEB_SVG_ELEMENT_POLYLINE_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Polyline
{
	Egueb_Svg_Shape base;
	/* properties */
	Egueb_Dom_Node *points;
	/* private */
	Enesim_Figure *figure;
	Enesim_Renderer *r;
} Egueb_Svg_Element_Polyline;

typedef struct _Egueb_Svg_Element_Polyline_Class
{
	Egueb_Svg_Shape_Class base;
} Egueb_Svg_Element_Polyline_Class;

static Eina_Bool _egueb_svg_element_polyline_points_cb(void *data, void *user_data)
{
	Egueb_Svg_Element_Polyline *thiz = user_data;
	Egueb_Svg_Point *pt = data;

	DBG("Adding point %g,%g", pt->x, pt->y);
	enesim_figure_polygon_vertex_add(thiz->figure, pt->x, pt->y);
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                               Shape interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_polyline_generate_geometry(Egueb_Svg_Shape *s,
		Egueb_Svg_Element *parent, Egueb_Dom_Node *doc)
{
	Egueb_Svg_Element_Polyline *thiz;
	Egueb_Svg_Element *e;
	Egueb_Dom_List *points;

	thiz = EGUEB_SVG_ELEMENT_POLYLINE(s);
	/* TODO Be sure that we modified the points */
	egueb_dom_attr_final_get(thiz->points, &points);
	enesim_figure_clear(thiz->figure);
	if (points)
	{
		enesim_figure_polygon_add(thiz->figure);
		egueb_dom_list_foreach(points, _egueb_svg_element_polyline_points_cb, thiz);
		egueb_dom_list_unref(points);
	}

	e = EGUEB_SVG_ELEMENT(s);
	/* set the transformation */
	enesim_renderer_transformation_set(thiz->r, &e->transform);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_element_polyline_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Polyline *thiz;

	thiz = EGUEB_SVG_ELEMENT_POLYLINE(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_polyline_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_polyline_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_POLYLINE);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_SHAPE_DESCRIPTOR,
		Egueb_Svg_Element_Polyline, Egueb_Svg_Element_Polyline_Class,
		egueb_svg_element_polyline);

static void _egueb_svg_element_polyline_class_init(void *k)
{
	Egueb_Svg_Shape_Class *klass;
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_SHAPE_CLASS(k);
	klass->generate_geometry = _egueb_svg_element_polyline_generate_geometry;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->bounds_get = _egueb_svg_element_polyline_bounds_get;
	r_klass->renderer_get = _egueb_svg_element_polyline_renderer_get;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_polyline_tag_name_get;
}

static void _egueb_svg_element_polyline_instance_init(void *o)
{
	Egueb_Svg_Element_Polyline *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_ELEMENT_POLYLINE(o);
	thiz->r = enesim_renderer_figure_new();
	thiz->figure = enesim_renderer_figure_inner_figure_get(thiz->r);

	/* Default values */

	/* create the properties */
	thiz->points = egueb_svg_attr_point_list_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_POINTS),
			NULL, EINA_TRUE, EINA_FALSE, EINA_FALSE);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->points), NULL);
}

static void _egueb_svg_element_polyline_instance_deinit(void *o)
{
	Egueb_Svg_Element_Polyline *thiz;

	thiz = EGUEB_SVG_ELEMENT_POLYLINE(o);
	enesim_renderer_unref(thiz->r);
	enesim_figure_unref(thiz->figure);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->points);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_polyline_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_polyline);
	return n;
}

#if 0
EAPI void egueb_svg_element_polyline_point_add(Ender_Element *e, Egueb_Svg_Point *p)
{
	ender_element_property_value_add(e, ESVG_ELEMENT_POLYLINE_POINT, p, NULL);
}
#endif
