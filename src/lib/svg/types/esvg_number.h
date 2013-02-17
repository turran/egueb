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
#ifndef _ESVG_NUMBER_H
#define _ESVG_NUMBER_H

typedef struct _Esvg_Number
{
	double value;
} Esvg_Number;

typedef struct _Esvg_Number_Animated
{
	Esvg_Number base;
	Esvg_Number anim;
} Esvg_Number_Animated;

EAPI Eina_Bool esvg_number_string_from(Esvg_Number *v, const char *attr_val, double default_nbr);

#endif

