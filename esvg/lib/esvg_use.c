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
#include "esvg_private_instantiable.h"

#include "esvg_renderable.h"
#include "esvg_g.h"
#include "esvg_use.h"
/*
 * The 'use' element should be able to create a new instance from another
 * svg tree. Basically we set the 'link' property of the 'use' to the svg
 * tree. Once it is attached on the setup we should clone it if it is not
 * already cloned and render it that new tree as a 'g' element.
 * We need to add a clone function on the element descriptor to be able
 * to clone renderers easily.
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Ender_Property *ESVG_USE_X;
static Ender_Property *ESVG_USE_Y;
static Ender_Property *ESVG_USE_WIDTH;
static Ender_Property *ESVG_USE_HEIGHT;
static Ender_Property *ESVG_USE_LINK;

typedef struct _Esvg_Use
{
	/* properties */
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	char *link;
	/* private */
	Edom_Tag *g_t;
	Ender_Element *g_e;
	Enesim_Renderer *g_r;
} Esvg_Use;

static Esvg_Use * _esvg_use_get(Edom_Tag *t)
{
	Esvg_Use *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_USE)
		return NULL;
	thiz = esvg_instantiable_data_get(t);

	return thiz;
}

#if 0
static void _post_parse_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Use *thiz = data;
	Edom_Tag *ref_t = NULL;
	Edom_Tag *tag;
	Enesim_Renderer *r;
	Eina_Bool ret;

	r = thiz->r;
	tag = thiz->tag;

	ret = esvg_href_get(&ref_t, tag, thiz->href);
	if (ret)
	{
		Enesim_Renderer *ref_r;
		Enesim_Renderer *clone;

		printf("tag found %p\n", ref_t);
		ref_r = esvg_parser_element_renderer_get(ref_t);
		clone = esvg_element_clone(ref_r);
		printf("clone = %p\n", clone);
		esvg_use_link_set(r, clone);
	}
	printf("reference!!! %p %s\n", ref_t, thiz->href);
}
#endif

/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_use_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value, ESVG_COORD_0);
		esvg_use_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value, ESVG_COORD_0);
		esvg_use_y_set(e, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value, ESVG_LENGTH_0);
		esvg_use_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value, ESVG_LENGTH_0);
		esvg_use_height_set(e, &height);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
#if 0
		Edom_Parser *parser;

		/* register the post parsing callback */
		parser = edom_tag_parser_get(tag);
		thiz->href = strdup(value);
		esvg_parser_post_parse_add(parser, _post_parse_cb, thiz);
#endif
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_use_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_use_renderer_get(Edom_Tag *t)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	return thiz->g_r;
}

static Esvg_Element_Setup_Return _esvg_use_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Esvg_Use *thiz;
	Enesim_Matrix translate;
	double tx, ty;

	thiz = _esvg_use_get(t);
	/* we should append a new transformation */
	tx = esvg_length_final_get(&thiz->x, ctx->viewbox.width);
	ty = esvg_length_final_get(&thiz->y, ctx->viewbox.height);
	enesim_matrix_translate(&translate, tx, ty);
	enesim_matrix_compose(&ctx->transform, &translate, &ctx->transform);

	/* we take the shortcut here because there's no need to go through
	 * the normal enesim API
	 */
	/* setup the g */
	printf("calling the setup on the use\n");

	return esvg_element_internal_setup(thiz->g_t, c, error);
}

static void _esvg_use_clone(Edom_Tag *t, Edom_Tag *dt)
{
	/* inst this too much? */
}

static void _esvg_use_free(Edom_Tag *t)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	free(thiz);
}

