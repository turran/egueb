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
#ifndef _EGUEB_SVG_LENGTH_H
#define _EGUEB_SVG_LENGTH_H

/* FIXME implement the unit type as the spec or just
 * ABSOLUTE/RELATIVE?
 */
typedef enum _Egueb_Svg_Unit_Length
{
	EGUEB_SVG_UNIT_LENGTH_EM,
	EGUEB_SVG_UNIT_LENGTH_EX,
	EGUEB_SVG_UNIT_LENGTH_PX,
	EGUEB_SVG_UNIT_LENGTH_PT,
	EGUEB_SVG_UNIT_LENGTH_PC,
	EGUEB_SVG_UNIT_LENGTH_CM,
	EGUEB_SVG_UNIT_LENGTH_MM,
	EGUEB_SVG_UNIT_LENGTH_IN,
	EGUEB_SVG_UNIT_LENGTH_PERCENT,
	EGUEB_SVG_UNIT_LENGTHS
} Egueb_Svg_Unit_Length;

typedef struct _Egueb_Svg_Length
{
	double value;
	Egueb_Svg_Unit_Length unit;
} Egueb_Svg_Length;

typedef Egueb_Svg_Length Egueb_Svg_Coord;

typedef struct _Egueb_Svg_Length_Animated
{
	Egueb_Svg_Length base;
	Egueb_Svg_Length anim;
} Egueb_Svg_Length_Animated;

typedef Egueb_Svg_Length_Animated Egueb_Svg_Coord_Animated;

EAPI extern const Egueb_Svg_Length EGUEB_SVG_LENGTH_0;
EAPI extern const Egueb_Svg_Length EGUEB_SVG_LENGTH_1;
EAPI extern const Egueb_Svg_Length EGUEB_SVG_LENGTH_100_PERCENT;
EAPI extern const Egueb_Svg_Length EGUEB_SVG_LENGTH_50_PERCENT;

static inline Eina_Bool egueb_svg_length_is_relative(Egueb_Svg_Length *length)
{
	if (length->unit == EGUEB_SVG_UNIT_LENGTH_PERCENT || length->unit == EGUEB_SVG_UNIT_LENGTH_EM)
		return EINA_TRUE;
	return EINA_FALSE;
}

static inline void egueb_svg_length_set(Egueb_Svg_Length *length, double value, Egueb_Svg_Unit_Length unit)
{
	length->value = value;
	length->unit = unit;
}

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_length_descriptor_get(void);
EAPI Eina_Bool egueb_svg_length_string_from(Egueb_Svg_Length *length, const char *attr_val);
EAPI char * egueb_svg_length_string_to(Egueb_Svg_Length *thiz);
EAPI Eina_Bool egueb_svg_length_is_equal(Egueb_Svg_Length *length1, Egueb_Svg_Length *length2);
EAPI double egueb_svg_coord_final_get(const Egueb_Svg_Length *l, double parent_length, double font_size);
EAPI double egueb_svg_length_final_get(const Egueb_Svg_Length *l, double width, double height, double font_size);

#endif
