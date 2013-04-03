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
#include "esvg_main_private.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_context_private.h"
#include "esvg_element_private.h"
#include "esvg_element_svg_private.h"
#include "esvg_referenceable_private.h"
#include "esvg_paint_server_private.h"
#include "esvg_element_gradient_private.h"
#include "esvg_element_stop_private.h"
#include "esvg_element_gradient.h"

/*
 * TODO
 * the stops_changed flag has to be cleared somewhere but where? add a cleanup
 * function?
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_element_gradient_log

static int _esvg_element_gradient_log = -1;

#define ESVG_ELEMENT_GRADIENT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_ELEMENT_GRADIENT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_ELEMENT_GRADIENT_MAGIC);\
	} while(0)

static Ender_Property *ESVG_ELEMENT_GRADIENT_HREF;
static Ender_Property *ESVG_ELEMENT_GRADIENT_GRADIENT_UNITS;
static Ender_Property *ESVG_ELEMENT_GRADIENT_GRADIENT_TRANSFORM;
static Ender_Property *ESVG_ELEMENT_GRADIENT_SPREAD_METHOD;

typedef struct _Esvg_Element_Gradient_Descriptor_Internal
{
	Edom_Tag_Free free;
	Edom_Tag_Child_Add child_add;
	Edom_Tag_Child_Remove child_remove;
	Esvg_Element_Attribute_Set attribute_set;
	Esvg_Element_Attribute_Animated_Fetch attribute_animated_fetch;
	Esvg_Referenceable_Setup setup;
	Esvg_Element_Gradient_Propagate propagate;
} Esvg_Element_Gradient_Descriptor_Internal;

typedef struct _Esvg_Element_Gradient_State
{
	char *href;
} Esvg_Element_Gradient_State;

typedef struct _Esvg_Element_Gradient
{
	EINA_MAGIC
	/* properties */
	Esvg_Attribute_Enum units;
	Esvg_Attribute_Enum spread_method;
	Esvg_Attribute_Animated_Transform transform;
	Esvg_Element_Gradient_State current;
	Esvg_Element_Gradient_State past;
	/* private */
	Esvg_Element_Gradient_Descriptor_Internal descriptor;
	Eina_Bool stops_changed : 1;
	Eina_Bool state_changed : 1;
	/* whenever the href is set, this points to the real objects */
	Edom_Tag *href_t;
	Ender_Element *href_e;
	void *data;
} Esvg_Element_Gradient;

static Esvg_Element_Gradient * _esvg_element_gradient_get(Edom_Tag *t)
{
	Esvg_Element_Gradient *thiz;

	thiz = esvg_paint_server_data_get(t);
	ESVG_ELEMENT_GRADIENT_MAGIC_CHECK(thiz);

	return thiz;
}

static void _esvg_element_gradient_stop_mutation_cb(Ender_Element *e,
		const char *event_name,
		void *event_data, void *data)
{
	Esvg_Element_Gradient *thiz = data;

	thiz->stops_changed = EINA_TRUE;
}

static void _esvg_element_gradient_deep_units_get(Esvg_Element_Gradient *thiz,
		Esvg_Element_Gradient_Units *units)
{
	if (!thiz->units.is_set && thiz->href_e)
	{
		Esvg_Element_Gradient *other;

		other = _esvg_element_gradient_get(thiz->href_t);
		_esvg_element_gradient_deep_units_get(other, units);
	}
	else
		*units = thiz->units.v;
}

static void _esvg_element_gradient_deep_transform_get(Esvg_Element_Gradient *thiz,
		Enesim_Matrix *transform)
{
	if (!esvg_attribute_animated_transform_is_set(&thiz->transform) && thiz->href_e)
	{
		Esvg_Element_Gradient *other;

		other = _esvg_element_gradient_get(thiz->href_t);
		_esvg_element_gradient_deep_transform_get(other, transform);
	}
	else
	{
		esvg_attribute_animated_transform_final_get(&thiz->transform, transform);
	}
}

