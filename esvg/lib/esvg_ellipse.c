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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_ellipse.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_ellipse
static Ender_Property *ESVG_ELLIPSE_CX;
static Ender_Property *ESVG_ELLIPSE_CY;
static Ender_Property *ESVG_ELLIPSE_RX;
static Ender_Property *ESVG_ELLIPSE_RY;

typedef struct _Esvg_Ellipse
{
	/* properties */
	Esvg_Attribute_Animated_Coord cx;
	Esvg_Attribute_Animated_Coord cy;
	Esvg_Attribute_Animated_Length rx;
	Esvg_Attribute_Animated_Length ry;
	/* private */
	/* generated at setup */
	double gcx;
	double gcy;
	double grx;
	double gry;
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Ellipse;

static Esvg_Ellipse * _esvg_ellipse_get(Edom_Tag *t)
{
	Esvg_Ellipse *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_ELLIPSE)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_ellipse_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "cx") == 0)
	{
		Esvg_Coord cx;

		esvg_length_string_from(&cx, value);
		esvg_ellipse_cx_set(e, &cx);
	}
	else if (strcmp(key, "cy") == 0)
	{
		Esvg_Coord cy;

		esvg_length_string_from(&cy, value);
		esvg_ellipse_cy_set(e, &cy);
	}
	else if (strcmp(key, "rx") == 0)
	{
		Esvg_Length rx;

		esvg_length_string_from(&rx, value);
		esvg_ellipse_rx_set(e, &rx);
	}
	else if (strcmp(key, "ry") == 0)
	{
		Esvg_Length ry;

		esvg_length_string_from(&ry, value);
		esvg_ellipse_ry_set(e, &ry);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_ellipse_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static int * _esvg_ellipse_attribute_animated_fetch(Edom_Tag *t, const char *attr)
{
	Esvg_Ellipse *thiz;
	int *animated = NULL;

	thiz = _esvg_ellipse_get(t);
	if (!strcmp(attr, "cx"))
	{
		animated = &thiz->cx.animated;
	}
	else if (!strcmp(attr, "cy"))
	{
		animated = &thiz->cy.animated;
	}
	else if (!strcmp(attr, "rx"))
	{
		animated = &thiz->rx.animated;
	}
	else if (!strcmp(attr, "ry"))
	{
		animated = &thiz->ry.animated;
	}
	return animated;
}

static Enesim_Renderer * _esvg_ellipse_renderer_get(Edom_Tag *t)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(t);
	return thiz->r;
}

static Esvg_Element_Setup_Return _esvg_ellipse_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Ellipse *thiz;
	Esvg_Length lcx, lcy;
	Esvg_Length lrx, lry;

	thiz = _esvg_ellipse_get(t);
	/* position */
	esvg_attribute_animated_length_final_get(&thiz->cx, &lcx);
	esvg_attribute_animated_length_final_get(&thiz->cy, &lcy);
	thiz->gcx = esvg_length_final_get(&lcx, ctx->viewbox.width, ctx->font_size);
	thiz->gcy = esvg_length_final_get(&lcy, ctx->viewbox.height, ctx->font_size);
	/* radius */
	esvg_attribute_animated_length_final_get(&thiz->rx, &lrx);
	esvg_attribute_animated_length_final_get(&thiz->ry, &lry);
	thiz->grx = esvg_length_final_get(&lrx, ctx->viewbox.width, ctx->font_size);
	thiz->gry = esvg_length_final_get(&lry, ctx->viewbox.height, ctx->font_size);
	/* set the bounds */
	enesim_rectangle_coords_from(&ctx->bounds,
			thiz->gcx - thiz->grx,
			thiz->gcy - thiz->gry,
			thiz->grx * 2, thiz->gry * 2);
	DBG("calling the setup on the ellipse (%g %g %g %g)", thiz->gcx, thiz->gcy, thiz->grx, thiz->gry);

	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_ellipse_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(t);

	/* set the position */
	enesim_renderer_ellipse_center_set(thiz->r, thiz->gcx, thiz->gcy);
	/* set the size */
	enesim_renderer_ellipse_radii_set(thiz->r, thiz->grx, thiz->gry);

	/* shape properties */
	enesim_renderer_shape_fill_color_set(thiz->r, rctx->fill_color);
	enesim_renderer_shape_fill_renderer_set(thiz->r, rctx->fill_renderer);
	enesim_renderer_shape_stroke_color_set(thiz->r, rctx->stroke_color);
	enesim_renderer_shape_stroke_renderer_set(thiz->r, rctx->stroke_renderer);

	enesim_renderer_shape_stroke_weight_set(thiz->r, rctx->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_draw_mode_set(thiz->r, rctx->draw_mode);
	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &ctx->transform);
	enesim_renderer_color_set(thiz->r, rctx->color);

	return EINA_TRUE;
}

static void _esvg_ellipse_free(Edom_Tag *t)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(t);
	enesim_renderer_unref(thiz->r);
	free(thiz);
}

