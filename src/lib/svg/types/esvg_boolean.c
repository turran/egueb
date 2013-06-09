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
#include "egueb_svg_types_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _egueb_svg_boolean_log

static int _egueb_svg_boolean_log = -1;

static Ender_Property *ESVG_BOOLEAN_ANIMATED_BASE;
static Ender_Property *ESVG_BOOLEAN_ANIMATED_ANIM;

#define _egueb_svg_boolean_animated_new NULL
#define _egueb_svg_boolean_animated_delete NULL
#define _egueb_svg_boolean_animated_base_set NULL
#define _egueb_svg_boolean_animated_base_get NULL
#define _egueb_svg_boolean_animated_base_is_set NULL
#define _egueb_svg_boolean_animated_anim_set NULL
#define _egueb_svg_boolean_animated_anim_get NULL
#define _egueb_svg_boolean_animated_anim_is_set NULL
#include "egueb_svg_generated_boolean_animated.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_boolean_init(void)
{
	_egueb_svg_boolean_log = eina_log_domain_register("egueb_svg_boolean", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_boolean_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_boolean_animated_init();
}

void egueb_svg_boolean_shutdown(void)
{
	_egueb_svg_boolean_animated_shutdown();
}


