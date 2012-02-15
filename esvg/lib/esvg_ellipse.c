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

typedef struct _Esvg_Ellipse
{
	EINA_MAGIC
	/* properties */
	Esvg_Coord cx;
	Esvg_Coord cy;
	Esvg_Length rx;
	Esvg_Length ry;
	/* private */
	Enesim_Renderer *r;
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
	cx = esvg_length_final_get(&thiz->cx, estate->viewbox_w);
	cy = esvg_length_final_get(&thiz->cy, estate->viewbox_h);
	/* set the size */
	rx = esvg_length_final_get(&thiz->rx, estate->viewbox_w);
	ry = esvg_length_final_get(&thiz->ry, estate->viewbox_h);
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

	other->cx = thiz->cx;
	other->cy = thiz->cy;
	other->rx = thiz->rx;
	other->ry = thiz->ry;
}

static void _esvg_ellipse_cleanup(Enesim_Renderer *r)
{

}

static Esvg_Shape_Descriptor _descriptor = {
	/* .setup =		*/ _esvg_ellipse_setup,
	/* .renderer_get =	*/ _esvg_ellipse_renderer_get,
	/* .name_get =		*/ _esvg_ellipse_name_get,
	/* .clone =		*/ _esvg_ellipse_clone,
	/* .cleanup =		*/ _esvg_ellipse_cleanup,
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
	thiz->cx = ESVG_COORD_0;
	thiz->cy = ESVG_COORD_0;
	thiz->rx = ESVG_LENGTH_0;
	thiz->ry = ESVG_LENGTH_0;

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
	if (cx) thiz->cx = *cx;
}

EAPI void esvg_ellipse_cx_get(Enesim_Renderer *r, Esvg_Coord *cx)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (cx) *cx = thiz->cx;
}

EAPI void esvg_ellipse_cy_set(Enesim_Renderer *r, const Esvg_Coord *cy)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (cy) thiz->cy = *cy;
}

EAPI void esvg_ellipse_cy_get(Enesim_Renderer *r, Esvg_Coord *cy)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (cy) *cy = thiz->cy;
}

EAPI void esvg_ellipse_rx_set(Enesim_Renderer *r, const Esvg_Length *rx)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (rx) thiz->rx = *rx;
}

EAPI void esvg_ellipse_rx_get(Enesim_Renderer *r, Esvg_Length *rx)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (rx) *rx = thiz->rx;
}

EAPI void esvg_ellipse_ry_set(Enesim_Renderer *r, const Esvg_Length *ry)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (ry) thiz->ry = *ry;
}

EAPI void esvg_ellipse_ry_get(Enesim_Renderer *r, Esvg_Length *ry)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(r);
	if (ry) *ry = thiz->ry;
}
