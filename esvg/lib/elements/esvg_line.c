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
#include "esvg_line.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_line_log

static int _esvg_line_log = -1;

static Ender_Property *ESVG_LINE_X1;
static Ender_Property *ESVG_LINE_Y1;
static Ender_Property *ESVG_LINE_X2;
static Ender_Property *ESVG_LINE_Y2;

typedef struct _Esvg_Line
{
	/* properties */
	Esvg_Attribute_Animated_Coord x1;
	Esvg_Attribute_Animated_Coord x2;
	Esvg_Attribute_Animated_Coord y1;
	Esvg_Attribute_Animated_Coord y2;
	/* private */
	/* generated at setup */
	double gx1;
	double gy1;
	double gx2;
	double gy2;
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Line;

static Esvg_Line * _esvg_line_get(Edom_Tag *t)
{
	Esvg_Line *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_LINE)
		return NULL;
	thiz = esvg_renderable_data_get(t);

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

		esvg_length_string_from(&x, value);
		esvg_line_x1_set(e, &x);
	}
	else if (strcmp(key, "y1") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_line_y1_set(e, &y);
	}
	else if (strcmp(key, "x2") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_line_x2_set(e, &x);
	}
	else if (strcmp(key, "y2") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_line_y2_set(e, &y);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_line_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static int * _esvg_line_attribute_animated_fetch(Edom_Tag *t, const char *attr)
{
	Esvg_Line *thiz;
	int *animated = NULL;

	thiz = _esvg_line_get(t);
	if (!strcmp(attr, "x1"))
	{
		animated = &thiz->x1.animated;
	}
	else if (!strcmp(attr, "y1"))
	{
		animated = &thiz->y1.animated;
	}
	else if (!strcmp(attr, "x2"))
	{
		animated = &thiz->x2.animated;
	}
	else if (!strcmp(attr, "y2"))
	{
		animated = &thiz->y2.animated;
	}
	return animated;
}

static Enesim_Renderer * _esvg_line_renderer_get(Edom_Tag *t)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	return thiz->r;
}

static Esvg_Element_Setup_Return _esvg_line_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Line *thiz;
	Esvg_Length lx1, ly1, lx2, ly2;

	thiz = _esvg_line_get(t);
	/* the position */
	esvg_attribute_animated_length_final_get(&thiz->x1, &lx1);
	esvg_attribute_animated_length_final_get(&thiz->y1, &ly1);
	esvg_attribute_animated_length_final_get(&thiz->x2, &lx2);
	esvg_attribute_animated_length_final_get(&thiz->y2, &ly2);
	thiz->gx1 = esvg_coord_final_get(&lx1, ctx->viewbox.width, ctx->font_size);
	thiz->gy1 = esvg_coord_final_get(&ly1, ctx->viewbox.height, ctx->font_size);
	thiz->gx2 = esvg_coord_final_get(&lx2, ctx->viewbox.width, ctx->font_size);
	thiz->gy2 = esvg_coord_final_get(&ly2, ctx->viewbox.height, ctx->font_size);
	/* the bounds */
	enesim_rectangle_coords_from(&ctx->bounds,
			thiz->gx1 < thiz->gx2 ? thiz->gx1 : thiz->gx2,
			thiz->gy1 < thiz->gy2 ? thiz->gy1 : thiz->gy2,
			fabs(thiz->gx1 - thiz->gx2),
			fabs(thiz->gy1 - thiz->gy2));

	DBG("calling the setup on the line (%g %g %g %g)", thiz->gx1, thiz->gy1, thiz->gx2, thiz->gy2);

	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_line_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);

	enesim_renderer_line_x0_set(thiz->r, thiz->gx1);
	enesim_renderer_line_y0_set(thiz->r, thiz->gy1);
	enesim_renderer_line_x1_set(thiz->r, thiz->gx2);
	enesim_renderer_line_y1_set(thiz->r, thiz->gy2);

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

	return EINA_TRUE;
}

