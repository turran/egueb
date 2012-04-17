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
#define ESVG_USE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_USE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_USE_MAGIC);\
	} while(0)

typedef struct _Esvg_Use
{
	EINA_MAGIC
	/* properties */
	Esvg_Coord x;
	Esvg_Coord y;
	Esvg_Length width;
	Esvg_Length height;
	Enesim_Renderer *link;
	/* private */
	Enesim_Renderer *g;
} Esvg_Use;

static Esvg_Use * _esvg_use_get(Enesim_Renderer *r)
{
	Esvg_Use *thiz;

	thiz = esvg_element_data_get(r);
	ESVG_USE_MAGIC_CHECK(thiz);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                         The ESVG element interface                         *
 *----------------------------------------------------------------------------*/
static const char * _esvg_use_name_get(Enesim_Renderer *r)
{
	return "use";
}

static Enesim_Renderer * _esvg_use_renderer_get(Enesim_Renderer *r,
		const Esvg_Element_Context *state,
		const Esvg_Attribute_Presentation *attr)
{
	Esvg_Use *thiz;
	Enesim_Renderer *g_r;

	thiz = _esvg_use_get(r);
	g_r = esvg_element_renderer_get(thiz->g);

	return g_r;
}

static Eina_Bool _esvg_use_setup(Enesim_Renderer *r,
		Esvg_Element_Context *estate,
		Esvg_Attribute_Presentation *attr,
		Enesim_Surface *s,
		Enesim_Error **error)
{
	Esvg_Use *thiz;
	Enesim_Matrix translate;
	double tx, ty;

	thiz = _esvg_use_get(r);
	/* we should append a new transformation */
	tx = esvg_length_final_get(&thiz->x, estate->viewbox_w);
	ty = esvg_length_final_get(&thiz->y, estate->viewbox_h);
	enesim_matrix_translate(&translate, tx, ty);
	enesim_matrix_compose(&estate->transform, &translate, &estate->transform);

	/* we take the shortcut here because there's no need to go through
	 * the normal enesim API
	 */
	/* setup the g */
	printf("calling the setup on the use\n");

	return esvg_element_setup(thiz->g, estate, attr, s, error);
}

static void _esvg_use_cleanup(Enesim_Renderer *r)
{

}

static void _esvg_use_clone(Enesim_Renderer *r, Enesim_Renderer *dr)
{
	/* inst this too much? */
}

static Esvg_Element_Descriptor _descriptor = {
	/* .name_get =		*/ _esvg_use_name_get,
	/* .renderer_get =	*/ _esvg_use_renderer_get,
	/* .clone =		*/ _esvg_use_clone,
	/* .setup =		*/ _esvg_use_setup,
	/* .cleanup =		*/ _esvg_use_cleanup,
	/* .is_renderable = 	*/ EINA_TRUE,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Renderer * esvg_use_new(void)
{
	Esvg_Use *thiz;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Use));
	if (!thiz) return NULL;
	EINA_MAGIC_SET(thiz, ESVG_USE_MAGIC);
	thiz->g = esvg_g_new();

	/* Default values */
	thiz->x = ESVG_COORD_0;
	thiz->y = ESVG_COORD_0;
	thiz->width = ESVG_LENGTH_0;
	thiz->height = ESVG_LENGTH_0;

	r = esvg_element_new(&_descriptor, thiz);
	return r;
}

EAPI Eina_Bool esvg_is_use(Enesim_Renderer *r)
{
	Esvg_Use *thiz;
	Eina_Bool ret;

	if (!esvg_is_element(r))
		return EINA_FALSE;
	thiz = esvg_element_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_USE_MAGIC);

	return ret;
}

EAPI void esvg_use_x_set(Enesim_Renderer *r, const Esvg_Coord *x)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(r);
	if (x) thiz->x = *x;
}

EAPI void esvg_use_x_get(Enesim_Renderer *r, Esvg_Coord *x)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(r);
	if (x) *x = thiz->x;
}

EAPI void esvg_use_y_set(Enesim_Renderer *r, const Esvg_Coord *y)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(r);
	if (y) thiz->y = *y;
}

EAPI void esvg_use_y_get(Enesim_Renderer *r, Esvg_Coord *y)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(r);
	if (y) *y = thiz->y;
}

EAPI void esvg_use_width_set(Enesim_Renderer *r, const Esvg_Length *width)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(r);
	if (width) thiz->width = *width;
}

EAPI void esvg_use_width_get(Enesim_Renderer *r, Esvg_Length *width)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(r);
	if (width) *width = thiz->width;
}

EAPI void esvg_use_height_set(Enesim_Renderer *r, const Esvg_Length *height)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(r);
	if (height) thiz->height = *height;
}

EAPI void esvg_use_height_get(Enesim_Renderer *r, Esvg_Length *height)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(r);
	if (height) *height = thiz->height;
}

EAPI void esvg_use_link_set(Enesim_Renderer *r, const Enesim_Renderer *link)
{
	Esvg_Use *thiz;

	thiz = _esvg_use_get(r);
	if (thiz->link == link) return;
	if (thiz->link)
	{
		esvg_container_element_remove(thiz->g, thiz->link);
		enesim_renderer_unref(thiz->link);
		thiz->link = NULL;
	}
	if (link)
	{
		esvg_container_element_add(thiz->g, link);
		thiz->link = enesim_renderer_ref(link);
	}
}

EAPI void esvg_use_link_get(Enesim_Renderer *r, Enesim_Renderer **link)
{
	Esvg_Use *thiz;

	if (!link) return;
	thiz = _esvg_use_get(r);
	*link = thiz->link;
	if (thiz->link)
		thiz->link = enesim_renderer_ref(thiz->link);
}
