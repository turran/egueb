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
#include "egueb_svg_main_private.h"
#include "egueb_svg_private_attribute_presentation.h"
#include "egueb_svg_context_private.h"
#include "egueb_svg_element_private.h"
#include "egueb_svg_renderable_private.h"
#include "egueb_svg_element_polyline.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _egueb_svg_element_polyline_log

static int _egueb_svg_element_polyline_log = -1;

static Ender_Property *ESVG_ELEMENT_POLYLINE_POINT;

typedef struct _Egueb_Svg_Element_Polyline
{
	/* properties */
	Eina_List *points;
	/* private */
	Enesim_Renderer *proxy;
	Enesim_Renderer *line;
	Enesim_Renderer *figure;
} Egueb_Svg_Element_Polyline;

static Egueb_Svg_Element_Polyline * _egueb_svg_element_polyline_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Polyline *thiz;

	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_POLYLINE)
		return NULL;
	thiz = egueb_svg_renderable_data_get(t);

	return thiz;
}

static void _egueb_svg_element_polyline_points_cb(Egueb_Svg_Point *p, void *data)
{
	Ender_Element *e = data;

	egueb_svg_element_polyline_point_add(e, p);
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_polyline_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "points") == 0)
	{
		egueb_svg_points_string_from(value, _egueb_svg_element_polyline_points_cb, e);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_polyline_attribute_get(Egueb_Dom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _egueb_svg_element_polyline_renderer_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Polyline *thiz;

	thiz = _egueb_svg_element_polyline_get(t);
	return thiz->proxy;
}

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_polyline_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	return ESVG_SETUP_OK;
}

static Eina_Bool _egueb_svg_element_polyline_renderer_propagate(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		Egueb_Svg_Renderable_Context *rctx,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Polyline *thiz;
	Enesim_Renderer *r;
	int nvert;

	thiz = _egueb_svg_element_polyline_get(t);

	/* FIXME gets the parents size or the topmost? */
	/* FIXME we should keep the old fill renderer */

	nvert = eina_list_count(thiz->points);
	if (nvert > 2)
		r = thiz->figure;
	else
		r = thiz->line;

	/* shape properties */
	enesim_renderer_shape_fill_color_set(r, rctx->fill_color);
	enesim_renderer_shape_fill_rule_set(r, rctx->fill_rule);
	enesim_renderer_shape_fill_renderer_set(r, rctx->fill_renderer);
	enesim_renderer_shape_stroke_color_set(r, rctx->stroke_color);
	enesim_renderer_shape_stroke_weight_set(r, rctx->stroke_weight);
	enesim_renderer_shape_draw_mode_set(r, rctx->draw_mode);
	enesim_renderer_shape_stroke_location_set(r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_stroke_cap_set(r, rctx->stroke_cap);
	enesim_renderer_shape_stroke_join_set(r, rctx->stroke_join);

	/* base properties */
	enesim_renderer_transformation_set(r, &ctx->transform);
	enesim_renderer_color_set(r, rctx->color);
	DBG("calling the setup on the polyline");
	/* setup the points */
	if (nvert > 2)
	{
		Egueb_Svg_Point *p;
		Eina_List *l;

		enesim_renderer_figure_clear(r);
		enesim_renderer_figure_polygon_add(r);
		DBG("using the figure for %d vertices", nvert);
		EINA_LIST_FOREACH(thiz->points, l, p)
		{
			DBG("(%f, %f) ", p->x, p->y);
			enesim_renderer_figure_polygon_vertex_add(r, p->x, p->y);
		}
	}
	else
	{
		Egueb_Svg_Point *p;
		Egueb_Svg_Point pts[2] = { { 0, 0 }, { 0, 0 } };
		Egueb_Svg_Point *pt = pts;
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
		DBG("using the line (%f, %f) (%f, %f)", pts[0].x, pts[0].y, pts[1].x, pts[1].y);
	}
	enesim_renderer_proxy_proxied_set(thiz->proxy, r);

	return EINA_TRUE;
}

static void _egueb_svg_element_polyline_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Polyline *thiz;
	Egueb_Svg_Point *p;

	thiz = _egueb_svg_element_polyline_get(t);
	EINA_LIST_FREE(thiz->points, p)
	{
		free(p);
	}
	enesim_renderer_unref(thiz->proxy);
	enesim_renderer_unref(thiz->line);
	enesim_renderer_unref(thiz->figure);
	free(thiz);
}

static Egueb_Svg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _egueb_svg_element_polyline_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get		     = */ NULL,
	/* .free 		= */ _egueb_svg_element_polyline_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _egueb_svg_element_polyline_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ _egueb_svg_element_polyline_setup,
	/* .renderer_get	= */ _egueb_svg_element_polyline_renderer_get,
	/* .renderer_propagate	= */ _egueb_svg_element_polyline_renderer_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_svg_element_polyline_new(void)
{
	Egueb_Svg_Element_Polyline *thiz;
	Egueb_Dom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Egueb_Svg_Element_Polyline));
	if (!thiz) return NULL;

	r = enesim_renderer_proxy_new();
	thiz->proxy = r;

	r = enesim_renderer_figure_new();
	/* FIXME for now */
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->figure = r;

	r = enesim_renderer_line_new();
	/* FIXME for now */
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->line = r;


	/* default values */
	t = egueb_svg_renderable_new(&_descriptor, ESVG_TYPE_POLYLINE, thiz);
	return t;
}

static void _egueb_svg_element_polyline_point_add(Egueb_Dom_Tag *t, Egueb_Svg_Point *p)
{
	Egueb_Svg_Element_Polyline *thiz;
	Egueb_Svg_Point *new_point;

	if (!p) return;

	thiz = _egueb_svg_element_polyline_get(t);
	new_point = calloc(1, sizeof(Egueb_Svg_Point));
	*new_point = *p;
	thiz->points = eina_list_append(thiz->points, new_point);
}

/* The ender wrapper */
#define _egueb_svg_element_polyline_delete NULL
#define _egueb_svg_element_polyline_point_is_set NULL
#define _egueb_svg_element_polyline_point_set NULL
#define _egueb_svg_element_polyline_point_get NULL
#define _egueb_svg_element_polyline_point_clear NULL
#define _egueb_svg_element_polyline_point_remove NULL
#include "egueb_svg_generated_element_polyline.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_element_polyline_init(void)
{
	_egueb_svg_element_polyline_log = eina_log_domain_register("egueb_svg_element_polyline", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_element_polyline_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_element_polyline_init();
}

void egueb_svg_element_polyline_shutdown(void)
{
	if (_egueb_svg_element_polyline_log < 0)
		return;
	_egueb_svg_element_polyline_shutdown();
	eina_log_domain_unregister(_egueb_svg_element_polyline_log);
	_egueb_svg_element_polyline_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * egueb_svg_element_polyline_new(void)
{
	return ESVG_ELEMENT_NEW("SVGPolylineElement");
}

EAPI void egueb_svg_element_polyline_point_add(Ender_Element *e, Egueb_Svg_Point *p)
{
	ender_element_property_value_add(e, ESVG_ELEMENT_POLYLINE_POINT, p, NULL);
}