static void _esvg_element_gradient_deep_spread_method_get(Esvg_Element_Gradient *thiz, 
		Esvg_Spread_Method *spread_method)
{
	if (!thiz->spread_method.is_set && thiz->href_e)
	{
		Esvg_Element_Gradient *other;

		other = _esvg_element_gradient_get(thiz->href_t);
		_esvg_element_gradient_deep_spread_method_get(other, spread_method);
	}
	else
		*spread_method = thiz->spread_method.v;
}

static void _esvg_element_gradient_context_generate(Esvg_Element_Gradient *thiz, Esvg_Element_Gradient_Context *ctx)
{
	/* if no href set just use our own properties */
	_esvg_element_gradient_deep_units_get(thiz, &ctx->units);
	_esvg_element_gradient_deep_spread_method_get(thiz, &ctx->spread_method);
	_esvg_element_gradient_deep_transform_get(thiz, &ctx->transform);
}

static Eina_Bool _esvg_element_gradient_stop_propagate(Edom_Tag *t, Edom_Tag *child,
		void *user_data)
{
	Esvg_Type type;
	Enesim_Renderer_Gradient_Stop *stop;
	Enesim_Renderer *r = user_data;

	/* only propagate stops */
	type = esvg_element_internal_type_get(child);
	if (type != ESVG_TYPE_STOP) return EINA_TRUE;

	stop = esvg_element_stop_gradient_stop_get(child);
	DBG("Adding a gradient stop at position %g with color %08x", stop->pos, stop->argb);
	enesim_renderer_gradient_stop_add(r, stop);
	return EINA_TRUE;
}

static void _esvg_element_gradient_stop_generate(Edom_Tag *t, Enesim_Renderer *r)
{
	Esvg_Element_Gradient *thiz;
	Edom_Tag *child;

	thiz = _esvg_element_gradient_get(t);
	/* clear all the stops */
	enesim_renderer_gradient_stop_clear(r);
	/* check if we need to generate our own childs or the other gradient childs */
	child = edom_tag_child_get(t);
	if (!child && thiz->href_t)
	{
		DBG("Generating relative stops in '%s' from '%s'",
				esvg_element_internal_id_get(t),
				thiz->current.href);
		_esvg_element_gradient_stop_generate(thiz->href_t, r);
	}
	else
	{
		DBG("Generating local stops");
		/* TODO we need to do the setup the stops as those
		 * might not have done the setup yet
		 */
		//esvg_element_internal_child_setup(t, c, error, _esvg_element_svg_setup_interceptor, thiz);
		edom_tag_child_foreach(t, _esvg_element_gradient_stop_propagate, r);
	}
}
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_element_gradient_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "xlink:href") == 0)
	{
		esvg_element_gradient_href_set(e, value);
	}
	else if (!strcmp(key, "gradientUnits"))
	{
		Esvg_Element_Gradient_Units units;
		esvg_parser_gradient_units_string_from(&units, value);
		esvg_element_gradient_units_set(e, units);
	}
	else if (strcmp(key, "gradientTransform") == 0)
	{
		Enesim_Matrix matrix;

		esvg_transformation_string_from(&matrix, value);
		esvg_element_gradient_transform_set(e, &matrix);
	}
	else if (strcmp(key, "spreadMethod") == 0)
	{
		Esvg_Spread_Method smethod;

		esvg_parser_spread_method_get(&smethod, value);
		esvg_element_gradient_spread_method_set(e, smethod);
	}
	else
	{
		Esvg_Element_Gradient *thiz;
		Edom_Tag *t;

		t = ender_element_object_get(e);
		thiz = _esvg_element_gradient_get(t);
		if (thiz->descriptor.attribute_set)
			return thiz->descriptor.attribute_set(e, key, value);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static int * _esvg_element_gradient_attribute_animated_fetch(Edom_Tag *t, const char *name)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	/* FIXME provide our own */
	if (thiz->descriptor.attribute_animated_fetch)
		return thiz->descriptor.attribute_animated_fetch(t, name);
	return NULL;
}

