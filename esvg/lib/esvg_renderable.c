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
#include "esvg_private_referenceable.h"
#include "esvg_private_clip_path.h"
#include "esvg_private_types.h"
#include "esvg_private_svg.h"

#include "esvg_renderable.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT esvg_log_renderable

#define ESVG_RENDERABLE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_RENDERABLE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_RENDERABLE_MAGIC);\
	} while(0)

static Ender_Property *ESVG_RENDERABLE_RENDERER;

typedef struct _Esvg_Renderable_Descriptor_Internal
{
	Edom_Tag_Free free;
	Esvg_Renderable_Setup setup;
	Esvg_Renderable_Renderer_Propagate renderer_propagate;
} Esvg_Renderable_Descriptor_Internal;

typedef struct _Esvg_Renderable
{
	EINA_MAGIC
	/* properties */
	/* interface */
	Esvg_Renderable_Descriptor_Internal descriptor;
	/* private */
	Esvg_Paint fill_paint_last;
	Esvg_Paint stroke_paint_last;
	Esvg_Referenceable_Reference *fill_reference;
	Esvg_Referenceable_Reference *stroke_reference;

	Esvg_Clip_Path clip_path_last;
	Esvg_Referenceable_Reference *clip_path_reference;

	Esvg_Renderable_Context context;
	void *data;
	Enesim_Renderer *r;
	Enesim_Renderer *implementation_r;
} Esvg_Renderable;

static Esvg_Renderable * _esvg_renderable_get(Edom_Tag *t)
{
	Esvg_Renderable *thiz;

	thiz = esvg_element_data_get(t);
	ESVG_RENDERABLE_MAGIC_CHECK(thiz);

	return thiz;
}

static Esvg_Referenceable_Reference * _esvg_renderable_get_reference(Edom_Tag *t, const char *uri)
{
	Ender_Element *topmost;
	Ender_Element *e = NULL;
	Edom_Tag *ref_t;
	Esvg_Referenceable_Reference *rr;

	/* FIXME remove the old reference in case we already had one */
	esvg_element_internal_topmost_get(t, &topmost);
	if (!topmost)
	{
		ERR("No topmost available");
		return NULL;
	}

	esvg_svg_element_get(topmost, uri, &e);
	if (!e)
	{
		ERR("'%s' not found", uri);
		return NULL;
	}

	/* TODO then, check that the referenced element is of type paint server */
	ref_t = ender_element_object_get(e);
	rr = esvg_referenceable_reference_add(ref_t, t);
	return rr;
}

static void _esvg_renderable_paint_set(Edom_Tag *t,
		Enesim_Shape_Draw_Mode *rdraw_mode,
		Enesim_Color *rcolor,
		Enesim_Renderer **renderer,
		Enesim_Shape_Draw_Mode mode,
		double opacity,
		const Esvg_Color *current_color,
		Esvg_Referenceable_Reference **reference,
		const Esvg_Paint *current,
		Esvg_Paint *old)
{
	uint8_t op = opacity * 255;

	/* FIXME the fill color multiplies the fill renderer */
	if (current->type == ESVG_PAINT_COLOR)
	{
		const Esvg_Color *c = &current->value.color;

		enesim_color_components_from(rcolor,
				op, c->r, c->g, c->b);
		*rdraw_mode |= mode;
	}
	else if (current->type == ESVG_PAINT_SERVER)
	{
		Esvg_Referenceable_Reference *rr;

		DBG("Trying to use '%s' as paint server", current->value.paint_server);
		if (!esvg_paint_is_equal(current, old))
		{
			rr = _esvg_renderable_get_reference(t, current->value.paint_server);
			if (!rr)
			{
				ERR("Impossible to get a reference to '%s' paint server", current->value.paint_server); 
				goto done;
			}
			/* TODO finally, get the renderer? */
			*renderer = rr->data;
			*reference = rr;
		}

		enesim_color_components_from(rcolor,
				op, 0xff, 0xff, 0xff);
		*rdraw_mode |= mode;
	}
	else if (current->type == ESVG_PAINT_NONE)
	{
		*rdraw_mode &= ~mode;
	}
	else if (current->type == ESVG_PAINT_CURRENT_COLOR)
	{
		enesim_color_components_from(rcolor,
				op, current_color->r, current_color->g, current_color->b);
		*rdraw_mode |= mode;
	}
done:
	/* update the old paint */
	esvg_paint_copy(old, (Esvg_Paint *)current);
}

