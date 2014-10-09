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
#include "egueb_dom_document.h"
#include "egueb_dom_character_data.h"
#include "egueb_dom_text.h"
#include "egueb_dom_cdata_section.h"
#include "egueb_dom_element.h"

#include "egueb_dom_parser_private.h"
#include <ctype.h> /* for isspace() */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* given that the eina array can not push NULL values, we need to create a fake
 * node
 */
static Egueb_Dom_Node * _fake_node = (Egueb_Dom_Node*)0xdeadbeef;

typedef struct _Egueb_Dom_Parser_Eina
{
	Eina_Array *contexts;
	Egueb_Dom_Parser *parser;
	Eina_Hash *entities;
} Egueb_Dom_Parser_Eina;

typedef void (*Egueb_Dom_Parser_Eina_Cb)(Egueb_Dom_Parser_Eina *thiz,
		Egueb_Dom_Node *node, const char *content,
		unsigned length);

typedef struct _Egueb_Dom_Parser_Eina_Attribute_Data
{
	Egueb_Dom_Parser_Eina *thiz;
	Egueb_Dom_Node *node;
} Egueb_Dom_Parser_Eina_Attribute_Data;

typedef struct _Egueb_Dom_Parser_Eina_NS_Data
{
	Egueb_Dom_Parser_Eina *thiz;
	const char *prefix;
	unsigned int prefix_length;
	char *ns;
} Egueb_Dom_Parser_Eina_NS_Data;

static Egueb_Dom_Node * _egueb_dom_parser_eina_context_get(
		Egueb_Dom_Parser_Eina *thiz)
{
	Egueb_Dom_Node *ret = NULL;
	int count;

	count = eina_array_count_get(thiz->contexts);
	if (count) ret = eina_array_data_get(thiz->contexts, count - 1);
	/* be sure that it is not the fake node */
	if (ret == _fake_node)
		ret = NULL;
	return ret;
}

static const char * _egueb_dom_parser_eina_get_entity(Egueb_Dom_Parser_Eina *thiz,
		const char *s, const char **e)
{
	const char *tmp;
	char *entity;
	int len;

	tmp = s;
	while (*tmp != ';')
		tmp++;
	len = tmp - s - 1;
	entity = strndup(s + 1, len);
	entity[len] = '\0';
	*e = tmp;

	return eina_hash_find(thiz->entities, entity);
}

static Egueb_Dom_String * _egueb_dom_parser_eina_transform_text(Egueb_Dom_Parser_Eina *thiz,
		const char *text, unsigned int len)
{
	Egueb_Dom_String *ret;
	Eina_Strbuf *strbuf = NULL;
	const char *last = NULL;
	const char *s = text;
	const char *end;

	/* until eos */
	if (!len)
	{
		len = strlen(text);
	}
	end = text + len;

	while (s < end && *s)
	{
		if (*s == '&')
		{
			const char *entity;
			const char *e;

			entity = _egueb_dom_parser_eina_get_entity(thiz, s, &e);
			if (!entity)
			{
				s++;
				continue;
			}
			DBG("Entity '%s' found", entity);
			if (!strbuf)
				strbuf = eina_strbuf_new();
			eina_strbuf_append_length(strbuf, text, s - text);
			if (last)
				eina_strbuf_append_length(strbuf, last, s - last);
			eina_strbuf_append(strbuf, entity);
			last = e + 1;
			s = e;
		}
		s++;
	}
	if (last)
	{
		eina_strbuf_append_length(strbuf, last, s - last);
	}

	if (strbuf)
	{
		ret = egueb_dom_string_new_with_string(eina_strbuf_string_steal(strbuf));
		eina_strbuf_free(strbuf);
	}
	else
	{
		ret = egueb_dom_string_new_with_length(text, len);
	}

	return ret;
}

static Eina_Bool _egueb_dom_parser_eina_tag_attributes_set_cb(void *data, const char *key,
		const char *value)
{
	Egueb_Dom_Parser_Eina_Attribute_Data *attr_data = data;
	Egueb_Dom_Parser_Eina *thiz = attr_data->thiz;
	Egueb_Dom_Node *node = attr_data->node;
	Egueb_Dom_String *name;
	Egueb_Dom_String *v;

	name = egueb_dom_string_new_with_static_string(key);
	v = _egueb_dom_parser_eina_transform_text(thiz, value, 0);
	DBG("Parsed attribute '%s' with value '%s'", key, value);
	egueb_dom_element_attribute_set(node, name, v, NULL);
	egueb_dom_string_unref(name);
	egueb_dom_string_unref(v);

	return EINA_TRUE;
}

static void _egueb_dom_parser_eina_tag_attribute_set(Egueb_Dom_Parser_Eina *thiz,
		Egueb_Dom_Node *node, const char *attributes, unsigned int length)
{
	Egueb_Dom_Parser_Eina_Attribute_Data data;

	data.thiz = thiz;
	data.node = node;
	eina_simple_xml_attributes_parse(attributes, length, _egueb_dom_parser_eina_tag_attributes_set_cb, &data);
}