static Esvg_Instantiable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_use_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_use_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ _esvg_use_attribute_set,
	/* .clone		= */ _esvg_use_clone,
	/* .setup		= */ _esvg_use_setup,
	/* .renderer_get	= */ _esvg_use_renderer_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_use_new(void)
{
	Esvg_Use *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Use));
	if (!thiz) return NULL;
	thiz->g_e = esvg_g_new();
	thiz->g_r = esvg_renderable_renderer_get(thiz->g_e);
	thiz->g_t = ender_element_object_get(thiz->g_e);

	/* Default values */
	thiz->x = ESVG_COORD_0;
	thiz->y = ESVG_COORD_0;
	thiz->width = ESVG_LENGTH_0;
	thiz->height = ESVG_LENGTH_0;

	t = esvg_instantiable_new(&_descriptor, ESVG_USE, thiz);
	return t;
}

static void _esvg_use_x_set(Edom_Tag *t, const Esvg_Coord *x)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	if (x) thiz->x = *x;
}

static void _esvg_use_x_get(Edom_Tag *t, Esvg_Coord *x)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	if (x) *x = thiz->x;
}

static void _esvg_use_y_set(Edom_Tag *t, const Esvg_Coord *y)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	if (y) thiz->y = *y;
}

static void _esvg_use_y_get(Edom_Tag *t, Esvg_Coord *y)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	if (y) *y = thiz->y;
}

static void _esvg_use_width_set(Edom_Tag *t, const Esvg_Length *width)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	if (width) thiz->width = *width;
}

static void _esvg_use_width_get(Edom_Tag *t, Esvg_Length *width)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	if (width) *width = thiz->width;
}

static void _esvg_use_height_set(Edom_Tag *t, const Esvg_Length *height)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	if (height) thiz->height = *height;
}

static void _esvg_use_height_get(Edom_Tag *t, Esvg_Length *height)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	if (height) *height = thiz->height;
}

static void _esvg_use_link_set(Edom_Tag *t, const char *link)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(t);
	if (thiz->link == link) return;
	if (thiz->link)
	{
#if 0
		esvg_container_element_remove(thiz->g, thiz->link);
		enesim_renderer_unref(thiz->link);
		thiz->link = NULL;
#endif
	}
	if (link)
	{
#if 0
		esvg_container_element_add(thiz->g, link);
		thiz->link = enesim_renderer_ref(link);
#endif
	}
}

static void _esvg_use_link_get(Edom_Tag *t, const char **link)
{
	Esvg_Use *thiz;

	if (!link) return;
	thiz = _esvg_use_get(t);
	*link = thiz->link;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#define _esvg_use_x_is_set NULL
#define _esvg_use_y_is_set NULL
#define _esvg_use_width_is_set NULL
#define _esvg_use_height_is_set NULL
#define _esvg_use_link_is_set NULL
#include "generated/esvg_generated_use.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_use_new(void)
{
	return ender_element_new_with_namespace("use", "esvg");
}

EAPI Eina_Bool esvg_is_use(Ender_Element *e)
{
	Eina_Bool ret = EINA_TRUE;

	return ret;
}

EAPI void esvg_use_x_set(Ender_Element *e, const Esvg_Coord *x)
{
	ender_element_property_value_set(e, ESVG_USE_X, x, NULL);
}

EAPI void esvg_use_x_get(Ender_Element *e, Esvg_Coord *x)
{
}

EAPI void esvg_use_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	ender_element_property_value_set(e, ESVG_USE_Y, y, NULL);
}

EAPI void esvg_use_y_get(Ender_Element *e, Esvg_Coord *y)
{
}

EAPI void esvg_use_width_set(Ender_Element *e, const Esvg_Length *width)
{
	ender_element_property_value_set(e, ESVG_USE_WIDTH, width, NULL);
}

EAPI void esvg_use_width_get(Ender_Element *e, Esvg_Length *width)
{
}

EAPI void esvg_use_height_set(Ender_Element *e, const Esvg_Length *height)
{
	ender_element_property_value_set(e, ESVG_USE_HEIGHT, height, NULL);
}

EAPI void esvg_use_height_get(Ender_Element *e, Esvg_Length *height)
{
}

EAPI void esvg_use_link_set(Ender_Element *e, const Ender_Element *link)
{
}

EAPI void esvg_use_link_get(Ender_Element *e, Ender_Element **link)
{
}
