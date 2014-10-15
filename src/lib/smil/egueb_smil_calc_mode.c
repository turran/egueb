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
#include "egueb_smil_calc_mode.h"

#include "egueb_smil_calc_mode_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
char * egueb_smil_calc_mode_string_to(Egueb_Smil_Calc_Mode thiz)
{
	switch (thiz)
	{
		case EGUEB_SMIL_CALC_MODE_DISCRETE:
		return strdup("discrete");

		case EGUEB_SMIL_CALC_MODE_LINEAR:
		return strdup("linear");

		case EGUEB_SMIL_CALC_MODE_PACED:
		return strdup("paced");

		case EGUEB_SMIL_CALC_MODE_SPLINE:
		return strdup("spline");

		default:
		return NULL;
	}
}

Eina_Bool egueb_smil_calc_mode_string_from(Egueb_Smil_Calc_Mode *thiz, const char *str)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(str, "discrete"))
		*thiz = EGUEB_SMIL_CALC_MODE_DISCRETE;
	else if (!strcmp(str, "linear"))
		*thiz = EGUEB_SMIL_CALC_MODE_LINEAR;
	else if (!strcmp(str, "paced"))
		*thiz = EGUEB_SMIL_CALC_MODE_PACED;
	else if (!strcmp(str, "spline"))
		*thiz = EGUEB_SMIL_CALC_MODE_SPLINE;
	else
		ret = EINA_FALSE;
	return ret;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
