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

#ifndef _EGUEB_DOM_ELEMENT_PRIVATE_H_
#define _EGUEB_DOM_ELEMENT_PRIVATE_H_

#include "egueb_dom_node.h"

#include "egueb_dom_node_private.h"

typedef struct _Egueb_Dom_Element
{
	Egueb_Dom_Node base;

	/* instance specific attributes */
	Eina_Hash *attributes;
	/* flag that informs that an inhertiable property has changed */
	Eina_Bool inheritable_changed;
	/* flag that informs that an attribute has changed */
	Eina_Bool attr_changed;

	/* TODO identifier of the last time an element has done the setup */
	int last_run;
	Eina_Bool enqueued;
} Egueb_Dom_Element;

typedef Egueb_Dom_String * (*Egueb_Dom_Element_Tag_Name_Get)(Egueb_Dom_Element *thiz);
typedef Eina_Bool (*Egueb_Dom_Element_Process)(Egueb_Dom_Element *thiz);
typedef void (*Egueb_Dom_Element_Clone)(Egueb_Dom_Element *thiz, Egueb_Dom_Element *other);
typedef Ender_Item * (*Egueb_Dom_Element_Item_Get)(Egueb_Dom_Element *thiz);

typedef struct _Egueb_Dom_Element_Class
{
	Egueb_Dom_Node_Class base;
	/* our own interface */
	Egueb_Dom_Element_Tag_Name_Get tag_name_get;
	Egueb_Dom_Element_Process process;
	Egueb_Dom_Element_Clone clone;
	Egueb_Dom_Element_Item_Get item_get;
} Egueb_Dom_Element_Class;

#define EGUEB_DOM_ELEMENT_DESCRIPTOR egueb_dom_element_descriptor_get()
#define EGUEB_DOM_ELEMENT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,			\
		Egueb_Dom_Element_Class, EGUEB_DOM_ELEMENT_DESCRIPTOR)
#define EGUEB_DOM_ELEMENT_CLASS_GET(o) EGUEB_DOM_ELEMENT_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_ELEMENT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Element, EGUEB_DOM_ELEMENT_DESCRIPTOR)

Enesim_Object_Descriptor * egueb_dom_element_descriptor_get(void);
Eina_Bool egueb_dom_element_process_children(Egueb_Dom_Element *thiz);

#endif
