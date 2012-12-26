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
#ifndef _ESVG_LENGTH_H
#define _ESVG_LENGTH_H

/* FIXME implement the unit type as the spec or just
 * ABSOLUTE/RELATIVE?
 */
typedef enum _Esvg_Unit_Length
{
	ESVG_UNIT_LENGTH_EM,
	ESVG_UNIT_LENGTH_EX,
	ESVG_UNIT_LENGTH_PX,
	ESVG_UNIT_LENGTH_PT,
	ESVG_UNIT_LENGTH_PC,
	ESVG_UNIT_LENGTH_CM,
	ESVG_UNIT_LENGTH_MM,
	ESVG_UNIT_LENGTH_IN,
	ESVG_UNIT_LENGTH_PERCENT,
	ESVG_UNIT_LENGTHS
} Esvg_Unit_Length;

typedef struct _Esvg_Length
{
	double value;
	Esvg_Unit_Length unit;
} __attribute__((packed)) Esvg_Length;

typedef Esvg_Length Esvg_Coord;

typedef struct _Esvg_Length_Animated
{
	Esvg_Length base;
	Esvg_Length anim;
} Esvg_Length_Animated;

typedef Esvg_Length_Animated Esvg_Animated_Coord;

extern Esvg_Length ESVG_LENGTH_0;
extern Esvg_Length ESVG_LENGTH_1;
extern Esvg_Length ESVG_LENGTH_100_PERCENT;
#define ESVG_COORD_0 ESVG_LENGTH_0

static inline Eina_Bool esvg_length_is_relative(Esvg_Length *length)
{
	if (length->unit == ESVG_UNIT_LENGTH_PERCENT || length->unit == ESVG_UNIT_LENGTH_EM)
		return EINA_TRUE;
	return EINA_FALSE;
}

static inline void esvg_length_set(Esvg_Length *length, double value, Esvg_Unit_Length unit)
{
	length->value = value;
	length->unit = unit;
}

EAPI Eina_Bool esvg_length_string_from(Esvg_Length *length, const char *attr_val);
EAPI Eina_Bool esvg_length_is_equal(Esvg_Length *length1, Esvg_Length *length2);
EAPI double esvg_coord_final_get(const Esvg_Length *l, double parent_length, double font_size);
EAPI double esvg_length_final_get(const Esvg_Length *l, double width, double height, double font_size);

#endif
