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
#ifndef _EGUEB_SVG_FONT_SIZE_H
#define _EGUEB_SVG_FONT_SIZE_H

typedef enum _Egueb_Svg_Font_Size_Type
{
	EGUEB_SVG_FONT_SIZE_TYPE_LENGTH,
	EGUEB_SVG_FONT_SIZE_TYPE_RELATIVE,
	EGUEB_SVG_FONT_SIZE_TYPE_ABSOLUTE,
} Egueb_Svg_Font_Size_Type;

typedef enum _Egueb_Svg_Font_Size_Absolute
{
	EGUEB_SVG_FONT_SIZE_ABSOLUTE_XX_SMALL,
	EGUEB_SVG_FONT_SIZE_ABSOLUTE_X_SMALL,
	EGUEB_SVG_FONT_SIZE_ABSOLUTE_SMALL,
	EGUEB_SVG_FONT_SIZE_ABSOLUTE_MEDIUM,
	EGUEB_SVG_FONT_SIZE_ABSOLUTE_LARGE,
	EGUEB_SVG_FONT_SIZE_ABSOLUTE_X_LARGE,
	EGUEB_SVG_FONT_SIZE_ABSOLUTE_XX_LARGE,
} Egueb_Svg_Font_Size_Absolute;

typedef enum _Egueb_Svg_Font_Size_Relative
{
	EGUEB_SVG_FONT_SIZE_RELATIVE_LARGER,
	EGUEB_SVG_FONT_SIZE_RELATIVE_SMALLER,
} Egueb_Svg_Font_Size_Relative;

typedef struct _Egueb_Svg_Font_Size_Value
{
	Egueb_Svg_Length length;
	Egueb_Svg_Font_Size_Relative relative;
	Egueb_Svg_Font_Size_Absolute absolute;
} Egueb_Svg_Font_Size_Value;

typedef struct _Egueb_Svg_Font_Size
{
	Egueb_Svg_Font_Size_Type type;
	Egueb_Svg_Font_Size_Value value;
} Egueb_Svg_Font_Size;

typedef struct _Egueb_Svg_Font_Size_Animated
{
	Egueb_Svg_Font_Size base;
	Egueb_Svg_Font_Size anim;
} Egueb_Svg_Font_Size_Animated;

const extern Egueb_Svg_Font_Size EGUEB_SVG_FONT_SIZE_MEDIUM;

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_font_size_descriptor_get(void);
EAPI Eina_Bool egueb_svg_font_size_string_from(Egueb_Svg_Font_Size *font_size, const char *attr_val);
EAPI char * egueb_svg_font_size_string_to(Egueb_Svg_Font_Size *thiz);
EAPI double egueb_svg_font_size_final_get(const Egueb_Svg_Font_Size *thiz,
		double width, double height, double font_size,
		double parent_font_size);

#endif

