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
#include "esvg_parser_private.h"
#include "esvg_values.h"
/* TODO add fthe following missing properties
 * in
 * result
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* several tags use the xlink:href property, we should add a single code for that
 * and some special callback */
static void _post_parse_cb(Edom_Parser *parser, void *data)
{
#if 0
	Esvg_Parser_Use *thiz = data;
	Edom_Tag *ref_t = NULL;
	Edom_Tag *tag;
	Enesim_Renderer *r;
	Eina_Bool ret;

	r = thiz->r;
	tag = thiz->tag;

	ret = esvg_href_get(&ref_t, tag, thiz->href);
	if (ret)
	{
		Enesim_Renderer *ref_r;
		Enesim_Renderer *clone;

		printf("tag found %p\n", ref_t);
		ref_r = esvg_parser_element_renderer_get(ref_t);
		clone = esvg_element_clone(ref_r);
		printf("clone = %p\n", clone);
		esvg_use_link_set(r, clone);
	}
	printf("reference!!! %p %s\n", ref_t, thiz->href);
#endif
}

static Eina_Bool _parser_filter_attribute_set(Edom_Tag *tag, const char *key,
		const char *value)
{
	Enesim_Renderer *r;

	r = esvg_parser_element_renderer_get(tag);
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_get(&x, value, ESVG_COORD_0);
		esvg_filter_x_set(r, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_get(&y, value, ESVG_COORD_0);
		esvg_filter_y_set(r, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_get(&width, value, ESVG_LENGTH_0);
		esvg_filter_width_set(r, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_get(&height, value, ESVG_LENGTH_0);
		esvg_filter_height_set(r, &height);
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
		Edom_Parser *parser;

		/* register the post parsing callback */
		parser = edom_tag_parser_get(tag);
		thiz->href = strdup(value);
		esvg_parser_post_parse_add(parser, _post_parse_cb, thiz);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static const char * _parser_filter_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_filter_name_get(Edom_Tag *tag)
{
	return "filter";
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_filter_name_get,
	/* .attribute_set 	= */ _parser_filter_attribute_set,
	/* .attribute_get 	= */ _parser_filter_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_filter_new(Edom_Context *c, Edom_Tag *topmost)
{
	Edom_Tag *tag;
	Enesim_Renderer *r;

	r = esvg_filter_new();
	tag = esvg_parser_shape_new(c, &_descriptor, ESVG_CIRCLE, topmost, r, NULL);

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

