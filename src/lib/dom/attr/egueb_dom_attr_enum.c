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
#include "egueb_dom_attr.h"

#include "egueb_dom_attr_private.h"
#include "egueb_dom_attr_basic_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_ATTR_ENUM_DESCRIPTOR egueb_dom_attr_enum_descriptor_get()
#define EGUEB_DOM_ATTR_ENUM(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Attr_Enum, EGUEB_DOM_ATTR_ENUM_DESCRIPTOR)

typedef struct _Egueb_Dom_Attr_Enum
{
	Egueb_Dom_Attr_Basic parent;
	const Egueb_Dom_Value_Descriptor *d;
	int styled;
	int anim;
	int base;
	int def;
} Egueb_Dom_Attr_Enum;

typedef struct _Egueb_Dom_Attr_Enum_Class
{
	Egueb_Dom_Attr_Basic_Class parent;
} Egueb_Dom_Attr_Enum_Class;

static Eina_Bool _egueb_dom_attr_enum_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void **o)
	
{
	Egueb_Dom_Attr_Enum *thiz;

	thiz = EGUEB_DOM_ATTR_ENUM(p);
	switch (type)
	{
		case EGUEB_DOM_ATTR_TYPE_ANIMATED:
		*o = &thiz->anim;
		break;

		case EGUEB_DOM_ATTR_TYPE_STYLED:
		*o = &thiz->styled;
		break;

		case EGUEB_DOM_ATTR_TYPE_BASE:
		*o = &thiz->base;
		break;

		case EGUEB_DOM_ATTR_TYPE_DEFAULT:
		*o = &thiz->def;
		break;

		default:
		return EINA_FALSE;
		break;
	}
	return EINA_TRUE;
}

static const Egueb_Dom_Value_Descriptor *
_egueb_dom_attr_enum_value_descriptor_get(Egueb_Dom_Attr *p)
{
	Egueb_Dom_Attr_Enum *thiz;

	thiz = EGUEB_DOM_ATTR_ENUM(p);
	return thiz->d;
}

/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_BASIC_DESCRIPTOR,
		Egueb_Dom_Attr_Enum, Egueb_Dom_Attr_Enum, egueb_dom_attr_enum)

static void _egueb_dom_attr_enum_class_init(void *k)
{
	Egueb_Dom_Attr_Class *p_klass;
	Egueb_Dom_Attr_Basic_Class *o_klass;

	p_klass = EGUEB_DOM_ATTR_CLASS(k);
	p_klass->value_descriptor_get = _egueb_dom_attr_enum_value_descriptor_get;
	o_klass = EGUEB_DOM_ATTR_BASIC_CLASS(k);
	o_klass->value_get = _egueb_dom_attr_enum_value_get;
}

static void _egueb_dom_attr_enum_instance_init(void *o)
{
}

static void _egueb_dom_attr_enum_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Node * egueb_dom_attr_enum_new(Egueb_Dom_String *name,
		Eina_Bool animatable, Eina_Bool stylable,
		Eina_Bool inheritable, const Egueb_Dom_Value_Descriptor *d)
{
	Egueb_Dom_Attr_Enum *thiz;
	Egueb_Dom_Node *n;

	n = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_attr_enum);
	/* set our own parameters */
	thiz = EGUEB_DOM_ATTR_ENUM(n);
	thiz->d = d;

	egueb_dom_attr_init(n, name, NULL, animatable, stylable, inheritable);
	return n;
}
