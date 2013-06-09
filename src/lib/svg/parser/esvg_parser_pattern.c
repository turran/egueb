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
#include "Egueb_Svg_Parser.h"
#include "egueb_svg_parser_private.h"
#include "egueb_svg_values.h"
/*
 * The pattern parser might be kind of complicated given that we can parse
 * the same as the svg i think, but everything must be rendered to an image
 * and then use it as the source of the pattern. Dunno what's best to do
 * internally, given that the paint server *must* be public
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Svg_Parser_Pattern
{
	Egueb_Dom_Tag *tag;
	Enesim_Renderer *r;
	char *href;
} Egueb_Svg_Parser_Pattern;

static Egueb_Svg_Parser_Pattern * _egueb_svg_parser_pattern_get(Egueb_Dom_Tag *tag)
{
	Egueb_Svg_Parser_Pattern *thiz;

	thiz = egueb_svg_parser_paint_server_data_get(tag);
	return thiz;
}

Eina_Bool _parser_pattern_attribute_set(Egueb_Dom_Tag *tag,
		const char *key, const char *value)
{
	Enesim_Renderer *r;
	Eina_Bool ret = EINA_TRUE;

	r = egueb_svg_parser_element_renderer_get(tag);

	if (strcmp(key, "x") == 0)
	{
		Egueb_Svg_Coord x;

		egueb_svg_length_string_from(&x, value, ESVG_COORD_0);
		egueb_svg_pattern_x_set(r, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Egueb_Svg_Coord y;

		egueb_svg_length_string_from(&y, value, ESVG_COORD_0);
		egueb_svg_pattern_y_set(r, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Egueb_Svg_Length width;

		egueb_svg_length_string_from(&width, value, ESVG_LENGTH_0);
		egueb_svg_pattern_width_set(r, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Egueb_Svg_Length height;

		egueb_svg_length_string_from(&height, value, ESVG_LENGTH_0);
		egueb_svg_pattern_height_set(r, &height);
	}
	else if (strcmp(key, "patternUnits") == 0)
	{
		Egueb_Svg_Element_Gradient_Units units;

		egueb_svg_parser_gradient_units_string_from(&units, value);
		egueb_svg_pattern_units_set(r, units);
	}
	else if (strcmp(key, "patternTransform") == 0)
	{
		Enesim_Matrix matrix;

		egueb_svg_transformation_string_from(&matrix, value);
		egueb_svg_pattern_transform_set(r, &matrix);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
#if 0
		Egueb_Svg_Parser_Pattern *thiz;
		Egueb_Dom_Parser *parser;

		parser = egueb_dom_tag_parser_get(tag);
		thiz = _egueb_svg_parser_pattern_get(tag);
		thiz->href = strdup(value);
		/* always add the callback, as we should inherit every property which is *not* set
		 * including the stops
		 */
		egueb_svg_parser_post_parse_add(parser, _post_parse_href_cb, thiz);
#endif
	}
	else
		return EINA_FALSE;

	return EINA_TRUE;
}

static const char * _parser_pattern_attribute_get(Egueb_Dom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_pattern_name_get(Egueb_Dom_Tag *tag)
{
	return "pattern";
}

static Eina_Bool _parser_pattern_child_supported(Egueb_Dom_Tag *tag, int tag_id)
{
	switch (tag_id)
	{
		case ESVG_TYPE_USE:
		case ESVG_ELEMENT_SVG:
		case ESVG_TYPE_CIRCLE:
		case ESVG_TYPE_ELLIPSE:
		case ESVG_ELEMENT_RECT:
		case ESVG_ELEMENT_LINE:
		case ESVG_ELEMENT_PATH:
		case ESVG_ELEMENT_POLYLINE:
		case ESVG_ELEMENT_POLYGON:
		case ESVG_TYPE_TEXT:
		case ESVG_TYPE_G:
		case ESVG_TYPE_IMAGE:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}

static Eina_Bool _parser_pattern_child_add(Egueb_Dom_Tag *tag, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Parser_Pattern *thiz;
	Enesim_Renderer *r;
	int tag_id;

	thiz = _egueb_svg_parser_pattern_get(tag);
	r = egueb_svg_parser_element_renderer_get(child);
	if (r)
		egueb_svg_pattern_content_set(thiz->r, r);

	return EINA_TRUE;
}

static Egueb_Dom_Tag_Descriptor _descriptor = {
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
Egueb_Dom_Tag * egueb_svg_parser_pattern_new(Egueb_Dom_Parser *parser)
{
	Egueb_Svg_Parser_Pattern *thiz;
	Enesim_Renderer *r;
	Egueb_Dom_Tag *tag;

	thiz = calloc(1, sizeof(Egueb_Svg_Parser_Pattern));
	r = egueb_svg_pattern_new();
	thiz->r = r;
	tag = egueb_svg_parser_paint_server_new(parser, &_descriptor, ESVG_PATTERN,
			r, thiz);
	thiz->tag = tag;

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

