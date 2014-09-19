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
#include "egueb_svg_matrix.h"
#include "egueb_svg_length.h"
#include "egueb_svg_string.h"
#include "egueb_svg_spread_method.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_attr_xlink_href.h"
#include "egueb_svg_element_stop.h"
#include "egueb_svg_gradient.h"

#include "egueb_svg_gradient_private.h"
#include "egueb_svg_attr_referenceable_units_private.h"
#include "egueb_svg_attr_matrix_private.h"
#include "egueb_svg_attr_spread_method_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_GRADIENT_DEEP_GET(n, attr, def, fun)				\
	if (egueb_svg_is_gradient(n))						\
	{									\
		Egueb_Svg_Gradient *thiz;					\
		thiz = EGUEB_SVG_GRADIENT(n);					\
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
		*attr = def;							\
	}

/* any change on the stop also trigger a change on the gradient */
static void _egueb_svg_gradient_stop_request_process_cb(Egueb_Dom_Event *e,
		void *user_data)
{
	Egueb_Svg_Gradient *thiz = user_data;

	INFO("A stop requested a process, let's request ourselves too");
	egueb_dom_element_request_process(EGUEB_DOM_NODE(thiz));
}

static Eina_Bool _egueb_svg_gradient_is_our_stop(Egueb_Svg_Gradient *thiz,
		Egueb_Dom_Event *e, Egueb_Dom_Node **stop)
{
	Egueb_Dom_Node *target;
	Egueb_Dom_Node *relative;
	Eina_Bool ret = EINA_TRUE;

	relative = egueb_dom_event_mutation_related_get(e);
	if (relative != EGUEB_DOM_NODE(thiz))
	{
		ret = EINA_FALSE;
		goto not_us;
	}
	target = egueb_dom_event_target_get(e);
	if (!egueb_svg_element_is_stop(target))
	{
		ret = EINA_FALSE;
		goto not_stop;
	}
	*stop = egueb_dom_node_ref(target);
not_stop:
	egueb_dom_node_unref(target);
not_us:
	egueb_dom_node_unref(relative);
	return ret;
}

static void _egueb_svg_gradient_node_inserted_cb(Egueb_Dom_Event *e,
		void *user_data)
{
	Egueb_Svg_Gradient *thiz = user_data;
	Egueb_Dom_Node *stop;

	if (!_egueb_svg_gradient_is_our_stop(thiz, e, &stop))
		return;
	egueb_dom_node_event_listener_add(stop,
			EGUEB_DOM_EVENT_PROCESS,
			_egueb_svg_gradient_stop_request_process_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_unref(stop);
}

static void _egueb_svg_gradient_node_removed_cb(Egueb_Dom_Event *e,
		void *user_data)
{
	Egueb_Svg_Gradient *thiz = user_data;
	Egueb_Dom_Node *stop;

	if (!_egueb_svg_gradient_is_our_stop(thiz, e, &stop))
		return;
	egueb_dom_node_event_listener_remove(stop,
			EGUEB_DOM_EVENT_PROCESS,
			_egueb_svg_gradient_stop_request_process_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_unref(stop);
}

static Eina_Bool _egueb_svg_gradient_resolve_xlink_href(Egueb_Svg_Gradient *thiz)
{
	return egueb_svg_attr_xlink_href_resolve(thiz->xlink_href);
}

static void _egueb_svg_gradient_deep_stop_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **stop)
{
	Egueb_Dom_Node *child = NULL;

	if (!egueb_svg_is_gradient(n))
	{
		*stop = NULL;
		return;
	}
	/* in case we do have a stop, return it, otherwise
	 * go find the stop on the xlink href node
	 */
	child = egueb_dom_node_child_first_get(n);
	if (!child)
	{
		Egueb_Dom_Node *href = NULL;
		egueb_svg_gradient_href_node_get(n, &href);
		if (!href)
		{
			*stop = NULL;
			return;
		}
		else
		{
			_egueb_svg_gradient_deep_stop_get(href, stop);
			egueb_dom_node_unref(href);
		}
	}
	else
	{
		while (child)
		{
			if (egueb_svg_element_is_stop(child))
			{
				*stop = child;
				break;
			}
			else
			{
				Egueb_Dom_Node *tmp = NULL;
				tmp = egueb_dom_node_sibling_next_get(child);
				egueb_dom_node_unref(child);
				child = tmp;
			}
		}
	}
}

static Eina_Bool _egueb_svg_gradient_children_process_cb(
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
static Eina_Bool _egueb_svg_gradient_process(
		Egueb_Svg_Referenceable *r)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(r);
	DBG("Processing a gradient");
	/* iterate over the children and call the process there */
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(r),
			_egueb_svg_gradient_children_process_cb,
			NULL);
	/* get the xlink:href node */
	if (!_egueb_svg_gradient_resolve_xlink_href(thiz))
		return EINA_FALSE;

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_PAINT_SERVER_DESCRIPTOR,
		Egueb_Svg_Gradient, Egueb_Svg_Gradient_Class,
		egueb_svg_gradient);