#if 0
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
#endif

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		     = */ NULL,
	/* .child_remove	     = */ NULL,
	/* .attribute_get 	     = */ _esvg_ellipse_attribute_get,
	/* .cdata_set 		     = */ NULL,
	/* .text_set 		     = */ NULL,
	/* .text_get		     = */ NULL,
	/* .free 		     = */ _esvg_ellipse_free,
	/* .initialize 		     = */ NULL,
	/* .attribute_set 	     = */ _esvg_ellipse_attribute_set,
	/* .attribute_animated_fetch = */ _esvg_ellipse_attribute_animated_fetch,
	/* .setup		     = */ _esvg_ellipse_setup,
	/* .renderer_get	     = */ _esvg_ellipse_renderer_get,
	/* .renderer_propagate	     = */ _esvg_ellipse_renderer_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_ellipse_new(void)
{
	Esvg_Ellipse *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Ellipse));
	if (!thiz) return NULL;

	r = enesim_renderer_ellipse_new();
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->r = r;

	/* Default values */
	thiz->cx.base.v = thiz->cx.anim.v = ESVG_COORD_0;
	thiz->cy.base.v = thiz->cy.anim.v = ESVG_COORD_0;
	thiz->rx.base.v = thiz->rx.anim.v = ESVG_LENGTH_0;
	thiz->ry.base.v = thiz->ry.anim.v = ESVG_LENGTH_0;

	t = esvg_renderable_new(&_descriptor, ESVG_ELLIPSE, thiz);
	return t;
}

static void _esvg_ellipse_cx_set(Edom_Tag *t, const Esvg_Animated_Coord *cx)
{
	Esvg_Ellipse *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_ellipse_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->cx, cx, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_ellipse_cx_get(Edom_Tag *t, Esvg_Animated_Coord *cx)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(t);
	esvg_attribute_animated_length_get(&thiz->cx, cx);
}

static void _esvg_ellipse_cy_set(Edom_Tag *t, const Esvg_Animated_Coord *cy)
{
	Esvg_Ellipse *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_ellipse_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->cy, cy, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_ellipse_cy_get(Edom_Tag *t, Esvg_Animated_Coord *cy)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(t);
	esvg_attribute_animated_length_get(&thiz->cy, cy);
}

static void _esvg_ellipse_rx_set(Edom_Tag *t, const Esvg_Animated_Length *rx)
{
	Esvg_Ellipse *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_ellipse_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->rx, rx, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_ellipse_rx_get(Edom_Tag *t, Esvg_Animated_Length *rx)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(t);
	esvg_attribute_animated_length_get(&thiz->rx, rx);
}

static void _esvg_ellipse_ry_set(Edom_Tag *t, const Esvg_Animated_Length *ry)
{
	Esvg_Ellipse *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_ellipse_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->ry, ry, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_ellipse_ry_get(Edom_Tag *t, Esvg_Animated_Length *ry)
{
	Esvg_Ellipse *thiz;

	thiz = _esvg_ellipse_get(t);
	esvg_attribute_animated_length_get(&thiz->ry, ry);
}

/* The ender wrapper */
#define _esvg_ellipse_delete NULL
#define _esvg_ellipse_cx_is_set NULL
#define _esvg_ellipse_cy_is_set NULL
#define _esvg_ellipse_rx_is_set NULL
#define _esvg_ellipse_ry_is_set NULL
#include "generated/esvg_generated_ellipse.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_ellipse_init(void)
{
	_esvg_ellipse_init();
}

void esvg_ellipse_shutdown(void)
{
	_esvg_ellipse_shutdown();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_ellipse_new(void)
{
	return ESVG_ELEMENT_NEW("ellipse");
}

EAPI Eina_Bool esvg_is_ellipse(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_ELLIPSE) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_ellipse_cx_set(Ender_Element *e, const Esvg_Coord *cx)
{
	esvg_element_property_length_set(e, ESVG_ELLIPSE_CX, cx);
}

EAPI void esvg_ellipse_cx_get(Ender_Element *e, Esvg_Coord *cx)
{
}

EAPI void esvg_ellipse_cy_set(Ender_Element *e, const Esvg_Coord *cy)
{
	esvg_element_property_length_set(e, ESVG_ELLIPSE_CY, cy);
}

EAPI void esvg_ellipse_cy_get(Ender_Element *e, Esvg_Coord *cy)
{
}

EAPI void esvg_ellipse_rx_set(Ender_Element *e, const Esvg_Length *rx)
{
	esvg_element_property_length_set(e, ESVG_ELLIPSE_RX, rx);
}

EAPI void esvg_ellipse_rx_get(Ender_Element *e, Esvg_Length *rx)
{
}

EAPI void esvg_ellipse_ry_set(Ender_Element *e, const Esvg_Length *ry)
{
	esvg_element_property_length_set(e, ESVG_ELLIPSE_RY, ry);
}

EAPI void esvg_ellipse_ry_get(Ender_Element *e, Esvg_Length *ry)
{
}
