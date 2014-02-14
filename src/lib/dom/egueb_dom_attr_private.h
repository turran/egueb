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

#ifndef _EGUEB_DOM_ATTR_PRIVATE_H_
#define _EGUEB_DOM_ATTR_PRIVATE_H_

#include "egueb_dom_string.h"
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"

#include "egueb_dom_node_private.h"

#define EGUEB_DOM_ATTR_DESCRIPTOR egueb_dom_attr_descriptor_get()
#define EGUEB_DOM_ATTR_CLASS_GET(o) EGUEB_DOM_ATTR_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_ATTR_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Attr_Class, EGUEB_DOM_ATTR_DESCRIPTOR)
#define EGUEB_DOM_ATTR(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Attr, EGUEB_DOM_ATTR_DESCRIPTOR)

typedef struct _Egueb_Dom_Attr
{
	Egueb_Dom_Node parent;
	Egueb_Dom_String *name;
	int ref;
	/* we keep a pointer to the element owner but not a reference */
	Egueb_Dom_Node *owner;
	/* The flags this property supports: stylable, animatable, inheritable */
	int flag_mask;
	/* Whenever a property type is set and the implementation supports
	 * it, then this mask will keep track of all the properties set.
	 * Whenever a property is unset, the mask will be updated too
	 */
	int set_mask;
	/* In case the property is set with the "inherited" value, this mask
	 * holds such cases
	 */
	int inherited_mask;
	/* Whenever a property is inherited and processed, this pointer
	 * will have the inherited property
	 */
	Egueb_Dom_Node *inherited;
	int changed;
} Egueb_Dom_Attr;

/* This function should set on the value pointer a reference to the internal
 * value of type type. For simple values (i.e values that can fit on the
 * Egueb_Dom_Value struct) the values is got by value. For structs/unions
 * you should get the pointer to such value. For objects?
 */
typedef Eina_Bool (*Egueb_Dom_Attr_Value_Get)(Egueb_Dom_Attr *thiz,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value);
typedef Eina_Bool (*Egueb_Dom_Attr_Value_Set)(Egueb_Dom_Attr *thiz,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value);
typedef const Egueb_Dom_Value_Descriptor * (*Egueb_Dom_Attr_Value_Descriptor_Get)
		(Egueb_Dom_Attr *thiz);
typedef void (*Egueb_Dom_Attr_Clone)(Egueb_Dom_Attr *thiz, Eina_Bool live, Eina_Bool deep, Egueb_Dom_Attr *clone);

typedef struct _Egueb_Dom_Attr_Class
{
	Egueb_Dom_Node_Class parent;
	Egueb_Dom_Attr_Value_Descriptor_Get value_descriptor_get;
	Egueb_Dom_Attr_Value_Get value_get;
	Egueb_Dom_Attr_Value_Set value_set;
	Egueb_Dom_Attr_Clone clone;
} Egueb_Dom_Attr_Class;

Eina_Bool egueb_dom_attr_get_va(Egueb_Dom_Node *n,
		int prop_mask, va_list args);
Eina_Bool egueb_dom_attr_set_va(Egueb_Dom_Node *n,
		int prop_mask, va_list args);
void egueb_dom_attr_copy(Egueb_Dom_Node *n,
		Egueb_Dom_Node *other);
void egueb_dom_attr_process(Egueb_Dom_Node *n);

#endif
