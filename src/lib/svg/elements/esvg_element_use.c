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
#include "esvg_private_renderable.h"
#include "esvg_private_renderable_container.h"
#include "esvg_private_clone.h"
#include "esvg_private_svg.h"

#include "esvg_renderable.h"
#include "esvg_element_g.h"
#include "esvg_element_use.h"
#include "esvg_main.h"
/*
 * The 'use' element should be able to create a new instance from another
 * svg tree. Basically we set the 'link' property of the 'use' to the svg
 * tree. Once it is attached on the setup we should clone it if it is not
 * already cloned and render it that new tree as a 'g' element.
 * Given that generated g does not have a parent (but it does have a topmost)
 * we need to propagate every attribute on the use to the g
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_element_use_log

static int _esvg_element_use_log = -1;

static Ender_Property *ESVG_ELEMENT_USE_X;
static Ender_Property *ESVG_ELEMENT_USE_Y;
static Ender_Property *ESVG_ELEMENT_USE_WIDTH;
static Ender_Property *ESVG_ELEMENT_USE_HEIGHT;
static Ender_Property *ESVG_ELEMENT_USE_LINK;

typedef struct _Esvg_Element_Use_State
{
	char *link;
} Esvg_Element_Use_State;

typedef struct _Esvg_Element_Use
{
	/* properties */
	Esvg_Element_Use_State current;
	Esvg_Element_Use_State past;
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	/* private */
	Esvg_Renderable_Container *container;
	Eina_Bool state_changed : 1;
	Ender_Element *cloned;
	/* the always present g tag */
	Edom_Tag *g_t;
	Ender_Element *g_e;
	Enesim_Renderer *g_r;
} Esvg_Element_Use;

static Esvg_Element_Use * _esvg_element_use_get(Edom_Tag *t)
{
	Esvg_Element_Use *thiz;

	if (esvg_element_internal_type_get(t) != ESVG_TYPE_USE)
		return NULL;
	thiz = esvg_renderable_data_get(t);

	return thiz;
}

static void _esvg_element_use_topmost_changed_cb(Ender_Element *e, const char *event_name,
		void *event_data, void *data)
{
	Esvg_Element_Use *thiz = data;
	Esvg_Element_Event_Topmost_Changed *ev = event_data;

	printf("topmost set on the use %p\n", ev->current);
	esvg_element_topmost_set(thiz->g_t, ev->current);
}
/*----------------------------------------------------------------------------*
 *                       The Esvg Renderable interface                        *
 *----------------------------------------------------------------------------*/
static void _esvg_element_use_initialize(Ender_Element *e)
{
	Esvg_Element_Use *thiz;
	Edom_Tag *t;

	t = ender_element_object_get(e);
	thiz = _esvg_element_use_get(t);
	/* whenever the topmost is set on the use
	 * we should also set the topmost on the g
	 */
	ender_event_listener_add(e, "TopmostChanged", _esvg_element_use_topmost_changed_cb, thiz);
	thiz->container = esvg_renderable_container_new(e);
	esvg_renderable_container_renderable_add(thiz->container, thiz->g_t);
}

static Eina_Bool _esvg_element_use_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value);
		esvg_element_use_x_set(e, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value);
		esvg_element_use_y_set(e, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value);
		esvg_element_use_width_set(e, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value);
		esvg_element_use_height_set(e, &height);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
		esvg_element_use_link_set(e, value);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool _esvg_element_use_attribute_get(Edom_Tag *tag, const char *attribute, char **value)
{
	return EINA_FALSE;
}

static Enesim_Renderer * _esvg_element_use_renderer_get(Edom_Tag *t)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	return thiz->g_r;
}

