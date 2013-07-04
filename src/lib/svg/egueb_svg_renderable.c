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
#include "egueb_svg_renderable.h"
#include "egueb_svg_attr_matrix.h"
#include "egueb_svg_renderable_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_renderable_process(Egueb_Svg_Element *e)
{
	Egueb_Svg_Renderable *thiz;
	Egueb_Svg_Renderable_Class *klass;
	Egueb_Svg_Clip_Path clip_path = EGUEB_SVG_CLIP_PATH_INIT;
	Egueb_Dom_Node *relative;
	Egueb_Dom_Node *doc = NULL;
	Enesim_Renderer *ren = NULL;
	Egueb_Svg_Matrix m;

	thiz = EGUEB_SVG_RENDERABLE(e);
	klass = EGUEB_SVG_RENDERABLE_CLASS_GET(e);

	/* set the new transformation */
	egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(e), &relative);
	if (relative)
	{
		Egueb_Svg_Element *e_parent;

		e_parent = EGUEB_SVG_ELEMENT(relative);
		egueb_dom_attr_final_get(thiz->transform, &m);
		enesim_matrix_compose(&e_parent->transform, &m, &e->transform);
		egueb_dom_node_unref(relative);
	}
	/* TODO in case there is no relative, set our own transform */

	DBG("New transformation %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS(&e->transform));

	/* first process the renderable itself */
	if (klass->process)
	{
		if (!klass->process(thiz))
		{
			WARN("Process failed");
			return EINA_FALSE;
		}
	}
	/* now resolve the clip path */
	egueb_svg_element_clip_path_final_get(EGUEB_DOM_NODE(e), &clip_path);
	egueb_svg_element_clip_path_resolve(EGUEB_DOM_NODE(e), &clip_path,
			&thiz->clip_path_last, &thiz->clip_path);
	egueb_svg_clip_path_reset(&clip_path);

	if (thiz->clip_path)
		egueb_svg_reference_clip_path_renderer_get(thiz->clip_path, &ren);

	/* set the correct renderer on the proxy */
	if (ren)
	{
		DBG("Clip path: %p", ren);
	}
	else
	{
		if (klass->renderer_get)
			ren = klass->renderer_get(thiz);
		DBG("Renderer: %p", ren);
		if (!ren)
		{
			WARN("No renderer found");
			return EINA_FALSE;
		}
	}
	enesim_renderer_proxy_proxied_set(thiz->proxy, ren);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_renderable, Egueb_Svg_Renderable, transform);

ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_ELEMENT_DESCRIPTOR,
		Egueb_Svg_Renderable, Egueb_Svg_Renderable_Class,
		egueb_svg_renderable);

static void _egueb_svg_renderable_class_init(void *k)
{
	Egueb_Svg_Element_Class *klass;

	klass = EGUEB_SVG_ELEMENT_CLASS(k);
	klass->process = _egueb_svg_renderable_process;
}

static void _egueb_svg_renderable_class_deinit(void *k)
{
}

static void _egueb_svg_renderable_instance_init(void *o)
{
	Egueb_Svg_Renderable *thiz;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_RENDERABLE(o);
	/* the properties */
	thiz->transform = egueb_svg_attr_matrix_new(
			egueb_dom_string_ref(EGUEB_SVG_TRANSFORM),
			&EGUEB_SVG_MATRIX_IDENTITY, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_renderable, transform);
	/* create the main proxy renderer */
	r = enesim_renderer_proxy_new();
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->proxy = r;
}

static void _egueb_svg_renderable_instance_deinit(void *o)
{
	Egueb_Svg_Renderable *thiz;

	thiz = EGUEB_SVG_RENDERABLE(o);
	/* the properties */
	egueb_dom_node_unref(thiz->transform);
	/* now remove the references */
	if (thiz->clip_path)
	{
		egueb_svg_reference_free(thiz->clip_path);
		thiz->clip_path = NULL;
	}
	/* the rendering */
	enesim_renderer_unref(thiz->proxy);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_is_renderable(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_RENDERABLE_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Enesim_Renderer * egueb_svg_renderable_renderer_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Renderable *thiz;

	thiz = EGUEB_SVG_RENDERABLE(n);
	return enesim_renderer_ref(thiz->proxy);
}

EAPI Enesim_Renderer * egueb_svg_renderable_class_renderer_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Renderable_Class *klass;
	Enesim_Renderer *r;

	klass = EGUEB_SVG_RENDERABLE_CLASS_GET(n);
	if (klass->renderer_get) return klass->renderer_get(EGUEB_SVG_RENDERABLE(n));
	else return NULL;
}

EAPI void egueb_svg_renderable_bounds_get(Egueb_Dom_Node *n,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Renderable_Class *klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS_GET(n);
	if (klass->bounds_get)
		klass->bounds_get(EGUEB_SVG_RENDERABLE(n), bounds);
	else
	{
		Enesim_Renderer *r;

		r = egueb_svg_renderable_renderer_get(n);
		if (!r) return;
		enesim_renderer_bounds(r, bounds);
	}
}

EAPI void egueb_svg_renderable_user_bounds_get(Egueb_Dom_Node *n,
		Eina_Rectangle *bounds)
{
	Enesim_Renderer *r;

	r = egueb_svg_renderable_renderer_get(n);
	if (!r) return;

	enesim_renderer_destination_bounds(r, bounds, 0, 0);
}

EAPI Eina_Error egueb_svg_renderable_transform_get(Egueb_Dom_Node *n,
		Egueb_Svg_Matrix_Animated *m)
{
	Egueb_Svg_Renderable *thiz;

	thiz = EGUEB_SVG_RENDERABLE(n);
	/* FIXME implement this as
	 * egueb_svg_element_attr_get(attr, void *base, void *anim)
	 */
	if (egueb_dom_attr_type_is_set(thiz->transform, EGUEB_DOM_ATTR_TYPE_BASE))
		egueb_dom_attr_get(thiz->transform, EGUEB_DOM_ATTR_TYPE_BASE, &m->base);
	else
		egueb_dom_attr_get(thiz->transform, EGUEB_DOM_ATTR_TYPE_DEFAULT, &m->base);
	if (egueb_dom_attr_type_is_set(thiz->transform, EGUEB_DOM_ATTR_TYPE_ANIMATED))
		egueb_dom_attr_get(thiz->transform, EGUEB_DOM_ATTR_TYPE_ANIMATED, &m->anim);
	else
		m->anim = m->base;
	return EINA_ERROR_NONE;
}

EAPI Eina_Error egueb_svg_renderable_transform_set(Egueb_Dom_Node *n,
		Enesim_Matrix *m)
{
	Egueb_Svg_Renderable *thiz;

	thiz = EGUEB_SVG_RENDERABLE(n);
	return egueb_dom_attr_set(thiz->transform, EGUEB_DOM_ATTR_TYPE_BASE, m);
}
