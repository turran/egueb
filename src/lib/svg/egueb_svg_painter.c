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
#include "egueb_svg_painter_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(ENESIM_OBJECT_DESCRIPTOR,
		Egueb_Svg_Painter, Egueb_Svg_Painter_Class, egueb_svg_painter);

static void _egueb_svg_painter_class_init(void *k)
{
}

static void _egueb_svg_painter_instance_init(void *o)
{
}

static void _egueb_svg_painter_instance_deinit(void *o)
{
	Egueb_Svg_Painter *thiz;

	thiz = EGUEB_SVG_PAINTER(o);
	if (thiz->stroke_renderer)
	{
		enesim_renderer_unref(thiz->stroke_renderer);
		thiz->stroke_renderer = NULL;
	}
	if (thiz->fill_renderer)
	{
		enesim_renderer_unref(thiz->fill_renderer);
		thiz->fill_renderer = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_painter_resolve(Egueb_Svg_Painter *thiz,
		Egueb_Svg_Element *e)
{
	Egueb_Svg_Painter_Class *klass;

	klass = EGUEB_SVG_PAINTER_CLASS_GET(thiz);
	if (klass->resolve)
		return klass->resolve(thiz, e);
	return EINA_FALSE;
}

void egueb_svg_painter_free(Egueb_Svg_Painter *thiz)
{
	enesim_object_instance_free(ENESIM_OBJECT_INSTANCE(thiz));
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_svg_painter_visibility_get(const Egueb_Svg_Painter *thiz,
		Eina_Bool *visibility)
{
	*visibility = thiz->visibility;
}

EAPI void egueb_svg_painter_color_get(const Egueb_Svg_Painter *thiz,
		Enesim_Color *color)
{
	*color = thiz->color;
}

EAPI void egueb_svg_painter_draw_mode_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer_Shape_Draw_Mode *draw_mode)
{
	*draw_mode = thiz->draw_mode;
}

EAPI void egueb_svg_painter_stroke_dasharray_get(const Egueb_Svg_Painter *thiz,
		Eina_List **l)
{
	*l = thiz->stroke_dasharray;
}

EAPI void egueb_svg_painter_stroke_cap_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer_Shape_Stroke_Cap *stroke_cap)
{
	*stroke_cap = thiz->stroke_cap;
}

EAPI void egueb_svg_painter_stroke_join_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer_Shape_Stroke_Join *stroke_join)
{
	*stroke_join = thiz->stroke_join;
}

EAPI void egueb_svg_painter_stroke_color_get(const Egueb_Svg_Painter *thiz,
		Enesim_Color *stroke_color)
{
	*stroke_color = thiz->stroke_color;
}

EAPI void egueb_svg_painter_stroke_weight_get(const Egueb_Svg_Painter *thiz,
		double *stroke_weight)
{
	*stroke_weight = thiz->stroke_weight;
}

EAPI void egueb_svg_painter_stroke_renderer_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer **stroke_renderer)
{
	if (thiz->stroke_renderer)
		*stroke_renderer = enesim_renderer_ref(thiz->stroke_renderer);
	else
		*stroke_renderer = NULL;
}

EAPI void egueb_svg_painter_fill_color_get(const Egueb_Svg_Painter *thiz,
		Enesim_Color *fill_color)
{
	*fill_color = thiz->fill_color;
}

EAPI void egueb_svg_painter_fill_renderer_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer **fill_renderer)
{
	if (thiz->fill_renderer)
		*fill_renderer = enesim_renderer_ref(thiz->fill_renderer);
	else
		*fill_renderer = NULL;
}

EAPI void egueb_svg_painter_fill_rule_get(const Egueb_Svg_Painter *thiz,
		Enesim_Renderer_Shape_Fill_Rule *fill_rule)
{
	*fill_rule = thiz->fill_rule;
}
