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

/*
 * TODO
 * typedef struct _Egueb_Svg_Text_Content_State
 * {
 *    Eina_List *
 *    double last_x, last_y;
 * }
 * Egueb_Svg_Text_Content_State;
 * + A content must be able to generate the initial list of coordinates
 * Like [dx,dy],[dx,dy] ... [x,y],[x,y] ...
 * + At process iterate every children, at each children consume one element
 *   from the list of relative/absolute values
 * + In case a children is a text, process it ourselves
 * + In case is a text content children process the node
 * + When processing a node, first check if there's a state already set, if so
 * merge it with our own state, check overflows on the values
 * + Update the last_x, last_y when done processing
 * + For textPath elements, the initial state should be all the rotations, x and y
 * positions for every char on all of its descendants.
 * + If a child of a textPath has dx, dy, increment the x,y by them. The next glyph
 * must continue what the path said adding the previous dx,dy
 * + If a child of a textPath has a x,y the offset must be reset
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_svg_text_content_initialize_state(
		Egueb_Svg_Text_Content *thiz)
{
	Egueb_Svg_Text_Content_Class *klass;

	klass = EGUEB_SVG_TEXT_CONTENT_CLASS_GET(thiz);
	if (thiz->state.x)
	{
		egueb_dom_list_unref(thiz->state.x);
		thiz->state.x = NULL;
	}
	if (thiz->state.y)
	{
		egueb_dom_list_unref(thiz->state.y);
		thiz->state.y = NULL;
	}
	if (thiz->state.dx)
	{
		egueb_dom_list_unref(thiz->state.dx);
		thiz->state.dx = NULL;
	}
	if (thiz->state.dy)
	{
		egueb_dom_list_unref(thiz->state.dy);
		thiz->state.dy = NULL;
	}
	if (thiz->state.rotate)
	{
		egueb_dom_list_unref(thiz->state.rotate);
		thiz->state.rotate = NULL;
	}

	if (klass->initialize_state)
	{
		klass->initialize_state(thiz);
	}
	else
	{
		thiz->state.x = egueb_dom_list_new(egueb_svg_length_descriptor_get());
		thiz->state.y = egueb_dom_list_new(egueb_svg_length_descriptor_get());
		thiz->state.dx = egueb_dom_list_new(egueb_svg_length_descriptor_get());
		thiz->state.dy = egueb_dom_list_new(egueb_svg_length_descriptor_get());
		thiz->state.rotate = egueb_dom_list_new(egueb_dom_value_double_descriptor_get());
		thiz->state.pen_x = 0;
		thiz->state.pen_y = 0;
		thiz->state.pen_rot = 0;
	}
}

static void _egueb_svg_text_content_propagate(Egueb_Svg_Text_Content *thiz,
		Enesim_Renderer *ren)
{
	Egueb_Svg_Renderable *r;
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

static void _egueb_svg_text_content_process_text(Egueb_Svg_Text_Content *thiz,
		Egueb_Svg_Text_Content *other,
		Egueb_Dom_Node *tn)
{
	Egueb_Dom_Node *relative;
	Egueb_Dom_Node *doc;
	Egueb_Svg_Element *e;
	Enesim_Text_Buffer *buffer;
	Eina_Unicode unicode;
	double doc_font_size;
	int last_idx = 0;
	int idx = 0;
	const char *text;

	/* the relative node */
	relative = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(thiz));
	/* the font size */
	doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(thiz));
	doc_font_size = egueb_svg_document_font_size_get(doc);
	egueb_dom_node_unref(doc);

	buffer = egueb_dom_character_data_buffer_get(tn);
	text = enesim_text_buffer_string_get(buffer);
	while ((unicode = eina_unicode_utf8_next_get(text + idx, &last_idx)))
	{
		Enesim_Renderer *r;
		Enesim_Renderer_Compound_Layer *layer;
		Enesim_Text_Buffer *r_buffer;
		Enesim_Rectangle bounds;
		Enesim_Matrix tx;
		double x = thiz->state.pen_x;
		double y = thiz->state.pen_y;
		double rotate = thiz->state.pen_rot;
		double dx = 0;
		double dy = 0;

		r = enesim_renderer_text_span_new();
		if (other)
		{
			if (egueb_dom_list_length(other->state.x))
			{
				Egueb_Svg_Coord *c;

				c = egueb_dom_list_item_get(other->state.x, 0);
				x = egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.w, doc_font_size);
				egueb_dom_list_item_remove(other->state.x, 0);
			}

			if (egueb_dom_list_length(other->state.y))
			{
				Egueb_Svg_Coord *c;

				c = egueb_dom_list_item_get(other->state.y, 0);
				y = egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.h, doc_font_size);
				egueb_dom_list_item_remove(other->state.y, 0);
			}

			if (egueb_dom_list_length(other->state.dx))
			{
				Egueb_Svg_Coord *c;

				c = egueb_dom_list_item_get(other->state.dx, 0);
				dx += egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.w, doc_font_size);
				egueb_dom_list_item_remove(other->state.dx, 0);
			}

			if (egueb_dom_list_length(other->state.dy))
			{
				Egueb_Svg_Coord *c;

				c = egueb_dom_list_item_get(other->state.dy, 0);
				dy += egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.h, doc_font_size);
				egueb_dom_list_item_remove(other->state.dy, 0);
			}

			if (egueb_dom_list_length(other->state.rotate))
			{
				double *rot;

				rot = egueb_dom_list_item_get(other->state.rotate, 0);
				rotate = *rot;
				egueb_dom_list_item_remove(other->state.rotate, 0);
			}
		}

		if (egueb_dom_list_length(thiz->state.x))
		{
			Egueb_Svg_Coord *c;

			c = egueb_dom_list_item_get(thiz->state.x, 0);
			x = egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.w, doc_font_size);
			egueb_dom_list_item_remove(thiz->state.x, 0);
		}

		if (egueb_dom_list_length(thiz->state.y))
		{
			Egueb_Svg_Coord *c;

			c = egueb_dom_list_item_get(thiz->state.y, 0);
			y = egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.h, doc_font_size);
			egueb_dom_list_item_remove(thiz->state.y, 0);
		}

		if (egueb_dom_list_length(thiz->state.dx))
		{
			Egueb_Svg_Coord *c;

			c = egueb_dom_list_item_get(thiz->state.dx, 0);
			dx += egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.w, doc_font_size);
			egueb_dom_list_item_remove(thiz->state.dx, 0);
		}

		if (egueb_dom_list_length(thiz->state.dy))
		{
			Egueb_Svg_Coord *c;

			c = egueb_dom_list_item_get(thiz->state.dy, 0);
			dy += egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.h, doc_font_size);
			egueb_dom_list_item_remove(thiz->state.dy, 0);
		}

		if (egueb_dom_list_length(thiz->state.rotate))
		{
			double *rot;

			rot = egueb_dom_list_item_get(thiz->state.rotate, 0);
			rotate = *rot;
			egueb_dom_list_item_remove(thiz->state.rotate, 0);
		}
		x += dx;
		y += dy;

		INFO("Setting position to x: %g y: %g rotate: %g", x, y, rotate);
		enesim_renderer_text_span_font_set(r, enesim_text_font_ref(thiz->gfont));
		enesim_renderer_text_span_position_set(r, x, y);
		/* set the rotation */
		e = EGUEB_SVG_ELEMENT(thiz);
		tx = e->transform;
		if (rotate != 0)
		{
			Enesim_Matrix trx, rx, cx;

			enesim_matrix_rotate(&rx, rotate * M_PI / 180.0);
			/* rotation over the current pen position */
			enesim_matrix_translate(&trx, x, y);
			enesim_matrix_compose(&trx, &rx, &cx);
			enesim_matrix_translate(&trx, -x, -y);
			enesim_matrix_compose(&cx, &trx, &cx);
			enesim_matrix_compose(&tx, &cx, &tx);
		}
		
		INFO("matrix %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS (&tx));
		enesim_renderer_transformation_set(r, &tx);

		layer = enesim_renderer_compound_layer_new();
		enesim_renderer_compound_layer_renderer_set(layer, enesim_renderer_ref(r));
		enesim_renderer_compound_layer_rop_set(layer, ENESIM_ROP_BLEND);
		enesim_renderer_compound_layer_add(thiz->r, layer);

		/* if no more position, expand until end of text */
		if (!egueb_dom_list_length(thiz->state.x) &&
			!egueb_dom_list_length(thiz->state.y) &&
			!egueb_dom_list_length(thiz->state.dx) &&
			!egueb_dom_list_length(thiz->state.dy) &&
			!egueb_dom_list_length(thiz->state.rotate) && !rotate)
		{
			r_buffer = enesim_text_buffer_utf8_new(0);
			enesim_text_buffer_string_insert(r_buffer, text + idx, -1, 0);
			/* force a break on the loop */
			text = NULL;
			idx = 0;
		}
		else
		{
			r_buffer = enesim_text_buffer_utf8_new(0);
			enesim_text_buffer_string_insert(r_buffer, (const char *)&unicode, 1, 0);
			idx += last_idx;
			last_idx = 0;
		}
		enesim_renderer_text_span_real_buffer_set(r, r_buffer);
		/* advance the pen */
		enesim_renderer_shape_geometry_get(r, &bounds);
		thiz->state.pen_x = x + bounds.w;
		thiz->state.pen_y = y;
		thiz->state.pen_rot = rotate;
		/* add the span to the list of spans */
		_egueb_svg_text_content_propagate(thiz, r);
		thiz->spans = eina_list_append(thiz->spans, r);
	}

	egueb_dom_node_unref(relative);
	enesim_text_buffer_unref(buffer);
}

