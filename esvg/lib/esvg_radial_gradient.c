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
#include "esvg_private_types.h"

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
	Esvg_Attribute_Coord cx;
	Esvg_Attribute_Coord cy;
	Esvg_Attribute_Coord fx;
	Esvg_Attribute_Coord fy;
	Esvg_Attribute_Length rad;
	/* private */
} Esvg_Radial_Gradient;

static Esvg_Radial_Gradient * _esvg_radial_gradient_get(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_RADIALGRADIENT)
		return NULL;
	thiz = esvg_gradient_data_get(t);

	return thiz;
}

static Eina_Bool _esvg_radial_gradient_deep_fy_get(Esvg_Radial_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *fy)
{
	Edom_Tag *href;

	href = esvg_gradient_href_tag_get(t);
	if (!thiz->fy.is_set)
	{

 		if (href)
		{
			Esvg_Radial_Gradient *other;

			other = _esvg_radial_gradient_get(href);
			return _esvg_radial_gradient_deep_fy_get(other, href, fy);
		}
		return EINA_FALSE;
	}
	else
	{
		*fy = thiz->fy.v;
		return EINA_TRUE;
	}
}

static Eina_Bool _esvg_radial_gradient_deep_fx_get(Esvg_Radial_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *fx)
{
	Edom_Tag *href;

	href = esvg_gradient_href_tag_get(t);
	if (!thiz->fx.is_set)
	{
		if (href)
		{
			Esvg_Radial_Gradient *other;

			other = _esvg_radial_gradient_get(href);
			return _esvg_radial_gradient_deep_fx_get(other, href, fx);
		}
		return EINA_FALSE;
	}
	else
	{
		*fx = thiz->fx.v;
		return EINA_TRUE;
	}
}

static void _esvg_radial_gradient_deep_cx_get(Esvg_Radial_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *cx)
{
	Edom_Tag *href;

	href = esvg_gradient_href_tag_get(t);
	if (!thiz->cx.is_set && href)
	{
		Esvg_Radial_Gradient *other;

		other = _esvg_radial_gradient_get(href);
		_esvg_radial_gradient_deep_cx_get(other, href, cx);
	}
	else
		*cx = thiz->cx.v;
}

static void _esvg_radial_gradient_deep_cy_get(Esvg_Radial_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *cy)
{
	Edom_Tag *href;

	href = esvg_gradient_href_tag_get(t);
	if (!thiz->cy.is_set && href)
	{
		Esvg_Radial_Gradient *other;

		other = _esvg_radial_gradient_get(href);
		_esvg_radial_gradient_deep_cy_get(other, href, cy);
	}
	else
		*cy = thiz->cy.v;
}

static void _esvg_radial_gradient_deep_rad_get(Esvg_Radial_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Length *rad)
{
	Edom_Tag *href;

	href = esvg_gradient_href_tag_get(t);
	if (!thiz->rad.is_set && href)
	{
		Esvg_Radial_Gradient *other;

		other = _esvg_radial_gradient_get(href);
		_esvg_radial_gradient_deep_rad_get(other, href, rad);
	}
	else
		*rad = thiz->rad.v;
}


