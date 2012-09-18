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
#include "esvg_private_renderable.h"
#include "esvg_rect.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_rect

static Ender_Property *ESVG_RECT_X;
static Ender_Property *ESVG_RECT_Y;
static Ender_Property *ESVG_RECT_RX;
static Ender_Property *ESVG_RECT_RY;
static Ender_Property *ESVG_RECT_WIDTH;
static Ender_Property *ESVG_RECT_HEIGHT;

typedef struct _Esvg_Rect_State
{
	Esvg_Animated_Coord x;
	Esvg_Animated_Coord y;
	Esvg_Animated_Length width;
	Esvg_Animated_Length height;
	Esvg_Animated_Length rx;
	Esvg_Animated_Length ry;
} Esvg_Rect_State;

typedef struct _Esvg_Rect
{
	/* properties */
	Esvg_Rect_State current;
	Esvg_Rect_State past;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Rect;

static Esvg_Rect * _esvg_rect_get(Edom_Tag *t)
{
	Esvg_Rect *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_RECT)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_rect_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_rect_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_rect_y_set(e, &y);
	}
	else if (strcmp(key, "rx") == 0)
	{
		Esvg_Coord rx;

		esvg_length_string_from(&rx, value);
		esvg_rect_rx_set(e, &rx);
	}
	else if (strcmp(key, "ry") == 0)
	{
		Esvg_Coord ry;

		esvg_length_string_from(&ry, value);
		esvg_rect_ry_set(e, &ry);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value);
		esvg_rect_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value);
		esvg_rect_height_set(e, &height);
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_rect_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_rect_renderer_get(Edom_Tag *t)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	return thiz->r;
}

static Esvg_Element_Setup_Return _esvg_rect_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_rect_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Rect *thiz;
	double x, y;
	double rx, ry;
	double width, height;

	thiz = _esvg_rect_get(t);

	/* set the position */
	/* TODO check if we are animating */
	x = esvg_length_final_get(&thiz->current.x.base, ctx->viewbox.width, ctx->font_size);
	y = esvg_length_final_get(&thiz->current.y.base, ctx->viewbox.height, ctx->font_size);
	enesim_renderer_rectangle_position_set(thiz->r, x, y);
	/* set the size */
	width = esvg_length_final_get(&thiz->current.width.base, ctx->viewbox.width, ctx->font_size);
	height = esvg_length_final_get(&thiz->current.height.base, ctx->viewbox.height, ctx->font_size);
	enesim_renderer_rectangle_size_set(thiz->r, width, height);

	/* set the bounds */
	enesim_rectangle_coords_from((Enesim_Rectangle *)&ctx->bounds, x, y, width, height);

	/* FIXME enesim does not supports rx *and* ry */
	rx = esvg_length_final_get(&thiz->current.rx.base, ctx->viewbox.width, ctx->font_size);
	enesim_renderer_rectangle_corner_radius_set(thiz->r, rx);
	enesim_renderer_rectangle_corners_set(thiz->r,
			EINA_TRUE, EINA_TRUE, EINA_TRUE, EINA_TRUE);

	/* shape properties */
	enesim_renderer_shape_fill_color_set(thiz->r, rctx->fill_color);
	enesim_renderer_shape_fill_renderer_set(thiz->r, rctx->fill_renderer);
	enesim_renderer_shape_stroke_color_set(thiz->r, rctx->stroke_color);
	enesim_renderer_shape_stroke_renderer_set(thiz->r, rctx->stroke_renderer);

	enesim_renderer_shape_stroke_weight_set(thiz->r, rctx->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_draw_mode_set(thiz->r, rctx->draw_mode);

	DBG("calling the setup on the rect (%g %g %g %g) %g", x, y, width, height, rctx->stroke_weight);
	DBG("fc %08x sc %08x c %08x", rctx->fill_color, rctx->stroke_color, rctx->color);
	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &ctx->transform);
	enesim_renderer_color_set(thiz->r, rctx->color);

	return EINA_TRUE;
}

#if 0
static Eina_Bool _esvg_rect_has_changed(Edom_Tag *t)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (!thiz->changed) return EINA_FALSE;

	if (esvg_length_is_equal(&thiz->current.x, &thiz->past.x))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.y, &thiz->past.y))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.width, &thiz->past.width))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.height, &thiz->past.height))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.rx, &thiz->past.rx))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.ry, &thiz->past.ry))
		return EINA_TRUE;

	return EINA_FALSE;
}
#endif

