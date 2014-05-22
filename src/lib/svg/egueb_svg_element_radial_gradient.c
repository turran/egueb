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
#include "egueb_svg_spread_method.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_gradient.h"
#include "egueb_svg_element_radial_gradient.h"

#include "egueb_svg_gradient_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DESCRIPTOR 				\
		egueb_svg_element_radial_gradient_descriptor_get()
#define EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, \
		Egueb_Svg_Element_Radial_Gradient_Class, 			\
		EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DESCRIPTOR)
#define EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 	\
		Egueb_Svg_Element_Radial_Gradient, 				\
		EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DESCRIPTOR)

typedef struct _Egueb_Svg_Element_Radial_Gradient
{
	Egueb_Svg_Gradient base;
	/* properties */
	Egueb_Dom_Node *cx;
	Egueb_Dom_Node *cy;
	Egueb_Dom_Node *fx;
	Egueb_Dom_Node *fy;
	Egueb_Dom_Node *r;
	/* private */
} Egueb_Svg_Element_Radial_Gradient;

typedef struct _Egueb_Svg_Element_Radial_Gradient_Class
{
	Egueb_Svg_Gradient_Class base;
} Egueb_Svg_Element_Radial_Gradient_Class;

#define EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DEEP_LENGTH_GET(n, attr, def, fun)	\
	if (egueb_svg_is_element_radial_gradient(n))				\
	{									\
		Egueb_Svg_Element_Radial_Gradient *thiz;			\
		thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);			\
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

#define EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DEEP_COND_LENGTH_GET(n, attr, fun)	\
	if (egueb_svg_is_element_radial_gradient(n))				\
	{									\
		Egueb_Svg_Element_Radial_Gradient *thiz;			\
		thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);			\
		if (!egueb_dom_attr_is_set(thiz->attr))				\
		{								\
			Egueb_Dom_Node *href = NULL;				\
			egueb_svg_gradient_href_node_get(n, &href);		\
			if (href)						\
			{							\
				Eina_Bool ret;					\
				ret =  fun(href, attr);				\
				egueb_dom_node_unref(href);			\
				return ret;					\
			}							\
			else							\
			{							\
				return EINA_FALSE;				\
			}							\
		}								\
		else								\
		{								\
			return egueb_dom_attr_final_get(thiz->attr, attr);	\
		}								\
	}									\
	else									\
	{									\
		Egueb_Dom_Node *href = NULL;					\
		egueb_svg_gradient_href_node_get(n, &href);			\
		if (href)							\
		{								\
			Eina_Bool ret;						\
			ret = fun(href, attr);					\
			egueb_dom_node_unref(href);				\
			return ret;						\
		}								\
		else								\
		{								\
			return EINA_FALSE;					\
		}								\
	}

#if 0
static Eina_Bool _egueb_svg_element_radial_gradient_deep_fy_get(Egueb_Svg_Element_Radial_Gradient *thiz,
		Egueb_Dom_Tag *t,
		Egueb_Svg_Length *fy)
{
	Egueb_Svg_Attribute_Length l;

	egueb_svg_attribute_animated_length_merge(&thiz->fy, &l);
	if (!l.is_set)
	{
		Egueb_Dom_Tag *href;

		href = egueb_svg_element_gradient_href_tag_get(t);
 		if (href)
		{
			Egueb_Svg_Element_Radial_Gradient *other;

			other = _egueb_svg_element_radial_gradient_get(href);
			if (!other) return EINA_FALSE;

			return _egueb_svg_element_radial_gradient_deep_fy_get(other, href, fy);
		}
		return EINA_FALSE;
	}
	else
	{
		*fy = l.v;
		return EINA_TRUE;
	}
}

static Eina_Bool _egueb_svg_element_radial_gradient_deep_fx_get(Egueb_Svg_Element_Radial_Gradient *thiz,
		Egueb_Dom_Tag *t,
		Egueb_Svg_Length *fx)
{
	Egueb_Svg_Attribute_Length l;

	egueb_svg_attribute_animated_length_merge(&thiz->fx, &l);
	if (!l.is_set)
	{
		Egueb_Dom_Tag *href;

		href = egueb_svg_element_gradient_href_tag_get(t);
		if (href)
		{
			Egueb_Svg_Element_Radial_Gradient *other;

			other = _egueb_svg_element_radial_gradient_get(href);
			if (!other) return EINA_FALSE;

			return _egueb_svg_element_radial_gradient_deep_fx_get(other, href, fx);
		}
		return EINA_FALSE;
	}
	else
	{
		*fx = l.v;
		return EINA_TRUE;
	}
}

