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
#define ESVG_PARSER_SHAPE_MAGIC 0xe5501002
#define ESVG_PARSER_SHAPE_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_PARSER_SHAPE_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_PARSER_SHAPE_MAGIC);\
	} while(0)

typedef struct _Esvg_Parser_Shape
{
	EINA_MAGIC
	Edom_Tag_Descriptor *descriptor;
	void *data;
} Esvg_Parser_Shape;

static Esvg_Parser_Shape * _esvg_parser_shape_get(Edom_Tag *tag)
{
	Esvg_Parser_Shape *thiz;

	thiz = esvg_parser_element_data_get(tag);
	ESVG_PARSER_SHAPE_MAGIC_CHECK(thiz);

	return thiz;
}

static Eina_Bool _parser_shape_attribute_set(Edom_Tag *tag, const char *key,
		const char *value)
{
	Esvg_Parser_Shape *thiz;
	Enesim_Renderer *r;

	thiz = _esvg_parser_shape_get(tag);
	r = esvg_parser_element_renderer_get(tag);

	if (thiz->descriptor)
	{
		if (thiz->descriptor->attribute_set)
			return thiz->descriptor->attribute_set(tag, key, value);
	}

	return EINA_TRUE;
}

static const char * _parser_shape_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_shape_name_get(Edom_Tag *tag)
{
	Esvg_Parser_Shape *thiz;

	thiz = _esvg_parser_shape_get(tag);
	return thiz->descriptor->name_get(tag);
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_shape_name_get,
	/* .attribute_set 	= */ _parser_shape_attribute_set,
	/* .attribute_get 	= */ _parser_shape_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_shape_new(Edom_Context *c,
		Edom_Tag_Descriptor *descriptor,
		Esvg_Parser_Tag_Type type, Edom_Tag *topmost,
		Enesim_Renderer *r, void *data)
{
	Esvg_Parser_Shape *thiz;
	Edom_Tag *tag;

	thiz = calloc(1, sizeof(Esvg_Parser_Shape));
	EINA_MAGIC_SET(thiz, ESVG_PARSER_SHAPE_MAGIC);
	thiz->data = data;
	thiz->descriptor = descriptor;

	tag = esvg_parser_element_new(c, &_descriptor, type, topmost, r, thiz);

	return tag;
}

void * esvg_parser_shape_data_get(Edom_Tag *tag)
{
	Esvg_Parser_Shape *thiz;

	thiz = _esvg_parser_shape_get(tag);
	return thiz->data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

