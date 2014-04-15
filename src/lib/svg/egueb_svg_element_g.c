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
#include "egueb_svg_element_g.h"

#include "egueb_svg_renderable_container_private.h"
#include "egueb_svg_attr_matrix_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_G_DESCRIPTOR egueb_svg_element_g_descriptor_get()
#define EGUEB_SVG_ELEMENT_G_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_G_Class, EGUEB_SVG_ELEMENT_G_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_G(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_G, EGUEB_SVG_ELEMENT_G_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_G
{
	Egueb_Svg_Renderable_Container base;
	Egueb_Svg_Painter *painter;
} Egueb_Svg_Element_G;

typedef struct _Egueb_Svg_Element_G_Class
{
	Egueb_Svg_Renderable_Container_Class base;
} Egueb_Svg_Element_G_Class;

static Eina_Bool _egueb_svg_element_g_bounds_cb(
		Egueb_Dom_Node *child, void *data)
{
	Egueb_Svg_Renderable *r;
	Enesim_Rectangle *bounds = data;
	Enesim_Matrix m;
	Enesim_Rectangle tmp;
	Enesim_Quad q;
	double nx1, nx2, ny1, ny2;

	if (!egueb_svg_is_renderable(child))
		return EINA_TRUE;

	egueb_svg_renderable_bounds_get(child, &tmp);
	/* multiply by the transform matrix of the child */
	r = EGUEB_SVG_RENDERABLE(child);
	egueb_dom_attr_final_get(r->transform, &m);
	enesim_matrix_rectangle_transform(&m, &tmp, &q);
	enesim_quad_rectangle_to(&q, &tmp);

	nx1 = tmp.x;
	ny1 = tmp.y;
	nx2 = tmp.x + tmp.w;
	ny2 = tmp.y + tmp.h;

	/* pick the bigger area */
	/* we use w and h as x2 and y2 */
	if (nx1 < bounds->x) bounds->x = nx1;
	if (ny1 < bounds->y) bounds->y = ny1;
	if (nx2 > bounds->w) bounds->w = nx2;
	if (ny2 > bounds->h) bounds->h = ny2;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_g_painter_apply(Egueb_Svg_Renderable *r,
		Egueb_Svg_Painter *painter)
{
	Eina_Bool visibility;
	Enesim_Renderer *ren;

	ren = egueb_svg_renderable_class_renderer_get(EGUEB_DOM_NODE(r));
	if (!ren)
	{
		WARN("No renderer available");
		return;
	}
	egueb_svg_painter_visibility_get(painter, &visibility);
	enesim_renderer_visibility_set(ren, visibility);
}

static Egueb_Svg_Painter * _egueb_svg_element_g_painter_get(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_G *thiz;

	thiz = EGUEB_SVG_ELEMENT_G(r);
	return egueb_svg_painter_ref(thiz->painter);
}

static void _egueb_svg_element_g_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
	Enesim_Rectangle tmp;

	enesim_rectangle_coords_from(&tmp, INT_MAX, INT_MAX, -INT_MAX, -INT_MAX);
	/* iterate over the children and call the setup there */
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(r),
			_egueb_svg_element_g_bounds_cb, &tmp);
	enesim_rectangle_coords_from(bounds, tmp.x, tmp.y, tmp.w - tmp.x, tmp.h - tmp.y);
	DBG("Bounds %" ENESIM_RECTANGLE_FORMAT, ENESIM_RECTANGLE_ARGS(bounds));
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_g_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_G);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_RENDERABLE_CONTAINER_DESCRIPTOR,
		Egueb_Svg_Element_G, Egueb_Svg_Element_G_Class,
		egueb_svg_element_g);

static void _egueb_svg_element_g_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->bounds_get = _egueb_svg_element_g_bounds_get;
	r_klass->painter_get = _egueb_svg_element_g_painter_get;
	r_klass->painter_apply = _egueb_svg_element_g_painter_apply;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_g_tag_name_get;
}

static void _egueb_svg_element_g_instance_init(void *o)
{
	Egueb_Svg_Element_G *thiz;

	thiz = EGUEB_SVG_ELEMENT_G(o);
	thiz->painter = egueb_svg_painter_g_new();
}

static void _egueb_svg_element_g_instance_deinit(void *o)
{
	Egueb_Svg_Element_G *thiz;

	thiz = EGUEB_SVG_ELEMENT_G(o);
	egueb_svg_painter_unref(thiz->painter);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Egueb_Dom_Node * egueb_svg_element_g_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_g);
	return n;
}
