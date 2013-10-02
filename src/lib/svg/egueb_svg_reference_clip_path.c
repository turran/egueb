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
#include "egueb_svg_painter.h"
#include "egueb_svg_reference.h"
#include "egueb_svg_clip_path.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_renderable.h"
#include "egueb_svg_renderable_container.h"
#include "egueb_svg_element.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_shape.h"

#include "egueb_svg_element_private.h"
#include "egueb_svg_element_clip_path_private.h"
#include "egueb_svg_reference_private.h"
#include "egueb_svg_painter_private.h"
#include "egueb_svg_shape_private.h"

/* TODO we still need to implement a way to inform whenever some node
 * has changed inside the <g> we need to enqueue it for later process
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_REFERENCE_CLIP_PATH_CLASS_GET(o) 				\
		EGUEB_SVG_REFERENCE_CLIP_PATH_CLASS(				\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_REFERENCE_CLIP_PATH_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,	\
		Egueb_Svg_Reference_Clip_Path_Class,				\
		EGUEB_SVG_REFERENCE_CLIP_PATH_DESCRIPTOR)
#define EGUEB_SVG_REFERENCE_CLIP_PATH(o) ENESIM_OBJECT_INSTANCE_CHECK(o,	\
		Egueb_Svg_Reference_Clip_Path,					\
		EGUEB_SVG_REFERENCE_CLIP_PATH_DESCRIPTOR)

typedef struct _Egueb_Svg_Reference_Clip_Path
{
	Egueb_Svg_Reference base;
	/* our own reference for the referenceable clip-path */
	Egueb_Svg_Reference *clip_path;
	Egueb_Svg_Clip_Path clip_path_last;
	Egueb_Dom_Node *g;
} Egueb_Svg_Reference_Clip_Path;

typedef struct _Egueb_Svg_Reference_Clip_Path_Class
{
	Egueb_Svg_Reference_Class base;
} Egueb_Svg_Reference_Clip_Path_Class;

/* function called whenever we want to clone the clip path children */
static Eina_Bool _egueb_svg_reference_clip_path_children_clone_cb(
		Egueb_Dom_Node *child, void *data)
{
	Egueb_Svg_Reference_Clip_Path *thiz = data;
	Egueb_Dom_Node *clone = NULL;

	/* process every child that is not a clonable element */
	if (!egueb_svg_element_clip_path_node_is_clonable(child))
		return EINA_TRUE;
	/* for a children that is clonable, clone it and add it to the g */
	egueb_dom_node_clone(child, EINA_TRUE, EINA_TRUE, &clone, NULL);
	egueb_dom_node_child_append(thiz->g, child, NULL);

	return EINA_TRUE;
}

static void _egueb_svg_reference_clip_path_set_painter(Egueb_Dom_Node *n,
		Egueb_Svg_Reference_Clip_Path *thiz)
{
	if (egueb_svg_is_shape(n))
	{
		Egueb_Svg_Painter *painter;

		painter = egueb_svg_painter_clip_path_new(EGUEB_SVG_REFERENCE(thiz));
		DBG("Setting the clip path painter on the shape");
		egueb_svg_shape_painter_set(n, painter);
	}
	else if (egueb_svg_is_renderable_container(n))
	{
		Egueb_Dom_Node *child;

		/* iterate over the shapes to set the painter too */
		child = egueb_dom_node_child_first_get(n);
		while (child)
		{
			Egueb_Dom_Node *tmp;

			_egueb_svg_reference_clip_path_set_painter(child, thiz);
			tmp = egueb_dom_node_sibling_next_get(child);
			egueb_dom_node_unref(child);
			child = tmp;
		}
	}
}

/*----------------------------------------------------------------------------*
 *                               Event listeners                              *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_reference_clip_path_node_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Svg_Reference_Clip_Path *thiz = data;
	Egueb_Dom_Node *target = NULL;

	target = egueb_dom_event_target_get(e);
	_egueb_svg_reference_clip_path_set_painter(target, thiz);
	egueb_dom_node_unref(target);
}

/*----------------------------------------------------------------------------*
 *                               Event monitors                               *
 *----------------------------------------------------------------------------*/
static void _egueb_dom_reference_clip_path_monitor_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Svg_Reference_Clip_Path *thiz = data;
	Egueb_Svg_Reference *r = EGUEB_SVG_REFERENCE(thiz);
	
	/* whenever we receive an event, just propagate it */
	egueb_dom_node_event_propagate(r->referencer, ev);
}
/*----------------------------------------------------------------------------*
 *                             Reference interface                            *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_reference_clip_path_setup(
		Egueb_Svg_Reference *r)
{
	Egueb_Svg_Reference_Clip_Path *thiz;
	Egueb_Dom_Node *doc;

	thiz = EGUEB_SVG_REFERENCE_CLIP_PATH(r);
	/* TODO we need to check what kind of referencer is */
	doc = egueb_dom_node_document_get(r->referenceable);
	thiz->g = egueb_dom_document_node_adopt(doc, thiz->g, NULL);
	egueb_dom_node_unref(doc);

	/* make the group get the presentation attributes from ourelves and
	 * the geometry relative from the referrer
	 */
	egueb_svg_element_presentation_relative_set(thiz->g, r->referenceable, NULL);
	egueb_svg_element_geometry_relative_set(thiz->g, r->referencer, NULL);

	/* iterate over the list of children of the referenceable 
	 * and clone the clonable elements */
	egueb_dom_node_children_foreach(r->referenceable,
			_egueb_svg_reference_clip_path_children_clone_cb,
			r);
}

