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
#include "egueb_svg_matrix.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_painter_private.h"
#include "egueb_svg_shape_private.h"
#include "egueb_svg_attr_matrix.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *                              Shape interface                               *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_shape_renderer_propagate(Egueb_Svg_Shape *thiz,
		Egueb_Svg_Painter *painter)
{
	Egueb_Svg_Shape_Class *klass;
	Enesim_Color color;
	Enesim_Shape_Draw_Mode draw_mode;
	Enesim_Shape_Stroke_Cap stroke_cap;
	Enesim_Shape_Stroke_Join stroke_join;
	Enesim_Color stroke_color;
	Enesim_Renderer *stroke_renderer;
	double stroke_weight;
	Enesim_Renderer *fill_renderer;
	Enesim_Color fill_color;
	Enesim_Shape_Fill_Rule fill_rule;
	Enesim_Renderer *r = NULL;
	Eina_Bool visibility;

	r = egueb_svg_renderable_class_renderer_get(EGUEB_DOM_NODE(thiz));
	if (!r)
	{
		WARN("No renderer available");
		return;
	}

	egueb_svg_painter_visibility_get(painter, &visibility);
	egueb_svg_painter_color_get(painter, &color);
	egueb_svg_painter_draw_mode_get(painter, &draw_mode);
	egueb_svg_painter_stroke_cap_get(painter, &stroke_cap);
	egueb_svg_painter_stroke_join_get(painter, &stroke_join);
	egueb_svg_painter_stroke_color_get(painter, &stroke_color);
	egueb_svg_painter_stroke_weight_get(painter, &stroke_weight);
	egueb_svg_painter_stroke_renderer_get(painter, &stroke_renderer);
	egueb_svg_painter_fill_color_get(painter, &fill_color);
	egueb_svg_painter_fill_renderer_get(painter, &fill_renderer);
	egueb_svg_painter_fill_rule_get(painter, &fill_rule);

	/* shape properties */
	enesim_renderer_shape_fill_color_set(r, fill_color);
	enesim_renderer_shape_fill_renderer_set(r, fill_renderer);
	enesim_renderer_shape_stroke_color_set(r, stroke_color);
	enesim_renderer_shape_stroke_renderer_set(r, stroke_renderer);

	enesim_renderer_shape_stroke_weight_set(r, stroke_weight);
	enesim_renderer_shape_stroke_location_set(r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_stroke_cap_set(r, stroke_cap);
	enesim_renderer_shape_stroke_join_set(r, stroke_join);
	enesim_renderer_shape_draw_mode_set(r, draw_mode);

	/* base properties */
	enesim_renderer_color_set(r, color);
	enesim_renderer_visibility_set(r, visibility);

	enesim_renderer_unref(r);
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_shape_process(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Shape *thiz;
	Egueb_Svg_Shape_Class *klass;

	thiz = EGUEB_SVG_SHAPE(r);
	klass = EGUEB_SVG_SHAPE_CLASS_GET(r);

	/* generate the geometry of the element */
	if (klass->generate_geometry)
	{
		if (!klass->generate_geometry(thiz))
		{
			WARN("Failed generating the geometry");
			return EINA_FALSE;
		}
	}

	/* propagate the presentation attributes */
	/* resolve the painter based on the presentation attributes */
	if (!thiz->painter)
	{
		WARN("No painter available");
		return EINA_FALSE;
	}

	if (!egueb_svg_painter_resolve(thiz->painter, EGUEB_SVG_ELEMENT(thiz)))
	{
		WARN("Painter resolving failed");
		return EINA_FALSE;
	}

	/* finally call the renderer propagate implementation */
	if (klass->renderer_propagate)
		klass->renderer_propagate(thiz, thiz->painter);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_RENDERABLE_DESCRIPTOR,
		Egueb_Svg_Shape, Egueb_Svg_Shape_Class,
		egueb_svg_shape);

static void _egueb_svg_shape_class_init(void *k)
{
	Egueb_Svg_Shape_Class *s_klass;
	Egueb_Svg_Renderable_Class *r_klass;

	s_klass = EGUEB_SVG_SHAPE_CLASS(k);
	s_klass->renderer_propagate = _egueb_svg_shape_renderer_propagate;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->process = _egueb_svg_shape_process;
}

static void _egueb_svg_shape_class_deinit(void *k)
{
}

static void _egueb_svg_shape_instance_init(void *o)
{
	Egueb_Svg_Shape *thiz;

	thiz = EGUEB_SVG_SHAPE(o);
	/* the properties */
}

static void _egueb_svg_shape_instance_deinit(void *o)
{
	Egueb_Svg_Shape *thiz;

	thiz = EGUEB_SVG_SHAPE(o);
	/* the properties */
	/* remove the painter */
	if (thiz->painter)
	{
		egueb_svg_painter_free(thiz->painter);
		thiz->painter = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_shape_painter_set(Egueb_Dom_Node *n, Egueb_Svg_Painter *painter)
{
	Egueb_Svg_Shape *thiz;

	thiz = EGUEB_SVG_SHAPE(n);
	if (thiz->painter)
	{
		egueb_svg_painter_free(thiz->painter);
		thiz->painter = NULL;
	}
	if (painter)
	{
		thiz->painter = painter;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_is_shape(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_SHAPE_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}
