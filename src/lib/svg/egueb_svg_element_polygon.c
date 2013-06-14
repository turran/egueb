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
#include "egueb_svg_element_polygon.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _egueb_svg_element_polygon_log

static int _egueb_svg_element_polygon_log = -1;

static Ender_Property *ESVG_ELEMENT_POLYGON_POINT;

typedef struct _Egueb_Svg_Element_Polygon
{
	/* properties */
	Eina_List *points;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Egueb_Svg_Element_Polygon;

static Egueb_Svg_Element_Polygon * _egueb_svg_element_polygon_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Polygon *thiz;

	if (egueb_svg_element_internal_type_get(t) != ESVG_TYPE_POLYGON)
		return NULL;
	thiz = egueb_svg_renderable_data_get(t);

	return thiz;
}

static void _egueb_svg_element_polygon_points_cb(Egueb_Svg_Point *p, void *data)
{
	Ender_Element *e = data;

	egueb_svg_element_polygon_point_add(e, p);
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_polygon_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "points") == 0)
	{
		egueb_svg_points_string_from(value, _egueb_svg_element_polygon_points_cb, e);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_polygon_attribute_get(Egueb_Dom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _egueb_svg_element_polygon_renderer_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Polygon *thiz;

	thiz = _egueb_svg_element_polygon_get(t);
	return thiz->r;
}

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_polygon_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	return ESVG_SETUP_OK;
}

static Eina_Bool _egueb_svg_element_polygon_renderer_propagate(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		Egueb_Svg_Renderable_Context *rctx,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Polygon *thiz;

	thiz = _egueb_svg_element_polygon_get(t);

	DBG("polygon setup");
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
	enesim_renderer_transformation_set(thiz->r, &ctx->transform);
	enesim_renderer_color_set(thiz->r, rctx->color);

	return EINA_TRUE;
}

static void _egueb_svg_element_polygon_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Polygon *thiz;
	Egueb_Svg_Point *p;

	thiz = _egueb_svg_element_polygon_get(t);
	EINA_LIST_FREE(thiz->points, p)
	{
		free(p);
	}
	enesim_renderer_unref(thiz->r);
	free(thiz);
}

static Egueb_Svg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _egueb_svg_element_polygon_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get		     = */ NULL,
	/* .free 		= */ _egueb_svg_element_polygon_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _egueb_svg_element_polygon_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ _egueb_svg_element_polygon_setup,
	/* .renderer_get	= */ _egueb_svg_element_polygon_renderer_get,
	/* .renderer_propagate	= */ _egueb_svg_element_polygon_renderer_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Tag * _egueb_svg_element_polygon_new(void)
{
	Egueb_Svg_Element_Polygon *thiz;
	Egueb_Dom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Egueb_Svg_Element_Polygon));
	if (!thiz) return NULL;

	r = enesim_renderer_figure_new();
	/* FIXME for now */
	enesim_renderer_figure_polygon_add(r);
	enesim_renderer_rop_set(r, ENESIM_BLEND);

	thiz->r = r;
	/* default values */

	t = egueb_svg_renderable_new(&_descriptor, ESVG_TYPE_POLYGON, thiz);
	return t;
}

static void _egueb_svg_element_polygon_point_add(Egueb_Dom_Tag *t, Egueb_Svg_Point *p)
{
	Egueb_Svg_Element_Polygon *thiz;
	Egueb_Svg_Point *new_point;

	if (!p) return;

	thiz = _egueb_svg_element_polygon_get(t);
	new_point = calloc(1, sizeof(Egueb_Svg_Point));
	*new_point = *p;
	thiz->points = eina_list_append(thiz->points, new_point);
	enesim_renderer_figure_polygon_vertex_add(thiz->r, new_point->x, new_point->y);
	thiz->changed = EINA_TRUE;
}

static void _egueb_svg_element_polygon_point_set(Egueb_Dom_Tag *t, const Eina_List *pts)
{
	Egueb_Svg_Element_Polygon *thiz;
	Egueb_Svg_Point *pt;
	const Eina_List *l;

	thiz = _egueb_svg_element_polygon_get(t);
	/* FIXME remove what we had */
	EINA_LIST_FOREACH (pts, l, pt)
	{
		_egueb_svg_element_polygon_point_add(t, pt);
	}
}

static void _egueb_svg_element_polygon_point_get(Egueb_Dom_Tag *t, const Eina_List **pts)
{
	Egueb_Svg_Element_Polygon *thiz;

	thiz = _egueb_svg_element_polygon_get(t);
	*pts = thiz->points;
}

/* The ender wrapper */
#define _egueb_svg_element_polygon_delete NULL
#define _egueb_svg_element_polygon_point_is_set NULL
#define _egueb_svg_element_polygon_point_clear NULL
#define _egueb_svg_element_polygon_point_remove NULL
#include "egueb_svg_generated_element_polygon.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_element_polygon_init(void)
{
	_egueb_svg_element_polygon_log = eina_log_domain_register("egueb_svg_element_polygon", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_element_polygon_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_element_polygon_init();
}

void egueb_svg_element_polygon_shutdown(void)
{
	if (_egueb_svg_element_polygon_log < 0)
		return;
	_egueb_svg_element_polygon_shutdown();
	eina_log_domain_unregister(_egueb_svg_element_polygon_log);
	_egueb_svg_element_polygon_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * egueb_svg_element_polygon_new(void)
{
	return ESVG_ELEMENT_NEW("SVGPolygonElement");
}

EAPI Eina_Bool egueb_svg_is_polygon(Ender_Element *e)
{
	Egueb_Dom_Tag *t;
	Egueb_Svg_Type type;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	type = egueb_svg_element_internal_type_get(t);
	return (type == ESVG_TYPE_POLYGON) ? EINA_TRUE : EINA_FALSE;
}

EAPI void egueb_svg_element_polygon_point_add(Ender_Element *e, Egueb_Svg_Point *p)
{
	ender_element_property_value_add(e, ESVG_ELEMENT_POLYGON_POINT, p, NULL);
}
