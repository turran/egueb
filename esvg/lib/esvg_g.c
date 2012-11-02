/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_private_renderable_container.h"
#include "esvg_g.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_g

typedef struct _Esvg_G
{
	/* properties */
	/* private */
	Esvg_Renderable_Container *container;
	/* keep track if the renderable tree has changed, includeing the <a> tag */
	Eina_Bool renderable_tree_changed : 1;
	/* our renderer */
	Enesim_Renderer *r;
	/* the boundings */
	double ll, rr, tt, bb;
} Esvg_G;

static Esvg_G * _esvg_g_get(Edom_Tag *t)
{
	Esvg_G *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_G)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}

static Eina_Bool _esvg_g_setup_post(Edom_Tag *t,
		Edom_Tag *child,
		Esvg_Context *c,
		Enesim_Error **error,
		void *data)
{
	Esvg_Type type;
	Esvg_G *thiz = data;

	type = esvg_element_internal_type_get(child);
	/* check if it is a renderable */
	if (esvg_type_is_renderable(type) && thiz->renderable_tree_changed)
	{
		const Esvg_Element_Context *child_ctx;
		Enesim_Renderer *child_r = NULL;

		/* get the context of the element */
		child_ctx = esvg_element_context_get(child);
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
		esvg_renderable_internal_renderer_get(child, &child_r);
		enesim_renderer_compound_layer_add(thiz->r, child_r);
		/* add it to the container */
		esvg_renderable_container_renderable_add(thiz->container, child);
	}
 	else if (type == ESVG_A)
	{
		return esvg_element_internal_child_setup(child, c, error,
				NULL, _esvg_g_setup_post, thiz);
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                          The Container interface                           *
 *----------------------------------------------------------------------------*/
static void _esvg_g_initialize(Ender_Element *e)
{
	Esvg_G *thiz;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_g_get(t);

	thiz->container = esvg_renderable_container_new(e);
}

static Eina_Bool _esvg_g_child_add(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_G *thiz;
	Esvg_Type type;

	thiz = _esvg_g_get(t);
	if (!esvg_is_element_internal(child))
		return EINA_FALSE;

	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) || type == ESVG_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_g_child_remove(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_G *thiz;
	Esvg_Type type;

	thiz = _esvg_g_get(t);
	if (!esvg_is_element_internal(child))
		return EINA_FALSE;

	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type) || type == ESVG_A)
	{
		thiz->renderable_tree_changed = EINA_TRUE;
	}

	return EINA_TRUE;
}

static Enesim_Renderer * _esvg_g_renderer_get(Edom_Tag *t)
{
	Esvg_G *thiz;

	thiz = _esvg_g_get(t);
	return thiz->r;
}

static Esvg_Element_Setup_Return _esvg_g_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_G *thiz;
	Esvg_Element_Setup_Return ret;

	thiz = _esvg_g_get(t);
	if (thiz->renderable_tree_changed)
	{
		esvg_renderable_container_clear(thiz->container);
		enesim_renderer_compound_layer_clear(thiz->r);
		/* reset our internal bounds */
		thiz->ll = thiz->tt = INT_MAX;
		thiz->rr = thiz->bb = -INT_MAX;
	}
	ret = esvg_element_internal_child_setup(t, c, error, NULL, _esvg_g_setup_post, thiz);
	enesim_rectangle_coords_from(&ctx->bounds, thiz->ll, thiz->tt, thiz->ll + thiz->rr + 1, thiz->tt + thiz->bb + 1);
	DBG("Final bounds %" ENESIM_RECTANGLE_FORMAT, ENESIM_RECTANGLE_ARGS(&ctx->bounds));
	thiz->renderable_tree_changed = EINA_FALSE;

	return ret;
}

static void _esvg_g_free(Edom_Tag *t)
{
	Esvg_G *thiz;

	thiz = _esvg_g_get(t);
	enesim_renderer_unref(thiz->r);
	free(thiz);
}

static Eina_Bool _esvg_g_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_G *thiz;

	thiz = _esvg_g_get(t);
	enesim_renderer_color_set(thiz->r, rctx->color);
	return EINA_TRUE;
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ _esvg_g_child_add,
	/* .child_remove	= */ _esvg_g_child_remove,
	/* .attribute_get 	= */ NULL,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _esvg_g_free,
	/* .initialize 		= */ _esvg_g_initialize,
	/* .attribute_set 	= */ NULL,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ _esvg_g_setup,
	/* .renderer_get	= */ _esvg_g_renderer_get,
	/* .renderer_propagate	= */ _esvg_g_renderer_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_g_new(void)
{
	Esvg_G *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_G));
	if (!thiz) return NULL;

	r = enesim_renderer_compound_new();
	thiz->r = r;
	enesim_renderer_rop_set(r, ENESIM_BLEND);

	t = esvg_renderable_new(&_descriptor, ESVG_G, thiz);
	return t;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_g_delete NULL
#include "generated/esvg_generated_g.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_g_new(void)
{
	return ESVG_ELEMENT_NEW("g");
}

EAPI Eina_Bool esvg_is_g(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_G) ? EINA_TRUE : EINA_FALSE;
}
