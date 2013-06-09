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
#include "egueb_svg_parser_private.h"
#include "egueb_svg_values.h"
/* TODO add fthe following missing properties
 * in
 * result
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* several tags use the xlink:href property, we should add a single code for that
 * and some special callback */
static void _post_parse_cb(Egueb_Dom_Parser *parser, void *data)
{
#if 0
	Egueb_Svg_Parser_Use *thiz = data;
	Egueb_Dom_Tag *ref_t = NULL;
	Egueb_Dom_Tag *tag;
	Enesim_Renderer *r;
	Eina_Bool ret;

	r = thiz->r;
	tag = thiz->tag;

	ret = egueb_svg_href_get(&ref_t, tag, thiz->href);
	if (ret)
	{
		Enesim_Renderer *ref_r;
		Enesim_Renderer *clone;

		printf("tag found %p\n", ref_t);
		ref_r = egueb_svg_parser_element_renderer_get(ref_t);
		clone = egueb_svg_element_clone(ref_r);
		printf("clone = %p\n", clone);
		egueb_svg_element_use_link_set(r, clone);
	}
	printf("reference!!! %p %s\n", ref_t, thiz->href);
#endif
}

static Eina_Bool _parser_filter_attribute_set(Egueb_Dom_Tag *tag, const char *key,
		const char *value)
{
	Enesim_Renderer *r;

	r = egueb_svg_parser_element_renderer_get(tag);
	if (strcmp(key, "x") == 0)
	{
		Egueb_Svg_Coord x;

		egueb_svg_length_string_from(&x, value, ESVG_COORD_0);
		egueb_svg_filter_x_set(r, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Egueb_Svg_Coord y;

		egueb_svg_length_string_from(&y, value, ESVG_COORD_0);
		egueb_svg_filter_y_set(r, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Egueb_Svg_Length width;

		egueb_svg_length_string_from(&width, value, ESVG_LENGTH_0);
		egueb_svg_filter_width_set(r, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Egueb_Svg_Length height;

		egueb_svg_length_string_from(&height, value, ESVG_LENGTH_0);
		egueb_svg_filter_height_set(r, &height);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
		Egueb_Dom_Parser *parser;

		/* register the post parsing callback */
		parser = egueb_dom_tag_parser_get(tag);
		thiz->href = strdup(value);
		egueb_svg_parser_post_parse_add(parser, _post_parse_cb, thiz);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static const char * _parser_filter_attribute_get(Egueb_Dom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_filter_name_get(Egueb_Dom_Tag *tag)
{
	return "filter";
}

static Egueb_Dom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_filter_name_get,
	/* .attribute_set 	= */ _parser_filter_attribute_set,
	/* .attribute_get 	= */ _parser_filter_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Tag * egueb_svg_parser_filter_new(Egueb_Dom_Context *c, Egueb_Dom_Tag *topmost)
{
	Egueb_Dom_Tag *tag;
	Enesim_Renderer *r;

	r = egueb_svg_filter_new();
	tag = egueb_svg_parser_shape_new(c, &_descriptor, ESVG_TYPE_CIRCLE, topmost, r, NULL);

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

