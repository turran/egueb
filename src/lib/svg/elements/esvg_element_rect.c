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
#include "esvg_main_private.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_context_private.h"
#include "esvg_element_private.h"
#include "esvg_renderable_private.h"
#include "esvg_element_rect.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_element_rect_log

static int _esvg_element_rect_log = -1;

static Ender_Property *ESVG_ELEMENT_RECT_X;
static Ender_Property *ESVG_ELEMENT_RECT_Y;
static Ender_Property *ESVG_ELEMENT_RECT_RX;
static Ender_Property *ESVG_ELEMENT_RECT_RY;
static Ender_Property *ESVG_ELEMENT_RECT_WIDTH;
static Ender_Property *ESVG_ELEMENT_RECT_HEIGHT;

typedef struct _Esvg_Element_Rect
{
	/* properties */
	Esvg_Attribute_Animated_Coord x;
	Esvg_Attribute_Animated_Coord y;
	Esvg_Attribute_Animated_Length width;
	Esvg_Attribute_Animated_Length height;
	Esvg_Attribute_Animated_Length rx;
	Esvg_Attribute_Animated_Length ry;
	/* private */
	/* generated at setup */
	double gx;
	double gy;
	double gwidth;
	double gheight;
	double grx;
	double gry;
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Element_Rect;

static Esvg_Element_Rect * _esvg_element_rect_get(Edom_Tag *t)
{
	Esvg_Element_Rect *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TYPE_RECT)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                               Helper getters                               *
 *----------------------------------------------------------------------------*/
static void _esvg_element_rect_final_width_get(Edom_Tag *t,
		Esvg_Length *width)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_final_get(&thiz->width, width);
}

static void _esvg_element_rect_final_height_get(Edom_Tag *t,
		Esvg_Length *height)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_final_get(&thiz->height, height);
}

static void _esvg_element_rect_final_x_get(Edom_Tag *t,
		Esvg_Length *x)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_final_get(&thiz->x, x);
}

static void _esvg_element_rect_final_y_get(Edom_Tag *t,
		Esvg_Length *y)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_final_get(&thiz->y, y);
}

static void _esvg_element_rect_final_rx_get(Edom_Tag *t,
		Esvg_Length *rx)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_final_get(&thiz->rx, rx);
}

