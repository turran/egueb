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
/*
 * This should be the common parser for the gradients (linear and radial)
 * as both have only one type of child, the <stop> tag
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Context_Gradient
{
	Edom_Tag *tag;
	Edom_Tag *topmost;
	Enesim_Renderer *r;
} Esvg_Parser_Context_Gradient;
/*----------------------------------------------------------------------------*
 *                               Stop parsing                                 *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_parser_gradient_stop(Edom_Context *context,
		Esvg_Parser_Context_Gradient *thiz, Edom_Parser *parser,
		const char *attributes, unsigned int length)
{
	Esvg_Gradient_Stop *stop;
	Edom_Tag *tag;

	tag = esvg_parser_stop_new(context, thiz->topmost);
	edom_tag_attributes_from_xml(tag, attributes, length);
	edom_tag_child_add(thiz->tag, tag);

	stop = edom_tag_data_get(tag);
	esvg_gradient_stop_add(thiz->r, stop);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _gradient_tag_open(void *data, int tag,
		Edom_Context *context, const char *attributes,
		unsigned int length)
{
	Esvg_Parser_Context_Gradient *thiz = data;
	Edom_Parser *parser;
	Eina_Bool ret;

	switch (tag)
	{
		case ESVG_STOP:
		parser = edom_context_parser_get(context);
		ret = _esvg_parser_gradient_stop(context, thiz, parser, attributes, length);
		break;

		default:
		ret = EINA_FALSE;
		break;
	}

	return ret;
}

static void _gradient_tag_close(void *data, int tag,
		Edom_Context *context)
{
	Edom_Parser *parser;

	switch (tag)
	{
		/* destroy ourselves */
		case ESVG_RADIALGRADIENT:
		case ESVG_LINEARGRADIENT:
		parser = edom_context_parser_get(context);
		context = edom_parser_context_pop(parser);
		edom_context_delete(context);
		break;

		default:
		break;
	}
}

static void _gradient_free(void *data)
{
	Esvg_Parser_Context_Gradient *thiz = data;

	free(thiz);
}

static Edom_Context_Descriptor _descriptor = {
	/* .tag_open = 	*/ _gradient_tag_open,
	/* .tag_close =	*/ _gradient_tag_close,
	/* .data =	*/ NULL,
	/* .cdata =	*/ NULL,
	/* .free = 	*/ _gradient_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Context * esvg_parser_context_gradient_new(Edom_Tag *tag, Enesim_Renderer *r)
{
	Esvg_Parser_Context_Gradient *thiz;
	Edom_Parser *parser;
	Edom_Tag *topmost;

	thiz = calloc(1, sizeof(Esvg_Parser_Context_Gradient));
	if (!thiz) return NULL;

	topmost = edom_tag_topmost_get(tag);
	if (!topmost)
	{
		printf("WTF!\n");
		return NULL;
	}

	thiz->tag = tag;
	thiz->topmost = topmost;
	thiz->r = r;

	parser = edom_tag_parser_get(tag);

	return edom_context_new(parser, &_descriptor, thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
