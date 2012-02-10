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
typedef struct _Esvg_Parser_Gradient
{
	Esvg_Parser_Gradient_Descriptor *descriptor;
	Edom_Tag *tag;
	char *href;
	void *data;
} Esvg_Parser_Gradient;

static Esvg_Parser_Gradient * _esvg_parser_gradient_get(Edom_Tag *tag)
{
	Esvg_Parser_Gradient *thiz;

	thiz = esvg_parser_paint_server_data_get(tag);
	return thiz;
}

static void _parser_gradient_merge(Esvg_Parser_Gradient *thiz,
			Enesim_Renderer *r,
			Enesim_Renderer *rel)
{
	Eina_Bool is_rg;
	Eina_Bool rel_is_rg;
	const Eina_List *l;

	/* set the stops */
	esvg_gradient_stop_get(r, &l);
	if (!l)
	{
		Esvg_Gradient_Stop *s;
		const Eina_List *l2;
		const Eina_List *tmp;

		esvg_gradient_stop_get(rel, &l2);
		EINA_LIST_FOREACH(l2, tmp, s)
		{
			printf("adding stops from the relative %g\n", s->offset.value);
			esvg_gradient_stop_add(r, s);
		}
	}
	if (!esvg_gradient_units_is_set(r))
	{
		Esvg_Gradient_Units gu;

		esvg_gradient_units_get(rel, &gu);
		esvg_gradient_units_set(r, gu);
	}
	is_rg = esvg_is_radial_gradient(r);
	rel_is_rg = esvg_is_radial_gradient(rel);
	/* same type */
	if (!(is_rg ^ rel_is_rg))
	{
		if (thiz->descriptor->merge)
			thiz->descriptor->merge(r, rel);
	}
}

static void _post_parse_href_cb(Edom_Parser *parser, void *data)
{
	Esvg_Parser_Gradient *thiz = data;
	Edom_Tag *rel = NULL;
	Enesim_Renderer *r;
	Enesim_Renderer *r_rel;
	Eina_Bool ret;

	ret = esvg_href_get(&rel, thiz->tag, thiz->href);
	if (!rel) return;

	r = esvg_parser_element_renderer_get(thiz->tag);
	r_rel = esvg_parser_element_renderer_get(rel);

	_parser_gradient_merge(thiz, r, r_rel);
}

static Eina_Bool _parser_gradient_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Enesim_Renderer *r;
	Eina_Bool ret = EINA_TRUE;

	r = esvg_parser_element_renderer_get(tag);

	if (strcmp(key, "gradientUnits") == 0)
	{
		Esvg_Gradient_Units units;
		esvg_parser_gradient_units_get(&units, value);
		esvg_gradient_units_set(r, units);
	}
	/* TODO how to handle mutiple hrefs?  and animating them?*/
	/* handle the xlink:xref */
	else if (strcmp(key, "xlink:href") == 0)
	{
		Esvg_Parser_Gradient *thiz;
		Edom_Parser *parser;

		parser = edom_tag_parser_get(tag);
		thiz = _esvg_parser_gradient_get(tag);
		thiz->href = strdup(value);
		/* always add the callback, as we should inherit every property which is *not* set
		 * including the stops
		 */
		esvg_parser_post_parse_add(parser, _post_parse_href_cb, thiz);
	}
	else if (strcmp(key, "gradientTransform") == 0)
	{
		Enesim_Matrix matrix;

		esvg_transformation_get(&matrix, value);
		esvg_gradient_transform_set(r, &matrix);
	}
	else if (strcmp(key, "spreadMethod") == 0)
	{
		Esvg_Spread_Method smethod;

		esvg_parser_spread_method_get(&smethod, value);
		esvg_gradient_spread_method_set(r, smethod);
	}
	else
	{
		Esvg_Parser_Gradient *thiz;

		thiz = _esvg_parser_gradient_get(tag);
 		if (thiz->descriptor->attribute_set)
			ret = thiz->descriptor->attribute_set(tag, key, value);
	}
	return ret;
}

static const char * _parser_gradient_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_gradient_name_get(Edom_Tag *tag)
{
	Esvg_Parser_Gradient *thiz;

	thiz = _esvg_parser_gradient_get(tag);
	return thiz->descriptor->name_get(tag);
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_gradient_name_get,
	/* .attribute_set 	= */ _parser_gradient_attribute_set,
	/* .attribute_get 	= */ _parser_gradient_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_gradient_new(Edom_Context *c,
		Esvg_Parser_Gradient_Descriptor *descriptor,
		int type,
		Edom_Tag *topmost,
		Enesim_Renderer *r,
		void *data)
{
	Esvg_Parser_Gradient *thiz;
	Edom_Tag *tag;

	thiz = calloc(1, sizeof(Esvg_Parser_Gradient));
	thiz->descriptor = descriptor;
	thiz->data = data;

	tag = esvg_parser_paint_server_new(c, &_descriptor, type,
			topmost, r, thiz);
	thiz->tag = tag;

	return tag;
}

void * esvg_parser_gradient_data_get(Edom_Tag *tag)
{
	Esvg_Parser_Gradient *thiz;

	thiz = _esvg_parser_gradient_get(tag);
	return thiz->data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