static void _esvg_element_rect_final_ry_get(Edom_Tag *t,
		Esvg_Length *ry)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_final_get(&thiz->ry, ry);
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_element_rect_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_element_rect_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_element_rect_y_set(e, &y);
	}
	else if (strcmp(key, "rx") == 0)
	{
		Esvg_Coord rx;

		esvg_length_string_from(&rx, value);
		esvg_element_rect_rx_set(e, &rx);
	}
	else if (strcmp(key, "ry") == 0)
	{
		Esvg_Coord ry;

		esvg_length_string_from(&ry, value);
		esvg_element_rect_ry_set(e, &ry);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value);
		esvg_element_rect_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value);
		esvg_element_rect_height_set(e, &height);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_element_rect_attribute_get(Edom_Tag *tag, const char *key, char **value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		_esvg_element_rect_final_x_get(tag, &x);
		*value = esvg_length_string_to(&x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		_esvg_element_rect_final_y_get(tag, &y);
		*value = esvg_length_string_to(&y);
	}
	else if (strcmp(key, "rx") == 0)
	{
		Esvg_Coord rx;

		_esvg_element_rect_final_rx_get(tag, &rx);
		*value = esvg_length_string_to(&rx);
	}
	else if (strcmp(key, "ry") == 0)
	{
		Esvg_Coord ry;

		_esvg_element_rect_final_ry_get(tag, &ry);
		*value = esvg_length_string_to(&ry);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Coord width;

		_esvg_element_rect_final_width_get(tag, &width);
		*value = esvg_length_string_to(&width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Coord height;

		_esvg_element_rect_final_height_get(tag, &height);
		*value = esvg_length_string_to(&height);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static int * _esvg_element_rect_attribute_animated_fetch(Edom_Tag *t, const char *attr)
{
	Esvg_Element_Rect *thiz;
	int *animated = NULL;

	thiz = _esvg_element_rect_get(t);
	if (!strcmp(attr, "x"))
	{
		animated = &thiz->x.animated;
	}
	else if (!strcmp(attr, "y"))
	{
		animated = &thiz->y.animated;
	}
	else if (!strcmp(attr, "width"))
	{
		animated = &thiz->width.animated;
	}
	else if (!strcmp(attr, "height"))
	{
		animated = &thiz->height.animated;
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

static Enesim_Renderer * _esvg_element_rect_renderer_get(Edom_Tag *t)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	return thiz->r;
}

static Esvg_Element_Setup_Return _esvg_element_rect_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Esvg_Element_Rect *thiz;
	Esvg_Length lx, ly;
	Esvg_Length lw, lh;
	Esvg_Length lrx, lry;

	thiz = _esvg_element_rect_get(t);

	/* position */
	esvg_attribute_animated_length_final_get(&thiz->x, &lx);
	esvg_attribute_animated_length_final_get(&thiz->y, &ly);
	thiz->gx = esvg_coord_final_get(&lx, ctx->viewbox.w, ctx->font_size);
	thiz->gy = esvg_coord_final_get(&ly, ctx->viewbox.h, ctx->font_size);
	/* size */
	esvg_attribute_animated_length_final_get(&thiz->width, &lw);
	esvg_attribute_animated_length_final_get(&thiz->height, &lh);
	thiz->gwidth = esvg_coord_final_get(&lw, ctx->viewbox.w, ctx->font_size);
	thiz->gheight = esvg_coord_final_get(&lh, ctx->viewbox.h, ctx->font_size);
	/* rx and ry */
	esvg_attribute_animated_length_final_get(&thiz->rx, &lrx);
	esvg_attribute_animated_length_final_get(&thiz->ry, &lry);
	thiz->grx = esvg_coord_final_get(&lrx, ctx->viewbox.w, ctx->font_size);
	thiz->gry = esvg_coord_final_get(&lry, ctx->viewbox.h, ctx->font_size);
	/* in case only rx is set, use rx as ry */
	/* FIXME we should use the is_set variant */
	if (!thiz->gry && thiz->grx)
		thiz->gry = thiz->grx;

	/* set the bounds */
	enesim_rectangle_coords_from(&ctx->bounds,
			thiz->gx, thiz->gy, thiz->gwidth, thiz->gheight);

	DBG("calling the setup on the rect (x: %g, y: %g, w: %g, h: %g, rx: %g"
			", ry: %g)", thiz->gx, thiz->gy, thiz->gwidth,
			thiz->gheight, thiz->grx, thiz->gry);
	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_element_rect_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Log **error)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);

	/* set the position */
	enesim_renderer_rectangle_position_set(thiz->r, thiz->gx, thiz->gy);
	/* set the size */
	enesim_renderer_rectangle_size_set(thiz->r, thiz->gwidth, thiz->gheight);

	enesim_renderer_rectangle_corner_radii_set(thiz->r, thiz->grx, thiz->gry);
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

	DBG("fc %08x sc %08x c %08x %g", rctx->fill_color, rctx->stroke_color, rctx->color, rctx->stroke_weight);
	/* base properties */
	enesim_renderer_transformation_set(thiz->r, &ctx->transform);
	enesim_renderer_color_set(thiz->r, rctx->color);

	return EINA_TRUE;
}

static void _esvg_element_rect_free(Edom_Tag *t)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	enesim_renderer_unref(thiz->r);
	free(thiz);
}

#if 0
static Eina_Bool _esvg_element_rect_has_changed(Edom_Tag *t)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
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

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		     = */ NULL,
	/* .child_remove	     = */ NULL,
	/* .attribute_get	     = */ _esvg_element_rect_attribute_get,
	/* .cdata_set		     = */ NULL,
	/* .text_set		     = */ NULL,
	/* .text_get		     = */ NULL,
	/* .free		     = */ _esvg_element_rect_free,
	/* .initialize		     = */ NULL,
	/* .attribute_set	     = */ _esvg_element_rect_attribute_set,
	/* .attribute_animated_fetch = */ _esvg_element_rect_attribute_animated_fetch,
	/* .setup		     = */ _esvg_element_rect_setup,
	/* .renderer_get	     = */ _esvg_element_rect_renderer_get,
	/* .renderer_propagate	     = */ _esvg_element_rect_renderer_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_rect_new(void)
{
	Esvg_Element_Rect *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Element_Rect));
	if (!thiz) return NULL;

	r = enesim_renderer_rectangle_new();
	thiz->r = r;

	/* Default values */
	enesim_renderer_rop_set(thiz->r, ENESIM_BLEND);
	thiz->x.base.v = thiz->x.anim.v = ESVG_COORD_0;
	thiz->y.base.v = thiz->y.anim.v = ESVG_COORD_0;
	thiz->width.base.v = thiz->width.anim.v = ESVG_LENGTH_0;
	thiz->height.base.v = thiz->height.anim.v = ESVG_LENGTH_0;
        /* FIXME: set rx and ry ? */
