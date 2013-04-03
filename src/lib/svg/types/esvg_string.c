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
#include "esvg_main_private.h"

#include "esvg_types.h"
#include "esvg_types_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_string_log

static int _esvg_string_log = -1;

static Ender_Property *ESVG_STRING_ANIMATED_BASE;
static Ender_Property *ESVG_STRING_ANIMATED_ANIM;

#define _esvg_string_animated_new NULL
#define _esvg_string_animated_delete NULL
#define _esvg_string_animated_base_set NULL
#define _esvg_string_animated_base_get NULL
#define _esvg_string_animated_base_is_set NULL
#define _esvg_string_animated_anim_set NULL
#define _esvg_string_animated_anim_get NULL
#define _esvg_string_animated_anim_is_set NULL
#include "esvg_generated_string_animated.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_string_init(void)
{
	_esvg_string_log = eina_log_domain_register("esvg_string", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_string_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_string_animated_init();
}

void esvg_string_shutdown(void)
{
	_esvg_string_animated_shutdown();
}

