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
/*
 * In order to support styling we need to keep track of every element
 * parsed, that is, generate the DOM tree. This way we can support Ecss
 * context to know whenver an element has to be applied with a specific
 * style
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Context_Style
{
	Edom_Tag *tag;
} Esvg_Parser_Context_Style;
/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
static void _style_cdata(void *data, const char *cdata, unsigned int length)
{
	Esvg_Parser_Context_Style *thiz = data;
	Esvg_Parser_Style *ps;
	Ecss_Style *s;

	ps = edom_tag_data_get(thiz->tag);
	s = ecss_style_load_from_content(cdata, length);
	if (!s) return;

	esvg_parser_style_style_set(ps, s);
}

static void _style_tag_close(void *data, int tag,
		Eina_Array *contexts)
{
	Edom_Context *context;

	switch (tag)
	{
		/* destroy ourselves */
		case ESVG_STYLE:
		context = eina_array_pop(contexts);
		edom_context_delete(context);
		break;

		default:
		break;
	}
}

static void _style_free(void *data)
{
	Esvg_Parser_Context_Style *thiz = data;

	free(thiz);
}
static Edom_Context_Descriptor _descriptor = {
	/* .tag_open = 	*/ NULL,
	/* .tag_close =	*/ _style_tag_close,
	/* .data =	*/ NULL,
	/* .cdata =	*/ _style_cdata,
	/* .free = 	*/ _style_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Context * esvg_parser_context_style_new(Edom_Tag *tag)
{
	Esvg_Parser_Context_Style *thiz;
	Edom_Parser *parser;

	thiz = calloc(1, sizeof(Esvg_Parser_Context_Style));
	if (!thiz) return NULL;

	thiz->tag = tag;

	parser = edom_tag_parser_get(tag);

	return edom_context_new(parser, &_descriptor, thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
