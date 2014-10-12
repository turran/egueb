/* Egueb_Xlink - XLINK
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
#include "egueb_xlink_main_private.h"
#include "egueb_xlink_attr_href.h"
#include "egueb_xlink_document.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_object_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_XLINK_ATTR_HREF_DESCRIPTOR 					\
		egueb_xlink_attr_href_descriptor_get()
#define EGUEB_XLINK_ATTR_HREF(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Egueb_Xlink_Attr_Xlink_Href, EGUEB_XLINK_ATTR_HREF_DESCRIPTOR)

typedef struct _Egueb_Xlink_Attr_Xlink_Href
{
	Egueb_Dom_Attr_Object base;
	Egueb_Dom_String *anim;
	Egueb_Dom_String *value;
	Egueb_Dom_String *def;

	Egueb_Dom_String *last;
	Egueb_Dom_Node *node;
} Egueb_Xlink_Attr_Xlink_Href;

typedef struct _Egueb_Xlink_Attr_Xlink_Href_Class
{
	Egueb_Dom_Attr_Object_Class base;
} Egueb_Xlink_Attr_Xlink_Href_Class;

/* any change on the xlink href element also trigger a process here */
static void _egueb_xlink_attr_href_node_request_cb(Egueb_Dom_Event *e,
		void *user_data)
{
	Egueb_Dom_Attr *attr = user_data;

	ERR("The xlink:href requested a process, let's request ourselves too");
	/* request a process on the owner of the attribute */
	egueb_dom_element_request_process(attr->owner);
}

static void _egueb_xlink_attr_href_cleanup(Egueb_Xlink_Attr_Xlink_Href *thiz)
{
	if (thiz->node)
	{
		egueb_dom_node_event_listener_remove(thiz->node,
				EGUEB_DOM_EVENT_PROCESS,
				_egueb_xlink_attr_href_node_request_cb,
				EINA_FALSE, thiz);
		egueb_dom_node_unref(thiz->node);
		thiz->node = NULL;
	}
}
/*----------------------------------------------------------------------------*
 *                            Attr object interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_xlink_attr_href_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void ***o)
{
	Egueb_Xlink_Attr_Xlink_Href *thiz;

	thiz = EGUEB_XLINK_ATTR_HREF(p);
	switch (type)
	{
		case EGUEB_DOM_ATTR_TYPE_ANIMATED:
		*o = (void **)&thiz->anim;
		break;

		case EGUEB_DOM_ATTR_TYPE_BASE:
		*o = (void **)&thiz->value;
		break;

		case EGUEB_DOM_ATTR_TYPE_DEFAULT:
		*o = (void **)&thiz->def;
		break;

		default:
		return EINA_FALSE;
		break;
	}
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                               Attr interface                               *
 *----------------------------------------------------------------------------*/
static void _egueb_xlink_attr_href_clone(Egueb_Dom_Attr *a, Eina_Bool live,
		Eina_Bool deep, Egueb_Dom_Attr *cloned)
{

}

static const Egueb_Dom_Value_Descriptor *
_egueb_xlink_attr_href_value_descriptor_get(Egueb_Dom_Attr *p)
{
	return egueb_dom_value_dom_string_descriptor_get();
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_OBJECT_DESCRIPTOR,
		Egueb_Xlink_Attr_Xlink_Href, Egueb_Xlink_Attr_Xlink_Href_Class,
		egueb_xlink_attr_href)

static void _egueb_xlink_attr_href_class_init(void *k)
{
	Egueb_Dom_Attr_Class *p_klass;
	Egueb_Dom_Attr_Object_Class *o_klass;

	p_klass = EGUEB_DOM_ATTR_CLASS(k);
	p_klass->value_descriptor_get = _egueb_xlink_attr_href_value_descriptor_get;
	p_klass->clone = _egueb_xlink_attr_href_clone;

	o_klass = EGUEB_DOM_ATTR_OBJECT_CLASS(k);
	o_klass->value_get = _egueb_xlink_attr_href_value_get;
}

static void _egueb_xlink_attr_href_instance_init(void *o)
{
}

static void _egueb_xlink_attr_href_instance_deinit(void *o)
{
	Egueb_Xlink_Attr_Xlink_Href *thiz;

	thiz = EGUEB_XLINK_ATTR_HREF(o);
	if (thiz->def)
		egueb_dom_string_unref(thiz->def);
	if (thiz->value)
		egueb_dom_string_unref(thiz->value);
	if (thiz->anim)
		egueb_dom_string_unref(thiz->anim);

	if (thiz->last)
		egueb_dom_string_unref(thiz->last);
	_egueb_xlink_attr_href_cleanup(thiz);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_xlink_attr_href_new(Egueb_Dom_String *name,
		Egueb_Dom_String *def)
{
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_xlink_attr_href);
	egueb_dom_attr_init(n, name, egueb_dom_string_ref(EGUEB_DOM_NAME_NS_XMLNS),
			EINA_TRUE, EINA_TRUE, EINA_TRUE);
	if (def) egueb_dom_attr_set(n, EGUEB_DOM_ATTR_TYPE_DEFAULT, def);
	return n;
}

EAPI Eina_Bool egueb_xlink_attr_href_resolve(Egueb_Dom_Node *attr)
{
	Egueb_Xlink_Attr_Xlink_Href *thiz;
	Egueb_Dom_String *str = NULL;
	Eina_Bool ret = EINA_TRUE;

	thiz = EGUEB_XLINK_ATTR_HREF(attr);
	egueb_dom_attr_final_get(attr, &str);
	if (!egueb_dom_string_is_equal(str, thiz->last))
	{
		_egueb_xlink_attr_href_cleanup(thiz);
		if (str)
		{
			Egueb_Dom_Attr *a;
			Egueb_Dom_Node *doc = NULL;

			a = EGUEB_DOM_ATTR(attr);
			/* TODO is the document also set on the attr? */
			doc = egueb_dom_node_owner_document_get(a->owner);
			if (!doc)
			{
				WARN("No document set");
				ret = EINA_FALSE;
				goto no_doc;
			}
			thiz->node = egueb_xlink_document_element_get_by_iri(doc, str);
			if (thiz->node)
			{
				egueb_dom_node_event_listener_add(thiz->node,
						EGUEB_DOM_EVENT_PROCESS,
						_egueb_xlink_attr_href_node_request_cb,
						EINA_FALSE, thiz);
			}
			egueb_dom_node_unref(doc);
		}
	}
no_doc:
	/* swap the xlink:href */
	if (thiz->last)
	{
		egueb_dom_string_unref(thiz->last);
		thiz->last = NULL;
	}
	thiz->last = str;
	return ret;
}

EAPI void egueb_xlink_attr_href_node_get(Egueb_Dom_Node *attr, Egueb_Dom_Node **n)
{
	Egueb_Xlink_Attr_Xlink_Href *thiz;

	thiz = EGUEB_XLINK_ATTR_HREF(attr);
	egueb_xlink_attr_href_resolve(attr);
	if (thiz->node)
		*n = egueb_dom_node_ref(thiz->node);
	else
		*n = NULL;
}

