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
#include "egueb_svg_text_content.h"
#include "egueb_svg_text_content_private.h"
#include "egueb_dom_string_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_svg_text_content_children_generate_geometry(
		Egueb_Svg_Text_Content *thiz, Enesim_Renderer *r)
{
#if 0
	Egueb_Svg_Element *e;
	Egueb_Svg_Text_Content_Pen *pen = &thiz->pen;
	Enesim_Rectangle bounds;
	int size;

	e = EGUEB_SVG_ELEMENT(thiz);
	size = ceil(thiz->gfont_size);
	if (size < 0)
	{
		ERR("Negative font size of %d", size);
		size = 0;
	}
	enesim_renderer_text_span_font_set(r, enesim_text_font_ref(thiz->gfont));
	enesim_renderer_text_span_position_set(r, pen->x, pen->y);

	INFO("matrix %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS (&e->transform));
	enesim_renderer_transformation_set(r, &e->transform);

	INFO("x: %g, y: %g, font-size: %d", pen->x, pen->y, size);
	/* increment the pen position */
	enesim_renderer_shape_geometry_get(r, &bounds);
	INFO("advancing by w: %g, h: %g", bounds.w, bounds.h);
	pen->x += bounds.w;
#endif
}

static Eina_Bool _egueb_svg_text_content_children_process_cb(Egueb_Dom_Node *child,
		void *data)
{
	Egueb_Svg_Text_Content *thiz = data;
	Egueb_Dom_Node_Type type;

	type = egueb_dom_node_type_get(child);
	if (type == EGUEB_DOM_NODE_TYPE_TEXT)
	{
		Egueb_Dom_String *private_data;
		Egueb_Svg_Renderable *r;
		Enesim_Renderer *ren;
		Enesim_Color color;
		Enesim_Renderer_Shape_Draw_Mode draw_mode;
		Enesim_Renderer_Shape_Stroke_Dash *dash;
		Enesim_Renderer_Shape_Stroke_Cap stroke_cap;
		Enesim_Renderer_Shape_Stroke_Join stroke_join;
		Enesim_Color stroke_color;
		Enesim_Renderer *stroke_renderer;
		Enesim_Renderer *fill_renderer;
		Enesim_Color fill_color;
		/* TODO we still miss the fill rule */
		Eina_List *stroke_dasharray;
		Eina_List *l;
		Eina_Bool visibility;
		double stroke_weight;

		private_data = egueb_dom_string_new_with_static_chars("_renderer");
		ren = egueb_dom_node_user_data_get(child, private_data);
		egueb_dom_string_unref(private_data);

		_egueb_svg_text_content_children_generate_geometry(thiz, ren);
		if (thiz->renderable_tree_changed)
		{
			Enesim_Renderer_Compound_Layer *layer;

			layer = enesim_renderer_compound_layer_new();
			enesim_renderer_compound_layer_renderer_set(layer, enesim_renderer_ref(ren));
			enesim_renderer_compound_layer_rop_set(layer, ENESIM_ROP_BLEND);
			enesim_renderer_compound_layer_add(thiz->r, layer);
		}

		r = EGUEB_SVG_RENDERABLE(thiz);
		egueb_svg_painter_visibility_get(r->painter, &visibility);
		egueb_svg_painter_color_get(r->painter, &color);
		egueb_svg_painter_draw_mode_get(r->painter, &draw_mode);
		egueb_svg_painter_stroke_dasharray_get(r->painter, &stroke_dasharray);
		egueb_svg_painter_stroke_cap_get(r->painter, &stroke_cap);
		egueb_svg_painter_stroke_join_get(r->painter, &stroke_join);
		egueb_svg_painter_stroke_color_get(r->painter, &stroke_color);
		egueb_svg_painter_stroke_weight_get(r->painter, &stroke_weight);
		egueb_svg_painter_stroke_renderer_get(r->painter, &stroke_renderer);
		egueb_svg_painter_fill_color_get(r->painter, &fill_color);
		egueb_svg_painter_fill_renderer_get(r->painter, &fill_renderer);

		enesim_renderer_shape_fill_color_set(ren, fill_color);
		enesim_renderer_shape_fill_renderer_set(ren, fill_renderer);
		enesim_renderer_shape_stroke_color_set(ren, stroke_color);
		enesim_renderer_shape_stroke_renderer_set(ren, stroke_renderer);
		EINA_LIST_FOREACH(stroke_dasharray, l, dash)
		{
			/* TODO add this check on enesim itself */
			if (!dash->length) continue;
			enesim_renderer_shape_stroke_dash_add(ren, dash);
		}

		enesim_renderer_shape_stroke_weight_set(ren, stroke_weight);
		enesim_renderer_shape_stroke_location_set(ren, ENESIM_RENDERER_SHAPE_STROKE_LOCATION_CENTER);
		enesim_renderer_shape_stroke_cap_set(ren, stroke_cap);
		enesim_renderer_shape_stroke_join_set(ren, stroke_join);
		enesim_renderer_shape_draw_mode_set(ren, draw_mode);

		/* base properties */
		enesim_renderer_color_set(ren, color);
		enesim_renderer_visibility_set(ren, visibility);
	}
	else if (type == EGUEB_DOM_NODE_TYPE_ELEMENT)
	{
		egueb_dom_element_process(child);
 		if (egueb_svg_is_element_tspan(child) && thiz->renderable_tree_changed)
		{
			Enesim_Renderer *r;
			Enesim_Renderer_Compound_Layer *layer;

			/* get the text span renderer in case it is a tspan */
			r = egueb_svg_renderable_renderer_get(child);
			layer = enesim_renderer_compound_layer_new();
			enesim_renderer_compound_layer_renderer_set(layer, r);
			enesim_renderer_compound_layer_rop_set(layer, ENESIM_ROP_BLEND);
			enesim_renderer_compound_layer_add(thiz->r, layer);
		}
	}
	else
	{
		return EINA_TRUE;
	}

	return EINA_TRUE;
}

/* FIXME when a node is inserted/removed, we need to process again the
 * the text node? to make every x, y, move?
 */
static void _egueb_svg_text_content_node_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *related = NULL;
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node_Type type;

	related = egueb_dom_event_mutation_related_get(e);
	if (related != n)
	{
		goto not_us;
	}
	target = EGUEB_DOM_NODE(egueb_dom_event_target_get(e));
	type = egueb_dom_node_type_get(target);
	if (type == EGUEB_DOM_NODE_TYPE_TEXT)
	{
		Egueb_Svg_Text_Content *thiz;
		Egueb_Dom_String *private_data;
		Enesim_Text_Buffer *nb = NULL;
		Enesim_Renderer *r;

		thiz = EGUEB_SVG_TEXT_CONTENT(n);
		/* create a renderer for this text node */
		r = enesim_renderer_text_span_new();
		/* set the internal buffer of the text span to be the one
		 * on the text node */
		nb = egueb_dom_character_data_buffer_get(target);
		enesim_renderer_text_span_real_buffer_set(r, nb);
		/* now make the text node to use our own buffer from now on */
		nb = enesim_renderer_text_span_buffer_get(r);
		egueb_dom_character_data_buffer_set(target, nb);

		/* set the private data */
		private_data = egueb_dom_string_new_with_static_chars("_renderer");
		egueb_dom_node_user_data_set(target, private_data, r);
		egueb_dom_string_unref(private_data);

		/* mark it as a change */
		thiz->renderable_tree_changed = EINA_TRUE;
	}
	else if (type == EGUEB_DOM_NODE_TYPE_ELEMENT)
	{
		Egueb_Svg_Text_Content *thiz;

		thiz = EGUEB_SVG_TEXT_CONTENT(n);
		/* mark it as a change */
		thiz->renderable_tree_changed = EINA_TRUE;
	}
	egueb_dom_node_unref(target);
not_us:
	egueb_dom_node_unref(related);
}

