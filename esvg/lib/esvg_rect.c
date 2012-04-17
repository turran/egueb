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
#include "esvg_rect.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_RECT_X;
static Ender_Property *ESVG_RECT_Y;
static Ender_Property *ESVG_RECT_RX;
static Ender_Property *ESVG_RECT_RY;
static Ender_Property *ESVG_RECT_WIDTH;
static Ender_Property *ESVG_RECT_HEIGHT;

typedef struct _Esvg_Rect_State
{
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	Esvg_Length rx;
	Esvg_Length ry;
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

	if (esvg_element_type_get_internal(t) != ESVG_RECT)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_rect_attribute_set(Ender_Element *e, const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_get(&x, value, ESVG_COORD_0);
		esvg_rect_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_get(&y, value, ESVG_COORD_0);
		esvg_rect_y_set(e, &y);
	}
	else if (strcmp(key, "rx") == 0)
	{
		Esvg_Coord rx;

		esvg_length_get(&rx, value, ESVG_COORD_0);
		esvg_rect_rx_set(e, &rx);
	}
	else if (strcmp(key, "ry") == 0)
	{
		Esvg_Coord ry;

		esvg_length_get(&ry, value, ESVG_COORD_0);
		esvg_rect_ry_set(e, &ry);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_get(&width, value, ESVG_LENGTH_0);
		esvg_rect_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_get(&height, value, ESVG_LENGTH_0);
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

static Eina_Bool _esvg_rect_setup(Edom_Tag *t,
		const Esvg_Element_Context *estate,
		const Esvg_Shape_Enesim_State *dstate)
{
	Esvg_Rect *thiz;
	double x, y;
	double rx, ry;
	double width, height;

	thiz = _esvg_rect_get(t);

	/* set the position */
	x = esvg_length_final_get(&thiz->current.x, estate->viewbox.width);
	y = esvg_length_final_get(&thiz->current.y, estate->viewbox.height);
	enesim_renderer_rectangle_position_set(thiz->r, x, y);
	/* set the size */
	width = esvg_length_final_get(&thiz->current.width, estate->viewbox.width);
	height = esvg_length_final_get(&thiz->current.height, estate->viewbox.height);
	enesim_renderer_rectangle_size_set(thiz->r, width, height);

	/* FIXME enesim does not supports rx *and* ry */
	rx = esvg_length_final_get(&thiz->current.rx, estate->viewbox.width);
	enesim_renderer_rectangle_corner_radius_set(thiz->r, rx);
	enesim_renderer_rectangle_corners_set(thiz->r,
			EINA_TRUE, EINA_TRUE, EINA_TRUE, EINA_TRUE);

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

	printf("calling the setup on the rect (%g %g %g %g) %g\n", x, y, width, height, dstate->stroke_weight);
	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &estate->transform);
	enesim_renderer_color_set(thiz->r, dstate->color);

	return EINA_TRUE;
}

static void _esvg_rect_clone(Edom_Tag *t, Edom_Tag *tr)
{
	Esvg_Rect *thiz;
	Esvg_Rect *other;

	thiz = _esvg_rect_get(t);
	other = _esvg_rect_get(tr);

	other->current.x = thiz->current.x;
	other->current.y = thiz->current.y;
	other->current.width = thiz->current.width;
	other->current.height = thiz->current.height;
	other->current.rx = thiz->current.rx;
	other->current.ry = thiz->current.ry;
}

static void _esvg_rect_cleanup(Edom_Tag *t)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	thiz->past = thiz->current;
	thiz->changed = EINA_FALSE;
}

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

void _esvg_rect_free(Edom_Tag *t)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	free(thiz);
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_rect_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_rect_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_rect_attribute_set,
	/* .clone		= */ _esvg_rect_clone,
	/* .setup		= */ _esvg_rect_setup,
	/* .renderer_get	= */ _esvg_rect_renderer_get,
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
	thiz->current.x = ESVG_COORD_0;
	thiz->current.y = ESVG_COORD_0;
	thiz->current.width = ESVG_LENGTH_0;
	thiz->current.height = ESVG_LENGTH_0;
        /* FIXME: set rx and ry ? */
