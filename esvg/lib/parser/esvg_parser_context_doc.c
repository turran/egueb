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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <Eina.h>

#include "Esvg.h"
#include "Esvg_Parser.h"
#include "esvg_parser_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Context_Doc
{
	Edom_Tag *tag;
} Esvg_Parser_Context_Doc;
/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _doc_tag_open(void *data, int tag_type, Edom_Context *context,
		const char *attributes, unsigned int length)
{
	Esvg_Parser_Context_Doc *thiz = data;
	Edom_Context *new_context;
	Edom_Parser *parser;
	Edom_Tag *tag;
	Enesim_Renderer *r;

	/* the document context only supports SVG tags */
	if (tag_type != ESVG_SVG)
		return EINA_FALSE;

	parser = edom_context_parser_get(context);
	tag = esvg_parser_svg_new(context, NULL);
	edom_tag_attributes_from_xml(tag, attributes, length);
	r = esvg_parser_element_renderer_get(tag);

	new_context = esvg_parser_context_svg_new(tag, r);
	edom_parser_context_push(parser, new_context);

	thiz->tag = tag;

	return EINA_TRUE;
}

static void _doc_free(void *data)
{
	Esvg_Parser_Context_Doc *thiz = data;

	free(thiz);
}

static Edom_Context_Descriptor _descriptor = {
	/* .tag_open = 	*/ _doc_tag_open,
	/* .tag_close = */ NULL,
	/* .data =	*/ NULL,
	/* .cdata =	*/ NULL,
	/* .free = 	*/ _doc_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Context * esvg_parser_context_doc_new(Edom_Parser *parser)
{
	Esvg_Parser_Context_Doc *thiz;

	thiz = calloc(1, sizeof(Esvg_Parser_Context_Doc));
	if (!thiz) return NULL;

	return edom_context_new(parser, &_descriptor, thiz);
}

Edom_Tag * esvg_parser_context_doc_topmost_get(Edom_Context *c)
{
	Esvg_Parser_Context_Doc *thiz;

	thiz = edom_context_data_get(c);
	if (!thiz) return NULL;

	return thiz->tag;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
