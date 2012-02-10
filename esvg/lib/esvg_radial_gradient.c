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
#define ESVG_RADIAL_GRADIENT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_RADIAL_GRADIENT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_RADIAL_GRADIENT_MAGIC);\
	} while(0)

typedef struct _Esvg_Radial_Gradient
{
	EINA_MAGIC
	/* properties */
	Esvg_Gradient_Units units;
	Esvg_Coord cx;
	Esvg_Coord cy;
	Esvg_Coord fx;
	Esvg_Coord fy;
	Esvg_Length rad;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool cx_is_set : 1;
	Eina_Bool cy_is_set : 1;
	Eina_Bool fx_is_set : 1;
	Eina_Bool fy_is_set : 1;
	Eina_Bool rad_is_set : 1;
} Esvg_Radial_Gradient;

static Esvg_Radial_Gradient * _esvg_radial_gradient_get(Enesim_Renderer *r)
{
	Esvg_Radial_Gradient *thiz;

	thiz = esvg_gradient_data_get(r);
	ESVG_RADIAL_GRADIENT_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
static const char * _esvg_radial_gradient_name_get(Enesim_Renderer *r)
{
	return "radial_gradient";
}

static Enesim_Renderer * _radial_gradient_renderer_get(Enesim_Renderer *r,
		const Esvg_Element_State *state,
		const Esvg_Attribute_Presentation *attr)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	return thiz->r;
}

static void _radial_gradient_clone(Enesim_Renderer *r, Enesim_Renderer *rr)
{

}

static Eina_Bool _radial_gradient_setup(Enesim_Renderer *r,
		const Esvg_Element_State *state,
		Enesim_Renderer *rel,
		const Esvg_Gradient_State *gstate)
{
	Esvg_Radial_Gradient *thiz;
	Esvg_Gradient_Stop *stop;
	Esvg_Gradient_Units gu;
	Enesim_Repeat_Mode mode;
	Eina_List *l;
	double cx;
	double cy;
	double fx;
	double fy;
	double rad;

	thiz = _esvg_radial_gradient_get(r);

	gu = gstate->units;
	switch (gstate->spread_method)
	{
		case ESVG_SPREAD_METHOD_PAD:
		mode = ENESIM_PAD;
		break;

		case ESVG_SPREAD_METHOD_REPEAT:
		mode = ENESIM_REPEAT;
		break;

		case ESVG_SPREAD_METHOD_REFLECT:
		mode = ENESIM_REFLECT;
		break;
	}
	enesim_renderer_gradient_mode_set(thiz->r, mode);

	if (gu == ESVG_OBJECT_BOUNDING_BOX)
	{
		Eina_Rectangle bbox;

		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		cx = esvg_length_final_get(&thiz->cx, 1);
		cy = esvg_length_final_get(&thiz->cy, 1);
		rad = esvg_length_final_get(&thiz->rad, 1);

		enesim_renderer_destination_boundings(rel, &bbox, 0, 0);
		cx = cx * (bbox.w) - bbox.x;
		cy = cy * (bbox.h) - bbox.y;

		/* fx and fy should be the 0% stop? */
		/* cx and cy + r should be the 100% stop */
	}
	else
	{
		double w;
		double h;

		/* use the user space coordiantes */
		w = state->viewbox_w;
		h = state->viewbox_h;

		cx = esvg_length_final_get(&thiz->cx, w);
		cy = esvg_length_final_get(&thiz->cy, h);
		rad = esvg_length_final_get(&thiz->rad, w);
	}


	/* FIXME for now we dont handle the focis */
	enesim_renderer_gradient_radial_center_x_set(thiz->r, cx);
	enesim_renderer_gradient_radial_center_y_set(thiz->r, cy);
	enesim_renderer_gradient_radial_radius_y_set(thiz->r, rad);
	enesim_renderer_gradient_radial_radius_x_set(thiz->r, rad);

	EINA_LIST_FOREACH(gstate->stops, l, stop)
	{
		Enesim_Renderer_Gradient_Stop s;

		enesim_argb_components_from(&s.argb, lrint(stop->stop_opacity * 255),
				stop->stop_color.r, stop->stop_color.g, stop->stop_color.b);
		if (stop->offset.unit == ESVG_UNIT_LENGTH_PERCENT)
			s.pos = stop->offset.value / 100.0;
		else
		{
			if (stop->offset.value > 1)
				s.pos = 1;
			else if (stop->offset.value < 0)
				s.pos = 0;
			else
				s.pos = stop->offset.value;
		}
		printf("color = %08x pos = %g\n", s.argb, s.pos);
		enesim_renderer_gradient_stop_add(thiz->r, &s);
	}
	/* TODO set the transformation geometry */
	{
		Enesim_Matrix m;

		enesim_matrix_identity(&m);
		if (gu != ESVG_OBJECT_BOUNDING_BOX)
		{
			Enesim_Matrix m;

			enesim_matrix_inverse(&state->transform, &m);
		}

		if (enesim_matrix_type_get(&gstate->transform) != ENESIM_MATRIX_IDENTITY)
		{
			Enesim_Matrix gm;

			enesim_matrix_inverse(&gstate->transform, &gm);
			enesim_matrix_compose(&gm, &m, &m);
		}
		enesim_renderer_transformation_set(thiz->r, &m);
	}

	return EINA_TRUE;
}

