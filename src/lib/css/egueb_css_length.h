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
#ifndef _EGUEB_CSS_LENGTH_H
#define _EGUEB_CSS_LENGTH_H

typedef enum _Egueb_Css_Length_Unit
{
	EGUEB_CSS_LENGTH_UNIT_EM,
	EGUEB_CSS_LENGTH_UNIT_EX,
	EGUEB_CSS_LENGTH_UNIT_PX,
	EGUEB_CSS_LENGTH_UNIT_PT,
	EGUEB_CSS_LENGTH_UNIT_PC,
	EGUEB_CSS_LENGTH_UNIT_CM,
	EGUEB_CSS_LENGTH_UNIT_MM,
	EGUEB_CSS_LENGTH_UNIT_IN,
	EGUEB_CSS_LENGTH_UNITS
} Egueb_Css_Length_Unit;

typedef struct _Egueb_Css_Length
{
	double value;
	Egueb_Css_Length_Unit unit;
} Egueb_Css_Length;

EAPI Eina_Bool egueb_css_length_string_from(Egueb_Css_Length *thiz,
		const char *str);
EAPI const char * egueb_css_length_string_to(Egueb_Css_Length thiz);

#endif

