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
#define ESVG_POLYLINE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_POLYLINE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_POLYLINE_MAGIC);\
	} while(0)

typedef struct _Esvg_Polyline
{
	EINA_MAGIC
	/* properties */
	Eina_List *points;
	/* private */
	Enesim_Renderer *line;
	Enesim_Renderer *figure;
} Esvg_Polyline;

static Esvg_Polyline * _esvg_polyline_get(Enesim_Renderer *r)
{
	Esvg_Polyline *thiz;

	thiz = esvg_shape_data_get(r);
	ESVG_POLYLINE_MAGIC_CHECK(thiz);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The ESVG element interface                         *
 *----------------------------------------------------------------------------*/
static const char * _esvg_polyline_name_get(Enesim_Renderer *r)
{
	return "polyline";
}

static Enesim_Renderer * _esvg_polyline_renderer_get(Enesim_Renderer *r)
{
	Esvg_Polyline *thiz;
	int nvert;

	thiz = _esvg_polyline_get(r);
	nvert = eina_list_count(thiz->points);

	if (nvert > 2)
		return thiz->figure;
	else
		return thiz->line;
}

static Eina_Bool _esvg_polyline_setup(Enesim_Renderer *r, const Esvg_Element_Context *estate,
		const Esvg_Shape_Enesim_State *dstate)
{
	Esvg_Polyline *thiz;
	int nvert;

	thiz = _esvg_polyline_get(r);

	/* FIXME gets the parents size or the topmost? */
	/* FIXME we should keep the old fill renderer */

	nvert = eina_list_count(thiz->points);
	if (nvert > 2)
		r = thiz->figure;
	else
		r = thiz->line;

	/* shape properties */
	enesim_renderer_shape_fill_color_set(r, dstate->fill_color);
	enesim_renderer_shape_fill_rule_set(r, dstate->fill_rule);
	enesim_renderer_shape_fill_renderer_set(r, dstate->fill_renderer);
	enesim_renderer_shape_stroke_color_set(r, dstate->stroke_color);
	enesim_renderer_shape_stroke_weight_set(r, dstate->stroke_weight);
	enesim_renderer_shape_draw_mode_set(r, dstate->draw_mode);
	enesim_renderer_shape_stroke_location_set(r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_stroke_cap_set(r, dstate->stroke_cap);
	enesim_renderer_shape_stroke_join_set(r, dstate->stroke_join);

	/* base properties */
	enesim_renderer_geometry_transformation_set(r, &estate->transform);
	enesim_renderer_color_set(r, dstate->color);
	printf("calling the setup on the polyline ");
	/* setup the points */
	if (nvert > 2)
	{
		Esvg_Point *p;
		Eina_List *l;

		enesim_renderer_figure_clear(r);
		enesim_renderer_figure_polygon_add(r);
		EINA_LIST_FOREACH(thiz->points, l, p)
		{
			printf("(%f, %f) ", p->x, p->y);
			enesim_renderer_figure_polygon_vertex_add(r, p->x, p->y);
		}
	}
	else
	{
		Esvg_Point *p;
		Esvg_Point pts[2] = { { 0, 0 }, { 0, 0 } };
		Esvg_Point *pt = pts;
		Eina_List *l;

		EINA_LIST_FOREACH(thiz->points, l, p)
		{
			*pt = *p;
			pt++;
		}
		enesim_renderer_line_x0_set(r, pts[0].x);
		enesim_renderer_line_y0_set(r, pts[0].y);
		enesim_renderer_line_x1_set(r, pts[1].x);
		enesim_renderer_line_y1_set(r, pts[1].y);
		printf("using the line (%f, %f) (%f, %f)", pts[0].x, pts[0].y, pts[1].x, pts[1].y);
	}
	printf("\n");

	return EINA_TRUE;
}

static void _esvg_polyline_cleanup(Enesim_Renderer *r)
{

}

static void _esvg_polyline_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{

}

static Esvg_Shape_Descriptor _descriptor = {
	/* .setup =		*/ _esvg_polyline_setup,
	/* .renderer_get =	*/ _esvg_polyline_renderer_get,
	/* .name_get =		*/ _esvg_polyline_name_get,
	/* .clone =		*/ _esvg_polyline_clone,
	/* .cleanup =		*/ _esvg_polyline_cleanup,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_polyline_new(void)
{
	Esvg_Polyline *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Polyline));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_POLYLINE_MAGIC);

	r = enesim_renderer_figure_new();
	/* FIXME for now */
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->figure = r;

	r = enesim_renderer_line_new();
	/* FIXME for now */
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->line = r;


	/* default values */

	r = esvg_shape_new(&_descriptor, thiz);
	return r;
}

EAPI void esvg_polyline_point_add(Enesim_Renderer *r, Esvg_Point *p)
{
	Esvg_Polyline *thiz;
	Esvg_Point *new_point;

	if (!p) return;

	thiz = _esvg_polyline_get(r);
	new_point = calloc(1, sizeof(Esvg_Point));
	*new_point = *p;
	thiz->points = eina_list_append(thiz->points, new_point);
}
