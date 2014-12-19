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

#include "egueb_svg_point_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_point_interpolate(Egueb_Svg_Point *v,
		Egueb_Svg_Point *a, Egueb_Svg_Point *b, double m,
		Egueb_Svg_Point *add, Egueb_Svg_Point *acc, int mul)
{
	egueb_dom_value_interpolate_double(a->x, b->x, m, &v->x);
	egueb_dom_value_interpolate_double(a->y, b->y, m, &v->y);
}

Eina_Bool egueb_svg_point_string_from(Egueb_Svg_Point *thiz, const char *attr)
{
	const char *tmp;
	char *endptr;

	if (!attr) return EINA_FALSE;
	EGUEB_DOM_SPACE_SKIP(attr);
	if (!attr) return EINA_FALSE;
	tmp = attr;

	if (!egueb_dom_double_get(tmp, &endptr, &thiz->x))
		return EINA_FALSE;

	tmp = endptr;
	EGUEB_DOM_SPACE_COMMA_SKIP(tmp);
	if (!tmp) return EINA_FALSE;

	if (!egueb_dom_double_get(tmp, &endptr, &thiz->y))
		return EINA_FALSE;

	return EINA_TRUE;
}

char * egueb_svg_point_string_to(Egueb_Svg_Point *thiz)
{
	ERR("Not implemented");
	return NULL;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

