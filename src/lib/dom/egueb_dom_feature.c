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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_dom_feature_render_draw(Egueb_Dom_Feature *f, Enesim_Surface *s,
		Enesim_Rop rop, Eina_Rectangle *clip, int x, int y,
		Enesim_Log **error)
{
	/* get the renderer */
}

EAPI Eina_Bool egueb_dom_feature_render_draw_list(Egueb_Dom_Feature *f,
		Enesim_Surface *s, Enesim_Rop rop, Eina_List *clips, int x,
		int y, Enesim_Log **error)
{
	/* get the renderer */
}

EAPI void egueb_dom_feature_render_damages_get(Egueb_Dom_Feature *f,
		Egueb_Dom_Feature_Render_Damage_Cb cb, void *data)
{
	/* get the renderer */
}

EAPI void egueb_dom_feature_render_add(Egueb_Dom_Node *n,
		const Egueb_Dom_Feature_Descriptor *d)
{

}
