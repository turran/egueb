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
#include "egueb_smil_additive.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_smil_additive_string_from(Egueb_Smil_Additive *additive, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "sum"))
		*additive = EGUEB_SMIL_ADDITIVE_SUM;
	else if (!strcmp(attr, "replace"))
		*additive = EGUEB_SMIL_ADDITIVE_REPLACE;
	else
		ret = EINA_FALSE;
	return ret;
}

char * egueb_smil_additive_string_to(Egueb_Smil_Additive additive)
{
	switch (additive)
	{
		case EGUEB_SMIL_ADDITIVE_SUM:
		return strdup("sum");

		case EGUEB_SMIL_ADDITIVE_REPLACE:
		return strdup("replace");

		default:
		return NULL;
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

