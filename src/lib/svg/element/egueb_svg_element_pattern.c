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
#include "egueb_svg_length.h"
#include "egueb_svg_matrix.h"
#include "egueb_svg_string.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_element_pattern.h"

#include "egueb_svg_element_pattern_private.h"
#include "egueb_svg_paint_server_private.h" 
#include "egueb_svg_reference_paint_server_private.h"
#include "egueb_svg_attr_matrix_private.h"
#include "egueb_svg_attr_referenceable_units_private.h"
#include "egueb_svg_attr_rect_private.h"
#include "egueb_svg_attr_xlink_href_private.h"
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
	Egueb_Dom_Node *viewbox;
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

static Eina_Bool _egueb_svg_pattern_children_process_cb(
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
 *                           Referenceable interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_pattern_process(
		Egueb_Svg_Referenceable *r)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(r);
	DBG("Processing a pattern");
	/* iterate over the children and call the process there */
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(r),
			_egueb_svg_pattern_children_process_cb,
			NULL);
	/* get the xlink:href node */
	if (!egueb_svg_attr_xlink_href_resolve(thiz->xlink_href))
		return EINA_FALSE;

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
			egueb_dom_string_ref(EGUEB_SVG_NAME_PATTERN_UNITS),
			EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	thiz->transform = egueb_svg_attr_matrix_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_PATTERN_TRANSFORM),
			&EGUEB_SVG_MATRIX_IDENTITY, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_xlink_href_new(
			egueb_dom_string_ref(EGUEB_DOM_NAME_XLINK_HREF),
			NULL);

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
	thiz->viewbox = egueb_svg_attr_rect_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_VIEWBOX),
			NULL);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->transform), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->units), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->xlink_href), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->x), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->y), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->width), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->height), NULL);
	egueb_dom_element_attribute_node_set(n, egueb_dom_node_ref(thiz->viewbox), NULL);
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
	egueb_dom_node_unref(thiz->viewbox);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_element_pattern_deep_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units *units)
{
	EGUEB_SVG_ELEMENT_PATTERN_DEEP_GET(n, units,
			EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX,
			egueb_svg_element_pattern_deep_units_get);
}

void egueb_svg_element_pattern_deep_transform_get(Egueb_Dom_Node *n,
		Enesim_Matrix *transform)
{
	Enesim_Matrix m;

	enesim_matrix_identity(&m);
	EGUEB_SVG_ELEMENT_PATTERN_DEEP_GET(n, transform, m,
			egueb_svg_element_pattern_deep_transform_get);
}

void egueb_svg_element_pattern_deep_x_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *x)
{
	EGUEB_SVG_ELEMENT_PATTERN_DEEP_GET(n, x,
		EGUEB_SVG_LENGTH_0, egueb_svg_element_pattern_deep_x_get);
}

void egueb_svg_element_pattern_deep_y_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *y)
{
	EGUEB_SVG_ELEMENT_PATTERN_DEEP_GET(n, y,
		EGUEB_SVG_LENGTH_0, egueb_svg_element_pattern_deep_y_get);
}

void egueb_svg_element_pattern_deep_width_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *width)
{
	EGUEB_SVG_ELEMENT_PATTERN_DEEP_GET(n, width,
		EGUEB_SVG_LENGTH_0, egueb_svg_element_pattern_deep_width_get);
}

void egueb_svg_element_pattern_deep_height_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *height)
{
	EGUEB_SVG_ELEMENT_PATTERN_DEEP_GET(n, height,
		EGUEB_SVG_LENGTH_0, egueb_svg_element_pattern_deep_height_get);
}

Eina_Bool egueb_svg_element_pattern_deep_viewbox_get(Egueb_Dom_Node *n,
		Egueb_Svg_Rect *vb)
{
	if (egueb_svg_element_is_pattern(n))
	{
		Egueb_Svg_Element_Pattern *thiz;
		thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
		if (!egueb_dom_attr_is_set(thiz->viewbox))
		{
			Egueb_Dom_Node *href = NULL;
			_egueb_svg_element_pattern_xlink_href_node_get(	
					n, &href);
			if (href)
			{
				Eina_Bool ret;
				ret = egueb_svg_element_pattern_deep_viewbox_get(href, vb);
				egueb_dom_node_unref(href);
				return ret;
			}
			else
			{
				return EINA_FALSE;
			}
		}
		else
		{
			egueb_dom_attr_final_get(thiz->viewbox, vb);
			return EINA_TRUE;
		}
	}
	else
	{
		return EINA_FALSE;
	}
}

Egueb_Dom_Node * egueb_svg_element_pattern_deep_children_get(Egueb_Dom_Node *n)
{
	if (egueb_svg_element_is_pattern(n))
	{
		Egueb_Dom_Node *children;
		Egueb_Dom_Node *href = NULL;

		children = egueb_dom_element_child_first_get(n);
		if (children)
		{
			egueb_dom_node_unref(children);
			return egueb_dom_node_ref(n);
		}

		_egueb_svg_element_pattern_xlink_href_node_get(n, &href);
		if (href)
		{
			children = egueb_svg_element_pattern_deep_children_get(href);
			egueb_dom_node_unref(href);
			return children;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

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

EAPI void egueb_svg_element_pattern_x_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	egueb_dom_attr_set(thiz->x, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_pattern_x_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->x, v);
}

EAPI void egueb_svg_element_pattern_y_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	egueb_dom_attr_set(thiz->y, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_pattern_y_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->y, v);
}

EAPI void egueb_svg_element_pattern_width_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	egueb_dom_attr_set(thiz->width, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_pattern_width_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->width, v);
}

EAPI void egueb_svg_element_pattern_height_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	egueb_dom_attr_set(thiz->height, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_pattern_height_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->height, v);
}

EAPI void egueb_svg_element_pattern_xlink_href_set(Egueb_Dom_Node *n, Egueb_Dom_String *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	egueb_dom_attr_set(thiz->xlink_href, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_pattern_xlink_href_get(Egueb_Dom_Node *n, Egueb_Svg_String_Animated *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->xlink_href, v);
}

EAPI void egueb_svg_element_pattern_pattern_units_set(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	egueb_dom_attr_set(thiz->units, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_pattern_pattern_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units_Animated *v)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->units, v);
}

EAPI void egueb_svg_element_pattern_pattern_transform_set(Egueb_Dom_Node *n,
		Enesim_Matrix *m)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	egueb_dom_attr_set(thiz->transform, EGUEB_DOM_ATTR_TYPE_BASE, m);
}

EAPI void egueb_svg_element_pattern_pattern_transform_get(Egueb_Dom_Node *n,
		Egueb_Svg_Matrix_Animated *m)
{
	Egueb_Svg_Element_Pattern *thiz;

	thiz = EGUEB_SVG_ELEMENT_PATTERN(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->transform, m);
}
