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
#define ESVG_ELLIPSE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_ELLIPSE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_ELLIPSE_MAGIC);\
	} while(0)

typedef struct _Esvg_Ellipse_State
{
	Esvg_Coord cx;
	Esvg_Coord cy;
	Esvg_Length rx;
	Esvg_Length ry;
} Esvg_Ellipse_State;

typedef struct _Esvg_Ellipse
{
	EINA_MAGIC
	/* properties */
	Esvg_Ellipse_State current;
	Esvg_Ellipse_State past;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Ellipse;

static Esvg_Ellipse * _esvg_ellipse_get(Enesim_Renderer *r)
{
	Esvg_Ellipse *thiz;

	thiz = esvg_shape_data_get(r);
	ESVG_ELLIPSE_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         The ESVG element interface                         *
 *----------------------------------------------------------------------------*/
static const char * _esvg_ellipse_name_get(Enesim_Renderer *r)
{
	return "ellipse";
}

static Enesim_Renderer * _esvg_ellipse_renderer_get(Enesim_Renderer *r)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	return thiz->r;
}

static Eina_Bool _esvg_ellipse_setup(Enesim_Renderer *r, const Esvg_Element_State *estate,
		const Esvg_Shape_Enesim_State *dstate)
{
	Esvg_Ellipse *thiz;
	double cx, cy;
	double rx, ry;

	thiz = _esvg_ellipse_get(r);

	/* FIXME gets the parents size or the topmost? */
	/* set the origin */
	cx = esvg_length_final_get(&thiz->current.cx, estate->viewbox_w);
	cy = esvg_length_final_get(&thiz->current.cy, estate->viewbox_h);
	/* set the size */
	rx = esvg_length_final_get(&thiz->current.rx, estate->viewbox_w);
	ry = esvg_length_final_get(&thiz->current.ry, estate->viewbox_h);
	//printf("calling the setup on the ellipse (%g %g %g %g)\n", cx, cy, rx, ry);
	enesim_renderer_ellipse_center_set(thiz->r, cx, cy);
	enesim_renderer_ellipse_radii_set(thiz->r, rx, ry);

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

static void _esvg_ellipse_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{
	Esvg_Ellipse *thiz;
	Esvg_Ellipse *other;

	thiz = _esvg_ellipse_get(r);
	other = _esvg_ellipse_get(dr);

	other->current.cx = thiz->current.cx;
	other->current.cy = thiz->current.cy;
	other->current.rx = thiz->current.rx;
	other->current.ry = thiz->current.ry;
}

static void _esvg_ellipse_cleanup(Enesim_Renderer *r)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	thiz->past = thiz->current;
	thiz->changed = EINA_FALSE;
}

static Eina_Bool _esvg_ellipse_has_changed(Enesim_Renderer *r)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (!thiz->changed) return EINA_FALSE;

	if (esvg_length_is_equal(&thiz->current.cx, &thiz->past.cx))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.cy, &thiz->past.cy))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.rx, &thiz->past.rx))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.ry, &thiz->past.ry))
		return EINA_TRUE;

	return EINA_FALSE;
}

static Esvg_Shape_Descriptor _descriptor = {
	/* .setup =		*/ _esvg_ellipse_setup,
	/* .renderer_get =	*/ _esvg_ellipse_renderer_get,
	/* .name_get =		*/ _esvg_ellipse_name_get,
	/* .clone =		*/ _esvg_ellipse_clone,
	/* .cleanup =		*/ _esvg_ellipse_cleanup,
	/* .has_changed =	*/ _esvg_ellipse_has_changed
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_ellipse_new(void)
{
	Esvg_Ellipse *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Ellipse));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_ELLIPSE_MAGIC);

	r = enesim_renderer_ellipse_new();
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->r = r;

	/* Default values */
	thiz->current.cx = ESVG_COORD_0;
	thiz->current.cy = ESVG_COORD_0;
	thiz->current.rx = ESVG_LENGTH_0;
	thiz->current.ry = ESVG_LENGTH_0;

	r = esvg_shape_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool esvg_is_ellipse(Enesim_Renderer *r)
{
	Esvg_Ellipse *thiz;
	Eina_Bool ret;

	if (!esvg_is_shape(r))
		return EINA_FALSE;
	thiz = esvg_shape_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_ELLIPSE_MAGIC);

	return ret;
}

EAPI void esvg_ellipse_cx_set(Enesim_Renderer *r, const Esvg_Coord *cx)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (cx)
	{
		thiz->current.cx = *cx;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_ellipse_cx_get(Enesim_Renderer *r, Esvg_Coord *cx)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (cx) *cx = thiz->current.cx;
}

EAPI void esvg_ellipse_cy_set(Enesim_Renderer *r, const Esvg_Coord *cy)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (cy)
	{
		thiz->current.cy = *cy;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_ellipse_cy_get(Enesim_Renderer *r, Esvg_Coord *cy)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (cy) *cy = thiz->current.cy;
}

EAPI void esvg_ellipse_rx_set(Enesim_Renderer *r, const Esvg_Length *rx)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (rx)
	{
		thiz->current.rx = *rx;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_ellipse_rx_get(Enesim_Renderer *r, Esvg_Length *rx)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (rx) *rx = thiz->current.rx;
}

EAPI void esvg_ellipse_ry_set(Enesim_Renderer *r, const Esvg_Length *ry)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (ry)
	{
		thiz->current.ry = *ry;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_ellipse_ry_get(Enesim_Renderer *r, Esvg_Length *ry)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (ry) *ry = thiz->current.ry;
}