static Eina_Bool _egueb_svg_reference_clip_path_process(
		Egueb_Svg_Reference *r)
{
	Egueb_Svg_Reference_Clip_Path *thiz;
	Egueb_Svg_Referenceable_Units_Animated units;
	Egueb_Svg_Clip_Path clip_path = EGUEB_SVG_CLIP_PATH_INIT;
	Enesim_Matrix m;
	Eina_Bool ret;

	DBG("Processing a clip path reference");
	thiz = EGUEB_SVG_REFERENCE_CLIP_PATH(r);

	/* get the needed attributes from the referencerable */
	egueb_svg_element_clip_path_units_get(r->referenceable, &units);
	egueb_svg_element_clip_path_final_get(r->referenceable, &clip_path);
	egueb_svg_element_clip_path_resolve(r->referencer, &clip_path,
			&thiz->clip_path_last,
			&thiz->clip_path);
	egueb_svg_clip_path_reset(&clip_path);

	/* whenever a reference is going to be processed
	 * we need to set a new transformation matrix based on the
	 * clip path units
	 */
	if (units.anim == EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX)
	{
		Enesim_Rectangle bounds;

		egueb_svg_renderable_bounds_get(r->referencer, &bounds);
		enesim_matrix_values_set(&m, bounds.w, 0, bounds.x, 0, bounds.h, bounds.y, 0, 0, 1);
		DBG("Using the object bounding box %" ENESIM_RECTANGLE_FORMAT, ENESIM_RECTANGLE_ARGS(&bounds));
		DBG("New transformation %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS(&m));
		egueb_svg_renderable_transform_set(thiz->g, &m);
	}
	else
	{
		DBG("Using the user space on use");
		enesim_matrix_identity(&m);
		egueb_svg_renderable_transform_set(thiz->g, &m);
	}
	ret = egueb_dom_element_process(thiz->g);

	return ret;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_REFERENCE_DESCRIPTOR,
		Egueb_Svg_Reference_Clip_Path,
		Egueb_Svg_Reference_Clip_Path_Class,
		egueb_svg_reference_clip_path);

static void _egueb_svg_reference_clip_path_class_init(void *k)
{
	Egueb_Svg_Reference_Class *klass;

	klass = EGUEB_SVG_REFERENCE_CLASS(k);
	klass->process = _egueb_svg_reference_clip_path_process;
	klass->setup = _egueb_svg_reference_clip_path_setup;
}

static void _egueb_svg_reference_clip_path_instance_init(void *o)
{
	Egueb_Svg_Reference_Clip_Path *thiz;

	thiz = EGUEB_SVG_REFERENCE_CLIP_PATH(o);
	thiz->g = egueb_svg_element_g_new();
	egueb_dom_node_event_listener_add(thiz->g,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_reference_clip_path_node_inserted_cb,
			EINA_TRUE, thiz);
	/* add the events that we need to propagate upstream */ 
	egueb_dom_node_event_monitor_add(thiz->g,
			_egueb_dom_reference_clip_path_monitor_cb, thiz);
}

static void _egueb_svg_reference_clip_path_instance_deinit(void *o)
{
	Egueb_Svg_Reference_Clip_Path *thiz;

	thiz = EGUEB_SVG_REFERENCE_CLIP_PATH(o);
	if (thiz->clip_path)
	{
		egueb_svg_reference_free(thiz->clip_path);
		thiz->clip_path = NULL;
	}
	egueb_svg_clip_path_reset(&thiz->clip_path_last);
	/* TODO whenever the clip path reference is destroyed
	 * we should remove every painter on the g children
	 */
	if (thiz->g)
	{
		egueb_dom_node_unref(thiz->g);
		thiz->g = NULL;
	}
	/* TODO remove our own event handlers */
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Svg_Reference * egueb_svg_reference_clip_path_new(void)
{
	Egueb_Svg_Reference *r;
	r = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_reference_clip_path);
	return r;
}

/* This is the function used on the renderables to swap the renderable
 * internal renderer and use this instead
 */
void egueb_svg_reference_clip_path_renderer_get(Egueb_Svg_Reference *r,
		Enesim_Renderer **ren)
{
	Egueb_Svg_Reference_Clip_Path *thiz;

	if (!ren) return;

	thiz = EGUEB_SVG_REFERENCE_CLIP_PATH(r);
	if (thiz->g)
		*ren = egueb_svg_renderable_class_renderer_get(thiz->g);
	else
		*ren = NULL;
}

/* This function is needed on the painter clip path to decide to use
 * the other renderer or the refererencer renderer
 */
void egueb_svg_reference_clip_path_resolved_renderer_get(Egueb_Svg_Reference *r,
		Enesim_Renderer **ren)
{
	Egueb_Svg_Reference_Clip_Path *thiz;

	thiz = EGUEB_SVG_REFERENCE_CLIP_PATH(r);
	if (thiz->clip_path)
	{
		/* in case the reference has another reference, use the other
		 * <g> renderable as the fill renderer
		 */
		egueb_svg_reference_clip_path_renderer_get(thiz->clip_path, ren);
	}
	else
	{
		/* otherwise, use the referencer (the shape that refered this
		 * reference)
		 */
		*ren = egueb_svg_renderable_class_renderer_get(r->referencer);
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
