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

#if 0
static void _esvg_element_draw(Enesim_Renderer *r,
		const Enesim_Renderer_State *state,
		int x, int y, unsigned int len, void *dst)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	/* the real renderer should be set at the setup */
	enesim_renderer_sw_draw(thiz->real_r, x, y, len, dst);
}

static const char * _esvg_element_name(Enesim_Renderer *r)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
	if (thiz->name_get) return thiz->name_get(r);
	return "element";
}

static void _esvg_element_boundings(Enesim_Renderer *r,
		const Enesim_Renderer_State *states[ENESIM_RENDERER_STATES],
		Enesim_Rectangle *rect)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	/* TODO in case we have changed call the setup again */
	real_r = _esvg_element_renderer_get(thiz, r);
	enesim_renderer_boundings(real_r, rect);
}

static void _esvg_element_destination_boundings(Enesim_Renderer *r,
		const Enesim_Renderer_State *states[ENESIM_RENDERER_STATES],
		Eina_Rectangle *rect)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	/* TODO in case we have changed call the setup again */
	real_r = _esvg_element_renderer_get(thiz, r);
	enesim_renderer_destination_boundings(real_r, rect, 0, 0);
}

static void _esvg_element_free(Enesim_Renderer *r)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
	/* TODO */
}

static Eina_Bool _esvg_element_sw_setup(Enesim_Renderer *r,
		const Enesim_Renderer_State *states[ENESIM_RENDERER_STATES],
		Enesim_Surface *s,
		Enesim_Renderer_Sw_Fill *fill, Enesim_Error **error)
{
	Esvg_Element *thiz;
	Enesim_Renderer *parent;
	Esvg_Attribute_Presentation *parent_attr = NULL;
	Esvg_Element_State *parent_state = NULL;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	parent = thiz->parent;
	if (parent)
	{
		Esvg_Element *parent_thiz;

		parent_thiz = _esvg_element_get(parent);
		parent_attr = &parent_thiz->attr_final;
		parent_state = &parent_thiz->state_final;
	}

	esvg_element_setup(r, parent_state, parent_attr, s, error);

	real_r = _esvg_element_renderer_get(thiz, r);
	if (!real_r)
		return EINA_FALSE;

	if (!enesim_renderer_setup(real_r, s, error))
		return EINA_FALSE;

	thiz->real_r = real_r;
	*fill = _esvg_element_draw;

	return EINA_TRUE;
}

static void _esvg_element_sw_cleanup(Enesim_Renderer *r, Enesim_Surface *s)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
}

static void _esvg_element_flags(Enesim_Renderer *r, const Enesim_Renderer_State *state,
		Enesim_Renderer_Flag *flags)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	real_r = _esvg_element_renderer_get(thiz, r);
	enesim_renderer_flags(real_r, flags);
}

static void _esvg_element_hints(Enesim_Renderer *r, const Enesim_Renderer_State *state,
		Enesim_Renderer_Hint *hints)
{
	Esvg_Element *thiz;
	Enesim_Renderer *real_r;

	thiz = _esvg_element_get(r);
	real_r = _esvg_element_renderer_get(thiz, r);
	enesim_renderer_hints_get(real_r, hints);
}


static Eina_Bool _esvg_element_has_changed(Enesim_Renderer *r,
		const Enesim_Renderer_State *states[ENESIM_RENDERER_STATES])
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
	if (thiz->has_changed)
		return thiz->has_changed(r);
	return EINA_FALSE;
}

static void _esvg_element_damage(Enesim_Renderer *r, Enesim_Renderer_Damage_Cb cb, void *data)
{
	Esvg_Element *thiz;

	thiz = _esvg_element_get(r);
}

static Eina_Bool _esvg_element_is_inside(Enesim_Renderer *r, double x, double y)
{
	/* FIXME */
	return EINA_TRUE;
}

static Enesim_Renderer_Descriptor _descriptor = {
	/* .version =               */ ENESIM_RENDERER_API,
	/* .name =                  */ _esvg_element_name,
	/* .free =                  */ _esvg_element_free,
	/* .boundings =             */ _esvg_element_boundings,
	/* .destination_boundings = */ _esvg_element_destination_boundings,
	/* .flags =                 */ _esvg_element_flags,
	/* .hints_get =             */ _esvg_element_hints,
	/* .is_inside =             */ _esvg_element_is_inside,
	/* .damage =                */ _esvg_element_damage,
	/* .has_changed =           */ _esvg_element_has_changed,
	/* .sw_setup =              */ _esvg_element_sw_setup,
	/* .sw_cleanup =            */ _esvg_element_sw_cleanup,
	/* .ocl_setup =             */ NULL,
	/* .ocl_kernel_cleanup =    */ NULL,
	/* .ocl_cleanup =           */ NULL
};
#endif

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