static Eina_Bool _egueb_dom_parser_eina_tag_ns_find_cb(void *data, const char *key,
		const char *value)
{
	Egueb_Dom_Parser_Eina_NS_Data *ns_data = data;

	if (!strncmp(key, "xmlns", 5))
	{
		/* check if the element name has a prefix, if so,
		 * check that the prefix matches the xmlns prefix
		 * if that's the case we have a found the creation
		 * namespace (<foo:bar xmlns:foo="http://www.foo.com">)
		 */
		if (ns_data->prefix_length)
		{
			if (key[5] == ':' && !strncmp(ns_data->prefix, &key[6],
					ns_data->prefix_length))
			{
				DBG("Namespace '%s' with prefix '%.*s' found",
						value, ns_data->prefix_length,
						ns_data->prefix);
				ns_data->ns = strdup(value);
				return EINA_FALSE;
			}
		}
		/* check that the element does not have a prefix, if
		 * so, we have a found the creation namespace
		 */
		else
		{
			if (key[5] == '\0')
			{
				DBG("Namespace '%s' found", value);
				ns_data->ns = strdup(value);
				return EINA_FALSE;
			}
		}
	}
	return EINA_TRUE;
}

static char * _egueb_dom_parser_eina_tag_ns_find(Egueb_Dom_Parser_Eina *thiz,
		const char *name, unsigned int name_length,
		const char *attrs,  unsigned int attr_length)
{
	Egueb_Dom_Parser_Eina_NS_Data data;
	const char *start, *end;

	data.prefix_length = 0;
	data.prefix = name;
	data.ns = NULL;

	/* get the element prefix first */
	start = name;
	end = start + name_length;
	while (start < end)
	{
		if (*start == ':')
		{
			data.prefix_length = start - name;
			break;
		}
		start++;
	}

	/* we need to parse the attributes first to find the namespaces */
	eina_simple_xml_attributes_parse(attrs, attr_length,
			_egueb_dom_parser_eina_tag_ns_find_cb, &data);
	/* TODO in case no namespace is found, find the namespace from the ancestors */
	return data.ns;
}

static Egueb_Dom_Node * _egueb_dom_parser_eina_tag_new(Egueb_Dom_Parser_Eina *thiz,
		const char *name, unsigned int name_length,
		const char *attrs,  unsigned int attr_length)
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *node;
	Egueb_Dom_Node *parent;
	Egueb_Dom_Node *topmost;
	Egueb_Dom_String *e_name;
	Egueb_Dom_String *e_ns;
	char *ns;

	parent = _egueb_dom_parser_eina_context_get(thiz);
	doc = egueb_dom_parser_document_get(thiz->parser);
	if (!doc) return NULL;

	/* get the namespace from the arguments */
	ns = _egueb_dom_parser_eina_tag_ns_find(thiz, name, name_length,
		attrs,  attr_length);
	e_ns = egueb_dom_string_steal(ns);

	/* now create the element with the namespace found */
	e_name = egueb_dom_string_new_with_length(name, name_length);
	node = egueb_dom_document_element_ns_create(doc, e_ns, e_name, NULL);
	egueb_dom_string_unref(e_ns);
	egueb_dom_string_unref(e_name);

	if (!node) return NULL;

	/* in case we dont have a topmost element, set it */
	topmost = egueb_dom_document_document_element_get(doc);
	if (!topmost)
	{
		egueb_dom_document_element_set(doc, node);
	}
	else
	{
		egueb_dom_node_unref(topmost);
	}


	/* first add the child */
	if (parent) egueb_dom_node_child_append(parent, node, NULL);
	/* parse the attributes */
	_egueb_dom_parser_eina_tag_attribute_set(thiz, node, attrs, attr_length);

	return node;
}

static void _egueb_dom_parser_eina_tag_cdata_set(Egueb_Dom_Parser_Eina *thiz,
		const char *cdata, unsigned int length)
{
	Egueb_Dom_Node *parent;
	Egueb_Dom_Node *node;
	Egueb_Dom_Node *doc;
	Egueb_Dom_String *str;

	parent = _egueb_dom_parser_eina_context_get(thiz);
	if (!parent) return;

	doc = egueb_dom_parser_document_get(thiz->parser);
	if (!doc) return;

	node = egueb_dom_cdata_section_new();
	if (!node) return;

	egueb_dom_node_child_append(parent, node, NULL);
	/* set the content */
	DBG("Appending string to a cdata section node");
	str = egueb_dom_string_new_with_length(cdata, length);
	egueb_dom_character_data_append_data(node, str, NULL);
}

