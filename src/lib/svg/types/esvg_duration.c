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
#include "egueb_svg_main_private.h"
#include "egueb_svg_types.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _egueb_svg_duration_log

static int _egueb_svg_duration_log = -1;

static Ender_Property *ESVG_DURATION_TYPE;
static Ender_Property *ESVG_DURATION_COLOR;
static Ender_Property *ESVG_DURATION_URI;

static Ender_Property *ESVG_DURATION_ANIMATED_BASE;
static Ender_Property *ESVG_DURATION_ANIMATED_ANIM;

#define _egueb_svg_duration_new NULL
#define _egueb_svg_duration_delete NULL
#define _egueb_svg_duration_type_set NULL
#define _egueb_svg_duration_type_get NULL
#define _egueb_svg_duration_type_is_set NULL
#define _egueb_svg_duration_clock_set NULL
#define _egueb_svg_duration_clock_get NULL
#define _egueb_svg_duration_clock_is_set NULL
#include "egueb_svg_generated_duration.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_duration_init(void)
{
	_egueb_svg_duration_log = eina_log_domain_register("egueb_svg_duration", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_duration_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_duration_init();
}

void egueb_svg_duration_shutdown(void)
{
	_egueb_svg_duration_shutdown();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
