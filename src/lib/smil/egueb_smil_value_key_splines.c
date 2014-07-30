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
#include "egueb_smil_value_key_spline_private.h"
#include "egueb_smil_value_key_splines_private.h"
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_smil_value_key_splines_cb(const char *attr, void *data)
{
	Egueb_Smil_Key_Spline spline;
	Egueb_Dom_List *l = data;

	if (egueb_smil_key_spline_string_from(&spline, attr))
	{
		Egueb_Smil_Key_Spline *pspline;
		pspline = malloc(sizeof(Egueb_Smil_Key_Spline));
		*pspline = spline;
		egueb_dom_list_item_append(l, pspline);
	}
}

static Eina_Bool egueb_smil_value_key_splines_string_from(Egueb_Dom_List *l, const char *str)
{
	return egueb_dom_list_get(str, ';', _egueb_smil_value_key_splines_cb, l);
}

static char * egueb_smil_value_key_splines_string_to(Egueb_Dom_List *l)
{
	return NULL;
}
/*----------------------------------------------------------------------------*
 *                             Value interface                                *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_VALUE_LIST_BOILERPLATE(egueb_smil_value_key_splines,
		egueb_smil_value_key_spline_descriptor_get());
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
const Egueb_Dom_Value_Descriptor * egueb_smil_value_key_splines_descriptor_get(void)
{
	return &_egueb_smil_value_key_splines_descriptor;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
