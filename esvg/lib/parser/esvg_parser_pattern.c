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
/*
 * The pattern parser might be kind of complicated given that we can parse
 * the same as the svg i think, but everything must be rendered to an image
 * and then use it as the source of the pattern. Dunno what's best to do
 * internally, given that the paint server *must* be public
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Pattern
{
	Edom_Tag *tag;
	Enesim_Renderer *r;
	char *href;
} Esvg_Parser_Pattern;

static Esvg_Parser_Pattern * _esvg_parser_pattern_get(Edom_Tag *tag)
{
	Esvg_Parser_Pattern *thiz;

	thiz = esvg_parser_paint_server_data_get(tag);
	return thiz;
}

Eina_Bool _parser_pattern_attribute_set(Edom_Tag *tag,
		const char *key, const char *value)
{
	Enesim_Renderer *r;
	Eina_Bool ret = EINA_TRUE;

	r = esvg_parser_element_renderer_get(tag);

	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_string_from(&x, value, ESVG_COORD_0);
		esvg_pattern_x_set(r, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_string_from(&y, value, ESVG_COORD_0);
		esvg_pattern_y_set(r, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_string_from(&width, value, ESVG_LENGTH_0);
		esvg_pattern_width_set(r, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_string_from(&height, value, ESVG_LENGTH_0);
		esvg_pattern_height_set(r, &height);
	}
	else if (strcmp(key, "patternUnits") == 0)
	{
		Esvg_Gradient_Units units;

		esvg_parser_gradient_units_string_from(&units, value);
		esvg_pattern_units_set(r, units);
	}
	else if (strcmp(key, "patternTransform") == 0)
	{
		Enesim_Matrix matrix;

		esvg_transformation_string_from(&matrix, value);
		esvg_pattern_transform_set(r, &matrix);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
#if 0
		Esvg_Parser_Pattern *thiz;
		Edom_Parser *parser;

		parser = edom_tag_parser_get(tag);
		thiz = _esvg_parser_pattern_get(tag);
		thiz->href = strdup(value);
		/* always add the callback, as we should inherit every property which is *not* set
		 * including the stops
		 */
		esvg_parser_post_parse_add(parser, _post_parse_href_cb, thiz);
#endif
	}
	else
		return EINA_FALSE;

	return EINA_TRUE;
}

static const char * _parser_pattern_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_pattern_name_get(Edom_Tag *tag)
{
	return "pattern";
}

static Eina_Bool _parser_pattern_child_supported(Edom_Tag *tag, int tag_id)
{
	switch (tag_id)
	{
		case ESVG_USE:
		case ESVG_SVG:
		case ESVG_CIRCLE:
		case ESVG_ELLIPSE:
		case ESVG_RECT:
		case ESVG_LINE:
		case ESVG_PATH:
		case ESVG_POLYLINE:
		case ESVG_POLYGON:
		case ESVG_TEXT:
		case ESVG_G:
		case ESVG_IMAGE:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}

static Eina_Bool _parser_pattern_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Esvg_Parser_Pattern *thiz;
	Enesim_Renderer *r;
	int tag_id;

	thiz = _esvg_parser_pattern_get(tag);
	r = esvg_parser_element_renderer_get(child);
	if (r)
		esvg_pattern_content_set(thiz->r, r);

	return EINA_TRUE;
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_pattern_name_get,
	/* .attribute_set 	= */ _parser_pattern_attribute_set,
	/* .attribute_get 	= */ _parser_pattern_attribute_get,
	/* .child_supported	= */ _parser_pattern_child_supported,
	/* .child_add		= */ _parser_pattern_child_add,
	/* .child_remove	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_pattern_new(Edom_Parser *parser)
{
	Esvg_Parser_Pattern *thiz;
	Enesim_Renderer *r;
	Edom_Tag *tag;

	thiz = calloc(1, sizeof(Esvg_Parser_Pattern));
	r = esvg_pattern_new();
	thiz->r = r;
	tag = esvg_parser_paint_server_new(parser, &_descriptor, ESVG_PATTERN,
			r, thiz);
	thiz->tag = tag;

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

