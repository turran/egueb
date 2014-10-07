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

#ifndef _EGUEB_CSS_FONT_H_
#define _EGUEB_CSS_FONT_H_

typedef enum _Egueb_Css_Font_Type {
	EGUEB_CSS_FONT_TYPE_USER,
	EGUEB_CSS_FONT_TYPE_CAPTION,
	EGUEB_CSS_FONT_TYPE_ICON,
	EGUEB_CSS_FONT_TYPE_MENU,
	EGUEB_CSS_FONT_TYPE_MESSAGE_BOX,
	EGUEB_CSS_FONT_TYPE_SMALL_CAPTION,
	EGUEB_CSS_FONT_TYPE_STATUS_BAR,
} Egueb_Css_Font_Type;

typedef struct _Egueb_Css_Font {
	Egueb_Css_Font_Type type;
	Egueb_Css_Font_Style style;
	Egueb_Css_Font_Variant variant;
	Egueb_Css_Font_Weight weight;
	Egueb_Css_Font_Size size;
	Egueb_Dom_List *family;
} Egueb_Css_Font;

#endif