static Eina_Bool _esvg_element_gradient_child_add(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Element_Gradient *thiz;
	Esvg_Type type;
	Ender_Element *child_e;

	if (!esvg_is_element_internal(child))
		return EINA_FALSE;

	thiz = _esvg_element_gradient_get(t);
	type = esvg_element_internal_type_get(child);
	/* only support stops and animate variants */
	switch (type)
	{
		case ESVG_TYPE_STOP:
		case ESVG_TYPE_ANIMATE:
		break;

		default:
		ERR("Child %d not supported", type);
		return EINA_FALSE;
	}

	child_e = esvg_element_ender_get(child);
	ender_event_listener_add(child_e, "Mutation", _esvg_element_gradient_stop_mutation_cb, thiz);
	thiz->stops_changed = EINA_TRUE;

	return EINA_TRUE;
}

static Eina_Bool _esvg_element_gradient_child_remove(Edom_Tag *t, Edom_Tag *child)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	thiz->stops_changed = EINA_TRUE;
	// remove the event handler
	//ender_event_listener_remove(child_e, "Mutation", _esvg_element_gradient_stop_mutation_cb, thiz);
	return EINA_TRUE;
}

static Esvg_Element_Setup_Return _esvg_element_gradient_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Esvg_Element_Gradient *thiz;
	Eina_Bool child_ret;

	thiz = _esvg_element_gradient_get(t);
	/* in case the href attribute has changed we need to
	 * pick a new reference and use those stops in case
	 * we dont have any
	 */
	if (thiz->state_changed)
	{
		Esvg_Element_Setup_Return ret;
		Ender_Element *topmost;
		Ender_Element *href_e;
		Edom_Tag *href_t;

		/* TODO propagate our own properties in case those are not set
		 * or we better use some "deep" functions that should get the
		 * attribute from its href in case it has one
		 * we need to start adding real properties, not only the values
		 * but the is_set, all wrapped into a single struct
		 */
		if (thiz->href_e)
		{
			/* TODO remove the event handlers from the old href */
			//ender_element_unref(thiz->href_e);
			thiz->href_e = NULL;
			thiz->href_t = NULL;
		}
		esvg_element_internal_topmost_get(t, &topmost);
		if (!topmost)
			goto state_changed_done;

		if (!thiz->current.href)
			goto state_changed_done;

		DBG("Using '%s' to inherit properties and stops from", thiz->current.href);
		/* TODO add the event handlers on the new generate */
		esvg_element_svg_element_get(topmost, thiz->current.href, &href_e);
		if (!href_e)
		{
			WARN("Impossible to get the element '%s'", thiz->current.href);
			goto state_changed_done;
		}

		/* TODO check that the referring gradient is of the same type? */
		/* TODO check if this gradient has childs, if not also generate them */
		thiz->stops_changed = EINA_TRUE;
		thiz->href_e = href_e;
		thiz->href_t = ender_element_object_get(href_e);

		/* do the setup on the href */
		DBG("Doing the setup on the href tag");
		ret = esvg_element_internal_setup(thiz->href_t, c, error);
		/* if the other needs to queue, also queue ourselves
		 * if the other failed we also failed
		 */
		if (ret != ESVG_SETUP_OK)
			return ret;
	}
state_changed_done:
	if (!thiz->stops_changed)
		return ESVG_SETUP_OK;

	/* call the setup on the stops */
	DBG("Doing the setup on the gradient children");
	child_ret = esvg_element_internal_child_setup(t, c, error, NULL, NULL, thiz);
	if (!child_ret) return ESVG_SETUP_FAILED;
	return ESVG_SETUP_OK;
}

static void _esvg_element_gradient_cleanup(Edom_Tag *t)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	/* remove the flags */
	thiz->stops_changed = EINA_FALSE;
	/* swap the states */
	if (thiz->state_changed)
	{
		if (thiz->past.href)
		{
			free(thiz->past.href);
			thiz->past.href = NULL;
		}
		if (thiz->current.href)
		{
			thiz->past.href = strdup(thiz->current.href);
		}
		thiz->state_changed = EINA_FALSE;
	}
}

