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
#include "egueb_svg_length.h"
#include "egueb_svg_element_text.h"
#include "egueb_svg_element_tspan.h"
#include "egueb_svg_document.h"
#include "egueb_svg_shape_private.h"
#include "egueb_svg_element_text_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_TEXT_DESCRIPTOR egueb_svg_element_text_descriptor_get()
#define EGUEB_SVG_ELEMENT_TEXT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Text_Class, EGUEB_SVG_ELEMENT_TEXT_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_TEXT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Text, EGUEB_SVG_ELEMENT_TEXT_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Text
{
	Egueb_Svg_Shape base;
	/* properties */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	/* private */
	/* TODO put the private string here */
	double gx;
	double gy;
	double gfont;
	Enesim_Text_Font *font;
	Enesim_Renderer *r;
	Eina_Bool renderable_tree_changed;
	Egueb_Svg_Element_Text_Pen pen;
} Egueb_Svg_Element_Text;

typedef struct _Egueb_Svg_Element_Text_Class
{
	Egueb_Svg_Shape_Class base;
} Egueb_Svg_Element_Text_Class;

static void _egueb_svg_element_text_children_generate_geometry(
		Egueb_Svg_Element_Text *thiz, Enesim_Renderer *r)
{
	Egueb_Svg_Element *e;
	Egueb_Svg_Element_Text_Pen *pen = &thiz->pen;
	Enesim_Rectangle bounds;
	int max;
	int size;

	e = EGUEB_SVG_ELEMENT(thiz);
	size = ceil(thiz->gfont);
	if (size < 0)
	{
		ERR("Negative font size of %d", size);
		size = 0;
	}
	enesim_renderer_text_span_font_set(r, enesim_text_font_ref(thiz->font));
	max = enesim_text_font_max_ascent_get(thiz->font);
	enesim_renderer_origin_set(r, pen->x, pen->y - max);

	INFO("matrix %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS (&e->transform));
	enesim_renderer_transformation_set(r, &e->transform);

	INFO("x: %g, y: %g, font-size: %d", pen->x, pen->y, size);
	/* increment the pen position */
	enesim_renderer_shape_geometry_get(r, &bounds);
	INFO("advancing by w: %g, h: %g", bounds.w, bounds.h);
	pen->x += bounds.w;
}

static Eina_Bool _egueb_svg_element_text_children_process_cb(Egueb_Dom_Node *child,
		void *data)
{
	Egueb_Svg_Element_Text *thiz = data;
	Egueb_Dom_Node_Type type;

	egueb_dom_node_type_get(child, &type);
	if (type == EGUEB_DOM_NODE_TYPE_TEXT_NODE)
	{
		Egueb_Dom_String *private_data;
		Enesim_Renderer *r;

		private_data = egueb_dom_string_new_with_static_string("_renderer");
		r = egueb_dom_node_user_data_get(child, private_data);
		egueb_dom_string_unref(private_data);

		_egueb_svg_element_text_children_generate_geometry(thiz, r);
		if (thiz->renderable_tree_changed)
		{
			Enesim_Renderer_Compound_Layer *layer;

			layer = enesim_renderer_compound_layer_new();
			enesim_renderer_compound_layer_renderer_set(layer, enesim_renderer_ref(r));
			enesim_renderer_compound_layer_rop_set(layer, ENESIM_ROP_BLEND);
			enesim_renderer_compound_layer_add(thiz->r, layer);
		}
	}
	else if (type == EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
	{
		egueb_dom_element_process(child);
 		if (egueb_svg_is_element_tspan(child) && thiz->renderable_tree_changed)
		{
			Enesim_Renderer *r;
			Enesim_Renderer_Compound_Layer *layer;

			/* get the tspan renderer in case it is a tspan */
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

static Eina_Bool _egueb_svg_element_text_children_propagate_cb(Egueb_Dom_Node *child,
		void *data)
{
	Egueb_Svg_Element_Text *thiz = data;
	Egueb_Dom_Node_Type type;
	Enesim_Color color;
	Enesim_Color fill_color;
	Eina_Bool visibility;

	egueb_dom_node_type_get(child, &type);
	if (type == EGUEB_DOM_NODE_TYPE_TEXT_NODE)
	{
		Egueb_Dom_String *private_data;
		Egueb_Svg_Shape *s;
		Enesim_Renderer *r;

		private_data = egueb_dom_string_new_with_static_string("_renderer");
		r = egueb_dom_node_user_data_get(child, private_data);
		egueb_dom_string_unref (private_data);

		s = EGUEB_SVG_SHAPE(thiz);
		egueb_svg_painter_visibility_get(s->painter, &visibility);
		egueb_svg_painter_color_get(s->painter, &color);
		egueb_svg_painter_fill_color_get(s->painter, &fill_color);

		enesim_renderer_color_set(r, fill_color);
		enesim_renderer_visibility_set(r, visibility);
	}
	return EINA_TRUE;
}

static void _egueb_svg_element_text_node_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *related = NULL;
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node_Type type;

	egueb_dom_event_mutation_related_get(e, &related);
	if (related != n)
	{
		goto not_us;
	}
	egueb_dom_event_target_get(e, &target);
	egueb_dom_node_type_get(target, &type);
	if (type == EGUEB_DOM_NODE_TYPE_TEXT_NODE)
	{
		Egueb_Svg_Element_Text *thiz;
		Egueb_Dom_String *private_data;
		Enesim_Text_Buffer *nb = NULL;
		Enesim_Renderer *r;

		thiz = EGUEB_SVG_ELEMENT_TEXT(n);
		/* create a renderer for this text node */
		r = enesim_renderer_text_span_new();
		enesim_renderer_text_span_font_set(r, enesim_text_font_ref(thiz->font));
		enesim_renderer_color_set(r, 0xff000000);

		/* set the internal buffer of the text span to be the one
		 * on the text node */
		egueb_dom_character_data_buffer_get(target, &nb);
		enesim_renderer_text_span_real_buffer_set(r, nb);
		/* now make the text node to use our own buffer from now on */
		enesim_renderer_text_span_buffer_get(r, &nb);
		egueb_dom_character_data_buffer_set(target, nb);

		/* set the private data */
		private_data = egueb_dom_string_new_with_static_string("_renderer");
		egueb_dom_node_user_data_set(target, private_data, r);
		egueb_dom_string_unref(private_data);

		/* mark it as a change */
		thiz->renderable_tree_changed = EINA_TRUE;
	}
	else if (type == EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
	{
		Egueb_Svg_Element_Text *thiz;
		Enesim_Text_Buffer *nb;

		thiz = EGUEB_SVG_ELEMENT_TEXT(n);
		/* mark it as a change */
		thiz->renderable_tree_changed = EINA_TRUE;
	}
not_text:
	egueb_dom_node_unref(target);
not_us:
	egueb_dom_node_unref(related);
}

static void _egueb_svg_element_text_node_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *related = NULL;
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node_Type type;

	egueb_dom_event_mutation_related_get(e, &related);
	if (related != n)
	{
		goto not_us;
	}
	egueb_dom_event_target_get(e, &target);
	egueb_dom_node_type_get(target, &type);
	if (type == EGUEB_DOM_NODE_TYPE_TEXT_NODE)
	{
		Egueb_Svg_Element_Text *thiz;
		Egueb_Dom_String *private_data;
		Enesim_Renderer *r;
		Enesim_Text_Buffer *nb;

		thiz = EGUEB_SVG_ELEMENT_TEXT(n);

		/* get the private data */
		private_data = egueb_dom_string_new_with_static_string("_renderer");
		r = egueb_dom_node_user_data_get(target, private_data);

		enesim_renderer_text_span_real_buffer_get(r, &nb);
		egueb_dom_character_data_buffer_set(target, nb);

		enesim_renderer_unref(r);
		egueb_dom_node_user_data_set(target, private_data, NULL);
		egueb_dom_string_unref(private_data);
		/* mark it as a change */
		thiz->renderable_tree_changed = EINA_TRUE;
	}
	else if (type == EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
	{
		Egueb_Svg_Element_Text *thiz;
		Enesim_Text_Buffer *nb;

		thiz = EGUEB_SVG_ELEMENT_TEXT(n);
		/* mark it as a change */
		thiz->renderable_tree_changed = EINA_TRUE;
	}
not_text:
	egueb_dom_node_unref(target);
not_us:
	egueb_dom_node_unref(related);
}

/*----------------------------------------------------------------------------*
 *                               Shape interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_text_generate_geometry(Egueb_Svg_Shape *s,
		Egueb_Svg_Element *relative, Egueb_Dom_Node *doc)
{
	Egueb_Svg_Element_Text *thiz;
	Egueb_Svg_Element *e;
	Egueb_Svg_Length x, y;
	Egueb_Svg_Font_Size font_size;
	double doc_font_size, gfont;

	thiz = EGUEB_SVG_ELEMENT_TEXT(s);
	egueb_dom_attr_final_get(thiz->x, &x);
	egueb_dom_attr_final_get(thiz->y, &y);

	/* calculate the real size */
	egueb_svg_document_font_size_get(doc, &doc_font_size);
	thiz->gx = egueb_svg_coord_final_get(&x, relative->viewbox.w, doc_font_size);
	thiz->gy = egueb_svg_coord_final_get(&y, relative->viewbox.h, doc_font_size);

	/* set the position */
	e = EGUEB_SVG_ELEMENT(s);
	/* FIXME the font size accumulates from its parent, so we need to do
	 * similar to the transformation
	 */
	thiz->gfont = e->final_font_size;

	/* iterate over the children and generate the position */
	/* reset our pen to */
	thiz->pen.x = thiz->gx;
	thiz->pen.y = thiz->gy;
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(s),
		_egueb_svg_element_text_children_process_cb,
		thiz);

	thiz->renderable_tree_changed = EINA_FALSE;

	return EINA_TRUE;
}

static void _egueb_svg_element_text_renderer_propagate(Egueb_Svg_Shape *s,
		Egueb_Svg_Painter *painter)
{
	Egueb_Svg_Element_Text *thiz;

	thiz = EGUEB_SVG_ELEMENT_TEXT(s);
	if (thiz->renderable_tree_changed)
	{
		enesim_renderer_compound_layer_clear(thiz->r);
	}
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(s),
		_egueb_svg_element_text_children_propagate_cb,
		thiz);
}
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_element_text_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Text *thiz;

	thiz = EGUEB_SVG_ELEMENT_TEXT(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_text_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
	Egueb_Svg_Element_Text *thiz;

	thiz = EGUEB_SVG_ELEMENT_TEXT(r);
#if 0
	enesim_rectangle_coords_from(bounds, thiz->gx, thiz->gy,
			thiz->gw, thiz->gh);
#endif
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_text_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_TEXT);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_text, Egueb_Svg_Element_Text, x);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_text, Egueb_Svg_Element_Text, y);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_SHAPE_DESCRIPTOR,
		Egueb_Svg_Element_Text, Egueb_Svg_Element_Text_Class,
		egueb_svg_element_text);

