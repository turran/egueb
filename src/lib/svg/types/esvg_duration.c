/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
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
#include "esvg_private_main.h"
#include "esvg_types.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_duration_log

static int _esvg_duration_log = -1;

static Ender_Property *ESVG_DURATION_TYPE;
static Ender_Property *ESVG_DURATION_COLOR;
static Ender_Property *ESVG_DURATION_URI;

static Ender_Property *ESVG_DURATION_ANIMATED_BASE;
static Ender_Property *ESVG_DURATION_ANIMATED_ANIM;

#define _esvg_duration_new NULL
#define _esvg_duration_delete NULL
#define _esvg_duration_type_set NULL
#define _esvg_duration_type_get NULL
#define _esvg_duration_type_is_set NULL
#define _esvg_duration_clock_set NULL
#define _esvg_duration_clock_get NULL
#define _esvg_duration_clock_is_set NULL
#include "esvg_generated_duration.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_duration_init(void)
{
	_esvg_duration_log = eina_log_domain_register("esvg_duration", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_duration_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_duration_init();
}

void esvg_duration_shutdown(void)
{
	_esvg_duration_shutdown();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_duration_string_from(Esvg_Duration *d, const char *attr)
{
	Eina_Bool ret = EINA_TRUE;

	if (!strcmp(attr, "indefinite"))
		d->type = ESVG_DURATION_TYPE_INDEFINITE;
	else if (!strcmp(attr, "media"))
		d->type = ESVG_DURATION_TYPE_MEDIA;
	else
	{
		ret = esvg_clock_string_from(&d->data.clock, attr);
		if (!ret)
		{
			ERR("Impossible to parse the duration '%s'", attr);
			return ret;
		}
		d->type = ESVG_DURATION_TYPE_CLOCK;
		DBG("duration is %" ETCH_TIME_FORMAT, ETCH_TIME_ARGS(d->data.clock));
	}
	return ret;
}


