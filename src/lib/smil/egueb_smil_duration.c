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
#include "egueb_smil_duration.h"

#include "egueb_smil_clock_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_smil_duration_string_from(Egueb_Smil_Duration *d, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "indefinite"))
		d->type = EGUEB_SMIL_DURATION_TYPE_INDEFINITE;
	else if (!strcmp(attr, "media"))
		d->type = EGUEB_SMIL_DURATION_TYPE_MEDIA;
	else
	{
		ret = egueb_smil_clock_string_from(&d->data.clock, attr);
		if (!ret)
		{
			ERR("Impossible to parse the duration '%s'", attr);
			return ret;
		}
		d->type = EGUEB_SMIL_DURATION_TYPE_CLOCK;
		DBG("duration is %" EGUEB_SMIL_CLOCK_FORMAT, EGUEB_SMIL_CLOCK_ARGS(d->data.clock));
	}
	return ret;
}

char * egueb_smil_duration_string_to(Egueb_Smil_Duration *d)
{
	ERR("Not supported");
	return NULL;
}

void egueb_smil_duration_interpolate(Egueb_Smil_Duration *v,
		Egueb_Smil_Duration *a, Egueb_Smil_Duration *b, double m,
		Egueb_Smil_Duration *add, Egueb_Smil_Duration *acc, int mul)
{
	ERR("Not implemented");
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
const Egueb_Smil_Duration EGUEB_SMIL_DURATION_INDEFINITE = { EGUEB_SMIL_DURATION_TYPE_INDEFINITE, { 0 }};
