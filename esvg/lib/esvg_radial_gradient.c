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
#include "esvg_private_stop.h"

#include "esvg_radial_gradient.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_RADIAL_GRADIENT_CX;
static Ender_Property *ESVG_RADIAL_GRADIENT_CY;
static Ender_Property *ESVG_RADIAL_GRADIENT_FX;
static Ender_Property *ESVG_RADIAL_GRADIENT_FY;
static Ender_Property *ESVG_RADIAL_GRADIENT_R;

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
	Eina_Bool cx_is_set : 1;
	Eina_Bool cy_is_set : 1;
	Eina_Bool fx_is_set : 1;
	Eina_Bool fy_is_set : 1;
	Eina_Bool rad_is_set : 1;
} Esvg_Radial_Gradient;

static Esvg_Radial_Gradient * _esvg_radial_gradient_get(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_RADIALGRADIENT)
		return NULL;
	thiz = esvg_gradient_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_radial_gradient_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "cx") == 0)
	{
		Esvg_Coord cx;

		esvg_length_string_from(&cx, value, ESVG_COORD_0);
		esvg_radial_gradient_cx_set(e, &cx);
	}
	else if (strcmp(key, "cy") == 0)
	{
		Esvg_Coord cy;

		esvg_length_string_from(&cy, value, ESVG_COORD_0);
		esvg_radial_gradient_cy_set(e, &cy);
	}
	else if (strcmp(key, "r") == 0)
	{
		Esvg_Length rad;

		esvg_length_string_from(&rad, value, ESVG_LENGTH_0);
		esvg_radial_gradient_r_set(e, &rad);
	}
	else if (strcmp(key, "fx") == 0)
	{
		Esvg_Length fx;

		esvg_length_string_from(&fx, value, ESVG_LENGTH_0);
		esvg_radial_gradient_fx_set(e, &fx);
	}
	else if (strcmp(key, "fy") == 0)
	{
		Esvg_Length fy;

		esvg_length_string_from(&fy, value, ESVG_LENGTH_0);
		esvg_radial_gradient_fy_set(e, &fy);
	}
	return EINA_TRUE;
}

static Eina_Bool _esvg_radial_gradient_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_radial_gradient_renderer_new(Edom_Tag *t)
{
	Enesim_Renderer *r;

	r = enesim_renderer_gradient_radial_new();
	enesim_renderer_gradient_mode_set(r, ENESIM_PAD);
	return r;
}

static Esvg_Element_Setup_Return _esvg_radial_gradient_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Renderer *r,
		Esvg_Gradient_Context *gctx,
		Enesim_Error **error)
{
	Esvg_Radial_Gradient *thiz;
	Esvg_Gradient_Units gu;
	Enesim_Repeat_Mode mode;
	Enesim_Matrix m;
	double cx;
	double cy;
	double fx;
	double fy;
	double rad;


	thiz = _esvg_radial_gradient_get(t);

	gu = gctx->units;
	switch (gctx->spread_method)
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
	enesim_renderer_gradient_mode_set(r, mode);

	if (gu == ESVG_OBJECT_BOUNDING_BOX)
	{
		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		cx = esvg_length_final_get(&thiz->cx, 1);
		cy = esvg_length_final_get(&thiz->cy, 1);
		fx = esvg_length_final_get(&thiz->fx, 1);
		fy = esvg_length_final_get(&thiz->fy, 1);
		rad = esvg_length_final_get(&thiz->rad, 1);
		enesim_matrix_values_set(&m, ctx->bounds.w, 0, ctx->bounds.x, 0, ctx->bounds.h, ctx->bounds.y, 0, 0, 1);
	}
	else
	{
		double w;
		double h;
		double rad_vp = 0;

		/* use the user space coordiantes */
		w = ctx->viewbox.width;
		h = ctx->viewbox.height;

		cx = esvg_length_final_get(&thiz->cx, w);
		cy = esvg_length_final_get(&thiz->cy, h);
		fx = esvg_length_final_get(&thiz->fx, w);
		fy = esvg_length_final_get(&thiz->fy, h);
		if (thiz->rad.unit == ESVG_UNIT_LENGTH_PERCENT)
		{
			rad_vp = hypot(w, h) / M_SQRT2;
		}
		rad = esvg_length_final_get(&thiz->rad, rad_vp);
		m = ctx->transform;
	}
	if (enesim_matrix_type_get(&gctx->transform) != ENESIM_MATRIX_IDENTITY)
	{
		enesim_matrix_compose(&m, &gctx->transform, &m);
	}
	enesim_renderer_geometry_transformation_set(r, &m);

	printf("cx %g cy %g fx %g fy %g r %g\n", cx, cy, fx, fy, rad);
	//printf("matrix = %" ENESIM_MATRIX_FORMAT "\n", ENESIM_MATRIX_ARGS(&m));

	/* FIXME for now we dont handle the focis */
	enesim_renderer_gradient_radial_center_x_set(r, cx);
	enesim_renderer_gradient_radial_center_y_set(r, cy);
	enesim_renderer_gradient_radial_focus_x_set(r, fx);
	enesim_renderer_gradient_radial_focus_y_set(r, fy);
	enesim_renderer_gradient_radial_radius_set(r, rad);

	return ESVG_SETUP_OK;
}

