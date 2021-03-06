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
#include "egueb_smil_clock.h"
#include "egueb_smil_repeat_count.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
char * egueb_smil_repeat_count_string_to(Egueb_Smil_Repeat_Count *thiz)
{
	ERR("Not implemented");
	return NULL;
}

Eina_Bool egueb_smil_repeat_count_string_from(Egueb_Smil_Repeat_Count *thiz, const char *str)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(str, "indefinite"))
		thiz->type = EGUEB_SMIL_REPEAT_COUNT_TYPE_INDEFINITE;
	else
	{
		if (sscanf(str, "%lg", &thiz->value) < 0)
		{
			ERR("Impossible to parse the repeat_count '%s'", str);
			ret = EINA_FALSE;
		}
		thiz->type = EGUEB_SMIL_REPEAT_COUNT_TYPE_FINITE;
	}
	return ret;
}

void egueb_smil_repeat_count_interpolate(Egueb_Smil_Repeat_Count *v,
		Egueb_Smil_Repeat_Count *a, Egueb_Smil_Repeat_Count *b, double m,
		Egueb_Smil_Repeat_Count *add, Egueb_Smil_Repeat_Count *acc, int mul)
{
	ERR("Not implemented");
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Smil_Repeat_Count EGUEB_SMIL_REPEAT_COUNT_INDEFINITE = { EGUEB_SMIL_REPEAT_COUNT_TYPE_INDEFINITE, 0 };
