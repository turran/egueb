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
#include "egueb_svg_element.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_use.h"
#include "egueb_svg_document.h"
#include "egueb_svg_renderable.h"
#include "egueb_svg_renderable_private.h"
#include "egueb_svg_attr_string.h"
/*
 * The 'use' element should be able to create a new instance from another
 * svg tree. Basically we set the 'link' property of the 'use' to the svg
 * tree. Once it is attached on the setup we should clone it if it is not
 * already cloned and render it that new tree as a 'g' element.
 * Given that generated g does not have a parent (but it does have a topmost)
 * we need to propagate every attribute on the use to the g
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_USE_DESCRIPTOR egueb_svg_element_use_descriptor_get()
#define EGUEB_SVG_ELEMENT_USE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Use_Class, EGUEB_SVG_ELEMENT_USE_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_USE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Use, EGUEB_SVG_ELEMENT_USE_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Use
{
	Egueb_Svg_Renderable base;
	/* properties */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	Egueb_Dom_Node *width;
	Egueb_Dom_Node *height;
	Egueb_Dom_Node *xlink_href;
	/* private */
	Enesim_Renderer *r;
	Egueb_Dom_Node *g;
	Egueb_Dom_Node *clone;
	/* the always present g tag */
	Egueb_Dom_String *last_xlink;
} Egueb_Svg_Element_Use;

typedef struct _Egueb_Svg_Element_Use_Class
{
	Egueb_Svg_Renderable_Class base;
} Egueb_Svg_Element_Use_Class;

/* Whenever the node has been removed from the document we need to make
 * sure that the <g> node also has the same document
 */
/*----------------------------------------------------------------------------*
 *                             Renderable interface                           *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_element_use_renderer_get(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(r);
	return enesim_renderer_ref(thiz->r);
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_use_process(Egueb_Svg_Element *e)
{
	Egueb_Svg_Element_Use *thiz;
	Egueb_Svg_Element *e_parent;
	Egueb_Svg_Length x, y, w, h;
	Egueb_Dom_String *xlink = NULL;
	Egueb_Dom_Node *relative, *doc;
	double gx, gy, gw, gh;
	double font_size;

	/* first set the geometry */
	thiz = EGUEB_SVG_ELEMENT_USE(e);
	egueb_dom_attr_final_get(thiz->x, &x);
	egueb_dom_attr_final_get(thiz->y, &y);
	egueb_dom_attr_final_get(thiz->width, &w);
	egueb_dom_attr_final_get(thiz->height, &h);
	egueb_dom_attr_final_get(thiz->xlink_href, &xlink);

	/* calculate the real size */
	egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(e), &relative);
	if (!relative)
	{
		WARN("No relative available");
		return EINA_FALSE;
	}
	egueb_dom_node_document_get(EGUEB_DOM_NODE(e), &doc);
	if (!doc)
	{
		WARN("No document set");
		egueb_dom_node_unref(relative);
		return EINA_FALSE;
	}

	e_parent = EGUEB_SVG_ELEMENT(relative);
	egueb_svg_document_font_size_get(doc, &font_size);

	gx = egueb_svg_coord_final_get(&x, e_parent->viewbox.w, font_size);
	gy = egueb_svg_coord_final_get(&y, e_parent->viewbox.h, font_size);
	gw = egueb_svg_coord_final_get(&w, e_parent->viewbox.w, font_size);
	gh = egueb_svg_coord_final_get(&h, e_parent->viewbox.h, font_size);

	egueb_dom_node_unref(relative);

	/* set the transformation */
	enesim_renderer_transformation_set(thiz->r, &e->transform);
	/* update the viewbox */
	enesim_rectangle_coords_from(&e->viewbox, gx, gy, gw, gh);

	/* in case the xlink attribute has changed, remove the clone
	 * and create a new one. if the clone has changed, remove
	 * it and create a new one too
	 */
	if (!egueb_dom_string_is_equal(xlink, thiz->last_xlink) ||
			(thiz->clone && egueb_dom_element_changed(thiz->clone)))
	{
		if (thiz->clone)
		{
			/* remove the clone, this will remove the renderer automatically */
			egueb_dom_node_child_remove(thiz->g, thiz->clone);
			egueb_dom_node_unref(thiz->clone);
			thiz->clone = NULL;
		}

		/* finally clone the reference */
		if (egueb_svg_document_iri_clone(doc, xlink, &thiz->clone) == EINA_ERROR_NONE)
		{
			/* add it to the g */
			egueb_dom_node_child_append(thiz->g, egueb_dom_node_ref(thiz->clone));
			/* process this element and set its relativeness */
			egueb_dom_element_process(thiz->clone);
		}
	}
	egueb_dom_node_unref(doc);

	/* finally swap */
	if (thiz->last_xlink)
	{
		egueb_dom_string_unref(thiz->last_xlink);
		thiz->last_xlink = NULL;
	}
	thiz->last_xlink = xlink;

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_use, Egueb_Svg_Element_Use, x);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_use, Egueb_Svg_Element_Use, y);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_use, Egueb_Svg_Element_Use, width);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_use, Egueb_Svg_Element_Use, height);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_use, Egueb_Svg_Element_Use, xlink_href);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_RENDERABLE_DESCRIPTOR,
		Egueb_Svg_Element_Use, Egueb_Svg_Element_Use_Class,
		egueb_svg_element_use);

