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
#ifndef _EGUEB_CSS_FONT_SIZE_H
#define _EGUEB_CSS_FONT_SIZE_H

typedef enum _Egueb_Css_Font_Size_Type
{
	EGUEB_CSS_FONT_SIZE_TYPE_ABSOLUTE,
	EGUEB_CSS_FONT_SIZE_TYPE_RELATIVE,
	EGUEB_CSS_FONT_SIZE_TYPE_LENGTH,
	EGUEB_CSS_FONT_SIZE_TYPE_PERCENTAGE,
} Egueb_Css_Font_Size_Type;

typedef enum _Egueb_Css_Font_Size_Absolute
{
	EGUEB_CSS_FONT_SIZE_ABSOLUTE_XX_SMALL,
	EGUEB_CSS_FONT_SIZE_ABSOLUTE_X_SMALL,
	EGUEB_CSS_FONT_SIZE_ABSOLUTE_SMALL,
	EGUEB_CSS_FONT_SIZE_ABSOLUTE_MEDIUM,
	EGUEB_CSS_FONT_SIZE_ABSOLUTE_LARGE,
	EGUEB_CSS_FONT_SIZE_ABSOLUTE_X_LARGE,
	EGUEB_CSS_FONT_SIZE_ABSOLUTE_XX_LARGE,
} Egueb_Css_Font_Size_Absolute;

typedef enum _Egueb_Css_Font_Size_Relative
{
	EGUEB_CSS_FONT_SIZE_RELATIVE_LARGER,
	EGUEB_CSS_FONT_SIZE_RELATIVE_SMALLER,
} Egueb_Css_Font_Size_Relative;

typedef union _Egueb_Css_Font_Size_Value
{
	Egueb_Css_Font_Size_Absolute absolute;
	Egueb_Css_Font_Size_Relative relative;
	Egueb_Css_Length length;
	double percentage;
} Egueb_Css_Font_Size_Value;

typedef struct _Egueb_Css_Font_Size
{
	Egueb_Css_Font_Size_Type type;
	Egueb_Css_Font_Size_Value value;
} Egueb_Css_Font_Size;

EAPI Eina_Bool egueb_css_font_size_string_from(Egueb_Css_Font_Size *thiz,
		const char *str);
EAPI const char * egueb_css_font_size_string_to(Egueb_Css_Font_Size thiz);

#endif
