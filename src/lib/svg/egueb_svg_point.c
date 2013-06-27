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

#include "egueb_svg_main_private.h"
#include "egueb_svg_point.h"

#include "egueb_dom_value_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_point_string_from(Egueb_Svg_Point *thiz, const char *attr)
{
	const char *tmp;
	char *endptr;

	EGUEB_DOM_SPACE_SKIP(attr);
	tmp = attr;

	thiz->x = eina_extra_strtod(tmp, &endptr);
	tmp = endptr;
	EGUEB_DOM_SPACE_COMMA_SKIP(tmp);
	thiz->y = eina_extra_strtod(tmp, &endptr);
	tmp = endptr;
	EGUEB_DOM_SPACE_COMMA_SKIP(tmp);

	return EINA_TRUE;
}
