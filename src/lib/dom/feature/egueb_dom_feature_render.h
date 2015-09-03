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

/**
 * @file
 * @ender_group{Egueb_Dom_Feature_Render}
 * @ender_group{Egueb_Dom_Feature_Render_Definition}
 */

/**
 * @defgroup Egueb_Dom_Feature_Render_Definition Definitions
 * @ingroup Egueb_Dom_Feature_Render
 * @{
 */

EAPI extern Egueb_Dom_String *EGUEB_DOM_FEATURE_RENDER_NAME;

/**
 * @ender_name{egueb.dom.feature.render.descriptor.renderer_get}
 */
typedef Enesim_Renderer * (*Egueb_Dom_Feature_Render_Descriptor_Renderer_Get)(
		Egueb_Dom_Node *n);

#define EGUEB_DOM_FEATURE_RENDER_DESCRIPTOR_VERSION 0
typedef struct _Egueb_Dom_Feature_Render_Descriptor
{
	int version;
	Egueb_Dom_Feature_Render_Descriptor_Renderer_Get renderer_get;
} Egueb_Dom_Feature_Render_Descriptor;

/**
 * @ender_name{egueb.dom.feature.render.damage_cb}
 */
typedef Eina_Bool (*Egueb_Dom_Feature_Render_Damage_Cb)(Egueb_Dom_Feature *f,
		Eina_Rectangle *damage, void *data);

/**
 * @}
 * @defgroup Egueb_Dom_Feature_Render Rendering
 * @brief Node rendering features @ender_inherits{Egueb_Dom_Feature}
 * @ingroup Egueb_Dom_Feature
 * @{
 */

EAPI Eina_Bool egueb_dom_feature_render_draw(Egueb_Dom_Feature *f, Enesim_Surface *s,
		Enesim_Rop rop, Eina_Rectangle *clip, int x, int y,
		Enesim_Log **error);
EAPI Eina_Bool egueb_dom_feature_render_draw_list(Egueb_Dom_Feature *f,
		Enesim_Surface *s, Enesim_Rop rop, Eina_List *clips, int x,
		int y, Enesim_Log **error);
EAPI Eina_Bool egueb_dom_feature_render_damages_get(Egueb_Dom_Feature *f,
		Enesim_Surface *s, Egueb_Dom_Feature_Render_Damage_Cb cb, void *data);

EAPI Eina_Bool egueb_dom_feature_render_add(Egueb_Dom_Node *n,
		const Egueb_Dom_Feature_Render_Descriptor *d);

/**
 * @}
 */

#endif
