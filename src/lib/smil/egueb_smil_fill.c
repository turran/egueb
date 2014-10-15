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
#include "egueb_smil_fill.h"

#include "egueb_smil_fill_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_smil_fill_string_from(Egueb_Smil_Fill *fill, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "freeze"))
		*fill = EGUEB_SMIL_FILL_FREEZE;
	else if (!strcmp(attr, "remove"))
		*fill = EGUEB_SMIL_FILL_REMOVE;
	else
		ret = EINA_FALSE;
	return ret;
}

char * egueb_smil_fill_string_to(Egueb_Smil_Fill fill)
{
	switch (fill)
	{
		case EGUEB_SMIL_FILL_FREEZE:
		return strdup("freeze");

		case EGUEB_SMIL_FILL_REMOVE:
		return strdup("remove");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

