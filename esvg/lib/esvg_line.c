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
#include "esvg_line.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_LINE_X1;
static Ender_Property *ESVG_LINE_Y1;
static Ender_Property *ESVG_LINE_X2;
static Ender_Property *ESVG_LINE_Y2;

typedef struct _Esvg_Line_State
{
	Esvg_Coord x1;
	Esvg_Coord x2;
	Esvg_Coord y1;
	Esvg_Coord y2;
} Esvg_Line_State;

typedef struct _Esvg_Line
{
	/* properties */
	Esvg_Line_State current;
	Esvg_Line_State past;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Line;

static Esvg_Line * _esvg_line_get(Edom_Tag *t)
{
	Esvg_Line *thiz;

	if (esvg_element_type_get_internal(t) != ESVG_LINE)
		return NULL;
	thiz = esvg_instantiable_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_line_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x1") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value, ESVG_COORD_0);
		esvg_line_x1_set(e, &x);
	}
	else if (strcmp(key, "y1") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value, ESVG_COORD_0);
		esvg_line_y1_set(e, &y);
	}
	else if (strcmp(key, "x2") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value, ESVG_COORD_0);
		esvg_line_x2_set(e, &x);
	}
	else if (strcmp(key, "y2") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value, ESVG_COORD_0);
		esvg_line_y2_set(e, &y);
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_line_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_line_renderer_get(Edom_Tag *t)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	return thiz->r;
}

static Eina_Bool _esvg_line_setup(Edom_Tag *t,
		Esvg_Element_Context *ctx,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Line *thiz;
	double x1, y1, x2, y2;

	thiz = _esvg_line_get(t);

	/* FIXME gets the parents size or the topmost? */
	x1 = esvg_length_final_get(&thiz->current.x1, ctx->viewbox.width);
	y1 = esvg_length_final_get(&thiz->current.y1, ctx->viewbox.height);
	x2 = esvg_length_final_get(&thiz->current.x2, ctx->viewbox.width);
	y2 = esvg_length_final_get(&thiz->current.y2, ctx->viewbox.height);

	enesim_renderer_line_x0_set(thiz->r, x1);
	enesim_renderer_line_y0_set(thiz->r, y1);
	enesim_renderer_line_x1_set(thiz->r, x2);
	enesim_renderer_line_y1_set(thiz->r, y2);

	/* shape properties */
	enesim_renderer_shape_fill_color_set(thiz->r, rctx->fill_color);
	enesim_renderer_shape_stroke_color_set(thiz->r, rctx->stroke_color);
	enesim_renderer_shape_stroke_weight_set(thiz->r, rctx->stroke_weight);
	enesim_renderer_shape_stroke_location_set(thiz->r, ENESIM_SHAPE_STROKE_CENTER);
	enesim_renderer_shape_stroke_cap_set(thiz->r, rctx->stroke_cap);
	enesim_renderer_shape_stroke_join_set(thiz->r, rctx->stroke_join);
	enesim_renderer_shape_draw_mode_set(thiz->r, rctx->draw_mode);

	/* base properties */
	enesim_renderer_geometry_transformation_set(thiz->r, &ctx->transform);
	enesim_renderer_color_set(thiz->r, rctx->color);

	//printf("calling the setup on the line (%g %g %g %g | %g %d)\n", x1, y1, x2, y2, dstate.stroke_weight, dstate.stroke_cap);
	return EINA_TRUE;
}

static void _esvg_line_clone(Edom_Tag *t, Edom_Tag *dt)
{
	Esvg_Line *thiz;
	Esvg_Line *other;

	thiz = _esvg_line_get(t);
	other = _esvg_line_get(dt);

	other->current.x1 = thiz->current.x1;
	other->current.y1 = thiz->current.y1;
	other->current.x2 = thiz->current.x2;
	other->current.y2 = thiz->current.y2;
}

#if 0
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
#endif

static void _esvg_line_free(Edom_Tag *t)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	free(thiz);
}

static Esvg_Instantiable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_line_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_line_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_line_attribute_set,
	/* .clone		= */ _esvg_line_clone,
	/* .setup		= */ _esvg_line_setup,
	/* .renderer_get	= */ _esvg_line_renderer_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_line_new(void)
{
	Esvg_Line *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Line));
	if (!thiz) return NULL;

	r = enesim_renderer_line_new();
	thiz->r = r;
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	/* default values */
	/* FIXME defualt values */

	t = esvg_instantiable_new(&_descriptor, ESVG_LINE, thiz);
	return t;
}

static void _esvg_line_x1_set(Edom_Tag *t, const Esvg_Coord *x1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	if (x1)
	{
		thiz->current.x1 = *x1;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_line_x1_get(Edom_Tag *t, Esvg_Coord *x1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	if (x1) *x1 = thiz->current.x1;
}

static void _esvg_line_y1_set(Edom_Tag *t, const Esvg_Coord *y1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	if (y1)
	{
		thiz->current.y1 = *y1;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_line_y1_get(Edom_Tag *t, Esvg_Coord *y1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	if (y1) *y1 = thiz->current.y1;
}

static void _esvg_line_x2_set(Edom_Tag *t, const Esvg_Coord *x2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	if (x2)
	{
		thiz->current.x2 = *x2;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_line_x2_get(Edom_Tag *t, Esvg_Coord *x2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	if (x2) *x2 = thiz->current.x2;
}

static void _esvg_line_y2_set(Edom_Tag *t, const Esvg_Coord *y2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	if (y2)
	{
		thiz->current.y2 = *y2;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_line_y2_get(Edom_Tag *t, Esvg_Coord *y2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	if (y2) *y2 = thiz->current.y2;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_line.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_line_new(void)
{
	return ender_element_new_with_namespace("line", "esvg");
}

EAPI Eina_Bool esvg_is_line(Ender_Element *e)
{
}

EAPI void esvg_line_x1_set(Ender_Element *e, const Esvg_Coord *x1)
{
	ender_element_property_value_set(e, ESVG_LINE_X1, x1, NULL);
}

EAPI void esvg_line_x1_get(Ender_Element *e, Esvg_Coord *x1)
{
}

EAPI void esvg_line_y1_set(Ender_Element *e, const Esvg_Coord *y1)
{
	ender_element_property_value_set(e, ESVG_LINE_Y1, y1, NULL);
}

EAPI void esvg_line_y1_get(Ender_Element *e, Esvg_Coord *y1)
{
}

EAPI void esvg_line_x2_set(Ender_Element *e, const Esvg_Coord *x2)
{
	ender_element_property_value_set(e, ESVG_LINE_X2, x2, NULL);
}

EAPI void esvg_line_x2_get(Ender_Element *e, Esvg_Coord *x2)
{
}

EAPI void esvg_line_y2_set(Ender_Element *e, const Esvg_Coord *y2)
{
	ender_element_property_value_set(e, ESVG_LINE_Y2, y2, NULL);
}

EAPI void esvg_line_y2_get(Ender_Element *e, Esvg_Coord *y2)
{
}
