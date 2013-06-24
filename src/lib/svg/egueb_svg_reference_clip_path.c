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
#include "egueb_svg_clip_path.h"
#include "egueb_svg_referenceable.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_renderable.h"
#include "egueb_svg_element.h"
#include "egueb_svg_element_clip_path.h"
#include "egueb_svg_reference_private.h"
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

/*----------------------------------------------------------------------------*
 *                             Reference interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_reference_clip_path_process(
		Egueb_Svg_Reference *r)
{
	Egueb_Svg_Reference_Clip_Path *thiz;
	Egueb_Svg_Referenceable_Units_Animated units;
	Egueb_Svg_Clip_Path clip_path = EGUEB_SVG_CLIP_PATH_INIT;
	Enesim_Renderer *ren;
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
}

static void _egueb_svg_reference_clip_path_class_deinit(void *k)
{
}

static void _egueb_svg_reference_clip_path_instance_init(void *o)
{
	Egueb_Svg_Reference_Clip_Path *thiz;

	thiz = EGUEB_SVG_REFERENCE_CLIP_PATH(o);
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

/* TODO remove this function */
void egueb_svg_reference_clip_path_g_set(Egueb_Svg_Reference *r, Egueb_Dom_Node *g)
{
	Egueb_Svg_Reference_Clip_Path *thiz;

	thiz = EGUEB_SVG_REFERENCE_CLIP_PATH(r);
	if (thiz->g)
	{
		egueb_dom_node_unref(thiz->g);
		thiz->g = NULL;
	}
	thiz->g = g;
}

void egueb_svg_reference_clip_path_g_get(Egueb_Svg_Reference *r, Egueb_Dom_Node **g)
{
	Egueb_Svg_Reference_Clip_Path *thiz;

	thiz = EGUEB_SVG_REFERENCE_CLIP_PATH(r);
	if (thiz->g)
		*g = egueb_dom_node_ref(thiz->g);
	else
		*g = NULL;
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
		Egueb_Dom_Node *referencer = NULL;
		/* otherwise, use the referencer (the shape that refered this
		 * reference)
		 */
		*ren = egueb_svg_renderable_class_renderer_get(r->referencer);
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
