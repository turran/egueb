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
#include "egueb_svg_point_list_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_svg_point_list_cb(const char *attr, void *data)
{
	Egueb_Svg_Point *pt;
	Egueb_Dom_List *l = data;

	pt = calloc(1, sizeof(Egueb_Svg_Point));
	if (!egueb_svg_point_string_from(pt, attr))
	{
		free(pt);
		return;
	}
	egueb_dom_list_item_append(l, pt);
	return;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_point_list_string_from(Egueb_Dom_List *l, const char *str)
{
	egueb_dom_list_clear(l);
	return egueb_dom_list_get(str, " ", _egueb_svg_point_list_cb, l);
}

char * egueb_svg_point_list_string_to(Egueb_Dom_List *l)
{
	ERR("Not supported");
	return NULL;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

