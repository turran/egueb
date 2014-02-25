/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
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
#include "egueb_dom_private.h"

#include "egueb_dom_string.h"
#include "egueb_dom_main.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_node.h"
#include "egueb_dom_feature.h"
#include "egueb_dom_feature_render.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_string_private.h"
#include "egueb_dom_feature_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_FEATURE_RENDER_DESCRIPTOR egueb_dom_feature_render_descriptor_get()
#define EGUEB_DOM_FEATURE_RENDER(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Dom_Feature_Render, EGUEB_DOM_FEATURE_RENDER_DESCRIPTOR)

typedef struct _Egueb_Dom_Feature_Render
{
	Egueb_Dom_Feature base;
	Egueb_Dom_Node *n;
	Eina_Tiler *tiler;
	int tw, th;
	const Egueb_Dom_Feature_Render_Descriptor *d;
} Egueb_Dom_Feature_Render;

typedef struct _Egueb_Dom_Feature_Render_Class
{
	Egueb_Dom_Feature_Class base;
} Egueb_Dom_Feature_Render_Class;

static Egueb_Dom_String _EGUEB_DOM_FEATURE_RENDER_NAME = EGUEB_DOM_STRING_STATIC("EguebDomRender");

static Eina_Bool _egueb_dom_feature_render_damage_cb(Enesim_Renderer *r,
		const Eina_Rectangle *area, Eina_Bool past,
		void *data)
{
	Eina_Tiler *tiler = data;

	eina_tiler_rect_add(tiler, area);
#if 0
	DBG("Renderer %s has changed at area %d %d %d %d",
			enesim_renderer_name_get(r),
			area->x, area->y, area->w, area->h);
#endif
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_FEATURE_DESCRIPTOR,
		Egueb_Dom_Feature_Render, Egueb_Dom_Feature_Render_Class,
		egueb_dom_feature_render);

static void _egueb_dom_feature_render_class_init(void *k)
{
}

static void _egueb_dom_feature_render_instance_init(void *o)
{
}

static void _egueb_dom_feature_render_instance_deinit(void *o)
{
	Egueb_Dom_Feature_Render *thiz;

	thiz = EGUEB_DOM_FEATURE_RENDER(o);
	if (thiz->tiler)
		eina_tiler_free(thiz->tiler);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Egueb_Dom_String *EGUEB_DOM_FEATURE_RENDER_NAME = &_EGUEB_DOM_FEATURE_RENDER_NAME;

EAPI Eina_Bool egueb_dom_feature_render_draw(Egueb_Dom_Feature *f, Enesim_Surface *s,
		Enesim_Rop rop, Eina_Rectangle *clip, int x, int y,
		Enesim_Log **error)
{
	Egueb_Dom_Feature_Render *thiz;
	Enesim_Renderer *r;
	Eina_Bool ret;

	thiz = EGUEB_DOM_FEATURE_RENDER(f);
	if (!thiz->n) return EINA_FALSE;

	r = thiz->d->renderer_get(thiz->n);
	ret = enesim_renderer_draw(r, s, rop, clip, x, y, error);
	enesim_renderer_unref(r);
	return ret;
}

EAPI Eina_Bool egueb_dom_feature_render_draw_list(Egueb_Dom_Feature *f,
		Enesim_Surface *s, Enesim_Rop rop, Eina_List *clips, int x,
		int y, Enesim_Log **error)
{
	Egueb_Dom_Feature_Render *thiz;
	Enesim_Renderer *r;
	Eina_Bool ret;

	thiz = EGUEB_DOM_FEATURE_RENDER(f);
	if (!thiz->n) return EINA_FALSE;

	r = thiz->d->renderer_get(thiz->n);
	ret = enesim_renderer_draw_list(r, s, rop, clips, x, y, error);
	enesim_renderer_unref(r);
	return ret;
}

EAPI Eina_Bool egueb_dom_feature_render_damages_get(Egueb_Dom_Feature *f,
		Enesim_Surface *s, Egueb_Dom_Feature_Render_Damage_Cb cb, void *data)
{
	Egueb_Dom_Feature_Render *thiz;
	Enesim_Renderer *r;
	Eina_Iterator *iter;
	Eina_Rectangle *rect;
	int w, h;

	thiz = EGUEB_DOM_FEATURE_RENDER(f);
	if (!thiz->n) return EINA_FALSE;

	enesim_surface_size_get(s, &w, &h);
	if (thiz->tw != w || thiz->th != h)
	{
		Eina_Rectangle full;

		if (thiz->tiler)
			eina_tiler_free(thiz->tiler);

		thiz->tiler = eina_tiler_new(w, h);
		thiz->tw = w;
		thiz->th = h;

		eina_rectangle_coords_from(&full, 0, 0, w, h);
		cb(f, &full, data);
		return EINA_TRUE;
	}
	r = thiz->d->renderer_get(thiz->n);
	enesim_renderer_damages_get(r, _egueb_dom_feature_render_damage_cb, thiz->tiler);

	iter = eina_tiler_iterator_new(thiz->tiler);
	EINA_ITERATOR_FOREACH(iter, rect)
	{
		cb(f, rect, data);
	}
	eina_iterator_free(iter);
	eina_tiler_clear(thiz->tiler);
	enesim_renderer_unref(r);
	return EINA_TRUE;
}

EAPI Eina_Bool egueb_dom_feature_render_add(Egueb_Dom_Node *n,
		const Egueb_Dom_Feature_Render_Descriptor *d)
{
	Egueb_Dom_Feature_Render *thiz;

	if (!n) return EINA_FALSE;
	if (!d) return EINA_FALSE;
	if (!d->renderer_get) return EINA_FALSE;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_feature_render);
	egueb_dom_node_weak_ref_add(n, &thiz->n);
	thiz->d = d;

	return egueb_dom_node_feature_add(n, EGUEB_DOM_FEATURE_RENDER_NAME, NULL, EGUEB_DOM_FEATURE(thiz));
}
