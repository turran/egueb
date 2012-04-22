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
#include "esvg_g.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_G
{
	/* properties */
	/* private */
	Enesim_Renderer *r;
} Esvg_G;

static Esvg_G * _esvg_g_get(Edom_Tag *t)
{
	Esvg_G *thiz;

	if (esvg_element_type_get_internal(t) != ESVG_G)
		return NULL;
	thiz = esvg_instantiable_data_get(t);

	return thiz;
}

static Eina_Bool _esvg_g_child_setup_filter(Edom_Tag *t, Edom_Tag *child)
{
	if (!esvg_is_instantiable_internal(child))
		return EINA_FALSE;
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                          The Container interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_g_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Esvg_G *thiz;

	/* if renderable, add the renderer into the compound */
	thiz = _esvg_g_get(tag);
	if (esvg_is_instantiable_internal(child))
	{
		Enesim_Renderer *r = NULL;

		esvg_renderable_internal_renderer_get(child, &r);
		enesim_renderer_compound_layer_add(thiz->r, r);
	}

	return EINA_TRUE;
}

Enesim_Renderer * _esvg_g_renderer_get(Edom_Tag *t)
{
	Esvg_G *thiz;

	thiz = _esvg_g_get(t);
	return thiz->r;
}

static void _esvg_g_clone(Edom_Tag *t, Edom_Tag *tr)
{
#if 0
	Esvg_G *thiz;
	Eina_List *l;
	Enesim_Renderer *child;

	thiz = _esvg_g_get(r);
	EINA_LIST_FOREACH(thiz->children, l, child)
	{
		Enesim_Renderer *new_child;

		new_child = esvg_element_clone(child);
		esvg_container_element_add(dr, new_child);
	}
#endif
}

static Eina_Bool _esvg_g_setup(Edom_Tag *t,
		Esvg_Element_Context *ctx,
		Esvg_Attribute_Presentation *attr,
		Esvg_Renderable_Context *rctx,
		Enesim_Error **error)
{
	Eina_Bool ret;

	ret = esvg_element_internal_child_setup(t, ctx,
		attr,
		error,
		_esvg_g_child_setup_filter,
		NULL,
		NULL,
		NULL);
	return ret;
}

static void _esvg_g_free(Edom_Tag *t)
{
	Esvg_G *thiz;

	thiz = _esvg_g_get(t);
	free(thiz);
}

static Esvg_Instantiable_Descriptor _descriptor = {
	/* .child_add		= */ _esvg_g_child_add,
	/* .child_remove	= */ NULL,
	/* .attribute_get 	= */ NULL,
	/* .cdata_set 		= */ NULL,
	/* .text_set 		= */ NULL,
	/* .free 		= */ _esvg_g_free,
	/* .initialize 		= */ NULL,
	/* .attribute_set 	= */ NULL,
	/* .clone		= */ _esvg_g_clone,
	/* .setup		= */ _esvg_g_setup,
	/* .renderer_get	= */ _esvg_g_renderer_get,
};
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static Edom_Tag * _esvg_g_new(void)
{
	Esvg_G *thiz;
	Edom_Tag *t;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_G));
	if (!thiz) return NULL;

	r = enesim_renderer_compound_new();
	thiz->r = r;
	enesim_renderer_rop_set(r, ENESIM_BLEND);

	t = esvg_instantiable_new(&_descriptor, ESVG_G, thiz);
	return t;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* The ender wrapper */
#include "generated/esvg_generated_g.c"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ender_Element * esvg_g_new(void)
{
	return ender_element_new_with_namespace("g", "esvg");
}

EAPI Eina_Bool esvg_is_g(Ender_Element *e)
{
}
