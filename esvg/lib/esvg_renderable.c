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
#include "esvg_private_element.h"
#include "esvg_private_renderable.h"
#include "esvg_svg.h"
#include "esvg_renderable.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_RENDERABLE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_RENDERABLE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_RENDERABLE_MAGIC);\
	} while(0)

static Ender_Property *ESVG_RENDERABLE_RENDERER;
static Ender_Property *ESVG_RENDERABLE_X_DPI;
static Ender_Property *ESVG_RENDERABLE_Y_DPI;
static Ender_Property *ESVG_RENDERABLE_CONTAINER_WIDTH;
static Ender_Property *ESVG_RENDERABLE_CONTAINER_HEIGHT;

typedef struct _Esvg_Renderable_Descriptor_Internal
{
	Edom_Tag_Free free;
	Esvg_Renderable_Setup setup;
	Esvg_Renderable_Renderer_Get renderer_get;
} Esvg_Renderable_Descriptor_Internal;

typedef struct _Esvg_Renderable_Paint_Server_State
{
	Esvg_Paint paint_last;
	Ender_Element *ender;
	Edom_Tag *tag;
} Esvg_Renderable_Paint_Server_State;

typedef struct _Esvg_Renderable
{
	EINA_MAGIC
	/* properties */
	double container_width;
	double container_height;
	double x_dpi;
	double y_dpi;
	/* interface */
	Esvg_Renderable_Descriptor_Internal descriptor;
	/* private */
	Esvg_Paint fill_paint_last;
	Esvg_Paint stroke_paint_last;
	Ender_Element *fill_ender;
	Edom_Tag *fill_tag;
	Ender_Element *stroke_ender;
	Edom_Tag *stroke_tag;
	Esvg_Renderable_Context context;
	void *data;
} Esvg_Renderable;

static Esvg_Renderable * _esvg_renderable_get(Edom_Tag *t)
{
	Esvg_Renderable *thiz;

	thiz = esvg_element_data_get(t);
	ESVG_RENDERABLE_MAGIC_CHECK(thiz);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                             The URI interface                              *
 *----------------------------------------------------------------------------*/
static void * _esvg_renderable_uri_local_get(const char *name, void *data)
{
	Ender_Element *topmost = data;
	Ender_Element *relative;

	relative = esvg_svg_element_find(topmost, name);
	return relative;
}

static Esvg_Uri_Descriptor _uri_descriptor = {
	/* .local_get 		= */ _esvg_renderable_uri_local_get,
	/* .absolute_get 	= */ NULL
};

static void _esvg_shape_enesim_state_get(Edom_Tag *t,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx)
{
	Esvg_Renderable *thiz;
	double stroke_viewport = 0;
	uint8_t fill_opacity;
	uint8_t stroke_opacity;
	uint8_t opacity;

	thiz = _esvg_renderable_get(t);

	rctx->draw_mode = 0;
	/* set the opacity */
	opacity = attr->opacity * 255;
	if (attr->color_set)
	{
		const Esvg_Color *c = &attr->color;
		enesim_color_components_from(&rctx->color,
				opacity, c->r, c->g, c->b);
	}
	else
	{
		enesim_color_components_from(&rctx->color,
				opacity, 0xff, 0xff, 0xff);
	}

	/* set the fill */
	fill_opacity = attr->fill_opacity * 255;
	/* FIXME the fill color multiplies the fill renderer */
	rctx->draw_mode |= ENESIM_SHAPE_DRAW_MODE_FILL;
	if (attr->fill.type == ESVG_PAINT_COLOR)
	{
		const Esvg_Color *c = &attr->fill.value.color;
		enesim_color_components_from(&rctx->fill_color,
				fill_opacity, c->r, c->g, c->b);
	}
	else if (attr->fill.type == ESVG_PAINT_SERVER)
	{
		Ender_Element *topmost;

		esvg_element_internal_topmost_get(t, &topmost);
		if (topmost)
		{
			Ender_Element *e;

			/* just get the renderer here, dont do the setup */
			e = esvg_uri_string_from(attr->fill.value.paint_server, &_uri_descriptor, topmost);
			if (e)
			{
				Enesim_Renderer *fill_r;

				thiz->fill_ender = e;
				thiz->fill_tag = ender_element_object_get(e);
				fill_r = esvg_renderable_renderer_get(e);
				rctx->fill_renderer = fill_r;
				printf("fill rendererrrrr!!! %p %s\n", fill_r, esvg_type_string_to(esvg_element_type_get_internal(t)));
			}
		}
		/* TODO here we should fetch the id from the property */
		/* TODO then, check that the referenced element is of type paint server */
		/* TODO finally, get the renderer? */
		//rctx->fill_renderer = esvg_element_renderer_get(attr->fill.value.paint_server);
	}
	else if (attr->fill.type == ESVG_PAINT_NONE)
	{
		rctx->draw_mode &= ~ENESIM_SHAPE_DRAW_MODE_FILL;
	}
	else if (attr->fill.type == ESVG_PAINT_CURRENT_COLOR)
	{
		rctx->fill_color = ENESIM_COLOR_FULL;
	}
	if (attr->fill_rule == ESVG_EVEN_ODD)
	{
		rctx->fill_rule = ENESIM_SHAPE_FILL_RULE_EVEN_ODD;
	}
	else
	{
		rctx->fill_rule = ENESIM_SHAPE_FILL_RULE_NON_ZERO;
	}
	/* set the stroke */
	stroke_opacity = attr->stroke_opacity * 255;
	rctx->draw_mode |= ENESIM_SHAPE_DRAW_MODE_STROKE;
	if (attr->stroke.type == ESVG_PAINT_COLOR)
	{
		const Esvg_Color *c = &attr->stroke.value.color;
		enesim_color_components_from(&rctx->stroke_color,
				stroke_opacity, c->r, c->g, c->b);
	}
	else if (attr->stroke.type == ESVG_PAINT_SERVER)
	{
		/* just get the renderer here, dont do the setup */
		//rctx->stroke_renderer = esvg_element_renderer_get(attr->stroke.value.paint_server);
	}
	else if (attr->stroke.type == ESVG_PAINT_NONE)
	{
		rctx->draw_mode &= ~ENESIM_SHAPE_DRAW_MODE_STROKE;
	}
	else if (attr->stroke.type == ESVG_PAINT_CURRENT_COLOR)
	{
		rctx->stroke_color = ENESIM_COLOR_FULL;
	}
	rctx->stroke_cap = attr->stroke_line_cap;
	rctx->stroke_join = attr->stroke_line_join;
	/* handle the stroke weight */
	if (attr->stroke_width.unit == ESVG_UNIT_LENGTH_PERCENT)
	{
		stroke_viewport = hypot(ctx->viewbox.width, ctx->viewbox.height) / M_SQRT2;
	}
	rctx->stroke_weight = esvg_length_final_get(
			&attr->stroke_width,
			stroke_viewport);
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _esvg_renderable_container_width_set(Edom_Tag *t, double container_width)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(t);
	thiz->container_width = container_width;
}

static void _esvg_renderable_container_height_set(Edom_Tag *t, double container_height)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(t);
	thiz->container_height = container_height;
}

