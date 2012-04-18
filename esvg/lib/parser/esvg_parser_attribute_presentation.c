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

/* TODO
 * the functions we set here must be transformed to function pointers
 * given that more than one possible tag can have this attributes
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
static Eina_Bool esvg_parser_attribute_presentation_attribute_set(Edom_Tag *tag,
		 const char *key, const char *value)
{
	else if (strcmp(key, "transform") == 0)
	{
		Enesim_Matrix matrix;

		esvg_transformation_get(&matrix, value);
		esvg_element_transform_set(r, &matrix);
	}
	else if (strcmp(key, "style") == 0)
	{
		Esvg_Attribute_Presentation attr;

		memset(&attr, 0, sizeof(Esvg_Attribute_Presentation));
		esvg_parser_style_inline_set(value, tag, &attr);
		esvg_element_style_set(r, &attr);
	}
	/* common presentation attributes */
	else if (strcmp(key, "clip-path") == 0)
	{
		Edom_Parser *parser;

		parser = edom_tag_parser_get(tag);
		thiz->clip_path = strdup(value);
		esvg_parser_post_parse_add(parser, _post_parse_clip_path_cb, thiz);
	}
	else if (strcmp(key, "opacity") == 0)
	{
		double opacity = esvg_number_string_from(value, 1.0);
		esvg_element_opacity_set(r, opacity);
	}
	else if (strcmp(key, "color") == 0)
	{
		Esvg_Color color;

		esvg_color_get(&color, value);
		esvg_element_color_set(r, &color);
	}
	else if (strcmp(key, "fill") == 0)
	{
		Esvg_Paint fill;

		if (!esvg_paint_string_from(&fill, tag, value))
		{
			/* we delay the parsing of fill for later in case
			 * the value has an uri
			 */
			if (esvg_parser_is_uri(value))
			{
				Edom_Parser *parser;

				parser = edom_tag_parser_get(tag);
				thiz->fill = strdup(value);
				esvg_parser_post_parse_add(parser, _post_parse_fill_cb, thiz);
			}
		}
		else
		{
			esvg_element_fill_set(r, &fill);
		}
	}
	else if (strcmp(key, "fill-rule") == 0)
	{
		Esvg_Fill_Rule fill_rule;

		esvg_parser_fill_rule_get(&fill_rule, value);
		esvg_element_fill_rule_set(r, fill_rule);
	}
	else if (strcmp(key, "fill-opacity") == 0)
	{
		double fill_opacity = esvg_number_string_from(value, 1.0);
		esvg_element_fill_opacity_set(r, fill_opacity);
	}
	else if (strcmp(key, "stroke") == 0)
	{
		Esvg_Paint stroke;

		if (!esvg_paint_string_from(&stroke, tag, value))
		{
			/* we delay the parsing of stroke for later in case
			 * the value has an uri
			 */
			if (esvg_parser_is_uri(value))
			{
				Edom_Parser *parser;

				parser = edom_tag_parser_get(tag);
				thiz->stroke = strdup(value);
				esvg_parser_post_parse_add(parser, _post_parse_stroke_cb, thiz);
			}
		}
		else
		{
			esvg_element_stroke_set(r, &stroke);
		}
	}
	else if (strcmp(key, "stroke-width") == 0)
	{
		Esvg_Length stroke_width;

		esvg_length_string_from(&stroke_width, value, ESVG_LENGTH_1);
		esvg_element_stroke_width_set(r, &stroke_width);
	}
	else if (strcmp(key, "stroke-opacity") == 0)
	{
		double stroke_opacity = esvg_number_string_from(value, 1.0);
		esvg_element_stroke_opacity_set(r, stroke_opacity);
	}
	else if (strcmp(key, "stroke-linecap") == 0)
	{
		Esvg_Stroke_Line_Cap stroke_line_cap;

		stroke_line_cap = esvg_stroke_line_cap_get(value);
		esvg_element_stroke_line_cap_set(r, stroke_line_cap);
	}
	else if (strcmp(key, "stroke-linejoin") == 0)
	{
		Esvg_Stroke_Line_Join stroke_line_join;

		stroke_line_join = esvg_stroke_line_join_get(value);
		esvg_element_stroke_line_join_set(r, stroke_line_join);
	}
	else if (strcmp(key, "stop-color") == 0)
	{
		Esvg_Color color;

		esvg_color_get(&color, value);
		/* TODO */
	}
	else if (strcmp(key, "stop-opacity") == 0)
	{
		double stop_opacity = esvg_number_string_from(value, 1.0);
		/* TODO */
	}
	else
	{
		if (thiz->descriptor)
		{
			if (thiz->descriptor->attribute_set)
				return thiz->descriptor->attribute_set(tag, key, value);
		}
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

