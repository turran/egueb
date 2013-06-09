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
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_gradient.h"
#include "egueb_svg_gradient_private.h"
#include "egueb_svg_element_linear_gradient.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_DESCRIPTOR 				\
		egueb_svg_element_linear_gradient_descriptor_get()
#define EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, \
		Egueb_Svg_Element_Linear_Gradient_Class, 			\
		EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_LINEAR_GRADIENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 	\
		Egueb_Svg_Element_Linear_Gradient, 				\
		EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Linear_Gradient
{
	Egueb_Svg_Gradient base;
	/* properties */
	Egueb_Dom_Node *x1;
	Egueb_Dom_Node *y1;
	Egueb_Dom_Node *x2;
	Egueb_Dom_Node *y2;
	/* private */
} Egueb_Svg_Element_Linear_Gradient;

typedef struct _Egueb_Svg_Element_Linear_Gradient_Class
{
	Egueb_Svg_Gradient_Class base;
} Egueb_Svg_Element_Linear_Gradient_Class;

#define EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_DEEP_LENGTH_GET(n, attr, def, fun)	\
	if (egueb_svg_is_element_linear_gradient(n))				\
	{									\
		Egueb_Svg_Element_Linear_Gradient *thiz;			\
		thiz = EGUEB_SVG_ELEMENT_LINEAR_GRADIENT(n);			\
		if (!egueb_dom_attr_is_set(thiz->attr))				\
		{								\
			Egueb_Dom_Node *href = NULL;				\
			egueb_svg_gradient_href_node_get(n, &href);		\
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
		Egueb_Dom_Node *href = NULL;					\
		egueb_svg_gradient_href_node_get(n, &href);			\
		if (href)							\
		{								\
			fun(href, attr);					\
			egueb_dom_node_unref(href);				\
		}								\
		else								\
		{								\
			*attr = def;						\
		}								\
	}
/*----------------------------------------------------------------------------*
 *                           Referenceable interface                          *
 *----------------------------------------------------------------------------*/
static Egueb_Svg_Reference * _egueb_svg_element_linear_gradient_reference_new(
		Egueb_Svg_Referenceable *r, Egueb_Dom_Node *referencer)
{
	Egueb_Svg_Reference *ref;

	DBG("Creating a new reference");
	ref = egueb_svg_reference_gradient_linear_new();
	return ref;
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_linear_gradient_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_LINEAR_GRADIENT);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_linear_gradient, Egueb_Svg_Element_Linear_Gradient, x1);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_linear_gradient, Egueb_Svg_Element_Linear_Gradient, y1);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_linear_gradient, Egueb_Svg_Element_Linear_Gradient, x2);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_element_linear_gradient, Egueb_Svg_Element_Linear_Gradient, y2);

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_GRADIENT_DESCRIPTOR,
		Egueb_Svg_Element_Linear_Gradient, Egueb_Svg_Element_Linear_Gradient_Class,
		egueb_svg_element_linear_gradient);

static void _egueb_svg_element_linear_gradient_class_init(void *k)
{
	Egueb_Svg_Referenceable_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_REFERENCEABLE_CLASS(k);
	klass->reference_new = _egueb_svg_element_linear_gradient_reference_new;

	e_klass = EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_linear_gradient_tag_name_get;
}

static void _egueb_svg_element_linear_gradient_class_deinit(void *k)
{
}

static void _egueb_svg_element_linear_gradient_instance_init(void *o)
{
	Egueb_Svg_Element_Linear_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_LINEAR_GRADIENT(o);
	/* create the properties */
	thiz->x1 = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X1),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y1 = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y1),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->x2 = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_X2),
			&EGUEB_SVG_LENGTH_100_PERCENT, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->y2 = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_Y2),
			&EGUEB_SVG_LENGTH_0, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_linear_gradient, x1);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_linear_gradient, y1);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_linear_gradient, x2);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_element_linear_gradient, y2);
}

static void _egueb_svg_element_linear_gradient_instance_deinit(void *o)
{
	Egueb_Svg_Element_Linear_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_LINEAR_GRADIENT(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->x1);
	egueb_dom_node_unref(thiz->y1);
	egueb_dom_node_unref(thiz->x2);
	egueb_dom_node_unref(thiz->y2);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_linear_gradient_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_linear_gradient);
	return n;
}

EAPI Eina_Bool egueb_svg_is_element_linear_gradient(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI void egueb_svg_element_linear_gradient_deep_x1_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *x1)
{
	EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_DEEP_LENGTH_GET(n, x1,
		EGUEB_SVG_LENGTH_0, egueb_svg_element_linear_gradient_deep_x1_get);
}

EAPI void egueb_svg_element_linear_gradient_deep_y1_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *y1)
{
	EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_DEEP_LENGTH_GET(n, y1,
		EGUEB_SVG_LENGTH_0, egueb_svg_element_linear_gradient_deep_y1_get);
}

