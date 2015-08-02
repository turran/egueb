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
#include "egueb_dom_list.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_node_map_named.h"
#include "egueb_dom_parser.h"
#include "egueb_dom_implementation.h"
#include "egueb_dom_registry.h"
#include "egueb_dom_document.h"

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
/**
 * Parse a document from an enesim stream
 * @param[in] s The stream to parse @ender_transfer{full}
 * @param[out] doc The resulting parsed document @ender_type{egueb.dom.document}
 * @return EINA_TRUE if the parsing is correct, EINA_FALSE otherwise
 */
EAPI Eina_Bool egueb_dom_parser_parse(Enesim_Stream *s, Egueb_Dom_Node **doc)
{
	Egueb_Dom_Parser *thiz;
	Eina_Bool ret = EINA_FALSE;
	const char *uri;

	if (!s)
		return ret;
	if (!doc)
		goto done;

	thiz = egueb_dom_parser_eina_new();

	if (!thiz->descriptor)
		goto no_parse;

	if (!thiz->descriptor->parse)
		goto no_parse;

	/* create a document in case the user does not provide it */
	if (!*doc)
	{
		Egueb_Dom_Implementation *i;
		Egueb_Dom_String *mime_s;
		const char *mime;

		mime = enesim_image_mime_data_from(s);
		if (!mime)
		{
			ERR("No document provided and no mime found");
			goto no_parse;
		}
		mime_s = egueb_dom_string_new_with_static_string(mime);
		i = egueb_dom_registry_implementation_get_by_mime(mime_s);
		egueb_dom_string_unref(mime_s);

		if (!i)
		{
			ERR("No implementation found for mime '%s'", mime);
			egueb_dom_implementation_unref(i);
			goto no_parse;
		}

		INFO("New document created for mime '%s'", mime);
		*doc = egueb_dom_implementation_document_create(i);
		egueb_dom_implementation_unref(i);
	}

	egueb_dom_parser_document_set(thiz, *doc);
	/* Set the URI in case we do have one */
	uri = enesim_stream_uri_get(s);
	if (uri)
	{
		Egueb_Dom_String *suri;
		suri = egueb_dom_string_new_with_string(uri);
		egueb_dom_document_uri_set(*doc, suri);
	}

	thiz->descriptor->parse(thiz->data, s);
	ret = EINA_TRUE;
no_parse:
	egueb_dom_parser_free(thiz);
done:
	enesim_stream_unref(s);
	return ret;
}
