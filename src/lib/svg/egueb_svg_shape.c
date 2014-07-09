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
/* FIXME remove this later */
#include "egueb_svg_length.h"
#include "egueb_svg_element.h"
#include "egueb_svg_element_text.h"

#include "egueb_svg_painter_private.h"
#include "egueb_svg_shape_private.h"
#include "egueb_svg_attr_matrix_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _egueb_svg_shape_children_process_cb(
		Egueb_Dom_Node *child, void *data)
{
	Egueb_Dom_Node_Type type;

	type = egueb_dom_node_type_get(child);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT)
		return EINA_TRUE;
	egueb_dom_element_process(child);
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_shape_painter_apply(Egueb_Svg_Renderable *ren,
		Egueb_Svg_Painter *painter)
{
	Enesim_Color color;
	Enesim_Renderer_Shape_Draw_Mode draw_mode;
	Enesim_Renderer_Shape_Stroke_Dash *dash;
	Enesim_Renderer_Shape_Stroke_Cap stroke_cap;
	Enesim_Renderer_Shape_Stroke_Join stroke_join;
	Enesim_Color stroke_color;
	Enesim_Renderer *stroke_renderer;
	Enesim_Renderer *fill_renderer;
	Enesim_Color fill_color;
	Enesim_Renderer_Shape_Fill_Rule fill_rule;
	Enesim_Renderer *r = NULL;
	Eina_List *stroke_dasharray;
	Eina_List *l;
	Eina_Bool visibility;
	double stroke_weight;

	r = egueb_svg_renderable_class_renderer_get(EGUEB_DOM_NODE(ren));
	if (!r)
	{
		WARN_ELEMENT(EGUEB_DOM_NODE(ren), "No renderer available");
		return;
	}

	egueb_svg_painter_visibility_get(painter, &visibility);
	egueb_svg_painter_color_get(painter, &color);
	egueb_svg_painter_draw_mode_get(painter, &draw_mode);
	egueb_svg_painter_stroke_dasharray_get(painter, &stroke_dasharray);
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
	EINA_LIST_FOREACH(stroke_dasharray, l, dash)
	{
		/* TODO add this check on enesim itself */
		if (!dash->length) continue;
		enesim_renderer_shape_stroke_dash_add(r, dash);
	}

	enesim_renderer_shape_stroke_weight_set(r, stroke_weight);
	enesim_renderer_shape_stroke_location_set(r, ENESIM_RENDERER_SHAPE_STROKE_LOCATION_CENTER);
	enesim_renderer_shape_stroke_cap_set(r, stroke_cap);
	enesim_renderer_shape_stroke_join_set(r, stroke_join);
	enesim_renderer_shape_draw_mode_set(r, draw_mode);

	/* base properties */
	enesim_renderer_color_set(r, color);
	enesim_renderer_visibility_set(r, visibility);

	enesim_renderer_unref(r);
}

static Egueb_Svg_Painter * _egueb_svg_shape_painter_get(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Shape *thiz;

	thiz = EGUEB_SVG_SHAPE(r);
	return egueb_svg_painter_ref(thiz->painter);
}

static Eina_Bool _egueb_svg_shape_process(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Shape *thiz;
	Egueb_Svg_Shape_Class *klass;

	thiz = EGUEB_SVG_SHAPE(r);
	klass = EGUEB_SVG_SHAPE_CLASS_GET(r);

	/* generate the geometry of the element */
	if (klass->generate_geometry)
	{
		Egueb_Dom_Node *relative, *doc;
		Eina_Bool ret;

		relative = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(r));
		if (!relative)
		{
			WARN_ELEMENT(EGUEB_DOM_NODE(r), "No relative available");
			return EINA_FALSE;
		}

		doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(r));
		if (!doc)
		{
			Egueb_Dom_String *name;
			name = egueb_dom_node_name_get(EGUEB_DOM_NODE(r));
			WARN_ELEMENT(EGUEB_DOM_NODE(r), "No document set on %s", egueb_dom_string_string_get(name));
			egueb_dom_node_unref(relative);
			return EINA_FALSE;
		}

		ret = klass->generate_geometry(thiz,
				EGUEB_SVG_ELEMENT(relative), doc);
		egueb_dom_node_unref(doc);
		egueb_dom_node_unref(relative);

		if (!ret)
		{
			WARN_ELEMENT(EGUEB_DOM_NODE(r), "Failed generating the geometry");
			return EINA_FALSE;
		}
	}

	/* process the children of the shapes, only smil nodes */
	/* FIXME once the text element is moved out of the shape inheritance
	 * we can safely remove this check. Basically a text handles
	 * its own child process
	 */
	if (!egueb_svg_element_is_text(EGUEB_DOM_NODE(r)))
		egueb_dom_node_children_foreach(EGUEB_DOM_NODE(r),
				_egueb_svg_shape_children_process_cb,
				NULL);

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
	Egueb_Svg_Renderable_Class *r_klass;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->process = _egueb_svg_shape_process;
	r_klass->painter_apply = _egueb_svg_shape_painter_apply;
	r_klass->painter_get = _egueb_svg_shape_painter_get;
}

static void _egueb_svg_shape_instance_init(void *o)
{
	Egueb_Svg_Shape *thiz;

	thiz = EGUEB_SVG_SHAPE(o);
	/* the properties */
	/* set the shape painter by default */
	thiz->painter = egueb_svg_painter_shape_new();
}

static void _egueb_svg_shape_instance_deinit(void *o)
{
	Egueb_Svg_Shape *thiz;

	thiz = EGUEB_SVG_SHAPE(o);
	/* the properties */
	/* the painter */
	if (thiz->painter)
	{
		egueb_svg_painter_unref(thiz->painter);
		thiz->painter = NULL;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
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
