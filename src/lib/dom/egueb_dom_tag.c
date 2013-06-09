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

#include "egueb_dom_private.h"

#include "Egueb_Dom.h"
/* TODO
 * + Add a function to iterate over the childs
 * + Add a descriptor function to iterate over the attributes
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* TODO this should go to eina */
#define EINA_INLIST_REVERSE_FOREACH_SAFE(list, list2, l) \
   for (l = (list ? _EINA_INLIST_CONTAINER(l, list->last) : NULL), list2 = l ? ((EINA_INLIST_GET(l) ? EINA_INLIST_GET(l)->prev : NULL)) : NULL; \
        l; \
        l = _EINA_INLIST_CONTAINER(l, list2), list2 = list2 ? list2->prev : NULL)



struct _Egueb_Dom_Tag
{
	EINA_INLIST;
	Egueb_Dom_Tag_Descriptor descriptor;
	Egueb_Dom_Tag *parent;
	Eina_Inlist *children;
	void *data;
};

static void _tag_dump(Egueb_Dom_Tag *thiz, int level)
{
	Egueb_Dom_Tag *child;
	const char *name;
	int i;

	for (i = 0; i < level; i++)
		printf(" ");
	name = egueb_dom_tag_name_get(thiz);
	printf("%s\n", name ? name : "(UNKNOWN)");
	EINA_INLIST_FOREACH(thiz->children, child)
	{
		_tag_dump(child, level + 1);
	}
}

static Eina_Bool _attribute_set(void *data, const char *key,
		const char *value)
{
	Egueb_Dom_Tag *thiz = data;

	if (thiz->descriptor.attribute_set)
		return thiz->descriptor.attribute_set(thiz, key, value);
	return EINA_FALSE;
}