static void _esvg_rect_free(Edom_Tag *t)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	enesim_renderer_unref(thiz->r);
	free(thiz);
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get	= */ _esvg_rect_attribute_get,
	/* .cdata_set		= */ NULL,
	/* .text_set		= */ NULL,
	/* .free		= */ _esvg_rect_free,
	/* .initialize		= */ NULL,
	/* .attribute_set	= */ _esvg_rect_attribute_set,
	/* .animation_add 	= */ NULL,
	/* .animation_remove 	= */ NULL,
	/* .setup		= */ _esvg_rect_setup,
	/* .renderer_get	= */ _esvg_rect_renderer_get,
	/* .renderer_propagate	= */ _esvg_rect_renderer_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_rect_new(void)
{
	Esvg_Rect *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Rect));
	if (!thiz) return NULL;

	r = enesim_renderer_rectangle_new();
	thiz->r = r;

	/* Default values */
	enesim_renderer_rop_set(thiz->r, ENESIM_BLEND);
	thiz->current.x.base = ESVG_COORD_0;
	thiz->current.y.base = ESVG_COORD_0;
	thiz->current.width.base = ESVG_LENGTH_0;
	thiz->current.height.base = ESVG_LENGTH_0;
        /* FIXME: set rx and ry ? */
/*	thiz->current.rx = ESVG_COORD_0; */
/*	thiz->current.ry = ESVG_COORD_0; */

	t = esvg_renderable_new(&_descriptor, ESVG_RECT, thiz);
	return t;
}

static void _esvg_rect_x_set(Edom_Tag *t, const Esvg_Animated_Coord *x)
{
	Esvg_Rect *thiz;

	if (!x) return;
	thiz = _esvg_rect_get(t);
	if (esvg_element_attribute_animate_get(t))
		thiz->current.x.anim = x->base;
	else
		thiz->current.x.base = x->base;
	thiz->changed = EINA_TRUE;
}

static void _esvg_rect_x_get(Edom_Tag *t, Esvg_Animated_Coord *x)
{
	Esvg_Rect *thiz;

	if (!x) return;
	thiz = _esvg_rect_get(t);
	*x = thiz->current.x;
}

static void _esvg_rect_y_set(Edom_Tag *t, const Esvg_Animated_Coord *y)
{
	Esvg_Rect *thiz;

	if (!y) return;
	thiz = _esvg_rect_get(t);
	if (esvg_element_attribute_animate_get(t))
		thiz->current.y.anim = y->base;
	else
		thiz->current.y.base = y->base;
	thiz->changed = EINA_TRUE;
}

static void _esvg_rect_y_get(Edom_Tag *t, Esvg_Animated_Coord *y)
{
	Esvg_Rect *thiz;

	if (!y) return;
	thiz = _esvg_rect_get(t);
	*y = thiz->current.y;
}

static void _esvg_rect_width_set(Edom_Tag *t, const Esvg_Animated_Length *width)
{
	Esvg_Rect *thiz;

	if (!width) return;
	thiz = _esvg_rect_get(t);
	if (esvg_element_attribute_animate_get(t))
		thiz->current.width.anim = width->base;
	else
		thiz->current.width.base = width->base;
	thiz->changed = EINA_TRUE;
}

static void _esvg_rect_width_get(Edom_Tag *t, Esvg_Animated_Length *width)
{
	Esvg_Rect *thiz;

	if (!width) return;
	thiz = _esvg_rect_get(t);
	*width = thiz->current.width;
}

static void _esvg_rect_height_set(Edom_Tag *t, const Esvg_Animated_Length *height)
{
	Esvg_Rect *thiz;

	if (!height) return;
	thiz = _esvg_rect_get(t);
	if (esvg_element_attribute_animate_get(t))
		thiz->current.height.anim = height->base;
	else
		thiz->current.height.base = height->base;
	thiz->changed = EINA_TRUE;
}

static void _esvg_rect_height_get(Edom_Tag *t, Esvg_Animated_Length *height)
{
	Esvg_Rect *thiz;

	if (!height) return;
	thiz = _esvg_rect_get(t);
	*height = thiz->current.height;
}

static void _esvg_rect_rx_set(Edom_Tag *t, const Esvg_Animated_Coord *rx)
{
	Esvg_Rect *thiz;

	if (!rx) return;
	thiz = _esvg_rect_get(t);
	if (esvg_element_attribute_animate_get(t))
		thiz->current.rx.anim = rx->base;
	else
		thiz->current.rx.base = rx->base;
	thiz->changed = EINA_TRUE;
}

