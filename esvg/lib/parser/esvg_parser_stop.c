/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#include "Esvg.h"
#include "esvg_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _parser_stop_attribute_set(Edom_Tag *tag, const char *key,
		const char *value)
{
	Esvg_Gradient_Stop *s;

	s = edom_tag_data_get(tag);
	if (strcmp(key, "stop-color") == 0)
	{
		Esvg_Color color;

		esvg_color_get(&color, value);
		s->stop_color = color;
	}
	else if (strcmp(key, "offset") == 0)
	{
		Esvg_Length offset;

		esvg_length_get(&offset, value, ESVG_LENGTH_0);
		s->offset = offset;
	}
	else if (strcmp(key, "stop-opacity") == 0)
	{
		double stop_opacity = esvg_number_get(value, 1.0);
		s->stop_opacity = stop_opacity;
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static const char * _parser_stop_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_stop_name_get(Edom_Tag *tag)
{
	return "stop";
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_stop_name_get,
	/* .attribute_set 	= */ _parser_stop_attribute_set,
	/* .attribute_get 	= */ _parser_stop_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_stop_new(Edom_Context *c, Edom_Tag *topmost)
{
	Esvg_Gradient_Stop *s;
	Edom_Tag *tag;

	s = calloc(1, sizeof(Esvg_Gradient_Stop));
	/* default values */
	s->stop_opacity = 1.0;
	tag = edom_tag_new(c, &_descriptor, ESVG_STOP, topmost, s);

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

