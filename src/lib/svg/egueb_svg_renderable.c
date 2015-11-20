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
#include "egueb_svg_element.h"

#include "egueb_svg_renderable_private.h"
#include "egueb_svg_event_request_painter_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_svg_attr_matrix_private.h"
#include "egueb_svg_document_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_svg_renderable_check_new_bounds(Egueb_Svg_Renderable *thiz)
{
	Egueb_Svg_Pointer_Events pevents;
	Egueb_Svg_Element *e;
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *topmost;

	e = EGUEB_SVG_ELEMENT(thiz);
	/* do not inform about a geometry change if we can not handle the input */
	egueb_dom_attr_final_get(e->pointer_events, &pevents);
	if (pevents == EGUEB_SVG_POINTER_EVENTS_NONE)
		return;

	/* get the previous/current bounds, if it is now inside the mouse, make sure
	 * to inform about it
	 */
	doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(thiz));
	if (doc)
	{
		topmost = egueb_dom_document_document_element_get(doc);
		if (topmost)
		{
			Eina_Rectangle bounds;
			Egueb_Dom_Input *input;

			/* only notify the input in case the bounds are different */
			enesim_renderer_destination_bounds_get(thiz->proxy, &bounds, 0, 0, NULL);
			if (thiz->last_processed_bounds.x != bounds.x ||
				thiz->last_processed_bounds.y != bounds.y ||
				thiz->last_processed_bounds.w != bounds.w ||
				thiz->last_processed_bounds.h != bounds.h)
			{
				Eina_Rectangle ibounds;
				Egueb_Dom_Feature *ifeature;
				int mx, my;

				ifeature = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_UI_NAME, NULL);
				input = egueb_dom_feature_ui_input_get(ifeature);

				egueb_dom_input_mouse_position_get(input, &mx, &my);
				eina_rectangle_coords_from(&ibounds, 0, 0, mx, my);
				if (eina_rectangles_intersect(&ibounds, &bounds))
				{
					egueb_svg_document_mouse_check(doc);
				}
				egueb_dom_input_unref(input);
				egueb_dom_feature_unref(ifeature);
				thiz->last_processed_bounds = bounds;
			}
			egueb_dom_node_unref(topmost);
		}
		egueb_dom_node_unref(doc);
	}
}
/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
/* Whenever a node has been inserted into the document, request the painter */
static void _egueb_svg_renderable_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Event_Phase phase;
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Event *request;

	phase = egueb_dom_event_phase_get(e);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	INFO_ELEMENT(n, "Renderable inserted into document, requesting a painter");
	request = egueb_svg_event_request_painter_new();
	egueb_dom_event_target_event_dispatch(EGUEB_DOM_EVENT_TARGET_CAST(n),
			egueb_dom_event_ref(request), NULL, NULL);

	egueb_svg_renderable_painter_set(n,
			egueb_svg_event_request_painter_painter_get(request));
	egueb_dom_event_unref(request);
}

/* Whenever a node has been removed from the document, remove the painter
 */
static void _egueb_svg_renderable_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Renderable *thiz;
	Egueb_Dom_Event_Phase phase;
	Egueb_Dom_Node *n = data;

	phase = egueb_dom_event_phase_get(e);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;
	INFO_ELEMENT(n, "Renderable removed from document");
	thiz = EGUEB_SVG_RENDERABLE(n);
	if (thiz->painter)
	{
		egueb_svg_painter_unref(thiz->painter);
		thiz->painter = NULL;
	}
}
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_renderable_process(Egueb_Svg_Element *e)
{
	Egueb_Svg_Renderable *thiz;
	Egueb_Svg_Renderable_Class *klass;
	Egueb_Svg_Clip_Path clip_path = EGUEB_SVG_CLIP_PATH_INIT;
	Egueb_Svg_Matrix m;
	Egueb_Svg_Painter *painter;
	Egueb_Dom_Node *relative;
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *topmost;
	Enesim_Renderer *ren = NULL;

	thiz = EGUEB_SVG_RENDERABLE(e);
	klass = EGUEB_SVG_RENDERABLE_CLASS_GET(e);

	/* set the new transformation */
	relative = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(e));
	if (relative)
	{
		Egueb_Svg_Element *e_parent;

		e_parent = EGUEB_SVG_ELEMENT(relative);
		egueb_dom_attr_final_get(thiz->transform, &m);
		enesim_matrix_compose(&e_parent->transform, &m, &e->transform);
		egueb_dom_node_unref(relative);
	}
	/* TODO in case there is no relative, set our own transform */

	DBG_ELEMENT(EGUEB_DOM_NODE(e), "New transformation %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS(&e->transform));

	/* first process the renderable itself */
	if (klass->process)
	{
		if (!klass->process(thiz))
		{
			WARN_ELEMENT(EGUEB_DOM_NODE(e), "Process failed");
			return EINA_FALSE;
		}
	}

	/* propagate the presentation attributes */
	/* resolve the painter based on the presentation attributes */
	painter = egueb_svg_painter_ref(thiz->painter);
	if (!painter)
	{
		doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(e));
		if (!doc)
		{
			WARN_ELEMENT(EGUEB_DOM_NODE(e), "No document available");
			return EINA_FALSE;
		}
		topmost = egueb_dom_document_document_element_get(doc);
		egueb_dom_node_unref(doc);

		/* The only special case is the topmost svg element */
		if (topmost == EGUEB_DOM_NODE(e))
		{
			painter = egueb_svg_renderable_class_painter_get(EGUEB_DOM_NODE(e));
			if (!painter)
			{
				WARN_ELEMENT(EGUEB_DOM_NODE(e), "Topmost element does not have a painter");
				egueb_dom_node_unref(topmost);
			}
			egueb_dom_node_unref(topmost);
		}
		else
		{
			egueb_dom_node_unref(topmost);
			WARN_ELEMENT(EGUEB_DOM_NODE(e), "No painter available");
		}
	}

	if (painter)
	{
		if (!egueb_svg_painter_resolve(painter, e))
		{
			WARN_ELEMENT(EGUEB_DOM_NODE(e), "Painter resolving failed");
			egueb_svg_painter_unref(painter);
			return EINA_FALSE;
		}
		/* finally call the renderer propagate implementation */
		if (klass->painter_apply)
			klass->painter_apply(thiz, painter);
		egueb_svg_painter_unref(painter);
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
		DBG_ELEMENT(EGUEB_DOM_NODE(e), "Clip path: %s", enesim_renderer_name_get(ren));
	}
	else
	{
		if (klass->renderer_get)
			ren = klass->renderer_get(thiz);
		if (!ren)
		{
			WARN_ELEMENT(EGUEB_DOM_NODE(e), "No renderer found");
			return EINA_FALSE;
		}
		else
		{
			DBG_ELEMENT(EGUEB_DOM_NODE(e), "Renderer: %s", enesim_renderer_name_get(ren));
		}
	}
	enesim_renderer_proxy_proxied_set(thiz->proxy, ren);
	_egueb_svg_renderable_check_new_bounds(thiz);
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_ELEMENT_DESCRIPTOR,
		Egueb_Svg_Renderable, Egueb_Svg_Renderable_Class,
		egueb_svg_renderable);