static void _esvg_rect_rx_get(Edom_Tag *t, Esvg_Animated_Coord *rx)
{
	Esvg_Rect *thiz;

	if (!rx) return;
	thiz = _esvg_rect_get(t);
	*rx = thiz->current.rx;
}

static void _esvg_rect_ry_set(Edom_Tag *t, const Esvg_Animated_Coord *ry)
{
	Esvg_Rect *thiz;

	if (!ry) return;
	thiz = _esvg_rect_get(t);
	if (esvg_element_attribute_animate_get(t))
		thiz->current.ry.anim = ry->base;
	else
		thiz->current.ry.base = ry->base;
	thiz->changed = EINA_TRUE;
}

static void _esvg_rect_ry_get(Edom_Tag *t, Esvg_Animated_Coord *ry)
{
	Esvg_Rect *thiz;

	if (!ry) return;
	thiz = _esvg_rect_get(t);
	*ry = thiz->current.ry;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_rect_delete NULL
#define _esvg_rect_x_is_set NULL
#define _esvg_rect_y_is_set NULL
#define _esvg_rect_rx_is_set NULL
#define _esvg_rect_ry_is_set NULL
#define _esvg_rect_width_is_set NULL
#define _esvg_rect_height_is_set NULL
#include "generated/esvg_generated_rect.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_rect_new(void)
{
	return ESVG_ELEMENT_NEW("rect");
}

EAPI Eina_Bool esvg_is_rect(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_RECT) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_rect_x_set(Ender_Element *e, const Esvg_Coord *x)
{
	Esvg_Animated_Coord ax;

	if (!x)
	{
		ender_element_property_value_set(e, ESVG_RECT_X, NULL, NULL);
		return;
	}

	ax.base = *x;
	ender_element_property_value_set(e, ESVG_RECT_X, &ax, NULL);
}

EAPI void esvg_rect_x_get(Ender_Element *e, Esvg_Coord *x)
{
}

EAPI void esvg_rect_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	Esvg_Animated_Coord ay;

	if (!y)
	{
		ender_element_property_value_set(e, ESVG_RECT_Y, NULL, NULL);
		return;
	}

	ay.base = *y;
	ender_element_property_value_set(e, ESVG_RECT_Y, &ay, NULL);
}

EAPI void esvg_rect_y_get(Ender_Element *e, Esvg_Coord *y)
{
}

EAPI void esvg_rect_width_set(Ender_Element *e, const Esvg_Length *width)
{
	Esvg_Animated_Length aw;

	if (!width)
	{
		ender_element_property_value_set(e, ESVG_RECT_WIDTH, NULL, NULL);
		return;
	}
	aw.base = *width;
	ender_element_property_value_set(e, ESVG_RECT_WIDTH, &aw, NULL);
}

EAPI void esvg_rect_width_get(Ender_Element *e, Esvg_Length *width)
{
}

EAPI void esvg_rect_height_set(Ender_Element *e, const Esvg_Length *height)
{
	Esvg_Animated_Length ah;

	if (!height)
	{
		ender_element_property_value_set(e, ESVG_RECT_HEIGHT, NULL, NULL);
		return;
	}
	ah.base = *height;
	ender_element_property_value_set(e, ESVG_RECT_HEIGHT, &ah, NULL);
}

EAPI void esvg_rect_height_get(Ender_Element *e, Esvg_Length *height)
{
}

EAPI void esvg_rect_rx_set(Ender_Element *e, const Esvg_Coord *rx)
{
	Esvg_Animated_Coord arx;

	if (!rx)
	{
		ender_element_property_value_set(e, ESVG_RECT_RX, NULL, NULL);
		return;
	}
	arx.base = *rx;
	ender_element_property_value_set(e, ESVG_RECT_RX, &arx, NULL);
}

EAPI void esvg_rect_rx_get(Ender_Element *e, Esvg_Coord *rx)
{
}

EAPI void esvg_rect_ry_set(Ender_Element *e, const Esvg_Coord *ry)
{
	Esvg_Animated_Coord ary;

	if (!ry)
	{
		ender_element_property_value_set(e, ESVG_RECT_RY, ry, NULL);
		return;
	}

	ary.base = *ry;
	ender_element_property_value_set(e, ESVG_RECT_RY, &ary, NULL);
}

EAPI void esvg_rect_ry_get(Ender_Element *e, Esvg_Coord *ry)
{
}
