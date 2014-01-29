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
#define EGUEB_DOM_ELEMENT_EXTERNAL(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Dom_Element_External, EGUEB_DOM_ELEMENT_EXTERNAL_DESCRIPTOR)

typedef struct _Egueb_Dom_Element_External
{
	Egueb_Dom_Element base;
	const Egueb_Dom_Element_External_Descriptor *descriptor;
	void *data;
} Egueb_Dom_Element_External;

typedef struct _Egueb_Dom_Element_External_Class
{
	Egueb_Dom_Element_Class base;
} Egueb_Dom_Element_External_Class;

/*----------------------------------------------------------------------------*
 *                             Element interface                              *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_element_external_process(Egueb_Dom_Element *e)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(e);
	if (thiz->descriptor->process)
		return thiz->descriptor->process(EGUEB_DOM_NODE(e), thiz->data);
	return EINA_TRUE;
}

static Egueb_Dom_String * _egueb_dom_element_external_tag_name_get(Egueb_Dom_Element *e)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(e);
	return thiz->descriptor->tag_name_get(EGUEB_DOM_NODE(e), thiz->data);
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_dom_element_external_child_appendable(
		Egueb_Dom_Node *n, Egueb_Dom_Node *child)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(n);
	if (thiz->descriptor->child_appendable)
		return thiz->descriptor->child_appendable(n, child);
	return EINA_TRUE;
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

	n_klass = EGUEB_DOM_NODE_CLASS(k);
	n_klass->child_appendable = _egueb_dom_element_external_child_appendable;
}

static void _egueb_dom_element_external_instance_init(void *o)
{
}

static void _egueb_dom_element_external_instance_deinit(void *o)
{
	Egueb_Dom_Element_External *thiz;

	thiz = EGUEB_DOM_ELEMENT_EXTERNAL(o);
	if (thiz->descriptor->deinit)
		thiz->descriptor->deinit(EGUEB_DOM_NODE(o), thiz->data);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
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
	thiz->descriptor = descriptor;
	thiz->data = data;

	/* now initialize it */
	if (thiz->descriptor->init)
		thiz->descriptor->init(EGUEB_DOM_NODE(thiz), thiz->data);
	return EGUEB_DOM_NODE(thiz);
}