static void _egueb_svg_element_use_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *klass;
	Egueb_Svg_Element_Class *e_klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	klass->renderer_get = _egueb_svg_element_use_renderer_get;

	e_klass = EGUEB_SVG_ELEMENT_CLASS(k);
	e_klass->process = _egueb_svg_element_use_process;
}

static void _egueb_svg_element_use_class_deinit(void *k)
{
}

static void _egueb_svg_element_use_instance_init(void *o)
{
	Egueb_Svg_Element_Use *thiz;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_USE(o);
	r = enesim_renderer_rectangle_new();
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->r = r;

	thiz->g = egueb_svg_element_g_new();
	r = egueb_svg_renderable_renderer_get(thiz->g);
	enesim_renderer_shape_fill_renderer_set(thiz->r, r);
	enesim_renderer_shape_draw_mode_set(thiz->r, ENESIM_SHAPE_DRAW_MODE_FILL);

	/* Default values */
	enesim_renderer_rop_set(thiz->r, ENESIM_BLEND);

	/* create the properties */
	thiz->x = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->width = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_WIDTH),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->height = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_HEIGHT),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_XLINK_HREF),
			NULL, EINA_TRUE, EINA_FALSE, EINA_FALSE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_use, x);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_use, y);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_use, width);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_use, height);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_use, xlink_href);
}

static void _egueb_svg_element_use_instance_deinit(void *o)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(o);

	egueb_dom_node_unref(thiz->g);
	enesim_renderer_unref(thiz->r);

	if (thiz->last_xlink)
	{
		egueb_dom_string_unref(thiz->last_xlink);
		thiz->last_xlink = NULL;
	}

	/* destroy the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
	egueb_dom_node_unref(thiz->width);
	egueb_dom_node_unref(thiz->height);
	egueb_dom_node_unref(thiz->xlink_href);
}

#if 0
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_use_initialize(Ender_Element *e)
{
	Egueb_Svg_Element_Use *thiz;
	Egueb_Dom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _egueb_svg_element_use_get(t);
	/* whenever the topmost is set on the use
	 * we should also set the topmost on the g
	 */
	ender_event_listener_add(e, "TopmostChanged", _egueb_svg_element_use_topmost_changed_cb, thiz);
	thiz->container = egueb_svg_renderable_container_new(e);
	egueb_svg_renderable_container_renderable_add(thiz->container, thiz->g_t);
}

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_use_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Use *thiz;
	Ender_Element *topmost;
	Ender_Element *link;
	Egueb_Dom_Tag *cloned_t;
	Enesim_Matrix translate;
	double tx, ty;

	thiz = _egueb_svg_element_use_get(t);
	/* we should append a new transformation */
	tx = egueb_svg_coord_final_get(&thiz->x, ctx->viewbox.w, ctx->font_size);
	ty = egueb_svg_coord_final_get(&thiz->y, ctx->viewbox.h, ctx->font_size);
	enesim_matrix_translate(&translate, tx, ty);
	enesim_matrix_compose(&ctx->transform, &translate, &ctx->transform);

	/* we take the shortcut here because there's no need to go through
	 * the normal enesim API
	 */
	/* FIXME this should be go away */
	egueb_svg_element_internal_topmost_get(t, &topmost);
	if (!topmost)
	{
		WARN("No topmost available");
		return EINA_TRUE;
	}

	if (thiz->state_changed)
	{
		if (thiz->cloned)
		{
			/* TODO remove the tree from the g_e */
			/* TODO remove previous clone */
			thiz->cloned = NULL;
		}
		if (thiz->past.link)
		{
			free(thiz->past.link);
			thiz->past.link = NULL;
		}
		if (thiz->current.link)
		{
			egueb_svg_element_svg_element_get(topmost, thiz->current.link, &link);
			thiz->cloned = egueb_svg_clone_new(link);

			if (!thiz->cloned)
			{
				WARN("Impossible to clone");
				return EINA_FALSE;
			}

			/* TODO add the clone to the generated g */
			cloned_t = ender_element_object_get(thiz->cloned);
			ender_element_property_value_add(thiz->g_e, EGUEB_DOM_CHILD, cloned_t, NULL);
			thiz->past.link = strdup(thiz->current.link);
		}
		/* FIXME this should go to the cleanup */
		thiz->state_changed = EINA_FALSE;
	}

	/* setup the g */
	/* FIXME for now */
	egueb_svg_element_topmost_set(thiz->g_t, topmost);
	/* Use our own context and attributes as the parent ones */
	return egueb_svg_element_setup_rel(thiz->g_t, c, ctx, attr, error);
}

