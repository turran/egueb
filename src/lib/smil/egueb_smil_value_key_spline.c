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
#include "egueb_smil_private.h"
#include "egueb_smil_key_spline_private.h"
#include "egueb_dom_value_private.h"
#include "egueb_dom_value_primitive_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool egueb_smil_value_key_spline_string_from(
		Egueb_Smil_Key_Value *value, const char *str)
{

}

static char * egueb_smil_value_key_spline_string_to(Egueb_Svg_Paint *value)
{
	return NULL;
}


static void egueb_smil_value_key_spline_interpolate(Egueb_Svg_Paint *v,
		Egueb_Svg_Paint *a, Egueb_Svg_Paint *b, double m,
		Egueb_Svg_Paint *add, Egueb_Svg_Paint *acc, int mul)
{
	ERR("Not implemented");
}

EGUEB_DOM_VALUE_PRIMITIVE_BOILERPLATE(egueb_smil_value_key_spline,
		Egueb_Smil_Key_Value);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
const Egueb_Dom_Value_Descriptor * egueb_smil_value_key_spline_descriptor_get(void)
{
	return &_egueb_smil_value_key_spline_descriptor;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