static void _egueb_svg_text_content_node_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *related = NULL;
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node_Type type;

	related = egueb_dom_event_mutation_related_get(e);
	if (related != n)
	{
		goto not_us;
	}
	target = EGUEB_DOM_NODE(egueb_dom_event_target_get(e));
	type = egueb_dom_node_type_get(target);
	if (type == EGUEB_DOM_NODE_TYPE_TEXT)
	{
		Egueb_Svg_Text_Content *thiz;
		Egueb_Dom_String *private_data;
		Enesim_Renderer *r;
		Enesim_Text_Buffer *nb;

		thiz = EGUEB_SVG_TEXT_CONTENT(n);

		/* get the private data */
		private_data = egueb_dom_string_new_with_static_chars("_renderer");
		r = egueb_dom_node_user_data_get(target, private_data);

		nb = enesim_renderer_text_span_real_buffer_get(r);
		egueb_dom_character_data_buffer_set(target, nb);

		enesim_renderer_unref(r);
		egueb_dom_node_user_data_set(target, private_data, NULL);
		egueb_dom_string_unref(private_data);
		/* mark it as a change */
		thiz->renderable_tree_changed = EINA_TRUE;
	}
	else if (type == EGUEB_DOM_NODE_TYPE_ELEMENT)
	{
		Egueb_Svg_Text_Content *thiz;

		thiz = EGUEB_SVG_TEXT_CONTENT(n);
		/* mark it as a change */
		thiz->renderable_tree_changed = EINA_TRUE;
	}
	egueb_dom_node_unref(target);
not_us:
	egueb_dom_node_unref(related);
}

