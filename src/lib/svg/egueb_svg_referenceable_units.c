/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include "egueb_svg_referenceable_units.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_VALUE_ENUM_BOLIERPLATE(egueb_svg_referenceable_units, Egueb_Svg_Referenceable_Units);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI const Egueb_Dom_Value_Descriptor * egueb_svg_referenceable_units_descriptor_get(void)
{
	return &_egueb_svg_referenceable_units_descriptor;
}

EAPI Eina_Bool egueb_svg_referenceable_units_string_from(Egueb_Svg_Referenceable_Units *thiz, const char *value)
{
	if (!strcmp(value, "userSpaceOnUse"))
		*thiz = EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE;
	else if (!strcmp(value, "objectBoundingBox"))
		*thiz = EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI char * egueb_svg_referenceable_units_string_to(Egueb_Svg_Referenceable_Units thiz)
{
	switch (thiz)
	{
		case EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE:
		return strdup("userSpaceOnUse");

		case EGUEB_SVG_REFERENCEABLE_UNITS_OBJECT_BOUNDING_BOX:
		return strdup("objectBoundingBox");

		default:
		return NULL;
	}
}
