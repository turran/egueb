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
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_private_instantiable.h"
#include "esvg_polygon.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_POLYGON_POINT;

typedef struct _Esvg_Polygon
{
	/* properties */
	Eina_List *points;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Polygon;

static Esvg_Polygon * _esvg_polygon_get(Edom_Tag *t)
{
	Esvg_Polygon *thiz;

	if (esvg_element_type_get_internal(t) != ESVG_POLYGON)
		return NULL;
	thiz = esvg_instantiable_data_get(t);

	return thiz;
}

static void _esvg_polygon_points_cb(Esvg_Point *p, void *data)
{
	Ender_Element *e = data;

	esvg_polygon_point_add(e, p);
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_polygon_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "points") == 0)
	{
		esvg_parser_points(value, _esvg_polygon_points_cb, e);
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_polygon_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_polygon_renderer_get(Edom_Tag *t)
{
	Esvg_Polygon *thiz;

	thiz = _esvg_polygon_get(t);
	return thiz->r;
}

static Eina_Bool _esvg_polygon_setup(Edom_Tag *t,
		Esvg_Element_Context *ctx,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Polygon *thiz;

	thiz = _esvg_polygon_get(t);

	printf("polygon setup\n");
	/* the shape must always be closed */
	if (thiz->changed)
		enesim_renderer_figure_polygon_close(thiz->r, EINA_TRUE);

	/* shape properties */
	if (!rctx->fill_renderer)
		enesim_renderer_shape_fill_color_set(thiz->r, rctx->fill_color);
	else
		enesim_renderer_shape_fill_renderer_set(thiz->r, rctx->fill_renderer);
	if (!rctx->stroke_renderer)
		enesim_renderer_shape_stroke_color_set(thiz->r, rctx->stroke_color);
	else
		enesim_renderer_shape_stroke_renderer_set(thiz->r, rctx->stroke_renderer);
	enesim_renderer_shape_fill_rule_set(thiz->r, rctx->fill_rule);
	enesim_renderer_shape_stroke_weight_set(thiz->r, rctx->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_stroke_cap_set(thiz->r, rctx->stroke_cap);
	enesim_renderer_shape_stroke_join_set(thiz->r, rctx->stroke_join);
	enesim_renderer_shape_draw_mode_set(thiz->r, rctx->draw_mode);
	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &ctx->transform);
	enesim_renderer_color_set(thiz->r, rctx->color);

	return EINA_TRUE;
}

static void _esvg_polygon_clone(Edom_Tag *t, Edom_Tag *dt)
{
#if 0
	Esvg_Polygon *thiz;
	Eina_List *l;
	Esvg_Point *point;

	thiz = _esvg_polygon_get(t);
	/* FIXME we could remove our own list of polygons and use a _get from the enesim_renderer_figure */
	EINA_LIST_FOREACH(thiz->points, l, point)
	{
		esvg_polygon_point_add(dr, point);
	}
#endif
}

static void _esvg_polygon_free(Edom_Tag *t)
{
	Esvg_Polygon *thiz;

	thiz = _esvg_polygon_get(t);
	free(thiz);
}

static Esvg_Instantiable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_polygon_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_polygon_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_polygon_attribute_set,
	/* .clone		= */ _esvg_polygon_clone,
	/* .setup		= */ _esvg_polygon_setup,
	/* .renderer_get	= */ _esvg_polygon_renderer_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_polygon_new(void)
{
	Esvg_Polygon *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Polygon));
	if (!thiz) return NULL;

	r = enesim_renderer_figure_new();
	/* FIXME for now */
	enesim_renderer_figure_polygon_add(r);
	enesim_renderer_rop_set(r, ENESIM_BLEND);

	thiz->r = r;
	/* default values */

	t = esvg_instantiable_new(&_descriptor, ESVG_POLYGON, thiz);
	return t;
}

static void _esvg_polygon_point_add(Edom_Tag *t, Esvg_Point *p)
{
	Esvg_Polygon *thiz;
	Esvg_Point *new_point;

	if (!p) return;

	thiz = _esvg_polygon_get(t);
	new_point = calloc(1, sizeof(Esvg_Point));
	*new_point = *p;
	thiz->points = eina_list_append(thiz->points, new_point);
	enesim_renderer_figure_polygon_vertex_add(thiz->r, new_point->x, new_point->y);
	thiz->changed = EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_polygon_point_set NULL
#define _esvg_polygon_point_get NULL
#define _esvg_polygon_point_clear NULL
#define _esvg_polygon_point_remove NULL
#include "generated/esvg_generated_polygon.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_polygon_new(void)
{
	return ender_element_new_with_namespace("polygon", "esvg");
}

EAPI Eina_Bool esvg_is_polygon(Ender_Element *e)
{
}

EAPI void esvg_polygon_point_add(Ender_Element *e, Esvg_Point *p)
{
	ender_element_property_value_add(e, ESVG_POLYGON_POINT, p, NULL);
}
