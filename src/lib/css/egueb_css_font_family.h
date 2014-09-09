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

#ifndef _EGUEB_CSS_FONT_FAMILY_H_
#define _EGUEB_CSS_FONT_FAMILY_H_

typedef enum _Egueb_Css_Font_Family_Type {
	EGUEB_CSS_FONT_FAMILY_TYPE_FAMILY,
	EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_SERIF,
	EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_SANS_SERIF,
	EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_CURSIVE,
	EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_FANTASY,
	EGUEB_CSS_FONT_FAMILY_TYPE_GENERIC_MONOSPACE,
} Egueb_Css_Font_Family_Type;

typedef struct _Egueb_Css_Font_Family_Value {
	Egueb_Css_Font_Family_Type type;
	Egueb_Dom_String *family;
} Egueb_Css_Font_Family_Value;

EAPI Egueb_Dom_List * egueb_css_font_family_new(void);

#endif
