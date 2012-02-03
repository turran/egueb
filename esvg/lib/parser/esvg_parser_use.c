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
#include "esvg_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Use
{
	Edom_Tag *tag;
	Enesim_Renderer *r;
	char *href;
} Esvg_Parser_Use;

static void _post_parse_cb(Edom_Parser *parser, void *data)
{
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
}

static Eina_Bool _parser_use_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Esvg_Parser_Use *thiz;
	Enesim_Renderer *r;

	thiz = esvg_parser_element_data_get(tag);
	r = thiz->r;
	if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_get(&x, value, ESVG_COORD_0);
		esvg_use_x_set(r, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_get(&y, value, ESVG_COORD_0);
		esvg_use_y_set(r, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_get(&width, value, ESVG_LENGTH_0);
		esvg_use_width_set(r, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_get(&height, value, ESVG_LENGTH_0);
		esvg_use_height_set(r, &height);
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

static const char * _parser_use_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_use_name_get(Edom_Tag *tag)
{
	return "use";
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_use_name_get,
	/* .attribute_set 	= */ _parser_use_attribute_set,
	/* .attribute_get 	= */ _parser_use_attribute_get,
};

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_use_new(Edom_Context *c, Edom_Tag *topmost)
{
	Esvg_Parser_Use *thiz;
	Edom_Tag *tag;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Parser_Use));

	r = esvg_use_new();
	thiz->r = r;

	tag = esvg_parser_element_new(c, &_descriptor, ESVG_USE, topmost, r, thiz);
	thiz->tag = tag;

	return tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

