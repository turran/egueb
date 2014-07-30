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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_smil_key_spline_string_from(
		Egueb_Smil_Key_Spline *value, const char *str)
{
	double vals[4];
	const char *tmp;
	char *endptr;
	int i;

	if (!str) return EINA_FALSE;
	EGUEB_DOM_SPACE_SKIP(str);
	if (!str) return EINA_FALSE;
	tmp = str;

	for (i = 0; i < 4; i++)
	{
		if (!tmp)
			break;
		if (!egueb_dom_double_get(tmp, &endptr, &vals[i]))
			break;

		tmp = endptr;
		EGUEB_DOM_SPACE_COMMA_SKIP(tmp);
	}

	if (i == 4)
	{
		value->x = vals[0];
		value->y = vals[1];
		value->cx = vals[2];
		value->cy = vals[3];
		return EINA_TRUE;
	}
	else
	{
		return EINA_FALSE;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