static void _egueb_svg_renderable_class_init(void *k)
{
	Egueb_Svg_Element_Class *klass;

	klass = EGUEB_SVG_ELEMENT_CLASS(k);
	klass->process = _egueb_svg_renderable_process;
}

static void _egueb_svg_renderable_instance_init(void *o)
{
	Egueb_Svg_Renderable *thiz;
	Egueb_Dom_Node *n;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_RENDERABLE(o);
	/* the properties */
	thiz->transform = egueb_svg_attr_matrix_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_TRANSFORM),
			&EGUEB_SVG_MATRIX_IDENTITY, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->transform), NULL);
	/* request for a painter whenever a renderable has been inserted into a document */
	egueb_dom_event_target_event_listener_add(
			EGUEB_DOM_EVENT_TARGET_CAST(n),
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_renderable_inserted_cb,
			EINA_FALSE, n);
	egueb_dom_event_target_event_listener_add(
			EGUEB_DOM_EVENT_TARGET_CAST(n),
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_renderable_removed_cb,
			EINA_FALSE, n);
	/* create the main proxy renderer */
	r = enesim_renderer_proxy_new();
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
	/* the painter */
	if (thiz->painter)
	{
		egueb_svg_painter_unref(thiz->painter);
		thiz->painter = NULL;
	}
	/* the rendering */
	enesim_renderer_unref(thiz->proxy);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Painter * egueb_svg_renderable_class_painter_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Renderable_Class *klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS_GET(n);
	if (klass->painter_get)
	{
		return klass->painter_get(EGUEB_SVG_RENDERABLE(n));
	}
	else
	{
		return NULL;
	}
}

Egueb_Svg_Painter * egueb_svg_renderable_painter_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Renderable *thiz;

	thiz = EGUEB_SVG_RENDERABLE(n);
	return egueb_svg_painter_ref(thiz->painter);
}

void egueb_svg_renderable_painter_set(Egueb_Dom_Node *n, Egueb_Svg_Painter *p)
{
	Egueb_Svg_Renderable *thiz;

	thiz = EGUEB_SVG_RENDERABLE(n);
	if (thiz->painter)
		egueb_svg_painter_unref(thiz->painter);
	thiz->painter = p;
}

Enesim_Renderer * egueb_svg_renderable_renderer_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Renderable *thiz;

	thiz = EGUEB_SVG_RENDERABLE(n);
	return enesim_renderer_ref(thiz->proxy);
}

Enesim_Renderer * egueb_svg_renderable_class_renderer_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Renderable_Class *klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS_GET(n);
	if (klass->renderer_get) return klass->renderer_get(EGUEB_SVG_RENDERABLE(n));
	else return NULL;
}

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

EAPI void egueb_svg_renderable_bounds_get(Egueb_Dom_Node *n,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Renderable_Class *klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS_GET(n);
	if (klass->bounds_get)
		klass->bounds_get(EGUEB_SVG_RENDERABLE(n), bounds);
}

EAPI void egueb_svg_renderable_user_bounds_get(Egueb_Dom_Node *n,
		Eina_Rectangle *bounds)
{
	Enesim_Renderer *r;

	r = egueb_svg_renderable_renderer_get(n);
	if (!r) return;

	enesim_renderer_destination_bounds_get(r, bounds, 0, 0, NULL);
}

EAPI void egueb_svg_renderable_transform_get(Egueb_Dom_Node *n,
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
}

EAPI void egueb_svg_renderable_transform_set(Egueb_Dom_Node *n,
		Enesim_Matrix *m)
{
	Egueb_Svg_Renderable *thiz;

	thiz = EGUEB_SVG_RENDERABLE(n);
	egueb_dom_attr_set(thiz->transform, EGUEB_DOM_ATTR_TYPE_BASE, m);
}