static Esvg_Element_Setup_Return _esvg_element_use_setup(Edom_Tag *t,
		Esvg_Context *c,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Enesim_Error **error)
{
	Esvg_Element_Use *thiz;
	Ender_Element *topmost;
	Ender_Element *link;
	Edom_Tag *cloned_t;
	Enesim_Matrix translate;
	double tx, ty;

	thiz = _esvg_element_use_get(t);
	/* we should append a new transformation */
	tx = esvg_coord_final_get(&thiz->x, ctx->viewbox.w, ctx->font_size);
	ty = esvg_coord_final_get(&thiz->y, ctx->viewbox.h, ctx->font_size);
	enesim_matrix_translate(&translate, tx, ty);
	enesim_matrix_compose(&ctx->transform, &translate, &ctx->transform);

	/* we take the shortcut here because there's no need to go through
	 * the normal enesim API
	 */
	/* FIXME this should be go away */
	esvg_element_internal_topmost_get(t, &topmost);
	if (!topmost)
	{
		WARN("No topmost available");
		return EINA_TRUE;
	}

	if (thiz->state_changed)
	{
		if (thiz->cloned)
		{
			/* TODO remove the tree from the g_e */
			/* TODO remove previous clone */
			thiz->cloned = NULL;
		}
		if (thiz->past.link)
		{
			free(thiz->past.link);
			thiz->past.link = NULL;
		}
		if (thiz->current.link)
		{
			esvg_element_svg_element_get(topmost, thiz->current.link, &link);
			thiz->cloned = esvg_clone_new(link);

			if (!thiz->cloned)
			{
				WARN("Impossible to clone");
				return EINA_FALSE;
			}

			/* TODO add the clone to the generated g */
			cloned_t = ender_element_object_get(thiz->cloned);
			ender_element_property_value_add(thiz->g_e, EDOM_CHILD, cloned_t, NULL);
			thiz->past.link = strdup(thiz->current.link);
		}
		/* FIXME this should go to the cleanup */
		thiz->state_changed = EINA_FALSE;
	}

	/* setup the g */
	/* FIXME for now */
	esvg_element_topmost_set(thiz->g_t, topmost);
	/* Use our own context and attributes as the parent ones */
	return esvg_element_setup_rel(thiz->g_t, c, ctx, attr, error);
}

static void _esvg_element_use_free(Edom_Tag *t)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	esvg_renderable_container_free(thiz->container);
	free(thiz);
}

static Esvg_Renderable_Descriptor _descriptor = {
	/* .child_add		= */ NULL,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ _esvg_element_use_attribute_get,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .text_get 		= */ NULL,
	/* .free 		= */ _esvg_element_use_free,
	/* .initialize 		= */ _esvg_element_use_initialize,
	/* .attribute_set 	= */ _esvg_element_use_attribute_set,
	/* .attribute_animated_fetch = */ NULL,
	/* .setup		= */ _esvg_element_use_setup,
	/* .renderer_get	= */ _esvg_element_use_renderer_get,
	/* .renderer_propagate	= */ NULL,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_element_use_new(void)
{
	Esvg_Element_Use *thiz;
	Edom_Tag *t;

	thiz = calloc(1, sizeof(Esvg_Element_Use));
	if (!thiz) return NULL;
	thiz->g_e = esvg_element_g_new();
	thiz->g_r = esvg_renderable_renderer_get(thiz->g_e);
	thiz->g_t = ender_element_object_get(thiz->g_e);

	/* Default values */
	thiz->x = ESVG_COORD_0;
	thiz->y = ESVG_COORD_0;
	thiz->width = ESVG_LENGTH_0;
	thiz->height = ESVG_LENGTH_0;

	t = esvg_renderable_new(&_descriptor, ESVG_TYPE_USE, thiz);
	return t;
}

static void _esvg_element_use_x_set(Edom_Tag *t, const Esvg_Coord *x)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	if (x) thiz->x = *x;
}

static void _esvg_element_use_x_get(Edom_Tag *t, Esvg_Coord *x)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	if (x) *x = thiz->x;
}

static void _esvg_element_use_y_set(Edom_Tag *t, const Esvg_Coord *y)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	if (y) thiz->y = *y;
}