static void _esvg_renderable_x_dpi_set(Edom_Tag *t, double x_dpi)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(t);
	thiz->x_dpi = x_dpi;
}

static void _esvg_renderable_x_dpi_get(Edom_Tag *t, double *x_dpi)
{
	Esvg_Renderable *thiz;

	if (!x_dpi)
		return;
	thiz = _esvg_renderable_get(t);
	*x_dpi = thiz->x_dpi;
}

static void _esvg_renderable_y_dpi_set(Edom_Tag *t, double y_dpi)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(t);
	thiz->y_dpi = y_dpi;
}

static void _esvg_renderable_y_dpi_get(Edom_Tag *t, double *y_dpi)
{
	Esvg_Renderable *thiz;

	if (!y_dpi)
		return;
	thiz = _esvg_renderable_get(t);
	*y_dpi = thiz->y_dpi;
}
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_renderable_free(Edom_Tag *t)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}

/* TODO optimize so many 'ifs' */
static Eina_Bool _esvg_renderable_setup(Edom_Tag *t,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(t);
	if (!parent_context)
	{
		context->viewbox.min_x = 0;
		context->viewbox.min_y = 0;
		context->viewbox.width = thiz->container_width;
		context->viewbox.height = thiz->container_height;

		context->dpi_y = thiz->x_dpi;
		context->dpi_x = thiz->y_dpi;
	}
	/* given that a shape can be setup for many uses (a shape as a clip path,
	 * a shape for rendering, a shape for masking, etc) the different
	 * enesim states despend on that behaviour
	 */
#if 0
	if (attr->clip_path_set)
	{
		esvg_clip_path_relative_set(attr->clip_path, thiz->renderer_get(r),
				&context->transform);
	}

	if (thiz->calculate)
		thiz->calculate(r, context, attr, &thiz->context, thiz->calculate_data);
	else
#endif
		_esvg_shape_enesim_state_get(t, context, attr, &thiz->context);

	/* do the setup */
	if (thiz->descriptor.setup)
	{
		if (!thiz->descriptor.setup(t, context, attr, &thiz->context, error))
			return EINA_FALSE;
	}
#if 0
	if (attr->clip_path_set)
	{
		Eina_Bool ret;

		ret = enesim_renderer_setup(attr->clip_path, s, error);
	}
#endif
	/* in case we are going to use the fill renderer do its own setup */
	if (attr->fill_set && attr->fill.type == ESVG_PAINT_SERVER && thiz->fill_ender)
	{
		/* we dont pass the attributes or the paint server
		 * will merge what it has with this
		 */
		esvg_element_internal_setup(thiz->fill_tag, context, NULL, error);
	}
#if 0
	/* in case we are going to use the stroke renderer do its own setup */
	if (attr->stroke_set && attr->stroke.type == ESVG_PAINT_SERVER)
		esvg_paint_server_renderer_setup(attr->stroke.value.paint_server, context, r);
#endif
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_renderable_internal_renderer_get(Edom_Tag *t, Enesim_Renderer **r)
{
	Esvg_Renderable *thiz;

	if (!r) return;
	thiz = _esvg_renderable_get(t);
	*r = thiz->descriptor.renderer_get(t);
}

void esvg_renderable_internal_container_width_get(Edom_Tag *t, double *container_width)
{
	Esvg_Renderable *thiz;

	if (!container_width)
		return;
	thiz = _esvg_renderable_get(t);
	*container_width = thiz->container_width;
}

void esvg_renderable_internal_container_height_get(Edom_Tag *t, double *container_height)
{
	Esvg_Renderable *thiz;

	if (!container_height)
		return;
	thiz = _esvg_renderable_get(t);
	*container_height = thiz->container_height;
}

/* The ender wrapper */
#define _esvg_renderable_renderer_get esvg_renderable_internal_renderer_get
#define _esvg_renderable_renderer_set NULL
#define _esvg_renderable_container_width_get esvg_renderable_internal_container_width_get
#define _esvg_renderable_container_height_get esvg_renderable_internal_container_height_get
#include "generated/esvg_generated_renderable.c"

Eina_Bool esvg_is_renderable_internal(Edom_Tag *t)
{
	Esvg_Renderable *thiz;
	Eina_Bool ret;

	if (!esvg_is_element_internal(t))
		return EINA_FALSE;
	thiz = esvg_element_data_get(t);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_RENDERABLE_MAGIC);

	return ret;
}