EAPI void egueb_svg_element_linear_gradient_deep_x2_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *x2)
{
	EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_DEEP_LENGTH_GET(n, x2,
		EGUEB_SVG_LENGTH_100_PERCENT, egueb_svg_element_linear_gradient_deep_x2_get);
}

EAPI void egueb_svg_element_linear_gradient_deep_y2_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *y2)
{
	EGUEB_SVG_ELEMENT_LINEAR_GRADIENT_DEEP_LENGTH_GET(n, y2,
		EGUEB_SVG_LENGTH_0, egueb_svg_element_linear_gradient_deep_y2_get);
}

#if 0
EAPI void egueb_svg_element_linear_gradient_deep_y1_get(Egueb_Svg_Element_Linear_Gradient *thiz,
		Egueb_Dom_Tag *t,
		Egueb_Svg_Coord *y1)
{
	Egueb_Dom_Tag *href;

	href = egueb_svg_element_gradient_href_tag_get(t);
	if (!thiz->y1.is_set && href)
	{
		Egueb_Svg_Element_Linear_Gradient *other;

		other = _egueb_svg_element_linear_gradient_get(href);
		_egueb_svg_element_linear_gradient_deep_y1_get(other, href, y1);
	}
	else
		*y1 = thiz->y1.v;
}

EAPI void egueb_svg_element_linear_gradient_deep_x2_get(Egueb_Svg_Element_Linear_Gradient *thiz,
		Egueb_Dom_Tag *t,
		Egueb_Svg_Coord *x2)
{
	Egueb_Dom_Tag *href;

	href = egueb_svg_element_gradient_href_tag_get(t);
	if (!thiz->x2.is_set && href)
	{
		Egueb_Svg_Element_Linear_Gradient *other;

		other = _egueb_svg_element_linear_gradient_get(href);
		_egueb_svg_element_linear_gradient_deep_x2_get(other, href, x2);
	}
	else
		*x2 = thiz->x2.v;
}

EAPI void egueb_svg_element_linear_gradient_deep_y2_get(Egueb_Svg_Element_Linear_Gradient *thiz,
		Egueb_Dom_Tag *t,
		Egueb_Svg_Coord *y2)
{
	Egueb_Dom_Tag *href;

	href = egueb_svg_element_gradient_href_tag_get(t);
	if (!thiz->y2.is_set && href)
	{
		Egueb_Svg_Element_Linear_Gradient *other;

		other = _egueb_svg_element_linear_gradient_get(href);
		_egueb_svg_element_linear_gradient_deep_y2_get(other, href, y2);
	}
	else
		*y2 = thiz->y2.v;
}
#endif

#if 0
EAPI void egueb_svg_element_linear_gradient_x1_set(Ender_Element *e, const Egueb_Svg_Coord *x1)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_LINEAR_GRADIENT_X1, x1, NULL);
}

EAPI void egueb_svg_element_linear_gradient_x1_get(Ender_Element *e, Egueb_Svg_Coord *x1)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_linear_gradient_x1_get(t, x1);
}

EAPI Eina_Bool egueb_svg_element_linear_gradient_x1_is_set(Ender_Element *e)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	return _egueb_svg_element_linear_gradient_x1_is_set(t);
}

EAPI void egueb_svg_element_linear_gradient_y1_set(Ender_Element *e, const Egueb_Svg_Coord *y1)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_LINEAR_GRADIENT_Y1, y1, NULL);
}

EAPI void egueb_svg_element_linear_gradient_y1_get(Ender_Element *e, Egueb_Svg_Coord *y1)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_linear_gradient_y1_get(t, y1);
}

EAPI Eina_Bool egueb_svg_element_linear_gradient_y1_is_set(Ender_Element *e)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	return _egueb_svg_element_linear_gradient_y1_is_set(t);
}

EAPI void egueb_svg_element_linear_gradient_x2_set(Ender_Element *e, const Egueb_Svg_Coord *x2)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_LINEAR_GRADIENT_X2, x2, NULL);
}

EAPI void egueb_svg_element_linear_gradient_x2_get(Ender_Element *e, Egueb_Svg_Coord *x2)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_linear_gradient_x2_get(t, x2);
}

EAPI Eina_Bool egueb_svg_element_linear_gradient_x2_is_set(Ender_Element *e)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	return _egueb_svg_element_linear_gradient_x2_is_set(t);
}

EAPI void egueb_svg_element_linear_gradient_y2_set(Ender_Element *e, const Egueb_Svg_Coord *y2)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_LINEAR_GRADIENT_Y2, y2, NULL);
}

EAPI void egueb_svg_element_linear_gradient_y2_get(Ender_Element *e, Egueb_Svg_Coord *y2)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_linear_gradient_y2_get(t, y2);
}

EAPI Eina_Bool egueb_svg_element_linear_gradient_y2_is_set(Ender_Element *e)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	return _egueb_svg_element_linear_gradient_y2_is_set(t);
}
#endif
