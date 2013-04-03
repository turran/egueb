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
#define ESVG_LOG_DEFAULT _esvg_point_log

static int _esvg_point_log = -1;

static Ender_Property *ESVG_POINT_X;
static Ender_Property *ESVG_POINT_Y;

#define _esvg_point_new NULL
#define _esvg_point_delete NULL
#define _esvg_point_x_set NULL
#define _esvg_point_x_get NULL
#define _esvg_point_x_is_set NULL
#define _esvg_point_y_set NULL
#define _esvg_point_y_get NULL
#define _esvg_point_y_is_set NULL
#include "esvg_generated_point.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_point_init(void)
{
	_esvg_point_log = eina_log_domain_register("esvg_point", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_point_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_point_init();
}

void esvg_point_shutdown(void)
{
	_esvg_point_shutdown();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_point_string_from(Esvg_Point *thiz, const char *attr)
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
