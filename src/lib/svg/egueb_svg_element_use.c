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
#include "egueb_svg_string.h"
#include "egueb_svg_element.h"
#include "egueb_svg_element_g.h"
#include "egueb_svg_element_use.h"
#include "egueb_svg_document.h"
#include "egueb_svg_renderable.h"
#include "egueb_svg_renderable_container.h"
#include "egueb_svg_shape.h"
#include "egueb_svg_painter.h"
#include "egueb_svg_painter.h"

#include "egueb_svg_element_use_private.h"
#include "egueb_svg_painter_private.h"
#include "egueb_svg_renderable_private.h"
#include "egueb_svg_shape_private.h"
#include "egueb_svg_attr_string_private.h"
/*
 * The 'use' element should be able to create a new instance from another
 * svg tree. Basically we set the 'link' property of the 'use' to the svg
 * tree. Once it is attached on the setup we should clone it if it is not
 * already cloned and render it that new tree as a 'g' element.
 * Given that generated g does not have a parent (but it does have a topmost)
 * we need to propagate every attribute on the use to the g
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_USE_DESCRIPTOR egueb_svg_element_use_descriptor_get()
#define EGUEB_SVG_ELEMENT_USE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Use_Class, EGUEB_SVG_ELEMENT_USE_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_USE(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Use, EGUEB_SVG_ELEMENT_USE_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Use
{
	Egueb_Svg_Renderable base;
	/* properties */
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	Egueb_Dom_Node *width;
	Egueb_Dom_Node *height;
	Egueb_Dom_Node *xlink_href;
	/* private */
	double gx, gy, gw, gh;

	Egueb_Dom_Node *g;
	Egueb_Dom_Node *clone;

	Eina_Bool document_changed;
	Egueb_Dom_String *last_xlink;
} Egueb_Svg_Element_Use;

typedef struct _Egueb_Svg_Element_Use_Class
{
	Egueb_Svg_Renderable_Class base;
} Egueb_Svg_Element_Use_Class;

/*----------------------------------------------------------------------------*
 *                               Event monitors                               *
 *----------------------------------------------------------------------------*/
static void _egueb_dom_element_use_g_node_monitor_cb(Egueb_Dom_Event *ev,
		void *data)
{
	Egueb_Svg_Element_Use *thiz = data;

	/* whenever we receive an event, just propagate it */
	egueb_dom_node_event_propagate(EGUEB_DOM_NODE(thiz), ev);
}

static void _egueb_svg_element_use_setup_cloned(Egueb_Svg_Element_Use *thiz,
		Egueb_Dom_Node *cloned)
{
	Egueb_Dom_Node *doc;
	Enesim_Matrix m;

	doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(thiz));
	if (doc)
	{
		thiz->g = egueb_dom_document_node_adopt(doc, thiz->g, NULL);
		egueb_dom_node_unref(doc);
	}
	thiz->clone = cloned;
	/* add the cloned to the group */
	egueb_dom_node_child_append(thiz->g, egueb_dom_node_ref(thiz->clone), NULL);
	/* transform the group */
	enesim_matrix_translate(&m, thiz->gx, thiz->gy);
	egueb_svg_renderable_transform_set(thiz->g, &m);
}

static void _egueb_svg_element_use_cleanup_cloned(Egueb_Svg_Element_Use *thiz)
{
	/* remove the clone, this will remove the renderer automatically */
	egueb_dom_node_child_remove(thiz->g, thiz->clone, NULL);
	egueb_dom_node_unref(thiz->clone);
	thiz->clone = NULL;
}

/* Whenever the node has been inserted/removed into/from the document we need
 * to make sure that the <g> node also has the same document
 */
static void _egueb_dom_element_use_insterted_into_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Svg_Element_Use *thiz = EGUEB_SVG_ELEMENT_USE(data);
	Egueb_Dom_Event_Phase phase;

	phase = egueb_dom_event_phase_get(ev);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	thiz->document_changed = EINA_TRUE;
}

