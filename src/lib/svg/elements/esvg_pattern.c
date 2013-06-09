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
#include "egueb_svg_private.h"
#include "egueb_svg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_PATTERN_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_PATTERN_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_PATTERN_MAGIC);\
	} while(0)

typedef struct _Egueb_Svg_Pattern
{
	EINA_MAGIC
	/* properties */
	Enesim_Matrix transform;
	Egueb_Svg_Pattern_Units units;
	Egueb_Svg_Coord x;
	Egueb_Svg_Coord y;
	Egueb_Svg_Length width;
	Egueb_Svg_Length height;
	Enesim_Renderer *content;
	/* private */
	Enesim_Renderer *r;
	Eina_Bool units_set : 1;
	Eina_Bool transform_is_set : 1;
} Egueb_Svg_Pattern;

static Egueb_Svg_Pattern * _egueb_svg_pattern_get(Enesim_Renderer *r)
{
	Egueb_Svg_Pattern *thiz;

	thiz = egueb_svg_paint_server_data_get(r);
	ESVG_PATTERN_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _pattern_setup(Enesim_Renderer *r,
		const Egueb_Svg_Element_Context *state,
		Enesim_Renderer *rel)
{
	Egueb_Svg_Pattern *thiz;
	Egueb_Svg_Pattern_Units pu;
	Enesim_Matrix m;
	double x;
	double y;
	double w;
	double h;

	thiz = _egueb_svg_pattern_get(r);
	pu = thiz->units;
	if (pu == ESVG_OBJECT_BOUNDING_BOX)
	{
		Eina_Rectangle bbox;

		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		x = egueb_svg_coord_final_get(&thiz->x, 1);
		y = egueb_svg_coord_final_get(&thiz->y, 1);
		w = egueb_svg_coord_final_get(&thiz->width, 1);
		h = egueb_svg_coord_final_get(&thiz->height, 1);

		enesim_renderer_destination_bounds(rel, &bbox, 0, 0);
		enesim_matrix_values_set(&m, bbox.w, 0, bbox.x, 0, bbox.h, bbox.y, 0, 0, 1);
	}
	else
	{
		double vw;
		double vh;

		/* use the user space coordiantes */
		vw = state->viewbox_w;
		vh = state->viewbox_h;
		x = egueb_svg_coord_final_get(&thiz->x, vw);
		y = egueb_svg_coord_final_get(&thiz->y, vh);
		w = egueb_svg_coord_final_get(&thiz->width, vw);
		h = egueb_svg_coord_final_get(&thiz->height, vh);

		m = state->transform;
	}

	/* set the properties */
	enesim_renderer_pattern_x_set(thiz->r, x);
	enesim_renderer_pattern_y_set(thiz->r, y);
	enesim_renderer_pattern_width_set(thiz->r, w);
	enesim_renderer_pattern_height_set(thiz->r, h);

	printf("pattern setup %g %g %g %g\n", x, y, w, h);
	if (enesim_matrix_type_get(&thiz->transform) != ENESIM_MATRIX_IDENTITY)
	{
		enesim_matrix_compose(&m, &thiz->transform, &m);
	}
	enesim_renderer_transformation_set(thiz->r, &m);
	enesim_renderer_pattern_source_set(thiz->r, thiz->content);
	printf("ok, the content set %p\n", thiz->content);

	/* TODO we need to set the new viewbox */
	/* 1. setup the content */
	/* 2. get the content renderer */
	/* 3. assign it */
#if 0
	{
		Egueb_Svg_Element_Context new_state;

		memset(&new_state, 0, sizeof(Egueb_Svg_Element_Context));
		new_state.viewbox_w = w;
		new_state.viewbox_h = h;
		new_state.transform = m;

		egueb_svg_element_setup(thiz->content, estate, attr, s, error);
		enesim_renderer_pattern_source_set(thiz->r, thiz->content);
	}
#endif

	return EINA_TRUE;
}

static const char * _pattern_name_get(Enesim_Renderer *r)
{
	return "egueb_svg_pattern";
}

static Enesim_Renderer * _pattern_renderer_get(Enesim_Renderer *r,
		const Egueb_Svg_Element_Context *state,
		const Egueb_Svg_Attribute_Presentation *attr)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	return thiz->r;
}

