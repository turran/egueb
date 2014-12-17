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

/**
 * @defgroup Egueb_Svg_Display_Group Display
 * @brief Display type
 * @ingroup Egueb_Svg_Type_Group
 * @{
 */

typedef enum _Egueb_Svg_Display
{
	EGUEB_SVG_DISPLAY_INLINE,
	EGUEB_SVG_DISPLAY_BLOCK,
	EGUEB_SVG_DISPLAY_LIST_ITEM,
	EGUEB_SVG_DISPLAY_RUN_IN,
	EGUEB_SVG_DISPLAY_COMPACT,
	EGUEB_SVG_DISPLAY_MARKER,
	EGUEB_SVG_DISPLAY_TABLE,
	EGUEB_SVG_DISPLAY_INLINE_TABLE,
	EGUEB_SVG_DISPLAY_TABLE_ROW_GROUP,
	EGUEB_SVG_DISPLAY_TABLE_HEADER_GROUP,
	EGUEB_SVG_DISPLAY_TABLE_FOOTER_GROUP,
	EGUEB_SVG_DISPLAY_TABLE_ROW,
	EGUEB_SVG_DISPLAY_TABLE_COLUMN_GROUP,
	EGUEB_SVG_DISPLAY_TABLE_COLUMN,
	EGUEB_SVG_DISPLAY_TABLE_CELL,
	EGUEB_SVG_DISPLAY_TABLE_CAPTION,
	EGUEB_SVG_DISPLAY_NONE,
} Egueb_Svg_Display;

/**
 * @}
 */

#endif
