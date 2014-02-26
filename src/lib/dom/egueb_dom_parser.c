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
#include "egueb_dom_implementation.h"
#include "egueb_dom_implementation_source.h"
#include "egueb_dom_registry.h"

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
EAPI Eina_Bool egueb_dom_parser_parse(Enesim_Stream *data, Egueb_Dom_Node **doc)
{
	Egueb_Dom_Parser *thiz;

	if (!doc) return EINA_FALSE;
	if (!data) return EINA_FALSE;

	thiz = egueb_dom_parser_eina_new();

	if (!thiz->descriptor) return EINA_FALSE;
	if (!thiz->descriptor->parse) return EINA_FALSE;

	/* create a document in case the user does not provide it */
	if (!*doc)
	{
		Egueb_Dom_Implementation *i;
		Egueb_Dom_String *mime_s;
		const char *mime;

		mime = enesim_image_mime_data_from(data);
		if (!mime)
		{
			ERR("No document provided and no mime found");
			return EINA_FALSE;
		}
		mime_s = egueb_dom_string_new_with_static_string(mime);
		i = egueb_dom_registry_implementation_get_by_mime(mime_s);
		egueb_dom_string_unref(mime_s);

		if (!i)
		{
			ERR("No implementation found for mime '%s'", mime);
			return EINA_FALSE;
		}

		INFO("New document created for mime '%s'", mime);
		*doc = egueb_dom_implementation_document_create(i);
		egueb_dom_implementation_unref(i);
	}

	egueb_dom_parser_document_set(thiz, *doc);
	thiz->descriptor->parse(thiz->data, data);
	egueb_dom_parser_free(thiz);

	return EINA_TRUE;
}
