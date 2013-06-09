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
#include "egueb_svg_attr_matrix.h"
#include "egueb_svg_renderable_container_private.h"
#include "egueb_svg_element_g.h"
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

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_g_tag_name_get;
}

static void _egueb_svg_element_g_class_deinit(void *k)
{
}

static void _egueb_svg_element_g_instance_init(void *o)
{
}

static void _egueb_svg_element_g_instance_deinit(void *o)
{
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


#if 0
static Eina_Bool _egueb_svg_element_g_setup_post(Egueb_Dom_Tag *t,
		Egueb_Dom_Tag *child,
		Egueb_Svg_Context *c,
		Enesim_Log **error,
		void *data)
{
	Egueb_Svg_Type type;
	Egueb_Svg_Element_G *thiz = data;

	type = egueb_svg_element_internal_type_get(child);
	/* check if it is a renderable */
	if (egueb_svg_type_is_renderable(type) && thiz->renderable_tree_changed)
	{
		const Egueb_Svg_Element_Context *child_ctx;
		Enesim_Renderer *child_r = NULL;

		/* get the context of the element */
		child_ctx = egueb_svg_element_context_get(child);
		/* get the max bounds */
		{
			double l, r, t, b;

			l = child_ctx->bounds.x;
			r = child_ctx->bounds.x + child_ctx->bounds.w - 1;
			t = child_ctx->bounds.y;
			b = child_ctx->bounds.y + child_ctx->bounds.h - 1;

			if (l < thiz->ll) thiz->ll = l;
			if (t < thiz->tt) thiz->tt = t;
			if (r > thiz->rr) thiz->rr = r;
			if (b > thiz->bb) thiz->bb = b;
		}
		/* add to the compound */
		egueb_svg_renderable_internal_renderer_get(child, &child_r);
		enesim_renderer_compound_layer_add(thiz->r, child_r);
		/* add it to the container */
		egueb_svg_renderable_container_renderable_add(thiz->container, child);
	}
 	else if (type == ESVG_TYPE_A)
	{
		return egueb_svg_element_internal_child_setup(child, c, error,
				NULL, _egueb_svg_element_g_setup_post, thiz);
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                          The Container interface                           *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_g_initialize(Ender_Element *e)
{
	Egueb_Svg_Element_G *thiz;
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_g_get(t);

	thiz->container = egueb_svg_renderable_container_new(e);
}

static Eina_Bool _egueb_svg_element_g_child_add(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element_G *thiz;
	Egueb_Svg_Type type;

	thiz = _egueb_svg_element_g_get(t);
	if (!egueb_svg_is_element_internal(child))
		return EINA_FALSE;

	type = egueb_svg_element_internal_type_get(child);
	if (egueb_svg_type_is_renderable(type) || type == ESVG_TYPE_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
	}

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_g_child_remove(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element_G *thiz;
	Egueb_Svg_Type type;

	thiz = _egueb_svg_element_g_get(t);
	if (!egueb_svg_is_element_internal(child))
		return EINA_FALSE;

	type = egueb_svg_element_internal_type_get(child);
	if (egueb_svg_type_is_renderable(type) || type == ESVG_TYPE_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
	}

	return EINA_TRUE;
}

static Enesim_Renderer * _egueb_svg_element_g_renderer_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_G *thiz;

	thiz = _egueb_svg_element_g_get(t);
	return thiz->r;
}

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_g_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element_G *thiz;
	Egueb_Svg_Element_Setup_Return ret;

	thiz = _egueb_svg_element_g_get(t);
	if (thiz->renderable_tree_changed)
	{
		egueb_svg_renderable_container_clear(thiz->container);
		enesim_renderer_compound_layer_clear(thiz->r);
		/* reset our internal bounds */
		thiz->ll = thiz->tt = INT_MAX;
		thiz->rr = thiz->bb = -INT_MAX;
	}
	ret = egueb_svg_element_internal_child_setup(t, c, error, NULL, _egueb_svg_element_g_setup_post, thiz);
	enesim_rectangle_coords_from(&ctx->bounds, thiz->ll, thiz->tt, thiz->ll + thiz->rr + 1, thiz->tt + thiz->bb + 1);
	DBG("Final bounds %" ENESIM_RECTANGLE_FORMAT, ENESIM_RECTANGLE_ARGS(&ctx->bounds));
	thiz->renderable_tree_changed = EINA_FALSE;

	return ret;
}

static void _egueb_svg_element_g_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_G *thiz;

	thiz = _egueb_svg_element_g_get(t);
	enesim_renderer_unref(thiz->r);
	free(thiz);
}

static Eina_Bool _egueb_svg_element_g_renderer_propagate(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		Egueb_Svg_Renderable_Context *rctx,
		Enesim_Log **error)
{
	Egueb_Svg_Element_G *thiz;

	thiz = _egueb_svg_element_g_get(t);
	enesim_renderer_color_set(thiz->r, rctx->color);
	return EINA_TRUE;
}

static Egueb_Svg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ _egueb_svg_element_g_child_add,
	/* .child_remove	= */ _egueb_svg_element_g_child_remove,
	/* .attribute_get 	= */ NULL,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _egueb_svg_element_g_free,
	/* .initialize 		= */ _egueb_svg_element_g_initialize,
	/* .attribute_set 	= */ NULL,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ _egueb_svg_element_g_setup,
	/* .renderer_get	= */ _egueb_svg_element_g_renderer_get,
	/* .renderer_propagate	= */ _egueb_svg_element_g_renderer_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_svg_element_g_new(void)
{
	Egueb_Svg_Element_G *thiz;
	Egueb_Dom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Egueb_Svg_Element_G));
	if (!thiz) return NULL;

	r = enesim_renderer_compound_new();
	thiz->r = r;
	enesim_renderer_rop_set(r, ENESIM_BLEND);

	t = egueb_svg_renderable_new(&_descriptor, ESVG_TYPE_G, thiz);
	return t;
}
/* The ender wrapper */
#define _egueb_svg_element_g_delete NULL
#include "egueb_svg_generated_element_g.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_element_g_init(void)
{
	_egueb_svg_element_g_log = eina_log_domain_register("egueb_svg_element_g", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_element_g_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_element_g_init();
}

void egueb_svg_element_g_shutdown(void)
{
	if (_egueb_svg_element_g_log < 0)
		return;
	_egueb_svg_element_g_shutdown();
	eina_log_domain_unregister(_egueb_svg_element_g_log);
	_egueb_svg_element_g_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * egueb_svg_element_g_new(void)
{
	return ESVG_ELEMENT_NEW("SVGGElement");
}

EAPI Eina_Bool egueb_svg_is_g(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Type type;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	type = egueb_svg_element_internal_type_get(t);
	return (type == ESVG_TYPE_G) ? EINA_TRUE : EINA_FALSE;
}
#endif
