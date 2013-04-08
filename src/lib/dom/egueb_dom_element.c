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
#include "egueb_dom_descriptor.h"
#include "egueb_dom_string.h"
#include "egueb_dom_node_list.h"
#include "egueb_dom_named_node_map.h"
#include "egueb_dom_node.h"
#include "egueb_dom_element.h"

#include "egueb_dom_node_private.h"
#include "egueb_dom_element_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
EGUEB_DOM_NODE_ABSTRACT_BOILERPLATE(EGUEB_DOM_NODE_DESCRIPTOR,
		Egueb_Dom_Element, Egueb_Dom_Element_Class, EDOM_NAMESPACE,
		egueb_dom_element);

static void _egueb_dom_element_class_init(void *k)
{
	Egueb_Dom_Node_Class *klass = EGUEB_DOM_NODE_CLASS(k);

	klass->type = EGUEB_DOM_NODE_TYPE_ELEMENT_NODE;
	printf("class init\n");
}

static void _egueb_dom_element_descriptor_init(Ender_Descriptor *d)
{
	printf("descriptor init\n");
}

static void _egueb_dom_element_instance_init(void *o)
{
	printf("instance init\n");
}

static void _egueb_dom_element_instance_deinit(void *o)
{
	printf("instance deinit\n");
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/*
 * readonly attribute DOMString       tagName;
 */
EAPI Eina_Error egueb_dom_element_tag_name_get(Egueb_Dom_Node *node, Egueb_Dom_String **name)
{

}

/*
 * DOMString          getAttribute(in DOMString name);
 */
EAPI Eina_Error egueb_dom_element_attribute_get(Egueb_Dom_Node *node, Egueb_Dom_String *name,
		Egueb_Dom_String **value)
{

}

/*
 * void               setAttribute(in DOMString name, in DOMString value)
 * raises(DOMException);
 */
EAPI Eina_Error egueb_dom_element_attribute_set(Egueb_Dom_Node *node, Egueb_Dom_String *name,
		Egueb_Dom_String *value)
{

}

#if 0
  void               removeAttribute(in DOMString name)
                                        raises(DOMException);
  Attr               getAttributeNode(in DOMString name);
  Attr               setAttributeNode(in Attr newAttr)
                                        raises(DOMException);
  Attr               removeAttributeNode(in Attr oldAttr)
                                        raises(DOMException);
  NodeList           getElementsByTagName(in DOMString name);
  // Introduced in DOM Level 2:
  DOMString          getAttributeNS(in DOMString namespaceURI, 
                                    in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  void               setAttributeNS(in DOMString namespaceURI, 
                                    in DOMString qualifiedName, 
                                    in DOMString value)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  void               removeAttributeNS(in DOMString namespaceURI, 
                                       in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  Attr               getAttributeNodeNS(in DOMString namespaceURI, 
                                        in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  Attr               setAttributeNodeNS(in Attr newAttr)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  NodeList           getElementsByTagNameNS(in DOMString namespaceURI, 
                                            in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 2:
  boolean            hasAttribute(in DOMString name);
  // Introduced in DOM Level 2:
  boolean            hasAttributeNS(in DOMString namespaceURI, 
                                    in DOMString localName)
                                        raises(DOMException);
  // Introduced in DOM Level 3:
  readonly attribute TypeInfo        schemaTypeInfo;
  // Introduced in DOM Level 3:
  void               setIdAttribute(in DOMString name, 
                                    in boolean isId)
                                        raises(DOMException);
  // Introduced in DOM Level 3:
  void               setIdAttributeNS(in DOMString namespaceURI, 
                                      in DOMString localName, 
                                      in boolean isId)
                                        raises(DOMException);
  // Introduced in DOM Level 3:
  void               setIdAttributeNode(in Attr idAttr, 
                                        in boolean isId)
                                        raises(DOMException);
};
#endif
