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
typedef struct _Esvg_Text
{
	/* properties */
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length font_size;
	/* private */
	Enesim_Renderer *r;
} Esvg_Text;

static Esvg_Text * _esvg_text_get(Enesim_Renderer *r)
{
	Esvg_Text *thiz;
	/* FIXME add some kind of type checking or make this a macro */
	thiz = esvg_shape_data_get(r);
	return thiz;
}

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

static Eina_Bool _esvg_text_setup(Enesim_Renderer *r, const Esvg_Element_State *estate,
		const Esvg_Shape_Enesim_State *dstate)
{
	Esvg_Text *thiz;
	Enesim_Matrix inv;
	double x, y;
	double font_size;
	int fz;
	char *str = NULL;

	thiz = _esvg_text_get(r);

	x = esvg_length_final_get(&thiz->x, estate->viewbox_w);
	y = esvg_length_final_get(&thiz->y, estate->viewbox_h);
	/* we should use the hypot(viewbox_w, viewbox_h) */
	font_size = esvg_length_final_get(&thiz->font_size, estate->viewbox_w);
	enesim_renderer_origin_set(thiz->r, x, y);

	//printf("calling the setup on the text (%g %g %s %g)\n", x, y, str, font_size);
	etex_base_size_set(thiz->r, (int)font_size);

	enesim_matrix_inverse(&estate->transform, &inv);
	enesim_renderer_geometry_transformation_set(thiz->r, &inv);

	return EINA_TRUE;
}

static void _esvg_text_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{

}


static void _esvg_text_cleanup(Enesim_Renderer *r)
{

}

static Esvg_Shape_Descriptor _descriptor = {
	/* .setup =		*/ _esvg_text_setup,
	/* .renderer_get =	*/ _esvg_text_renderer_get,
	/* .name_get =		*/ _esvg_text_name_get,
	/* .clone =		*/ _esvg_text_clone,
	/* .cleanup =		*/ _esvg_text_cleanup,
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

	r = etex_span_new();
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	/* FIXME for now */
	thiz->font_size = ESVG_LENGTH_0;

	etex_base_font_name_set(r, "/usr/share/fonts/truetype/freefont/FreeSans.ttf");
	enesim_renderer_color_set(r, 0xff000000);
	etex_span_text_set(r, "Hello");

	thiz->r = r;

	/* Default values */
	r = esvg_shape_new(&_descriptor, thiz);
	return r;
}

EAPI void esvg_text_x_set(Enesim_Renderer *r, const Esvg_Coord *x)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (x) thiz->x = *x;
}

EAPI void esvg_text_x_get(Enesim_Renderer *r, Esvg_Coord *x)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (x) *x = thiz->x;
}

EAPI void esvg_text_y_set(Enesim_Renderer *r, const Esvg_Coord *y)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (y) thiz->y = *y;
}

EAPI void esvg_text_y_get(Enesim_Renderer *r, Esvg_Coord *y)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (y) *y = thiz->y;
}

EAPI void esvg_text_font_size_set(Enesim_Renderer *r, const Esvg_Length *size)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (size) thiz->font_size = *size;
}

EAPI void esvg_text_font_size_get(Enesim_Renderer *r, Esvg_Length *size)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	if (size) *size = thiz->font_size;
}

EAPI void esvg_text_font_family_set(Enesim_Renderer *r, const char *family)
{

}

EAPI void esvg_text_text_set(Enesim_Renderer *r, const char *text)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	etex_span_text_set(thiz->r, text);
}

EAPI void esvg_text_text_get(Enesim_Renderer *r, const char **text)
{
	Esvg_Text *thiz;

	thiz = _esvg_text_get(r);
	etex_span_text_get(thiz->r, text);
}
