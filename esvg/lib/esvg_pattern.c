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
#define ESVG_PATTERN_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_PATTERN_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_PATTERN_MAGIC);\
	} while(0)

typedef struct _Esvg_Pattern
{
	EINA_MAGIC
	/* properties */
	Enesim_Matrix transform;
	Esvg_Pattern_Units units;
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	Enesim_Renderer *content;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool units_set : 1;
	Eina_Bool transform_is_set : 1;
} Esvg_Pattern;

static Esvg_Pattern * _esvg_pattern_get(Enesim_Renderer *r)
{
	Esvg_Pattern *thiz;

	thiz = esvg_paint_server_data_get(r);
	ESVG_PATTERN_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _pattern_setup(Enesim_Renderer *r,
		const Esvg_Element_State *state,
		Enesim_Renderer *rel)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	printf("doing the pattern setup\n");
	return EINA_TRUE;
}

static const char * _pattern_name_get(Enesim_Renderer *r)
{
	return "pattern";
}

static Enesim_Renderer * _pattern_renderer_get(Enesim_Renderer *r,
		const Esvg_Element_State *state,
		const Esvg_Attribute_Presentation *attr)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	return thiz->r;
}

static void _pattern_cleanup(Enesim_Renderer *r)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	/* FIXME for later */
}

static Esvg_Paint_Server_Descriptor _descriptor = {
	/* .setup =		*/ _pattern_setup,
	/* .name_get =		*/ _pattern_name_get,
	/* .renderer_get =	*/ _pattern_renderer_get,
	/* .clone =		*/ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_pattern_new(void)
{
	Esvg_Pattern *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Pattern));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_PATTERN_MAGIC);
	r = enesim_renderer_pattern_new();
	thiz->r = r;

	/* Default values */
	thiz->units = ESVG_OBJECT_BOUNDING_BOX;
	enesim_matrix_identity(&thiz->transform);

	r = esvg_paint_server_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool esvg_is_pattern(Enesim_Renderer *r)
{
	Esvg_Pattern *thiz;
	Eina_Bool ret;

	if (!esvg_is_paint_server(r))
		return EINA_FALSE;
	thiz = esvg_paint_server_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_PATTERN_MAGIC);

	return ret;
}

EAPI void esvg_pattern_x_set(Enesim_Renderer *r, const Esvg_Coord *x)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (x) thiz->x = *x;
}

EAPI void esvg_pattern_x_get(Enesim_Renderer *r, Esvg_Coord *x)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (x) *x = thiz->x;
}

EAPI void esvg_pattern_y_set(Enesim_Renderer *r, const Esvg_Coord *y)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (y) thiz->y = *y;
}

EAPI void esvg_pattern_y_get(Enesim_Renderer *r, Esvg_Coord *y)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (y) *y = thiz->y;
}

EAPI void esvg_pattern_width_set(Enesim_Renderer *r, const Esvg_Length *width)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (width) thiz->width = *width;
}

EAPI void esvg_pattern_width_get(Enesim_Renderer *r, Esvg_Length *width)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (width) *width = thiz->width;
}

EAPI void esvg_pattern_height_set(Enesim_Renderer *r, const Esvg_Length *height)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (height) thiz->height = *height;
}

EAPI void esvg_pattern_height_get(Enesim_Renderer *r, Esvg_Length *height)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (height) *height = thiz->height;
}

EAPI void esvg_pattern_units_set(Enesim_Renderer *r, Esvg_Pattern_Units units)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	thiz->units = units;
	thiz->units_set = EINA_TRUE;
}

EAPI void esvg_pattern_units_get(Enesim_Renderer *r, Esvg_Pattern_Units *units)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (units) *units = thiz->units;
}

EAPI Eina_Bool esvg_pattern_units_is_set(Enesim_Renderer *r)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	return thiz->units_set;
}

EAPI void esvg_pattern_transform_set(Enesim_Renderer *r, const Enesim_Matrix *transform)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (transform) thiz->transform = *transform;
}

EAPI void esvg_pattern_transform_get(Enesim_Renderer *r, Enesim_Matrix *transform)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	if (transform) *transform = thiz->transform;
}

EAPI Eina_Bool esvg_pattern_transform_is_set(Enesim_Renderer *r)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	return thiz->transform_is_set;
}

EAPI void esvg_pattern_content_set(Enesim_Renderer *r, Enesim_Renderer *content)
{
	Esvg_Pattern *thiz;

	thiz = _esvg_pattern_get(r);
	/* FIXME for now */
	thiz->content = content;
}
