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
#include "egueb_svg_element_tspan.h"
#include "egueb_svg_document.h"
#include "egueb_svg_attr_length_list_private.h"
#include "egueb_svg_text_content_private.h"
#include "egueb_svg_element_text_private.h"
/*
 * TODO
 * this element must have a list of text spans renderers too, given that
 * several attributes are of a list type, we need to createa text span
 * for each group of glyphs
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_TSPAN_DESCRIPTOR egueb_svg_element_tspan_descriptor_get()
#define EGUEB_SVG_ELEMENT_TSPAN_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Tspan_Class, EGUEB_SVG_ELEMENT_TSPAN_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_TSPAN(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Tspan, EGUEB_SVG_ELEMENT_TSPAN_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Tspan
{
	Egueb_Svg_Text_Content base;
	/* properties */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	Egueb_Dom_Node *dx;
	Egueb_Dom_Node *dy;
	/* private */
	double gx;
	double gy;
	Enesim_Text_Font *gfont;
	Enesim_Renderer *r;
} Egueb_Svg_Element_Tspan;

typedef struct _Egueb_Svg_Element_Tspan_Class
{
	Egueb_Svg_Text_Content_Class base;
} Egueb_Svg_Element_Tspan_Class;

/* from a node inserted or node removed event, get the tspan node in case
 * the node inserted/removed is of that type and the parent is ourselves
 */
static Eina_Bool _egueb_svg_element_tspan_mutation_get_tspan(Egueb_Dom_Event *e,
		Egueb_Dom_Node *us, Egueb_Dom_Node **tspan)
{
	Egueb_Dom_Node *target = NULL;
	Egueb_Dom_Node *related = NULL;
	Egueb_Dom_Node_Type type;
	Eina_Bool ret = EINA_FALSE;

	related = egueb_dom_event_mutation_related_get(e);
	if (related != us)
	{
		goto not_us;
	}

	target = EGUEB_DOM_NODE(egueb_dom_event_target_get(e));
	type = egueb_dom_node_type_get(target);
	if (type != EGUEB_DOM_NODE_TYPE_TEXT)
	{
		goto not_tspan;
	}

	*tspan = egueb_dom_node_ref(target);
	ret = EINA_TRUE;
not_tspan:
	egueb_dom_node_unref(target);
not_us:
	egueb_dom_node_unref(related);
	return ret;
}

static void _egueb_svg_element_tspan_node_inserted_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *tspan = NULL;

	if (_egueb_svg_element_tspan_mutation_get_tspan(e, n, &tspan))
	{
		Egueb_Svg_Element_Tspan *thiz;
		Enesim_Text_Buffer *nb = NULL;

		thiz = EGUEB_SVG_ELEMENT_TSPAN(n);
		/* set the internal buffer of the tspan span to be the one
		 * on the tspan node */
		nb = egueb_dom_character_data_buffer_get(tspan);
		enesim_renderer_text_span_real_buffer_set(thiz->r, nb);
		/* now make the tspan node to use our own buffer from now on */
		nb = enesim_renderer_text_span_buffer_get(thiz->r);
		egueb_dom_character_data_buffer_set(tspan, nb);
		egueb_dom_node_unref(tspan);
	}
}

static void _egueb_svg_element_tspan_node_removed_cb(Egueb_Dom_Event *e,
		void *data)
{
	Egueb_Dom_Node *n = data;
	Egueb_Dom_Node *tspan = NULL;

	if (_egueb_svg_element_tspan_mutation_get_tspan(e, n, &tspan))
	{
		Egueb_Svg_Element_Tspan *thiz;
		Enesim_Text_Buffer *nb;

		thiz = EGUEB_SVG_ELEMENT_TSPAN(n);
		nb = enesim_renderer_text_span_real_buffer_get(thiz->r);
		egueb_dom_character_data_buffer_set(tspan, nb);
		egueb_dom_node_unref(tspan);
	}
}

