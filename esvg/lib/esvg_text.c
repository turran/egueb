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
#include "esvg_text.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_text

static Ender_Property *ESVG_TEXT_X;
static Ender_Property *ESVG_TEXT_Y;

typedef struct _Esvg_Text
{
	/* properties */
	Esvg_Attribute_Animated_Coord x;
	Esvg_Attribute_Animated_Coord y;
	/* private */
	double gx;
	double gy;
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Text;

static Esvg_Text * _esvg_text_get(Edom_Tag *t)
{
	Esvg_Text *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TEXT)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_text_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_text_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_text_y_set(e, &y);
	}
#if 0
	else if (strcmp(key, "font-size") == 0)
	{
		Esvg_Length font_size;

		esvg_length_string_from(&font_size, value, ESVG_COORD_0);
		esvg_text_font_size_set(r, &font_size);
	}
#endif
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_text_attribute_get(Edom_Tag *tag,
		const char *attribute, char **value)
{
	return EINA_FALSE;
}


static void _esvg_text_text_set(Edom_Tag *t, const char *text, unsigned int length)
{
	Esvg_Text *thiz;
	char *modified;
	char past;

	thiz = _esvg_text_get(t);
	if (!text) return;

	past = text[length];
	modified = (char *)text;
	modified[length] = '\0';

	DBG("setting text %s", modified);
	etex_span_text_set(thiz->r, modified);
	modified[length] = past;
	thiz->changed = EINA_TRUE;
}

static void _esvg_text_text_get(Edom_Tag *t, const char **text, unsigned int *length)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);

	DBG("getting text");
	etex_span_text_get(thiz->r, text);
	*length = 0;
}

static Enesim_Renderer * _esvg_text_renderer_get(Edom_Tag *t)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);
	return thiz->r;
}

static Esvg_Element_Setup_Return _esvg_text_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Text *thiz;
	Esvg_Length lx, ly;

	thiz = _esvg_text_get(t);

	/* position */
	esvg_attribute_animated_length_final_get(&thiz->x, &lx);
	esvg_attribute_animated_length_final_get(&thiz->y, &ly);
	thiz->gx = esvg_length_final_get(&lx, ctx->viewbox.width, ctx->font_size);
	thiz->gy = esvg_length_final_get(&ly, ctx->viewbox.height, ctx->font_size);

	DBG("calling the setup on the text (%g %g)", thiz->gx, thiz->gy);

	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_text_renderer_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Text *thiz;
	Enesim_Matrix inv;
	int max;

	thiz = _esvg_text_get(t);

	etex_base_size_set(thiz->r, (int)ctx->font_size);
	etex_base_max_ascent_get(thiz->r, &max);

	enesim_renderer_origin_set(thiz->r, thiz->gx, thiz->gy - max);
	enesim_renderer_color_set(thiz->r, rctx->fill_color);

	DBG("matrix %" ENESIM_MATRIX_FORMAT, ENESIM_MATRIX_ARGS (&ctx->transform));
	enesim_matrix_inverse(&ctx->transform, &inv);
	enesim_renderer_transformation_set(thiz->r, &inv);

	return EINA_TRUE;
}

#if 0
static Eina_Bool _esvg_text_has_changed(Edom_Tag *t)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (!thiz->changed) return EINA_FALSE;

	if (esvg_length_is_equal(&thiz->current.x, &thiz->past.x))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.y, &thiz->past.y))
		return EINA_TRUE;
	if (esvg_length_is_equal(&thiz->current.font_size, &thiz->past.font_size))
		return EINA_TRUE;

	return EINA_FALSE;
}
#endif

static void _esvg_text_free(Edom_Tag *t)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);
	enesim_renderer_unref(thiz->r);
	free(thiz);
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get	= */ _esvg_text_attribute_get,
	/* .cdata_set		= */ NULL,
	/* .text_set		= */ _esvg_text_text_set,
	/* .text_get		     = */ _esvg_text_text_get,
	/* .free		= */ _esvg_text_free,
	/* .initialize		= */ NULL,
	/* .attribute_set	= */ _esvg_text_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ _esvg_text_setup,
	/* .renderer_get	= */ _esvg_text_renderer_get,
	/* .renderer_propagate	= */ _esvg_text_renderer_propagate,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_text_new(void)
{
	Esvg_Text *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Text));
	if (!thiz) return NULL;

	r = etex_span_new();
	etex_base_font_name_set(r, "/usr/share/fonts/truetype/freefont/FreeSans.ttf");
	enesim_renderer_color_set(r, 0xff000000);
	etex_span_text_set(r, "Hello");
	thiz->r = r;

	/* Default values */
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	/* FIXME for now */
	thiz->x.base.v = thiz->x.anim.v = ESVG_COORD_0;
	thiz->y.base.v = thiz->y.anim.v = ESVG_COORD_0;

	t = esvg_renderable_new(&_descriptor, ESVG_TEXT, thiz);
	return t;
}

static void _esvg_text_x_set(Edom_Tag *t, const Esvg_Animated_Coord *x)
{
	Esvg_Text *thiz;
	Esvg_Coord def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_text_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->x, x, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_text_x_get(Edom_Tag *t, Esvg_Animated_Coord *x)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);
	esvg_attribute_animated_length_get(&thiz->x, x);
}

static void _esvg_text_y_set(Edom_Tag *t, const Esvg_Animated_Coord *y)
{
	Esvg_Text *thiz;
	Esvg_Coord def = { ESVG_UNIT_LENGTH_PX, 0 };
	Eina_Bool animating;

	thiz = _esvg_text_get(t);
	animating = esvg_element_attribute_animate_get(t);
	esvg_attribute_animated_length_set(&thiz->y, y, &def, animating);
	thiz->changed = EINA_TRUE;
}

static void _esvg_text_y_get(Edom_Tag *t, Esvg_Animated_Coord *y)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);
	esvg_attribute_animated_length_get(&thiz->y, y);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_text_delete NULL
#define _esvg_text_x_is_set NULL
#define _esvg_text_y_is_set NULL
#include "generated/esvg_generated_text.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_text_new(void)
{
	return ESVG_ELEMENT_NEW("text");
}

EAPI Eina_Bool esvg_is_text(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_TEXT) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_text_x_set(Ender_Element *e, const Esvg_Coord *x)
{
	esvg_element_property_length_set(e, ESVG_TEXT_X, x);
}

EAPI void esvg_text_x_get(Ender_Element *e, Esvg_Coord *x)
{
}

EAPI void esvg_text_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	esvg_element_property_length_set(e, ESVG_TEXT_Y, y);
}

EAPI void esvg_text_y_get(Ender_Element *e, Esvg_Coord *y)
{
}

EAPI void esvg_text_text_set(Ender_Element *e, const char *text)
{
	/* set the edom TEXT */
}

EAPI void esvg_text_text_get(Ender_Element *e, const char **text)
{
}