/*	thiz->current.rx = ESVG_COORD_0; */
/*	thiz->current.ry = ESVG_COORD_0; */

	t = esvg_renderable_new(&_descriptor, ESVG_TYPE_RECT, thiz);
	return t;
}

static void _esvg_element_rect_x_set(Edom_Tag *t, const Esvg_Animated_Coord *x)
{
	Esvg_Element_Rect *thiz;
	Esvg_Coord def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_rect_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->x, x, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_rect_x_get(Edom_Tag *t, Esvg_Animated_Coord *x)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_get(&thiz->x, x);
}

static void _esvg_element_rect_y_set(Edom_Tag *t, const Esvg_Animated_Coord *y)
{
	Esvg_Element_Rect *thiz;
	Esvg_Coord def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_rect_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->y, y, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_rect_y_get(Edom_Tag *t, Esvg_Animated_Coord *y)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_get(&thiz->y, y);
}

static void _esvg_element_rect_width_set(Edom_Tag *t, const Esvg_Length_Animated *width)
{
	Esvg_Element_Rect *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_rect_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->width, width, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_rect_width_get(Edom_Tag *t, Esvg_Length_Animated *width)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_get(&thiz->width, width);
}

static void _esvg_element_rect_height_set(Edom_Tag *t, const Esvg_Length_Animated *height)
{
	Esvg_Element_Rect *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_rect_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->height, height, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_rect_height_get(Edom_Tag *t, Esvg_Length_Animated *height)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_get(&thiz->height, height);
}

static void _esvg_element_rect_rx_set(Edom_Tag *t, const Esvg_Animated_Coord *rx)
{
	Esvg_Element_Rect *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_rect_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->rx, rx, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_rect_rx_get(Edom_Tag *t, Esvg_Animated_Coord *rx)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_get(&thiz->rx, rx);
}

static void _esvg_element_rect_ry_set(Edom_Tag *t, const Esvg_Animated_Coord *ry)
{
	Esvg_Element_Rect *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_element_rect_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->ry, ry, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_element_rect_ry_get(Edom_Tag *t, Esvg_Animated_Coord *ry)
{
	Esvg_Element_Rect *thiz;

	thiz = _esvg_element_rect_get(t);
	esvg_attribute_animated_length_get(&thiz->ry, ry);
}

/* The ender wrapper */
#define _esvg_element_rect_delete NULL
#define _esvg_element_rect_x_is_set NULL
#define _esvg_element_rect_y_is_set NULL
#define _esvg_element_rect_rx_is_set NULL
#define _esvg_element_rect_ry_is_set NULL
#define _esvg_element_rect_width_is_set NULL
#define _esvg_element_rect_height_is_set NULL
#include "esvg_generated_element_rect.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_rect_init(void)
{
	_esvg_element_rect_log = eina_log_domain_register("esvg_element_rect", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_rect_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_rect_init();
}

void esvg_element_rect_shutdown(void)
{
	if (_esvg_element_rect_log < 0)
		return;
	_esvg_element_rect_shutdown();
	eina_log_domain_unregister(_esvg_element_rect_log);
	_esvg_element_rect_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_element_rect_new(void)
{
	return ESVG_ELEMENT_NEW("SVGRectElement");
}

EAPI Eina_Bool esvg_is_rect(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_TYPE_RECT) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_element_rect_x_set(Ender_Element *e, const Esvg_Coord *x)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_RECT_X, x);
}

EAPI void esvg_element_rect_x_get(Ender_Element *e, Esvg_Coord *x)
{
}

EAPI void esvg_element_rect_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_RECT_Y, y);
}

EAPI void esvg_element_rect_y_get(Ender_Element *e, Esvg_Coord *y)
{
}

EAPI void esvg_element_rect_width_set(Ender_Element *e, const Esvg_Length *width)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_RECT_WIDTH, width);
}

EAPI void esvg_element_rect_width_get(Ender_Element *e, Esvg_Length *width)
{
}

EAPI void esvg_element_rect_height_set(Ender_Element *e, const Esvg_Length *height)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_RECT_HEIGHT, height);
}

EAPI void esvg_element_rect_height_get(Ender_Element *e, Esvg_Length *height)
{
}

EAPI void esvg_element_rect_rx_set(Ender_Element *e, const Esvg_Coord *rx)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_RECT_RX, rx);
}

EAPI void esvg_element_rect_rx_get(Ender_Element *e, Esvg_Coord *rx)
{
}

EAPI void esvg_element_rect_ry_set(Ender_Element *e, const Esvg_Coord *ry)
{
	esvg_element_property_length_set(e, ESVG_ELEMENT_RECT_RY, ry);
}

EAPI void esvg_element_rect_ry_get(Ender_Element *e, Esvg_Coord *ry)
{
}
