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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_CONTAINER_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_CONTAINER_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_CONTAINER_MAGIC);\
	} while(0)

typedef struct _Esvg_Container
{
	EINA_MAGIC
	/* properties */
	/* private */
	Esvg_Container_Element_Add element_add;
	Esvg_Container_Element_Remove element_remove;
	Esvg_Container_Element_At element_at;
	Esvg_Element_Clone clone;
	void *data;
} Esvg_Container;

static Esvg_Container * _esvg_container_get(Enesim_Renderer *r)
{
	Esvg_Container *thiz;

	thiz = esvg_element_data_get(r);
	ESVG_CONTAINER_MAGIC_CHECK(thiz);

	return thiz;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * esvg_container_data_get(Enesim_Renderer *r)
{
	Esvg_Container *thiz;

	thiz = _esvg_container_get(r);
	return thiz->data;
}

Enesim_Renderer * esvg_container_new(Esvg_Container_Descriptor *descriptor, void *data)
{
	Esvg_Container *thiz;
	Esvg_Element_Descriptor pdescriptor;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Container));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_CONTAINER_MAGIC);
	thiz->data = data;

	thiz->element_add = descriptor->element_add;
	thiz->element_remove = descriptor->element_remove;
	thiz->element_at = descriptor->element_at;

	pdescriptor.name_get = descriptor->name_get;
	pdescriptor.renderer_get = descriptor->renderer_get;
	pdescriptor.clone = descriptor->clone;
	pdescriptor.setup = descriptor->setup;
	pdescriptor.cleanup = descriptor->cleanup;
	pdescriptor.is_renderable = descriptor->is_renderable;

	r = esvg_element_new(&pdescriptor, thiz);
	return r;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_is_container(Enesim_Renderer *r)
{
	Esvg_Container *thiz;
	Eina_Bool ret;

	if (!esvg_is_element(r))
		return EINA_FALSE;
	thiz = esvg_element_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_CONTAINER_MAGIC);

	return ret;
}

/* TODO
 * add common API functions for external accessible functions like fill/stroke/whatever
 */
EAPI void esvg_container_element_add(Enesim_Renderer *r, Enesim_Renderer *child)
{
	Esvg_Container *thiz;

	if (!r) return;
	if (!child) return;

	if (!esvg_is_element(child))
		return;
	thiz = _esvg_container_get(r);
	if (!thiz) return;

	/* TODO check if the child already has a parent */
	if (thiz->element_add)
	{
		if (thiz->element_add(r, child))
			esvg_element_parent_set(child, r);
	}
}

EAPI void esvg_container_element_remove(Enesim_Renderer *r, Enesim_Renderer *child)
{
	Esvg_Container *thiz;

	if (!r) return;
	if (!child) return;

	if (!esvg_is_element(child))
		return;
	thiz = _esvg_container_get(r);
	if (!thiz) return;
	if (thiz->element_remove)
	{
		thiz->element_remove(r, child);
		esvg_element_parent_set(child, NULL);
	}
}

EAPI Enesim_Renderer * esvg_container_element_at(Enesim_Renderer *r, double x, double y)
{
	Esvg_Container *thiz;
	Enesim_Renderer *at = NULL;

	thiz = _esvg_container_get(r);

	if (thiz->element_at)
		at = thiz->element_at(r, x, y);

	return at;
}
