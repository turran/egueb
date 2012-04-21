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
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_private_paint_server.h"
#include "esvg_private_gradient.h"
#include "esvg_gradient.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_GRADIENT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_GRADIENT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_GRADIENT_MAGIC);\
	} while(0)

static Ender_Property *ESVG_GRADIENT_STOP;

typedef struct _Esvg_Gradient_Descriptor_Internal
{
	Edom_Tag_Free free;
	Edom_Tag_Child_Add child_add;
	Esvg_Gradient_Setup setup;
	Esvg_Renderable_Renderer_Get renderer_get;
} Esvg_Gradient_Descriptor_Internal;

typedef struct _Esvg_Gradient
{
	EINA_MAGIC
	/* properties */
	Esvg_Gradient_Context state;
	/* private */
	Esvg_Gradient_Descriptor_Internal descriptor;
	Eina_Bool units_set : 1;
	Eina_Bool transform_is_set : 1;
	void *data;
} Esvg_Gradient;

static Esvg_Gradient * _esvg_gradient_get(Edom_Tag *t)
{
	Esvg_Gradient *thiz;

	thiz = esvg_paint_server_data_get(t);
	ESVG_GRADIENT_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_gradient_child_add(Edom_Tag *t, Edom_Tag *child_t)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	/* only support stops */
	printf("adding child\n");
	if (1)
	{
		return EINA_TRUE;
	}
	else if (thiz->descriptor.child_add)
		return thiz->descriptor.child_add(t, child_t);
	else
		return EINA_FALSE;
}

static Eina_Bool _esvg_gradient_setup(Edom_Tag *t,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (thiz->descriptor.setup)
		thiz->descriptor.setup(t, ctx, rctx, &thiz->state, error);
	return EINA_TRUE;
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
#if 0
static void _esvg_gradient_stop_add(Edom_Tag *t, Esvg_Gradient_Stop *s)
{
	Esvg_Gradient *thiz;
	Esvg_Gradient_Stop *stop;

	if (!s) return;

	thiz = _esvg_gradient_get(t);

	stop = malloc(sizeof(Esvg_Gradient_Stop));
	*stop = *s;
	thiz->state.stops = eina_list_append(thiz->state.stops, stop);
}

static void _esvg_gradient_stop_get(Edom_Tag *t, const Eina_List **l)
{
	Esvg_Gradient *thiz;

	if (!l) return;

	thiz = _esvg_gradient_get(t);
	*l = thiz->state.stops;
}
#endif

static void _esvg_gradient_units_set(Edom_Tag *t, Esvg_Gradient_Units units)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	thiz->state.units = units;
	thiz->units_set = EINA_TRUE;
}

static void _esvg_gradient_units_get(Edom_Tag *t, Esvg_Gradient_Units *units)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (units) *units = thiz->state.units;
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
	if (transform) thiz->state.transform = *transform;
}

static void _esvg_gradient_transform_get(Edom_Tag *t, Enesim_Matrix *transform)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (transform) *transform = thiz->state.transform;
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
	thiz->state.spread_method = spread_method;
}

static void _esvg_gradient_spread_method_get(Edom_Tag *t, Esvg_Spread_Method *spread_method)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(t);
	if (spread_method) *spread_method = thiz->state.spread_method;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
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
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.child_add = descriptor->child_add;
	thiz->data = data;

	pdescriptor.child_add = _esvg_gradient_child_add;
	pdescriptor.child_remove = descriptor->child_remove;
	pdescriptor.attribute_set = descriptor->attribute_set;
	pdescriptor.attribute_get = descriptor->attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.free = _esvg_gradient_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = _esvg_gradient_setup;
	pdescriptor.renderer_get = descriptor->renderer_get;

	/* Default values */
	thiz->state.units = ESVG_OBJECT_BOUNDING_BOX;
	enesim_matrix_identity(&thiz->state.transform);

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

EAPI void esvg_gradient_stop_add(Ender_Element *e, Ender_Element *s)
{
}

EAPI void esvg_gradient_stop_get(Ender_Element *e, const Eina_List **l)
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

