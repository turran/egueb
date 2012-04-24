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
#include "esvg_private_instantiable.h"
#include "esvg_clip_path.h"

/*
 * if a clipping path has more than one child then the clip area is the union
 * of every child. that it, we need to use a compound and render every child
 * with the same fill renderer
 * To do an intersection of clips, the clip-path element (not its children)
 * should have the clip-path property set
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Clip_Path
{
	/* properties */
	Esvg_Clip_Path_Units units;
	/* private */
	Enesim_Renderer *rel;
	Enesim_Matrix rel_m;
	Enesim_Renderer *r;
} Esvg_Clip_Path;

static Esvg_Clip_Path * _esvg_clip_path_get(Edom_Tag *t)
{
	Esvg_Clip_Path *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_CLIP_PATH)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_clip_path_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	/* parse clip path units */
	return EINA_TRUE;
}

static Eina_Bool _esvg_clip_path_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Esvg_Type type;

	type = esvg_element_internal_type_get(child_t);
#if 0
	switch (type)
	{
		case ESVG_CIRCLE:
		case ESVG_ELLIPSE:
		case ESVG_RECT:
		case ESVG_LINE:
		case ESVG_PATH:
		case ESVG_POLYLINE:
		case ESVG_POLYGON:
		case ESVG_TEXT:
		case ESVG_USE:
		break;

		default:
		return EINA_FALSE;
	}
#endif
}
#if 0
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_clip_path_new(Edom_Parser *parser)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	r = esvg_clip_path_new();
	tag = esvg_parser_element_new(parser, &_descriptor, ESVG_CLIPPATH, r, NULL);

	return tag;
}
#endif

static void _esvg_clip_path_enesim_state_calculate(Edom_Tag *e,
		Esvg_Element_Context *estate,
		const Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *dstate,
		void *data)
{
	Esvg_Clip_Path *thiz = data;

	dstate->color = ENESIM_COLOR_FULL;
	dstate->fill_color = ENESIM_COLOR_FULL;
	dstate->draw_mode = ENESIM_SHAPE_DRAW_MODE_FILL;
	/* set the fill renderer */
	dstate->fill_renderer = thiz->rel;
	/* set the transformation */
	/* for now the current user space, later the object boundings box */
	estate->transform = thiz->rel_m;
}

static Eina_Bool _esvg_clip_path_set_enesim_state_handle(Edom_Tag *e,
		Enesim_Renderer *layer, void *data)
{
	Enesim_Renderer *shape = layer;
	Eina_Bool unref = EINA_FALSE;

	/* TODO in case of <use> check that the link is a shape */
	/* TODO handle the <use> tag. Given that it is not a shape ... */
	if (esvg_is_use(shape))
	{
		esvg_use_link_get(shape, &shape);
		if (!shape) return EINA_FALSE;
		if (!esvg_is_shape(shape))
		{
			printf("use found but not a shape on the link\n");
			return EINA_FALSE;
		}
	}

	esvg_shape_enesim_state_calculate_set(shape, _esvg_clip_path_enesim_state_calculate, data);
	/* TODO for the sake of it, this should be done on the enesim state handler */
	enesim_renderer_rop_set(shape, ENESIM_FILL);
	/* TODO unref the shape in case comes from the use */
	if (unref)
		enesim_renderer_unref(shape);
	return EINA_TRUE;
}

