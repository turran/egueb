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

#ifndef _EGUEB_DOM_ELEMENT_H_
#define _EGUEB_DOM_ELEMENT_H_

#include "egueb_dom_string.h"
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"

EAPI Enesim_Object_Descriptor * egueb_dom_element_descriptor_get(void);

EAPI Egueb_Dom_String * egueb_dom_element_tag_name_get(Egueb_Dom_Node *node);
EAPI Egueb_Dom_String * egueb_dom_element_attribute_get(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name);
EAPI Eina_Bool egueb_dom_element_attribute_set(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_String *value,
		Eina_Error *err);
EAPI Eina_Bool egueb_dom_element_attribute_type_set(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_String *value, Eina_Error *err);

EAPI Egueb_Dom_Node * egueb_dom_element_property_fetch(Egueb_Dom_Node *node,
		const Egueb_Dom_String *name);
EAPI Eina_Error egueb_dom_element_property_set(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, ...);
EAPI Eina_Error egueb_dom_element_property_get(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, ...);

EAPI Eina_Error egueb_dom_element_property_set_va(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, va_list args);
EAPI Eina_Error egueb_dom_element_property_get_va(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, int prop_mask, va_list args);

EAPI Eina_Bool egueb_dom_element_property_value_set(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_Value *v, Eina_Error *err);
EAPI Eina_Bool egueb_dom_element_property_value_get(Egueb_Dom_Node *node,
		Egueb_Dom_String *name, Egueb_Dom_Attr_Type type,
		Egueb_Dom_Value *v, Eina_Error *err);

EAPI Eina_Bool egueb_dom_element_class_property_add(Egueb_Dom_Node *n,
		Egueb_Dom_Node *p, Egueb_Dom_Attr_Fetch fetch, Eina_Error *err);
EAPI Eina_Bool egueb_dom_element_changed(Egueb_Dom_Node *n);
EAPI void egueb_dom_element_request_process(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_dom_element_is_enqueued(Egueb_Dom_Node *n);
EAPI Eina_Bool egueb_dom_element_process(Egueb_Dom_Node *n);

#define EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, prefix, name)		\
	egueb_dom_element_class_property_add(EGUEB_DOM_NODE(thiz),		\
			 egueb_dom_node_ref(thiz->name),			\
			 _##prefix##_##name##_fetch, NULL)

#endif
