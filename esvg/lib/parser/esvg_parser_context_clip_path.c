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
/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _clip_path_tag_is_supported(int tag)
{
	switch (tag)
	{
		case ESVG_CIRCLE:
		case ESVG_ELLIPSE:
		case ESVG_RECT:
		case ESVG_LINE:
		case ESVG_PATH:
		case ESVG_POLYLINE:
		case ESVG_POLYGON:
		case ESVG_TEXT:
		case ESVG_USE:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Context * esvg_parser_context_clip_path_new(
		Edom_Tag *svg,
		Edom_Tag *parent,
		Enesim_Renderer *parent_r)
{
	if (!parent) return NULL;

	return esvg_parser_context_new(_clip_path_tag_is_supported,
		ESVG_CLIPPATH, svg, parent, parent_r);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

