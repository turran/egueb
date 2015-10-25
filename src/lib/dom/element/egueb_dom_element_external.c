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
#include "egueb_dom_element_external.h"
#include "egueb_dom_element_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_ELEMENT_EXTERNAL_DESCRIPTOR egueb_dom_element_external_descriptor_get()
#define EGUEB_DOM_ELEMENT_EXTERNAL_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 		\
		Egueb_Dom_Element_External_Class, EGUEB_DOM_ELEMENT_EXTERNAL_DESCRIPTOR)
#define EGUEB_DOM_ELEMENT_EXTERNAL_CLASS_GET(o) EGUEB_DOM_ELEMENT_EXTERNAL_CLASS(			\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_ELEMENT_EXTERNAL(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Dom_Element_External, EGUEB_DOM_ELEMENT_EXTERNAL_DESCRIPTOR)

typedef struct _Egueb_Dom_Element_External
{
	Egueb_Dom_Element base;
	Egueb_Dom_Element_External_Descriptor descriptor;
	void *data;
} Egueb_Dom_Element_External;

typedef struct _Egueb_Dom_Element_External_Class
{
	Egueb_Dom_Element_Class base;
} Egueb_Dom_Element_External_Class;


static void _egueb_dom_element_external_init(Egueb_Dom_Element_External *thiz)
{
	if (!thiz->descriptor.init)
		return;
	thiz->descriptor.init(EGUEB_DOM_NODE(thiz), thiz->data);
}
/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_element_external_process(Egueb_Dom_Element *e)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(e);
	if (thiz->descriptor.process)
		return thiz->descriptor.process(EGUEB_DOM_NODE(e), thiz->data);
	return EINA_TRUE;
}

static Egueb_Dom_String * _egueb_dom_element_external_tag_name_get(Egueb_Dom_Element *e)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(e);
	if (thiz->descriptor.tag_name_get)
		return thiz->descriptor.tag_name_get(EGUEB_DOM_NODE(e), thiz->data);
	return NULL;
}

static void _egueb_dom_element_external_clone(Egueb_Dom_Element *e, Egueb_Dom_Element *o)
{
	Egueb_Dom_Element_External *thiz;
	Egueb_Dom_Element_External *other;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(e);
	other = EGUEB_DOM_ELEMENT_EXTERNAL(o);
	other->descriptor = thiz->descriptor;
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_element_external_child_appendable(
		Egueb_Dom_Node *n, Egueb_Dom_Node *child)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(n);
	if (thiz->descriptor.child_appendable)
		return thiz->descriptor.child_appendable(n, thiz->data, child);
	return EINA_TRUE;
}

static Egueb_Dom_Node * _egueb_dom_element_external_ctor(
		Egueb_Dom_Node *n)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(n);
	if (thiz->descriptor.ctor)
		return thiz->descriptor.ctor(thiz->data);
	return NULL;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ELEMENT_DESCRIPTOR,
		Egueb_Dom_Element_External, Egueb_Dom_Element_External_Class,
		egueb_dom_element_external);

static void _egueb_dom_element_external_class_init(void *k)
{
	Egueb_Dom_Element_Class *klass;
	Egueb_Dom_Node_Class *n_klass;

	klass = EGUEB_DOM_ELEMENT_CLASS(k);
	klass->tag_name_get = _egueb_dom_element_external_tag_name_get;
	klass->process = _egueb_dom_element_external_process;
	klass->clone = _egueb_dom_element_external_clone;

	n_klass = EGUEB_DOM_NODE_CLASS(k);
	n_klass->ctor = _egueb_dom_element_external_ctor;
	n_klass->child_appendable = _egueb_dom_element_external_child_appendable;
}

static void _egueb_dom_element_external_instance_init(void *o)
{
}

static void _egueb_dom_element_external_instance_deinit(void *o)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(o);
	/* invalidate the descriptor */
	thiz->descriptor.tag_name_get = NULL;
	if (thiz->descriptor.deinit)
		thiz->descriptor.deinit(EGUEB_DOM_NODE(o), thiz->data);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_dom_element_is_external(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_DOM_ELEMENT_EXTERNAL_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI void * egueb_dom_element_external_data_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(n);
	return thiz->data;
}

EAPI Egueb_Dom_Node * egueb_dom_element_external_new(
		const Egueb_Dom_Element_External_Descriptor *descriptor,
		void *data)
{
	Egueb_Dom_Element_External *thiz;

	if (!descriptor) return NULL;
	if (!descriptor->tag_name_get) return NULL;

	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_element_external);
	thiz->descriptor = *descriptor;
	thiz->data = data;

	/* now initialize it */
	_egueb_dom_element_external_init(thiz);
	return EGUEB_DOM_NODE(thiz);
}
