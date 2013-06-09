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
#include "egueb_svg_main_private.h"
#include "egueb_svg_private_attribute_presentation.h"
#include "egueb_svg_context_private.h"
#include "egueb_svg_element_private.h"
#include "egueb_svg_renderable_private.h"

#include "egueb_svg_element.h"
#include "egueb_svg_renderable.h"

/* This abstraction should add every shape into its own list of renderables
 * Basically <g> and <svg> should inherit from here
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
//#define ESVG_LOG_DEFAULT egueb_svg_log_container

#define ESVG_CONTAINER_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_CONTAINER_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_CONTAINER_MAGIC);\
	} while(0)

typedef struct _Egueb_Svg_Container_Descriptor_Internal
{
	Egueb_Dom_Tag_Free free;
	Egueb_Dom_Tag_Child_Add child_add;
	Egueb_Dom_Tag_Child_Remove child_remove;
} Egueb_Svg_Container_Descriptor_Internal;

typedef struct _Egueb_Svg_Container
{
	EINA_MAGIC
	/* properties */
	/* private */
	Egueb_Svg_Container_Descriptor_Internal descriptor;
	void *data;
} Egueb_Svg_Container;

static Egueb_Svg_Container * _egueb_svg_container_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Container *thiz;

	thiz = egueb_svg_renderable_data_get(t);
	ESVG_CONTAINER_MAGIC_CHECK(thiz);

	return thiz;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * egueb_svg_container_data_get(Enesim_Renderer *r)
{
	Egueb_Svg_Container *thiz;

	thiz = _egueb_svg_container_get(r);
	return thiz->data;
}

Enesim_Renderer * egueb_svg_container_new(Egueb_Svg_Container_Descriptor *descriptor, void *data)
{
	Egueb_Svg_Container *thiz;
	Egueb_Svg_Element_Descriptor pdescriptor;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Egueb_Svg_Container));
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
	pdescriptor.has_changed = NULL;
	pdescriptor.is_renderable = descriptor->is_renderable;

	r = egueb_svg_element_new(&pdescriptor, thiz);
	return r;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_is_container(Enesim_Renderer *r)
{
	Egueb_Svg_Container *thiz;
	Eina_Bool ret;

	if (!egueb_svg_is_element(r))
		return EINA_FALSE;
	thiz = egueb_svg_element_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_CONTAINER_MAGIC);

	return ret;
}

/* TODO
 * add common API functions for external accessible functions like fill/stroke/whatever
 */
EAPI void egueb_svg_container_element_add(Enesim_Renderer *r, Enesim_Renderer *child)
{
	Egueb_Svg_Container *thiz;

	if (!r) return;
	if (!child) return;

	if (!egueb_svg_is_element(child))
		return;
	thiz = _egueb_svg_container_get(r);
	if (!thiz) return;

	/* TODO check if the child already has a parent */
	if (thiz->element_add)
	{
		if (thiz->element_add(r, child))
			egueb_svg_element_parent_set(child, r);
	}
}

EAPI void egueb_svg_container_element_remove(Enesim_Renderer *r, Enesim_Renderer *child)
{
	Egueb_Svg_Container *thiz;

	if (!r) return;
	if (!child) return;

	if (!egueb_svg_is_element(child))
		return;
	thiz = _egueb_svg_container_get(r);
	if (!thiz) return;
	if (thiz->element_remove)
	{
		thiz->element_remove(r, child);
		egueb_svg_element_parent_set(child, NULL);
	}
}

EAPI Enesim_Renderer * egueb_svg_container_element_at(Enesim_Renderer *r, double x, double y)
{
	Egueb_Svg_Container *thiz;
	Enesim_Renderer *at = NULL;

	thiz = _egueb_svg_container_get(r);

	if (thiz->element_at)
		at = thiz->element_at(r, x, y);

	return at;
}