static void _esvg_renderable_context_set(Edom_Tag *t,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx)
{
	Esvg_Renderable *thiz;
	const Esvg_Element_Context *ctx;
	double stroke_viewport = 0;
	uint8_t opacity;

	thiz = _esvg_renderable_get(t);
	ctx = esvg_element_context_get(t);

	opacity = attr->opacity.v * 255;
	enesim_color_components_from(&rctx->color,
			opacity, 0xff, 0xff, 0xff);

	/* set the fill */
	_esvg_renderable_paint_set(t, &rctx->draw_mode, &rctx->fill_color,
			&rctx->fill_renderer,
			ENESIM_SHAPE_DRAW_MODE_FILL,
			attr->fill_opacity.v,
			&attr->color.v,
			&thiz->fill_reference,
			&attr->fill.v,
			&thiz->fill_paint_last);
	if (attr->fill_rule.v == ESVG_EVEN_ODD)
	{
		rctx->fill_rule = ENESIM_SHAPE_FILL_RULE_EVEN_ODD;
	}
	else
	{
		rctx->fill_rule = ENESIM_SHAPE_FILL_RULE_NON_ZERO;
	}
	/* set the stroke */
	_esvg_renderable_paint_set(t, &rctx->draw_mode, &rctx->stroke_color,
			&rctx->stroke_renderer,
			ENESIM_SHAPE_DRAW_MODE_STROKE,
			attr->stroke_opacity.v,
			&attr->color.v,
			&thiz->stroke_reference,
			&attr->stroke.v,
			&thiz->stroke_paint_last);
	rctx->stroke_cap = attr->stroke_line_cap.v;
	rctx->stroke_join = attr->stroke_line_join.v;
	/* handle the stroke weight */
	if (attr->stroke_width.v.unit == ESVG_UNIT_LENGTH_PERCENT)
	{
		stroke_viewport = hypot(ctx->viewbox.width, ctx->viewbox.height) / M_SQRT2;
	}
	/* FIXME */
	rctx->stroke_weight = esvg_length_final_get(
			&attr->stroke_width.v,
			stroke_viewport, ctx->font_size);
}

static Esvg_Element_Setup_Return _esvg_renderable_propagate(Esvg_Renderable *thiz, Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Element_Setup_Return ret;
	Eina_Bool visible;

	/* check if it is visible or not */
	if (attr->visibility.v == ESVG_VISIBILITY_VISIBLE)
		visible = EINA_TRUE;
	else
		visible = EINA_FALSE;

	/* check if it should display or not */
	if (attr->display.v != ESVG_DISPLAY_NONE)
		visible = visible && EINA_TRUE;
	else
		visible = EINA_FALSE;

	enesim_renderer_visibility_set(thiz->implementation_r, visible);

	if (!thiz->descriptor.renderer_propagate)
		return ESVG_SETUP_OK;

	/* given that a shape can be setup for many uses (a shape as a clip path,
	 * a shape for rendering, a shape for masking, etc) the different
	 * enesim states despend on that behaviour
	 */
	/* FIXME this should be part of the renderable behaviour too */
	if (!esvg_clip_path_is_equal(&attr->clip_path.v, &thiz->clip_path_last))
	{
		/* whenever a clip path is set, we should reference it, etc, etc
		 * similar to the gradient and also make the renderable renderer
		 * use the clip path renderer to render
		 */
		if (thiz->clip_path_last.type == ESVG_CLIP_PATH_IRI)
		{
			free(thiz->clip_path_last.value.iri);
			/* TODO destroy the reference */
			thiz->clip_path_last.value.iri = NULL;
			enesim_renderer_proxy_proxied_set(thiz->r, thiz->implementation_r);
		}

		if (attr->clip_path.v.type == ESVG_CLIP_PATH_IRI)
		{
			Esvg_Referenceable_Reference *rr;

			thiz->clip_path_last.type = ESVG_CLIP_PATH_IRI;
			thiz->clip_path_last.value.iri = strdup(attr->clip_path.v.value.iri);
			rr = _esvg_renderable_get_reference(t, thiz->clip_path_last.value.iri);
			if (!rr)
			{
				ERR("Impossible to get a reference to '%s' clip path", thiz->clip_path_last.value.iri);
			}
			else
			{
				Esvg_Clip_Path_Referenceable_Data *rdata;

				thiz->clip_path_reference = rr;
				rdata = rr->data;
				/* get the clip path renderer and use that as our new proxied renderer */
				enesim_renderer_proxy_proxied_set(thiz->r, rdata->proxy);
			}
		}
	}
	/* FIXME there are cases where this is not needed, liek the 'use' given that
	 * the 'g' will do it
	 */
	if (!context->renderable_behaviour.context_set)
		_esvg_renderable_context_set(t, attr, &thiz->context);
	else
		context->renderable_behaviour.context_set(t, attr, &thiz->context, context->renderable_behaviour.data);
	/* do the renderer propagate */
	if (!thiz->descriptor.renderer_propagate(t, c, context, attr, &thiz->context, error))
		return ESVG_SETUP_FAILED;
	/* given that the propagate above actually sets the bounding box, etc, we need to call the setup
	 * of the referenceables *after*
	 * we should really call the "propagate" version and the function should do the setup if needed
	 */
	if (thiz->clip_path_reference)
	{
		ret = esvg_element_internal_setup(thiz->clip_path_reference->t, c, error);
		if (ret != ESVG_SETUP_OK)
			return ret;
	}
	/* we need to propagate the referenceables paint servers so they can catch up
	 * our new context (bounds, transformation, whatever)
	 */
	if (attr->fill.is_set && attr->fill.v.type == ESVG_PAINT_SERVER && thiz->fill_reference)
	{
		DBG("Using '%s' as fill paint server", attr->fill.v.value.paint_server);
		esvg_referenceable_reference_propagate(thiz->fill_reference, c, error);
	}
	/* in case we are going to use the stroke renderer do its own setup */
	if (attr->stroke.is_set && attr->stroke.v.type == ESVG_PAINT_SERVER && thiz->stroke_reference)
	{
		DBG("Using '%s' as stroke paint server", attr->fill.v.value.paint_server);
		esvg_referenceable_reference_propagate(thiz->stroke_reference, c, error);
	}
	return ESVG_SETUP_OK;
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                         The Esvg Element interface                         *
 *----------------------------------------------------------------------------*/
static void _esvg_renderable_free(Edom_Tag *t)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(t);
	/* TODO free the old clip path */
	/* TODO fill_paint_last, stroke_paint_last */
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	enesim_renderer_unref(thiz->r);
	free(thiz);
}

