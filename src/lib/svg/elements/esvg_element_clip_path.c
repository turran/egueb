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
#include "esvg_private_main.h"
#include "esvg_private_attribute_presentation.h"
#include "esvg_private_context.h"
#include "esvg_private_element.h"
#include "esvg_private_referenceable.h"
#include "esvg_private_clone.h"
#include "esvg_private_renderable.h"
#include "esvg_private_clip_path.h"

#include "esvg_main.h"
#include "esvg_element_g.h"
#include "esvg_element_clip_path.h"

/*
 * Basically a clipPath has childs of any shape type and when the element
 * is referenced, the object that refers to the clipPath should use a new
 * reference. For now, a reference always stores a renderer, but that
 * needs to change, given that what we need to do is to create a new dom
 * tree (a clone) that will be used to setup the renderers on the correct
 * units space.
 *
 * if a clipping path has more than one child then the clip area is the union
 * of every child. that it, we need to use a compound and render every child
 * with the same fill renderer
 * To do an intersection of clips, the clip-path element (not its children)
 * should have the clip-path property set
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_element_clip_path_log

static int _esvg_element_clip_path_log = -1;

static Ender_Property *ESVG_ELEMENT_CLIP_PATH_CLIP_PATH_UNITS;

/* FIXME share this */
typedef struct _Esvg_Attribute_Units
{
	Esvg_Element_Gradient_Units v;
	Eina_Bool is_set;
} Esvg_Attribute_Units;

typedef struct _Esvg_Element_Clip_Path
{
	/* properties */
	Esvg_Attribute_Units units;
	/* private */
	Eina_Bool tree_changed : 1;
} Esvg_Element_Clip_Path;

static Esvg_Element_Clip_Path * _esvg_element_clip_path_get(Edom_Tag *t)
{
	Esvg_Element_Clip_Path *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TYPE_CLIPPATH)
		return NULL;
	thiz = esvg_referenceable_data_get(t);

	return thiz;
}

