/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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

#include <stdlib.h>
#include <errno.h>

#include "Esvg.h"
#include "esvg_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
double esvg_length_final_get(const Esvg_Length *l, double parent_length)
{
	double ret;
	/* Here we should transform the length/coord to an absolute
	 * value. For some cases we'll need the dpi, should it be global of
	 * the lib? per svg instance?
	 */
	/* FIXME make this numbers preprocessor macros */
	switch (l->unit)
	{
		case ESVG_UNIT_LENGTH_PERCENT:
		ret = (parent_length * l->value) / 100.0;
		break;

		case ESVG_UNIT_LENGTH_PT:
		ret = 1.25 * l->value;
		break;

		case ESVG_UNIT_LENGTH_PC:
		ret = 15 * l->value;
		break;

		case ESVG_UNIT_LENGTH_CM:
		ret = 35.43307 * l->value;
		break;

		case ESVG_UNIT_LENGTH_MM:
		ret = 3.543307 * l->value;
		break;

		case ESVG_UNIT_LENGTH_IN:
		ret = 90 * l->value;
		break;

		case ESVG_UNIT_LENGTH_PX:
		/* FIXME this one depends on the font size */
		case ESVG_UNIT_LENGTH_EM:
		case ESVG_UNIT_LENGTH_EX:
		default:
		ret = l->value;
		break;
	}

	return ret;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

#if 0
double
esvg_length_get(const char *attr_val, double default_length)
{
	Esvg_Unit unit;
	double res;
	char *endptr;

	if (!attr_val || !*attr_val) return default_length;

	res = strtod(attr_val, &endptr);
	if (errno == ERANGE)
		return default_length;
	if ((res == 0) && (attr_val == endptr))
		return default_length;

	/* else, conversion has been done */
	if ((endptr == NULL) || (*endptr == '\0'))
		return res;

	/* strlen(endptr) >= 1 */
	if (endptr[1] == '\0')
	{
		if(endptr[0] == '%')
		{
			unit = ESVG_UNIT_LENGTH_PERCENT;
		}
		else
			return default_length;
	}
	else if (endptr[2] == '\0')
	{
		if (endptr[0] == 'e')
		{
			if (endptr[1] == 'm')
			{
				unit = ESVG_UNIT_LENGTH_EM;
			}
			else if (endptr[1] == 'x')
			{
				unit = ESVG_UNIT_LENGTH_EX;
			}
			else
				return default_length;
		}
		else if (endptr[0] == 'p')
		{
			if (endptr[1] == 'c')
			{
				return _esvg_dpi * res / 6.0;
			}
			else if (endptr[1] == 't')
			{
				return _esvg_dpi * res / 72.0;
			}
			else if (endptr[1] == 'x')
			{
				return res;
			}
			else
				return default_length;
		}
		else if ((endptr[0] == 'c') && (endptr[1] == 'm'))
		{
			return _esvg_dpi * res / 2.54;
		}
		else if ((endptr[0] == 'm') && (endptr[1] == 'm'))
		{
			return _esvg_dpi * res / 25.4;
		}
		else if ((endptr[0] == 'i') && (endptr[1] == 'n'))
		{
			return _esvg_dpi * res;
		}
		else
			return default_length;
	}
	else
		return default_length;
}
#endif
