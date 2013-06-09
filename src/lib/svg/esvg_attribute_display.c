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
#include "egueb_svg_main_private.h"
#include "egueb_svg_types.h"
#include "egueb_svg_attribute_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _egueb_svg_attribute_display_value_get(const char *attr, void **value)
{
	Egueb_Svg_Display *v = *value;

	egueb_svg_display_string_from(v, attr);
	return EINA_TRUE;
}

/* The different animated desriptors */
Egueb_Svg_Attribute_Animated_Descriptor egueb_svg_attribute_animated_display_descriptor = {
	/* .value_new 			= */ egueb_svg_attribute_enum_value_new,
	/* .value_get 			= */ _egueb_svg_attribute_display_value_get,
	/* .value_free 			= */ free,
	/* .destination_new 		= */ egueb_svg_attribute_enum_destination_new,
	/* .destination_free 		= */ egueb_svg_attribute_enum_destination_free,
	/* .destination_keep 		= */ NULL,
	/* .destination_value_from 	= */ NULL,
	/* .destination_value_to 	= */ egueb_svg_attribute_enum_destination_value_to,
	/* .interpolate 		= */ egueb_svg_attribute_enum_interpolate,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

