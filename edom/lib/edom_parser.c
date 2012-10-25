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

#include "Edom.h"
#include "edom_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* We use this context to be able to know if the closing tag is of a type
 * we do support
 */
typedef struct _Edom_Parser_Context
{
	void *tag;
	int id;
} Edom_Parser_Context;

struct _Edom_Parser
{
	Eina_Array *contexts;
	Eina_Hash *entities;
	Edom_Parser_Descriptor *descriptor;
	Eina_Strbuf *string;
	char *root;
	char *location;
	void *data;
};

typedef struct _Edom_Parser_Attribute_Data
{
	Edom_Parser *thiz;
	void *tag;
} Edom_Parser_Attribute_Data;

static const char * _edom_parser_get_entity(Edom_Parser *thiz, const char *s, const char **e)
{
	const char *tmp;
	char entity[128];
	const char *rep;
	int len;

	tmp = s;
	while (*tmp != ';')
		tmp++;
	len = tmp - s - 1;
	strncpy(entity, s + 1, len);
	entity[len] = '\0';
	*e = tmp;

	return eina_hash_find(thiz->entities, entity);
}

static Eina_Bool _edom_parser_tag_get(Edom_Parser *thiz, const char *content, size_t sz,
		int *tag_id)
{
	if (!thiz->descriptor) return EINA_FALSE;
	if (!thiz->descriptor->tag_get) return EINA_FALSE;
	return thiz->descriptor->tag_get(thiz, content, sz, tag_id);
}

static Eina_Bool _edom_parser_tag_attributes_set_cb(void *data, const char *key,
		const char *value)
{
	Edom_Parser_Attribute_Data *attr_data = data;
	Edom_Parser *thiz = attr_data->thiz;
	void *tag = attr_data->tag;
	const char *s = value;
	Eina_Bool transformed = EINA_FALSE;

	while (*s)
	{
		if (*s == '&')
		{
			const char *entity;
			const char *e;
			entity = _edom_parser_get_entity(thiz, s, &e);
			if (!entity)
			{
				s++;
				continue;
			}
			s = e;
			if (!transformed)
			{
				eina_strbuf_reset(thiz->string);
				eina_strbuf_append_length(thiz->string, s, s - e);
				transformed = EINA_TRUE;
			}
			eina_strbuf_append(thiz->string, entity);
		}
		else if (transformed)
		{
			eina_strbuf_append_char(thiz->string, *s);
		}
		s++;
	}

	return thiz->descriptor->tag_attribute_set(thiz, tag, key, transformed ? eina_strbuf_string_get(thiz->string) : value);
}

static void * _edom_parser_topmost_get(Edom_Parser *thiz)
{
	if (!thiz->descriptor) return NULL;
	if (!thiz->descriptor->topmost_get) return NULL;
	return thiz->descriptor->topmost_get(thiz);
}

static void _edom_parser_tag_attribute_set(Edom_Parser *thiz, void *t, const char *attributes, unsigned int length)
{
	Edom_Parser_Attribute_Data data;

	if (!thiz->descriptor) return;

	data.thiz = thiz;
	data.tag = t;

	eina_simple_xml_attributes_parse(attributes, length, _edom_parser_tag_attributes_set_cb, &data);
}

static void * _edom_parser_tag_new(Edom_Parser *thiz, void *parent, int tag_id, const char *attrs, unsigned int attr_length)
{
	void *tag;

	if (!thiz->descriptor) return NULL;
	if (!thiz->descriptor->tag_new) return NULL;
	tag = thiz->descriptor->tag_new(thiz, tag_id);

	if (!tag) return NULL;

	/* parse the attributes */
	if (thiz->descriptor->tag_attribute_set)
	{
		_edom_parser_tag_attribute_set(thiz, tag, attrs, attr_length);
	}
	/* add the child */
	if (thiz->descriptor->tag_child_add && parent)
	{
		thiz->descriptor->tag_child_add(thiz, parent, tag);
	}

	return tag;
}

static void _edom_parser_tag_cdata_set(Edom_Parser *thiz, void *t, const char *cdata, unsigned int length)
{
	if (!thiz->descriptor) return;
	if (!thiz->descriptor->tag_cdata_set) return;
	thiz->descriptor->tag_cdata_set(thiz, t, cdata, length);
}

static void _edom_parser_tag_text_set(Edom_Parser *thiz, void *t, const char *text, unsigned int length)
{
	if (!thiz->descriptor) return;
	if (!thiz->descriptor->tag_text_set) return;
	thiz->descriptor->tag_text_set(thiz, t, text, length);
}
		
