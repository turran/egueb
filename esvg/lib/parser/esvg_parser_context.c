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

/* This is just a wrapper on top of the edom context to trigger correctly
 * the parser descriptor functions
 */
#include "Esvg.h"
#include "Esvg_Parser.h"
#include "esvg_parser_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Context
{
	Edom_Context_Descriptor descriptor;
	Edom_Parser *parser;
	void *data;
} Esvg_Parser_Context;
/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _context_tag_open(void *data, int tag,
		Edom_Context *context, const char *attributes,
		unsigned int length)
{
	Esvg_Parser_Context *thiz = data;
	Eina_Bool ret = EINA_FALSE;

	if (thiz->descriptor.tag_open)
		ret = thiz->descriptor.tag_open(thiz->data, tag, context, attributes, length);

	return ret;
}

static void _context_tag_close(void *data, int tag,
		Edom_Context *context)
{
	Esvg_Parser_Context *thiz = data;

	if (thiz->descriptor.tag_close)
		thiz->descriptor.tag_close(thiz->data, tag, context);
}

static void _context_free(void *data)
{
	Esvg_Parser_Context *thiz = data;

	if (thiz->descriptor.free)
		thiz->descriptor.free(thiz->data);
	free(thiz);
}

static Edom_Context_Descriptor _descriptor = {
	/* .tag_open = 	*/ _context_tag_open,
	/* .tag_close =	*/ _context_tag_close,
	/* .data =	*/ NULL,
	/* .cdata =	*/ NULL,
	/* .free = 	*/ _context_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Edom_Context * esvg_parser_context_new(Edom_Parser *parser,
		Edom_Context_Descriptor *descriptor,
		void *data)
{
	Esvg_Parser_Context *thiz;

	thiz = calloc(1, sizeof(Esvg_Parser_Context));
	thiz->descriptor = *descriptor;
	thiz->data = data;
	thiz->parser = parser;

	return edom_context_new(parser, &_descriptor, thiz);
}
