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
#ifndef _EGUEB_DOM_FEATURE_RENDER_H_
#define _EGUEB_DOM_FEATURE_RENDER_H_

/* We need to implement this kind of common behaviour of nodes
 * using features so we can create common libs on top of it.
 * The render and input interfaces will be used on window system
 * maybe later we need to add a window manager one, for things
 * like eon's hints
 */

EAPI extern Egueb_Dom_String *EGUEB_DOM_FEATURE_RENDER;

typedef Enesim_Renderer * (*Egueb_Dom_Feature_Render_Renderer_Get)(
		Egueb_Dom_Node *n);

typedef struct _Egueb_Dom_Feature_Descriptor
{
	Egueb_Dom_Feature_Render_Renderer_Get renderer_get;
} Egueb_Dom_Feature_Descriptor;

typedef Eina_Bool (*Egueb_Dom_Feature_Render_Damage_Cb)(Egueb_Dom_Feature *f,
		Eina_Rectangle *damage, void *data);

EAPI Eina_Bool egueb_dom_feature_render_draw(Egueb_Dom_Feature *f, Enesim_Surface *s,
		Enesim_Rop rop, Eina_Rectangle *clip, int x, int y,
		Enesim_Log **error);
EAPI Eina_Bool egueb_dom_feature_render_draw_list(Egueb_Dom_Feature *f,
		Enesim_Surface *s, Enesim_Rop rop, Eina_List *clips, int x,
		int y, Enesim_Log **error);
EAPI void egueb_dom_feature_render_damages_get(Egueb_Dom_Feature *f,
		Egueb_Dom_Feature_Render_Damage_Cb cb, void *data);

EAPI void egueb_dom_feature_render_add(Egueb_Dom_Node *n,
		const Egueb_Dom_Feature_Descriptor *d);

#endif