static void _egueb_dom_parser_eina_tag_text_set(Egueb_Dom_Parser_Eina *thiz,
		const char *text, unsigned int length)
{
	Egueb_Dom_Node *parent;
	Egueb_Dom_Node *node;
	Egueb_Dom_Node *doc;
	Egueb_Dom_String *str;

	parent = _egueb_dom_parser_eina_context_get(thiz);
	if (!parent) return;

	doc = egueb_dom_parser_document_get(thiz->parser);
	if (!doc) return;

	node = egueb_dom_text_new();
	if (!node) return;

	egueb_dom_node_child_append(parent, node, NULL);
	/* set the content */
	DBG("Appending string to a text node");
	str = _egueb_dom_parser_eina_transform_text(thiz, text, length);
	egueb_dom_character_data_append_data(node, str, NULL);
}

#ifdef EINA_POST_1_8_0
static void _egueb_dom_parser_eina_doctype_child(Egueb_Dom_Parser_Eina *thiz,
		const char *text, unsigned int length)
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
		DBG("Adding entity '%s' for '%s'", attr, name);
		eina_hash_add(thiz->entities, name, attr);
	}
}
#endif

static void _egueb_dom_parser_eina_xml_open(Egueb_Dom_Parser_Eina *thiz,
		const char *content, unsigned int length)
{
	Egueb_Dom_Node *node = NULL;
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
	node = _egueb_dom_parser_eina_tag_new(thiz, content, sz, attrs, attr_length);
	if (!node) node = _fake_node;
	eina_array_push(thiz->contexts, node);
}

static void _egueb_dom_parser_eina_xml_close(Egueb_Dom_Parser_Eina *thiz,
		const char *content, unsigned int length)
{
	eina_array_pop(thiz->contexts);
}
/*----------------------------------------------------------------------------*
 *                      Eina's simple XML interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_parser_eina_cb(void *data, Eina_Simple_XML_Type type,
		const char *content, unsigned int offset,
		unsigned int length)
{
	Egueb_Dom_Parser_Eina *thiz = data;

	switch (type)
	{
		case EINA_SIMPLE_XML_OPEN:
		_egueb_dom_parser_eina_xml_open(thiz, content, length);
		break;

		case EINA_SIMPLE_XML_OPEN_EMPTY:
		_egueb_dom_parser_eina_xml_open(thiz, content, length);
		_egueb_dom_parser_eina_xml_close(thiz, content, length);
		break;

		case EINA_SIMPLE_XML_CLOSE:
		_egueb_dom_parser_eina_xml_close(thiz, content, length);
		break;

		case EINA_SIMPLE_XML_DATA:
		_egueb_dom_parser_eina_tag_text_set(thiz, content, length);
		break;

		case EINA_SIMPLE_XML_CDATA:
		_egueb_dom_parser_eina_tag_cdata_set(thiz, content, length);
		break;

#ifdef EINA_POST_1_8_0
		case EINA_SIMPLE_XML_DOCTYPE_CHILD:
		_egueb_dom_parser_eina_doctype_child(thiz, content, length);
		break;
#endif

		case EINA_SIMPLE_XML_IGNORED:
		case EINA_SIMPLE_XML_COMMENT:
		case EINA_SIMPLE_XML_DOCTYPE:
		break;

		default:
		WARN("Unsupported case %d", type);
		break;
	}

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                             Parser interface                               *
 *----------------------------------------------------------------------------*/
static void _egueb_dom_parser_eina_parse(void *data, Enesim_Stream *source)
{
	Egueb_Dom_Parser_Eina *thiz = data;
	void *content;
	size_t len = 0;

	content = enesim_stream_mmap(source, &len);
	if (!content) return;

	eina_simple_xml_parse(content, len, EINA_TRUE, _egueb_dom_parser_eina_cb,
			thiz);
	enesim_stream_munmap(source, content);
}

static void _egueb_dom_parser_eina_free(void *data)
{
	Egueb_Dom_Parser_Eina *thiz = data;

	/* pop the global void parent */
	eina_array_pop(thiz->contexts);
	eina_array_free(thiz->contexts);
	eina_hash_free(thiz->entities);
	free(thiz);
}

static Egueb_Dom_Parser_Descriptor _descriptor = {
	/* .parse = 	*/ _egueb_dom_parser_eina_parse,
	/* .free =	*/ _egueb_dom_parser_eina_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Parser * egueb_dom_parser_eina_new(void)
{
	Egueb_Dom_Parser_Eina *thiz;
	Egueb_Dom_Parser *parser;

	thiz = calloc(1, sizeof(Egueb_Dom_Parser_Eina));
	thiz->entities = eina_hash_string_superfast_new(free);
	/* add common entities */
	eina_hash_add(thiz->entities, "quot", strdup("\""));
	eina_hash_add(thiz->entities, "amp", strdup("&"));
	eina_hash_add(thiz->entities, "apos", strdup("'"));
	eina_hash_add(thiz->entities, "lt", strdup("<"));
	eina_hash_add(thiz->entities, "gt", strdup(">"));

	parser = egueb_dom_parser_new(&_descriptor, thiz);

	thiz->contexts = eina_array_new(1);
	/* push the global void parent */
	eina_array_push(thiz->contexts, NULL);

	thiz->parser = parser;
	return parser;
}
