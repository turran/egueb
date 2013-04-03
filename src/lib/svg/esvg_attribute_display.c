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
#include "esvg_main_private.h"
#include "esvg_types.h"
#include "esvg_attribute_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _esvg_attribute_display_value_get(const char *attr, void **value)
{
	Esvg_Display *v = *value;

	esvg_display_string_from(v, attr);
	return EINA_TRUE;
}

/* The different animated desriptors */
Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_display_descriptor = {
	/* .value_new 			= */ esvg_attribute_enum_value_new,
	/* .value_get 			= */ _esvg_attribute_display_value_get,
	/* .value_free 			= */ free,
	/* .destination_new 		= */ esvg_attribute_enum_destination_new,
	/* .destination_free 		= */ esvg_attribute_enum_destination_free,
	/* .destination_keep 		= */ NULL,
	/* .destination_value_from 	= */ NULL,
	/* .destination_value_to 	= */ esvg_attribute_enum_destination_value_to,
	/* .interpolate 		= */ esvg_attribute_enum_interpolate,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

