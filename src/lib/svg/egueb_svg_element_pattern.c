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
#include "egueb_svg_main.h"
#include "egueb_svg_matrix.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_attr_referenceable_units.h"
#include "egueb_svg_attr_matrix.h"
#include "egueb_svg_attr_string.h"
#include "egueb_svg_element_pattern.h"

#include "egueb_svg_paint_server_private.h" 
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_PATTERN_DESCRIPTOR egueb_svg_element_pattern_descriptor_get()
#define EGUEB_SVG_ELEMENT_PATTERN_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Svg_Element_Pattern_Class, 					\
		EGUEB_SVG_ELEMENT_PATTERN_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_PATTERN_CLASS_GET(o) EGUEB_SVG_ELEMENT_PATTERN_CLASS(	\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_SVG_ELEMENT_PATTERN(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Svg_Element_Pattern, EGUEB_SVG_ELEMENT_PATTERN_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Pattern
{
	Egueb_Svg_Paint_Server base;
	/* properties */
	Egueb_Dom_Node *units;
	Egueb_Dom_Node *transform;
	Egueb_Dom_Node *xlink_href;
	Egueb_Dom_Node *x;
	Egueb_Dom_Node *y;
	Egueb_Dom_Node *width;
	Egueb_Dom_Node *height;
	/* private */
	Enesim_Renderer *content;
	Enesim_Renderer *r;
#if 0
	Eina_Bool units_set : 1;
	Eina_Bool transform_is_set : 1;
#endif
} Egueb_Svg_Element_Pattern;

typedef struct _Egueb_Svg_Element_Pattern_Class
{
	Egueb_Svg_Paint_Server_Class base;
} Egueb_Svg_Element_Pattern_Class;
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
#if 0
static Eina_Bool _pattern_setup(Enesim_Renderer *r,
		const Egueb_Svg_Element_Context *state,
		Enesim_Renderer *rel)
{
	Egueb_Svg_Pattern *thiz;
	Egueb_Svg_Pattern_Units pu;
	Enesim_Matrix m;
	double x;
	double y;
	double w;
	double h;

	thiz = _egueb_svg_pattern_get(r);
	pu = thiz->units;
	if (pu == ESVG_OBJECT_BOUNDING_BOX)
	{
		Eina_Rectangle bbox;

		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		x = egueb_svg_coord_final_get(&thiz->x, 1);
		y = egueb_svg_coord_final_get(&thiz->y, 1);
		w = egueb_svg_coord_final_get(&thiz->width, 1);
		h = egueb_svg_coord_final_get(&thiz->height, 1);

		enesim_renderer_destination_bounds(rel, &bbox, 0, 0);
		enesim_matrix_values_set(&m, bbox.w, 0, bbox.x, 0, bbox.h, bbox.y, 0, 0, 1);
	}
	else
	{
		double vw;
		double vh;

		/* use the user space coordiantes */
		vw = state->viewbox_w;
		vh = state->viewbox_h;
		x = egueb_svg_coord_final_get(&thiz->x, vw);
		y = egueb_svg_coord_final_get(&thiz->y, vh);
		w = egueb_svg_coord_final_get(&thiz->width, vw);
		h = egueb_svg_coord_final_get(&thiz->height, vh);

		m = state->transform;
	}

	/* set the properties */
	enesim_renderer_pattern_x_set(thiz->r, x);
	enesim_renderer_pattern_y_set(thiz->r, y);
	enesim_renderer_pattern_width_set(thiz->r, w);
	enesim_renderer_pattern_height_set(thiz->r, h);

	printf("pattern setup %g %g %g %g\n", x, y, w, h);
	if (enesim_matrix_type_get(&thiz->transform) != ENESIM_MATRIX_IDENTITY)
	{
		enesim_matrix_compose(&m, &thiz->transform, &m);
	}
	enesim_renderer_transformation_set(thiz->r, &m);
	enesim_renderer_pattern_source_set(thiz->r, thiz->content);
	printf("ok, the content set %p\n", thiz->content);

	/* TODO we need to set the new viewbox */
	/* 1. setup the content */
	/* 2. get the content renderer */
	/* 3. assign it */
#if 0
	{
		Egueb_Svg_Element_Context new_state;

		memset(&new_state, 0, sizeof(Egueb_Svg_Element_Context));
		new_state.viewbox_w = w;
		new_state.viewbox_h = h;
		new_state.transform = m;

		egueb_svg_element_setup(thiz->content, estate, attr, s, error);
		enesim_renderer_pattern_source_set(thiz->r, thiz->content);
	}
#endif

	return EINA_TRUE;
}
#endif
/*----------------------------------------------------------------------------*
 *                           Referenceable interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_pattern_process(
		Egueb_Svg_Referenceable *r)
{
	DBG("Processing a element_pattern");
	/* iterate over the children and call the process there */
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_pattern_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_RECT);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_pattern, Egueb_Svg_Element_Pattern, transform);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_pattern, Egueb_Svg_Element_Pattern, units);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_pattern, Egueb_Svg_Element_Pattern, xlink_href);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_pattern, Egueb_Svg_Element_Pattern, x);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_pattern, Egueb_Svg_Element_Pattern, y);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_pattern, Egueb_Svg_Element_Pattern, width);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_pattern, Egueb_Svg_Element_Pattern, height);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_PAINT_SERVER_DESCRIPTOR,
		Egueb_Svg_Element_Pattern, Egueb_Svg_Element_Pattern_Class,
		egueb_svg_element_pattern);

