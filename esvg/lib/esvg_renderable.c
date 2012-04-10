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
#define ESVG_RENDERABLE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_RENDERABLE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_RENDERABLE_MAGIC);\
	} while(0)

typedef struct _Esvg_Renderable
{
	EINA_MAGIC
	/* properties */
	/* interface */
	Esvg_Renderable_Renderer_Get renderer_get;
	/* private */
	void *data;
} Esvg_Renderable;

static Esvg_Renderable * _esvg_renderable_get(Enesim_Renderer *r)
{
	Esvg_Renderable *thiz;

	thiz = esvg_element_data_get(r);
	ESVG_RENDERABLE_MAGIC_CHECK(thiz);

	return thiz;
}

static Enesim_Renderer * _esvg_renderable_renderer_get(Enesim_Renderer *r
		const Esvg_Element_State *state,
		const Esvg_Attribute_Presentation *attr)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * esvg_renderable_data_get(Enesim_Renderer *r)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(r);
	return thiz->data;
}

Enesim_Renderer * esvg_renderable_new(Esvg_Renderable_Descriptor *descriptor, void *data)
{
	Esvg_Renderable *thiz;
	Esvg_Element_Descriptor pdescriptor = {NULL, NULL, NULL, NULL, NULL, NULL, EINA_TRUE};
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Renderable));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_RENDERABLE_MAGIC);
	thiz->data = data;
	thiz->setup = descriptor->setup;
	thiz->clone = descriptor->clone;
	thiz->renderer_get = descriptor->renderer_get;
	thiz->has_changed = descriptor->has_changed;

	pdescriptor.renderer_get = _esvg_renderable_renderer_get;
	pdescriptor.name_get = descriptor->name_get;
	pdescriptor.setup = _esvg_renderable_setup;
	pdescriptor.cleanup = descriptor->cleanup;
	pdescriptor.clone = descriptor->clone;
	pdescriptor.has_changed = descriptor->has_changed;
	pdescriptor.is_renderable = EINA_TRUE;

	r = esvg_element_new(&pdescriptor, thiz);

	return r;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_is_renderable(Enesim_Renderer *r)
{
	Esvg_Renderable *thiz;
	Eina_Bool ret;

	if (!esvg_is_element(r))
		return EINA_FALSE;
	thiz = esvg_element_data_get(r);
	ret = EINA_MAGIC_CHECK(thiz, ESVG_RENDERABLE_MAGIC);

	return ret;
}

EAPI Enesim_Renderer * esvg_renderable_renderer_get(Enesim_Renderer *r)
{
	Esvg_Renderable *thiz;

	thiz = _esvg_renderable_get(r);
	return thiz->renderer_get(r);
}
