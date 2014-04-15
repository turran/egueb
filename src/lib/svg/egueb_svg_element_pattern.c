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
#include "egueb_svg_attr_xlink_href.h"
#include "egueb_svg_element_pattern.h"

#include "egueb_svg_paint_server_private.h" 
#include "egueb_svg_reference_paint_server_private.h"
#include "egueb_svg_attr_matrix_private.h"
#include "egueb_svg_attr_referenceable_units_private.h"
/* A pattern from the implementation point of view is a mix between a
 * gradient and a clipPath. We need to have live clones of the children
 * on every reference like a clipPath but in case the pattern element
 * does not have any child, the xlink:href attribute is used to get them
 * like the gradient
 */
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

#define EGUEB_SVG_ELEMENT_PATTERN_DEEP_GET(n, attr, def, fun)			\
	if (egueb_svg_element_is_pattern(n))					\
	{									\
		Egueb_Svg_Element_Pattern *thiz;				\
		thiz = EGUEB_SVG_ELEMENT_PATTERN(n);				\
		if (!egueb_dom_attr_is_set(thiz->attr))				\
		{								\
			Egueb_Dom_Node *href = NULL;				\
			_egueb_svg_element_pattern_xlink_href_node_get(		\
					n, &href);				\
			if (href)						\
			{							\
				fun(href, attr);				\
				egueb_dom_node_unref(href);			\
			}							\
			else							\
			{							\
				*attr = def;					\
			}							\
		}								\
		else								\
		{								\
			egueb_dom_attr_final_get(thiz->attr, attr);		\
		}								\
	}									\
	else									\
	{									\
		*attr = def;							\
	}

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
} Egueb_Svg_Element_Pattern;

typedef struct _Egueb_Svg_Element_Pattern_Class
{
	Egueb_Svg_Paint_Server_Class base;
} Egueb_Svg_Element_Pattern_Class;

static void _egueb_svg_element_pattern_xlink_href_node_get(Egueb_Dom_Node *n, Egueb_Dom_Node **href)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	egueb_svg_attr_xlink_href_node_get(thiz->xlink_href, href);
}
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

		enesim_renderer_destination_bounds_get(rel, &bbox, 0, 0);
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

static Egueb_Svg_Reference * _egueb_svg_element_pattern_reference_new(
		Egueb_Svg_Referenceable *r, Egueb_Dom_Node *referencer)
{
	Egueb_Svg_Reference *ref;

	ref = egueb_svg_reference_pattern_new();
	return ref;
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_pattern_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_PATTERN);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_PAINT_SERVER_DESCRIPTOR,
		Egueb_Svg_Element_Pattern, Egueb_Svg_Element_Pattern_Class,
		egueb_svg_element_pattern);

static void _egueb_svg_element_pattern_class_init(void *k)
{
	Egueb_Svg_Referenceable_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_REFERENCEABLE_CLASS(k);
	klass->process = _egueb_svg_element_pattern_process;
	klass->reference_new = _egueb_svg_element_pattern_reference_new;

	e_klass= EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_pattern_tag_name_get;
}

static void _egueb_svg_element_pattern_instance_init(void *o)
{
	Egueb_Svg_Element_Pattern *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(o);
	/* create the properties */
	thiz->units = egueb_svg_attr_referenceable_units_new(
			egueb_dom_string_ref(EGUEB_SVG_PATTERN_UNITS),
			EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	thiz->transform = egueb_svg_attr_matrix_new(
			egueb_dom_string_ref(EGUEB_SVG_PATTERN_TRANSFORM),
			&EGUEB_SVG_MATRIX_IDENTITY, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_xlink_href_new(
			egueb_dom_string_ref(EGUEB_DOM_XLINK_HREF),
			NULL);

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

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->transform), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->units), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->xlink_href), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->x), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->y), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->width), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->height), NULL);
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
EAPI Eina_Bool egueb_svg_element_is_pattern(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_PATTERN_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI Egueb_Dom_Node * egueb_svg_element_pattern_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_pattern);
	return n;
}

EAPI void egueb_svg_element_pattern_deep_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units *units)
{
	EGUEB_SVG_ELEMENT_PATTERN_DEEP_GET(n, units,
			EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE,
			egueb_svg_element_pattern_deep_units_get);
}

EAPI void egueb_svg_element_pattern_deep_transform_get(Egueb_Dom_Node *n,
		Enesim_Matrix *transform)
{
	Enesim_Matrix m;

	enesim_matrix_identity(&m);
	EGUEB_SVG_ELEMENT_PATTERN_DEEP_GET(n, transform, m,
			egueb_svg_element_pattern_deep_transform_get);
}

#if 0
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
