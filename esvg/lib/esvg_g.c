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
#define ESVG_G_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_G_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_G_MAGIC);\
	} while(0)

typedef struct _Esvg_G
{
	EINA_MAGIC
	/* properties */
	/* private */
	Enesim_Renderer *r;
	Eina_List *children; /* FIXME remove this */
} Esvg_G;

static Esvg_G * _esvg_g_get(Enesim_Renderer *r)
{
	Esvg_G *thiz;

	thiz = esvg_container_data_get(r);
	ESVG_G_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                          The Container interface                           *
 *----------------------------------------------------------------------------*/
static const char * _esvg_g_name_get(Enesim_Renderer *r)
{
	return "g";
}

static Eina_Bool _esvg_g_element_add(Enesim_Renderer *r, Enesim_Renderer *child)
{
	Esvg_G *thiz;
	Enesim_Renderer *real_r;

	if (!esvg_element_is_renderable(child))
		return EINA_FALSE;

	thiz = _esvg_g_get(r);
	thiz->children = eina_list_append(thiz->children, child);
	enesim_renderer_compound_layer_add(thiz->r, child);

	return EINA_TRUE;
}

static void _esvg_g_element_remove(Enesim_Renderer *r, Enesim_Renderer *child)
{
	Esvg_G *thiz;

	thiz = _esvg_g_get(r);
	thiz->children = eina_list_remove(thiz->children, child);
	enesim_renderer_compound_layer_remove(thiz->r, child);
}

static Enesim_Renderer * _esvg_g_element_at(Enesim_Renderer *r, double x, double y)
{
	/* TODO */
	return NULL;
}

static Eina_Bool _esvg_g_setup(Enesim_Renderer *r, Esvg_Element_State *state,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error)
{
	/* nothing to do here */
	printf("calling the setup on the g\n");
	return EINA_TRUE;
}

Enesim_Renderer * _esvg_g_renderer_get(Enesim_Renderer *r,
		const Esvg_Element_State *state,
		const Esvg_Attribute_Presentation *attr)
{
	Esvg_G *thiz;

	thiz = _esvg_g_get(r);
	return thiz->r;
}

static void _esvg_g_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{
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
}

static Esvg_Container_Descriptor _descriptor = {
	/* .element_add	= */ _esvg_g_element_add,
	/* .element_remove	= */ _esvg_g_element_remove,
	/* .element_at		= */ _esvg_g_element_at,
	/* .name_get		= */ _esvg_g_name_get,
	/* .renderer_get	= */ _esvg_g_renderer_get,
	/* .clone		= */ _esvg_g_clone,
	/* .setup		= */ _esvg_g_setup,
	/* .cleanup		= */ NULL,
	/* .is_renderable	= */ EINA_TRUE,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_g_new(void)
{
	Esvg_G *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_G));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_G_MAGIC);

	r = enesim_renderer_compound_new();
	thiz->r = r;
	enesim_renderer_rop_set(r, ENESIM_BLEND);

	r = esvg_container_new(&_descriptor, thiz);

	return r;
}

EAPI Eina_Bool esvg_is_g(Enesim_Renderer *r)
{
	Esvg_G *thiz;
	Eina_Bool ret;

	if (!esvg_is_container(r))
		return EINA_FALSE;
	thiz = esvg_container_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_G_MAGIC);

	return ret;
}