static Eina_Bool _esvg_clip_path_unset_enesim_state_handle(Edom_Tag *e,
		Enesim_Renderer *layer, void *data)
{
	Enesim_Renderer *shape = layer;
	Eina_Bool unref = EINA_FALSE;

	/* TODO in case of <use> check that the link is a shape */
	/* TODO handle the <use> tag. Given that it is not a shape ... */
	if (esvg_is_use(shape))
	{
		esvg_use_link_get(shape, &shape);
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
/*----------------------------------------------------------------------------*
 *                         The ESVG element interface                         *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_clip_path_element_add(Edom_Tag *e, Enesim_Renderer *child)
{
	Esvg_Clip_Path *thiz;

	thiz = _esvg_clip_path_get(e);
	thiz->children = eina_list_append(thiz->children, child);
	enesim_renderer_compound_layer_add(thiz->r, child);
	return EINA_TRUE;
}

static void _esvg_clip_path_element_remove(Edom_Tag *e, Enesim_Renderer *child)
{
	Esvg_Clip_Path *thiz;

	thiz = _esvg_clip_path_get(e);
	thiz->children = eina_list_remove(thiz->children, child);
	enesim_renderer_compound_layer_remove(thiz->r, child);
}

static Eina_Bool _esvg_clip_path_setup(Edom_Tag *e,
		Esvg_Element_Context *state,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error)
{
	Esvg_Clip_Path *thiz;


	thiz = _esvg_clip_path_get(e);
	/* TODO add a new transformation in case of the units */
	/* TODO we should use the same transformation as the relative renderer */
	/* in case this clip path has a clip-path set, call the setup on other */
	if (attr->clip_path_set)
	{
		Eina_Bool ret;

		esvg_clip_path_relative_set(attr->clip_path, thiz->r,
				&state->transform);
		ret = enesim_renderer_setup(attr->clip_path, s, error);
		printf("clip path setup %d\n", ret);
	}
	return EINA_TRUE;
}

Enesim_Renderer * _esvg_clip_path_renderer_get(Edom_Tag *e,
		const Esvg_Element_Context *state,
		const Esvg_Attribute_Presentation *attr)
{
	Esvg_Clip_Path *thiz;

	thiz = _esvg_clip_path_get(e);
	/* if the clip path is set use that as the renderer element */
	if (!attr->clip_path_set)
		return thiz->r;
	else
		return esvg_element_renderer_get(attr->clip_path);
}

static void _esvg_clip_path_clone(Edom_Tag *e, Enesim_Renderer *dr)
{
	Esvg_Clip_Path *thiz;
	Eina_List *l;
	Enesim_Renderer *child;

	thiz = _esvg_clip_path_get(e);
	/* TODO we need a code on the compound to get the list of layers */
	EINA_LIST_FOREACH(thiz->children, l, child)
	{
		Enesim_Renderer *new_child;

		new_child = esvg_element_clone(child);
		esvg_container_element_add(dr, new_child);
	}
}

static Esvg_Container_Descriptor _descriptor = {
	/* .element_add	= */ _esvg_clip_path_element_add,
	/* .element_remove	= */ _esvg_clip_path_element_remove,
	/* .element_at		= */ NULL,
	/* .name_get		= */ _esvg_clip_path_name_get,
	/* .renderer_get	= */ _esvg_clip_path_renderer_get,
	/* .clone		= */ _esvg_clip_path_clone,
	/* .setup		= */ _esvg_clip_path_setup,
	/* .cleanup		= */ NULL,
	/* .is_renderable	= */ EINA_FALSE,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
EAPI Edom_Tag * _esvg_clip_path_new(void)
{
	Esvg_Clip_Path *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Clip_Path));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_CLIP_PATH_MAGIC);

	r = enesim_renderer_compound_new();
	enesim_renderer_compound_pre_setup_set(e, _esvg_clip_path_set_enesim_state_handle, thiz);
	enesim_renderer_compound_post_setup_set(e, _esvg_clip_path_unset_enesim_state_handle, NULL);
	thiz->r = r;

	t = esvg_container_new(&_descriptor, thiz);

	return t;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* set the renderer this clip path should clip, is not that i like this way of setup much */
void esvg_clip_path_relative_set(Edom_Tag *e, Enesim_Renderer *rel,
		Enesim_Matrix *rel_m)
{
	Esvg_Clip_Path *thiz;

	thiz = _esvg_clip_path_get(e);
	if (thiz->rel == rel) return;
	if (thiz->rel)
	{
		enesim_renderer_unref(thiz->rel);
	}
	thiz->rel = rel;
	thiz->rel_m = *rel_m;
	if (thiz->rel)
	{
		enesim_renderer_ref(thiz->rel);
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_clip_path_new(void)
{
	return ender_element_new_with_namespace("clip_path", "esvg");
}

EAPI Eina_Bool esvg_is_clip_path(Edom_Tag *e)
{
}

