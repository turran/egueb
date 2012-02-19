/* Edom - DOM
 * Copyright (C) 2011 Jorge Luis Zapata
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

#include <ctype.h> /* for isspace() */

#include <Eina.h>

#include "Edom.h"
#include "edom_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Edom_Parser
{
	Eina_Array *contexts;
	Edom_Parser_Descriptor *descriptor;
	char *root;
	char *location;
	void *data;
};

static Eina_Bool _edom_parser_tag_get(Edom_Parser *thiz, const char *content, size_t sz,
		int *tag)
{
	if (!thiz->descriptor) return EINA_FALSE;
	if (!thiz->descriptor->tag_get) return EINA_FALSE;
	return thiz->descriptor->tag_get(thiz, content, sz, tag);
}
/*----------------------------------------------------------------------------*
 *                      Eina's simple XML interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _edom_parser_cb(void *data, Eina_Simple_XML_Type type,
		const char *content, unsigned offset, unsigned length)
{
	Edom_Parser *thiz = data;
	Edom_Context *context;
	Eina_Array *contexts;
	Eina_Bool ret;
	int tag;

	contexts = thiz->contexts;
	context = eina_array_data_get(contexts, eina_array_count_get(contexts) - 1);

	switch (type)
	{
		case EINA_SIMPLE_XML_COMMENT:
			/* FIXME what to do here ... the comments are still being parsed */
			break;
		case EINA_SIMPLE_XML_OPEN:
		case EINA_SIMPLE_XML_OPEN_EMPTY:
		{
			int sz;
			const char *attrs;
			int attr_length;

			attrs = eina_simple_xml_tag_attributes_find(content, length);
			sz = length;
			if (attrs)
			{
				sz = attrs - content;
				attr_length = length - sz;
				while ((sz > 0) && (isspace(content[sz - 1])))
					sz--;
			}
			ret = _edom_parser_tag_get(thiz, content, sz, &tag);
			if (!ret)
			{
				/* TODO we should add some flag on the parser to either break or return FALSE
				 * i.e stop on error or not
				 */
				break;
			}
			edom_context_tag_open(context, tag, contexts, attrs, attr_length);
			/* kind of ugly but works */
			if (type == EINA_SIMPLE_XML_OPEN_EMPTY)
			{
				Edom_Context *last_context;

				last_context = eina_array_data_get(contexts, eina_array_count_get(contexts) - 1);
				edom_context_tag_close(last_context, tag, contexts);
			}
		}
		break;

		case EINA_SIMPLE_XML_CLOSE:
		ret = _edom_parser_tag_get(thiz, content, length, &tag);
		if (!ret)
		{
			break;
		}
		edom_context_tag_close(context, tag, contexts);
		break;

		case EINA_SIMPLE_XML_DATA:
		edom_context_data(context, content, length);
		break;

		case EINA_SIMPLE_XML_CDATA:
		edom_context_cdata(context, content, length);
		break;

		default:
		break;
	}

	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Edom_Parser * edom_parser_new(Edom_Parser_Descriptor *descriptor, void *data)
{
	Edom_Parser *thiz;

	thiz = calloc(1, sizeof(Edom_Parser));
	thiz->contexts = eina_array_new(1);
	thiz->descriptor = descriptor;
	thiz->data = data;

	return thiz;
}

EAPI void edom_parser_delete(Edom_Parser *thiz)
{
	eina_array_free(thiz->contexts);
	free(thiz);
}

EAPI void edom_parser_location_set(Edom_Parser *thiz, const char *location)
{
	if (thiz->location)
	{
		free(thiz->location);
		thiz->location = NULL;
	}
	if (location)
	{
		const char *tmp;
		size_t len;

		thiz->location = strdup(location);
		/* get last '/' */
		len = strlen(location);
		tmp = location + len;
		while (*tmp != '/' && tmp != location)
			tmp--;

		len = (tmp - location) + 1;
		thiz->root = malloc(len + 1);
		strncpy(thiz->root, location, len);
		thiz->root[len] = '\0';
	}
}

EAPI const char * edom_parser_location_get(Edom_Parser *thiz)
{
	if (!thiz) return NULL;
	return thiz->location;
}

EAPI const char * edom_parser_root_get(Edom_Parser *thiz)
{
	if (!thiz) return NULL;
	return thiz->root;
}

EAPI Eina_Bool edom_parser_parse(Edom_Parser *thiz, Edom_Context *doc, const char *content, size_t len)
{
	if (!thiz) return EINA_FALSE;

	eina_array_push(thiz->contexts, doc);
	return eina_simple_xml_parse(content, len, EINA_TRUE, _edom_parser_cb, thiz);
}

EAPI void * edom_parser_data_get(Edom_Parser *thiz)
{
	return thiz->data;
}