static void _esvg_radial_gradient_merge(Esvg_Radial_Gradient *thiz,
		Edom_Tag *t,
		Esvg_Coord *cx, Esvg_Coord *cy,
		Esvg_Coord *fx, Esvg_Coord *fy,
		Esvg_Length *rad)
{
	_esvg_radial_gradient_deep_cx_get(thiz, t, cx);
	_esvg_radial_gradient_deep_cy_get(thiz, t, cy);
	/* for fx and fy, if those are not set or inherited, then use
	 * cx or cy accordingly
	 */
	if (!_esvg_radial_gradient_deep_fx_get(thiz, t, fx))
		*fx = *cx;
	if (!_esvg_radial_gradient_deep_fy_get(thiz, t, fy))
		*fy = *cy;
	_esvg_radial_gradient_deep_rad_get(thiz, t, rad);
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

		esvg_length_string_from(&cx, value);
		esvg_radial_gradient_cx_set(e, &cx);
	}
	else if (strcmp(key, "cy") == 0)
	{
		Esvg_Coord cy;

		esvg_length_string_from(&cy, value);
		esvg_radial_gradient_cy_set(e, &cy);
	}
	else if (strcmp(key, "r") == 0)
	{
		Esvg_Length rad;

		esvg_length_string_from(&rad, value);
		esvg_radial_gradient_r_set(e, &rad);
	}
	else if (strcmp(key, "fx") == 0)
	{
		Esvg_Length fx;

		esvg_length_string_from(&fx, value);
		esvg_radial_gradient_fx_set(e, &fx);
	}
	else if (strcmp(key, "fy") == 0)
	{
		Esvg_Length fy;

		esvg_length_string_from(&fy, value);
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

static Eina_Bool _esvg_radial_gradient_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Gradient_Context *gctx,
		Enesim_Renderer *r,
		Enesim_Error **error)
{
	Esvg_Radial_Gradient *thiz;
	Esvg_Gradient_Units gu;
	Enesim_Repeat_Mode mode;
	Enesim_Matrix m;
	Esvg_Coord lcx;
	Esvg_Coord lcy;
	Esvg_Coord lfx;
	Esvg_Coord lfy;
	Esvg_Coord lrad;
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

		default:
		return EINA_FALSE;
	}
	enesim_renderer_gradient_mode_set(r, mode);

	/* the coordinates can come from the href
	 * we need to found which one isnt set and use that
	 * for the calculus
	 */
	_esvg_radial_gradient_merge(thiz, t, &lcx, &lcy, &lfx, &lfy, &lrad);

	if (gu == ESVG_OBJECT_BOUNDING_BOX)
	{
		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		cx = esvg_length_final_get(&lcx, 1, 1);
		cy = esvg_length_final_get(&lcy, 1, 1);
		fx = esvg_length_final_get(&lfx, 1, 1);
		fy = esvg_length_final_get(&lfy, 1, 1);
		rad = esvg_length_final_get(&lrad, 1, 1);
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

		cx = esvg_length_final_get(&lcx, w, ctx->font_size);
		cy = esvg_length_final_get(&lcy, h, ctx->font_size);
		fx = esvg_length_final_get(&lfx, w, ctx->font_size);
		fy = esvg_length_final_get(&lfy, h, ctx->font_size);
		if (thiz->rad.v.unit == ESVG_UNIT_LENGTH_PERCENT)
		{
			rad_vp = hypot(w, h) / M_SQRT2;
		}
		rad = esvg_length_final_get(&lrad, rad_vp, ctx->font_size);
		m = ctx->transform;
	}
	if (enesim_matrix_type_get(&gctx->transform) != ENESIM_MATRIX_IDENTITY)
	{
		enesim_matrix_compose(&m, &gctx->transform, &m);
	}
	enesim_renderer_geometry_transformation_set(r, &m);
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
	/* .setup		= */ NULL,
	/* .renderer_new	= */ _esvg_radial_gradient_renderer_new,
	/* .propagate		= */ _esvg_radial_gradient_propagate,
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
		thiz->cx.is_set = EINA_FALSE;
	}
	else
	{
		thiz->cx.v = *cx;
		thiz->cx.is_set = EINA_TRUE;
	}
}

static void _esvg_radial_gradient_cx_get(Edom_Tag *t, Esvg_Coord *cx)
{
	Esvg_Radial_Gradient *thiz;

	if (!cx) return;
	thiz = _esvg_radial_gradient_get(t);
	*cx = thiz->cx.v;
}

static Eina_Bool _esvg_radial_gradient_cx_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->cx.is_set;
}

static void _esvg_radial_gradient_cy_set(Edom_Tag *t, const Esvg_Coord *cy)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	if (!cy)
	{
		thiz->cy.is_set = EINA_FALSE;
	}
	else
	{
		thiz->cy.v = *cy;
		thiz->cy.is_set = EINA_TRUE;
	}
}

static void _esvg_radial_gradient_cy_get(Edom_Tag *t, Esvg_Coord *cy)
{
	Esvg_Radial_Gradient *thiz;

	if (!cy) return;
	thiz = _esvg_radial_gradient_get(t);
	*cy = thiz->cy.v;
}

static Eina_Bool _esvg_radial_gradient_cy_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->cy.is_set;
}

static void _esvg_radial_gradient_fx_set(Edom_Tag *t, const Esvg_Coord *fx)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	if (!fx)
	{
		thiz->fx.is_set = EINA_FALSE;
	}
	else
	{
		thiz->fx.v = *fx;
		thiz->fx.is_set = EINA_TRUE;
	}
}