static Eina_Bool _egueb_svg_element_tspan_pen_get(Egueb_Dom_Node *n,
		Egueb_Svg_Element_Text_Pen **pen)
{
	Egueb_Dom_Node *relative;

	relative = egueb_svg_element_geometry_relative_get(n);
	if (!egueb_svg_element_text_pen_get(relative, pen))
	{
		Egueb_Dom_String *name;
		Egueb_Dom_String *tname;

		name = egueb_dom_element_name_get(n);
		tname = egueb_dom_element_tag_name_get(n);
		WARN("Relative element '%s' is not a text, is a '%s'",
				egueb_dom_string_string_get(name),
				egueb_dom_string_string_get(tname));
		egueb_dom_string_unref(name);
		egueb_dom_string_unref(tname);

		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                            Text content interface                          *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                            Renderable interface                            *
 *----------------------------------------------------------------------------*/

static void _egueb_svg_element_tspan_painter_apply(Egueb_Svg_Renderable *r,
		Egueb_Svg_Painter *painter)
{
	Egueb_Svg_Element_Tspan *thiz;
	Enesim_Color color;
	Enesim_Color fill_color;
	Eina_Bool visibility;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(r);

	egueb_svg_painter_visibility_get(painter, &visibility);
	egueb_svg_painter_color_get(painter, &color);
	egueb_svg_painter_fill_color_get(painter, &fill_color);

	enesim_renderer_color_set(thiz->r, fill_color);
	enesim_renderer_visibility_set(thiz->r, visibility);
}

static Eina_Bool _egueb_svg_element_tspan_process(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Tspan *thiz;
	Egueb_Svg_Element_Text_Pen *pen = NULL;
	Egueb_Svg_Element *e;
	Egueb_Svg_Length *c;
	Egueb_Dom_List *l;
	Egueb_Dom_Node *n;
	Egueb_Dom_Node *relative;
	Egueb_Dom_Node *doc;
	Enesim_Text_Font *font;
	Enesim_Rectangle bounds;
	double gx, gy;
	double doc_font_size, gfont_size;
	int max;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(r);

	/* calculate the real size */
	doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(r));
	doc_font_size = egueb_svg_document_font_size_get(doc);
	egueb_dom_node_unref(doc);

	/* calculate the final attributes */
	relative = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(r));
	egueb_dom_attr_final_get(thiz->x, &l);
	c = egueb_dom_list_item_get(l, 0);
	thiz->gx = egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.w, doc_font_size);
	egueb_dom_list_unref(l);

	egueb_dom_attr_final_get(thiz->y, &l);
	c = egueb_dom_list_item_get(l, 0);
	thiz->gy = egueb_svg_coord_final_get(c, (EGUEB_SVG_ELEMENT(relative))->viewbox.h, doc_font_size);
	egueb_dom_node_unref(relative);

	/* set the position */
	e = EGUEB_SVG_ELEMENT(r);
	gfont_size = e->final_font_size;

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

	enesim_renderer_text_span_font_set(thiz->r, enesim_text_font_ref(thiz->gfont));
	max = enesim_text_font_max_ascent_get(thiz->gfont);

	/* get the pen we should use for every span */
	if (_egueb_svg_element_tspan_pen_get(EGUEB_DOM_NODE(relative), &pen))
	{
		gx = pen->x;
		gy = pen->y;
	}
	else
	{
		gx = 0;
		gy = 0;
	}
	enesim_renderer_origin_set(thiz->r, gx, gy - max);

	INFO("matrix %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS (&e->transform));
	enesim_renderer_transformation_set(thiz->r, &e->transform);

	INFO("x: %g, y: %g, font-size: %g", gx, gy, gfont_size);
	enesim_renderer_shape_geometry_get(thiz->r, &bounds);
	if (pen)
	{
		INFO("advancing by w: %g, h: %g", bounds.w, bounds.h);
		pen->x += bounds.w;
	}

	return EINA_TRUE;
}

