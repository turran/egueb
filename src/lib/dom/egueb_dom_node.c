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

#include "egueb_dom_main.h"
#include "egueb_dom_string.h"
#include "egueb_dom_descriptor.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_named_node_map.h"
#include "egueb_dom_node.h"

#include "egueb_dom_node_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _egueb_dom_node_class_init(void *k)
{

}

static void _egueb_dom_node_descriptor_init(Ender_Descriptor *d)
{

}


static void _egueb_dom_node_instance_init(void *o)
{

}

static void _egueb_dom_node_instance_deinit(void *o)
{

}

static Ender_Descriptor * _egueb_dom_node_ender_descriptor_get(void)
{
	static Ender_Descriptor *d = NULL;
	if (!d)
	{
		Ender_Descriptor *ed;

		d = ender_namespace_descriptor_add(EDOM_NAMESPACE, "node", 
				NULL,
				ENDER_DESTRUCTOR(egueb_dom_node_unref),
				ed, ENDER_TYPE_ABSTRACT, -1);
		_egueb_dom_node_descriptor_init(d);
	}
	return d;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Enesim_Object_Descriptor * egueb_dom_node_descriptor_get(void)
{
	static Enesim_Object_Descriptor *d = NULL;
	if (!d)	
	{
		Egueb_Dom_Descriptor *edd;

		d = enesim_object_descriptor_new(ENESIM_OBJECT_DESCRIPTOR,
			sizeof(Egueb_Dom_Node_Class), _egueb_dom_node_class_init,
			sizeof(Egueb_Dom_Node), _egueb_dom_node_instance_init,
			_egueb_dom_node_instance_deinit, "node");

		edd = egueb_dom_descriptor_new();
		egueb_dom_descriptor_ender_set(edd,
				_egueb_dom_node_ender_descriptor_get());
		enesim_object_descriptor_private_set(d, edd);
		egueb_dom_descriptor_enesim_set(edd, d);
	}
	return d;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_dom_node_unref(Egueb_Dom_Node *thiz)
{

}
/*
 * readonly attribute DOMString nodeName;
 */
EAPI Eina_Error egueb_dom_node_name_get(Egueb_Dom_Node *thiz, Egueb_Dom_String **name)
{

}

/* attribute DOMString nodeValue;
 * raises(DOMException) on setting
 * raises(DOMException) on retrieval
 */
EAPI Eina_Error egueb_dom_node_value_get(Egueb_Dom_Node *thiz, Egueb_Dom_String **value)
{

}

/*
 * readonly attribute unsigned short nodeType;
 */
EAPI Eina_Error egueb_dom_node_type_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node_Type *type)
{

}

/*
 * readonly attribute Node parentNode;
 */
EAPI Eina_Error egueb_dom_node_parent_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **parent)
{

}

/* 
 * readonly attribute NodeList childNodes;
 */
EAPI Eina_Error egueb_dom_node_children_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node_List *children)
{

}

/*
 * readonly attribute Node firstChild;
 */
EAPI Eina_Error egueb_dom_node_child_first_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **last)
{

}

/*
 * readonly attribute Node lastChild;
 */
EAPI Eina_Error egueb_dom_node_child_last_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **last)
{

}

/*
 * readonly attribute Node previousSibling;
 */
EAPI Eina_Error egueb_dom_node_sibling_previous_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **sibling)
{

}

/*
 * readonly attribute Node nextSibling;
 */
EAPI Eina_Error egueb_dom_node_sibling_next_get(Egueb_Dom_Node *thiz, Egueb_Dom_Node **sibling)
{

}

/*
 * readonly attribute NamedNodeMap attributes;
 */
EAPI Eina_Error egueb_dom_node_attributes_get(Egueb_Dom_Node *thiz, Egueb_Dom_Named_Node_Map *map)
{

}


#if 0
  // Modified in DOM Level 2:
  readonly attribute Document        ownerDocument;
  // Modified in DOM Level 3:
  Node               insertBefore(in Node newChild, 
                                  in Node refChild)
                                        raises(DOMException);
  // Modified in DOM Level 3:
  Node               replaceChild(in Node newChild, 
                                  in Node oldChild)
                                        raises(DOMException);
  // Modified in DOM Level 3:
  Node               removeChild(in Node oldChild)
                                        raises(DOMException);
  // Modified in DOM Level 3:
  Node               appendChild(in Node newChild)
                                        raises(DOMException);
  boolean            hasChildNodes();
  Node               cloneNode(in boolean deep);
  // Modified in DOM Level 3:
  void               normalize();
  // Introduced in DOM Level 2:
  boolean            isSupported(in DOMString feature, 
                                 in DOMString version);
  // Introduced in DOM Level 2:
  readonly attribute DOMString       namespaceURI;
  // Introduced in DOM Level 2:
           attribute DOMString       prefix;
                                        // raises(DOMException) on setting

  // Introduced in DOM Level 2:
  readonly attribute DOMString       localName;
  // Introduced in DOM Level 2:
  boolean            hasAttributes();
  // Introduced in DOM Level 3:
  readonly attribute DOMString       baseURI;

  // DocumentPosition
  const unsigned short      DOCUMENT_POSITION_DISCONNECTED = 0x01;
  const unsigned short      DOCUMENT_POSITION_PRECEDING    = 0x02;
  const unsigned short      DOCUMENT_POSITION_FOLLOWING    = 0x04;
  const unsigned short      DOCUMENT_POSITION_CONTAINS     = 0x08;
  const unsigned short      DOCUMENT_POSITION_CONTAINED_BY = 0x10;
  const unsigned short      DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC = 0x20;

  // Introduced in DOM Level 3:
  unsigned short     compareDocumentPosition(in Node other)
                                        raises(DOMException);
  // Introduced in DOM Level 3:
           attribute DOMString       textContent;
                                        // raises(DOMException) on setting
                                        // raises(DOMException) on retrieval

  // Introduced in DOM Level 3:
  boolean            isSameNode(in Node other);
  // Introduced in DOM Level 3:
  DOMString          lookupPrefix(in DOMString namespaceURI);
  // Introduced in DOM Level 3:
  boolean            isDefaultNamespace(in DOMString namespaceURI);
  // Introduced in DOM Level 3:
  DOMString          lookupNamespaceURI(in DOMString prefix);
  // Introduced in DOM Level 3:
  boolean            isEqualNode(in Node arg);
  // Introduced in DOM Level 3:
  DOMObject          getFeature(in DOMString feature, 
                                in DOMString version);
  // Introduced in DOM Level 3:
  DOMUserData        setUserData(in DOMString key, 
                                 in DOMUserData data, 
                                 in UserDataHandler handler);
  // Introduced in DOM Level 3:
  DOMUserData        getUserData(in DOMString key);

#endif
