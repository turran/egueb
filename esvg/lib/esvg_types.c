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
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI double esvg_length_final_get(const Esvg_Length *l, double parent_length)
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
