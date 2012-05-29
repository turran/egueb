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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_referenceable.h"
#include "esvg_private_paint_server.h"
#include "esvg_private_gradient.h"
#include "esvg_gradient.h"

/*
 * TODO
 * the stops_changed flag has to be cleared somewhere but where? add a cleanup
 * function?
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_gradient

#define ESVG_GRADIENT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_GRADIENT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_GRADIENT_MAGIC);\
	} while(0)

static Ender_Property *ESVG_GRADIENT_HREF;

typedef struct _Esvg_Gradient_Descriptor_Internal
{
	Edom_Tag_Free free;
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	Esvg_Element_Attribute_Set attribute_set;
	Esvg_Referenceable_Setup setup;
	Esvg_Gradient_Propagate propagate;
} Esvg_Gradient_Descriptor_Internal;

typedef struct _Esvg_Gradient_State
{	
	char *href;
} Esvg_Gradient_State;

typedef struct _Esvg_Gradient
{
	EINA_MAGIC
	/* properties */
	Esvg_Gradient_Context context;
	Esvg_Gradient_State current;
	Esvg_Gradient_State past;
	/* private */
	Esvg_Gradient_Descriptor_Internal descriptor;
	Eina_Bool units_set : 1;
	Eina_Bool transform_is_set : 1;
	Eina_Bool stops_changed : 1;
	void *data;
} Esvg_Gradient;

static Esvg_Gradient * _esvg_gradient_get(Edom_Tag *t)
{
	Esvg_Gradient *thiz;

	thiz = esvg_paint_server_data_get(t);
	ESVG_GRADIENT_MAGIC_CHECK(thiz);

	return thiz;
}

static void _esvg_gradient_stop_mutation_cb(Ender_Element *e,
		const char *event_name,
		void *event_data, void *data)
{
	Esvg_Gradient *thiz = data;

	thiz->stops_changed = EINA_TRUE;
}

static Eina_Bool _esvg_gradient_stop_propagate(Edom_Tag *t, Edom_Tag *child,
		void *user_data)
{
	Enesim_Renderer_Gradient_Stop *stop;
	Enesim_Renderer *r = user_data;

	stop = esvg_stop_gradient_stop_get(child);
	DBG("Adding a gradient stop at position %g with color %08x", stop->pos, stop->argb);
	enesim_renderer_gradient_stop_add(r, stop);
}
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_gradient_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "xlink:href") == 0)
	{
		esvg_gradient_href_set(e, value);
	}
	else
	{
		Esvg_Gradient *thiz;
		Edom_Tag *t;

		t = ender_element_object_get(e);
		thiz = _esvg_gradient_get(t);
		if (thiz->descriptor.attribute_set)
			return thiz->descriptor.attribute_set(e, key, value);
	}
	return EINA_TRUE;
}

static Eina_Bool _esvg_gradient_child_add(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Gradient *thiz;
	Esvg_Type type;
	Ender_Element *child_e;

	if (!esvg_is_element_internal(child))
		return EINA_FALSE;

	thiz = _esvg_gradient_get(t);
	type = esvg_element_internal_type_get(child);
	/* only support stops */
	if (type != ESVG_STOP)
		return EINA_FALSE;

	child_e = esvg_element_ender_get(child);
	ender_event_listener_add(child_e, "Mutation", _esvg_gradient_stop_mutation_cb, thiz);
	thiz->stops_changed = EINA_TRUE;

	return EINA_TRUE;
}

static Eina_Bool _esvg_gradient_child_remove(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	thiz->stops_changed = EINA_TRUE;
	// remove the event handler
	//ender_event_listener_remove(child_e, "Mutation", _esvg_gradient_stop_mutation_cb, thiz);
	return EINA_TRUE;
}

static Eina_Bool _esvg_gradient_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (!thiz->stops_changed)
		return EINA_TRUE;

	/* call the setup on the stops */
	return esvg_element_internal_child_setup(t, c, error, NULL, thiz);
}

static void _esvg_gradient_cleanup(Edom_Tag *t)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	thiz->stops_changed = EINA_FALSE;
}

static Eina_Bool _esvg_gradient_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Enesim_Renderer *r,
		Enesim_Error **error)
{
	Esvg_Gradient *thiz;
	Eina_Bool ret = EINA_TRUE;

	thiz = _esvg_gradient_get(t);
	if (thiz->stops_changed)
	{
		enesim_renderer_gradient_stop_clear(r);
		edom_tag_child_foreach(t, _esvg_gradient_stop_propagate, r);
	}
	if (thiz->descriptor.propagate)
		ret = thiz->descriptor.propagate(t, c, ctx, attr, &thiz->context, r, error);
	return ret;
}

