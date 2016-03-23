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
#include "egueb_svg_number.h"
#include "egueb_svg_number_private.h"

#include "egueb_svg_number_list_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _egueb_svg_number_list_string_to_cb(void *data, void *user_data)
{
	Egueb_Svg_Number *v = data;
	Eina_Strbuf *strbuf = user_data;
	char *str;

	str = egueb_svg_number_string_to(*v);
	if (eina_strbuf_length_get(strbuf))
	{
		eina_strbuf_append_char(strbuf, ' ');
	}
	if (str)
	{
		eina_strbuf_append(strbuf, str);
		free(str);
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool egueb_svg_number_list_string_from(Egueb_Dom_List *l,
		const char *value)
{
	EGUEB_DOM_SPACE_COMMA_SKIP(value);
	while (value)
	{
		Egueb_Svg_Number *number;
		Egueb_Svg_Number v;
		char *endptr;

		if (!egueb_dom_double_get(value, &endptr, &v))
		{
			WARN("Failed the parsing");
			break;
		}
		number = calloc(1, sizeof(Egueb_Svg_Number));
		*number = v;
		egueb_dom_list_item_append(l, number);

		value = endptr;
		EGUEB_DOM_SPACE_COMMA_SKIP(value);
	}
	return EINA_TRUE;
}

char * egueb_svg_number_list_string_to(Egueb_Dom_List *l)
{
	Eina_Strbuf *strbuf;
	char *ret;

	strbuf = eina_strbuf_new();
	egueb_dom_list_foreach(l, _egueb_svg_number_list_string_to_cb, strbuf);
	ret = eina_strbuf_string_steal(strbuf);
	eina_strbuf_free(strbuf);
	return ret;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