static Esvg_Gradient_Descriptor _descriptor = {
	/* .setup =		*/ _radial_gradient_setup,
	/* .name_get =		*/ _esvg_radial_gradient_name_get,
	/* .renderer_get =	*/ _radial_gradient_renderer_get,
	/* .clone =		*/ _radial_gradient_clone,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_radial_gradient_new(void)
{
	Esvg_Radial_Gradient *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Radial_Gradient));

	EINA_MAGIC_SET(thiz, ESVG_RADIAL_GRADIENT_MAGIC);

	r = enesim_renderer_gradient_radial_new();
	enesim_renderer_gradient_mode_set(r, ENESIM_PAD);
	thiz->r = r;
	/* default values */

	r = esvg_gradient_new(&_descriptor, thiz);

	return r;
}

EAPI Eina_Bool esvg_is_radial_gradient(Enesim_Renderer *r)
{
	Esvg_Radial_Gradient *thiz;
	Eina_Bool ret;

	if (!esvg_is_gradient(r))
		return EINA_FALSE;
	thiz = esvg_gradient_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_RADIAL_GRADIENT_MAGIC);

	return ret;
}

EAPI void esvg_radial_gradient_cx_set(Enesim_Renderer *r, const Esvg_Coord *cx)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	if (!cx)
	{
		thiz->cx_is_set = EINA_FALSE;
	}
	else
	{
		thiz->cx = *cx;
		thiz->cx_is_set = EINA_TRUE;
	}
}

EAPI void esvg_radial_gradient_cx_get(Enesim_Renderer *r, Esvg_Coord *cx)
{
	Esvg_Radial_Gradient *thiz;

	if (!cx) return;
	thiz = _esvg_radial_gradient_get(r);
	*cx = thiz->cx;
}

EAPI Eina_Bool esvg_radial_gradient_cx_is_set(Enesim_Renderer *r)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	return thiz->cx_is_set;
}

EAPI void esvg_radial_gradient_cy_set(Enesim_Renderer *r, const Esvg_Coord *cy)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	if (!cy)
	{
		thiz->cy_is_set = EINA_FALSE;
	}
	else
	{
		thiz->cy = *cy;
		thiz->cy_is_set = EINA_TRUE;
	}
}

EAPI void esvg_radial_gradient_cy_get(Enesim_Renderer *r, Esvg_Coord *cy)
{
	Esvg_Radial_Gradient *thiz;

	if (!cy) return;
	thiz = _esvg_radial_gradient_get(r);
	*cy = thiz->cy;
}

EAPI Eina_Bool esvg_radial_gradient_cy_is_set(Enesim_Renderer *r)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	return thiz->cy_is_set;
}

EAPI void esvg_radial_gradient_fx_set(Enesim_Renderer *r, const Esvg_Coord *fx)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	if (!fx)
	{
		thiz->fx_is_set = EINA_FALSE;
	}
	else
	{
		thiz->fx = *fx;
		thiz->fx_is_set = EINA_TRUE;
	}
}

EAPI void esvg_radial_gradient_fx_get(Enesim_Renderer *r, Esvg_Coord *fx)
{
	Esvg_Radial_Gradient *thiz;

	if (!fx) return;
	thiz = _esvg_radial_gradient_get(r);
	*fx = thiz->fx;
}

EAPI Eina_Bool esvg_radial_gradient_fx_is_set(Enesim_Renderer *r)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	return thiz->fx_is_set;
}

EAPI void esvg_radial_gradient_fy_set(Enesim_Renderer *r, const Esvg_Coord *fy)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	if (!fy)
	{
		thiz->fy_is_set = EINA_FALSE;
	}
	else
	{
		thiz->fy = *fy;
		thiz->fy_is_set = EINA_TRUE;
	}
}

EAPI void esvg_radial_gradient_fy_get(Enesim_Renderer *r, Esvg_Coord *fy)
{
	Esvg_Radial_Gradient *thiz;

	if (!fy) return;
	thiz = _esvg_radial_gradient_get(r);
	*fy = thiz->fy;
}

EAPI Eina_Bool esvg_radial_gradient_fy_is_set(Enesim_Renderer *r)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	return thiz->fy_is_set;
}

EAPI void esvg_radial_gradient_r_set(Enesim_Renderer *r, const Esvg_Length *rad)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	if (!rad)
	{
		thiz->rad_is_set = EINA_FALSE;
	}
	else
	{
		thiz->rad = *rad;
		thiz->rad_is_set = EINA_TRUE;
	}
}

EAPI void esvg_radial_gradient_r_get(Enesim_Renderer *r, Esvg_Length *rad)
{
	Esvg_Radial_Gradient *thiz;

	if (!rad) return;
	thiz = _esvg_radial_gradient_get(r);
	*rad = thiz->rad;
}

EAPI Eina_Bool esvg_radial_gradient_r_is_set(Enesim_Renderer *r)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(r);
	return thiz->rad_is_set;
}
