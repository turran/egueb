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
#define ESVG_CIRCLE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_CIRCLE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_CIRCLE_MAGIC);\
	} while(0)

typedef struct _Esvg_Circle_State
{
	Esvg_Coord cx;
	Esvg_Coord cy;
	Esvg_Length radius;
} Esvg_Circle_State;

typedef struct _Esvg_Circle
{
	EINA_MAGIC
	/* properties */
	Esvg_Circle_State current;
	Esvg_Circle_State past;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Circle;

static Esvg_Circle * _esvg_circle_get(Enesim_Renderer *r)
{
	Esvg_Circle *thiz;

	thiz = esvg_shape_data_get(r);
	ESVG_CIRCLE_MAGIC_CHECK(thiz);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The ESVG element interface                         *
 *----------------------------------------------------------------------------*/
static const char * _esvg_circle_name_get(Enesim_Renderer *r)
{
	return "circle";
}

static Enesim_Renderer * _esvg_circle_renderer_get(Enesim_Renderer *r)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(r);
	return thiz->r;
}

static Eina_Bool _esvg_circle_setup(Enesim_Renderer *r, const Esvg_Element_State *estate,
		const Esvg_Shape_Enesim_State *dstate)
{
	Esvg_Circle *thiz;
	double cx, cy;
	double radius;

	thiz = _esvg_circle_get(r);

	/* FIXME gets the parents size or the topmost? */
	/* set the origin */
	cx = esvg_length_final_get(&thiz->current.cx, estate->viewbox_w);
	cy = esvg_length_final_get(&thiz->current.cy, estate->viewbox_h);
	/* set the size */
	radius = esvg_length_final_get(&thiz->current.radius, estate->viewbox_w);
	printf("calling the setup on the circle (%g %g %g)\n", cx, cy, radius);
	enesim_renderer_circle_center_set(thiz->r, cx, cy);
	enesim_renderer_circle_radius_set(thiz->r, radius);

	/* shape properties */
	if (!dstate->fill_renderer)
		enesim_renderer_shape_fill_color_set(thiz->r, dstate->fill_color);
	else
		enesim_renderer_shape_fill_renderer_set(thiz->r, dstate->fill_renderer);
	if (!dstate->stroke_renderer)
		enesim_renderer_shape_stroke_color_set(thiz->r, dstate->stroke_color);
	else
		enesim_renderer_shape_stroke_renderer_set(thiz->r, dstate->stroke_renderer);

	enesim_renderer_shape_stroke_weight_set(thiz->r, dstate->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_draw_mode_set(thiz->r, dstate->draw_mode);

	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &estate->transform);
	enesim_renderer_color_set(thiz->r, dstate->color);

	return EINA_TRUE;
}

static void _esvg_circle_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{
	Esvg_Circle *thiz;
	Esvg_Circle *other;

	thiz = _esvg_circle_get(r);
	other = _esvg_circle_get(dr);

	other->current.cx = thiz->current.cx;
	other->current.cy = thiz->current.cy;
	other->current.radius = thiz->current.radius;
}

static void _esvg_circle_cleanup(Enesim_Renderer *r)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(r);
	thiz->past = thiz->current;
	thiz->changed = EINA_FALSE;
}

static Eina_Bool _esvg_circle_has_changed(Enesim_Renderer *r)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(r);
	if (!thiz->changed) return EINA_FALSE;

	if (esvg_length_is_equal(&thiz->current.cx, &thiz->past.cx))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.cy, &thiz->past.cy))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.radius, &thiz->past.radius))
		return EINA_TRUE;

	return EINA_FALSE;
}

static Esvg_Shape_Descriptor _descriptor = {
	/* .setup =		*/ _esvg_circle_setup,
	/* .renderer_get =	*/ _esvg_circle_renderer_get,
	/* .name_get =		*/ _esvg_circle_name_get,
	/* .clone =		*/ _esvg_circle_clone,
	/* .cleanup =		*/ _esvg_circle_cleanup,
	/* .has_changed	=	*/ _esvg_circle_has_changed
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_circle_new(void)
{
	Esvg_Circle *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Circle));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_CIRCLE_MAGIC);

	r = enesim_renderer_circle_new();
	/* FIXME for now */
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->r = r;

	/* Default values */
	thiz->current.cx = ESVG_COORD_0;
	thiz->current.cy = ESVG_COORD_0;
	thiz->current.radius = ESVG_LENGTH_0;

	r = esvg_shape_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool esvg_is_circle(Enesim_Renderer *r)
{
	Esvg_Circle *thiz;
	Eina_Bool ret;

	if (!esvg_is_shape(r))
		return EINA_FALSE;
	thiz = esvg_shape_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_CIRCLE_MAGIC);

	return ret;
}

EAPI void esvg_circle_cx_set(Enesim_Renderer *r, const Esvg_Coord *cx)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(r);
	if (cx)
	{
		thiz->current.cx = *cx;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_circle_cx_get(Enesim_Renderer *r, Esvg_Coord *cx)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(r);
	if (cx) *cx = thiz->current.cx;
}

EAPI void esvg_circle_cy_set(Enesim_Renderer *r, const Esvg_Coord *cy)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(r);
	if (cy)
	{
		thiz->current.cy = *cy;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_circle_cy_get(Enesim_Renderer *r, Esvg_Coord *cy)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(r);
	if (cy) *cy = thiz->current.cy;
}

EAPI void esvg_circle_radius_set(Enesim_Renderer *r, const Esvg_Length *radius)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(r);
	if (radius)
	{
		thiz->current.radius = *radius;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_circle_radius_get(Enesim_Renderer *r, Esvg_Length *radius)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(r);
	if (radius) *radius = thiz->current.radius;
}
