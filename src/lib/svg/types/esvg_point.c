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
#define ESVG_LOG_DEFAULT _egueb_svg_point_log

static int _egueb_svg_point_log = -1;

static Ender_Property *ESVG_POINT_X;
static Ender_Property *ESVG_POINT_Y;

#define _egueb_svg_point_new NULL
#define _egueb_svg_point_delete NULL
#define _egueb_svg_point_x_set NULL
#define _egueb_svg_point_x_get NULL
#define _egueb_svg_point_x_is_set NULL
#define _egueb_svg_point_y_set NULL
#define _egueb_svg_point_y_get NULL
#define _egueb_svg_point_y_is_set NULL
#include "egueb_svg_generated_point.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_point_init(void)
{
	_egueb_svg_point_log = eina_log_domain_register("egueb_svg_point", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_point_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_point_init();
}

void egueb_svg_point_shutdown(void)
{
	_egueb_svg_point_shutdown();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_point_string_from(Egueb_Svg_Point *thiz, const char *attr)
{
	const char *tmp;
	char *endptr;

	ESVG_SPACE_SKIP(attr);
	tmp = attr;

	thiz->x = eina_strtod(tmp, &endptr);
	tmp = endptr;
	ESVG_SPACE_COMMA_SKIP(tmp);
	thiz->y = eina_strtod(tmp, &endptr);
	tmp = endptr;
	ESVG_SPACE_COMMA_SKIP(tmp);

	return EINA_TRUE;
}
