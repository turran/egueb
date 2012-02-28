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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LINE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_LINE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_LINE_MAGIC);\
	} while(0)

typedef struct _Esvg_Line_State
{
	Esvg_Coord x1;
	Esvg_Coord x2;
	Esvg_Coord y1;
	Esvg_Coord y2;
} Esvg_Line_State;

typedef struct _Esvg_Line
{
	EINA_MAGIC
	/* properties */
	Esvg_Line_State current;
	Esvg_Line_State past;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Line;

static Esvg_Line * _esvg_line_get(Enesim_Renderer *r)
{
	Esvg_Line *thiz;

	thiz = esvg_shape_data_get(r);
	ESVG_LINE_MAGIC_CHECK(thiz);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The ESVG element interface                         *
 *----------------------------------------------------------------------------*/
static const char * _esvg_line_name_get(Enesim_Renderer *r)
{
	return "line";
}

static Enesim_Renderer * _esvg_line_renderer_get(Enesim_Renderer *r)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	return thiz->r;
}

static Eina_Bool _esvg_line_setup(Enesim_Renderer *r, const Esvg_Element_State *estate,
		const Esvg_Shape_Enesim_State *dstate)
{
	Esvg_Line *thiz;
	double x1, y1, x2, y2;

	thiz = _esvg_line_get(r);

	/* FIXME gets the parents size or the topmost? */
	x1 = esvg_length_final_get(&thiz->current.x1, estate->viewbox_w);
	y1 = esvg_length_final_get(&thiz->current.y1, estate->viewbox_h);
	x2 = esvg_length_final_get(&thiz->current.x2, estate->viewbox_w);
	y2 = esvg_length_final_get(&thiz->current.y2, estate->viewbox_h);

	enesim_renderer_line_x0_set(thiz->r, x1);
	enesim_renderer_line_y0_set(thiz->r, y1);
	enesim_renderer_line_x1_set(thiz->r, x2);
	enesim_renderer_line_y1_set(thiz->r, y2);

	/* shape properties */
	enesim_renderer_shape_fill_color_set(thiz->r, dstate->fill_color);
	enesim_renderer_shape_stroke_color_set(thiz->r, dstate->stroke_color);
	enesim_renderer_shape_stroke_weight_set(thiz->r, dstate->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_stroke_cap_set(thiz->r, dstate->stroke_cap);
	enesim_renderer_shape_stroke_join_set(thiz->r, dstate->stroke_join);
	enesim_renderer_shape_draw_mode_set(thiz->r, dstate->draw_mode);

	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &estate->transform);
	enesim_renderer_color_set(thiz->r, dstate->color);

	//printf("calling the setup on the line (%g %g %g %g | %g %d)\n", x1, y1, x2, y2, dstate.stroke_weight, dstate.stroke_cap);
	return EINA_TRUE;
}

static void _esvg_line_cleanup(Enesim_Renderer *r)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	thiz->past = thiz->current;
	thiz->changed = EINA_FALSE;
}

static void _esvg_line_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{
	Esvg_Line *thiz;
	Esvg_Line *other;

	thiz = _esvg_line_get(r);
	other = _esvg_line_get(dr);

	other->current.x1 = thiz->current.x1;
	other->current.y1 = thiz->current.y1;
	other->current.x2 = thiz->current.x2;
	other->current.y2 = thiz->current.y2;
}

static Eina_Bool _esvg_line_has_changed(Enesim_Renderer *r)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	if (!thiz->changed) return EINA_FALSE;

	if (esvg_length_is_equal(&thiz->current.x1, &thiz->past.x1))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.x2, &thiz->past.x2))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.y1, &thiz->past.y1))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.y2, &thiz->past.y2))
		return EINA_TRUE;

	return EINA_FALSE;
}

static Esvg_Shape_Descriptor _descriptor = {
	/* .setup =		*/ _esvg_line_setup,
	/* .renderer_get =	*/ _esvg_line_renderer_get,
	/* .name_get =		*/ _esvg_line_name_get,
	/* .clone =		*/ _esvg_line_clone,
	/* .cleanup =		*/ _esvg_line_cleanup,
	/* .has_changed =	*/ _esvg_line_has_changed
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_line_new(void)
{
	Esvg_Line *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Line));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_LINE_MAGIC);

	r = enesim_renderer_line_new();
	thiz->r = r;
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	/* default values */
	/* FIXME defualt values */

	r = esvg_shape_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool esvg_is_line(Enesim_Renderer *r)
{
	Esvg_Line *thiz;
	Eina_Bool ret;

	if (!esvg_is_shape(r))
		return EINA_FALSE;
	thiz = esvg_shape_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_LINE_MAGIC);

	return ret;
}

EAPI void esvg_line_x1_set(Enesim_Renderer *r, const Esvg_Coord *x1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	if (x1)
	{
		thiz->current.x1 = *x1;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_line_x1_get(Enesim_Renderer *r, Esvg_Coord *x1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	if (x1) *x1 = thiz->current.x1;
}

EAPI void esvg_line_y1_set(Enesim_Renderer *r, const Esvg_Coord *y1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	if (y1)
	{
		thiz->current.y1 = *y1;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_line_y1_get(Enesim_Renderer *r, Esvg_Coord *y1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	if (y1) *y1 = thiz->current.y1;
}

EAPI void esvg_line_x2_set(Enesim_Renderer *r, const Esvg_Coord *x2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	if (x2)
	{
		thiz->current.x2 = *x2;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_line_x2_get(Enesim_Renderer *r, Esvg_Coord *x2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	if (x2) *x2 = thiz->current.x2;
}

EAPI void esvg_line_y2_set(Enesim_Renderer *r, const Esvg_Coord *y2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	if (y2)
	{
		thiz->current.y2 = *y2;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_line_y2_get(Enesim_Renderer *r, Esvg_Coord *y2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(r);
	if (y2) *y2 = thiz->current.y2;
}
