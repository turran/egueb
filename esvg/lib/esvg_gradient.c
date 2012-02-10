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
#include "Esvg.h"
#include "esvg_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_GRADIENT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_GRADIENT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_GRADIENT_MAGIC);\
	} while(0)

typedef struct _Esvg_Gradient
{
	EINA_MAGIC
	Esvg_Gradient_State state;
	Esvg_Gradient_Setup setup;
	Eina_Bool units_set : 1;
	Eina_Bool transform_is_set : 1;
	void *data;
} Esvg_Gradient;
/*----------------------------------------------------------------------------*
 *                       The radial gradient paint server                     *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                       The linear gradient paint server                     *
 *----------------------------------------------------------------------------*/
static Esvg_Gradient * _esvg_gradient_get(Enesim_Renderer *r)
{
	Esvg_Gradient *thiz;

	thiz = esvg_paint_server_data_get(r);
	ESVG_GRADIENT_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         Esvg Element interface                             *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_gradient_setup(Enesim_Renderer *r,
		const Enesim_Renderer_State *state,
		Enesim_Renderer *rel)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	thiz->setup(r, state, rel, &thiz->state);
	return EINA_TRUE;
}

static void _esvg_gradient_cleanup(Enesim_Renderer *r)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	/* FIXME for later */
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Enesim_Renderer * esvg_gradient_new(Esvg_Gradient_Descriptor *descriptor,
		void *data)
{
	Esvg_Gradient *thiz;
	Esvg_Paint_Server_Descriptor pdescriptor;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Gradient));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_GRADIENT_MAGIC);
	thiz->setup = descriptor->setup;
	thiz->data = data;

	pdescriptor.name_get = descriptor->name_get;
	pdescriptor.renderer_get = descriptor->renderer_get;
	pdescriptor.setup = _esvg_gradient_setup;
	pdescriptor.clone = descriptor->clone;

	/* Default values */
	thiz->state.units = ESVG_OBJECT_BOUNDING_BOX;
	enesim_matrix_identity(&thiz->state.transform);

	r = esvg_paint_server_new(&pdescriptor, thiz);
	return r;
}

void * esvg_gradient_data_get(Enesim_Renderer *r)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	return thiz->data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_is_gradient(Enesim_Renderer *r)
{
	Esvg_Gradient *thiz;
	Eina_Bool ret;

	if (!esvg_is_paint_server(r))
		return EINA_FALSE;
	thiz = esvg_paint_server_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_GRADIENT_MAGIC);

	return ret;
}

EAPI void esvg_gradient_stop_add(Enesim_Renderer *r, Esvg_Gradient_Stop *s)
{
	Esvg_Gradient *thiz;
	Esvg_Gradient_Stop *stop;

	if (!s) return;

	thiz = _esvg_gradient_get(r);

	stop = malloc(sizeof(Esvg_Gradient_Stop));
	*stop = *s;
	thiz->state.stops = eina_list_append(thiz->state.stops, stop);
}

EAPI void esvg_gradient_stop_get(Enesim_Renderer *r, const Eina_List **l)
{
	Esvg_Gradient *thiz;

	if (!l) return;

	thiz = _esvg_gradient_get(r);
	*l = thiz->state.stops;
}

EAPI void esvg_gradient_units_set(Enesim_Renderer *r, Esvg_Gradient_Units units)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	thiz->state.units = units;
	thiz->units_set = EINA_TRUE;
}

EAPI void esvg_gradient_units_get(Enesim_Renderer *r, Esvg_Gradient_Units *units)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	if (units) *units = thiz->state.units;
}

EAPI Eina_Bool esvg_gradient_units_is_set(Enesim_Renderer *r)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	return thiz->units_set;
}

EAPI void esvg_gradient_transform_set(Enesim_Renderer *r, const Enesim_Matrix *transform)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	if (transform) thiz->state.transform = *transform;
}

EAPI void esvg_gradient_transform_get(Enesim_Renderer *r, Enesim_Matrix *transform)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	if (transform) *transform = thiz->state.transform;
}

EAPI Eina_Bool esvg_gradient_transform_is_set(Enesim_Renderer *r)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	return thiz->transform_is_set;
}

EAPI void esvg_gradient_spread_method_set(Enesim_Renderer *r, Esvg_Spread_Method spread_method)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	thiz->state.spread_method = spread_method;
}

EAPI void esvg_gradient_spread_method_get(Enesim_Renderer *r, Esvg_Spread_Method *spread_method)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	if (spread_method) *spread_method = thiz->state.spread_method;
}

