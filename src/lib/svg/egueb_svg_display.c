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

#include "egueb_svg_main_private.h"
#include "egueb_svg_display.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_display_string_from(Egueb_Svg_Display *d, const char *attr)
{
	if (!strcmp(attr, "inline"))
		*d = EGUEB_SVG_DISPLAY_INLINE;
	else if (!strcmp(attr, "block"))
		*d = EGUEB_SVG_DISPLAY_BLOCK;
	else if (!strcmp(attr, "list-item"))
		*d = EGUEB_SVG_DISPLAY_LIST_ITEM;
	else if (!strcmp(attr, "run-in"))
		*d = EGUEB_SVG_DISPLAY_RUN_IN;
	else if (!strcmp(attr, "compact"))
		*d = EGUEB_SVG_DISPLAY_COMPACT;
	else if (!strcmp(attr, "marker"))
		*d = EGUEB_SVG_DISPLAY_MARKER;
	else if (!strcmp(attr, "table"))
		*d = EGUEB_SVG_DISPLAY_TABLE;
	else if (!strcmp(attr, "inline-table"))
		*d = EGUEB_SVG_DISPLAY_INLINE_TABLE;
	else if (!strcmp(attr, "table-row-group"))
		*d = EGUEB_SVG_DISPLAY_TABLE_ROW_GROUP;
	else if (!strcmp(attr, "table-header-group"))
		*d = EGUEB_SVG_DISPLAY_TABLE_HEADER_GROUP;
	else if (!strcmp(attr, "table-footer-group"))
		*d = EGUEB_SVG_DISPLAY_TABLE_FOOTER_GROUP;
	else if (!strcmp(attr, "table-row"))
		*d = EGUEB_SVG_DISPLAY_TABLE_ROW;
	else if (!strcmp(attr, "table-column-group"))
		*d = EGUEB_SVG_DISPLAY_TABLE_COLUMN_GROUP;
	else if (!strcmp(attr, "table-column"))
		*d = EGUEB_SVG_DISPLAY_TABLE_COLUMN;
	else if (!strcmp(attr, "table-cell"))
		*d = EGUEB_SVG_DISPLAY_TABLE_CELL;
	else if (!strcmp(attr, "table-caption"))
		*d = EGUEB_SVG_DISPLAY_TABLE_CAPTION;
	else if (!strcmp(attr, "none"))
		*d = EGUEB_SVG_DISPLAY_NONE;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}
