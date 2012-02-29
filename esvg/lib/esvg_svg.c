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
/*
 * Given that a svg element can clip, we should use a clipper with a compound
 * inside as the renderer
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_SVG_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_SVG_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_SVG_MAGIC);\
	} while(0)

typedef struct _Esvg_Svg
{
	EINA_MAGIC
	/* properties */
	double version;
	Esvg_View_Box view_box;
	Eina_Bool view_box_set;
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	/* private */
	Enesim_Renderer *r;
} Esvg_Svg;

static Esvg_Svg * _esvg_svg_get(Enesim_Renderer *r)
{
	Esvg_Svg *thiz;

	thiz = esvg_container_data_get(r);
	ESVG_SVG_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                          The Container interface                           *
 *----------------------------------------------------------------------------*/
static const char * _esvg_svg_name_get(Enesim_Renderer *r)
{
	return "esvg_svg";
}

static Eina_Bool _esvg_svg_element_add(Enesim_Renderer *r, Enesim_Renderer *child)
{
	Esvg_Svg *thiz;
	Enesim_Renderer *real_r;

	if (!esvg_element_is_renderable(child))
	{
		return EINA_FALSE;
	}

	thiz = _esvg_svg_get(r);
	enesim_renderer_compound_layer_add(thiz->r, child);

	return EINA_TRUE;
}

static void _esvg_svg_element_remove(Enesim_Renderer *r, Enesim_Renderer *child)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	enesim_renderer_compound_layer_remove(thiz->r, child);
}

static Enesim_Renderer * _esvg_svg_element_at(Enesim_Renderer *r, double x, double y)
{
	/* TODO */
	return NULL;
}

static Eina_Bool _esvg_svg_setup(Enesim_Renderer *r, Esvg_Element_State *state,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error)
{
	Esvg_Svg *thiz;
	Eina_List *l;
	Enesim_Renderer *child;
	Enesim_Renderer *parent;
	double width, height;

	thiz = _esvg_svg_get(r);
	width = esvg_length_final_get(&thiz->width, state->viewbox_w);
	height = esvg_length_final_get(&thiz->height, state->viewbox_h);
	/* the viewbox will set a new user space coordinate */
	/* FIXME check zeros */
	if (thiz->view_box_set)
	{
		Enesim_Matrix scale;
		double new_vw;
		double new_vh;

		new_vw = thiz->view_box.width / width;
		new_vh = thiz->view_box.height / height;

		printf("setting scale %p to %g %g - %g %g (%g %g)\n",
				state,
				thiz->view_box.width,
				thiz->view_box.height,
				width,
				height,
				new_vw, new_vh);
		width = thiz->view_box.width;
		height = thiz->view_box.height;

		enesim_matrix_scale(&scale, 1.0/new_vw, 1.0/new_vh);
		enesim_matrix_compose(&scale, &state->transform, &state->transform);
		/* TODO handle current matrix */
	}
	state->viewbox_w = width;
	state->viewbox_h = height;

	return EINA_TRUE;
}

static Enesim_Renderer * _esvg_svg_renderer_get(Enesim_Renderer *r,
		const Esvg_Element_State *state,
		const Esvg_Attribute_Presentation *attr)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	return thiz->r;
}

static void _esvg_svg_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{

}

static Esvg_Container_Descriptor _descriptor = {
	/* .element_add	= */ _esvg_svg_element_add,
	/* .element_remove	= */ _esvg_svg_element_remove,
	/* .element_at		= */ _esvg_svg_element_at,
	/* .name_get		= */ _esvg_svg_name_get,
	/* .renderer_get	= */ _esvg_svg_renderer_get,
	/* .clone		= */ _esvg_svg_clone,
	/* .setup		= */ _esvg_svg_setup,
	/* .cleanup		= */ NULL,
	/* .is_renderable	= */ EINA_TRUE,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_svg_new(void)
{
	Esvg_Svg *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Svg));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_SVG_MAGIC);

	r = enesim_renderer_compound_new();
	enesim_renderer_rop_set(r, ENESIM_BLEND);
	thiz->r = r;

	/* Default values */
	thiz->version = 1.0;
	thiz->x = ESVG_COORD_0;
	thiz->y = ESVG_COORD_0;
	thiz->width = ESVG_LENGTH_100_PERCENT;
	thiz->height = ESVG_LENGTH_100_PERCENT;

	/* no default value for the view_box */

	r = esvg_container_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool esvg_is_svg(Enesim_Renderer *r)
{
	Esvg_Svg *thiz;
	Eina_Bool ret;

	if (!esvg_is_container(r))
		return EINA_FALSE;
	thiz = esvg_container_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_SVG_MAGIC);

	return ret;
}

EAPI void esvg_svg_version_set(Enesim_Renderer *r, double version)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	thiz->version = version;
}

EAPI void esvg_svg_version_get(Enesim_Renderer *r, double *version)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	if (version) *version = thiz->version;
}

EAPI void esvg_svg_x_set(Enesim_Renderer *r, Esvg_Coord *x)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	thiz->x = *x;
}

EAPI void esvg_svg_y_set(Enesim_Renderer *r, Esvg_Coord *y)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	thiz->y = *y;
}

EAPI void esvg_svg_width_set(Enesim_Renderer *r, Esvg_Length *width)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	thiz->width = *width;
}

EAPI void esvg_svg_height_set(Enesim_Renderer *r, Esvg_Length *height)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	thiz->height = *height;
}

EAPI void esvg_svg_viewbox_set(Enesim_Renderer *r, Esvg_View_Box *vb)
{
	Esvg_Svg *thiz;

	thiz = _esvg_svg_get(r);
	if (!vb)
	{
		thiz->view_box_set = EINA_FALSE;
	}
	else
	{
		thiz->view_box = *vb;
		thiz->view_box_set = EINA_TRUE;
	}
}

/* FIXME the below two functions should return the actual width/height based
 * on the width and height properties, if it is relative it should use the
 * the container width/height to compute them, this is are helper functions
 * to know the real size of the svg, this way we'll know on the upper libraries
 * the preferred area
 */
EAPI void esvg_svg_actual_width_get(Enesim_Renderer *r, double *actual_width)
{
	Esvg_Svg *thiz;
	double aw;
	double cw;

	thiz = _esvg_svg_get(r);
	esvg_element_container_width_get(r, &cw);
	aw = esvg_length_final_get(&thiz->width, cw);
	*actual_width = aw;
}

EAPI void esvg_svg_actual_height_get(Enesim_Renderer *r, double *actual_height)
{
	Esvg_Svg *thiz;
	double ah;
	double ch;

	thiz = _esvg_svg_get(r);
	esvg_element_container_height_get(r, &ch);
	ah = esvg_length_final_get(&thiz->height, ch);
	*actual_height = ah;
}
