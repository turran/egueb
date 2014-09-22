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

#ifndef _EGUEB_CSS_FONT_WEIGHT_H_
#define _EGUEB_CSS_FONT_WEIGHT_H_

typedef enum _Egueb_Css_Font_Weight {
	EGUEB_CSS_FONT_WEIGHT_NORMAL,
	EGUEB_CSS_FONT_WEIGHT_BOLD,
	EGUEB_CSS_FONT_WEIGHT_BOLDER,
	EGUEB_CSS_FONT_WEIGHT_LIGHTER, 
	EGUEB_CSS_FONT_WEIGHT_100,
	EGUEB_CSS_FONT_WEIGHT_200,
	EGUEB_CSS_FONT_WEIGHT_300,
	EGUEB_CSS_FONT_WEIGHT_400,
	EGUEB_CSS_FONT_WEIGHT_500,
	EGUEB_CSS_FONT_WEIGHT_600,
	EGUEB_CSS_FONT_WEIGHT_700,
	EGUEB_CSS_FONT_WEIGHT_800,
	EGUEB_CSS_FONT_WEIGHT_900,
} Egueb_Css_Font_Weight;

EAPI Egueb_Dom_Node * egueb_css_font_weight_attr_new(
		Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable);

EAPI const Egueb_Dom_Value_Descriptor * egueb_css_font_weight_value_descriptor_get(void);


#endif