static void _esvg_line_free(Edom_Tag *t)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	enesim_renderer_unref(thiz->r);
	free(thiz);
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

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		     = */ NULL,
	/* .child_remove	     = */ NULL,
	/* .attribute_get 	     = */ _esvg_line_attribute_get,
	/* .cdata_set 		     = */ NULL,
	/* .text_set 		     = */ NULL,
	/* .text_get		     = */ NULL,
	/* .free 		     = */ _esvg_line_free,
	/* .initialize 		     = */ NULL,
	/* .attribute_set 	     = */ _esvg_line_attribute_set,
	/* .attribute_animated_fetch = */ _esvg_line_attribute_animated_fetch,
	/* .setup		     = */ _esvg_line_setup,
	/* .renderer_get	     = */ _esvg_line_renderer_get,
	/* .renderer_propagate	     = */ _esvg_line_renderer_propagate,
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
	thiz->x1.base.v = thiz->x1.anim.v = ESVG_COORD_0;
	thiz->y1.base.v = thiz->y1.anim.v = ESVG_COORD_0;
	thiz->x2.base.v = thiz->x2.anim.v = ESVG_COORD_0;
	thiz->y2.base.v = thiz->y2.anim.v = ESVG_COORD_0;

	t = esvg_renderable_new(&_descriptor, ESVG_LINE, thiz);
	return t;
}

static void _esvg_line_x1_set(Edom_Tag *t, const Esvg_Animated_Coord *x1)
{
	Esvg_Line *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_line_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->x1, x1, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_line_x1_get(Edom_Tag *t, Esvg_Animated_Coord *x1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	esvg_attribute_animated_length_get(&thiz->x1, x1);
}

static void _esvg_line_y1_set(Edom_Tag *t, const Esvg_Animated_Coord *y1)
{
	Esvg_Line *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_line_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->y1, y1, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_line_y1_get(Edom_Tag *t, Esvg_Animated_Coord *y1)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	esvg_attribute_animated_length_get(&thiz->y1, y1);
}

static void _esvg_line_x2_set(Edom_Tag *t, const Esvg_Animated_Coord *x2)
{
	Esvg_Line *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_line_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->x2, x2, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_line_x2_get(Edom_Tag *t, Esvg_Animated_Coord *x2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	esvg_attribute_animated_length_get(&thiz->x2, x2);
}

static void _esvg_line_y2_set(Edom_Tag *t, const Esvg_Animated_Coord *y2)
{
	Esvg_Line *thiz;
	Esvg_Length def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_line_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->y2, y2, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_line_y2_get(Edom_Tag *t, Esvg_Animated_Coord *y2)
{
	Esvg_Line *thiz;

	thiz = _esvg_line_get(t);
	esvg_attribute_animated_length_get(&thiz->y2, y2);
}

/* The ender wrapper */
#define _esvg_line_delete NULL
#define _esvg_line_x1_is_set NULL
#define _esvg_line_x2_is_set NULL
#define _esvg_line_y1_is_set NULL
#define _esvg_line_y2_is_set NULL

#include "generated/esvg_generated_line.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_line_init(void)
{
	_esvg_line_log = eina_log_domain_register("esvg_line", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_line_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_line_init();
}

void esvg_line_shutdown(void)
{
	if (_esvg_line_log < 0)
		return;
	_esvg_line_shutdown();
	eina_log_domain_unregister(_esvg_line_log);
	_esvg_line_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_line_new(void)
{
	return ESVG_ELEMENT_NEW("line");
}

EAPI Eina_Bool esvg_is_line(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_LINE) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_line_x1_set(Ender_Element *e, const Esvg_Coord *x1)
{
	esvg_element_property_length_set(e, ESVG_LINE_X1, x1);
}

EAPI void esvg_line_x1_get(Ender_Element *e, Esvg_Coord *x1)
{
}

EAPI void esvg_line_y1_set(Ender_Element *e, const Esvg_Coord *y1)
{
	esvg_element_property_length_set(e, ESVG_LINE_Y1, y1);
}

EAPI void esvg_line_y1_get(Ender_Element *e, Esvg_Coord *y1)
{
}

EAPI void esvg_line_x2_set(Ender_Element *e, const Esvg_Coord *x2)
{
	esvg_element_property_length_set(e, ESVG_LINE_X2, x2);
}

EAPI void esvg_line_x2_get(Ender_Element *e, Esvg_Coord *x2)
{
}

EAPI void esvg_line_y2_set(Ender_Element *e, const Esvg_Coord *y2)
{
	esvg_element_property_length_set(e, ESVG_LINE_Y2, y2);
}

EAPI void esvg_line_y2_get(Ender_Element *e, Esvg_Coord *y2)
{
}