static void _egueb_svg_element_pattern_class_init(void *k)
{
	Egueb_Svg_Referenceable_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_REFERENCEABLE_CLASS(k);
	klass->process = _egueb_svg_element_pattern_process;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_pattern_tag_name_get;
}

static void _egueb_svg_element_pattern_class_deinit(void *k)
{
}

static void _egueb_svg_element_pattern_instance_init(void *o)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(o);
	/* create the properties */
	thiz->units = egueb_svg_attr_referenceable_units_new(
			egueb_dom_string_ref(EGUEB_SVG_ELEMENT_PATTERN_UNITS),
			EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	thiz->transform = egueb_svg_attr_matrix_new(
			egueb_dom_string_ref(EGUEB_SVG_ELEMENT_PATTERN_TRANSFORM),
			&EGUEB_SVG_MATRIX_IDENTITY, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_XLINK_HREF),
			NULL, EINA_TRUE, EINA_FALSE, EINA_FALSE);

	thiz->x = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->width = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_WIDTH),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);
	thiz->height = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_HEIGHT),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_pattern, transform);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_pattern, units);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_pattern, xlink_href);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_pattern, x);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_pattern, y);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_pattern, width);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_pattern, height);
}

static void _egueb_svg_element_pattern_instance_deinit(void *o)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(o);
	/* the properties */
	egueb_dom_node_unref(thiz->units);
	egueb_dom_node_unref(thiz->transform);
	egueb_dom_node_unref(thiz->xlink_href);
	egueb_dom_node_unref(thiz->x);
	egueb_dom_node_unref(thiz->y);
	egueb_dom_node_unref(thiz->width);
	egueb_dom_node_unref(thiz->height);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_pattern_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_pattern);
	return n;
}
#if 0
EAPI Enesim_Renderer * egueb_svg_pattern_new(void)
{
	Egueb_Svg_Pattern *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Egueb_Svg_Pattern));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_PATTERN_MAGIC);
	r = enesim_renderer_pattern_new();
	thiz->r = r;

	/* Default values */
	thiz->units = ESVG_OBJECT_BOUNDING_BOX;
	enesim_matrix_identity(&thiz->transform);

	r = egueb_svg_paint_server_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool egueb_svg_is_pattern(Enesim_Renderer *r)
{
	Egueb_Svg_Pattern *thiz;
	Eina_Bool ret;

	if (!egueb_svg_is_paint_server(r))
		return EINA_FALSE;
	thiz = egueb_svg_paint_server_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_PATTERN_MAGIC);

	return ret;
}

EAPI void egueb_svg_pattern_x_set(Enesim_Renderer *r, const Egueb_Svg_Coord *x)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (x) thiz->x = *x;
}

EAPI void egueb_svg_pattern_x_get(Enesim_Renderer *r, Egueb_Svg_Coord *x)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (x) *x = thiz->x;
}

EAPI void egueb_svg_pattern_y_set(Enesim_Renderer *r, const Egueb_Svg_Coord *y)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (y) thiz->y = *y;
}

EAPI void egueb_svg_pattern_y_get(Enesim_Renderer *r, Egueb_Svg_Coord *y)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (y) *y = thiz->y;
}

EAPI void egueb_svg_pattern_width_set(Enesim_Renderer *r, const Egueb_Svg_Length *width)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (width) thiz->width = *width;
}

EAPI void egueb_svg_pattern_width_get(Enesim_Renderer *r, Egueb_Svg_Length *width)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (width) *width = thiz->width;
}

EAPI void egueb_svg_pattern_height_set(Enesim_Renderer *r, const Egueb_Svg_Length *height)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (height) thiz->height = *height;
}

EAPI void egueb_svg_pattern_height_get(Enesim_Renderer *r, Egueb_Svg_Length *height)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (height) *height = thiz->height;
}

EAPI void egueb_svg_pattern_units_set(Enesim_Renderer *r, Egueb_Svg_Pattern_Units units)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	thiz->units = units;
	thiz->units_set = EINA_TRUE;
}

EAPI void egueb_svg_pattern_units_get(Enesim_Renderer *r, Egueb_Svg_Pattern_Units *units)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (units) *units = thiz->units;
}

EAPI Eina_Bool egueb_svg_pattern_units_is_set(Enesim_Renderer *r)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	return thiz->units_set;
}

EAPI void egueb_svg_pattern_transform_set(Enesim_Renderer *r, const Enesim_Matrix *transform)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (transform) thiz->transform = *transform;
}

EAPI void egueb_svg_pattern_transform_get(Enesim_Renderer *r, Enesim_Matrix *transform)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (transform) *transform = thiz->transform;
}

EAPI Eina_Bool egueb_svg_pattern_transform_is_set(Enesim_Renderer *r)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	return thiz->transform_is_set;
}

EAPI void egueb_svg_pattern_content_set(Enesim_Renderer *r, Enesim_Renderer *content)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	/* FIXME for now */
	thiz->content = content;
	egueb_svg_element_parent_set(content, r);
}
#endif
