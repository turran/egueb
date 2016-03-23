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
#include "egueb_svg_main_private.h"
#include "egueb_svg_length.h"

#include "egueb_dom_value_private.h"
#include "egueb_svg_length_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_svg_value_length_list_cb(const char *attr, void *data)
{
	Egueb_Svg_Length length;
	Egueb_Dom_List *l = data;

	if (egueb_svg_length_string_from(&length, attr))
	{
		Egueb_Svg_Length *plength;

		plength = malloc(sizeof(Egueb_Svg_Length));
		*plength = length;
		egueb_dom_list_item_append(l, plength);
	}
}

static Eina_Bool egueb_svg_value_length_list_string_from(Egueb_Dom_List *l, const char *str)
{
	return egueb_dom_list_get(str, ';', _egueb_svg_value_length_list_cb, l);
}

static char * egueb_svg_value_length_list_string_to(Egueb_Dom_List *l)
{
	return NULL;
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_VALUE_LIST_BOILERPLATE(egueb_svg_value_length_list,
		egueb_svg_length_descriptor_get());
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
const Egueb_Dom_Value_Descriptor * egueb_svg_value_length_list_descriptor_get(void)
{
	return &_egueb_svg_value_length_list_descriptor;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