static Eina_Bool _esvg_element_gradient_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		void *data,
		Enesim_Log **error)
{
	Esvg_Element_Gradient *thiz;
	Esvg_Element_Gradient_Context gctx;
	Eina_Bool ret = EINA_TRUE;
	Enesim_Renderer *r = data;

	thiz = _esvg_element_gradient_get(t);
	if (thiz->stops_changed)
	{
		_esvg_element_gradient_stop_generate(t, r);
	}
	/* generate the context */
	_esvg_element_gradient_context_generate(thiz, &gctx);

	if (thiz->descriptor.propagate)
		ret = thiz->descriptor.propagate(t, c, ctx, attr, &gctx, r, error);
	return ret;
}

static Eina_Bool _esvg_element_gradient_reference_add(Edom_Tag *t, Esvg_Referenceable_Reference *rr)
{
	Esvg_Element_Gradient *thiz;
	Enesim_Renderer *r;

	thiz = _esvg_element_gradient_get(t);
	/* in case some stop has changed:
	 *  the setup will be called
	 *  the stops will be generated
	 *  the stops will be propagated to each reference
	 * so no need to do twice
	 */
	if (thiz->stops_changed)
		return EINA_TRUE;
	if (thiz->state_changed)
		return EINA_TRUE;
	/* add every stop to our newly created reference */
	r = rr->data;
	_esvg_element_gradient_stop_generate(t, r);
	return EINA_TRUE;
}

static void _esvg_element_gradient_free(Edom_Tag *t)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _esvg_element_gradient_href_set(Edom_Tag *t, const char *href)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	ERR("Setting href = %s", href);
	if (thiz->current.href)
	{
		free(thiz->current.href);
		thiz->current.href = NULL;
	}

	if (href)
	{
		thiz->current.href = strdup(href);
	}
	thiz->state_changed = EINA_TRUE;
}

static void _esvg_element_gradient_href_get(Edom_Tag *t, const char **href)
{
	Esvg_Element_Gradient *thiz;

	if (!href) return;

	thiz = _esvg_element_gradient_get(t);
	*href = thiz->current.href;
}

static void _esvg_element_gradient_gradient_units_set(Edom_Tag *t, Esvg_Element_Gradient_Units units)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	thiz->units.v = units;
	thiz->units.is_set = EINA_TRUE;
}

static void _esvg_element_gradient_gradient_units_get(Edom_Tag *t, Esvg_Element_Gradient_Units *units)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	if (units) *units = thiz->units.v;
}

static Eina_Bool _esvg_element_gradient_gradient_units_is_set(Edom_Tag *t)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	return thiz->units.is_set;
}

static void _esvg_element_gradient_gradient_transform_set(Edom_Tag *t, const Esvg_Matrix_Animated *transform)
{
	Esvg_Element_Gradient *thiz;
	Enesim_Matrix m;
	Eina_Bool animating;

	thiz = _esvg_element_gradient_get(t);
	enesim_matrix_identity(&m);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_transform_set(&thiz->transform,
		transform, &m, animating);
}

static void _esvg_element_gradient_gradient_transform_get(Edom_Tag *t, Esvg_Matrix_Animated *transform)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	esvg_attribute_animated_transform_get(&thiz->transform, transform);
}

static Eina_Bool _esvg_element_gradient_gradient_transform_is_set(Edom_Tag *t)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	return esvg_attribute_animated_transform_is_set(&thiz->transform);
}

static void _esvg_element_gradient_spread_method_set(Edom_Tag *t, Esvg_Spread_Method spread_method)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	thiz->spread_method.v = spread_method;
}

static void _esvg_element_gradient_spread_method_get(Edom_Tag *t, Esvg_Spread_Method *spread_method)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	if (spread_method) *spread_method = thiz->spread_method.v;
}