/* 	thiz->current.rx = ESVG_COORD_0; */
/* 	thiz->current.ry = ESVG_COORD_0; */

	t = esvg_renderable_new(&_descriptor, ESVG_RECT, thiz);
	return t;
}

static void _esvg_rect_x_set(Edom_Tag *t, const Esvg_Coord *x)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (x)
	{
		thiz->current.x = *x;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_rect_x_get(Edom_Tag *t, Esvg_Coord *x)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (x) *x = thiz->current.x;
}

static void _esvg_rect_y_set(Edom_Tag *t, const Esvg_Coord *y)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (y)
	{
		thiz->current.y = *y;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_rect_y_get(Edom_Tag *t, Esvg_Coord *y)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (y) *y = thiz->current.y;
}

static void _esvg_rect_width_set(Edom_Tag *t, const Esvg_Length *width)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (width)
	{
		thiz->current.width = *width;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_rect_width_get(Edom_Tag *t, Esvg_Length *width)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (width) *width = thiz->current.width;
}

static void _esvg_rect_height_set(Edom_Tag *t, const Esvg_Length *height)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (height)
	{
		thiz->current.height = *height;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_rect_height_get(Edom_Tag *t, Esvg_Length *height)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (height) *height = thiz->current.height;
}

static void _esvg_rect_rx_set(Edom_Tag *t, const Esvg_Coord *rx)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (rx)
	{
		thiz->current.rx = *rx;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_rect_rx_get(Edom_Tag *t, Esvg_Coord *rx)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (rx) *rx = thiz->current.rx;
}

static void _esvg_rect_ry_set(Edom_Tag *t, const Esvg_Coord *ry)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (ry)
	{
		thiz->current.ry = *ry;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_rect_ry_get(Edom_Tag *t, Esvg_Coord *ry)
{
	Esvg_Rect *thiz;

	thiz = _esvg_rect_get(t);
	if (ry) *ry = thiz->current.ry;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_rect.c"

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_rect_new(void)
{
	return ender_element_new_with_namespace("rect", "esvg");
}

EAPI Eina_Bool esvg_is_rect(Ender_Element *e)
{
	Eina_Bool ret = EINA_TRUE;

	return ret;
}

EAPI void esvg_rect_x_set(Ender_Element *e, const Esvg_Coord *x)
{
	ender_element_property_value_set(e, ESVG_RECT_X, x, NULL);
}

EAPI void esvg_rect_x_get(Ender_Element *e, Esvg_Coord *x)
{
}

EAPI void esvg_rect_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	ender_element_property_value_set(e, ESVG_RECT_Y, y, NULL);
}

EAPI void esvg_rect_y_get(Ender_Element *e, Esvg_Coord *y)
{
}

EAPI void esvg_rect_width_set(Ender_Element *e, const Esvg_Length *width)
{
	ender_element_property_value_set(e, ESVG_RECT_WIDTH, width, NULL);
}

EAPI void esvg_rect_width_get(Ender_Element *e, Esvg_Length *width)
{
}

EAPI void esvg_rect_height_set(Ender_Element *e, const Esvg_Length *height)
{
	ender_element_property_value_set(e, ESVG_RECT_HEIGHT, height, NULL);
}

EAPI void esvg_rect_height_get(Ender_Element *e, Esvg_Length *height)
{
}

EAPI void esvg_rect_rx_set(Ender_Element *e, const Esvg_Coord *rx)
{
	ender_element_property_value_set(e, ESVG_RECT_RX, rx, NULL);
}

EAPI void esvg_rect_rx_get(Ender_Element *e, Esvg_Coord *rx)
{
}

EAPI void esvg_rect_ry_set(Ender_Element *e, const Esvg_Coord *ry)
{
	ender_element_property_value_set(e, ESVG_RECT_RY, ry, NULL);
}

EAPI void esvg_rect_ry_get(Ender_Element *e, Esvg_Coord *ry)
{
}
