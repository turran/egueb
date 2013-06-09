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
#ifndef _EGUEB_DOM_ATTR_PRIMITIVE_PRIVATE_H_
#define _EGUEB_DOM_ATTR_PRIMITIVE_PRIVATE_H_

#include "egueb_dom_string.h"
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"
#include "egueb_dom_attr_private.h"

#define EGUEB_DOM_ATTR_PRIMITIVE_DESCRIPTOR egueb_dom_attr_primitive_descriptor_get()
#define EGUEB_DOM_ATTR_PRIMITIVE_CLASS_GET(o) EGUEB_DOM_ATTR_PRIMITIVE_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_ATTR_PRIMITIVE_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Attr_Primitive_Class, EGUEB_DOM_ATTR_PRIMITIVE_DESCRIPTOR)
#define EGUEB_DOM_ATTR_PRIMITIVE(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Attr, EGUEB_DOM_ATTR_PRIMITIVE_DESCRIPTOR)

typedef struct _Egueb_Dom_Attr_Primitive
{
	Egueb_Dom_Attr parent;
} Egueb_Dom_Attr_Primitive;

typedef Eina_Bool (*Egueb_Dom_Attr_Primitive_Value_Get)(Egueb_Dom_Attr *thiz,
		Egueb_Dom_Attr_Type type, void **o);
typedef Egueb_Dom_Value_Descriptor * (*Egueb_Dom_Attr_Primitive_Value_Descriptor_Get)
		(Egueb_Dom_Attr *thiz);

typedef struct _Egueb_Dom_Attr_Primitive_Class
{
	Egueb_Dom_Attr_Class parent;
	Egueb_Dom_Attr_Primitive_Value_Descriptor_Get value_descriptor_get;
	Egueb_Dom_Attr_Primitive_Value_Get value_get;
} Egueb_Dom_Attr_Primitive_Class;

Enesim_Object_Descriptor * egueb_dom_attr_primitive_descriptor_get(void);
Eina_Bool egueb_dom_attr_primitive_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void ***o);
void egueb_dom_attr_primitive_unref(Egueb_Dom_Attr *p,
		void *o);
void * egueb_dom_attr_primitive_ref(Egueb_Dom_Attr *p,
		void *o);

#endif
