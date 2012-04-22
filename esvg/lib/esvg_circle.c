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
#include "esvg_private_instantiable.h"
#include "esvg_circle.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_CIRCLE_CX;
static Ender_Property *ESVG_CIRCLE_CY;
static Ender_Property *ESVG_CIRCLE_R;

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

static Esvg_Circle * _esvg_circle_get(Edom_Tag *t)
{
	Esvg_Circle *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_CIRCLE)
		return NULL;
	thiz = esvg_instantiable_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_circle_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "cx") == 0)
	{
		Esvg_Coord cx;

		esvg_length_string_from(&cx, value, ESVG_COORD_0);
		esvg_circle_cx_set(e, &cx);
	}
	else if (strcmp(key, "cy") == 0)
	{
		Esvg_Coord cy;

		esvg_length_string_from(&cy, value, ESVG_COORD_0);
		esvg_circle_cy_set(e, &cy);
	}
	else if (strcmp(key, "r") == 0)
	{
		Esvg_Length radius;

		esvg_length_string_from(&radius, value, ESVG_LENGTH_0);
		esvg_circle_r_set(e, &radius);
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_circle_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_circle_renderer_get(Edom_Tag *t)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(t);
	return thiz->r;
}

static Eina_Bool _esvg_circle_setup(Edom_Tag *t,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Circle *thiz;
	double cx, cy;
	double radius;

	thiz = _esvg_circle_get(t);

	/* FIXME gets the parents size or the topmost? */
	/* set the origin */
	cx = esvg_length_final_get(&thiz->current.cx, ctx->viewbox.width);
	cy = esvg_length_final_get(&thiz->current.cy, ctx->viewbox.height);
	/* set the size */
	radius = esvg_length_final_get(&thiz->current.radius, ctx->viewbox.width);
	printf("calling the setup on the circle (%g %g %g)\n", cx, cy, radius);
	enesim_renderer_circle_center_set(thiz->r, cx, cy);
	enesim_renderer_circle_radius_set(thiz->r, radius);

	/* shape properties */
	if (!rctx->fill_renderer)
		enesim_renderer_shape_fill_color_set(thiz->r, rctx->fill_color);
	else
		enesim_renderer_shape_fill_renderer_set(thiz->r, rctx->fill_renderer);
	if (!rctx->stroke_renderer)
		enesim_renderer_shape_stroke_color_set(thiz->r, rctx->stroke_color);
	else
		enesim_renderer_shape_stroke_renderer_set(thiz->r, rctx->stroke_renderer);

	enesim_renderer_shape_stroke_weight_set(thiz->r, rctx->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_draw_mode_set(thiz->r, rctx->draw_mode);

	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &ctx->transform);
	enesim_renderer_color_set(thiz->r, rctx->color);

	return EINA_TRUE;
}

static void _esvg_circle_clone(Edom_Tag *t, Edom_Tag *dt)
{
	Esvg_Circle *thiz;
	Esvg_Circle *other;

	thiz = _esvg_circle_get(t);
	other = _esvg_circle_get(dt);

	other->current.cx = thiz->current.cx;
	other->current.cy = thiz->current.cy;
	other->current.radius = thiz->current.radius;
}

static void _esvg_circle_free(Edom_Tag *t)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(t);
	free(thiz);
}

#if 0
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
#endif

static Esvg_Instantiable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_circle_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_circle_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_circle_attribute_set,
	/* .clone		= */ _esvg_circle_clone,
	/* .setup		= */ _esvg_circle_setup,
	/* .renderer_get	= */ _esvg_circle_renderer_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
EAPI Edom_Tag * _esvg_circle_new(void)
{
	Esvg_Circle *thiz;
	Edom_Tag *t;
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

	t = esvg_instantiable_new(&_descriptor, ESVG_CIRCLE, thiz);
	return t;
}

static void _esvg_circle_cx_set(Edom_Tag *t, const Esvg_Coord *cx)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(t);
	if (cx)
	{
		thiz->current.cx = *cx;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_circle_cx_get(Edom_Tag *t, Esvg_Coord *cx)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(t);
	if (cx) *cx = thiz->current.cx;
}

static void _esvg_circle_cy_set(Edom_Tag *t, const Esvg_Coord *cy)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(t);
	if (cy)
	{
		thiz->current.cy = *cy;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_circle_cy_get(Edom_Tag *t, Esvg_Coord *cy)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(t);
	if (cy) *cy = thiz->current.cy;
}

static void _esvg_circle_r_set(Edom_Tag *t, const Esvg_Length *radius)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(t);
	if (radius)
	{
		thiz->current.radius = *radius;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_circle_r_get(Edom_Tag *t, Esvg_Length *radius)
{
	Esvg_Circle *thiz;

	thiz = _esvg_circle_get(t);
	if (radius) *radius = thiz->current.radius;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_circle.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_circle_new(void)
{
	return ender_element_new_with_namespace("circle", "esvg");
}

EAPI Eina_Bool esvg_is_circle(Ender_Element *e)
{
}

EAPI void esvg_circle_cx_set(Ender_Element *e, const Esvg_Coord *cx)
{
	ender_element_property_value_set(e, ESVG_CIRCLE_CX, cx, NULL);
}

EAPI void esvg_circle_cx_get(Ender_Element *e, Esvg_Coord *cx)
{
}

EAPI void esvg_circle_cy_set(Ender_Element *e, const Esvg_Coord *cy)
{
	ender_element_property_value_set(e, ESVG_CIRCLE_CY, cy, NULL);
}

EAPI void esvg_circle_cy_get(Ender_Element *e, Esvg_Coord *cy)
{
}

EAPI void esvg_circle_r_set(Ender_Element *e, const Esvg_Length *r)
{
	ender_element_property_value_set(e, ESVG_CIRCLE_R, r, NULL);
}

EAPI void esvg_circle_r_get(Ender_Element *e, Esvg_Length *r)
{
}
