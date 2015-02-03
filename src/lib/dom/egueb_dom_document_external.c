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
#include "egueb_dom_list.h"
#include "egueb_dom_document.h"
#include "egueb_dom_document_external.h"

#include "egueb_dom_document_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_DOM_DOCUMENT_EXTERNAL_DESCRIPTOR egueb_dom_document_external_descriptor_get()
#define EGUEB_DOM_DOCUMENT_EXTERNAL_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k, 	\
		Egueb_Dom_Document_External_Class,				\
		EGUEB_DOM_DOCUMENT_EXTERNAL_DESCRIPTOR)
#define EGUEB_DOM_DOCUMENT_EXTERNAL_CLASS_GET(o) 				\
		EGUEB_DOM_DOCUMENT_EXTERNAL_CLASS(				\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_DOCUMENT_EXTERNAL(o) ENESIM_OBJECT_INSTANCE_CHECK(o, 		\
		Egueb_Dom_Document_External,					\
		EGUEB_DOM_DOCUMENT_EXTERNAL_DESCRIPTOR)

typedef struct _Egueb_Dom_Document_External
{
	Egueb_Dom_Document parent;
	const Egueb_Dom_Document_External_Descriptor *descriptor;
	void *data;
} Egueb_Dom_Document_External;

typedef struct _Egueb_Dom_Document_External_Class
{
	Egueb_Dom_Document_Class parent;
} Egueb_Dom_Document_External_Class;

static void _egueb_dom_document_external_init(Egueb_Dom_Document_External *thiz)
{
	if (!thiz->descriptor)
		return;
	if (!thiz->descriptor->init)
		return;
	/* now initialize it */
	thiz->descriptor->init(EGUEB_DOM_NODE(thiz), thiz->data);
}

/*----------------------------------------------------------------------------*
 *                            Document interface                              *
 *----------------------------------------------------------------------------*/
static Egueb_Dom_Node * _egueb_dom_document_external_element_create(
		Egueb_Dom_Document *d, const char *ns, const char *name)
{
	Egueb_Dom_Document_External *thiz;

	thiz = EGUEB_DOM_DOCUMENT_EXTERNAL(d);
	if (thiz->descriptor->element_create)
		return thiz->descriptor->element_create(EGUEB_DOM_NODE(d),
				thiz->data, ns, name);
	return NULL;
}

static Eina_Bool _egueb_dom_document_external_child_appendable(
		Egueb_Dom_Document *d, Egueb_Dom_Node *child)
{
	Egueb_Dom_Document_External *thiz;

	thiz = EGUEB_DOM_DOCUMENT_EXTERNAL(d);
	if (thiz->descriptor->child_appendable)
		return thiz->descriptor->child_appendable(EGUEB_DOM_NODE(d), thiz->data, child);
	return EINA_FALSE;
}
/*----------------------------------------------------------------------------*
 *                               Node interface                               *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_DOCUMENT_DESCRIPTOR,
		Egueb_Dom_Document_External, Egueb_Dom_Document_External_Class,
		egueb_dom_document_external)

static void _egueb_dom_document_external_class_init(void *k)
{
	Egueb_Dom_Document_Class *klass = EGUEB_DOM_DOCUMENT_CLASS(k);

	klass->element_create = _egueb_dom_document_external_element_create;

	klass->child_appendable = _egueb_dom_document_external_child_appendable;
}

static void _egueb_dom_document_external_instance_init(void *o)
{
}

static void _egueb_dom_document_external_instance_deinit(void *o)
{
	Egueb_Dom_Document_External *thiz;

	thiz = EGUEB_DOM_DOCUMENT_EXTERNAL(o);
	if (thiz->descriptor->deinit)
		thiz->descriptor->deinit(EGUEB_DOM_NODE(o), thiz->data);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void * egueb_dom_document_external_data_get(Egueb_Dom_Node *n)
{
	Egueb_Dom_Document_External *thiz;

	thiz = EGUEB_DOM_DOCUMENT_EXTERNAL(n);
	return thiz->data;
}

EAPI Egueb_Dom_Node * egueb_dom_document_external_new(
		const Egueb_Dom_Document_External_Descriptor *descriptor,
		void *data)
{
	Egueb_Dom_Document_External *thiz;

	if (!descriptor) return NULL;
	thiz = ENESIM_OBJECT_INSTANCE_NEW(egueb_dom_document_external);
	thiz->descriptor = descriptor;
	thiz->data = data;
	_egueb_dom_document_external_init(thiz);
	return EGUEB_DOM_NODE(thiz);
}
