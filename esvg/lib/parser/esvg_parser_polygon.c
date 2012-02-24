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
static void _parser_points_cb(Esvg_Point *p, void *data)
{
	Enesim_Renderer *r = data;

	esvg_polygon_point_add(r, p);
}

static Eina_Bool _parser_polygon_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(tag);
	if (strcmp(key, "points") == 0)
	{
		esvg_parser_points(value, _parser_points_cb, r);
	}

	return EINA_TRUE;
}

static const char * _parser_polygon_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_polygon_name_get(Edom_Tag *tag)
{
	return "polygon";
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_polygon_name_get,
	/* .attribute_set 	= */ _parser_polygon_attribute_set,
	/* .attribute_get 	= */ _parser_polygon_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_polygon_new(Edom_Parser *parser)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	r = esvg_polygon_new();
	tag = esvg_parser_shape_new(parser, &_descriptor, ESVG_POLYGON, r, NULL);

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