static void _esvg_radial_gradient_free(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	free(thiz);
}

static Esvg_Gradient_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_radial_gradient_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_radial_gradient_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_radial_gradient_attribute_set,
	/* .clone		= */ NULL,
	/* .setup		= */ _esvg_radial_gradient_setup,
	/* .renderer_new	= */ _esvg_radial_gradient_renderer_new,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_radial_gradient_new(void)
{
	Esvg_Radial_Gradient *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Radial_Gradient));

	/* default values */

	t = esvg_gradient_new(&_descriptor, ESVG_RADIALGRADIENT, thiz);
	return t;
}

static void _esvg_radial_gradient_cx_set(Edom_Tag *t, const Esvg_Coord *cx)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
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

static void _esvg_radial_gradient_cx_get(Edom_Tag *t, Esvg_Coord *cx)
{
	Esvg_Radial_Gradient *thiz;

	if (!cx) return;
	thiz = _esvg_radial_gradient_get(t);
	*cx = thiz->cx;
}

static Eina_Bool _esvg_radial_gradient_cx_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->cx_is_set;
}

static void _esvg_radial_gradient_cy_set(Edom_Tag *t, const Esvg_Coord *cy)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
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

static void _esvg_radial_gradient_cy_get(Edom_Tag *t, Esvg_Coord *cy)
{
	Esvg_Radial_Gradient *thiz;

	if (!cy) return;
	thiz = _esvg_radial_gradient_get(t);
	*cy = thiz->cy;
}

static Eina_Bool _esvg_radial_gradient_cy_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->cy_is_set;
}

static void _esvg_radial_gradient_fx_set(Edom_Tag *t, const Esvg_Coord *fx)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
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

static void _esvg_radial_gradient_fx_get(Edom_Tag *t, Esvg_Coord *fx)
{
	Esvg_Radial_Gradient *thiz;

	if (!fx) return;
	thiz = _esvg_radial_gradient_get(t);
	*fx = thiz->fx;
}

static Eina_Bool _esvg_radial_gradient_fx_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->fx_is_set;
}

static void _esvg_radial_gradient_fy_set(Edom_Tag *t, const Esvg_Coord *fy)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
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

static void _esvg_radial_gradient_fy_get(Edom_Tag *t, Esvg_Coord *fy)
{
	Esvg_Radial_Gradient *thiz;

	if (!fy) return;
	thiz = _esvg_radial_gradient_get(t);
	*fy = thiz->fy;
}

static Eina_Bool _esvg_radial_gradient_fy_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->fy_is_set;
}

static void _esvg_radial_gradient_r_set(Edom_Tag *t, const Esvg_Length *rad)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
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

static void _esvg_radial_gradient_r_get(Edom_Tag *t, Esvg_Length *rad)
{
	Esvg_Radial_Gradient *thiz;

	if (!rad) return;
	thiz = _esvg_radial_gradient_get(t);
	*rad = thiz->rad;
}

static Eina_Bool _esvg_radial_gradient_r_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->rad_is_set;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_radial_gradient.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_radial_gradient_new(void)
{
	return ender_element_new_with_namespace("radial_gradient", "esvg");
}

EAPI Eina_Bool esvg_is_radial_gradient(Ender_Element *e)
{
}

EAPI void esvg_radial_gradient_cx_set(Ender_Element *e, const Esvg_Coord *cx)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_CX, cx, NULL);
}

EAPI void esvg_radial_gradient_cx_get(Ender_Element *e, Esvg_Coord *cx)
{
}

EAPI Eina_Bool esvg_radial_gradient_cx_is_set(Ender_Element *e)
{
}

EAPI void esvg_radial_gradient_cy_set(Ender_Element *e, const Esvg_Coord *cy)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_CY, cy, NULL);
}

EAPI void esvg_radial_gradient_cy_get(Ender_Element *e, Esvg_Coord *cy)
{
}

EAPI Eina_Bool esvg_radial_gradient_cy_is_set(Ender_Element *e)
{
}

EAPI void esvg_radial_gradient_fx_set(Ender_Element *e, const Esvg_Coord *fx)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_FX, fx, NULL);
}

EAPI void esvg_radial_gradient_fx_get(Ender_Element *e, Esvg_Coord *fx)
{
}

EAPI Eina_Bool esvg_radial_gradient_fx_is_set(Ender_Element *e)
{
}

EAPI void esvg_radial_gradient_fy_set(Ender_Element *e, const Esvg_Coord *fy)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_FY, fy, NULL);
}

EAPI void esvg_radial_gradient_fy_get(Ender_Element *e, Esvg_Coord *fy)
{
}

EAPI Eina_Bool esvg_radial_gradient_fy_is_set(Ender_Element *e)
{
}

EAPI void esvg_radial_gradient_r_set(Ender_Element *e, const Esvg_Length *rad)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_R, rad, NULL);
}

EAPI void esvg_radial_gradient_r_get(Ender_Element *e, Esvg_Length *rad)
{
}

EAPI Eina_Bool esvg_radial_gradient_r_is_set(Ender_Element *e)
{
}