static Eina_Bool _attribute_get(void *data, const char *key, char **value)
{
	Egueb_Dom_Tag *thiz = data;

	if (thiz->descriptor.attribute_get)
		return thiz->descriptor.attribute_get(thiz, key, value);
	return EINA_FALSE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_dom_tag_dump(Egueb_Dom_Tag *thiz)
{
	if (!thiz) return;
	_tag_dump(thiz, 0);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Egueb_Dom_Tag * egueb_dom_tag_new(Egueb_Dom_Tag_Descriptor *descriptor,
		void *data)
{
	Egueb_Dom_Tag *thiz;

	if (!descriptor)
		return NULL;

	thiz = calloc(1, sizeof(Egueb_Dom_Tag));
	thiz->descriptor = *descriptor;
	thiz->data = data;

	return thiz;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_delete(Egueb_Dom_Tag *thiz)
{
	if (thiz->descriptor.free)
		thiz->descriptor.free(thiz);
	free(thiz);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_attributes_from_xml(Egueb_Dom_Tag *thiz, const char *attributes,
		unsigned int length)
{
	eina_simple_xml_attributes_parse(attributes, length, _attribute_set, thiz);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_attribute_set(Egueb_Dom_Tag *thiz, const char *name,
		const char *value)
{
	_attribute_set(thiz, name, value);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI char * egueb_dom_tag_attribute_get(Egueb_Dom_Tag *thiz, const char *name)
{
	char *value;
	_attribute_get(thiz, name, &value);
	return value;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI char * egueb_dom_tag_id_get(Egueb_Dom_Tag *thiz)
{
	return egueb_dom_tag_attribute_get(thiz, "id");
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_id_set(Egueb_Dom_Tag *thiz, const char *id)
{
	egueb_dom_tag_attribute_set(thiz, "id", id);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI char * egueb_dom_tag_class_get(Egueb_Dom_Tag *thiz)
{
	return egueb_dom_tag_attribute_get(thiz, "class");
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_class_set(Egueb_Dom_Tag *thiz, const char *class)
{
	egueb_dom_tag_attribute_set(thiz, "class", class);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void * egueb_dom_tag_data_get(Egueb_Dom_Tag *thiz)
{
	return thiz->data;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_dom_tag_child_add(Egueb_Dom_Tag *thiz, Egueb_Dom_Tag *child)
{
	Eina_Bool ret = EINA_TRUE;

	if (!child) return EINA_FALSE;

	if (thiz->descriptor.child_add)
		ret = thiz->descriptor.child_add(thiz, child);
	if (ret)
	{
		thiz->children = eina_inlist_append(thiz->children, EINA_INLIST_GET(child));
		child->parent = thiz;
	}

	return ret;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool egueb_dom_tag_child_remove(Egueb_Dom_Tag *thiz, Egueb_Dom_Tag *child)
{
	Eina_Bool ret = EINA_TRUE;

	if (!child) return EINA_FALSE;
	if (child->parent != thiz) return EINA_FALSE;

	if (thiz->descriptor.child_remove)
		ret = thiz->descriptor.child_remove(thiz, child);
	if (ret)
	{
		thiz->children = eina_inlist_remove(thiz->children, EINA_INLIST_GET(child));
		child->parent = NULL;
	}
	return ret;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Egueb_Dom_Tag * egueb_dom_tag_child_get(Egueb_Dom_Tag *thiz)
{
	return EINA_INLIST_CONTAINER_GET(thiz->children, Egueb_Dom_Tag);
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_child_foreach(Egueb_Dom_Tag *thiz, Egueb_Dom_Tag_Foreach foreach, void *data)
{
	Egueb_Dom_Tag *child;
	Eina_Inlist *tmp;

	if (!foreach) return;

	EINA_INLIST_FOREACH_SAFE(thiz->children, tmp, child)
	{
		if (!foreach(thiz, child, data))
			break;
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_child_reverse_foreach(Egueb_Dom_Tag *thiz, Egueb_Dom_Tag_Foreach foreach, void *data)
{
	Egueb_Dom_Tag *child;
	Eina_Inlist *tmp;

	if (!foreach) return;

	EINA_INLIST_REVERSE_FOREACH_SAFE(thiz->children, tmp, child)
	{
		if (!foreach(thiz, child, data))
			break;
	}
}


/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Egueb_Dom_Tag * egueb_dom_tag_next_get(Egueb_Dom_Tag *thiz)
{
	Eina_Inlist *il;

	il = EINA_INLIST_GET(thiz);
	return EINA_INLIST_CONTAINER_GET(il->next, Egueb_Dom_Tag);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Egueb_Dom_Tag * egueb_dom_tag_topmost_get(Egueb_Dom_Tag *thiz)
{
	if (thiz->descriptor.topmost_get)
		return thiz->descriptor.topmost_get(thiz);
	return NULL;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Egueb_Dom_Tag * egueb_dom_tag_parent_get(Egueb_Dom_Tag *thiz)
{
	return thiz->parent;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI const char * egueb_dom_tag_name_get(Egueb_Dom_Tag *thiz)
{
	if (!thiz->descriptor.name_get) return NULL;
	return thiz->descriptor.name_get(thiz);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_cdata_set(Egueb_Dom_Tag *thiz, Egueb_Dom_String *string)
{
	int length;

	if (!thiz->descriptor.cdata_set) return;
	length = string->length;
	if (!length)
	{
		if (string->s)
			length = strlen(string->s);
	}
	thiz->descriptor.cdata_set(thiz, string->s, length);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_text_set(Egueb_Dom_Tag *thiz, Egueb_Dom_String *string)
{
	int length;

	if (!thiz->descriptor.text_set) return;
	length = string->length;
	if (!length)
	{
		if (string->s)
			length = strlen(string->s);
	}
	thiz->descriptor.text_set(thiz, string->s, length);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void egueb_dom_tag_text_get(Egueb_Dom_Tag *thiz, Egueb_Dom_String *string)
{
	if (!thiz->descriptor.text_get) return;
	thiz->descriptor.text_get(thiz, &string->s, &string->length);
}
