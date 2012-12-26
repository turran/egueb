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
		const Esvg_Element_Context *state,
		Enesim_Renderer *rel)
{
	Esvg_Pattern *thiz;
	Esvg_Pattern_Units pu;
	Enesim_Matrix m;
	double x;
	double y;
	double w;
	double h;

	thiz = _esvg_pattern_get(r);
	pu = thiz->units;
	if (pu == ESVG_OBJECT_BOUNDING_BOX)
	{
		Eina_Rectangle bbox;

		/* check that the coordinates shold be set with (0,0) -> (1, 1) */
		x = esvg_coord_final_get(&thiz->x, 1);
		y = esvg_coord_final_get(&thiz->y, 1);
		w = esvg_coord_final_get(&thiz->width, 1);
		h = esvg_coord_final_get(&thiz->height, 1);

		enesim_renderer_destination_boundings(rel, &bbox, 0, 0);
		enesim_matrix_values_set(&m, bbox.w, 0, bbox.x, 0, bbox.h, bbox.y, 0, 0, 1);
	}
	else
	{
		double vw;
		double vh;

		/* use the user space coordiantes */
		vw = state->viewbox_w;
		vh = state->viewbox_h;
		x = esvg_coord_final_get(&thiz->x, vw);
		y = esvg_coord_final_get(&thiz->y, vh);
		w = esvg_coord_final_get(&thiz->width, vw);
		h = esvg_coord_final_get(&thiz->height, vh);

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
	enesim_renderer_geometry_transformation_set(thiz->r, &m);
	enesim_renderer_pattern_source_set(thiz->r, thiz->content);
	printf("ok, the content set %p\n", thiz->content);

	/* TODO we need to set the new viewbox */
	/* 1. setup the content */
	/* 2. get the content renderer */
	/* 3. assign it */
#if 0
	{
		Esvg_Element_Context new_state;

		memset(&new_state, 0, sizeof(Esvg_Element_Context));
		new_state.viewbox_w = w;
		new_state.viewbox_h = h;
		new_state.transform = m;
	
		esvg_element_setup(thiz->content, estate, attr, s, error);
		enesim_renderer_pattern_source_set(thiz->r, thiz->content);
	}
#endif

	return EINA_TRUE;
}

static const char * _pattern_name_get(Enesim_Renderer *r)
{
	return "esvg_pattern";
}

static Enesim_Renderer * _pattern_renderer_get(Enesim_Renderer *r,
		const Esvg_Element_Context *state,
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
	esvg_element_parent_set(content, r);
}
