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
 * The pattern parser might be kind of complicated given that we can parse
 * the same as the svg i think, but everything must be rendered to an image
 * and then use it as the source of the pattern. Dunno what's best to do
 * internally, given that the paint server *must* be public
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Esvg_Parser_Pattern
{
	Eina_Hash *ids;
} Esvg_Parser_Pattern;
/*----------------------------------------------------------------------------*
 *                         The context interface                               *
 *----------------------------------------------------------------------------*/
static Edom_Context_Descriptor _descriptor = {
	/* .tag_open = 	*/ NULL,
	/* .tag_close =	*/ NULL,
	/* .data =	*/ NULL,
	/* .cdata =	*/ NULL,
	/* .free = 	*/ NULL,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Edom_Context * esvg_parser_pattern_new(Eina_Hash *ids)
{
	Esvg_Parser_Pattern *thiz;

	thiz = calloc(1, sizeof(Esvg_Parser_Pattern));
	if (!thiz) return NULL;

	thiz->ids = ids;

	return edom_context_new(NULL, &_descriptor, thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

