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
/* If we dont find common properties among every paint server then this interface
 * can go away
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Paint_Server
{
	Edom_Tag_Descriptor *descriptor;
	void *data;
} Esvg_Parser_Paint_Server;

static Esvg_Parser_Paint_Server * _esvg_parser_paint_server_get(Edom_Tag *tag)
{
	Esvg_Parser_Paint_Server *thiz;

	thiz = esvg_parser_element_data_get(tag);
	return thiz;
}

static Eina_Bool _parser_paint_server_attribute_set(Edom_Tag *tag, const char *key, const char *value)
{
	Esvg_Parser_Paint_Server *thiz;
	Eina_Bool ret = EINA_TRUE;

	thiz = _esvg_parser_paint_server_get(tag);
	if (thiz->descriptor->attribute_set)
	{
		ret = thiz->descriptor->attribute_set(tag, key, value);
	}
	return ret;
}

static const char * _parser_paint_server_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_paint_server_name_get(Edom_Tag *tag)
{
	Esvg_Parser_Paint_Server *thiz;

	thiz = _esvg_parser_paint_server_get(tag);
	return thiz->descriptor->name_get(tag);
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get 		= */ _parser_paint_server_name_get,
	/* .attribute_set 	= */ _parser_paint_server_attribute_set,
	/* .attribute_get 	= */ _parser_paint_server_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Tag * esvg_parser_paint_server_new(Edom_Context *c,
		Edom_Tag_Descriptor *descriptor, int type,
		Edom_Tag *topmost,
		Enesim_Renderer *r,
		void *data)
{
	Esvg_Parser_Paint_Server *thiz;
	Edom_Tag *tag;

	thiz = calloc(1, sizeof(Esvg_Parser_Paint_Server));
	thiz->descriptor = descriptor;
	thiz->data = data;

	tag = esvg_parser_element_new(c, &_descriptor, type, topmost, r, thiz);

	return tag;
}

void * esvg_parser_paint_server_data_get(Edom_Tag *tag)
{
	Esvg_Parser_Paint_Server *thiz;

	thiz = _esvg_parser_paint_server_get(tag);
	return thiz->data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

