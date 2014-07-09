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
#include "egueb_css_length.h"
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_css_length_string_from(Egueb_Css_Length *thiz,
		const char *str)
{
	const char *last;
	char *endptr;
	double val;

	last = str + strlen(str);
	if (!egueb_dom_double_get(str, &endptr, &val))
		return EINA_FALSE;
	
	/* else, conversion has been done */
	if ((endptr == NULL) || (*endptr == '\0'))
	{
		thiz->value = val;
		thiz->unit = EGUEB_CSS_LENGTH_UNIT_PX;
		return EINA_TRUE;
	}

	if ((last - endptr) != 2)
		return EINA_FALSE;

	/* strlen(endptr) >= 1 */
	if (endptr[0] == 'e')
	{
		if (endptr[1] == 'm')
		{
			thiz->value = val;
			thiz->unit = EGUEB_CSS_LENGTH_UNIT_EM;
		}
		else if (endptr[1] == 'x')
		{
			thiz->value = val;
			thiz->unit = EGUEB_CSS_LENGTH_UNIT_EX;
		}
	}
	else if (endptr[0] == 'p')
	{
		if (endptr[1] == 'c')
		{
			thiz->value = val;
			thiz->unit = EGUEB_CSS_LENGTH_UNIT_PC;
		}
		else if (endptr[1] == 't')
		{
			thiz->value = val;
			thiz->unit = EGUEB_CSS_LENGTH_UNIT_PT;
		}
		else if (endptr[1] == 'x')
		{
			thiz->value = val;
			thiz->unit = EGUEB_CSS_LENGTH_UNIT_PX;
		}
	}
	else if ((endptr[0] == 'c') && (endptr[1] == 'm'))
	{
		thiz->value = val;
		thiz->unit = EGUEB_CSS_LENGTH_UNIT_CM;
	}
	else if ((endptr[0] == 'm') && (endptr[1] == 'm'))
	{
		thiz->value = val;
		thiz->unit = EGUEB_CSS_LENGTH_UNIT_MM;
	}
	else if ((endptr[0] == 'i') && (endptr[1] == 'n'))
	{
		thiz->value = val;
		thiz->unit = EGUEB_CSS_LENGTH_UNIT_IN;
	}
	else
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

EAPI const char * egueb_css_length_string_to(Egueb_Css_Length thiz)
{
	return NULL;
}