static Esvg_Element_Setup_Return _esvg_renderable_setup(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *parent_context,
		Esvg_Element_Context *context,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Renderable *thiz;
	Esvg_Element_Setup_Return ret;

	thiz = _esvg_renderable_get(t);
	if (thiz->descriptor.setup)
	{
		if (!thiz->descriptor.setup(t, c, context, attr, error))
			return ESVG_SETUP_FAILED;
	}
	ret = _esvg_renderable_propagate(thiz, t, c, parent_context, context, attr, error);
	if (ret != ESVG_SETUP_OK)
		return ret;

	return ESVG_SETUP_OK;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_renderable_implementation_renderer_get(Edom_Tag *t, Enesim_Renderer **r)
{
	Esvg_Renderable *thiz;

	if (!r) return;
	thiz = _esvg_renderable_get(t);
	*r = thiz->implementation_r;
}

void esvg_renderable_internal_renderer_get(Edom_Tag *t, Enesim_Renderer **r)
{
	Esvg_Renderable *thiz;

	if (!r) return;
	thiz = _esvg_renderable_get(t);
	*r = thiz->r;
}

/* The ender wrapper */
#define _esvg_renderable_delete NULL
#define _esvg_renderable_renderer_get esvg_renderable_internal_renderer_get
#define _esvg_renderable_renderer_set NULL
#define _esvg_renderable_renderer_is_set NULL
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
	Enesim_Renderer *r;

	/* initial checks */
	if (!descriptor->renderer_get)
		return NULL;

	thiz = calloc(1, sizeof(Esvg_Renderable));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_RENDERABLE_MAGIC);
	thiz->data = data;
	/* create the proxy renderer */
	r = enesim_renderer_proxy_new();
	thiz->r = r;

	/* our own descriptor */
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.renderer_propagate = descriptor->renderer_propagate;
	thiz->descriptor.free = descriptor->free;
	/* default values */
	/* set the previous values with the same value as the default */
	thiz->fill_paint_last.type = ESVG_PAINT_COLOR;
	thiz->fill_paint_last.value.color.r = 0;
	thiz->fill_paint_last.value.color.g = 0;
	thiz->fill_paint_last.value.color.b = 0;
	thiz->stroke_paint_last.type = ESVG_PAINT_NONE;
	/* the initial context */
	thiz->context.draw_mode = ENESIM_SHAPE_DRAW_MODE_FILL;
	thiz->context.fill_color = 0xff000000;

	pdescriptor.child_add = descriptor->child_add;
	pdescriptor.child_remove = descriptor->child_remove;
	pdescriptor.attribute_set = descriptor->attribute_set;
	pdescriptor.attribute_get = descriptor->attribute_get;
	pdescriptor.attribute_animated_fetch = descriptor->attribute_animated_fetch;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = descriptor->text_set;
	pdescriptor.text_get = descriptor->text_get;
	pdescriptor.free = _esvg_renderable_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = _esvg_renderable_setup;

	t = esvg_element_new(&pdescriptor, type, thiz);

	/* call the renderer_get and store it */
	r = descriptor->renderer_get(t);
	if (!r)
	{
		/* TODO destroy the tag */
		return NULL;
	}

	thiz->implementation_r = r;
	/* set the proxied renderer by default */
	enesim_renderer_proxy_proxied_set(thiz->r, r);

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
	Edom_Tag *t;
	Enesim_Renderer *r;

	t = ender_element_object_get(e);
	_esvg_renderable_renderer_get(t, &r);
	return r;
}