static Eina_Bool _esvg_gradient_reference_add(Edom_Tag *t, Esvg_Referenceable_Reference *rr)
{
	Esvg_Gradient *thiz;
	Enesim_Renderer *r;

	thiz = _esvg_gradient_get(t);
	/* in case some stop has changed:
	 *  the setup will be called
	 *  the stops will be generated
	 *  the stops will be propagated to each reference
	 * so no need to do twice
	 */
	if (thiz->stops_changed)
		return EINA_TRUE;
	/* add every stop to our newly created reference */
	r = rr->data;
	enesim_renderer_gradient_stop_clear(r);
	edom_tag_child_foreach(t, _esvg_gradient_stop_propagate, r);
}

static void _esvg_gradient_free(Edom_Tag *t)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _esvg_gradient_href_set(Edom_Tag *t, const char *href)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (thiz->current.href)
	{
		free(thiz->current.href);
		thiz->current.href = NULL;
	}

	if (href)
	{
		thiz->current.href = strdup(href);
	}
}

static void _esvg_gradient_href_get(Edom_Tag *t, const char **href)
{
	Esvg_Gradient *thiz;

	if (!href) return;

	thiz = _esvg_gradient_get(t);
	*href = thiz->current.href;
}

static void _esvg_gradient_units_set(Edom_Tag *t, Esvg_Gradient_Units units)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	thiz->context.units = units;
	thiz->units_set = EINA_TRUE;
}

static void _esvg_gradient_units_get(Edom_Tag *t, Esvg_Gradient_Units *units)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (units) *units = thiz->context.units;
}

static Eina_Bool _esvg_gradient_units_is_set(Edom_Tag *t)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	return thiz->units_set;
}

static void _esvg_gradient_transform_set(Edom_Tag *t, const Enesim_Matrix *transform)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (transform) thiz->context.transform = *transform;
}

static void _esvg_gradient_transform_get(Edom_Tag *t, Enesim_Matrix *transform)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (transform) *transform = thiz->context.transform;
}

static Eina_Bool _esvg_gradient_transform_is_set(Edom_Tag *t)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	return thiz->transform_is_set;
}

static void _esvg_gradient_spread_method_set(Edom_Tag *t, Esvg_Spread_Method spread_method)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	thiz->context.spread_method = spread_method;
}

static void _esvg_gradient_spread_method_get(Edom_Tag *t, Esvg_Spread_Method *spread_method)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (spread_method) *spread_method = thiz->context.spread_method;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_gradient_href_is_set NULL
#include "generated/esvg_generated_gradient.c"

Edom_Tag * esvg_gradient_new(Esvg_Gradient_Descriptor *descriptor,
		Esvg_Type type,
		void *data)
{
	Esvg_Gradient *thiz;
	Esvg_Paint_Server_Descriptor pdescriptor;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Gradient));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_GRADIENT_MAGIC);
	thiz->descriptor.propagate = descriptor->propagate;
	thiz->descriptor.child_add = descriptor->child_add;
	thiz->descriptor.child_remove = descriptor->child_remove;
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->data = data;

	pdescriptor.child_add = _esvg_gradient_child_add;
	pdescriptor.child_remove = _esvg_gradient_child_remove;
	pdescriptor.attribute_set = _esvg_gradient_attribute_set;
	pdescriptor.attribute_get = descriptor->attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.free = _esvg_gradient_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = _esvg_gradient_setup;
	pdescriptor.cleanup = _esvg_gradient_cleanup;
	pdescriptor.renderer_new = descriptor->renderer_new;
	pdescriptor.propagate = _esvg_gradient_propagate;
	pdescriptor.reference_add = _esvg_gradient_reference_add;

	/* Default values */
	thiz->context.units = ESVG_OBJECT_BOUNDING_BOX;
	enesim_matrix_identity(&thiz->context.transform);

	t = esvg_paint_server_new(&pdescriptor, type, thiz);
	return t;
}

void * esvg_gradient_data_get(Edom_Tag *t)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	return thiz->data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_is_gradient(Ender_Element *e)
{
}

EAPI void esvg_gradient_href_set(Ender_Element *e, const char *href)
{
	ender_element_property_value_set(e, ESVG_GRADIENT_HREF, href, NULL);
}

EAPI void esvg_gradient_href_get(Ender_Element *e, const char **href)
{
}

EAPI void esvg_gradient_units_set(Ender_Element *e, Esvg_Gradient_Units units)
{
}

EAPI void esvg_gradient_units_get(Ender_Element *e, Esvg_Gradient_Units *units)
{
}

EAPI Eina_Bool esvg_gradient_units_is_set(Ender_Element *e)
{
}

EAPI void esvg_gradient_transform_set(Ender_Element *e, const Enesim_Matrix *transform)
{
}

EAPI void esvg_gradient_transform_get(Ender_Element *e, Enesim_Matrix *transform)
{
}

EAPI Eina_Bool esvg_gradient_transform_is_set(Ender_Element *e)
{
}

EAPI void esvg_gradient_spread_method_set(Ender_Element *e, Esvg_Spread_Method spread_method)
{
}

EAPI void esvg_gradient_spread_method_get(Ender_Element *e, Esvg_Spread_Method *spread_method)
{
}

