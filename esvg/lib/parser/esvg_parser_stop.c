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
#include "Esvg_Parser.h"
#include "esvg_parser_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _parser_stop_attribute_set(Edom_Tag *tag, const char *key,
		const char *value)
{
	Esvg_Gradient_Stop *s;

	s = edom_tag_data_get(tag);
	if (strcmp(key, "id") == 0)
	{
		/* nothing to do here yet */
	}
	else if (strcmp(key, "offset") == 0)
	{
		Esvg_Length offset;

		esvg_length_string_from(&offset, value, ESVG_LENGTH_0);
		printf("offset %g\n", offset.value);
		s->offset = offset;
	}
	else if (strcmp(key, "stop-color") == 0)
	{
		Esvg_Color color;

		esvg_color_get(&color, value);
		s->stop_color = color;
	}
	else if (strcmp(key, "stop-opacity") == 0)
	{
		double stop_opacity = esvg_number_string_from(value, 1.0);
		s->stop_opacity = stop_opacity;
	}
	else if (strcmp(key, "style") == 0)
	{
		Esvg_Attribute_Presentation attr;

		memset(&attr, 0, sizeof(Esvg_Attribute_Presentation));
		esvg_parser_style_inline_set(value, tag, &attr);

		printf("style = %s\n", value);

		if (attr.stop_opacity_set)
			s->stop_opacity = attr.stop_opacity;
		
		if (attr.stop_color_set)
			s->stop_color = attr.stop_color;
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
Edom_Tag * esvg_parser_stop_new(Edom_Parser *parser)
{
	Esvg_Gradient_Stop *s;
	Edom_Tag *tag;

	s = calloc(1, sizeof(Esvg_Gradient_Stop));
	/* default values */
	s->stop_opacity = 1.0;
	tag = edom_tag_new(parser, &_descriptor, ESVG_STOP, s);

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