static void _egueb_dom_element_use_removed_from_document_cb(
		Egueb_Dom_Event *ev, void *data)
{
	Egueb_Svg_Element_Use *thiz = EGUEB_SVG_ELEMENT_USE(data);
	Egueb_Dom_Event_Phase phase;

	phase = egueb_dom_event_phase_get(ev);
	if (phase != EGUEB_DOM_EVENT_PHASE_AT_TARGET)
		return;

	thiz->document_changed = EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Renderable interface                           *
 *----------------------------------------------------------------------------*/
static Enesim_Renderer * _egueb_svg_element_use_renderer_get(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(r);
	return egueb_svg_renderable_renderer_get(thiz->g);
}

static Eina_Bool _egueb_svg_element_use_process(Egueb_Svg_Renderable *r)
{
	Egueb_Svg_Element_Use *thiz;
	Egueb_Svg_Element *e_parent;
	Egueb_Svg_Element *e;
	Egueb_Svg_Length x, y, w, h;
	Egueb_Dom_String *xlink = NULL;
	Egueb_Dom_Node *relative, *doc;
	double font_size;

	/* first set the geometry */
	thiz = EGUEB_SVG_ELEMENT_USE(r);
	egueb_dom_attr_final_get(thiz->x, &x);
	egueb_dom_attr_final_get(thiz->y, &y);
	egueb_dom_attr_final_get(thiz->width, &w);
	egueb_dom_attr_final_get(thiz->height, &h);
	egueb_dom_attr_final_get(thiz->xlink_href, &xlink);

	/* calculate the real size */
	relative = egueb_svg_element_geometry_relative_get(EGUEB_DOM_NODE(r));
	if (!relative)
	{
		WARN("No relative available");
		return EINA_FALSE;
	}
	doc = egueb_dom_node_owner_document_get(EGUEB_DOM_NODE(r));
	if (!doc)
	{
		WARN("No document set");
		egueb_dom_node_unref(relative);
		return EINA_FALSE;
	}

	e_parent = EGUEB_SVG_ELEMENT(relative);
	font_size = egueb_svg_document_font_size_get(doc);

	thiz->gx = egueb_svg_coord_final_get(&x, e_parent->viewbox.w, font_size);
	thiz->gy = egueb_svg_coord_final_get(&y, e_parent->viewbox.h, font_size);
	thiz->gw = egueb_svg_coord_final_get(&w, e_parent->viewbox.w, font_size);
	thiz->gh = egueb_svg_coord_final_get(&h, e_parent->viewbox.h, font_size);

	egueb_dom_node_unref(relative);

	/* update the viewbox */
	e = EGUEB_SVG_ELEMENT(r);
	enesim_rectangle_coords_from(&e->viewbox, thiz->gx, thiz->gy, thiz->gw, thiz->gh);

	DBG("x: %g, y: %g, w: %g, h: %g", thiz->gx, thiz->gy, thiz->gw, thiz->gh);

	/* in case the xlink attribute has changed, remove the clone
	 * and create a new one.
	 */
	if (thiz->document_changed || !egueb_dom_string_is_equal(xlink, thiz->last_xlink))
	{
		Egueb_Dom_Node *cloned;
		if (thiz->clone)
		{
			_egueb_svg_element_use_cleanup_cloned(thiz);
		}

		/* finally clone the reference */
		cloned = egueb_svg_document_iri_clone(doc, xlink, NULL);
		if (cloned)
		{
			_egueb_svg_element_use_setup_cloned(thiz, cloned);
		}
		thiz->document_changed = EINA_FALSE;

		/* finally swap */
		if (thiz->last_xlink)
		{
			egueb_dom_string_unref(thiz->last_xlink);
			thiz->last_xlink = NULL;
		}
		thiz->last_xlink = egueb_dom_string_dup(xlink);
	}
	egueb_dom_node_unref(doc);
	egueb_dom_string_unref(xlink);

	return egueb_dom_element_process(thiz->g);
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_use_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_USE);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_RENDERABLE_DESCRIPTOR,
		Egueb_Svg_Element_Use, Egueb_Svg_Element_Use_Class,
		egueb_svg_element_use);

static void _egueb_svg_element_use_class_init(void *k)
{
	Egueb_Svg_Renderable_Class *klass;
	Egueb_Dom_Element_Class *edom_klass;

	klass = EGUEB_SVG_RENDERABLE_CLASS(k);
	klass->renderer_get = _egueb_svg_element_use_renderer_get;
	klass->process = _egueb_svg_element_use_process;

	edom_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	edom_klass->tag_name_get = _egueb_svg_element_use_tag_name_get;
}

static void _egueb_svg_element_use_instance_init(void *o)
{
	Egueb_Svg_Element_Use *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_ELEMENT_USE(o);

	thiz->g = egueb_svg_element_g_new();
	/* set the relativeness of the node */
	egueb_svg_element_geometry_relative_set(thiz->g, EGUEB_DOM_NODE(o), NULL);
	egueb_svg_element_presentation_relative_set(thiz->g, EGUEB_DOM_NODE(o), NULL);
	/* add the events that we need to propagate upstream */ 
	egueb_dom_node_event_monitor_add(thiz->g,
			_egueb_dom_element_use_g_node_monitor_cb, thiz);

	/* create the properties */
	thiz->x = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_X),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_Y),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->width = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_WIDTH),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->height = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_HEIGHT),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_DOM_NAME_XLINK_HREF),
			NULL);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->x), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->y), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->width), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->height), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->xlink_href), NULL);

	/* whenever the use element is inserted into a document, be sure
	 * to set the document on our g too
	 */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED_INTO_DOCUMENT,
			_egueb_dom_element_use_insterted_into_document_cb,
			EINA_TRUE, thiz);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED_FROM_DOCUMENT,
			_egueb_dom_element_use_removed_from_document_cb,
			EINA_TRUE, thiz);
}