#define _esvg_element_gradient_delete NULL
#define _esvg_element_gradient_href_is_set NULL
#define _esvg_element_gradient_spread_method_is_set NULL
#include "esvg_generated_element_gradient.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
void esvg_element_gradient_init(void)
{
	_esvg_element_gradient_log = eina_log_domain_register("esvg_element_gradient", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_gradient_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_gradient_init();
}

void esvg_element_gradient_shutdown(void)
{
	if (_esvg_element_gradient_log < 0)
		return;
	_esvg_element_gradient_shutdown();
	eina_log_domain_unregister(_esvg_element_gradient_log);
	_esvg_element_gradient_log = -1;
}

Edom_Tag * esvg_element_gradient_new(Esvg_Element_Gradient_Descriptor *descriptor,
		Esvg_Type type,
		void *data)
{
	Esvg_Element_Gradient *thiz;
	Esvg_Paint_Server_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Element_Gradient));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_ELEMENT_GRADIENT_MAGIC);
	thiz->descriptor.propagate = descriptor->propagate;
	thiz->descriptor.child_add = descriptor->child_add;
	thiz->descriptor.child_remove = descriptor->child_remove;
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->descriptor.attribute_animated_fetch = descriptor->attribute_animated_fetch;
	thiz->data = data;

	pdescriptor.child_add = _esvg_element_gradient_child_add;
	pdescriptor.child_remove = _esvg_element_gradient_child_remove;
	pdescriptor.attribute_set = _esvg_element_gradient_attribute_set;
	pdescriptor.attribute_animated_fetch = _esvg_element_gradient_attribute_animated_fetch;
	pdescriptor.attribute_get = descriptor->attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = NULL;
	pdescriptor.text_get = NULL;
	pdescriptor.free = _esvg_element_gradient_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = _esvg_element_gradient_setup;
	pdescriptor.cleanup = _esvg_element_gradient_cleanup;
	pdescriptor.renderer_new = descriptor->renderer_new;
	pdescriptor.propagate = _esvg_element_gradient_propagate;
	pdescriptor.reference_add = _esvg_element_gradient_reference_add;

	/* Default values */
	thiz->units.v = ESVG_OBJECT_BOUNDING_BOX;
	thiz->spread_method.v = ESVG_SPREAD_METHOD_PAD;
	enesim_matrix_identity(&thiz->transform.base.v);
	enesim_matrix_identity(&thiz->transform.anim.v);

	t = esvg_paint_server_new(&pdescriptor, type, thiz);
	return t;
}

void * esvg_element_gradient_data_get(Edom_Tag *t)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	return thiz->data;
}

Edom_Tag * esvg_element_gradient_href_tag_get(Edom_Tag *t)
{
	Esvg_Element_Gradient *thiz;

	thiz = _esvg_element_gradient_get(t);
	return thiz->href_t;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_is_gradient(Ender_Element *e)
{
	/* for now return false */
	return EINA_FALSE;
}

EAPI void esvg_element_gradient_href_set(Ender_Element *e, const char *href)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_HREF, href, NULL);
}

EAPI void esvg_element_gradient_href_get(Ender_Element *e, const char **href)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_gradient_href_get(t, href);
}

EAPI void esvg_element_gradient_units_set(Ender_Element *e, Esvg_Element_Gradient_Units units)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_GRADIENT_UNITS, units, NULL);
}

EAPI void esvg_element_gradient_units_get(Ender_Element *e, Esvg_Element_Gradient_Units *units)
{
}

EAPI Eina_Bool esvg_element_gradient_units_is_set(Ender_Element *e)
{
	/* for now return false */
	return EINA_FALSE;
}

EAPI void esvg_element_gradient_transform_set(Ender_Element *e, const Enesim_Matrix *transform)
{
	Esvg_Matrix_Animated a;

	if (!transform)
	{
		ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_GRADIENT_TRANSFORM, NULL, NULL);
		return;
	}
	a.base = *transform;
	ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_GRADIENT_TRANSFORM, &a, NULL);
}

EAPI void esvg_element_gradient_transform_get(Ender_Element *e, Enesim_Matrix *transform)
{
}

EAPI Eina_Bool esvg_element_gradient_transform_is_set(Ender_Element *e)
{
	/* for now return false */
	return EINA_FALSE;
}

EAPI void esvg_element_gradient_spread_method_set(Ender_Element *e, Esvg_Spread_Method spread_method)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_SPREAD_METHOD, spread_method, NULL);
}

EAPI void esvg_element_gradient_spread_method_get(Ender_Element *e, Esvg_Spread_Method *spread_method)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_gradient_spread_method_get(t, spread_method);
}