static void _egueb_svg_element_text_class_init(void *k)
{
	Egueb_Svg_Shape_Class *klass;
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_SHAPE_CLASS(k);
	klass->generate_geometry = _egueb_svg_element_text_generate_geometry;
	klass->renderer_propagate = _egueb_svg_element_text_renderer_propagate;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->bounds_get = _egueb_svg_element_text_bounds_get;
	r_klass->renderer_get = _egueb_svg_element_text_renderer_get;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_text_tag_name_get;
}

static void _egueb_svg_element_text_class_deinit(void *k)
{
}

static void _egueb_svg_element_text_instance_init(void *o)
{
	Egueb_Svg_Element_Text *thiz;
	Enesim_Renderer *r;
	Enesim_Text_Engine *e;

	thiz = EGUEB_SVG_ELEMENT_TEXT(o);

	/* Default values */
	thiz->r = enesim_renderer_compound_new();

	e = enesim_text_engine_default_get();
	thiz->font = enesim_text_font_new_description_from(e, "Sans:style=Regular", 16);
	enesim_text_engine_unref(e);

	/* create the properties */
	thiz->x = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_text, x);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_text, y);

	/* add the events */
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_element_text_node_inserted_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(EGUEB_DOM_NODE(o),
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_element_text_node_removed_cb,
			EINA_TRUE, thiz);
}

