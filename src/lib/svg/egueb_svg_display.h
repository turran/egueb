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
#ifndef _EGUEB_SVG_DISPLAY_H
#define _EGUEB_SVG_DISPLAY_H

typedef enum _Egueb_Svg_Display
{
	ESVG_DISPLAY_INLINE,
	ESVG_DISPLAY_BLOCK,
	ESVG_DISPLAY_LIST_ITEM,
	ESVG_DISPLAY_RUN_IN,
	ESVG_DISPLAY_COMPACT,
	ESVG_DISPLAY_MARKER,
	ESVG_DISPLAY_TABLE,
	ESVG_DISPLAY_INLINE_TABLE,
	ESVG_DISPLAY_TABLE_ROW_GROUP,
	ESVG_DISPLAY_TABLE_HEADER_GROUP,
	ESVG_DISPLAY_TABLE_FOOTER_GROUP,
	ESVG_DISPLAY_TABLE_ROW,
	ESVG_DISPLAY_TABLE_COLUMN_GROUP,
	ESVG_DISPLAY_TABLE_COLUMN,
	ESVG_DISPLAY_TABLE_CELL,
	ESVG_DISPLAY_TABLE_CAPTION,
	ESVG_DISPLAY_NONE,
} Egueb_Svg_Display;

EAPI Eina_Bool egueb_svg_display_string_from(Egueb_Svg_Display *d, const char *attr);

#endif
