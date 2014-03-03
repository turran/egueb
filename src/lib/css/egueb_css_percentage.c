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
#include "egueb_css_private.h"
#include "egueb_css_percentage.h"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_css_percentage_string_from(double *thiz,
		const char *str)
{
	const char *last;
	char *endptr;

	last = str + strlen(str);
	if (!egueb_base_double_get(str, &endptr, thiz))
		return EINA_FALSE;
	
	if ((last - endptr) != 1)
		return EINA_FALSE;

	if (strcmp(endptr, "%"))
		return EINA_FALSE;

	return EINA_TRUE;
}

EAPI const char * egueb_css_percentage_string_to(double thiz)
{
	return NULL;
}

