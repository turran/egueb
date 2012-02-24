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
#include "Esvg_Parser.h"
#include "esvg_parser_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static const char * _parser_defs_name_get(Edom_Tag *tag)
{
	return "defs";
}

static Eina_Bool _parser_defs_child_supported(Edom_Tag *tag, int tag_id)
{
	switch (tag_id)
	{
		case ESVG_LINEARGRADIENT:
		case ESVG_RADIALGRADIENT:
		case ESVG_PATTERN:
		case ESVG_DEFS:
		case ESVG_USE:
		case ESVG_SVG:
		case ESVG_CIRCLE:
		case ESVG_ELLIPSE:
		case ESVG_RECT:
		case ESVG_LINE:
		case ESVG_PATH:
		case ESVG_POLYLINE:
		case ESVG_POLYGON:
		case ESVG_TEXT:
		case ESVG_G:
		case ESVG_STYLE:
		case ESVG_IMAGE:
		case ESVG_CLIPPATH:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}

static Eina_Bool _parser_defs_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Edom_Tag *topmost;

	/* FIXME we need to add some other way to do this, maybe in the future
	 * events that bubble?
	 */
	topmost = edom_tag_topmost_get(tag);
	esvg_parser_svg_tag_add(topmost, child);
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_defs_name_get,
	/* .attribute_set 	= */ NULL,
	/* .attribute_get 	= */ NULL,
	/* .child_supported	= */ _parser_defs_child_supported,
	/* .child_add		= */ _parser_defs_child_add,
	/* .child_remove	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Context * esvg_parser_defs_new(Edom_Parser *parser)
{
	return edom_tag_new(parser, &_descriptor, ESVG_DEFS, NULL);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