static void _esvg_element_use_y_get(Edom_Tag *t, Esvg_Coord *y)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	if (y) *y = thiz->y;
}

static void _esvg_element_use_width_set(Edom_Tag *t, const Esvg_Length *width)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	if (width) thiz->width = *width;
}

static void _esvg_element_use_width_get(Edom_Tag *t, Esvg_Length *width)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	if (width) *width = thiz->width;
}

static void _esvg_element_use_height_set(Edom_Tag *t, const Esvg_Length *height)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	if (height) thiz->height = *height;
}

static void _esvg_element_use_height_get(Edom_Tag *t, Esvg_Length *height)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	if (height) *height = thiz->height;
}

static void _esvg_element_use_link_set(Edom_Tag *t, const char *link)
{
	Esvg_Element_Use *thiz;

	thiz = _esvg_element_use_get(t);
	if (thiz->current.link)
	{
		free(thiz->current.link);
		thiz->current.link = NULL;
	}
	if (link)
	{
		thiz->current.link = strdup(link);
	}
	thiz->state_changed = EINA_TRUE;
}

static void _esvg_element_use_link_get(Edom_Tag *t, const char **link)
{
	Esvg_Element_Use *thiz;

	if (!link) return;
	thiz = _esvg_element_use_get(t);
	*link = thiz->current.link;
}

/* The ender wrapper */
#define _esvg_element_use_delete NULL
#define _esvg_element_use_x_is_set NULL
#define _esvg_element_use_y_is_set NULL
#define _esvg_element_use_width_is_set NULL
#define _esvg_element_use_height_is_set NULL
#define _esvg_element_use_link_is_set NULL
#include "generated/esvg_generated_element_use.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_element_use_init(void)
{
	_esvg_element_use_log = eina_log_domain_register("esvg_element_use", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_element_use_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_element_use_init();
}

void esvg_element_use_shutdown(void)
{
	if (_esvg_element_use_log < 0)
		return;
	_esvg_element_use_shutdown();
	eina_log_domain_unregister(_esvg_element_use_log);
	_esvg_element_use_log = -1;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_element_use_new(void)
{
	return ESVG_ELEMENT_NEW("SVGUseElement");
}

EAPI Eina_Bool esvg_is_use(Ender_Element *e)
{
	Edom_Tag *t;
	Esvg_Type type;

	t = (Edom_Tag *)ender_element_object_get(e);
	type = esvg_element_internal_type_get(t);
	return (type == ESVG_TYPE_USE) ? EINA_TRUE : EINA_FALSE;
}

EAPI void esvg_element_use_x_set(Ender_Element *e, const Esvg_Coord *x)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_X, x, NULL);
}

EAPI void esvg_element_use_x_get(Ender_Element *e, Esvg_Coord *x)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_use_x_get(t, x);
}

EAPI void esvg_element_use_y_set(Ender_Element *e, const Esvg_Coord *y)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_Y, y, NULL);
}

EAPI void esvg_element_use_y_get(Ender_Element *e, Esvg_Coord *y)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_use_y_get(t, y);
}

EAPI void esvg_element_use_width_set(Ender_Element *e, const Esvg_Length *width)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_WIDTH, width, NULL);
}

EAPI void esvg_element_use_width_get(Ender_Element *e, Esvg_Length *width)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_use_width_get(t, width);
}

EAPI void esvg_element_use_height_set(Ender_Element *e, const Esvg_Length *height)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_HEIGHT, height, NULL);
}

EAPI void esvg_element_use_height_get(Ender_Element *e, Esvg_Length *height)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_use_height_get(t, height);
}

EAPI void esvg_element_use_link_set(Ender_Element *e, const char *v)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_USE_LINK, v, NULL);
}

EAPI void esvg_element_use_link_get(Ender_Element *e, const char **link)
{
	Edom_Tag *t;

	t = (Edom_Tag *)ender_element_object_get(e);
	_esvg_element_use_link_get(t, link);
}
