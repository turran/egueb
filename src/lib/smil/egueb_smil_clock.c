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
#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/* The clock is defined in miliseconds? nanoseconds? */
/* TODO maybe we should use doubles directly? */
/* Clock-val         ::= Full-clock-val | Partial-clock-val | Timecount-val */
EAPI Eina_Bool egueb_smil_clock_string_from(int64_t *clock, const char *attr)
{
	Eina_Bool ret = EINA_FALSE;
	long v;
	const char *tmp;

	if (!egueb_dom_long_get(attr, &tmp, &v))
		return EINA_FALSE;

	if (*tmp == ':')
	{
		long v2;

		tmp++;
		if (!egueb_dom_long_get(tmp, &tmp, &v2))
			return EINA_FALSE;

		/* Full-clock-val::= Hours ":" Minutes ":" Seconds ("." Fraction)? */
		if (*tmp == ':')
		{

		}
		/* Partial-clock-val::= Minutes ":" Seconds ("." Fraction)? */
		else
		{
			long m = v;
			long s = v2;

			*clock = (m * 60) + (s);
			/* Fraction::= DIGIT+ */
			if (*tmp == '.')
			{
				long f;

				tmp++;
				if (!egueb_dom_long_get(tmp, &tmp, &f))
					return EINA_FALSE;
			}
			ret = EINA_TRUE;

		}
	}
	/* Timecount-val::= Timecount ("." Fraction)? (Metric)? */
	else
	{
		int64_t scale;
		long f = 0;

		/* Fraction::= DIGIT+ */
		if (*tmp == '.')
		{
			tmp++;
			if (!egueb_dom_long_get(tmp, &tmp, &f))
				return EINA_FALSE;
		}

		/* Metric::= "h" | "min" | "s" | "ms" */
		if (!strncmp(tmp, "ms", 2))
			scale = EGUEB_SMIL_CLOCK_MSECONDS;
		else if (*tmp == 's')
			scale = EGUEB_SMIL_CLOCK_SECONDS;
		else if (*tmp == 'm')
			scale = EGUEB_SMIL_CLOCK_SECONDS * 60;
		else if (*tmp == 'h')
			scale = EGUEB_SMIL_CLOCK_SECONDS * 60 * 60;
		else if (!*tmp)
			scale = EGUEB_SMIL_CLOCK_SECONDS;
		else
			return EINA_FALSE;

		*clock = v * scale;
		if (f)
			*clock += (double)(f / 10.0) * scale;

		DBG("clock %" ETCH_TIME_FORMAT " parsed from attr '%s'", ETCH_TIME_ARGS(*clock), attr);
		ret = EINA_TRUE;
	}

	return ret;
}