static Enesim_Renderer * _egueb_svg_element_tspan_renderer_get(
		Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(r);
	return enesim_renderer_ref(thiz->r);
}

static void _egueb_svg_element_tspan_bounds_get(Egueb_Svg_Renderable *r,
		Enesim_Rectangle *bounds)
{
#if 0
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(r);
	enesim_rectangle_coords_from(bounds, thiz->gx, thiz->gy,
			thiz->gw, thiz->gh);
#endif
}

/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_tspan_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_TSPAN);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_TEXT_CONTENT_DESCRIPTOR,
		Egueb_Svg_Element_Tspan, Egueb_Svg_Element_Tspan_Class,
		egueb_svg_element_tspan);

static void _egueb_svg_element_tspan_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *r_klass;
	Egueb_Dom_Element_Class *e_klass;

	r_klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	r_klass->bounds_get = _egueb_svg_element_tspan_bounds_get;
	r_klass->renderer_get = _egueb_svg_element_tspan_renderer_get;
	r_klass->painter_apply = _egueb_svg_element_tspan_painter_apply;
	r_klass->process = _egueb_svg_element_tspan_process;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_tspan_tag_name_get;
}

static void _egueb_svg_element_tspan_instance_init(void *o)
{
	Egueb_Svg_Element_Tspan *thiz;
	Egueb_Svg_Length *zero;
	Egueb_Dom_Node *n;
	Egueb_Dom_List *def;
	Egueb_Dom_Event_Target *evt;
	Enesim_Renderer *r;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(o);

	r = enesim_renderer_text_span_new();
	enesim_renderer_color_set(r, 0xff000000);
	thiz->r = r;

	/* Default values */

	/* create the properties */
	/* create a default list with 0 as the value of the coordinate */
	def = egueb_dom_list_new(egueb_svg_length_descriptor_get());
	zero = calloc(1, sizeof(Egueb_Svg_Length));
	*zero = EGUEB_SVG_LENGTH_0;
	egueb_dom_list_item_append(def, zero);
	thiz->x = egueb_svg_attr_length_list_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_X),
			egueb_dom_list_ref(def), EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_list_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_Y),
			egueb_dom_list_ref(def), EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->dx = egueb_svg_attr_length_list_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_DX),
			egueb_dom_list_ref(def), EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->dy = egueb_svg_attr_length_list_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_DY),
			egueb_dom_list_ref(def), EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	egueb_dom_list_unref(def);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->x), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->y), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->dx), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->dy), NULL);

	/* add the events */
	evt = EGUEB_DOM_EVENT_TARGET_CAST(o);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_element_tspan_node_inserted_cb,
			EINA_TRUE, thiz);
	egueb_dom_event_target_event_listener_add(evt,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_element_tspan_node_removed_cb,
			EINA_TRUE, thiz);
}

static void _egueb_svg_element_tspan_instance_deinit(void *o)
{
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(o);
	enesim_text_font_unref(thiz->gfont);
	enesim_renderer_unref(thiz->r);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
	egueb_dom_node_unref(thiz->dx);
	egueb_dom_node_unref(thiz->dy);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_is_element_tspan(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_TSPAN_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Egueb_Dom_Node * egueb_svg_element_tspan_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_tspan);
	return n;
}

EAPI void egueb_svg_element_tspan_x_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Length *x)
{
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(n);
	egueb_dom_attr_set(thiz->x, EGUEB_DOM_ATTR_TYPE_BASE, x);
}

EAPI void egueb_svg_element_tspan_y_set(Egueb_Dom_Node *n,
		const Egueb_Svg_Length *y)
{
	Egueb_Svg_Element_Tspan *thiz;

	thiz = EGUEB_SVG_ELEMENT_TSPAN(n);
	egueb_dom_attr_set(thiz->y, EGUEB_DOM_ATTR_TYPE_BASE, y);
}

