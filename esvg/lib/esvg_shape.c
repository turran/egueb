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
#define ESVG_SHAPE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_SHAPE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_SHAPE_MAGIC);\
	} while(0)

typedef struct _Esvg_Shape
{
	EINA_MAGIC
	/* properties */
	/* interface */
	Esvg_Shape_Setup setup;
	Esvg_Shape_Renderer_Get renderer_get;
	Esvg_Renderable_Context_Calculate calculate;
	void *calculate_data;
	Esvg_Element_Clone clone;
	Esvg_Element_Has_Changed has_changed;
	/* private */
	Esvg_Renderable_Context dstate;
	void *data;
} Esvg_Shape;

static Esvg_Shape * _esvg_shape_get(Enesim_Renderer *r)
{
	Esvg_Shape *thiz;

	thiz = esvg_element_data_get(r);
	ESVG_SHAPE_MAGIC_CHECK(thiz);

	return thiz;
}

static void _esvg_shape_enesim_state_get(Enesim_Renderer *r,
		const Esvg_Element_Context *estate,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *dstate)
{
	double stroke_viewport = 0;
	uint8_t fill_opacity;
	uint8_t stroke_opacity;
	uint8_t opacity;

	dstate->draw_mode = 0;
	/* set the opacity */
	opacity = attr->opacity * 255;
	if (attr->color_set)
	{
		const Esvg_Color *c = &attr->color;
		enesim_color_components_from(&dstate->color,
				opacity, c->r, c->g, c->b);
	}
	else
	{
		enesim_color_components_from(&dstate->color,
				opacity, 0xff, 0xff, 0xff);
	}

	/* set the fill */
	fill_opacity = attr->fill_opacity * 255;
	/* FIXME the fill color multiplies the fill renderer */
	dstate->draw_mode |= ENESIM_SHAPE_DRAW_MODE_FILL;
	if (attr->fill.type == ESVG_PAINT_COLOR)
	{
		const Esvg_Color *c = &attr->fill.value.color;
		enesim_color_components_from(&dstate->fill_color,
				fill_opacity, c->r, c->g, c->b);
	}
	else if (attr->fill.type == ESVG_PAINT_SERVER)
	{
		/* just get the renderer here, dont do the setup */
		dstate->fill_renderer = esvg_element_renderer_get(attr->fill.value.paint_server);
	}
	else if (attr->fill.type == ESVG_PAINT_NONE)
	{
		dstate->draw_mode &= ~ENESIM_SHAPE_DRAW_MODE_FILL;
	}
	else if (attr->fill.type == ESVG_PAINT_CURRENT_COLOR)
	{
		dstate->fill_color = ENESIM_COLOR_FULL;
	}
	if (attr->fill_rule == ESVG_EVEN_ODD)
		dstate->fill_rule = ENESIM_SHAPE_FILL_RULE_EVEN_ODD;
	else
		dstate->fill_rule = ENESIM_SHAPE_FILL_RULE_NON_ZERO;
	/* set the stroke */
	stroke_opacity = attr->stroke_opacity * 255;
	dstate->draw_mode |= ENESIM_SHAPE_DRAW_MODE_STROKE;
	if (attr->stroke.type == ESVG_PAINT_COLOR)
	{
		const Esvg_Color *c = &attr->stroke.value.color;
		enesim_color_components_from(&dstate->stroke_color,
				stroke_opacity, c->r, c->g, c->b);
	}
	else if (attr->stroke.type == ESVG_PAINT_SERVER)
	{
		/* just get the renderer here, dont do the setup */
		dstate->stroke_renderer = esvg_element_renderer_get(attr->stroke.value.paint_server);
	}
	else if (attr->stroke.type == ESVG_PAINT_NONE)
	{
		dstate->draw_mode &= ~ENESIM_SHAPE_DRAW_MODE_STROKE;
	}
	else if (attr->stroke.type == ESVG_PAINT_CURRENT_COLOR)
	{
		dstate->stroke_color = ENESIM_COLOR_FULL;
	}
	dstate->stroke_cap = attr->stroke_line_cap;
	dstate->stroke_join = attr->stroke_line_join;
	/* handle the stroke weight */
	if (attr->stroke_width.unit == ESVG_UNIT_LENGTH_PERCENT)
	{
		stroke_viewport = hypot(estate->viewbox_w, estate->viewbox_h) / M_SQRT2;
	}
	dstate->stroke_weight = esvg_length_final_get(
			&attr->stroke_width,
			stroke_viewport);
}


