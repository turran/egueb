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
#ifndef _EGUEB_SVG_REFERENCEABLE_UNITS_H
#define _EGUEB_SVG_REFERENCEABLE_UNITS_H

typedef enum _Egueb_Svg_Referenceable_Units
{
	EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE,
	EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX,
} Egueb_Svg_Referenceable_Units;

typedef struct _Egueb_Svg_Referencable_Units_Animated
{
	Egueb_Svg_Referenceable_Units base;
	Egueb_Svg_Referenceable_Units anim;
} Egueb_Svg_Referenceable_Units_Animated;

EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_referenceable_units_descriptor_get(void);
EAPI Eina_Bool egueb_svg_referenceable_units_string_from(Egueb_Svg_Referenceable_Units *thiz, const char *value);
EAPI char * egueb_svg_referenceable_units_string_to(Egueb_Svg_Referenceable_Units thiz);

#endif

