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

#include "egueb_dom_string.h"
#include "egueb_dom_main.h"
#include "egueb_dom_element.h"
#include "egueb_dom_attr_external.h"
#include "egueb_dom_attr_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_ATTR_EXTERNAL_DESCRIPTOR egueb_dom_attr_external_descriptor_get()
#define EGUEB_DOM_ATTR_EXTERNAL_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Dom_Attr_External_Class, EGUEB_DOM_ATTR_EXTERNAL_DESCRIPTOR)
#define EGUEB_DOM_ATTR_EXTERNAL(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Dom_Attr_External, EGUEB_DOM_ATTR_EXTERNAL_DESCRIPTOR)

typedef struct _Egueb_Dom_Attr_External
{
	Egueb_Dom_Attr base;
	const Egueb_Dom_Attr_External_Descriptor *descriptor;
	void *data;
} Egueb_Dom_Attr_External;

typedef struct _Egueb_Dom_Attr_External_Class
{
	Egueb_Dom_Attr_Class base;
} Egueb_Dom_Attr_External_Class;

static void * _egueb_dom_attr_external_init(Egueb_Dom_Attr_External *thiz)
{
	if (!thiz->descriptor)
		return NULL;
	if (!thiz->descriptor->init)
		return NULL;
	return 	thiz->descriptor->init(EGUEB_DOM_NODE(thiz));
}
/*----------------------------------------------------------------------------*
 *                               Attr interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_attr_external_value_get(Egueb_Dom_Attr *a,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Egueb_Dom_Attr_External *thiz;

	thiz = EGUEB_DOM_ATTR_EXTERNAL(a);
	if (thiz->descriptor->value_get)
		return thiz->descriptor->value_get(EGUEB_DOM_NODE(a),
			thiz->data, type, value);
	return EINA_FALSE;
}

static Eina_Bool _egueb_dom_attr_external_value_set(Egueb_Dom_Attr *a,
		Egueb_Dom_Attr_Type type, Egueb_Dom_Value *value)
{
	Egueb_Dom_Attr_External *thiz;

	thiz = EGUEB_DOM_ATTR_EXTERNAL(a);
	if (thiz->descriptor->value_set)
		return thiz->descriptor->value_set(EGUEB_DOM_NODE(a),
			thiz->data, type, value);
	return EINA_FALSE;
}

static const Egueb_Dom_Value_Descriptor *
_egueb_dom_attr_external_value_descriptor_get(Egueb_Dom_Attr *a)
{
	Egueb_Dom_Attr_External *thiz;

	thiz = EGUEB_DOM_ATTR_EXTERNAL(a);
	if (thiz->descriptor->value_descriptor_get)
		return thiz->descriptor->value_descriptor_get(EGUEB_DOM_NODE(a),
			thiz->data);
	return NULL;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_DESCRIPTOR,
		Egueb_Dom_Attr_External, Egueb_Dom_Attr_External_Class,
		egueb_dom_attr_external);

static void _egueb_dom_attr_external_class_init(void *k)
{
	Egueb_Dom_Attr_Class *klass;

	klass = EGUEB_DOM_ATTR_CLASS(k);
	klass->value_get = _egueb_dom_attr_external_value_get;
	klass->value_set = _egueb_dom_attr_external_value_set;
	klass->value_descriptor_get = _egueb_dom_attr_external_value_descriptor_get;
}

static void _egueb_dom_attr_external_instance_init(void *o)
{
}

static void _egueb_dom_attr_external_instance_deinit(void *o)
{
	Egueb_Dom_Attr_External *thiz;

	thiz = EGUEB_DOM_ATTR_EXTERNAL(o);
	if (thiz->descriptor->deinit)
		thiz->descriptor->deinit(EGUEB_DOM_NODE(o), thiz->data);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void * egueb_dom_attr_external_data_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Attr_External *thiz;

	thiz = EGUEB_DOM_ATTR_EXTERNAL(n);
	return thiz->data;
}

EAPI Egueb_Dom_Node * egueb_dom_attr_external_new(
		const Egueb_Dom_Attr_External_Descriptor *descriptor)
{
	Egueb_Dom_Attr_External *thiz;

	if (!descriptor) return NULL;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_attr_external);
	thiz->descriptor = descriptor;

	/* now initialize it */
	thiz->data = _egueb_dom_attr_external_init(thiz);
	return EGUEB_DOM_NODE(thiz);
}

