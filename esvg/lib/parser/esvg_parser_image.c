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
static Eina_Bool _parser_image_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(tag);
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value, ESVG_COORD_0);
		esvg_image_x_set(r, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value, ESVG_COORD_0);
		esvg_image_y_set(r, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value, ESVG_LENGTH_0);
		esvg_image_width_set(r, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value, ESVG_LENGTH_0);
		esvg_image_height_set(r, &height);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
		/* absolute */
		if (*value == '/')
		{
			esvg_image_href_set(r, value);
		}
		/* relative */
		else
		{
			Edom_Parser *parser;
			char real[PATH_MAX];
			const char *root;

			parser = edom_tag_parser_get(tag);
			root = edom_parser_root_get(parser);
			strcpy(real, root);
			strcat(real, value);
			esvg_image_href_set(r, real);
		}
	}

	return EINA_TRUE;
}

static const char * _parser_image_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_image_name_get(Edom_Tag *tag)
{
	return "image";
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_image_name_get,
	/* .attribute_set 	= */ _parser_image_attribute_set,
	/* .attribute_get 	= */ _parser_image_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_image_new(Edom_Parser *parser)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	r = esvg_image_new();
	tag = esvg_parser_element_new(parser, &_descriptor, ESVG_IMAGE, r, NULL);

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
