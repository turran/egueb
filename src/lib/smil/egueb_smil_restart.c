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
#include "egueb_smil_restart.h"

#include "egueb_smil_restart_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
char * egueb_smil_restart_string_to(Egueb_Smil_Restart thiz)
{
	switch (thiz)
	{
		case EGUEB_SMIL_RESTART_ALWAYS:
		return strdup("always");

		case EGUEB_SMIL_RESTART_WHEN_NOT_ACTIVE:
		return strdup("whenNotActive");

		case EGUEB_SMIL_RESTART_NEVER:
		return strdup("never");

		default:
		return NULL;
	}
}

Eina_Bool egueb_smil_restart_string_from(Egueb_Smil_Restart *thiz, const char *str)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(str, "always"))
		*thiz = EGUEB_SMIL_RESTART_ALWAYS;
	else if (!strcmp(str, "whenNotActive"))
		*thiz = EGUEB_SMIL_RESTART_WHEN_NOT_ACTIVE;
	else if (!strcmp(str, "never"))
		*thiz = EGUEB_SMIL_RESTART_NEVER;
	else
		ret = EINA_FALSE;
	return ret;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