static void _egueb_svg_element_radial_gradient_deep_cx_get(Egueb_Svg_Element_Radial_Gradient *thiz,
		Egueb_Dom_Tag *t,
		Egueb_Svg_Length *cx)
{
	Egueb_Svg_Attribute_Length l;
	Egueb_Dom_Tag *href;

	href = egueb_svg_element_gradient_href_tag_get(t);
	egueb_svg_attribute_animated_length_merge(&thiz->cx, &l);
	if (!l.is_set && href)
	{
		Egueb_Svg_Element_Radial_Gradient *other;

		other = _egueb_svg_element_radial_gradient_get(href);
		_egueb_svg_element_radial_gradient_deep_cx_get(other, href, cx);
	}
	else
		*cx = l.v;
}

static void _egueb_svg_element_radial_gradient_deep_cy_get(Egueb_Svg_Element_Radial_Gradient *thiz,
		Egueb_Dom_Tag *t,
		Egueb_Svg_Coord *cy)
{
	Egueb_Svg_Attribute_Length l;
	Egueb_Dom_Tag *href;

	href = egueb_svg_element_gradient_href_tag_get(t);
	egueb_svg_attribute_animated_length_merge(&thiz->cy, &l);
	if (!l.is_set && href)
	{
		Egueb_Svg_Element_Radial_Gradient *other;

		other = _egueb_svg_element_radial_gradient_get(href);
		_egueb_svg_element_radial_gradient_deep_cy_get(other, href, cy);
	}
	else
		*cy = l.v;
}

static void _egueb_svg_element_radial_gradient_deep_rad_get(Egueb_Svg_Element_Radial_Gradient *thiz,
		Egueb_Dom_Tag *t,
		Egueb_Svg_Length *rad)
{
	Egueb_Svg_Attribute_Length l;
	Egueb_Dom_Tag *href;

	href = egueb_svg_element_gradient_href_tag_get(t);
	egueb_svg_attribute_animated_length_merge(&thiz->rad, &l);
	if (!l.is_set && href)
	{
		Egueb_Svg_Element_Radial_Gradient *other;

		other = _egueb_svg_element_radial_gradient_get(href);
		_egueb_svg_element_radial_gradient_deep_rad_get(other, href, rad);
	}
	else
		*rad = l.v;
}

static void _egueb_svg_element_radial_gradient_merge(Egueb_Svg_Element_Radial_Gradient *thiz,
		Egueb_Dom_Tag *t,
		Egueb_Svg_Coord *cx, Egueb_Svg_Coord *cy,
		Egueb_Svg_Coord *fx, Egueb_Svg_Coord *fy,
		Egueb_Svg_Length *rad)
{
	_egueb_svg_element_radial_gradient_deep_cx_get(thiz, t, cx);
	_egueb_svg_element_radial_gradient_deep_cy_get(thiz, t, cy);
	/* for fx and fy, if those are not set or inherited, then use
	 * cx or cy accordingly
	 */
	if (!_egueb_svg_element_radial_gradient_deep_fx_get(thiz, t, fx))
		*fx = *cx;
	if (!_egueb_svg_element_radial_gradient_deep_fy_get(thiz, t, fy))
		*fy = *cy;
	_egueb_svg_element_radial_gradient_deep_rad_get(thiz, t, rad);
}
#endif
/*----------------------------------------------------------------------------*
 *                           Referenceable interface                          *
 *----------------------------------------------------------------------------*/