static void _egueb_svg_element_text_instance_deinit(void *o)
{
	Egueb_Svg_Element_Text *thiz;

	thiz = EGUEB_SVG_ELEMENT_TEXT(o);
	enesim_renderer_unref(thiz->r);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
EAPI void egueb_svg_element_text_pen_get(Egueb_Dom_Node *n,
		Egueb_Svg_Element_Text_Pen **pen)
{
	Egueb_Svg_Element_Text *thiz;

	thiz = EGUEB_SVG_ELEMENT_TEXT(n);
	*pen = &thiz->pen;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_text_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_text);
	return n;
}

EAPI Eina_Bool egueb_svg_element_is_text(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_TEXT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Eina_Error egueb_svg_element_text_x_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Length *x)
{
	Egueb_Svg_Element_Text *thiz;

	thiz = EGUEB_SVG_ELEMENT_TEXT(n);
	return egueb_dom_attr_set(thiz->x, EGUEB_DOM_ATTR_TYPE_BASE, x);
}

EAPI Eina_Error egueb_svg_element_text_y_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Length *y)
{
	Egueb_Svg_Element_Text *thiz;

	thiz = EGUEB_SVG_ELEMENT_TEXT(n);
	return egueb_dom_attr_set(thiz->y, EGUEB_DOM_ATTR_TYPE_BASE, y);
}
