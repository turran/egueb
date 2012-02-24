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
typedef struct _Esvg_Parser_A
{
	Edom_Parser *parser;
	char *href;
} Esvg_Parser_A;

static Eina_Bool _parser_a_attribute_set(Edom_Tag *tag, const char *key,
		const char *value)
{
	if (strcmp(key, "id") == 0)
	{
		/* nothing to do here yet */
	}
	else if (strcmp(key, "xlink:href") == 0)
	{
		Esvg_Parser_A *thiz;

		thiz = edom_tag_data_get(tag);
		/* absolute */
		if (*value == '/')
		{
			thiz->href = strdup(value);
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
			thiz->href = strdup(real);
		}
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static const char * _parser_a_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_a_name_get(Edom_Tag *tag)
{
	return "a";
}

static Eina_Bool _parser_a_child_supported(Edom_Tag *tag, int tag_id)
{
	printf("svg child supported %d\n", tag_id);
	switch (tag_id)
	{
		case ESVG_A:
		case ESVG_LINEARGRADIENT:
		case ESVG_RADIALGRADIENT:
		case ESVG_PATTERN:
		case ESVG_DEFS:
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
		case ESVG_STYLE:
		case ESVG_IMAGE:
		case ESVG_CLIPPATH:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}

static Eina_Bool _parser_a_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Enesim_Renderer *r;
	Enesim_Renderer *rr = NULL;
	Edom_Tag *parent;
	int tag_id;

	/* FIXME we are assuming that an <a> tag is always a children
	 * of a svg or g */
	parent = edom_tag_parent_get(tag);
	if (!parent) return EINA_TRUE;

	rr = esvg_parser_element_renderer_get(parent);
	tag_id = edom_tag_type_get(child);
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
		r = esvg_parser_element_renderer_get(child);

		default:
		break;
	}
	if (rr && r)
	{
		Esvg_Parser_A *thiz;

		thiz = edom_tag_data_get(tag);
		esvg_container_element_add(rr, r);
		/* trigger the parser callback */
		esvg_parser_href_set(thiz->parser, r, thiz->href);
	}

	return EINA_TRUE;
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_a_name_get,
	/* .attribute_set 	= */ _parser_a_attribute_set,
	/* .attribute_get 	= */ _parser_a_attribute_get,
	/* .child_supported	= */ _parser_a_child_supported,
	/* .child_add		= */ _parser_a_child_add,
	/* .child_remove	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_a_new(Edom_Parser *p)
{
	Esvg_Parser_A *thiz;
	Edom_Tag *tag;

	thiz = calloc(1, sizeof(Esvg_Parser_A));
	thiz->parser = p;
	tag = edom_tag_new(p, &_descriptor, ESVG_A, thiz);

	return tag;
}

const char * esvg_parser_a_href_get(Edom_Tag *t)
{
	Esvg_Parser_A *thiz;

	thiz = edom_tag_data_get(t);
	return thiz->href;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

