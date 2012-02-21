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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Context_A
{
	Edom_Parser *parser;
	Edom_Tag *a;
} Esvg_Parser_Context_A;
/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _a_tag_is_supported(void *data, int tag)
{
	switch (tag)
	{
		case ESVG_LINEARGRADIENT:
		case ESVG_RADIALGRADIENT:
		case ESVG_PATTERN:
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
		case ESVG_CLIPPATH:
		case ESVG_USE:
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}

static void _a_tag_added(void *data, Edom_Tag *tag)
{
	Esvg_Parser_Context_A *thiz = data;
	Enesim_Renderer *r;
	const char *href;

	printf("adding an element to the a tag\n");
	href = esvg_parser_a_href_get(thiz->a);
	/* every tag we support has an associated renderer */
	r = esvg_parser_element_renderer_get(tag);
	esvg_parser_href_set(thiz->parser, r, href);
}

static void _a_tag_free(void *data)
{
	Esvg_Parser_Context_A *thiz = data;

	thiz = data;
	free(thiz);
}

static Esvg_Parser_Context_Simple_Descriptor _descriptor = {
	/* .tag_is_supported 	= */ _a_tag_is_supported,
	/* .tag_added 		= */ _a_tag_added,
	/* .free		= */ _a_tag_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Context * esvg_parser_context_a_new(
		Edom_Tag *svg,
		Edom_Tag *parent,
		Enesim_Renderer *parent_r)
{
	Esvg_Parser_Context_A *thiz;
	Edom_Parser *parser;

	if (!parent) return NULL;
	if (!svg) return NULL;

	parser = edom_tag_parser_get(svg);
	if (!parser) return NULL;

	thiz = calloc(1, sizeof(Esvg_Parser_Context_A));
	thiz->parser = parser;
	thiz->a = parent;

	return esvg_parser_context_simple_new(ESVG_G, svg, parent, parent_r,
		&_descriptor, thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