static void _egueb_svg_text_content_process_text_content(
		Egueb_Svg_Text_Content *thiz, Egueb_Dom_Node *tc)
{
	egueb_dom_element_process(tc);
 	if (thiz->renderable_tree_changed)
	{
		Enesim_Renderer *r;
		Enesim_Renderer_Compound_Layer *layer;

		/* get the text span renderer in case it is a tspan */
		r = egueb_svg_renderable_renderer_get(tc);
		layer = enesim_renderer_compound_layer_new();
		enesim_renderer_compound_layer_renderer_set(layer, enesim_renderer_ref(r));
		enesim_renderer_compound_layer_rop_set(layer, ENESIM_ROP_BLEND);
		enesim_renderer_compound_layer_add(thiz->r, layer);
		/* add the span to the list of spans */
		thiz->spans = eina_list_append(thiz->spans, r);
	}
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

	thiz = EGUEB_SVG_TEXT_CONTENT(r);
	/* In this moment the painter has done its own setup so we do know
	 * the different painter properties. Is time to setup the text
	 * renderers
	 */
	if (thiz->renderable_tree_changed)
	{
		Egueb_Svg_Text_Content *other = NULL;
		Egueb_Dom_Node *n;
		Egueb_Dom_Node *child;
		Egueb_Dom_Node *parent;
		Enesim_Renderer *ren;

		n = EGUEB_DOM_NODE(r);
		/* remove every span */
		EINA_LIST_FREE(thiz->spans, ren)
			enesim_renderer_unref(ren);
		/* remove every layer */
		enesim_renderer_compound_layer_clear(thiz->r);

		_egueb_svg_text_content_initialize_state(thiz);
		parent = egueb_dom_node_parent_get(EGUEB_DOM_NODE(thiz));
		if (!egueb_svg_is_text_content(parent))
		{
			egueb_dom_node_unref(parent);
			parent = NULL;
		}
		else
		{
			other = EGUEB_SVG_TEXT_CONTENT(parent);
			thiz->state.pen_x = other->state.pen_x;
			thiz->state.pen_y = other->state.pen_y;
			thiz->state.pen_rot = other->state.pen_rot;
			INFO("Starting with pen x: %g y: %g rot: %g",
					thiz->state.pen_x, thiz->state.pen_y,
					thiz->state.pen_rot);
		}

		child = egueb_dom_node_child_first_get(n);
		while (child)
		{
			Egueb_Dom_Node *tmp;
			Egueb_Dom_Node_Type type;

			type = egueb_dom_node_type_get(child);
			if (type == EGUEB_DOM_NODE_TYPE_TEXT)
			{
				_egueb_svg_text_content_process_text(thiz, other, child);
			}
			else if (type == EGUEB_DOM_NODE_TYPE_ELEMENT)
			{
				_egueb_svg_text_content_process_text_content(thiz, child);
			}
			/* update the parent's pen position */
			if (other)
			{
				other->state.pen_x = thiz->state.pen_x;
				other->state.pen_y = thiz->state.pen_y;
				other->state.pen_rot = thiz->state.pen_rot;
			}

			tmp = egueb_dom_node_sibling_next_get(child);
			egueb_dom_node_unref(child);
			child = tmp;
		}
		thiz->renderable_tree_changed = EINA_FALSE;
		egueb_dom_node_unref(parent);
	}
	else
	{
		Enesim_Renderer *ren;
		Eina_List *l;

		EINA_LIST_FOREACH(thiz->spans, l, ren)
			_egueb_svg_text_content_propagate(thiz, ren);
	}
}

static Eina_Bool _egueb_svg_text_content_process(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Text_Content *thiz;
	Egueb_Svg_Element *e;
	Egueb_Dom_Node *n;
	Enesim_Text_Font *font;

	thiz = EGUEB_SVG_TEXT_CONTENT(r);

	/* set the position */
	e = EGUEB_SVG_ELEMENT(r);
	thiz->gfont_size = e->final_font_size;

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
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_TEXT_CONTENT(o);
	if (thiz->painter)
	{
		egueb_svg_painter_unref(thiz->painter);
		thiz->painter = NULL;
	}
	enesim_renderer_unref(thiz->r);
	EINA_LIST_FREE(thiz->spans, r)
		enesim_renderer_unref(r);

	egueb_dom_list_unref(thiz->state.x);
	egueb_dom_list_unref(thiz->state.y);
	egueb_dom_list_unref(thiz->state.dx);
	egueb_dom_list_unref(thiz->state.dy);

	enesim_text_font_unref(thiz->gfont);
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

EAPI Eina_Bool egueb_svg_is_text_content(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_TEXT_CONTENT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}