static void _edom_parser_doctype_child(Edom_Parser *thiz, const char *text, unsigned int length)
{
	while (isspace(*text))
		text++;
	/* entity NAME "ATTR" */
	if (!strncmp(text, "ENTITY", 6))
	{
		const char *tmp;
		char *name;
		char *attr;

		text += 6;
		/* skip the spaces until the name */
		while (isspace(*text))
			text++;
		tmp = text;
		while (!isspace(*text))
			text++;
		name = strndup(tmp, text - tmp);
		/* get the attribute value */
		/* skip the spaces until the "" */
		while (*text != '"')
			text++;
		text++;
		tmp = text;
		while (*text != '"')
			text++;
		/* get the string inside the " " */
		attr = strndup(tmp, text - tmp);
		eina_hash_add(thiz->entities, name, attr);
	}
}
/*----------------------------------------------------------------------------*
 *                      Eina's simple XML interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _edom_parser_cb(void *data, Eina_Simple_XML_Type type,
		const char *content, unsigned offset, unsigned length)
{
	Edom_Parser_Context *parent = NULL;
	Edom_Parser *thiz = data;
	Eina_Array *contexts;
	Eina_Bool ret;
	int count;
	int tag_id;

	contexts = thiz->contexts;
	count = eina_array_count_get(contexts);
	if (count)
	{
		parent = eina_array_data_get(contexts, count - 1);
	}

	switch (type)
	{
		case EINA_SIMPLE_XML_OPEN:
		case EINA_SIMPLE_XML_OPEN_EMPTY:
		{
			void *tag;
			int sz;
			const char *attrs = NULL;
			int attr_length = 0;

			attrs = eina_simple_xml_tag_attributes_find(content, length);
			sz = length;
			if (attrs)
			{
				sz = attrs - content;
				attr_length = length - sz;
				while ((sz > 0) && (isspace(content[sz - 1])))
					sz--;
			}
			ret = _edom_parser_tag_get(thiz, content, sz, &tag_id);
			if (!ret)
			{
				/* TODO we should add some flag on the parser to either break or return FALSE
				 * i.e stop on error or not
				 */
				break;
			}
			tag = _edom_parser_tag_new(thiz, parent ? parent->tag : NULL, tag_id, attrs, attr_length);

			/* kind of ugly but works */
			if (type == EINA_SIMPLE_XML_OPEN && tag)
			{
				Edom_Parser_Context *current;

				current = calloc(1, sizeof(Edom_Parser_Context));
				current->tag = tag;
				current->id = tag_id;
				/* push it the new tag as a new context */
				eina_array_push(thiz->contexts, current);
			}
		}
		break;

		case EINA_SIMPLE_XML_CLOSE:
		ret = _edom_parser_tag_get(thiz, content, length, &tag_id);
		if (!ret)
		{
			break;
		}
		/* FIXME */
		/* the approach we were using to pop a tag was to compare the close
		 * id tag with the current tag on the stack, if this are the same
		 * pop it. that means that we need to create some kind of wrapper
		 * of the tag to store that information
		 */
		if (parent && (parent->id == tag_id))
		{
			eina_array_pop(thiz->contexts);
			free(parent);
		}
		break;

		case EINA_SIMPLE_XML_DATA:
		if (parent)
			_edom_parser_tag_text_set(thiz, parent->tag, content, length);
		break;

		case EINA_SIMPLE_XML_CDATA:
		if (parent)
			_edom_parser_tag_cdata_set(thiz, parent->tag, content, length);
		break;

		case EINA_SIMPLE_XML_DOCTYPE_CHILD:
		_edom_parser_doctype_child(thiz, content, length);
		break;

		case EINA_SIMPLE_XML_IGNORED:
		case EINA_SIMPLE_XML_COMMENT:
		case EINA_SIMPLE_XML_DOCTYPE:
		break;

		default:
		ERR("Unsupported case %d", type);
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
	thiz->entities = eina_hash_string_superfast_new(NULL);
	thiz->string = eina_strbuf_new();

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

EAPI void * edom_parser_topmost_get(Edom_Parser *thiz)
{
	if (!thiz) return NULL;
	return _edom_parser_topmost_get(thiz);
}

EAPI Eina_Bool edom_parser_parse(Edom_Parser *thiz, const char *content, size_t len)
{
	if (!thiz) return EINA_FALSE;
	return eina_simple_xml_parse(content, len, EINA_TRUE, _edom_parser_cb, thiz);
}

EAPI void * edom_parser_data_get(Edom_Parser *thiz)
{
	return thiz->data;
}