static void _esvg_radial_gradient_fx_get(Edom_Tag *t, Esvg_Coord *fx)
{
	Esvg_Radial_Gradient *thiz;

	if (!fx) return;
	thiz = _esvg_radial_gradient_get(t);
	*fx = thiz->fx.v;
}

static Eina_Bool _esvg_radial_gradient_fx_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->fx.is_set;
}

static void _esvg_radial_gradient_fy_set(Edom_Tag *t, const Esvg_Coord *fy)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	if (!fy)
	{
		thiz->fy.is_set = EINA_FALSE;
	}
	else
	{
		thiz->fy.v = *fy;
		thiz->fy.is_set = EINA_TRUE;
	}
}

static void _esvg_radial_gradient_fy_get(Edom_Tag *t, Esvg_Coord *fy)
{
	Esvg_Radial_Gradient *thiz;

	if (!fy) return;
	thiz = _esvg_radial_gradient_get(t);
	*fy = thiz->fy.v;
}

static Eina_Bool _esvg_radial_gradient_fy_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->fy.is_set;
}

static void _esvg_radial_gradient_r_set(Edom_Tag *t, const Esvg_Length *rad)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	if (!rad)
	{
		thiz->rad.is_set = EINA_FALSE;
	}
	else
	{
		thiz->rad.v = *rad;
		thiz->rad.is_set = EINA_TRUE;
	}
}

static void _esvg_radial_gradient_r_get(Edom_Tag *t, Esvg_Length *rad)
{
	Esvg_Radial_Gradient *thiz;

	if (!rad) return;
	thiz = _esvg_radial_gradient_get(t);
	*rad = thiz->rad.v;
}

static Eina_Bool _esvg_radial_gradient_r_is_set(Edom_Tag *t)
{
	Esvg_Radial_Gradient *thiz;

	thiz = _esvg_radial_gradient_get(t);
	return thiz->rad.is_set;
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
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_RADIALGRADIENT) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_radial_gradient_cx_set(Ender_Element *e, const Esvg_Coord *cx)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_CX, cx, NULL);
}

EAPI void esvg_radial_gradient_cx_get(Ender_Element *e, Esvg_Coord *cx)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_radial_gradient_cx_get(t, cx);
}

EAPI Eina_Bool esvg_radial_gradient_cx_is_set(Ender_Element *e)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	return _esvg_radial_gradient_cx_is_set(t);
}

EAPI void esvg_radial_gradient_cy_set(Ender_Element *e, const Esvg_Coord *cy)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_CY, cy, NULL);
}

EAPI void esvg_radial_gradient_cy_get(Ender_Element *e, Esvg_Coord *cy)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_radial_gradient_cy_get(t, cy);
}

EAPI Eina_Bool esvg_radial_gradient_cy_is_set(Ender_Element *e)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	return _esvg_radial_gradient_cy_is_set(t);
}

EAPI void esvg_radial_gradient_fx_set(Ender_Element *e, const Esvg_Coord *fx)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_FX, fx, NULL);
}

EAPI void esvg_radial_gradient_fx_get(Ender_Element *e, Esvg_Coord *fx)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_radial_gradient_fx_get(t, fx);
}

EAPI Eina_Bool esvg_radial_gradient_fx_is_set(Ender_Element *e)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	return _esvg_radial_gradient_fx_is_set(t);
}

EAPI void esvg_radial_gradient_fy_set(Ender_Element *e, const Esvg_Coord *fy)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_FY, fy, NULL);
}

EAPI void esvg_radial_gradient_fy_get(Ender_Element *e, Esvg_Coord *fy)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_radial_gradient_fy_get(t, fy);
}

EAPI Eina_Bool esvg_radial_gradient_fy_is_set(Ender_Element *e)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	return _esvg_radial_gradient_fy_is_set(t);
}

EAPI void esvg_radial_gradient_r_set(Ender_Element *e, const Esvg_Length *rad)
{
	ender_element_property_value_set(e, ESVG_RADIAL_GRADIENT_R, rad, NULL);
}

EAPI void esvg_radial_gradient_r_get(Ender_Element *e, Esvg_Length *rad)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_radial_gradient_r_get(t, rad);
}

EAPI Eina_Bool esvg_radial_gradient_r_is_set(Ender_Element *e)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	return _esvg_radial_gradient_r_is_set(t);
}