static void _egueb_svg_gradient_class_init(void *k)
{
	Egueb_Svg_Referenceable_Class *klass;

	klass = EGUEB_SVG_REFERENCEABLE_CLASS(k);
	klass->process = _egueb_svg_gradient_process;
}

static void _egueb_svg_gradient_instance_init(void *o)
{
	Egueb_Svg_Gradient *thiz;
	Egueb_Dom_Node *n;

	thiz = EGUEB_SVG_GRADIENT(o);
	thiz->units = egueb_svg_attr_referenceable_units_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_GRADIENT_UNITS),
			EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	thiz->transform = egueb_svg_attr_matrix_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_GRADIENT_TRANSFORM),
			&EGUEB_SVG_MATRIX_IDENTITY, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_xlink_href_new(
			egueb_dom_string_ref(EGUEB_DOM_NAME_XLINK_HREF),
			NULL);
	thiz->spread_method = egueb_svg_attr_spread_method_new(
			egueb_dom_string_ref(EGUEB_SVG_NAME_SPREAD_METHOD),
			EGUEB_SVG_SPREAD_METHOD_PAD, EINA_TRUE, EINA_FALSE,
			EINA_FALSE);
	n = EGUEB_DOM_NODE(o);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->transform), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->units), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->spread_method), NULL);
	egueb_dom_element_attribute_add(n, egueb_dom_node_ref(thiz->xlink_href), NULL);

	/* to identify in case a stop was added, we need to process again ourselves */
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_INSERTED,
			_egueb_svg_gradient_node_inserted_cb,
			EINA_FALSE, thiz);
	egueb_dom_node_event_listener_add(n,
			EGUEB_DOM_EVENT_MUTATION_NODE_REMOVED,
			_egueb_svg_gradient_node_removed_cb,
			EINA_FALSE, thiz);
}

static void _egueb_svg_gradient_instance_deinit(void *o)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(o);
	/* the properties */
	egueb_dom_node_unref(thiz->units);
	egueb_dom_node_unref(thiz->transform);
	egueb_dom_node_unref(thiz->spread_method);
	egueb_dom_node_unref(thiz->xlink_href);
	/* the private data */
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_gradient_href_node_get(Egueb_Dom_Node *n, Egueb_Dom_Node **href)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(n);
	egueb_svg_attr_xlink_href_node_get(thiz->xlink_href, href);
}

void egueb_svg_gradient_deep_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units *units)
{
	EGUEB_SVG_GRADIENT_DEEP_GET(n, units,
			EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX,
			egueb_svg_gradient_deep_units_get);
}

void egueb_svg_gradient_deep_transform_get(Egueb_Dom_Node *n,
		Enesim_Matrix *transform)
{
	Enesim_Matrix m;

	enesim_matrix_identity(&m);
	EGUEB_SVG_GRADIENT_DEEP_GET(n, transform, m,
			egueb_svg_gradient_deep_transform_get);
}

void egueb_svg_gradient_deep_stop_get(Egueb_Dom_Node *n,
		Egueb_Dom_Node **stop)
{
	_egueb_svg_gradient_deep_stop_get(n, stop);
}

void egueb_svg_gradient_deep_spread_method_get(Egueb_Dom_Node *n,
		Egueb_Svg_Spread_Method *spread_method)
{
	EGUEB_SVG_GRADIENT_DEEP_GET(n, spread_method,
			EGUEB_SVG_SPREAD_METHOD_PAD,
			egueb_svg_gradient_deep_spread_method_get);
}/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_is_gradient(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_GRADIENT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI void egueb_svg_gradient_xlink_href_set(Egueb_Dom_Node *n, Egueb_Dom_String *v)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(n);
	egueb_dom_attr_set(thiz->xlink_href, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_gradient_xlink_href_get(Egueb_Dom_Node *n, Egueb_Svg_String_Animated *v)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->xlink_href, v);
}

EAPI void egueb_svg_gradient_gradient_units_set(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units v)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(n);
	egueb_dom_attr_set(thiz->units, EGUEB_DOM_ATTR_TYPE_BASE, v);
}

EAPI void egueb_svg_gradient_gradient_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units_Animated *v)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->units, v);
}

EAPI void egueb_svg_gradient_gradient_transform_set(Egueb_Dom_Node *n,
		Enesim_Matrix *m)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(n);
	egueb_dom_attr_set(thiz->transform, EGUEB_DOM_ATTR_TYPE_BASE, m);
}

EAPI void egueb_svg_gradient_gradient_transform_get(Egueb_Dom_Node *n,
		Egueb_Svg_Matrix_Animated *m)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(n);
	EGUEB_SVG_ELEMENT_ATTR_ANIMATED_GET(thiz->transform, m);
}
