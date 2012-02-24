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
static Eina_Bool _parser_path_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(tag);
	if (strcmp(key, "d") == 0)
	{
		if (!esvg_parser_path(value, r))
			return EINA_FALSE;
	}

	return EINA_TRUE;
}

static const char * _parser_path_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_path_name_get(Edom_Tag *tag)
{
	return "path";
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_path_name_get,
	/* .attribute_set 	= */ _parser_path_attribute_set,
	/* .attribute_get 	= */ _parser_path_attribute_get,
};

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_path_new(Edom_Parser *parser)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	r = esvg_path_new();
	tag = esvg_parser_shape_new(parser, &_descriptor, ESVG_PATH, r, NULL);

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