void * esvg_renderable_data_get(Edom_Tag *t)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(t);
	return thiz->data;
}

Edom_Tag * esvg_renderable_new(Esvg_Renderable_Descriptor *descriptor, Esvg_Type type,
		void *data)
{
	Esvg_Renderable *thiz;
	Esvg_Element_Descriptor pdescriptor;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Renderable));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_RENDERABLE_MAGIC);
	thiz->data = data;
	/* our own descriptor */
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.renderer_get = descriptor->renderer_get;
	/* default values */
	thiz->container_width = 640;
	thiz->container_height = 480;
	thiz->x_dpi = 96.0;
	thiz->y_dpi = 96.0;
	
	pdescriptor.child_add = descriptor->child_add;
	pdescriptor.child_remove = descriptor->child_remove;
	pdescriptor.attribute_set = descriptor->attribute_set;
	pdescriptor.attribute_get = descriptor->attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.free = _esvg_renderable_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = _esvg_renderable_setup;

	t = esvg_element_new(&pdescriptor, type, thiz);

	return t;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool esvg_is_renderable(Ender_Element *e)
{
	Edom_Tag *t;

	t = ender_element_object_get(e);
	return esvg_is_renderable_internal(t);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * esvg_renderable_renderer_get(Ender_Element *e)
{
	Esvg_Renderable *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	t = ender_element_object_get(e);
	_esvg_renderable_renderer_get(t, &r);
	return r;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_renderable_container_width_set(Ender_Element *e, double container_width)
{
	ender_element_property_value_set(e, ESVG_RENDERABLE_CONTAINER_WIDTH, container_width, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_renderable_container_width_get(Ender_Element *e, double *container_width)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_renderable_container_height_set(Ender_Element *e, double container_height)
{
	ender_element_property_value_set(e, ESVG_RENDERABLE_CONTAINER_HEIGHT, container_height, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_renderable_container_height_get(Ender_Element *e, double *container_height)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_renderable_x_dpi_set(Ender_Element *e, double x_dpi)
{
	ender_element_property_value_set(e, ESVG_RENDERABLE_X_DPI, x_dpi, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_renderable_x_dpi_get(Ender_Element *e, double *x_dpi)
{
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_renderable_y_dpi_set(Ender_Element *e, double y_dpi)
{
	ender_element_property_value_set(e, ESVG_RENDERABLE_Y_DPI, y_dpi, NULL);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void esvg_renderable_y_dpi_get(Ender_Element *e, double *y_dpi)
{
}


