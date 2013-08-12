/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
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
#include "egueb_dom_private.h"

#include "egueb_dom_string.h"
#include "egueb_dom_main.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_parser.h"

#include "egueb_dom_parser_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Egueb_Dom_Parser
{
	Egueb_Dom_Parser_Descriptor *descriptor;
	Egueb_Dom_Node *doc;
	void *data;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Egueb_Dom_Parser * egueb_dom_parser_new(
		Egueb_Dom_Parser_Descriptor * descriptor, void *data)
{
	Egueb_Dom_Parser *thiz;
	thiz = calloc(1, sizeof(Egueb_Dom_Parser));
	thiz->descriptor = descriptor;
	thiz->data = data;

	return thiz;
}

void egueb_dom_parser_free(Egueb_Dom_Parser *thiz)
{
	if (thiz->descriptor && thiz->descriptor->free)
		thiz->descriptor->free(thiz->data);
	free(thiz);
}

void * egueb_dom_parser_data_get(Egueb_Dom_Parser *thiz)
{
	return thiz->data;
}

Egueb_Dom_Node * egueb_dom_parser_document_get(Egueb_Dom_Parser *thiz)
{
	return thiz->doc;
}

void egueb_dom_parser_document_set(Egueb_Dom_Parser *thiz, Egueb_Dom_Node *doc)
{
	thiz->doc = doc;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_dom_parser_parse(Enesim_Stream *data, Egueb_Dom_Node *doc)
{
	Egueb_Dom_Parser *thiz;

	if (!data) return;

	thiz = egueb_dom_parser_eina_new();

	if (!thiz->descriptor) return;
	if (!thiz->descriptor->parse) return;

	egueb_dom_parser_document_set(thiz, doc);
	thiz->descriptor->parse(thiz->data, data);
	egueb_dom_parser_free(thiz);
}