static void _egueb_svg_element_use_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	egueb_svg_renderable_container_free(thiz->container);
	free(thiz);
}

static Egueb_Svg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _egueb_svg_element_use_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _egueb_svg_element_use_free,
	/* .initialize 		= */ _egueb_svg_element_use_initialize,
	/* .attribute_set 	= */ _egueb_svg_element_use_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ _egueb_svg_element_use_setup,
	/* .renderer_get	= */ _egueb_svg_element_use_renderer_get,
	/* .renderer_propagate	= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_svg_element_use_new(void)
{
	Egueb_Svg_Element_Use *thiz;
	Egueb_Dom_Tag *t;

	thiz = calloc(1, sizeof(Egueb_Svg_Element_Use));
	if (!thiz) return NULL;
	thiz->g_e = egueb_svg_element_g_new();
	thiz->g_r = egueb_svg_renderable_renderer_get(thiz->g_e);
	thiz->g_t = ender_element_object_get(thiz->g_e);

	/* Default values */
	thiz->x = ESVG_COORD_0;
	thiz->y = ESVG_COORD_0;
	thiz->width = ESVG_LENGTH_0;
	thiz->height = ESVG_LENGTH_0;

	t = egueb_svg_renderable_new(&_descriptor, ESVG_TYPE_USE, thiz);
	return t;
}

static void _egueb_svg_element_use_x_set(Egueb_Dom_Tag *t, const Egueb_Svg_Coord *x)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	if (x) thiz->x = *x;
}

static void _egueb_svg_element_use_x_get(Egueb_Dom_Tag *t, Egueb_Svg_Coord *x)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	if (x) *x = thiz->x;
}

static void _egueb_svg_element_use_y_set(Egueb_Dom_Tag *t, const Egueb_Svg_Coord *y)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	if (y) thiz->y = *y;
}

static void _egueb_svg_element_use_y_get(Egueb_Dom_Tag *t, Egueb_Svg_Coord *y)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	if (y) *y = thiz->y;
}

static void _egueb_svg_element_use_width_set(Egueb_Dom_Tag *t, const Egueb_Svg_Length *width)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	if (width) thiz->width = *width;
}

static void _egueb_svg_element_use_width_get(Egueb_Dom_Tag *t, Egueb_Svg_Length *width)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	if (width) *width = thiz->width;
}

static void _egueb_svg_element_use_height_set(Egueb_Dom_Tag *t, const Egueb_Svg_Length *height)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	if (height) thiz->height = *height;
}

static void _egueb_svg_element_use_height_get(Egueb_Dom_Tag *t, Egueb_Svg_Length *height)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	if (height) *height = thiz->height;
}

static void _egueb_svg_element_use_link_set(Egueb_Dom_Tag *t, const char *link)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = _egueb_svg_element_use_get(t);
	if (thiz->current.link)
	{
		free(thiz->current.link);
		thiz->current.link = NULL;
	}
	if (link)
	{
		thiz->current.link = strdup(link);
	}
	thiz->state_changed = EINA_TRUE;
}

static void _egueb_svg_element_use_link_get(Egueb_Dom_Tag *t, const char **link)
{
	Egueb_Svg_Element_Use *thiz;

	if (!link) return;
	thiz = _egueb_svg_element_use_get(t);
	*link = thiz->current.link;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
#endif
EAPI Egueb_Dom_Node * egueb_svg_element_use_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_use);
	return n;
}

#if 0
EAPI Eina_Bool egueb_svg_is_use(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Type type;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	type = egueb_svg_element_internal_type_get(t);
	return (type == ESVG_TYPE_USE) ? EINA_TRUE : EINA_FALSE;
}

EAPI void egueb_svg_element_use_x_set(Ender_Element *e, const Egueb_Svg_Coord *x)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_X, x, NULL);
}

EAPI void egueb_svg_element_use_x_get(Ender_Element *e, Egueb_Svg_Coord *x)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_use_x_get(t, x);
}

EAPI void egueb_svg_element_use_y_set(Ender_Element *e, const Egueb_Svg_Coord *y)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_Y, y, NULL);
}

EAPI void egueb_svg_element_use_y_get(Ender_Element *e, Egueb_Svg_Coord *y)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_use_y_get(t, y);
}

EAPI void egueb_svg_element_use_width_set(Ender_Element *e, const Egueb_Svg_Length *width)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_WIDTH, width, NULL);
}

EAPI void egueb_svg_element_use_width_get(Ender_Element *e, Egueb_Svg_Length *width)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_use_width_get(t, width);
}

EAPI void egueb_svg_element_use_height_set(Ender_Element *e, const Egueb_Svg_Length *height)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_HEIGHT, height, NULL);
}

EAPI void egueb_svg_element_use_height_get(Ender_Element *e, Egueb_Svg_Length *height)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_use_height_get(t, height);
}

EAPI void egueb_svg_element_use_link_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_LINK, v, NULL);
}

EAPI void egueb_svg_element_use_link_get(Ender_Element *e, const char **link)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_use_link_get(t, link);
}
#endif
