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
#include "esvg_private.h"
#include "esvg_parser_private.h"
#include "esvg_values.h"
/* TODO
 * Handle the following attributes
 * onunload
 * onabort 
 * onerror 
 * onresize
 * onscroll
 * onzoom
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_PARSER_SVG_MAGIC 0xe5501003
#define ESVG_PARSER_SVG_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_PARSER_SVG_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_PARSER_SVG_MAGIC);\
	} while(0)

typedef struct _Esvg_Parser_Svg
{
	EINA_MAGIC
	Edom_Tag *tag; /* the tag associated with this svg */
	Eina_List *styles; /* the list of styles found on this svg scope */
	Eina_List *svgs; /* the list of svg documents found on the svg */
	Enesim_Renderer *r; /* the renderer associated with this svg */
	Eina_Hash *ids; /* the ids found */
} Esvg_Parser_Svg;

static Esvg_Parser_Svg * _esvg_parser_svg_get(Edom_Tag *tag)
{
	Esvg_Parser_Svg *thiz;

	thiz = esvg_parser_element_data_get(tag);
	ESVG_PARSER_SVG_MAGIC_CHECK(thiz);

	return thiz;
}

static Eina_Bool _parser_svg_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Esvg_Parser_Svg *thiz;
	Enesim_Renderer *r;

	thiz = _esvg_parser_svg_get(tag);
	r = thiz->r;
	if (strcmp(key, "version") == 0)
	{
		double version = esvg_number_get(value, 0.0);
		esvg_svg_version_set(r, version);
	}
	else if (strcmp(key, "x") == 0)
	{
		Esvg_Coord x;

		esvg_length_get(&x, value, ESVG_COORD_0);
		esvg_svg_x_set(r, &x);
	}
	else if (strcmp(key, "y") == 0)
	{
		Esvg_Coord y;

		esvg_length_get(&y, value, ESVG_COORD_0);
		esvg_svg_y_set(r, &y);
	}
	else if (strcmp(key, "width") == 0)
	{
		Esvg_Length width;

		esvg_length_get(&width, value, ESVG_LENGTH_0);
		esvg_svg_width_set(r, &width);
	}
	else if (strcmp(key, "height") == 0)
	{
		Esvg_Length height;

		esvg_length_get(&height, value, ESVG_LENGTH_0);
		esvg_svg_height_set(r, &height);
	}
	else if (strcmp(key, "viewBox") == 0)
	{
		Esvg_View_Box vb = esvg_view_box_get(value);
		esvg_svg_viewbox_set(r, &vb);
	}

	return EINA_TRUE;
}

static const char * _parser_svg_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_svg_name_get(Edom_Tag *tag)
{
	return "svg";
}

static Eina_Bool _parser_svg_child_supported(Edom_Tag *tag, int tag_id)
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

static Eina_Bool _parser_svg_child_add(Edom_Tag *tag, Edom_Tag *child)
{
	Enesim_Renderer *r = NULL;
	Esvg_Parser_Svg *thiz;
	int tag_id;

	thiz = _esvg_parser_svg_get(tag);
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
	if (r)
		esvg_container_element_add(thiz->r, r);
	esvg_parser_svg_tag_add(tag, child);

	return EINA_TRUE;
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_svg_name_get,
	/* .attribute_set 	= */ _parser_svg_attribute_set,
	/* .attribute_get 	= */ _parser_svg_attribute_get,
	/* .child_supported	= */ _parser_svg_child_supported,
	/* .child_add		= */ _parser_svg_child_add,
	/* .child_remove	= */ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_svg_new(Edom_Parser *parser)
{
	Esvg_Parser_Svg *thiz;
	Edom_Tag *tag;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Parser_Svg));
	EINA_MAGIC_SET(thiz, ESVG_PARSER_SVG_MAGIC);
	thiz->ids = eina_hash_string_superfast_new(NULL);

	r = esvg_svg_new();
	thiz->r = r;

	tag = esvg_parser_element_new(parser, &_descriptor, ESVG_SVG, r, thiz);
	thiz->tag = tag;

	return tag;
}

void esvg_parser_svg_style_add(Edom_Tag *tag, Esvg_Parser_Style *s)
{
	Esvg_Parser_Svg *thiz;

	thiz = _esvg_parser_svg_get(tag);
	thiz->styles = eina_list_append(thiz->styles, s);
}

void esvg_parser_svg_style_apply(Edom_Tag *tag)
{
	Esvg_Parser_Svg *thiz;
	Esvg_Parser_Style *s;
	Eina_List *l;

	/* FIXME  we are iterating over every style and going through the
	 * tag tree on ecss, we better merge the styles to only parse the tree
	 * once
	 */
	thiz = _esvg_parser_svg_get(tag);
	EINA_LIST_FOREACH(thiz->styles, l, s)
	{
		printf("applying style %p\n", s);
		esvg_parser_style_apply(s, thiz->tag);
	}
}

void esvg_parser_svg_svg_add(Edom_Tag *tag, Edom_Tag *svg)
{

}

Edom_Tag * esvg_parser_svg_tag_find(Edom_Tag *tag, const char *id)
{
	Esvg_Parser_Svg *thiz;

	thiz = _esvg_parser_svg_get(tag);
	return eina_hash_find(thiz->ids, id);
}

void esvg_parser_svg_tag_add(Edom_Tag *tag, Edom_Tag *child_tag)
{
	Esvg_Parser_Svg *thiz;
	const char *id;

	thiz = _esvg_parser_svg_get(tag);
	id = edom_tag_id_get(child_tag);
	if (id)
	{
		eina_hash_add(thiz->ids, id, child_tag);
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