static Egueb_Svg_Reference * _egueb_svg_element_radial_gradient_reference_new(
		Egueb_Svg_Referenceable *r, Egueb_Dom_Node *referencer)
{
	Egueb_Svg_Reference *ref;

	DBG("Creating a new reference");
	ref = egueb_svg_reference_gradient_radial_new();
	return ref;
}
/*----------------------------------------------------------------------------*
 *                              Element interface                             *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_String * _egueb_svg_element_radial_gradient_tag_name_get(
		Egueb_Dom_Element *e)
{
	return egueb_dom_string_ref(EGUEB_SVG_NAME_RADIAL_GRADIENT);
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_SVG_GRADIENT_DESCRIPTOR,
		Egueb_Svg_Element_Radial_Gradient, Egueb_Svg_Element_Radial_Gradient_Class,
		egueb_svg_element_radial_gradient);

static void _egueb_svg_element_radial_gradient_class_init(void *k)
{
	Egueb_Svg_Referenceable_Class *klass;
	Egueb_Dom_Element_Class *e_klass;

	klass = EGUEB_SVG_REFERENCEABLE_CLASS(k);
	klass->reference_new = _egueb_svg_element_radial_gradient_reference_new;

	e_klass = EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->tag_name_get = _egueb_svg_element_radial_gradient_tag_name_get;
}

static void _egueb_svg_element_radial_gradient_instance_init(void *o)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(o);
	/* create the properties */
	thiz->cx = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_CX),
			&EGUEB_SVG_LENGTH_50_PERCENT, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->cy = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_CY),
			&EGUEB_SVG_LENGTH_50_PERCENT, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->fx = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_FX),
			NULL, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->fy = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_FY),
			NULL, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->r = egueb_svg_attr_length_new(
			egueb_dom_string_ref(EGUEB_SVG_R),
			&EGUEB_SVG_LENGTH_0,
			EINA_TRUE, EINA_FALSE, EINA_FALSE);

	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->cx), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->cy), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->fx), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->fy), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->r), NULL);
}

static void _egueb_svg_element_radial_gradient_instance_deinit(void *o)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(o);
	/* destroy the properties */
	egueb_dom_node_unref(thiz->cx);
	egueb_dom_node_unref(thiz->cy);
	egueb_dom_node_unref(thiz->fx);
	egueb_dom_node_unref(thiz->fy);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_element_radial_gradient_deep_cx_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *cx)
{
	EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DEEP_LENGTH_GET(n, cx,
		EGUEB_SVG_LENGTH_50_PERCENT,
		egueb_svg_element_radial_gradient_deep_cx_get);
}

void egueb_svg_element_radial_gradient_deep_cy_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *cy)
{
	EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DEEP_LENGTH_GET(n, cy,
		EGUEB_SVG_LENGTH_50_PERCENT,
		egueb_svg_element_radial_gradient_deep_cy_get);
}

Eina_Bool egueb_svg_element_radial_gradient_deep_fx_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *fx)
{
	EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DEEP_COND_LENGTH_GET(n, fx,
		egueb_svg_element_radial_gradient_deep_fx_get);
}

Eina_Bool egueb_svg_element_radial_gradient_deep_fy_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *fy)
{
	EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DEEP_COND_LENGTH_GET(n, fy,
		egueb_svg_element_radial_gradient_deep_fy_get);
}

void egueb_svg_element_radial_gradient_deep_r_get(Egueb_Dom_Node *n,
		Egueb_Svg_Length *r)
{
	EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DEEP_LENGTH_GET(n, r,
		EGUEB_SVG_LENGTH_50_PERCENT,
		egueb_svg_element_radial_gradient_deep_r_get);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_svg_element_radial_gradient_new(void)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_svg_element_radial_gradient);
	return n;
}

EAPI Eina_Bool egueb_svg_is_element_radial_gradient(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_ELEMENT_RADIAL_GRADIENT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI void egueb_svg_element_radial_gradient_cx_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	egueb_dom_attr_set(thiz->cx, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_radial_gradient_cx_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->cx, v);
}

EAPI void egueb_svg_element_radial_gradient_cy_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	egueb_dom_attr_set(thiz->cy, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_radial_gradient_cy_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->cy, v);
}

EAPI void egueb_svg_element_radial_gradient_fx_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	egueb_dom_attr_set(thiz->fx, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_radial_gradient_fx_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->fx, v);
}

EAPI void egueb_svg_element_radial_gradient_fy_set(Egueb_Dom_Node *n, const Egueb_Svg_Coord *v)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	egueb_dom_attr_set(thiz->fy, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_element_radial_gradient_fy_get(Egueb_Dom_Node *n, Egueb_Svg_Coord_Animated *v)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->fy, v);
}

EAPI void egueb_svg_element_radial_gradient_r_set(Egueb_Dom_Node *n, const Egueb_Svg_Length *r)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	egueb_dom_attr_set(thiz->r, EGUEB_DOM_ATTR_TYPE_BASE, r);
}

EAPI void egueb_svg_element_radial_gradient_r_get(Egueb_Dom_Node *n, Egueb_Svg_Length_Animated *r)
{
	Egueb_Svg_Element_Radial_Gradient *thiz;

	thiz = EGUEB_SVG_ELEMENT_RADIAL_GRADIENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->r, r);
}