static Enesim_Renderer * _esvg_shape_renderer_get(Enesim_Renderer *r,
		const Esvg_Element_Context *state,
		const Esvg_Attribute_Presentation *attr)
{
	Esvg_Shape *thiz;

	thiz = _esvg_shape_get(r);
	/* if we have a clip path use that as the renderer instead */
	if (attr->clip_path_set)
		return esvg_element_renderer_get(attr->clip_path);
	else
		return thiz->renderer_get(r);
}

/* TODO optimize so many 'ifs' */
static Eina_Bool _esvg_shape_setup(Enesim_Renderer *r, Esvg_Element_Context *estate,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error)
{
	Esvg_Shape *thiz;

	thiz = _esvg_shape_get(r);

	/* given that a shape can be setup for many uses (a shape as a clip path,
	 * a shape for rendering, a shape for masking, etc) the different
	 * enesim states despend on that behaviour
	 */
	if (attr->clip_path_set)
	{
		esvg_clip_path_relative_set(attr->clip_path, thiz->renderer_get(r),
				&estate->transform);
	}

	if (thiz->calculate)
		thiz->calculate(r, estate, attr, &thiz->dstate, thiz->calculate_data);
	else
		_esvg_shape_enesim_state_get(r, estate, attr, &thiz->dstate);

	if (!thiz->setup) return EINA_FALSE;
	if (!thiz->setup(r, estate, &thiz->dstate)) return EINA_FALSE;

	if (attr->clip_path_set)
	{
		Eina_Bool ret;

		ret = enesim_renderer_setup(attr->clip_path, s, error);
	}
	/* in case we are going to use the fill renderer do its own setup */
	if (attr->fill_set && attr->fill.type == ESVG_PAINT_SERVER)
		esvg_paint_server_renderer_setup(attr->fill.value.paint_server, estate, r);
	/* in case we are going to use the stroke renderer do its own setup */
	if (attr->stroke_set && attr->stroke.type == ESVG_PAINT_SERVER)
		esvg_paint_server_renderer_setup(attr->stroke.value.paint_server, estate, r);
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * esvg_shape_data_get(Enesim_Renderer *r)
{
	Esvg_Shape *thiz;

	thiz = _esvg_shape_get(r);
	return thiz->data;
}

Enesim_Renderer * esvg_shape_new(Esvg_Shape_Descriptor *descriptor, void *data)
{
	Esvg_Shape *thiz;
	Esvg_Element_Descriptor pdescriptor = {NULL, NULL, NULL, NULL, NULL, NULL, EINA_TRUE};
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Shape));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_SHAPE_MAGIC);
	thiz->data = data;
	thiz->setup = descriptor->setup;
	thiz->clone = descriptor->clone;
	thiz->renderer_get = descriptor->renderer_get;
	thiz->has_changed = descriptor->has_changed;

	pdescriptor.renderer_get = _esvg_shape_renderer_get;
	pdescriptor.name_get = descriptor->name_get;
	pdescriptor.setup = _esvg_shape_setup;
	pdescriptor.cleanup = descriptor->cleanup;
	pdescriptor.clone = descriptor->clone;
	pdescriptor.has_changed = descriptor->has_changed;
	pdescriptor.is_renderable = EINA_TRUE;

	r = esvg_element_new(&pdescriptor, thiz);

	return r;
}

/* Given that a shape can be part of different parents (g, svg, mask, clip-path, etc)
 * and every of them manage the setup differently (the clip path for example should
 * not set any stroking, should just set the fill renderer) we better make such behaviour
 * a function pointer
 */
void esvg_shape_enesim_state_calculate_set(Enesim_Renderer *r, Esvg_Renderable_Context_Calculate calculate, void *data)
{
	Esvg_Shape *thiz;

	thiz = _esvg_shape_get(r);
	thiz->calculate = calculate;
	thiz->calculate_data = data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_is_shape(Enesim_Renderer *r)
{
	Esvg_Shape *thiz;
	Eina_Bool ret;

	if (!esvg_is_element(r))
		return EINA_FALSE;
	thiz = esvg_element_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_SHAPE_MAGIC);

	return ret;
}
