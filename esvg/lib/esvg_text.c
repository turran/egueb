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

typedef struct _Esvg_Text_State
{
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length font_size;
} Esvg_Text_State;

typedef struct _Esvg_Text
{
	/* properties */
	Esvg_Text_State current;
	Esvg_Text_State past;
	/* private */
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
		return EINA_FALSE;

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
	past = text[length];
	modified = (char *)text;
	modified[length] = '\0';

	DBG("setting text %s", modified);
	etex_span_text_set(thiz->r, modified);
	modified[length] = past;
	thiz->changed = EINA_TRUE;
}

#if 0
static void _esvg_text_text_get(Edom_Tag *t, const char **text)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);
	etex_span_text_get(thiz->t, text);
}
#endif

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
	double x, y;
	double font_size;

	thiz = _esvg_text_get(t);

	x = esvg_length_final_get(&thiz->current.x, ctx->viewbox.width, ctx->font_size);
	y = esvg_length_final_get(&thiz->current.y, ctx->viewbox.height, ctx->font_size);
	/* we should use the hypot(viewbox_w, viewbox_h) */
	font_size = esvg_length_final_get(&thiz->current.font_size, ctx->viewbox.width, ctx->font_size);
	enesim_renderer_origin_set(thiz->r, x, y);
	enesim_renderer_color_set(thiz->r, rctx->fill_color);

	//DBG("calling the setup on the text (%g %g %s %g)", x, y, str, font_size);
	etex_base_size_set(thiz->r, (int)ctx->font_size);

	DBG("calling the setup on the text");

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
	/* .free		= */ _esvg_text_free,
	/* .initialize		= */ NULL,
	/* .attribute_set	= */ _esvg_text_attribute_set,
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
	thiz->current.x = ESVG_COORD_0;
	thiz->current.y = ESVG_COORD_0;
	thiz->current.font_size = ESVG_LENGTH_0;

	t = esvg_renderable_new(&_descriptor, ESVG_TEXT, thiz);
	return t;
}

static void _esvg_text_x_set(Edom_Tag *t, const Esvg_Coord *x)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);
	if (x)
	{
		thiz->current.x = *x;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_text_x_get(Edom_Tag *t, Esvg_Coord *x)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);
	if (x) *x = thiz->current.x;
}

static void _esvg_text_y_set(Edom_Tag *t, const Esvg_Coord *y)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);
	if (y)
	{
		thiz->current.y = *y;
		thiz->changed = EINA_TRUE;
	}
}

static void _esvg_text_y_get(Edom_Tag *t, Esvg_Coord *y)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(t);
	if (y) *y = thiz->current.y;
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
	ender_element_property_value_set(e, ESVG_TEXT_X, &x, NULL);
}

EAPI void esvg_text_x_get(Ender_Element *e, Esvg_Coord *x)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_text_x_get(t, x);
}

EAPI void esvg_text_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	ender_element_property_value_set(e, ESVG_TEXT_Y, &y, NULL);
}

EAPI void esvg_text_y_get(Ender_Element *e, Esvg_Coord *y)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_text_y_get(t, y);
}

EAPI void esvg_text_text_set(Ender_Element *e, const char *text)
{
	/* set the edom TEXT */
}

EAPI void esvg_text_text_get(Ender_Element *e, const char **text)
{
}
