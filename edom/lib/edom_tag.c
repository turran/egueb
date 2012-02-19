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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Eina.h>

#include "Edom.h"
/* TODO
 * Instead of using a list for children we should use an INLIST to get
 * the siblings and that's all
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Edom_Tag
{
	EINA_INLIST;
	Edom_Context *context;
	Edom_Tag_Descriptor *descriptor;
	int type;
	Edom_Tag *parent;
	Edom_Tag *topmost;
	Edom_Tag *child;
	/* FIXME this two properties should be gone */
	char *id;
	char *class;
	void *data;
};

static void _tag_dump(Edom_Tag *thiz, int level)
{
	Edom_Tag *child;
	const char *name;
	int i;

	for (i = 0; i < level; i++)
		printf(" ");
	name = edom_tag_name_get(thiz);
	printf("%s\n", name ? name : "(UNKNOWN)");
	child = thiz->child;
	while (child)
	{
		_tag_dump(child, level + 1);
		child = (Edom_Tag *)EINA_INLIST_GET(child)->next;
	}
}

static Eina_Bool _attributes_set_cb(void *data, const char *key,
		const char *value)
{
	Edom_Tag *thiz = data;

	if (!thiz->descriptor) return EINA_FALSE;
	if (thiz->descriptor->attribute_set)
		return thiz->descriptor->attribute_set(thiz, key, value);
	return EINA_FALSE;
}

static const char * _attributes_get_cb(void *data, const char *attr)
{
	Edom_Tag *thiz = data;

	if (!thiz->descriptor) return NULL;
	if (thiz->descriptor->attribute_get)
		return thiz->descriptor->attribute_get(thiz, attr);
	return NULL;
}

static Eina_Bool _parser_default_attribute_set(Edom_Tag *tag, const char *key,
		const char *value)
{
	if (strcmp(key, "id") == 0)
	{
		edom_tag_id_set(tag, value);
	}
	else if (strcmp(key, "class") == 0)
	{
		edom_tag_class_set(tag, value);
	}
	else
	{
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static const char * _parser_default_attribute_get(Edom_Tag *tag, const char *attribute)
{
	return NULL;
}

static const char * _parser_default_name_get(Edom_Tag *tag)
{
	return "default";
}

static Edom_Tag_Descriptor _descriptor = {
	/* .name_get		= */ _parser_default_name_get,
	/* .attribute_set	= */ _parser_default_attribute_set,
	/* .attribute_get	= */ _parser_default_attribute_get,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void edom_tag_dump(Edom_Tag *thiz)
{
	if (!thiz) return;
	_tag_dump(thiz, 0);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Edom_Tag * edom_tag_new(Edom_Context *context, Edom_Tag_Descriptor *descriptor,
		int type,
		Edom_Tag *topmost,
		void *data)
{
	Edom_Tag *thiz;

	thiz = calloc(1, sizeof(Edom_Tag));
	if (!descriptor)
		descriptor = &_descriptor;
	thiz->descriptor = descriptor;
	thiz->type = type;
	thiz->data = data;
	thiz->topmost = topmost;
	thiz->context = context;

	return thiz;
}

EAPI void edom_tag_attributes_from_xml(Edom_Tag *thiz,
		const char *attributes, unsigned int length)
{
	eina_simple_xml_attributes_parse(attributes, length, _attributes_set_cb, thiz);
}

EAPI Eina_Bool edom_tag_attribute_set(Edom_Tag *thiz, const char *key, const char *value)
{
	return  _attributes_set_cb(thiz, key, value);
}

EAPI int edom_tag_type_get(Edom_Tag *thiz)
{
	return thiz->type;
}

/* FIXME the id and class should be treated as attributes using a simple interface */
EAPI const char * edom_tag_id_get(Edom_Tag *thiz)
{
	return thiz->id;
}

EAPI void edom_tag_id_set(Edom_Tag *thiz, const char *id)
{
	thiz->id = strdup(id);
}

EAPI void edom_tag_class_set(Edom_Tag *thiz, const char *class)
{
	thiz->class = strdup(class);
}

EAPI const char * edom_tag_class_get(Edom_Tag *thiz)
{
	return thiz->class;
}

EAPI void * edom_tag_data_get(Edom_Tag *thiz)
{
	return thiz->data;
}

EAPI void edom_tag_child_add(Edom_Tag *thiz, Edom_Tag *child)
{
	Edom_Tag *first_child;
	Eina_Inlist *il;

	if (!thiz->child)
		thiz->child = child;
	else
		eina_inlist_append(EINA_INLIST_GET(thiz->child), EINA_INLIST_GET(child));
	child->parent = thiz;
}

EAPI Edom_Tag * edom_tag_child_get(Edom_Tag *thiz)
{
	return thiz->child;
}

EAPI Edom_Tag * edom_tag_next_get(Edom_Tag *thiz)
{
	Eina_Inlist *il;

	il = EINA_INLIST_GET(thiz);
	return EINA_INLIST_CONTAINER_GET(il->next, Edom_Tag);
}

EAPI Edom_Tag * edom_tag_topmost_get(Edom_Tag *thiz)
{
	return thiz->topmost;
}

EAPI const char * edom_tag_name_get(Edom_Tag *thiz)
{
	if (!thiz->descriptor) return NULL;
	if (!thiz->descriptor->name_get) return NULL;
	return thiz->descriptor->name_get(thiz);
}

EAPI Edom_Context * edom_tag_context_get(Edom_Tag *thiz)
{
	return thiz->context;
}

EAPI Edom_Parser * edom_tag_parser_get(Edom_Tag *thiz)
{
	if (!thiz) return NULL;
	if (!thiz->context) return NULL;
	return edom_context_parser_get(thiz->context);
}
