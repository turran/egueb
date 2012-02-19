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
#define ESVG_PARSER_ELEMENT_MAGIC 0xe5501001
#define ESVG_PARSER_ELEMENT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_PARSER_ELEMENT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_PARSER_ELEMENT_MAGIC);\
	} while(0)

typedef struct _Esvg_Parser_Element
{
	EINA_MAGIC
	Edom_Tag_Descriptor *descriptor;
	Enesim_Renderer *r;
	Edom_Tag *tag;
	char *fill;
	char *stroke;
	char *clip_path;
	void *data;
} Esvg_Parser_Element;

static Esvg_Parser_Element * _esvg_parser_element_get(Edom_Tag *tag)
{
	Esvg_Parser_Element *thiz;

	thiz = edom_tag_data_get(tag);
	ESVG_PARSER_ELEMENT_MAGIC_CHECK(thiz);

	return thiz;
}

static void _post_parse_fill_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Element *thiz = data;
	Esvg_Paint fill;
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(thiz->tag);
	esvg_paint_get(&fill, thiz->tag, thiz->fill);
	esvg_element_fill_set(r, &fill);
}

static void _post_parse_stroke_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Element *thiz = data;
	Esvg_Paint stroke;
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(thiz->tag);
	esvg_paint_get(&stroke, thiz->tag, thiz->stroke);
	esvg_element_stroke_set(r, &stroke);
}

static void _post_parse_clip_path_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Element *thiz = data;
	Edom_Tag *rel = NULL;
	Enesim_Renderer *r;
	Enesim_Renderer *r_rel;
	Eina_Bool ret;

	r = esvg_parser_element_renderer_get(thiz->tag);
	ret = esvg_uri_get(&rel, thiz->tag, thiz->clip_path);
	if (!rel) return;

	r_rel = esvg_parser_element_renderer_get(rel);
	esvg_element_clip_path_set(r, r_rel);
}

static Eina_Bool _parser_element_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Esvg_Parser_Element *thiz;
	Enesim_Renderer *r;

	thiz = _esvg_parser_element_get(tag);
	r = thiz->r;

	if (strcmp(key, "id") == 0)
	{
		esvg_element_id_set(r, value);
		edom_tag_id_set(tag, value);
	}
	/* FIXME the class should not be here, but for now ... */
	else if (strcmp(key, "class") == 0)
	{
		edom_tag_class_set(tag, value);
	}
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
		double opacity = esvg_number_get(value, 1.0);
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

		if (!esvg_paint_get(&fill, tag, value))
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
		double fill_opacity = esvg_number_get(value, 1.0);
		esvg_element_fill_opacity_set(r, fill_opacity);
	}
	else if (strcmp(key, "stroke") == 0)
	{
		Esvg_Paint stroke;

		if (!esvg_paint_get(&stroke, tag, value))
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

		esvg_length_get(&stroke_width, value, ESVG_LENGTH_1);
		esvg_element_stroke_width_set(r, &stroke_width);
	}
	else if (strcmp(key, "stroke-opacity") == 0)
	{
		double stroke_opacity = esvg_number_get(value, 1.0);
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
		double stop_opacity = esvg_number_get(value, 1.0);
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

	return EINA_TRUE;
}

static const char * _parser_element_attribute_get(Edom_Tag *tag, const char *attribute)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	/* FIXME handle common properties */
	if (thiz->descriptor)
	{
		if (thiz->descriptor->attribute_get)
			return thiz->descriptor->attribute_get(tag, attribute);
	}
	return NULL;
}

static const char * _parser_element_name_get(Edom_Tag *tag)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	return thiz->descriptor->name_get(tag);
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_element_name_get,
	/* .attribute_set 	= */ _parser_element_attribute_set,
	/* .attribute_get 	= */ _parser_element_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_element_new(Edom_Context *context,
		Edom_Tag_Descriptor *descriptor,
		Esvg_Parser_Tag_Type type,
		Edom_Tag *topmost,
		Enesim_Renderer *r,
		void *data)
{
	Esvg_Parser_Element *thiz;
	Edom_Tag *tag;

	thiz = calloc(1, sizeof(Esvg_Parser_Element));
	EINA_MAGIC_SET(thiz, ESVG_PARSER_ELEMENT_MAGIC);
	thiz->r = r;
	thiz->descriptor = descriptor;
	thiz->data = data;

	tag = edom_tag_new(context, &_descriptor, type, topmost, thiz);
	thiz->tag = tag;

	return tag;
}

void * esvg_parser_element_data_get(Edom_Tag *tag)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	return thiz->data;
}

Enesim_Renderer * esvg_parser_element_renderer_get(Edom_Tag *tag)
{
	Esvg_Parser_Element *thiz;

	thiz = _esvg_parser_element_get(tag);
	return thiz->r;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

