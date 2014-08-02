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
#include "egueb_smil_accumulate.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_smil_accumulate_string_from(Egueb_Smil_Accumulate *accumulate, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "sum"))
		*accumulate = EGUEB_SMIL_ACCUMULATE_SUM;
	else if (!strcmp(attr, "none"))
		*accumulate = EGUEB_SMIL_ACCUMULATE_NONE;
	else
		ret = EINA_FALSE;
	return ret;
}

char * egueb_smil_accumulate_string_to(Egueb_Smil_Accumulate accumulate)
{
	switch (accumulate)
	{
		case EGUEB_SMIL_ACCUMULATE_SUM:
		return strdup("sum");

		case EGUEB_SMIL_ACCUMULATE_NONE:
		return strdup("none");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