static void _egueb_svg_element_use_instance_deinit(void *o)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(o);

	egueb_dom_node_unref(thiz->g);

	if (thiz->last_xlink)
	{
		egueb_dom_string_unref(thiz->last_xlink);
		thiz->last_xlink = NULL;
	}

	/* destroy the properties */
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
	egueb_dom_node_unref(thiz->width);
	egueb_dom_node_unref(thiz->height);
	egueb_dom_node_unref(thiz->xlink_href);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Node * egueb_svg_element_use_g_get(Egueb_Dom_Node *n)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	return egueb_dom_node_ref(thiz->g);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_use_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_use);
	return n;
}

EAPI Eina_Bool egueb_svg_element_is_use(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_USE_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

/**
 * Sets the x coordinate of an use element
 * @param[in] n The use element to set the x coordinate @ender_transfer{none}
 * @param[in] x The x coordinate to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_use_x_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *x)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	egueb_dom_attr_set(thiz->x, EGUEB_DOM_ATTR_TYPE_BASE, x);
}

/**
 * Gets the x coordinate of an use element
 * @ender_prop{x}
 * @param[in] n The use element to get the x coordinate @ender_transfer{none}
 * @param[out] x The pointer to store the x coordinate @ender_transfer{content}
 */
EAPI void egueb_svg_element_use_x_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *x)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->x, x);
}

/**
 * Sets the y coordinate of an use element
 * @param[in] n The use element to set the y coordinate @ender_transfer{none}
 * @param[in] y The y coordinate to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_use_y_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Coord *y)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	egueb_dom_attr_set(thiz->y, EGUEB_DOM_ATTR_TYPE_BASE, y);
}

/**
 * Gets the y coordinate of an use element
 * @ender_prop{y}
 * @param[in] n The use element to get the y coordinate @ender_transfer{none}
 * @param[out] y The pointer to store the y coordinate @ender_transfer{content}
 */
EAPI void egueb_svg_element_use_y_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *y)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->y, y);
}

/**
 * Sets the width of an use element
 * @param[in] n The use element to set the width @ender_transfer{none}
 * @param[in] width The width to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_use_width_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *width)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	egueb_dom_attr_set(thiz->width, EGUEB_DOM_ATTR_TYPE_BASE, width);
}

/**
 * Gets the width of an use element
 * @ender_prop{width}
 * @param[in] n The use element to get the width @ender_transfer{none}
 * @param[out] width The pointer to store the width @ender_transfer{content}
 */
EAPI void egueb_svg_element_use_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *width)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->width, width);
}

/**
 * Sets the height of an use element
 * @param[in] n The use element to set the height @ender_transfer{none}
 * @param[in] height The height to set @ender_transfer{content}
 */
EAPI void egueb_svg_element_use_height_set_simple(Egueb_Dom_Node *n, const Egueb_Svg_Length *height)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	egueb_dom_attr_set(thiz->height, EGUEB_DOM_ATTR_TYPE_BASE, height);
}

/**
 * Gets the height of an use element
 * @ender_prop{height}
 * @param[in] n The use element to get the height @ender_transfer{none}
 * @param[out] height The pointer to store the height @ender_transfer{content}
 */
EAPI void egueb_svg_element_use_height_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *height)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->height, height);
}

/**
 * Sets the href of an use element
 * @param[in] n The use element to get the href @ender_transfer{none}
 * @param[out] v The href to set @ender_transfer{full}
 */
EAPI void egueb_svg_element_use_href_set_simple(Egueb_Dom_Node *n, Egueb_Dom_String *v)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	egueb_dom_attr_set(thiz->xlink_href, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

/**
 * Gets the href of an use element
 * @ender_prop{href}
 * @param[in] n The use element to get the href @ender_transfer{none}
 * @param[out] v The pointer to store the href @ender_transfer{content}
 */
EAPI void egueb_svg_element_use_href_get(Egueb_Dom_Node *n, Egueb_Svg_String_Animated *v)
{
	Egueb_Svg_Element_Use *thiz;

	thiz = EGUEB_SVG_ELEMENT_USE(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->xlink_href, v);
}