/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Egueb_Svg_Painter * _egueb_svg_text_content_painter_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Text_Content *thiz;

	thiz = EGUEB_SVG_TEXT_CONTENT(r);
	return egueb_svg_painter_ref(thiz->painter);
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_text_content_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Text_Content *thiz;

	thiz = EGUEB_SVG_TEXT_CONTENT(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_text_content_painter_apply(Egueb_Svg_Renderable *r,
		Egueb_Svg_Painter *painter)
{
	Egueb_Svg_Text_Content *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_TEXT_CONTENT(r);
	n = EGUEB_DOM_NODE(r);
	/* In this moment the painter has done its own setup so we do know
	 * the different painter properties. Is time to setup the text
	 * renderers
	 */
	egueb_dom_node_children_foreach(n,
		_egueb_svg_text_content_children_process_cb,
		thiz);
	thiz->renderable_tree_changed = EINA_FALSE;
}

static Eina_Bool _egueb_svg_text_content_process(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Text_Content *thiz;
	Egueb_Svg_Element *e;
	Egueb_Svg_Length *c;
	Egueb_Dom_List *l;
	Egueb_Dom_Node *n;
	Egueb_Dom_Node *relative;
	Egueb_Dom_Node *doc;
	Enesim_Text_Font *font;
	double doc_font_size;

	thiz = EGUEB_SVG_TEXT_CONTENT(r);

	/* calculate the real size */
	doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(r));
	doc_font_size = egueb_svg_document_font_size_get(doc);
	egueb_dom_node_unref(doc);

	relative = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(r));
#if 0
	egueb_dom_attr_final_get(thiz->x, &l);
	c = egueb_dom_list_item_get(l, 0);
	thiz->gx = egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.w, doc_font_size);
	egueb_dom_list_unref(l);

	egueb_dom_attr_final_get(thiz->y, &l);
	c = egueb_dom_list_item_get(l, 0);
	thiz->gy = egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.h, doc_font_size);
	egueb_dom_node_unref(relative);
#endif
	/* set the position */
	e = EGUEB_SVG_ELEMENT(r);
	thiz->gfont_size = e->final_font_size;

	/* iterate over the children and generate the position */
	/* reset our pen to */
#if 0
	thiz->pen.x = thiz->gx;
	thiz->pen.y = thiz->gy;
#endif

	n = EGUEB_DOM_NODE(r);
	font = egueb_svg_element_font_resolve(n);
	if (thiz->gfont)
	{
		enesim_text_font_unref(thiz->gfont);
		thiz->gfont = NULL;
	}
	if (font)
	{
		thiz->gfont = font;
	}

	if (!thiz->gfont)
	{
		ERR("No valid font found");
		return EINA_FALSE;
	}

	if (thiz->renderable_tree_changed)
	{
		enesim_renderer_compound_layer_clear(thiz->r);
	}
	return EINA_TRUE;
}


/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_RENDERABLE_DESCRIPTOR,
		Egueb_Svg_Text_Content, Egueb_Svg_Text_Content_Class,
		egueb_svg_text_content);

static void _egueb_svg_text_content_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *r_klass;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->painter_get = _egueb_svg_text_content_painter_get;
	r_klass->renderer_get = _egueb_svg_text_content_renderer_get;
	r_klass->process = _egueb_svg_text_content_process;
	r_klass->painter_apply = _egueb_svg_text_content_painter_apply;
}

static void _egueb_svg_text_content_instance_init(void *o)
{
	Egueb_Svg_Text_Content *thiz;
	Egueb_Dom_Event_Target *evt;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_TEXT_CONTENT(o);

	/* Default values */
	thiz->painter = egueb_svg_painter_shape_new();
	thiz->r = enesim_renderer_compound_new();

	n = EGUEB_DOM_NODE(o);
	/* add the events */
	evt = EGUEB_DOM_EVENT_TARGET_CAST(n);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_text_content_node_inserted_cb,
			EINA_TRUE, thiz);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_text_content_node_removed_cb,
			EINA_TRUE, thiz);
}

static void _egueb_svg_text_content_instance_deinit(void *o)
{
	Egueb_Svg_Text_Content *thiz;

	thiz = EGUEB_SVG_TEXT_CONTENT(o);
	if (thiz->painter)
	{
		egueb_svg_painter_unref(thiz->painter);
		thiz->painter = NULL;
	}
	enesim_renderer_unref(thiz->r);

#if 0
	enesim_text_font_unref(thiz->gfont);
#endif
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
#if 0
// lengthAdjust Types
  const unsigned short LENGTHADJUST_UNKNOWN = 0;
  const unsigned short LENGTHADJUST_SPACING = 1;
  const unsigned short LENGTHADJUST_SPACINGANDGLYPHS = 2;

  readonly attribute SVGAnimatedLength textLength;
  readonly attribute SVGAnimatedEnumeration lengthAdjust;

  long getNumberOfChars();
  float getComputedTextLength();
  float getSubStringLength(in unsigned long charnum, in unsigned long nchars) raises(DOMException);
  SVGPoint getStartPositionOfChar(in unsigned long charnum) raises(DOMException);
  SVGPoint getEndPositionOfChar(in unsigned long charnum) raises(DOMException);
  SVGRect getExtentOfChar(in unsigned long charnum) raises(DOMException);
  float getRotationOfChar(in unsigned long charnum) raises(DOMException);
  long getCharNumAtPosition(in SVGPoint point);
  void selectSubString(in unsigned long charnum, in unsigned long nchars) raises(DOMException);
#endif
