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
#include "Esvg.h"
#include "esvg_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_POLYGON_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_POLYGON_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_POLYGON_MAGIC);\
	} while(0)

typedef struct _Esvg_Polygon
{
	EINA_MAGIC
	/* properties */
	Eina_List *points;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Polygon;

static Esvg_Polygon * _esvg_polygon_get(Enesim_Renderer *r)
{
	Esvg_Polygon *thiz;

	thiz = esvg_shape_data_get(r);
	ESVG_POLYGON_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         The ESVG element interface                         *
 *----------------------------------------------------------------------------*/
static const char * _esvg_polygon_name_get(Enesim_Renderer *r)
{
	return "polygon";
}

static Enesim_Renderer * _esvg_polygon_renderer_get(Enesim_Renderer *r)
{
	Esvg_Polygon *thiz;

	thiz = _esvg_polygon_get(r);
	return thiz->r;
}

static Eina_Bool _esvg_polygon_setup(Enesim_Renderer *r, const Esvg_Element_State *estate,
		const Esvg_Shape_Enesim_State *dstate)
{
	Esvg_Polygon *thiz;

	thiz = _esvg_polygon_get(r);

	printf("polygon setup\n");
	/* the shape must always be closed */
	if (thiz->changed)
		enesim_renderer_figure_polygon_close(thiz->r, EINA_TRUE);

	/* shape properties */
	enesim_renderer_shape_fill_color_set(thiz->r, dstate->fill_color);
	enesim_renderer_shape_fill_rule_set(thiz->r, dstate->fill_rule);
	enesim_renderer_shape_fill_renderer_set(thiz->r, dstate->fill_renderer);
	enesim_renderer_shape_stroke_color_set(thiz->r, dstate->stroke_color);
	enesim_renderer_shape_stroke_weight_set(thiz->r, dstate->stroke_weight);
	enesim_renderer_shape_draw_mode_set(thiz->r, dstate->draw_mode);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_stroke_cap_set(thiz->r, dstate->stroke_cap);
	enesim_renderer_shape_stroke_join_set(thiz->r, dstate->stroke_join);
	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &estate->transform);
	enesim_renderer_color_set(thiz->r, dstate->color);

	return EINA_TRUE;
}

static void _esvg_polygon_cleanup(Enesim_Renderer *r)
{

}

static void _esvg_polygon_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{
	Esvg_Polygon *thiz;
	Eina_List *l;
	Esvg_Point *point;

	thiz = _esvg_polygon_get(r);
	/* FIXME we could remove our own list of polygons and use a _get from the enesim_renderer_figure */
	EINA_LIST_FOREACH(thiz->points, l, point)
	{
		esvg_polygon_point_add(dr, point);
	}
}


static Esvg_Shape_Descriptor _descriptor = {
	/* .setup =		*/ _esvg_polygon_setup,
	/* .renderer_get =	*/ _esvg_polygon_renderer_get,
	/* .name_get =		*/ _esvg_polygon_name_get,
	/* .clone =		*/ _esvg_polygon_clone,
	/* .cleanup =		*/ _esvg_polygon_cleanup,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_polygon_new(void)
{
	Esvg_Polygon *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Polygon));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_POLYGON_MAGIC);

	r = enesim_renderer_figure_new();
	/* FIXME for now */
	enesim_renderer_figure_polygon_add(r);
	enesim_renderer_rop_set(r, ENESIM_BLEND);

	thiz->r = r;
	/* default values */

	r = esvg_shape_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool esvg_is_polygon(Enesim_Renderer *r)
{
	Esvg_Polygon *thiz;
	Eina_Bool ret;

	if (!esvg_is_shape(r))
		return EINA_FALSE;
	thiz = esvg_shape_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_POLYGON_MAGIC);

	return ret;
}

EAPI void esvg_polygon_point_add(Enesim_Renderer *r, Esvg_Point *p)
{
	Esvg_Polygon *thiz;
	Esvg_Point *new_point;

	if (!p) return;

	thiz = _esvg_polygon_get(r);
	new_point = calloc(1, sizeof(Esvg_Point));
	*new_point = *p;
	thiz->points = eina_list_append(thiz->points, new_point);
	enesim_renderer_figure_polygon_vertex_add(thiz->r, new_point->x, new_point->y);
	thiz->changed = EINA_TRUE;
}
