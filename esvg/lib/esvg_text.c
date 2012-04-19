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
#include "Esvg.h"
#include "esvg_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_TEXT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_TEXT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_TEXT_MAGIC);\
	} while(0)

typedef struct _Esvg_Text_State
{
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length font_size;
} Esvg_Text_State;

typedef struct _Esvg_Text
{
	EINA_MAGIC
	/* properties */
	Esvg_Text_State current;
	Esvg_Text_State past;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool changed : 1;
} Esvg_Text;

static Esvg_Text * _esvg_text_get(Enesim_Renderer *r)
{
	Esvg_Text *thiz;

	thiz = esvg_shape_data_get(r);
	ESVG_TEXT_MAGIC_CHECK(thiz);
	return thiz;
}
#if 0
static Eina_Bool _esvg_rect_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value, ESVG_COORD_0);
		esvg_text_x_set(r, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value, ESVG_COORD_0);
		esvg_text_y_set(r, &y);
	}
	else if (strcmp(key, "font-size") == 0)
	{
		Esvg_Length font_size;

		esvg_length_string_from(&font_size, value, ESVG_COORD_0);
		esvg_text_font_size_set(r, &font_size);
	}
}
#endif
/*----------------------------------------------------------------------------*
 *                         Esvg Element interface                             *
 *----------------------------------------------------------------------------*/
static const char * _esvg_text_name_get(Enesim_Renderer *r)
{
	return "text";
}

static Enesim_Renderer * _esvg_text_renderer_get(Enesim_Renderer *r)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	return thiz->r;
}

static Eina_Bool _esvg_text_setup(Enesim_Renderer *r, const Esvg_Element_Context *estate,
		const Esvg_Renderable_Context *dstate)
{
	Esvg_Text *thiz;
	Enesim_Matrix inv;
	double x, y;
	double font_size;

	thiz = _esvg_text_get(r);

	x = esvg_length_final_get(&thiz->current.x, estate->viewbox_w);
	y = esvg_length_final_get(&thiz->current.y, estate->viewbox_h);
	/* we should use the hypot(viewbox_w, viewbox_h) */
	font_size = esvg_length_final_get(&thiz->current.font_size, estate->viewbox_w);
	enesim_renderer_origin_set(thiz->r, x, y);
	enesim_renderer_color_set(thiz->r, dstate->fill_color);

	//printf("calling the setup on the text (%g %g %s %g)\n", x, y, str, font_size);
	etex_base_size_set(thiz->r, (int)font_size);

	enesim_matrix_inverse(&estate->transform, &inv);
	enesim_renderer_transformation_set(thiz->r, &inv);

	return EINA_TRUE;
}

static void _esvg_text_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{

}


static void _esvg_text_cleanup(Enesim_Renderer *r)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	thiz->past = thiz->current;
	thiz->changed = EINA_FALSE;
}

static Eina_Bool _esvg_text_has_changed(Enesim_Renderer *r)
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

static Esvg_Shape_Descriptor _descriptor = {
	/* .setup =		*/ _esvg_text_setup,
	/* .renderer_get =	*/ _esvg_text_renderer_get,
	/* .name_get =		*/ _esvg_text_name_get,
	/* .clone =		*/ _esvg_text_clone,
	/* .cleanup =		*/ _esvg_text_cleanup,
	/* .has_changed	=	*/ _esvg_text_has_changed
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_text_new(void)
{
	Esvg_Text *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Text));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_TEXT_MAGIC);

	r = etex_span_new();
	thiz->r = r;

	/* Default values */
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	/* FIXME for now */
	thiz->current.x = ESVG_COORD_0;
	thiz->current.y = ESVG_COORD_0;
	thiz->current.font_size = ESVG_LENGTH_0;

	etex_base_font_name_set(r, "/usr/share/fonts/truetype/freefont/FreeSans.ttf");
	enesim_renderer_color_set(r, 0xff000000);
	etex_span_text_set(r, "Hello");

	r = esvg_shape_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool esvg_is_text(Enesim_Renderer *r)
{
	Esvg_Text *thiz;
	Eina_Bool ret;

	if (!esvg_is_shape(r))
		return EINA_FALSE;
	thiz = esvg_shape_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_TEXT_MAGIC);

	return ret;
}

EAPI void esvg_text_x_set(Enesim_Renderer *r, const Esvg_Coord *x)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (x)
	{
		thiz->current.x = *x;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_text_x_get(Enesim_Renderer *r, Esvg_Coord *x)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (x) *x = thiz->current.x;
}

EAPI void esvg_text_y_set(Enesim_Renderer *r, const Esvg_Coord *y)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (y)
	{
		thiz->current.y = *y;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_text_y_get(Enesim_Renderer *r, Esvg_Coord *y)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (y) *y = thiz->current.y;
}

EAPI void esvg_text_font_size_set(Enesim_Renderer *r, const Esvg_Length *size)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (size)
	{
		thiz->current.font_size = *size;
		thiz->changed = EINA_TRUE;
	}
}

EAPI void esvg_text_font_size_get(Enesim_Renderer *r, Esvg_Length *size)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (size) *size = thiz->current.font_size;
}

EAPI void esvg_text_font_family_set(Enesim_Renderer *r, const char *family)
{

}

EAPI void esvg_text_text_set(Enesim_Renderer *r, const char *text)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	etex_span_text_set(thiz->r, text);
	thiz->changed = EINA_TRUE;
}

EAPI void esvg_text_text_get(Enesim_Renderer *r, const char **text)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	etex_span_text_get(thiz->r, text);
}