static Eina_Bool _esvg_element_clip_path_clone(Edom_Tag *t, Edom_Tag *child,
		void *user_data)
{
	Esvg_Clip_Path_Referenceable_Data *data = user_data;
	Ender_Element *cloned;
	Edom_Tag *cloned_t;
	Ender_Element *child_e;

	/* TODO skip the text */
	/* TODO check that the renderable implementation renderer is a
	 * shape, if so, set the fill renderer of the cloned renderer
	 * as the referer renderer
	 */
	child_e = esvg_element_ender_get(child);
	cloned = esvg_clone_new(child_e);
	cloned_t = ender_element_object_get(cloned);
	ender_element_property_value_add(data->g, EDOM_CHILD, cloned_t, NULL);

	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_element_clip_path_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	/* parse clip path units */
	if (!strcmp(key, "clipPathUnits"))
	{
		Esvg_Clip_Path_Units units;
		esvg_parser_gradient_units_string_from(&units, value);
		esvg_element_clip_path_units_set(e, units);
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static Eina_Bool _esvg_element_clip_path_attribute_get(Edom_Tag *tag,
		const char *attribute, char **value)
{
	return EINA_FALSE;
}


static Eina_Bool _esvg_element_clip_path_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Esvg_Type type;

	if (!esvg_is_element_internal(child))
		return EINA_FALSE;

	type = esvg_element_internal_type_get(child);
	if (esvg_type_is_renderable(type))
	{
		Esvg_Element_Clip_Path *thiz;

		thiz = _esvg_element_clip_path_get(tag);
		thiz->tree_changed = EINA_TRUE;
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

static void _esvg_element_clip_path_context_set(Edom_Tag *t,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		void *user_data)
{
	Esvg_Clip_Path_Referenceable_Data *data = user_data;
	/* FIXME how to get the clip path, the referencer, etc? */

	rctx->color = ENESIM_COLOR_FULL;
	rctx->fill_color = ENESIM_COLOR_FULL;
	rctx->draw_mode = ENESIM_SHAPE_DRAW_MODE_FILL;
	/* set the fill renderer */
	rctx->fill_renderer = data->referrer;
	/* set the transformation */
	/* for now the current user space, later the object boundings box */
	//estate->transform = thiz->rel_m;
}

#if 0
static Eina_Bool _esvg_element_clip_path_set_enesim_state_handle(Edom_Tag *e,
		Enesim_Renderer *layer, void *data)
{
	Enesim_Renderer *shape = layer;
	Eina_Bool unref = EINA_FALSE;

	/* TODO in case of <use> check that the link is a shape */
	/* TODO handle the <use> tag. Given that it is not a shape ... */
	if (esvg_is_use(shape))
	{
		esvg_element_use_link_get(shape, &shape);
		if (!shape) return EINA_FALSE;
		if (!esvg_is_shape(shape))
		{
			printf("use found but not a shape on the link\n");
			return EINA_FALSE;
		}
	}

	esvg_shape_enesim_state_calculate_set(shape, _esvg_element_clip_path_enesim_state_calculate, data);
	/* TODO for the sake of it, this should be done on the enesim state handler */
	enesim_renderer_rop_set(shape, ENESIM_FILL);
	/* TODO unref the shape in case comes from the use */
	if (unref)
		enesim_renderer_unref(shape);
	return EINA_TRUE;
}

static Eina_Bool _esvg_element_clip_path_unset_enesim_state_handle(Edom_Tag *e,
		Enesim_Renderer *layer, void *data)
{
	Enesim_Renderer *shape = layer;
	Eina_Bool unref = EINA_FALSE;

	/* TODO in case of <use> check that the link is a shape */
	/* TODO handle the <use> tag. Given that it is not a shape ... */
	if (esvg_is_use(shape))
	{
		esvg_element_use_link_get(shape, &shape);
		if (!shape) return EINA_FALSE;
		if (!esvg_is_shape(shape))
		{
			printf("use found but not a shape on the link\n");
			return EINA_FALSE;
		}
	}

	esvg_shape_enesim_state_calculate_set(shape, NULL);
	/* TODO unref the shape in case comes from the use */
	if (unref)
		enesim_renderer_unref(shape);
	return EINA_TRUE;
}
#endif

static Esvg_Element_Setup_Return _esvg_element_clip_path_setup(Edom_Tag *e,
		Esvg_Context *c,
		Esvg_Element_Context *state,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Element_Clip_Path *thiz;

	thiz = _esvg_element_clip_path_get(e);
	DBG("Clip path '%s' setup", esvg_element_internal_id_get(e));
#if 0
	Esvg_Element_Clip_Path *thiz;

	thiz = _esvg_element_clip_path_get(t);
	/* if the clip path is set use that as the renderer element */
	if (!attr->clip_path_set)
		return thiz->r;
	else
		return esvg_element_renderer_get(attr->clip_path);
	/* TODO add a new transformation in case of the units */
	/* TODO we should use the same transformation as the relative renderer */
	/* in case this clip path has a clip-path set, call the setup on other */
	if (attr->clip_path_set)
	{
		Eina_Bool ret;

		esvg_element_clip_path_relative_set(attr->clip_path, thiz->r,
				&state->transform);
		ret = enesim_renderer_setup(attr->clip_path, s, error);
		printf("clip path setup %d\n", ret);
	}
#endif
	return ESVG_SETUP_OK;
}

static Eina_Bool _esvg_element_clip_path_reference_add(Edom_Tag *t, Esvg_Referenceable_Reference *rr)
{
	Esvg_Clip_Path_Referenceable_Data *data;
	Ender_Element *g;
	Edom_Tag *g_t;
	Edom_Tag *referer_t;
	Enesim_Renderer *r;
	Enesim_Renderer *proxy;

	g = esvg_element_g_new();
	g_t = ender_element_object_get(g);
	esvg_renderable_internal_renderer_get(g_t, &proxy);
	/* TODO handle the tree changed */
	/* TODO two different objects can reference us, or either a renderable (shapes)
	 * or a referenceable (another clip path)
	 */
	/* now get the renderer from the element that references us */
	referer_t = rr->referencer;
	esvg_renderable_implementation_renderer_get(referer_t, &r);

	data = calloc(1, sizeof(Esvg_Clip_Path_Referenceable_Data));
	data->referrer = r;
	data->g = g;
	data->proxy = proxy;

	/* clone each child and reparent it */
	edom_tag_child_foreach(t, _esvg_element_clip_path_clone, data);
	rr->data = data;

	/* if it is a renderable then store the renderer it has */
	return EINA_TRUE;
}

static Eina_Bool _esvg_element_clip_path_reference_propagate(Edom_Tag *t,
		Esvg_Context *c,
		const Esvg_Element_Context *ctx,
		const Esvg_Attribute_Presentation *attr,
		void *user_data,
		Enesim_Error **error)
{
	Esvg_Element_Context clone_ctx;
	Esvg_Clip_Path_Referenceable_Data *data = user_data;
	Edom_Tag *g_t;

	/* if the tree has changed then re-create the clone */

	/* copy the context but use our own behaviour data */
	/* FIXME not the that ctx here is from the referrer, that is,
	 * the one with the clip-path set, we need to handle
	 * that by removing the clip path check from the common renderable
	 * setup
	 */
	clone_ctx = *ctx;
	clone_ctx.renderable_behaviour.context_set = _esvg_element_clip_path_context_set;
	clone_ctx.renderable_behaviour.data = data;

	/* set the correct proxied renderer */
	/* FIXME for now we dont support clip path referencing another
	 * on that case our own proxy should be the referenced one
	 */
	/* do the setup on the new tree but using this new context */
	g_t = ender_element_object_get(data->g);
	esvg_element_setup_rel(g_t, c, &clone_ctx, NULL, error);

	return ESVG_SETUP_OK;
}

static void _esvg_element_clip_path_free(Edom_Tag *t)
{
	Esvg_Element_Clip_Path *thiz;

	thiz = _esvg_element_clip_path_get(t);
	free(thiz);
}

static Esvg_Referenceable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get	= */ _esvg_element_clip_path_attribute_get,
	/* .cdata_set		= */ NULL,
	/* .text_set		= */ NULL,
	/* .text_get		= */ NULL,
	/* .free		= */ _esvg_element_clip_path_free,
	/* .initialize		= */ NULL,
	/* .attribute_set	= */ _esvg_element_clip_path_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ _esvg_element_clip_path_setup,
	/* .cleanup		= */ NULL,
	/* .reference_propagate	= */ _esvg_element_clip_path_reference_propagate,
	/* .reference_add	= */ _esvg_element_clip_path_reference_add,
	/* .reference_remove	= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_clip_path_new(void)
{
	Esvg_Element_Clip_Path *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Element_Clip_Path));
	if (!thiz) return NULL;

	t = esvg_referenceable_new(&_descriptor, ESVG_TYPE_CLIPPATH, thiz);

	return t;
}

static void _esvg_element_clip_path_clip_path_units_set(Edom_Tag *t, Esvg_Clip_Path_Units units)
{
	Esvg_Element_Clip_Path *thiz;

	thiz = _esvg_element_clip_path_get(t);
	thiz->units.v = units;
	thiz->units.is_set = EINA_TRUE;
}

static void _esvg_element_clip_path_clip_path_units_get(Edom_Tag *t, Esvg_Clip_Path_Units *units)
{
	Esvg_Element_Clip_Path *thiz;

	thiz = _esvg_element_clip_path_get(t);
	if (units) *units = thiz->units.v;
}

static Eina_Bool _esvg_element_clip_path_clip_path_units_is_set(Edom_Tag *t)
{
	Esvg_Element_Clip_Path *thiz;

	thiz = _esvg_element_clip_path_get(t);
	return thiz->units.is_set;
}

/* The ender wrapper */
#define _esvg_element_clip_path_delete NULL
#include "esvg_generated_element_clip_path.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_clip_path_init(void)
{
	_esvg_element_clip_path_log = eina_log_domain_register("esvg_element_clip_path", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_clip_path_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_clip_path_init();
}

void esvg_element_clip_path_shutdown(void)
{
	if (_esvg_element_clip_path_log < 0)
		return;
	_esvg_element_clip_path_shutdown();
	eina_log_domain_unregister(_esvg_element_clip_path_log);
	_esvg_element_clip_path_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_element_clip_path_new(void)
{
	return ESVG_ELEMENT_NEW("SVGClipPathElement");
}

EAPI Eina_Bool esvg_is_clip_path(Ender_Element *e)
{
	return EINA_FALSE;
}

EAPI void esvg_element_clip_path_units_set(Ender_Element *e, Esvg_Clip_Path_Units units)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_CLIP_PATH_CLIP_PATH_UNITS, units, NULL);
}

EAPI void esvg_element_clip_path_units_get(Ender_Element *e, Esvg_Clip_Path_Units *units)
{
}