static void _pattern_cleanup(Enesim_Renderer *r)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	/* FIXME for later */
}

static Egueb_Svg_Paint_Server_Descriptor _descriptor = {
	/* .setup =		*/ _pattern_setup,
	/* .name_get =		*/ _pattern_name_get,
	/* .renderer_get =	*/ _pattern_renderer_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * egueb_svg_pattern_new(void)
{
	Egueb_Svg_Pattern *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Egueb_Svg_Pattern));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_PATTERN_MAGIC);
	r = enesim_renderer_pattern_new();
	thiz->r = r;

	/* Default values */
	thiz->units = ESVG_OBJECT_BOUNDING_BOX;
	enesim_matrix_identity(&thiz->transform);

	r = egueb_svg_paint_server_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool egueb_svg_is_pattern(Enesim_Renderer *r)
{
	Egueb_Svg_Pattern *thiz;
	Eina_Bool ret;

	if (!egueb_svg_is_paint_server(r))
		return EINA_FALSE;
	thiz = egueb_svg_paint_server_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_PATTERN_MAGIC);

	return ret;
}

EAPI void egueb_svg_pattern_x_set(Enesim_Renderer *r, const Egueb_Svg_Coord *x)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (x) thiz->x = *x;
}

EAPI void egueb_svg_pattern_x_get(Enesim_Renderer *r, Egueb_Svg_Coord *x)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (x) *x = thiz->x;
}

EAPI void egueb_svg_pattern_y_set(Enesim_Renderer *r, const Egueb_Svg_Coord *y)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (y) thiz->y = *y;
}

EAPI void egueb_svg_pattern_y_get(Enesim_Renderer *r, Egueb_Svg_Coord *y)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (y) *y = thiz->y;
}

EAPI void egueb_svg_pattern_width_set(Enesim_Renderer *r, const Egueb_Svg_Length *width)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (width) thiz->width = *width;
}

EAPI void egueb_svg_pattern_width_get(Enesim_Renderer *r, Egueb_Svg_Length *width)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (width) *width = thiz->width;
}

EAPI void egueb_svg_pattern_height_set(Enesim_Renderer *r, const Egueb_Svg_Length *height)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (height) thiz->height = *height;
}

EAPI void egueb_svg_pattern_height_get(Enesim_Renderer *r, Egueb_Svg_Length *height)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (height) *height = thiz->height;
}

EAPI void egueb_svg_pattern_units_set(Enesim_Renderer *r, Egueb_Svg_Pattern_Units units)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	thiz->units = units;
	thiz->units_set = EINA_TRUE;
}

EAPI void egueb_svg_pattern_units_get(Enesim_Renderer *r, Egueb_Svg_Pattern_Units *units)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (units) *units = thiz->units;
}

EAPI Eina_Bool egueb_svg_pattern_units_is_set(Enesim_Renderer *r)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	return thiz->units_set;
}

EAPI void egueb_svg_pattern_transform_set(Enesim_Renderer *r, const Enesim_Matrix *transform)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (transform) thiz->transform = *transform;
}

EAPI void egueb_svg_pattern_transform_get(Enesim_Renderer *r, Enesim_Matrix *transform)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	if (transform) *transform = thiz->transform;
}

EAPI Eina_Bool egueb_svg_pattern_transform_is_set(Enesim_Renderer *r)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	return thiz->transform_is_set;
}

EAPI void egueb_svg_pattern_content_set(Enesim_Renderer *r, Enesim_Renderer *content)
{
	Egueb_Svg_Pattern *thiz;

	thiz = _egueb_svg_pattern_get(r);
	/* FIXME for now */
	thiz->content = content;
	egueb_svg_element_parent_set(content, r);
}
