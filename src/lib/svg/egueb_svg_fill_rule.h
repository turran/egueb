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
#ifndef _EGUEB_SVG_FILL_RULE_H
#define _EGUEB_SVG_FILL_RULE_H

typedef enum _Egueb_Svg_Fill_Rule
{
	ESVG_NON_ZERO,
	ESVG_EVEN_ODD
} Egueb_Svg_Fill_Rule;

typedef struct _Egueb_Svg_Fill_Rule_Animated
{
	Egueb_Svg_Fill_Rule base;
	Egueb_Svg_Fill_Rule anim;
} Egueb_Svg_Fill_Rule_Animated;

EAPI Eina_Bool egueb_svg_parser_fill_rule_string_from(Egueb_Svg_Fill_Rule *rule, const char *attr);

#endif
