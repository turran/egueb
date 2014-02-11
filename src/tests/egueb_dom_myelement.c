/* A test of the new dom interface, here we will
 * create a new element that inherits from a node
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>

#include "Egueb_Dom.h"
#include "egueb_dom_node_private.h"
#include "egueb_dom_element_private.h"

#include "egueb_dom_myelement.h"

typedef struct _MyElement
{
	Egueb_Dom_Element parent;
	Egueb_Dom_Node *prop1;
} MyElement;

typedef struct _MyElement_Class
{
	Egueb_Dom_Element_Class parent;
} MyElement_Class;

static Eina_Bool _myelement_process(Egueb_Dom_Element *e)
{
	printf("processing %p\n", e);
	return EINA_TRUE;
}

static Egueb_Dom_String * _myelement_tag_name_get(Egueb_Dom_Element *e)
{
	static Egueb_Dom_String *tag_name = NULL;
	if (!tag_name)
	{
		tag_name = egueb_dom_string_new_with_string("myelement");
	}
	return egueb_dom_string_ref(tag_name);
}

ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ELEMENT_DESCRIPTOR,
		MyElement, MyElement_Class, myelement);
#define MYELEMENT_DESCRIPTOR myelement_descriptor_get()
#define MYELEMENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, MyElement_Class, MYELEMENT_DESCRIPTOR)
#define MYELEMENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o, MyElement, MYELEMENT_DESCRIPTOR)

static void _myelement_class_init(void *k)
{
	Egueb_Dom_Element_Class *e_klass;

	printf("[myelement] class init\n");

	e_klass = EGUEB_DOM_ELEMENT_CLASS(k);
	e_klass->process = _myelement_process;
	e_klass->tag_name_get = _myelement_tag_name_get;
}

static void _myelement_instance_init(void *o)
{
	MyElement *thiz;

	printf("[myelement] instance init\n");
	thiz = MYELEMENT(o);
	thiz->prop1 = egueb_dom_attr_string_new(
			egueb_dom_string_new_with_string("prop1"),
			egueb_dom_string_new_with_string("default"),
			EINA_FALSE, EINA_FALSE, EINA_FALSE);
	/* add the class attributes (properties) */
	egueb_dom_element_attribute_add(EGUEB_DOM_NODE(o), egueb_dom_node_ref(thiz->prop1), NULL);
}

static void _myelement_instance_deinit(void *o)
{
	MyElement *thiz;

	printf("[myelement] instance deinit\n");
	thiz = MYELEMENT(o);
	egueb_dom_node_unref(thiz->prop1);
}

static char * _myelement_to_string(Egueb_Dom_Node *n)
{
	Egueb_Dom_Node *doc = NULL;
	Egueb_Dom_String *name;
	Egueb_Dom_Node_Map_Named *attrs = NULL;
	char *str = NULL;
	char *ret = NULL;
	int len;

	name = egueb_dom_element_tag_name_get(n);
	if (asprintf(&str, "<%s", egueb_dom_string_string_get(name)) < 0)
		return NULL;
	/* dump every proeprty */
	attrs = egueb_dom_node_attributes_get(n);
	if (attrs)
	{
		int count;
		int i;

		count = egueb_dom_node_map_named_length(attrs);
		for (i = 0; i < count; i++)
		{
			Egueb_Dom_Node *attr = NULL;
			Egueb_Dom_String *attr_name;
			Egueb_Dom_String *attr_value = NULL;

			attr = egueb_dom_node_map_named_at(attrs, i);
			if (!attr) continue;
			if (!egueb_dom_attr_is_set(attr)) goto no_name;

			attr_name = egueb_dom_attr_name_get(attr);
			if (!attr_name) goto no_name;

			egueb_dom_attr_string_get(attr, EGUEB_DOM_ATTR_TYPE_BASE, &attr_value);
			if (!attr_value) goto no_value;

			if (asprintf(&ret, "%s %s=\"%s\"", str,
					egueb_dom_string_string_get(attr_name),
					egueb_dom_string_string_get(attr_value)) < 0)
				goto no_conversion;

			free(str);
			str = ret;
no_conversion:
			egueb_dom_string_unref(attr_value);
no_value:
			egueb_dom_string_unref(attr_name);
no_name:
			egueb_dom_node_unref(attr);
		}
		egueb_dom_node_map_named_free(attrs);
	}
	/* nor some meta information */
	doc = egueb_dom_node_document_get(n);
	len = asprintf(&ret, "%s> (ref: %d, doc: %p)", str,
			egueb_dom_node_ref_get(n), doc);
	free(str);
	egueb_dom_string_unref(name);
	if (doc) egueb_dom_node_unref(doc);

	if (len)
		return ret;
	return NULL;
}

static void _myelement_dump(Egueb_Dom_Node *thiz, Eina_Bool deep, int level)
{
	Egueb_Dom_Node *child = NULL;
	char *str;
	int i;

	for (i = 0; i < level; i++)
	{
		printf(" ");
	}
	/* print the element */
	str = _myelement_to_string(thiz);
	printf("%s\n", str);
	free(str);

	/* in case of deep, also do the children */
	if (!deep) goto done;

	child = egueb_dom_node_child_first_get(thiz);
	if (!child) goto done;

	do
	{
		Egueb_Dom_Node *sibling = NULL;

		sibling = egueb_dom_node_sibling_next_get(child);
		_myelement_dump(child, deep, level + 1);
		child = sibling;
	} while (child);
done:
	egueb_dom_node_unref(thiz);
}

Egueb_Dom_Node * myelement_new(void)
{
	Egueb_Dom_Node *n;

	n = EGUEB_DOM_NODE(ENESIM_OBJECT_INSTANCE_NEW(myelement));
	return n;
}

void myelement_prop1_set(Egueb_Dom_Node *n, Egueb_Dom_String *s)
{
	MyElement *thiz = MYELEMENT(n);
	egueb_dom_attr_set(thiz->prop1, EGUEB_DOM_ATTR_TYPE_BASE, s, NULL);
}

void myelement_dump(Egueb_Dom_Node *thiz, Eina_Bool deep)
{
	/* ref it because the dump will unref it */
	thiz = egueb_dom_node_ref(thiz);
	_myelement_dump(thiz, deep, 0);
}
